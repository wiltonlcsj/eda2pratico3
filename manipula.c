#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNSIMBOLOS 5
typedef struct {
    char palavra[30];
    int frequencia;
    int inicioProximosLista;
} RegistroArquivoDados;

typedef struct {
  char letra[1];
  int dados;
  int esquerda;
  int direita;
} RegistroArquivoArvore;

typedef struct {
  char palavra[30];
  int frequencia;
  int proximo;
} RegistroArquivoLista;

typedef struct {
    int proximoDadosLivre;
    int proximoArvoreLivre;
    int proximoListaLivre;
    int deslocamentoRaiz;
} Controle;

typedef struct {
  int length;
  char sugestoes[3][30];
} Sugestoes;

void abreArquivo(FILE **dados, FILE **arvore, FILE **lista, Controle *c) {
  *dados = fopen("dados.dat", "r+");
  if (*dados == NULL) {
    *dados = fopen("dados.dat", "w+");
    if (*dados == NULL) {
      c->proximoArvoreLivre = -1;
      return;
    }
  }

  *lista = fopen("lista.dat", "r+");
  if (*lista == NULL) {
    *lista = fopen("lista.dat", "w+");
    if (*lista == NULL) {
      c->proximoListaLivre = -1;
      return;
    }
  }

  *arvore = fopen("arvore.dat", "r+");
  if (*arvore == NULL) {
    *arvore = fopen("arvore.dat", "w+");
    if (*arvore == NULL) {
      c->proximoArvoreLivre = -1;
      return;
    } else {
      c->proximoArvoreLivre = 0;
      c->proximoDadosLivre = 0;
      c->proximoListaLivre = 0;
      c->deslocamentoRaiz = -1;
      fseek(*arvore, 0, SEEK_SET);
      if (!fwrite(c, sizeof(*c), 1, *arvore)) {
        c->proximoArvoreLivre = -1;
        return;
      }
    }
  }

  fseek(*arvore, 0, SEEK_SET);
  fread(c, sizeof(*c), 1, *arvore);
}

void setRaiz(Controle *c, FILE **arvore) {
  if (c->deslocamentoRaiz == -1) {
    RegistroArquivoArvore raiz;
    raiz.letra[0] = '*';
    raiz.esquerda = -1;
    raiz.direita = -1;

    fseek(*arvore, sizeof(*c), SEEK_SET);
    if (!fwrite(&raiz, sizeof(raiz), 1, *arvore)) {
      c->deslocamentoRaiz = -1;
      return;
    }

    c->deslocamentoRaiz = 0;
    c->proximoArvoreLivre = 1;
  }
}

void fechaArquivo(FILE **arquivo) { fclose(*arquivo); }

void cadastraLinha(char palavraEntrada[30], FILE **dados, FILE **arvore, Controle *c){
  RegistroArquivoArvore raiz;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(raiz), SEEK_SET);
  fread(&raiz, sizeof(raiz), 1, *arvore); 

  char palavraCorrente[30];
  strcpy(palavraCorrente, palavraEntrada);

  int length = strlen(palavraEntrada);

  int i = 0;
  int found = 1;
  int pai = c->deslocamentoRaiz;
  RegistroArquivoArvore registro = raiz;

  while (i != length) {
    if (registro.letra[0] == '*') {
      registro.letra[0] = palavraCorrente[i];
      registro.letra[1] = '\0';

      pai = c->deslocamentoRaiz;
      fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(registro), SEEK_SET);
      fwrite(&registro, sizeof(registro), 1, *arvore);
      found = 0;
    }

    if (registro.letra[0] == '+') {
      registro.letra[0] = palavraCorrente[i];
      registro.letra[1] = '\0';
      fseek(*arvore, sizeof(*c) + pai * sizeof(registro), SEEK_SET);
      fwrite(&registro, sizeof(registro), 1, *arvore);
      found = 0;
    }

    if (registro.letra[0] == palavraCorrente[i]) {
      if (registro.esquerda == -1) {
        RegistroArquivoArvore novoRegistro;
        novoRegistro.letra[0] = '+';
        registro.letra[1] = '\0';
        novoRegistro.esquerda = -1;
        novoRegistro.direita = -1;
        fseek(*arvore, sizeof(*c) + c->proximoArvoreLivre * sizeof(novoRegistro), SEEK_SET);
        fwrite(&novoRegistro, sizeof(novoRegistro), 1, *arvore);

        registro.esquerda = c->proximoArvoreLivre;
        fseek(*arvore, sizeof(*c) + pai * sizeof(registro), SEEK_SET);
        fwrite(&registro, sizeof(registro), 1, *arvore);

        pai = c->proximoArvoreLivre;
        c->proximoArvoreLivre++;
        registro = novoRegistro;
        found = 0;
        i++;
        continue;
      } else {
        pai = registro.esquerda;
        i++;
        fseek(*arvore, sizeof(*c) + registro.esquerda * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    } else {
      if (registro.direita == -1) {
        RegistroArquivoArvore novoRegistro;
        novoRegistro.letra[0] = '+';
        registro.letra[1] = '\0';
        novoRegistro.esquerda = -1;
        novoRegistro.direita = -1;
        fseek(*arvore, sizeof(*c) + c->proximoArvoreLivre * sizeof(novoRegistro), SEEK_SET);
        fwrite(&novoRegistro, sizeof(novoRegistro), 1, *arvore);

        registro.direita = c->proximoArvoreLivre;
        fseek(*arvore, sizeof(*c) + pai * sizeof(registro), SEEK_SET);
        fwrite(&registro, sizeof(registro), 1, *arvore);

        pai = c->proximoArvoreLivre;
        c->proximoArvoreLivre++;
        registro = novoRegistro;
        found = 0;
        continue;
      } else {
        pai = registro.direita;
        fseek(*arvore, sizeof(*c) + registro.direita * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    }
  }

  if (found == 0) {
    RegistroArquivoDados dadosPalavra;
    dadosPalavra.frequencia = 0;
    dadosPalavra.inicioProximosLista = -1;
    strcpy(dadosPalavra.palavra, palavraCorrente);

    if (registro.letra[0] == '+') {
      registro.letra[0] = '=';
      registro.letra[1] = '\0';
      registro.dados = c->proximoDadosLivre;
      fseek(*arvore, sizeof(*c) + pai * sizeof(registro), SEEK_SET);
      fwrite(&registro, sizeof(registro), 1, *arvore);
    }

    fseek(*dados, c->proximoDadosLivre * sizeof(dadosPalavra), SEEK_SET);
    fwrite(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
    c->proximoDadosLivre++;
  }
}

void cadastrar(FILE **dados, FILE **arvore, Controle *c) {
  int linhas;
  scanf("%d%*c", &linhas);

  char palavrasEntrada[linhas][30];

  for (int i = 0; i < linhas; i++) {
    fgets(palavrasEntrada[i], 30, stdin);
    size_t ln = strlen(palavrasEntrada[i]) - 1;
    if (palavrasEntrada[i][ln] == '\n') 
      palavrasEntrada[i][ln] = '\0';
  }

  for (int i = 0; i < linhas; i++) {
    cadastraLinha(palavrasEntrada[i], dados, arvore, c);
  }
}

int buscaPalavra(char palavra[30], FILE **arvore, Controle *c) {
  RegistroArquivoArvore raiz;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(raiz), SEEK_SET);
  fread(&raiz, sizeof(raiz), 1, *arvore); 

  int length = strlen(palavra);

  int i = 0;
  int found = -1;
  RegistroArquivoArvore registro = raiz;

  while (i != length) {
    if (registro.letra[0] == '*') {
      break;
    }

    if (registro.letra[0] == palavra[i]) {
      if (registro.esquerda == -1) {
        break;
      } else {
        i++;
        fseek(*arvore, sizeof(*c) + registro.esquerda * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    } else {
      if (registro.direita == -1) {
        break;
      } else {
        fseek(*arvore, sizeof(*c) + registro.direita * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    }
  }

  if (i == length && registro.letra[0] == '=') {
    found = registro.dados;
  }

  return found;
}

Sugestoes procuraProximasPalavras(char palavra[30], FILE **dados, FILE **arvore, Controle *c) {
  //TODO implementar busca
  Sugestoes sugestoes;
  sugestoes.length = 3;
  for (int i = 0; i < sugestoes.length; i++) {
    strcpy(sugestoes.sugestoes[i], " *");
  }

  return sugestoes;
}

Sugestoes procuraPossiveisPalavras(char palavra[30], FILE **dados, FILE **arvore, Controle *c) {
  //TODO implementar busca
  Sugestoes sugestoes;
  sugestoes.length = 3;
  for (int i = 0; i < sugestoes.length; i++) {
    strcpy(sugestoes.sugestoes[i], " *");
  }

  return sugestoes;
}

void consultar(FILE **dados, FILE **arvore, FILE **lista, Controle *c) {
  char palavra[30];
  fgets(palavra, 30, stdin);
  size_t ln = strlen(palavra) - 1;
  if (palavra[ln] == '\n') 
    palavra[ln] = '\0';

  int found = buscaPalavra(palavra, arvore, c);

  if (found != -1) {
    RegistroArquivoDados dadosPalavra;
    fseek(*dados, found * sizeof(dadosPalavra), SEEK_SET);
    fwrite(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
    dadosPalavra.frequencia++;
    fseek(*dados, found * sizeof(dadosPalavra), SEEK_SET);
    fwrite(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);

    Sugestoes sugestoes = procuraProximasPalavras(palavra, dados, arvore, c);

    char sugestoesSaida[100] = "";
    for (int i = 0; i < sugestoes.length; i++) {
      char aux[30] = " ";
      strcat(aux, sugestoes.sugestoes[i]);
      strcat(sugestoesSaida, aux);
    }

    printf("proximas palavras:%s\n", sugestoesSaida);
  } else {
    Sugestoes sugestoesDesc = procuraPossiveisPalavras(palavra, dados, arvore, c);
    char sugestoesSaida[100] = "";;
    for (int i = 0; i < sugestoesDesc.length; i++) {
      char aux[30] = " ";
      strcat(aux, sugestoesDesc.sugestoes[i]);
      strcat(sugestoesSaida, aux);
    }

    printf("palavra desconhecida - possiveis correcoes:%s\n", sugestoesSaida);

    char palavraCorrigida[30];
    fgets(palavraCorrigida, 30, stdin);
    size_t ln = strlen(palavraCorrigida) - 1;
    if (palavraCorrigida[ln] == '\n') 
      palavraCorrigida[ln] = '\0';

    if (palavraCorrigida == palavra) {
      cadastraLinha(palavraCorrigida, dados, arvore, c);
      return;
    }

    Sugestoes sugestoesProx = procuraProximasPalavras(palavraCorrigida, dados, arvore, c);
    char sugestoesSaidaProx[100] = "";;
    for (int i = 0; i < sugestoesProx.length; i++) {
      char aux[30] = " ";
      strcat(aux, sugestoesProx.sugestoes[i]);
      strcat(sugestoesSaidaProx, aux);
    }

    printf("proximas palavras:%s\n", sugestoesSaidaProx);
  }
}

void imprimePalavras(FILE **dados, FILE **arvore, Controle *c) {
  RegistroArquivoDados registros[MAXNSIMBOLOS];

  for (int i = 0; i < MAXNSIMBOLOS; i++) {
    
  }
}

void frequenciaProximasPalavras(FILE **dados, FILE **arvore, Controle *c) {
  char palavra[30];
  fgets(palavra, 30, stdin);
  size_t ln = strlen(palavra) - 1;
  if (palavra[ln] == '\n') 
    palavra[ln] = '\0';

  int found = buscaPalavra(palavra, arvore, c);

  if (found == -1) {
    printf("palavra nao encontrada\n");
    return;
  }
}

int main(void) {
  FILE *pont_dados;
  FILE *pont_arvore;
  FILE *pont_lista;
  char opcao;

  Controle controle;
  abreArquivo(&pont_dados, &pont_arvore, &pont_lista, &controle);
  if (controle.proximoArvoreLivre == -1) {
    printf("Erro na abertura dos arquivos!");
    exit(-1);
  }

  setRaiz(&controle, &pont_arvore);
  if (controle.deslocamentoRaiz == -1) {
    printf("Erro ao ler a raiz!");
    exit(-1);
  }

  do {
    scanf("%c%*c", &opcao);
    switch (opcao) {
      case 'e': {
        break;
      }
      case 'i': {
        // Deve inserir a palavra
        cadastrar(&pont_dados, &pont_arvore, &controle);
        break;
      }
      case 'd': {
        // Deve consultar a palavra
        consultar(&pont_dados, &pont_arvore, &pont_lista, &controle);
        break;
      }
      case 'f': {
        // Deve imprimir as palavras em ordem alfabética com frequência
        imprimePalavras(&pont_dados, &pont_arvore, &controle);
        break;
      }
      case 'p': {
        // Deve imprimir as próximas palavras mais frequentes
        frequenciaProximasPalavras(&pont_dados, &pont_arvore, &controle);
        break;
      }
      default:
        break;
    }
  } while (opcao != 'e');
  // Salvando alterações da struct de controle
  fseek(pont_arvore, 0, SEEK_SET);
  fwrite(&controle, sizeof(controle), 1, pont_arvore);

  fechaArquivo(&pont_dados);
  fechaArquivo(&pont_arvore);
  fechaArquivo(&pont_lista);
  return (0);
}
