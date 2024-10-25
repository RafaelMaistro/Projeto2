#ifndef TAREFA_H
#define TAREFA_H

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

FILE *abrirArquivoBinario(const char *nomeArquivo);
FILE *abrirArquivoTexto(const char *nomeArquivo);
void obterDataHora(char *buffer, int tamanhoBuffer);
void cadastrarUsuario(FILE *fileBin);
int realizarLogin(FILE *fileBinUsuarios, Usuario *usuarioLogado);
void cadastrarLivro(FILE *fileBin);
void listarLivros(FILE *fileBin);
void excluirLivro(FILE *fileBin);
void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios,
                        Usuario *usuarioLogado, Emprestimo *emprestimos,
                        int *numEmprestimos);
void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios,
                   Usuario *usuarioLogado, Emprestimo *emprestimos,
                   int *numEmprestimos);
void visualizarHistorico(FILE *fileTxtHistorico);
void listarEmprestimosAtivos(Emprestimo *emprestimos, int numEmprestimos,
                             Usuario *usuarioLogado);

#endif // TAREFA_H
