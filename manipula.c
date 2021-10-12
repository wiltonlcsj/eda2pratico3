#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRAUMINIMO 2
#define MIN_NO GRAUMINIMO-1
#define MAX_NO 2*(GRAUMINIMO)-1
#define END_POINT 4*(GRAUMINIMO)-1

typedef struct {
  int chave;
  char nome[20];
  int idade;
} RegistroArquivoDados;

typedef struct {
  int chave;
  int apontador;
} No;

typedef struct {
  int ocupados;
  No chaves[GRAUMINIMO];
  No ponteiros[END_POINT];
} RegistroArquivoArvore;

typedef struct {
  int proximoDadosLivre;
  int proximoArvoreLivre;
} Controle;

void abreArquivo(FILE **dados, FILE **arvore, Controle *c) {
  *dados = fopen("dados", "r+");
  if (*dados == NULL) {
    *dados = fopen("dados", "w+");
    if (*dados == NULL) {
      c->proximoArvoreLivre = 0;
      return;
    }
  }

  *arvore = fopen("arvore", "r+");
  if (*arvore == NULL) {
    *arvore = fopen("arvore", "w+");
    if (*arvore == NULL) {
      c->proximoArvoreLivre = 0;
      return;
    } else {
      c->proximoArvoreLivre = 1;
      c->proximoDadosLivre = 0;
      fseek(*arvore, sizeof(Controle), SEEK_SET);
      if (!fwrite(c, sizeof(Controle), 1, *arvore)) {
        c->proximoArvoreLivre = 0;
        return;
      }

      return;
    }
  }

  fseek(*arvore, sizeof(Controle), SEEK_SET);
  fread(c, sizeof(Controle), 1, *arvore);
}

void fechaArquivo(FILE **arquivo) { fclose(*arquivo); }

void imprimir(RegistroArquivoDados registro) {
  printf("chave: %d", registro.chave);
  printf("\n%s", registro.nome);
  printf("\n%d", registro.idade);
  printf("\n");
}

void getRaiz(Controle *c, FILE **arvore, RegistroArquivoArvore *raiz) {
  if (c->proximoDadosLivre == 1) {
    raiz->ocupados = 0;
    for (int i = 0; i < MAX_NO; i++) {
      raiz->chaves[i].apontador = -1;
      raiz->chaves[i].chave = -1;
    }

    for (int i = 0; i < END_POINT; i++) {
      raiz->ponteiros[i].apontador = -1;
      raiz->ponteiros[i].chave = -1;
    }

    fseek(*arvore, sizeof(c), SEEK_SET);
    if (!fwrite(&raiz, sizeof(raiz), 1, *arvore)) {
      raiz->ocupados = -1;
      return;
    }

    c->proximoArvoreLivre = 2;
  } else {
    fseek(*arvore, sizeof(c), SEEK_SET);
    fread(raiz, sizeof(raiz), 1, *arvore);
  }
}

int buscaChave(int chave, FILE **arvore, Controle *c, RegistroArquivoArvore *filho,
               RegistroArquivoArvore *pai) {

  bool exist = false;
  int next = -1;

  if (filho->ocupados == MAX_NO) {
    if (pai == NULL) {
      int index = ceil(MAX_NO / 2);

      RegistroArquivoArvore root;
      root.chaves[0] = filho->chaves[index];

      RegistroArquivoArvore esq;
      for (int i; i < index - 1; i++) {
        esq.ocupados++;
        esq.chaves[i] = filho->chaves[i];
      }

      RegistroArquivoArvore dir;
      for (int i = index + 1; i <= MAX_NO; i++) {
        dir.ocupados++;
        dir.chaves[i] = filho->chaves[i];
      }
    }
  }

  for (int i = 0; i < MAX_NO; i++) {
    if (filho->chaves[i].chave == chave) {
      exist = true;
      break;
    }

    if (filho->chaves[i].chave > chave) {
      next = pai->ponteiros[i - 0].apontador;
      break;
    }
  }

  if (exist) return -1;

  RegistroArquivoArvore r;
  fseek(*arvore, next * sizeof(RegistroArquivoArvore), SEEK_SET);
  fread(&r, sizeof(r), 1, *arvore);
  // TODO continuar no próximo nó

  return 0;
}

void cadastrar(FILE **dados, FILE **arvore, Controle *c, RegistroArquivoArvore *root,
               RegistroArquivoArvore *pai) {
  RegistroArquivoDados r;
  scanf("%d%*c", &r.chave);

  fgets(r.nome, 20, stdin);
  // Limpando enter final do buffer
  size_t ln = strlen(r.nome) - 1;
  if (r.nome[ln] == '\n') r.nome[ln] = '\0';

  scanf("%d%*c", &r.idade);

  int deslocamento = buscaChave(r.chave, arvore, c, root, pai);
  if (deslocamento != -1) {
    printf("chave ja existente: %d\n", r.chave);
    return;
  }

  fseek(*dados, deslocamento * sizeof(r), SEEK_SET);
  if (fwrite(&r, sizeof(r), 1, *dados))
    printf("insercao com sucesso: %d\n", r.chave);
}

void consultar(FILE **dados, FILE **arvore, Controle *c, RegistroArquivoArvore *root,
               RegistroArquivoArvore *pai) {
  int chave;
  scanf("%d%*c", &chave);

  int deslocamento = buscaChave(chave, arvore, c, root, pai);
  if (deslocamento == -1) {
    printf("chave nao encontrada: %d\n", chave);
    return;
  }

  RegistroArquivoDados r;
  fseek(*dados, deslocamento * sizeof(r), SEEK_SET);
  fread(&r, sizeof(r), 1, *dados);
  imprimir(r);
}

int main(void) {
  FILE *pont_dados;
  FILE *pont_arvore;
  char opcao;

  Controle controle;
  abreArquivo(&pont_dados, &pont_arvore, &controle);
  if (controle.proximoArvoreLivre == 0) {
    printf("Erro na abertura dos arquivos!");
    exit(-1);
  }

  RegistroArquivoArvore raiz;
  getRaiz(&controle, &pont_arvore, &raiz);
  if (raiz.ocupados == -1) {
    printf("Erro ao ler a raiz!");
    exit(-1);
  }

  do {
    scanf("%c%*c", &opcao);
    switch (opcao) {
      case 'e': {
        break;
      }
      case 'c': {
        // Deve consultar a chave
        consultar(&pont_dados, &pont_arvore, &controle, &raiz, NULL);
        break;
      }
      case 'i': {
        // Deve inserir o registro
        cadastrar(&pont_dados, &pont_arvore, &controle, &raiz, NULL);
        break;
      }
      case 'p': {
        // Deve imprimir a arvore
        break;
      }
      case 'o': {
        // Deve imprimir as chaves em ordem crescente
        break;
      }
      case 't': {
        // Deve imprimir a taxa de ocupação da arvore
        break;
      }
      default:
        break;
    }
  } while (opcao != 'e');

  fechaArquivo(&pont_dados);
  fechaArquivo(&pont_arvore);
  return (0);
}
