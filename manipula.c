#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNSIMBOLOS 5
typedef struct {
    char palavra[30];
    int frequencia;
} RegistroArquivoDados;

typedef struct {
  char letra[1];
  int dados;
  int esquerda;
  int direita;
} RegistroArquivoArvore;

typedef struct {
    int proximoDadosLivre;
    int proximoArvoreLivre;
    int deslocamentoRaiz;
} Controle;

void abreArquivo(FILE **dados, FILE **arvore, Controle *c) {
  *dados = fopen("dados", "r+");
  if (*dados == NULL) {
    *dados = fopen("dados", "w+");
    if (*dados == NULL) {
      c->proximoArvoreLivre = -1;
      return;
    }
  }

  *arvore = fopen("arvore", "r+");
  if (*arvore == NULL) {
    *arvore = fopen("arvore", "w+");
    if (*arvore == NULL) {
      c->proximoArvoreLivre = -1;
      return;
    } else {
      c->proximoArvoreLivre = 0;
      c->proximoDadosLivre = 0;
      c->deslocamentoRaiz = -1;
      fseek(*arvore, 0, SEEK_SET);
      if (!fwrite(c, sizeof(*c), 1, *arvore)) {
        c->proximoArvoreLivre = -1;
        return;
      }

      return;
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

int buscaChaveCadastro(int chave, FILE **arvore, Controle *c) {
  RegistroArquivoArvore registro;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(registro), SEEK_SET);
  fread(&registro, sizeof(registro), 1, *arvore);

  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(registro), SEEK_SET);
  fwrite(&registro, sizeof(registro), 1, *arvore);

  int deslocamento = 0;

  return deslocamento;
}

int buscaChaveConsulta(int chave, FILE **arvore, Controle *c) {
  RegistroArquivoArvore registro;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(registro), SEEK_SET);
  fread(&registro, sizeof(registro), 1, *arvore);

  int deslocamento = 0;

  return deslocamento;
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
    RegistroArquivoArvore raiz;
    fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(raiz), SEEK_SET);
    fread(&raiz, sizeof(raiz), 1, *arvore); 

    char palavraCorrente[30];
    strcpy(palavraCorrente, palavrasEntrada[i]);

    int length = strlen(palavrasEntrada[i]);

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
}

void consultar(FILE **dados, FILE **arvore, Controle *c) {
  int chave;
  scanf("%d%*c", &chave);

  int deslocamento = buscaChaveConsulta(chave, arvore, c);
  if (deslocamento == -1) {
    printf("chave nao encontrada: %d\n", chave);
    return;
  }

  RegistroArquivoDados r;
  fseek(*dados, deslocamento * sizeof(r), SEEK_SET);
  fread(&r, sizeof(r), 1, *dados);
}

void printNo (int deslocamento, FILE **arvore) {
  RegistroArquivoArvore registro;
  fseek(*arvore, sizeof(Controle) + deslocamento * sizeof(registro), SEEK_SET);
  fread(&registro, sizeof(registro), 1, *arvore); 

  printf("\ndesl: %d no: %s esq: %d dir: %d", deslocamento, registro.letra, registro.esquerda, registro.direita);

  if (registro.esquerda != -1){
    printNo(registro.esquerda, arvore);
  }

  if (registro.direita != -1){
    printNo(registro.direita, arvore);
  }
}

void imprimeArvore(FILE **dados, FILE **arvore, Controle *c) {
  printNo(c->deslocamentoRaiz, arvore);
}

int main(void) {
  FILE *pont_dados;
  FILE *pont_arvore;
  char opcao;

  Controle controle;
  abreArquivo(&pont_dados, &pont_arvore, &controle);
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
        consultar(&pont_dados, &pont_arvore, &controle);
        break;
      }
      case 'f': {
        // Deve imprimir as palavras em ordem alfabética com frequência
        break;
      }
      case 'p': {
        // Deve imprimir as próximas palavras mais frequentes
        break;
      }
      case 't': {
        // Deve imprimir a arvore
        imprimeArvore(&pont_dados, &pont_arvore, &controle);
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
  return (0);
}
