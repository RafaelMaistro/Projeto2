#include "tarefa.h"

int main() {
  FILE *fileBinUsuarios = abrirArquivoBinario("usuarios.bin");
  FILE *fileBinLivros = abrirArquivoBinario("livros.bin");
  FILE *fileTxtHistorico = abrirArquivoTexto("historico.txt");
  Usuario usuarioLogado;
  Emprestimo emprestimos[MAX_EMPRESTIMOS];
  int numEmprestimos = 0;
  int logado = 0;

  printf("Bem-vindo ao Sistema de Empréstimos de Livros!\n");

  // Exibe menu de cadastro e login antes do menu principal
  int opcaoInicial;
  do {
    printf("\nMenu Inicial:\n");
    printf("1. Cadastrar usuário\n");
    printf("2. Fazer login\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcaoInicial);

    switch (opcaoInicial) {
    case 1:
      cadastrarUsuario(fileBinUsuarios);
      break;
    case 2:
      logado = realizarLogin(fileBinUsuarios, &usuarioLogado);
      break;
    case 0:
      printf("Saindo...\n");
      return 0;
    default:
      printf("Opção inválida! Tente novamente.\n");
      break;
    }
  } while (!logado);

  // Menu principal após login
  int opcao;
  do {
    printf("\nMenu Principal:\n");
    printf("1. Cadastrar livro\n");
    printf("2. Listar livros\n");
    printf("3. Excluir livro\n");
    printf("4. Realizar empréstimo\n");
    printf("5. Devolver livro\n");
    printf("6. Visualizar histórico\n");
    printf("7. Listar empréstimos ativos\n");
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
      excluirLivro(fileBinLivros);
      break;
    case 4:
      realizarEmprestimo(fileBinLivros, fileBinUsuarios, &usuarioLogado,
                         emprestimos, &numEmprestimos);
      break;
    case 5:
      devolverLivro(fileBinLivros, fileBinUsuarios, &usuarioLogado, emprestimos,
                    &numEmprestimos);
      break;
    case 6:
      visualizarHistorico(fileTxtHistorico);
      break;
    case 7:
      listarEmprestimosAtivos(emprestimos, numEmprestimos, &usuarioLogado);
      break;
    case 0:
      printf("Saindo...\n");
      break;
    default:
      printf("Opção inválida! Tente novamente.\n");
      break;
    }
  } while (opcao != 0);

  fclose(fileBinUsuarios);
  fclose(fileBinLivros);
  fclose(fileTxtHistorico);

  return 0;
}
