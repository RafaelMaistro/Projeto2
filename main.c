#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPRESTIMOS 100

// Estruturas para armazenar informações
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

// Estrutura para armazenar informações dos empréstimos
typedef struct {
  char usuario[50];
  char livro[50];
  char autor[50];
  int tipo; // 1 para empréstimo, 2 para devolução
} Emprestimo;

// Função para abrir arquivos binários, criando-os se necessário
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

// Função para abrir arquivo de texto, criando-o se necessário
FILE *abrirArquivoTexto(const char *nomeArquivo) {
  FILE *file = fopen(nomeArquivo, "a+");
  if (file == NULL) {
    printf("Erro ao criar o arquivo %s!\n", nomeArquivo);
    exit(1);
  }
  return file;
}

// Função para cadastrar cliente (usuário) com nome e senha
void cadastrarUsuario(FILE *fileBin) {
  Usuario usuario;
  printf("Digite o nome do usuário: ");
  getchar(); // Limpa o buffer
  fgets(usuario.nome, 50, stdin);
  usuario.nome[strcspn(usuario.nome, "\n")] = 0;
  printf("Digite a senha do usuário: ");
  fgets(usuario.senha, 20, stdin);
  usuario.senha[strcspn(usuario.senha, "\n")] = 0;
  usuario.num_emprestimos = 0;

  fseek(fileBin, 0,
        SEEK_END); // Vai para o final do arquivo para não sobrescrever
  fwrite(&usuario, sizeof(Usuario), 1, fileBin);

  printf("Usuário cadastrado com sucesso!\n");
}

// Função para realizar o login do usuário
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

  rewind(fileBinUsuarios); // Volta ao início do arquivo

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

// Função para cadastrar um livro no arquivo binário
void cadastrarLivro(FILE *fileBin) {
  Livro livro;
  int livroEncontrado = 0;

  // Solicita o nome do livro
  printf("Digite o título do livro: ");
  getchar(); // Limpa o buffer
  fgets(livro.titulo, 50, stdin);
  livro.titulo[strcspn(livro.titulo, "\n")] = 0;

  // Solicita o nome do autor
  printf("Digite o nome do autor: ");
  fgets(livro.autor, 50, stdin);
  livro.autor[strcspn(livro.autor, "\n")] = 0;

  // Verifica se o livro já existe
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
    // Se o livro já existir, pergunta quantos exemplares adicionais o usuário
    // deseja adicionar
    int adicionais;
    printf("O livro \"%s\" de %s já existe. Quantos exemplares adicionais "
           "deseja adicionar? ",
           livro.titulo, livro.autor);
    scanf("%d", &adicionais);
    livroExistente.num_exemplares += adicionais;
    livroExistente.exemplares_disponiveis += adicionais;

    // Atualiza o registro do livro existente
    fseek(fileBin, -sizeof(Livro), SEEK_CUR);
    fwrite(&livroExistente, sizeof(Livro), 1, fileBin);
    printf("Exemplares adicionados com sucesso!\n");
  } else {
    // Se o livro não existir, solicita o número de exemplares
    printf("Digite o número de exemplares: ");
    scanf("%d", &livro.num_exemplares);
    livro.exemplares_disponiveis = livro.num_exemplares;

    fseek(fileBin, 0, SEEK_END); // Vai para o final do arquivo
    fwrite(&livro, sizeof(Livro), 1, fileBin);
    printf("Livro cadastrado com sucesso!\n");
  }
}

// Função para listar livros cadastrados
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

// Função para registrar empréstimo
void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios,
                        Usuario *usuarioLogado, Emprestimo *emprestimos,
                        int *numEmprestimos) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  int encontradoLivro = 0;

  printf("Digite o título do livro a ser emprestado: ");
  getchar(); // Limpa o buffer
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

      // Atualiza o número de empréstimos do usuário no arquivo de usuários
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

      // Adiciona registro de empréstimo
      strcpy(emprestimos[*numEmprestimos].usuario, usuarioLogado->nome);
      strcpy(emprestimos[*numEmprestimos].livro, livro.titulo);
      strcpy(emprestimos[*numEmprestimos].autor, livro.autor);
      emprestimos[*numEmprestimos].tipo = 1; // Tipo 1 para empréstimo
      (*numEmprestimos)++;

      // Grava no arquivo de histórico
      FILE *fileTxtHistoricoTemp = abrirArquivoTexto("historico.txt");
      fprintf(fileTxtHistoricoTemp, "Usuário %s emprestou o livro %s de %s\n",
              usuarioLogado->nome, livro.titulo, livro.autor);
      fclose(fileTxtHistoricoTemp);

      printf("Empréstimo realizado com sucesso!\n");
    } else if (livro.exemplares_disponiveis == 0) {
      printf("Não há exemplares disponíveis para empréstimo.\n");
    } else {
      printf("Você já alcançou o limite de empréstimos.\n");
    }
  } else {
    printf("Livro não encontrado.\n");
  }
}

// Função para devolver um livro
void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios,
                   Usuario *usuarioLogado, Emprestimo *emprestimos,
                   int *numEmprestimos) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  int encontradoLivro = 0;

  printf("Digite o título do livro a ser devolvido: ");
  getchar(); // Limpa o buffer
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
    livro.exemplares_disponiveis++;
    usuarioLogado->num_emprestimos--;

    fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
    fwrite(&livro, sizeof(Livro), 1, fileBinLivros);

    // Atualiza o número de empréstimos do usuário no arquivo de usuários
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

    // Adiciona registro de devolução
    strcpy(emprestimos[*numEmprestimos].usuario, usuarioLogado->nome);
    strcpy(emprestimos[*numEmprestimos].livro, livro.titulo);
    strcpy(emprestimos[*numEmprestimos].autor, livro.autor);
    emprestimos[*numEmprestimos].tipo = 2; // Tipo 2 para devolução
    (*numEmprestimos)++;

    // Grava no arquivo de histórico
    FILE *fileTxtHistoricoTemp = abrirArquivoTexto("historico.txt");
    fprintf(fileTxtHistoricoTemp, "Usuário %s devolveu o livro %s de %s\n",
            usuarioLogado->nome, livro.titulo, livro.autor);
    fclose(fileTxtHistoricoTemp);

    printf("Devolução realizada com sucesso!\n");
  } else {
    printf("Livro não encontrado.\n");
  }
}

// Função para buscar ou visualizar detalhes de um livro
void buscarOuVisualizarLivro(FILE *fileBin) {
  char tituloLivro[50];
  char autorLivro[50];
  Livro livro;
  int encontrado = 0;

  printf("Digite o título do livro a ser buscado: ");
  getchar(); // Limpa o buffer
  fgets(tituloLivro, 50, stdin);
  tituloLivro[strcspn(tituloLivro, "\n")] = 0;

  printf("Digite o nome do autor do livro: ");
  fgets(autorLivro, 50, stdin);
  autorLivro[strcspn(autorLivro, "\n")] = 0;

  rewind(fileBin);
  while (fread(&livro, sizeof(Livro), 1, fileBin)) {
    if (strcmp(livro.titulo, tituloLivro) == 0 &&
        strcmp(livro.autor, autorLivro) == 0) {
      printf("Título: %s | Autor: %s | Exemplares: %d | Disponíveis: %d\n",
             livro.titulo, livro.autor, livro.num_exemplares,
             livro.exemplares_disponiveis);
      encontrado = 1;
      break; // Sai do loop após encontrar o livro
    }
  }

  if (!encontrado) {
    printf("Nenhum livro encontrado com o título \"%s\" e autor \"%s\".\n",
           tituloLivro, autorLivro);
  }
}

// Função para carregar empréstimos de um arquivo
void carregarEmprestimos(Emprestimo *emprestimos, int *numEmprestimos) {
  FILE *fileTxtHistorico = fopen("historico.txt", "r");
  if (fileTxtHistorico == NULL) {
    printf("Erro ao abrir arquivo de histórico!\n");
    return;
  }

  char linha[200];
  while (fgets(linha, sizeof(linha), fileTxtHistorico)) {
    // Exclui a nova linha e formata os dados
    linha[strcspn(linha, "\n")] = 0;
    sscanf(linha, "Usuário %[^ ] emprestou o livro %[^ ] de %[^ ]",
           emprestimos[*numEmprestimos].usuario,
           emprestimos[*numEmprestimos].livro,
           emprestimos[*numEmprestimos].autor);
    emprestimos[*numEmprestimos].tipo = 1; // Tipo 1 para empréstimo
    (*numEmprestimos)++;
  }

  fclose(fileTxtHistorico);
}

// Função para exibir todos os empréstimos
void exibirEmprestimos(Emprestimo *emprestimos, int numEmprestimos) {
  printf("\nEmpréstimos e Devoluções realizados:\n");
  for (int i = 0; i < numEmprestimos; i++) {
    if (emprestimos[i].tipo == 1) {
      printf("Empréstimo - Usuário: %s | Livro: %s de %s\n",
             emprestimos[i].usuario, emprestimos[i].livro,
             emprestimos[i].autor);
    } else if (emprestimos[i].tipo == 2) {
      printf("Devolução - Usuário: %s | Livro: %s de %s\n",
             emprestimos[i].usuario, emprestimos[i].livro,
             emprestimos[i].autor);
    }
  }
}

int main() {
  FILE *fileBinLivros = abrirArquivoBinario("livros.bin");
  FILE *fileBinUsuarios = abrirArquivoBinario("usuarios.bin");

  Emprestimo emprestimos[MAX_EMPRESTIMOS];
  int numEmprestimos = 0;

  // Carregar os empréstimos ao iniciar o programa
  carregarEmprestimos(emprestimos, &numEmprestimos);

  int opcao;
  Usuario usuarioLogado;

  // Menu de Cadastro
  do {
    printf("\n--- Menu Inicial ---\n");
    printf("1. Cadastrar Usuário\n");
    printf("2. Fazer Login\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
    case 1:
      cadastrarUsuario(fileBinUsuarios);
      break;
    case 2:
      // Menu de Login
      if (realizarLogin(fileBinUsuarios, &usuarioLogado)) {
        // Menu Principal após login
        do {
          printf("\n--- Menu Principal ---\n");
          printf("1. Cadastrar Livro\n");
          printf("2. Listar Livros\n");
          printf("3. Realizar Empréstimo\n");
          printf("4. Devolver Livro\n");
          printf("5. Buscar Livro\n");
          printf("6. Exibir Todos os Empréstimos\n");
          printf("0. Sair\n");
          printf("Escolha uma opção: ");
          scanf("%d", &opcao);

          switch (opcao) {
          case 1:
            cadastrarLivro(fileBinLivros);
            break;
          case 2:
            listarLivros(fileBinLivros);
            break;
          case 3:
            realizarEmprestimo(fileBinLivros, fileBinUsuarios, &usuarioLogado,
                               emprestimos, &numEmprestimos);
            break;
          case 4:
            devolverLivro(fileBinLivros, fileBinUsuarios, &usuarioLogado,
                          emprestimos, &numEmprestimos);
            break;
          case 5:
            buscarOuVisualizarLivro(fileBinLivros);
            break;
          case 6:
            exibirEmprestimos(emprestimos, numEmprestimos);
            break;
          }
        } while (opcao != 0);
      }
      break;
    }
  } while (opcao != 0);

  fclose(fileBinLivros);
  fclose(fileBinUsuarios);

  return 0;
}