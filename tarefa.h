// tarefa.h
#ifndef TAREFA_H
#define TAREFA_H

#include <stdio.h>

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

typedef struct {
  char usuario[50];
  char livro[50];
  char autor[50];
  int tipo; 
} Emprestimo;

// Funções
FILE *abrirArquivoBinario(const char *nomeArquivo);
void cadastrarUsuario(FILE *fileBin);
int realizarLogin(FILE *fileBinUsuarios, Usuario *usuarioLogado);
void cadastrarLivro(FILE *fileBin);
void listarLivros(FILE *fileBin);
void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos);
void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos);
void buscarOuVisualizarLivro(FILE *fileBin);
void carregarEmprestimos(Emprestimo *emprestimos, int *numEmprestimos);
void exibirEmprestimos(Emprestimo *emprestimos, int numEmprestimos);

#endif // TAREFA_H
