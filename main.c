#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EMPRESTIMOS 100


typedef struct {
  char titulo[50];
  char autor[50];
  int num_exemplares;
  int exemplares_disponiveis;
} Livro;

typedef struct {
  char nome[50];
  char senha[20];
  int num_emprestimos;
} Usuario;

typedef struct {
  char usuario[50];
  char livro[50];
  char autor[50];
  int tipo;
  char dataHora[100];
} Emprestimo;

FILE *abrirArquivoBinario(const char *nomeArquivo) {
  FILE *file = fopen(nomeArquivo, "rb+");
  if (file == NULL) {
    file = fopen(nomeArquivo, "wb+");
    if (file == NULL) {
      printf("Erro ao criar o arquivo %s!\n", nomeArquivo);
      exit(1);
    }
  }
  return file;
}

FILE *abrirArquivoTexto(const char *nomeArquivo) {
  FILE *file = fopen(nomeArquivo, "a+");
  if (file == NULL) {
    printf("Erro ao criar o arquivo %s!\n", nomeArquivo);
    exit(1);
  }
  return file;
}

void obterDataHora(char *buffer, int tamanhoBuffer) {
  time_t agora;
  struct tm *infoTempo;

  time(&agora);
  infoTempo = localtime(&agora);
  strftime(buffer, tamanhoBuffer, "%d/%m/%Y %H:%M:%S", infoTempo);
}

void cadastrarUsuario(FILE *fileBin) {
  Usuario usuario;
  printf("Digite o nome do usuário: ");
  getchar();
  fgets(usuario.nome, 50, stdin);
  usuario.nome[strcspn(usuario.nome, "\n")] = 0;
  printf("Digite a senha do usuário: ");
  fgets(usuario.senha, 20, stdin);
  usuario.senha[strcspn(usuario.senha, "\n")] = 0;
  usuario.num_emprestimos = 0;

  fseek(fileBin, 0, SEEK_END);
  fwrite(&usuario, sizeof(Usuario), 1, fileBin);

  printf("Usuário cadastrado com sucesso!\n");
}

int realizarLogin(FILE *fileBinUsuarios, Usuario *usuarioLogado) {
  char nome[50];
  char senha[20];
  Usuario usuario;
  int loginBemSucedido = 0;

  printf("Digite o nome do usuário: ");
  getchar();
  fgets(nome, 50, stdin);
  nome[strcspn(nome, "\n")] = 0;

  printf("Digite a senha do usuário: ");
  fgets(senha, 20, stdin);
  senha[strcspn(senha, "\n")] = 0;

  rewind(fileBinUsuarios); 

  while (fread(&usuario, sizeof(Usuario), 1, fileBinUsuarios)) {
    if (strcmp(usuario.nome, nome) == 0 && strcmp(usuario.senha, senha) == 0) {
      loginBemSucedido = 1;
      *usuarioLogado = usuario;
      break;
    }
  }

  if (loginBemSucedido) {
    printf("Login bem-sucedido! Bem-vindo, %s.\n", usuarioLogado->nome);
  } else {
    printf("Nome ou senha não cadastrados.\n");
  }

  return loginBemSucedido;
}


void cadastrarLivro(FILE *fileBin) {
  Livro livro;
  int livroEncontrado = 0;

  
  printf("Digite o título do livro: ");
  getchar(); 
  fgets(livro.titulo, 50, stdin);
  livro.titulo[strcspn(livro.titulo, "\n")] = 0;

  
  printf("Digite o nome do autor: ");
  fgets(livro.autor, 50, stdin);
  livro.autor[strcspn(livro.autor, "\n")] = 0;

  
  Livro livroExistente;
  rewind(fileBin);
  while (fread(&livroExistente, sizeof(Livro), 1, fileBin)) {
    if (strcmp(livroExistente.titulo, livro.titulo) == 0 &&
        strcmp(livroExistente.autor, livro.autor) == 0) {
      livroEncontrado = 1;
      break;
    }
  }

  if (livroEncontrado) {
    
    int adicionais;
    printf("O livro \"%s\" de %s já existe. Quantos exemplares adicionais "
           "deseja adicionar? ",
           livro.titulo, livro.autor);
    scanf("%d", &adicionais);
    livroExistente.num_exemplares += adicionais;
    livroExistente.exemplares_disponiveis += adicionais;

    
    fseek(fileBin, -sizeof(Livro), SEEK_CUR);
    fwrite(&livroExistente, sizeof(Livro), 1, fileBin);
    printf("Exemplares adicionados com sucesso!\n");
  } else {
    
    printf("Digite o número de exemplares: ");
    scanf("%d", &livro.num_exemplares);
    livro.exemplares_disponiveis = livro.num_exemplares;

    fseek(fileBin, 0, SEEK_END); 
    fwrite(&livro, sizeof(Livro), 1, fileBin);
    printf("Livro cadastrado com sucesso!\n");
  }
}


void listarLivros(FILE *fileBin) {
  Livro livro;
  rewind(fileBin);
  printf("\nLivros cadastrados:\n");
  int encontrouLivro = 0;
  while (fread(&livro, sizeof(Livro), 1, fileBin)) {
    printf("Título: %s | Autor: %s | Exemplares disponíveis: %d\n",
           livro.titulo, livro.autor, livro.exemplares_disponiveis);
    encontrouLivro = 1;
  }

  if (!encontrouLivro) {
    printf("Nenhum livro cadastrado.\n");
  }
}

void excluirLivro(FILE *fileBin) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  FILE *fileBinTemp = fopen("livros_temp.bin", "wb");

  if (fileBinTemp == NULL) {
    printf("Erro ao criar arquivo temporário para exclusão de livro.\n");
    return;
  }

  printf("Digite o título do livro a ser excluído: ");
  getchar(); 
  fgets(tituloLivro, 50, stdin);
  tituloLivro[strcspn(tituloLivro, "\n")] = 0;

  printf("Digite o nome do autor do livro: ");
  fgets(autorLivro, 50, stdin);
  autorLivro[strcspn(autorLivro, "\n")] = 0;

  int livroEncontrado = 0;
  rewind(fileBin);

 
  while (fread(&livro, sizeof(Livro), 1, fileBin)) {
    if (strcmp(livro.titulo, tituloLivro) == 0 &&
        strcmp(livro.autor, autorLivro) == 0) {
      livroEncontrado = 1; 
      printf("Livro \"%s\" de \"%s\" excluído com sucesso!\n", tituloLivro,
             autorLivro);
      continue; 
    }
    fwrite(&livro, sizeof(Livro), 1,
           fileBinTemp); 
  }

  if (!livroEncontrado) {
    printf("Livro \"%s\" de \"%s\" não encontrado.\n", tituloLivro, autorLivro);
  }

  fclose(fileBinTemp);
  fclose(fileBin);

  
  rename("livros_temp.bin", "livros.bin");
  fileBin = abrirArquivoBinario("livros.bin"); 
}

void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios,
                        Usuario *usuarioLogado, Emprestimo *emprestimos,
                        int *numEmprestimos) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  int encontradoLivro = 0;
  char dataHora[100];

  printf("Digite o título do livro a ser emprestado: ");
  getchar(); 
  fgets(tituloLivro, 50, stdin);
  tituloLivro[strcspn(tituloLivro, "\n")] = 0;

  printf("Digite o nome do autor do livro: ");
  fgets(autorLivro, 50, stdin);
  autorLivro[strcspn(autorLivro, "\n")] = 0;

  rewind(fileBinLivros);

  while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
    if (strcmp(livro.titulo, tituloLivro) == 0 &&
        strcmp(livro.autor, autorLivro) == 0) {
      encontradoLivro = 1;
      break;
    }
  }

  if (encontradoLivro) {
    if (livro.exemplares_disponiveis > 0 &&
        usuarioLogado->num_emprestimos < 3) {
      livro.exemplares_disponiveis--;
      usuarioLogado->num_emprestimos++;

      fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
      fwrite(&livro, sizeof(Livro), 1, fileBinLivros);

      
      FILE *fileBinUsuariosTemp = fopen("usuarios.bin", "rb+");
      rewind(fileBinUsuariosTemp);
      Usuario usuarioTemp;
      while (fread(&usuarioTemp, sizeof(Usuario), 1, fileBinUsuariosTemp)) {
        if (strcmp(usuarioTemp.nome, usuarioLogado->nome) == 0) {
          usuarioTemp.num_emprestimos = usuarioLogado->num_emprestimos;
          fseek(fileBinUsuariosTemp, -sizeof(Usuario), SEEK_CUR);
          fwrite(&usuarioTemp, sizeof(Usuario), 1, fileBinUsuariosTemp);
          break;
        }
      }
      fclose(fileBinUsuariosTemp);

      
      obterDataHora(dataHora, sizeof(dataHora));
      strcpy(emprestimos[*numEmprestimos].usuario, usuarioLogado->nome);
      strcpy(emprestimos[*numEmprestimos].livro, livro.titulo);
      strcpy(emprestimos[*numEmprestimos].autor, livro.autor);
      emprestimos[*numEmprestimos].tipo = 1; 
      strcpy(emprestimos[*numEmprestimos].dataHora, dataHora);
      (*numEmprestimos)++;

    
      FILE *fileTxtHistoricoTemp = abrirArquivoTexto("historico.txt");
      fprintf(fileTxtHistoricoTemp,
              "[%s] Usuário %s emprestou o livro %s de %s\n", dataHora,
              usuarioLogado->nome, livro.titulo, livro.autor);
      fclose(fileTxtHistoricoTemp);

      printf("Empréstimo realizado com sucesso!\n");
    } else if (usuarioLogado->num_emprestimos >= 3) {
      printf("Você já atingiu o limite de 3 empréstimos simultâneos!\n");
    } else {
      printf("Não há exemplares disponíveis para o livro \"%s\" no momento.\n",
             livro.titulo);
    }
  } else {
    printf("Livro \"%s\" de \"%s\" não encontrado.\n", tituloLivro, autorLivro);
  }
}


void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios,
                   Usuario *usuarioLogado, Emprestimo *emprestimos,
                   int *numEmprestimos) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  int encontradoLivro = 0;
  char dataHora[100];
  int livroEmprestado =
      0; 
  int indiceEmprestimo = -1; 

  printf("Digite o título do livro a ser devolvido: ");
  getchar(); 
  fgets(tituloLivro, 50, stdin);
  tituloLivro[strcspn(tituloLivro, "\n")] = 0;

  printf("Digite o nome do autor do livro: ");
  fgets(autorLivro, 50, stdin);
  autorLivro[strcspn(autorLivro, "\n")] = 0;

  
  for (int i = 0; i < *numEmprestimos; i++) {
    if (strcmp(emprestimos[i].usuario, usuarioLogado->nome) == 0 &&
        strcmp(emprestimos[i].livro, tituloLivro) == 0 &&
        strcmp(emprestimos[i].autor, autorLivro) == 0 &&
        emprestimos[i].tipo == 1) { 
      livroEmprestado = 1;
      indiceEmprestimo = i; 
      break;
    }
  }

  if (!livroEmprestado) {
    printf("Você não emprestou o livro %s de %s.\n", tituloLivro, autorLivro);
    return; 
  }

  rewind(fileBinLivros);

  while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
    if (strcmp(livro.titulo, tituloLivro) == 0 &&
        strcmp(livro.autor, autorLivro) == 0) {
      encontradoLivro = 1;
      break;
    }
  }

  if (encontradoLivro) {
    livro.exemplares_disponiveis++;
    usuarioLogado->num_emprestimos--;

    fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
    fwrite(&livro, sizeof(Livro), 1, fileBinLivros);

    
    FILE *fileBinUsuariosTemp = fopen("usuarios.bin", "rb+");
    rewind(fileBinUsuariosTemp);
    Usuario usuarioTemp;
    while (fread(&usuarioTemp, sizeof(Usuario), 1, fileBinUsuariosTemp)) {
      if (strcmp(usuarioTemp.nome, usuarioLogado->nome) == 0) {
        usuarioTemp.num_emprestimos = usuarioLogado->num_emprestimos;
        fseek(fileBinUsuariosTemp, -sizeof(Usuario), SEEK_CUR);
        fwrite(&usuarioTemp, sizeof(Usuario), 1, fileBinUsuariosTemp);
        break;
      }
    }
    fclose(fileBinUsuariosTemp);

    
    obterDataHora(dataHora, sizeof(dataHora));
    strcpy(emprestimos[indiceEmprestimo].dataHora, dataHora);
    emprestimos[indiceEmprestimo].tipo = 2; 
    printf("O livro %s de %s foi devolvido.\n", tituloLivro, autorLivro);

    
    FILE *fileTxtHistoricoTemp = abrirArquivoTexto("historico.txt");
    fprintf(fileTxtHistoricoTemp, "[%s] Usuário %s devolveu o livro %s de %s\n",
            dataHora, usuarioLogado->nome, livro.titulo, livro.autor);
    fclose(fileTxtHistoricoTemp);

    printf("Devolução realizada com sucesso!\n");
  } else {
    printf("Livro %s de %s não encontrado.\n", tituloLivro, autorLivro);
  }
}


void visualizarHistorico(FILE *fileTxtHistorico) {
  char linha[256];
  rewind(fileTxtHistorico);
  printf("\nHistórico de transações:\n");
  while (fgets(linha, sizeof(linha), fileTxtHistorico)) {
    printf("%s", linha);
  }
}
void listarEmprestimosAtivos(Emprestimo *emprestimos, int numEmprestimos,
                             Usuario *usuarioLogado) {
  printf("\nLivros emprestados ainda não devolvidos:\n");
  int encontrouEmprestimo = 0;

  for (int i = 0; i < numEmprestimos; i++) {
    
    if (strcmp(emprestimos[i].usuario, usuarioLogado->nome) == 0 &&
        emprestimos[i].tipo == 1) { 
      printf("Título: %s | Autor: %s | Data e Hora: %s\n", emprestimos[i].livro,
             emprestimos[i].autor, emprestimos[i].dataHora);
      encontrouEmprestimo = 1;
    }
  }

  if (!encontrouEmprestimo) {
    printf("Nenhum livro foi emprestado.\n");
  }
}


int main() {
  FILE *fileBinLivros = abrirArquivoBinario("livros.bin");
  FILE *fileBinUsuarios = abrirArquivoBinario("usuarios.bin");
  FILE *fileTxtHistorico = abrirArquivoTexto("historico.txt");

  Usuario usuarioLogado;
  Emprestimo emprestimos[MAX_EMPRESTIMOS];
  int numEmprestimos = 0;

  int opcaoLogin;
  int opcaoPrincipal;
  int loginBemSucedido = 0;

  
  do {
    printf("\nMenu de Login/Cadastro:\n");
    printf("1. Cadastrar Usuário\n");
    printf("2. Realizar Login\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcaoLogin);

    switch (opcaoLogin) {
    case 1:
      cadastrarUsuario(fileBinUsuarios);
      break;
    case 2:
      loginBemSucedido = realizarLogin(fileBinUsuarios, &usuarioLogado);
      if (loginBemSucedido) {
        
        do {
          printf("\nMenu Principal:\n");
          printf("1. Cadastrar Livro\n");
          printf("2. Listar Livros\n");
          printf("3. Excluir Livro\n");
          printf("4. Realizar Empréstimo\n");
          printf("5. Devolver Livro\n");
          printf("6. Visualizar Histórico\n");
          printf("7. Listar Empréstimos Ativos\n"); 
          printf("8. Sair\n");
          printf("Escolha uma opção: ");
          scanf("%d", &opcaoPrincipal);

          switch (opcaoPrincipal) {
          case 1:
            cadastrarLivro(fileBinLivros);
            break;
          case 2:
            listarLivros(fileBinLivros);
            break;
          case 3:
            excluirLivro(fileBinLivros);
            break;
          case 4:
            realizarEmprestimo(fileBinLivros, fileBinUsuarios, &usuarioLogado,
                               emprestimos, &numEmprestimos);
            break;
          case 5:
            devolverLivro(fileBinLivros, fileBinUsuarios, &usuarioLogado,
                          emprestimos, &numEmprestimos);
            break;
          case 6:
            visualizarHistorico(fileTxtHistorico);
            break;
          case 7:
            listarEmprestimosAtivos(
                emprestimos, numEmprestimos,
                &usuarioLogado); 
            break;
          case 8:
            printf("Saindo do menu principal...\n");
            break;
          default:
            printf("Opção inválida. Tente novamente.\n");
          }
        } while (opcaoPrincipal != 8);
      }
      break;
    case 3:
      printf("Saindo...\n");
      break;
    default:
      printf("Opção inválida. Tente novamente.\n");
    }
  } while (opcaoLogin != 3 && !loginBemSucedido);

  fclose(fileBinLivros);
  fclose(fileBinUsuarios);
  fclose(fileTxtHistorico);

  return 0;
}