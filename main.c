// main.c
#include "tarefa.h"

int main() {
  FILE *fileBinLivros = abrirArquivoBinario("livros.bin");
  FILE *fileBinUsuarios = abrirArquivoBinario("usuarios.bin");

  Emprestimo emprestimos[MAX_EMPRESTIMOS];
  int numEmprestimos = 0;

  carregarEmprestimos(emprestimos, &numEmprestimos);

  int opcao;
  Usuario usuarioLogado;

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
      if (realizarLogin(fileBinUsuarios, &usuarioLogado)) {
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
