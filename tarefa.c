// tarefa.c
#include "tarefa.h"
#include <stdlib.h>
#include <string.h>

FILE *abrirArquivoBinario(const char *nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "ab+");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    return file;
}

void cadastrarUsuario(FILE *fileBin) {
    Usuario usuario;
    printf("Nome: ");
    scanf("%s", usuario.nome);
    printf("Senha: ");
    scanf("%s", usuario.senha);
    usuario.num_emprestimos = 0;

    fseek(fileBin, 0, SEEK_END);
    fwrite(&usuario, sizeof(Usuario), 1, fileBin);
    printf("Usuário cadastrado com sucesso!\n");
}

int realizarLogin(FILE *fileBinUsuarios, Usuario *usuarioLogado) {
    char nome[50], senha[20];
    Usuario usuario;

    printf("Nome: ");
    scanf("%s", nome);
    printf("Senha: ");
    scanf("%s", senha);

    fseek(fileBinUsuarios, 0, SEEK_SET);
    while (fread(&usuario, sizeof(Usuario), 1, fileBinUsuarios)) {
        if (strcmp(usuario.nome, nome) == 0 && strcmp(usuario.senha, senha) == 0) {
            *usuarioLogado = usuario;
            printf("Login bem-sucedido!\n");
            return 1;
        }
    }
    printf("Nome ou senha incorretos.\n");
    return 0;
}

void cadastrarLivro(FILE *fileBin) {
    Livro livro;
    printf("Título: ");
    scanf("%s", livro.titulo);
    printf("Autor: ");
    scanf("%s", livro.autor);
    printf("Número de exemplares: ");
    scanf("%d", &livro.num_exemplares);
    livro.exemplares_disponiveis = livro.num_exemplares;

    fseek(fileBin, 0, SEEK_END);
    fwrite(&livro, sizeof(Livro), 1, fileBin);
    printf("Livro cadastrado com sucesso!\n");
}

void listarLivros(FILE *fileBin) {
    Livro livro;
    fseek(fileBin, 0, SEEK_SET);

    while (fread(&livro, sizeof(Livro), 1, fileBin)) {
        printf("Título: %s, Autor: %s, Disponíveis: %d/%d\n", livro.titulo, livro.autor, livro.exemplares_disponiveis, livro.num_exemplares);
    }
}

void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos) {
    char titulo[50];
    Livro livro;
    int livroEncontrado = 0;

    printf("Título do livro a ser emprestado: ");
    scanf("%s", titulo);

    fseek(fileBinLivros, 0, SEEK_SET);
    while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
        if (strcmp(livro.titulo, titulo) == 0 && livro.exemplares_disponiveis > 0) {
            livroEncontrado = 1;
            livro.exemplares_disponiveis--;
            fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
            fwrite(&livro, sizeof(Livro), 1, fileBinLivros);
            printf("Livro emprestado com sucesso!\n");

            Emprestimo novoEmprestimo;
            strcpy(novoEmprestimo.usuario, usuarioLogado->nome);
            strcpy(novoEmprestimo.livro, livro.titulo);
            strcpy(novoEmprestimo.autor, livro.autor);
            novoEmprestimo.tipo = 1; // Empréstimo
            emprestimos[*numEmprestimos] = novoEmprestimo;
            (*numEmprestimos)++;
            break;
        }
    }

    if (!livroEncontrado) {
        printf("Livro não encontrado ou não disponível.\n");
    }
}

void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos) {
    char titulo[50];
    Livro livro;
    int livroEncontrado = 0;

    printf("Título do livro a ser devolvido: ");
    scanf("%s", titulo);

    fseek(fileBinLivros, 0, SEEK_SET);
    while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
        if (strcmp(livro.titulo, titulo) == 0) {
            livroEncontrado = 1;
            livro.exemplares_disponiveis++;
            fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
            fwrite(&livro, sizeof(Livro), 1, fileBinLivros);
            printf("Livro devolvido com sucesso!\n");

            Emprestimo novoEmprestimo;
            strcpy(novoEmprestimo.usuario, usuarioLogado->nome);
            strcpy(novoEmprestimo.livro, livro.titulo);
            strcpy(novoEmprestimo.autor, livro.autor);
            novoEmprestimo.tipo = 2; // Devolução
            emprestimos[*numEmprestimos] = novoEmprestimo;
            (*numEmprestimos)++;
            break;
        }
    }

    if (!livroEncontrado) {
        printf("Livro não encontrado.\n");
    }
}

void buscarOuVisualizarLivro(FILE *fileBin) {
    char titulo[50];
    Livro livro;
    int livroEncontrado = 0;

    printf("Título do livro a ser buscado: ");
    scanf("%s", titulo);

    fseek(fileBin, 0, SEEK_SET);
    while (fread(&livro, sizeof(Livro), 1, fileBin)) {
        if (strcmp(livro.titulo, titulo) == 0) {
            livroEncontrado = 1;
            printf("Título: %s, Autor: %s, Disponíveis: %d/%d\n", livro.titulo, livro.autor, livro.exemplares_disponiveis, livro.num_exemplares);
            break;
        }
    }

    if (!livroEncontrado) {
        printf("Livro não encontrado.\n");
    }
}

void carregarEmprestimos(Emprestimo *emprestimos, int *numEmprestimos) {
    // Neste exemplo, você pode carregar os empréstimos de um arquivo
    // Para simplicidade, inicializamos manualmente
    *numEmprestimos = 0; // Inicia o contador de empréstimos
}

void exibirEmprestimos(Emprestimo *emprestimos, int numEmprestimos) {
    printf("Exibindo empréstimos:\n");
    for (int i = 0; i < numEmprestimos; i++) {
        printf("Usuário: %s, Livro: %s, Autor: %s, Tipo: %s\n",
               emprestimos[i].usuario, emprestimos[i].livro, emprestimos[i].autor,
               emprestimos[i].tipo == 1 ? "Empréstimo" : "Devolução");
    }
}
