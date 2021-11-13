#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    char ultimaPalavra[30];
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
      strcpy(c->ultimaPalavra, "");
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

void cadastraLinha(bool insert, char palavraEntrada[30], FILE **dados, FILE **arvore, Controle *c) {
  RegistroArquivoArvore raiz;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(raiz), SEEK_SET);
  fread(&raiz, sizeof(raiz), 1, *arvore);

  char palavraCorrente[30];
  strcpy(palavraCorrente, palavraEntrada);

  int length = strlen(palavraEntrada);

  int i = 0;
  int found = 1;
  int pai = -1;
  char direcaoPai[4] = "dir";
  int corrente = c->deslocamentoRaiz;
  RegistroArquivoArvore registro = raiz;

  while (i != length) {
    if (registro.letra[0] == '*') {
      registro.letra[0] = palavraCorrente[i];
      fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(registro), SEEK_SET);
      fwrite(&registro, sizeof(registro), 1, *arvore);
      found = 0;
    }

    if (registro.letra[0] == '+') {
      registro.letra[0] = palavraCorrente[i];
      fseek(*arvore, sizeof(*c) + corrente * sizeof(registro), SEEK_SET);
      fwrite(&registro, sizeof(registro), 1, *arvore);
      found = 0;
    }

    if (registro.letra[0] == palavraCorrente[i]) {
      if (registro.esquerda == -1) {
        RegistroArquivoArvore novoRegistro;
        novoRegistro.letra[0] = '+';
        novoRegistro.esquerda = -1;
        novoRegistro.direita = -1;
        fseek(*arvore, sizeof(*c) + c->proximoArvoreLivre * sizeof(novoRegistro), SEEK_SET);
        fwrite(&novoRegistro, sizeof(novoRegistro), 1, *arvore);

        registro.esquerda = c->proximoArvoreLivre;
        fseek(*arvore, sizeof(*c) + corrente * sizeof(registro), SEEK_SET);
        fwrite(&registro, sizeof(registro), 1, *arvore);

        pai = corrente;
        strcpy(direcaoPai, "esq");
        corrente = c->proximoArvoreLivre;
        c->proximoArvoreLivre++;
        registro = novoRegistro;
        found = 0;
        i++;
        continue;
      } else {
        strcpy(direcaoPai, "esq");
        pai = corrente;
        corrente = registro.esquerda;
        i++;
        fseek(*arvore, sizeof(*c) + registro.esquerda * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    } else if (registro.letra[0] < palavraCorrente[i]) {
      if (registro.direita == -1) {
        RegistroArquivoArvore novoRegistro;
        novoRegistro.letra[0] = '+';
        novoRegistro.esquerda = -1;
        novoRegistro.direita = -1;
        fseek(*arvore, sizeof(*c) + c->proximoArvoreLivre * sizeof(novoRegistro), SEEK_SET);
        fwrite(&novoRegistro, sizeof(novoRegistro), 1, *arvore);

        registro.direita = c->proximoArvoreLivre;
        fseek(*arvore, sizeof(*c) + corrente * sizeof(registro), SEEK_SET);
        fwrite(&registro, sizeof(registro), 1, *arvore);

        strcpy(direcaoPai, "dir");
        pai = corrente;
        corrente = c->proximoArvoreLivre;
        c->proximoArvoreLivre++;
        registro = novoRegistro;
        found = 0;
        continue;
      } else {
        strcpy(direcaoPai, "dir");
        pai = corrente;
        corrente = registro.direita;
        fseek(*arvore, sizeof(*c) + registro.direita * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    } else {
      RegistroArquivoArvore novoRegistro;
      novoRegistro.letra[0] = '+';
      novoRegistro.esquerda = -1;
      novoRegistro.direita = corrente;
      fseek(*arvore, sizeof(*c) + c->proximoArvoreLivre * sizeof(novoRegistro), SEEK_SET);
      fwrite(&novoRegistro, sizeof(novoRegistro), 1, *arvore);

      if (pai != -1) {
        RegistroArquivoArvore registroPai;
        fseek(*arvore, sizeof(*c) + pai * sizeof(registroPai), SEEK_SET);
        fread(&registroPai, sizeof(registroPai), 1, *arvore);
        if(strcmp(direcaoPai, "dir") == 0) {
          registroPai.direita = c->proximoArvoreLivre;
        } else {
          registroPai.esquerda = c->proximoArvoreLivre;
        }

        fseek(*arvore, sizeof(*c) + pai * sizeof(registroPai), SEEK_SET);
        fwrite(&registroPai, sizeof(registroPai), 1, *arvore);
      } else {
        c->deslocamentoRaiz = c->proximoArvoreLivre;
        pai = -1;
      }

      corrente = c->proximoArvoreLivre;
      c->proximoArvoreLivre++;
      registro = novoRegistro;
      found = 0;
    }
  }

  if (found == 0) {
    RegistroArquivoDados dadosPalavra;
    dadosPalavra.frequencia = insert ? 0 : 1;
    dadosPalavra.inicioProximosLista = -1;
    strcpy(dadosPalavra.palavra, palavraCorrente);

    if (registro.letra[0] == '+') {
      registro.letra[0] = '=';
      registro.dados = c->proximoDadosLivre;
      fseek(*arvore, sizeof(*c) + corrente * sizeof(registro), SEEK_SET);
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
    cadastraLinha(true, palavrasEntrada[i], dados, arvore, c);
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
    } else if (registro.letra[0] < palavra[i]) {
      if (registro.direita == -1) {
        break;
      } else {
        fseek(*arvore, sizeof(*c) + registro.direita * sizeof(registro), SEEK_SET);
        fread(&registro, sizeof(registro), 1, *arvore);
      }
    } else {
      break;
    }
  }

  if (i == length && registro.letra[0] == '=') {
    found = registro.dados;
  }

  return found;
}

Sugestoes procuraProximasPalavras(char palavra[30], FILE **dados, FILE **arvore, FILE **lista, Controle *c) {
  Sugestoes sugestoes;

  int found = buscaPalavra(palavra, arvore, c);

  if (found == -1) {
    sugestoes.length = 0;
    return sugestoes;
  }

  RegistroArquivoDados dadosPalavra;
  fseek(*dados, sizeof(dadosPalavra) * found, SEEK_SET);
  fread(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);

  if (dadosPalavra.inicioProximosLista == -1) {
    sugestoes.length = 0;
    return sugestoes;
  }

  RegistroArquivoDados registros[3];
  for (int i = 0; i < 3; i++) {
    registros[i].frequencia = 0;
  }

  int fim = 0;
  int used = 0;
  RegistroArquivoLista registroLista;
  fseek(*lista, sizeof(registroLista) * dadosPalavra.inicioProximosLista, SEEK_SET);
  fread(&registroLista, sizeof(registroLista), 1, *lista);
  while (fim == 0) {
    int emptyUsed = 0;
    for (int i = 0; i < 3; i++) {
      if (registros[i].frequencia == 0) {
        used++;
        emptyUsed = 1;
        registros[i].frequencia = registroLista.frequencia;
        strcpy(registros[i].palavra, registroLista.palavra);
        break;
      }
    }

    if (emptyUsed == 0) {
      for (int i = 0; i < 3; i++) {
        if (registros[i].frequencia < registroLista.frequencia) {
          registros[i].frequencia = registroLista.frequencia;
          strcpy(registros[i].palavra, registroLista.palavra);
          break;
        }
      }
    }

    if (registroLista.proximo == -1) {
      fim = 1;
      continue;
    }

    fseek(*lista, sizeof(registroLista) * registroLista.proximo, SEEK_SET);
    fread(&registroLista, sizeof(registroLista), 1, *lista);
  }

  int changed = 1;
  while (changed == 1) {
    changed = 0;
    for (int i = 0; i < used - 1; i++) {
      if (registros[i].frequencia < registros[i + 1].frequencia) {
        RegistroArquivoDados aux;
        aux.frequencia = registros[i].frequencia;
        strcpy(aux.palavra, registros[i].palavra);
        registros[i] = registros[i + 1];
        registros[i + 1] = aux;
        changed = 1;
      }
    }
  }

  sugestoes.length = used;
  for (int i = 0; i < sugestoes.length; i++) {
    strcpy(sugestoes.sugestoes[i], registros[i].palavra);
  }

  return sugestoes;
}

Sugestoes procuraPossiveisPalavras(char palavra[30], FILE **dados, FILE **arvore, Controle *c) {
  //TODO implementar busca
  Sugestoes sugestoes;
  sugestoes.length = 0;
  return sugestoes;
}

void salvaFrequenciaProximasPalavras(char palavra[30], FILE **dados, FILE **arvore, FILE **lista, Controle *c) {
  // Se não tem alguma palavra anterior buscada
  if (strlen(c->ultimaPalavra) == 0) {
    return;
  }

  int newFound = buscaPalavra(c->ultimaPalavra, arvore, c);

  RegistroArquivoDados palavraAnterior;
  fseek(*dados, newFound * sizeof(palavraAnterior), SEEK_SET);
  fread(&palavraAnterior, sizeof(palavraAnterior), 1, *dados);

  int ultimoDeslocamentoLista;
  RegistroArquivoLista registroLista;
  if (palavraAnterior.inicioProximosLista == -1) {
    // Se a palavra anterior não tem lista de frequência cria o primeiro registro
    registroLista.frequencia = 0;
    registroLista.proximo = -1;
    strcpy(registroLista.palavra, palavra);
    fseek(*lista, c->proximoListaLivre * sizeof(registroLista), SEEK_SET);
    fwrite(&registroLista, sizeof(registroLista), 1, *lista);
    ultimoDeslocamentoLista = c->proximoListaLivre;
    c->proximoListaLivre++;

    palavraAnterior.inicioProximosLista = ultimoDeslocamentoLista;
    fseek(*dados, newFound * sizeof(palavraAnterior), SEEK_SET);
    fwrite(&palavraAnterior, sizeof(palavraAnterior), 1, *dados);
  } else {
    // Se a palavra anterior tem lista de frequencia pega o inicio da lista
    ultimoDeslocamentoLista = palavraAnterior.inicioProximosLista;
    fseek(*lista, sizeof(registroLista) * palavraAnterior.inicioProximosLista, SEEK_SET);
    fread(&registroLista, sizeof(registroLista), 1, *lista);
  }

  int found = 0;
  while (found == 0) {
    if (strcmp(registroLista.palavra, palavra) == 0) {
      registroLista.frequencia++;
      found = 1;
      continue;
    }

    //Se chegou no último elemento mas não encontrou cria novo No
    if (registroLista.proximo == -1) {
      RegistroArquivoLista novoNo;
      novoNo.frequencia = 0;
      novoNo.proximo = -1;
      strcpy(novoNo.palavra, palavra);

      fseek(*lista, c->proximoListaLivre * sizeof(novoNo), SEEK_SET);
      fwrite(&novoNo, sizeof(novoNo), 1, *lista);

      registroLista.proximo = c->proximoListaLivre;
      fseek(*lista, ultimoDeslocamentoLista * sizeof(registroLista), SEEK_SET);
      fwrite(&registroLista, sizeof(registroLista), 1, *lista);

      registroLista = novoNo;
      ultimoDeslocamentoLista = c->proximoListaLivre;
      c->proximoListaLivre++;
      continue;
    }

    //Caso exista outro elemento na lista
    ultimoDeslocamentoLista = registroLista.proximo;
    fseek(*lista, sizeof(registroLista) * registroLista.proximo, SEEK_SET);
    fread(&registroLista, sizeof(registroLista), 1, *lista);
  }

  fseek(*lista, ultimoDeslocamentoLista * sizeof(registroLista), SEEK_SET);
  fwrite(&registroLista, sizeof(registroLista), 1, *lista);
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
    fread(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
    dadosPalavra.frequencia++;
    fseek(*dados, found * sizeof(dadosPalavra), SEEK_SET);
    fwrite(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);

    if (strcmp(c->ultimaPalavra, palavra) != 0) {
      salvaFrequenciaProximasPalavras(palavra, dados, arvore, lista, c);
      strcpy(c->ultimaPalavra, palavra);
    }

    /*
     * Próximas palavras para casos de acerto
    Sugestoes sugestoes = procuraProximasPalavras(palavra, dados, arvore, lista, c);

    if (sugestoes.length > 0) {
      char sugestoesSaida[100] = "";
      for (int i = 0; i < sugestoes.length; i++) {
        char aux[30] = " ";
        strcat(aux, sugestoes.sugestoes[i]);
        strcat(sugestoesSaida, aux);
      }

      printf("proximas palavras:%s\n", sugestoesSaida);
    }
     */
  } else {
    Sugestoes sugestoesDesc = procuraPossiveisPalavras(palavra, dados, arvore, c);
    char sugestoesSaida[100] = "";
    for (int i = 0; i < sugestoesDesc.length; i++) {
      char aux[30] = " ";
      strcat(aux, sugestoesDesc.sugestoes[i]);
      strcat(sugestoesSaida, aux);
    }

    printf("palavra desconhecida - possiveis correcoes:%s\n", sugestoesSaida);

    char palavraCorrigida[30];
    fgets(palavraCorrigida, 30, stdin);
    size_t enln = strlen(palavraCorrigida) - 1;
    if (palavraCorrigida[enln] == '\n')
      palavraCorrigida[enln] = '\0';

    if (strcmp(palavraCorrigida, palavra) == 0) {
      cadastraLinha(false, palavraCorrigida, dados, arvore, c);
      salvaFrequenciaProximasPalavras(palavraCorrigida, dados, arvore, lista, c);
      strcpy(c->ultimaPalavra, palavraCorrigida);
      return;
    }

    if (strcmp(palavraCorrigida, c->ultimaPalavra) != 0) {
      salvaFrequenciaProximasPalavras(palavraCorrigida, dados, arvore, lista, c);
      strcpy(c->ultimaPalavra, palavraCorrigida);
    }

    Sugestoes sugestoesProx = procuraProximasPalavras(palavraCorrigida, dados, arvore, lista, c);
    char sugestoesSaidaProx[100] = "";
    for (int i = 0; i < sugestoesProx.length; i++) {
      char aux[30] = " ";
      strcat(aux, sugestoesProx.sugestoes[i]);
      strcat(sugestoesSaidaProx, aux);
    }

    printf("proximas palavras:%s\n", sugestoesSaidaProx);

    int foundCorrigida = buscaPalavra(palavraCorrigida, arvore, c);
    RegistroArquivoDados dadosPalavra;
    fseek(*dados, foundCorrigida * sizeof(dadosPalavra), SEEK_SET);
    fread(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
    dadosPalavra.frequencia++;
    fseek(*dados, foundCorrigida * sizeof(dadosPalavra), SEEK_SET);
    fwrite(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
  }
}

void imprimeNo(RegistroArquivoArvore no, FILE **arvore, FILE **dados, Controle *c) {
  if (no.letra[0] == '=') {
    RegistroArquivoDados dadosPalavra;
    fseek(*dados, no.dados * sizeof(dadosPalavra), SEEK_SET);
    fread(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);
    printf("%s %d\n", dadosPalavra.palavra, dadosPalavra.frequencia);
  }

  if (no.esquerda != -1) {
    RegistroArquivoArvore novoNo;
    fseek(*arvore, sizeof(*c) + no.esquerda * sizeof(novoNo), SEEK_SET);
    fread(&novoNo, sizeof(novoNo), 1, *arvore);
    imprimeNo(novoNo, arvore, dados, c);
  }

  if (no.direita != -1) {
    RegistroArquivoArvore novoNo;
    fseek(*arvore, sizeof(*c) + no.direita * sizeof(novoNo), SEEK_SET);
    fread(&novoNo, sizeof(novoNo), 1, *arvore);
    imprimeNo(novoNo, arvore, dados, c);
  }
}

void imprimePalavras(FILE **arvore, FILE **dados, Controle *c) {
  RegistroArquivoArvore raiz;
  fseek(*arvore, sizeof(*c) + c->deslocamentoRaiz * sizeof(raiz), SEEK_SET);
  fread(&raiz, sizeof(raiz), 1, *arvore);
  imprimeNo(raiz, arvore, dados, c);
}

void frequenciaProximasPalavras(FILE **dados, FILE **arvore, FILE **lista, Controle *c) {
  char palavra[30];
  fgets(palavra, 30, stdin);
  size_t ln = strlen(palavra) - 1;
  if (palavra[ln] == '\n')
    palavra[ln] = '\0';

  int found = buscaPalavra(palavra, arvore, c);

  if (found == -1) {
    return;
  }

  RegistroArquivoDados dadosPalavra;
  fseek(*dados, sizeof(dadosPalavra) * found, SEEK_SET);
  fread(&dadosPalavra, sizeof(dadosPalavra), 1, *dados);

  if (dadosPalavra.inicioProximosLista == -1) {
    return;
  }

  RegistroArquivoDados registros[3];
  for (int i = 0; i < 3; i++) {
    registros[i].frequencia = 0;
  }

  int fim = 0;
  int used = 0;
  RegistroArquivoLista registroLista;
  fseek(*lista, sizeof(registroLista) * dadosPalavra.inicioProximosLista, SEEK_SET);
  fread(&registroLista, sizeof(registroLista), 1, *lista);
  while (fim == 0) {
    int emptyUsed = 0;
    for (int i = 0; i < 3; i++) {
      if (registros[i].frequencia == 0) {
        used++;
        emptyUsed = 1;
        registros[i].frequencia = registroLista.frequencia;
        strcpy(registros[i].palavra, registroLista.palavra);
        break;
      }
    }

    if (emptyUsed == 0) {
      for (int i = 0; i < 3; i++) {
        if (registros[i].frequencia < registroLista.frequencia) {
          registros[i].frequencia = registroLista.frequencia;
          strcpy(registros[i].palavra, registroLista.palavra);
          break;
        }
      }
    }

    if (registroLista.proximo == -1) {
      fim = 1;
      continue;
    }

    fseek(*lista, sizeof(registroLista) * registroLista.proximo, SEEK_SET);
    fread(&registroLista, sizeof(registroLista), 1, *lista);
  }

  int changed = 1;
  while (changed == 1) {
    changed = 0;
    for (int i = 0; i < used - 1; i++) {
      if (registros[i].frequencia < registros[i + 1].frequencia) {
        RegistroArquivoDados aux;
        aux.frequencia = registros[i].frequencia;
        strcpy(aux.palavra, registros[i].palavra);
        registros[i] = registros[i + 1];
        registros[i + 1] = aux;
        changed = 1;
      }
    }
  }

  for (int i = 0; i < used; i++) {
    printf("%s %d\n", registros[i].palavra, registros[i].frequencia);
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
        imprimePalavras(&pont_arvore, &pont_dados, &controle);
        break;
      }
      case 'p': {
        // Deve imprimir as próximas palavras mais frequentes
        frequenciaProximasPalavras(&pont_dados, &pont_arvore, &pont_lista, &controle);
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
