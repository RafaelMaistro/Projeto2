#include "tarefa.h"

// Implementação das funções

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
               "deseja adicionar? ", livro.titulo, livro.autor);
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
            printf("Livro \"%s\" de \"%s\" excluído com sucesso!\n", tituloLivro, autorLivro);
            continue; 
        }
        fwrite(&livro, sizeof(Livro), 1, fileBinTemp); 
    }

    if (!livroEncontrado) {
        printf("Livro \"%s\" de \"%s\" não encontrado.\n", tituloLivro, autorLivro);
    }

    fclose(fileBinTemp);
    fclose(fileBin);

    rename("livros_temp.bin", "livros.bin");
    fileBin = abrirArquivoBinario("livros.bin"); 
}

void realizarEmprestimo(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos) {
    char tituloLivro[50];
    char autorLivro[50];
    Livro livro;
    Emprestimo emprestimo;

    printf("Digite o título do livro: ");
    getchar();
    fgets(tituloLivro, 50, stdin);
    tituloLivro[strcspn(tituloLivro, "\n")] = 0;

    printf("Digite o nome do autor: ");
    fgets(autorLivro, 50, stdin);
    autorLivro[strcspn(autorLivro, "\n")] = 0;

    rewind(fileBinLivros);
    int livroEncontrado = 0;

    while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
        if (strcmp(livro.titulo, tituloLivro) == 0 &&
            strcmp(livro.autor, autorLivro) == 0 && livro.exemplares_disponiveis > 0) {
            livroEncontrado = 1;
            livro.exemplares_disponiveis--;
            strcpy(emprestimo.usuario, usuarioLogado->nome);
            strcpy(emprestimo.livro, livro.titulo);
            strcpy(emprestimo.autor, livro.autor);
            emprestimo.tipo = 1; // 1 indica empréstimo
            obterDataHora(emprestimo.dataHora, sizeof(emprestimo.dataHora));
            emprestimos[*numEmprestimos] = emprestimo;
            (*numEmprestimos)++;

            printf("Empréstimo realizado com sucesso!\n");
            break;
        }
    }

    if (livroEncontrado) {
        fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
        fwrite(&livro, sizeof(Livro), 1, fileBinLivros); 
    } else {
        printf("Livro não encontrado ou não há exemplares disponíveis.\n");
    }
}

void devolverLivro(FILE *fileBinLivros, FILE *fileBinUsuarios, Usuario *usuarioLogado, Emprestimo *emprestimos, int *numEmprestimos) {
    char tituloLivro[50];
    char autorLivro[50];
    Livro livro;
    int emprestimoEncontrado = 0;

    printf("Digite o título do livro a ser devolvido: ");
    getchar();
    fgets(tituloLivro, 50, stdin);
    tituloLivro[strcspn(tituloLivro, "\n")] = 0;

    printf("Digite o nome do autor: ");
    fgets(autorLivro, 50, stdin);
    autorLivro[strcspn(autorLivro, "\n")] = 0;

    rewind(fileBinLivros);
    while (fread(&livro, sizeof(Livro), 1, fileBinLivros)) {
        if (strcmp(livro.titulo, tituloLivro) == 0 &&
            strcmp(livro.autor, autorLivro) == 0) {
            livro.exemplares_disponiveis++;
            emprestimoEncontrado = 1;
            printf("Devolução realizada com sucesso!\n");
            break;
        }
    }

    if (emprestimoEncontrado) {
        fseek(fileBinLivros, -sizeof(Livro), SEEK_CUR);
        fwrite(&livro, sizeof(Livro), 1, fileBinLivros); 
    } else {
        printf("Livro não encontrado.\n");
    }
}

void visualizarHistorico(FILE *fileTxtHistorico) {
    char linha[256];

    rewind(fileTxtHistorico);
    printf("Histórico de empréstimos:\n");
    while (fgets(linha, sizeof(linha), fileTxtHistorico)) {
        printf("%s", linha);
    }
}

void listarEmprestimosAtivos(Emprestimo *emprestimos, int numEmprestimos, Usuario *usuarioLogado) {
    printf("Empréstimos ativos de %s:\n", usuarioLogado->nome);
    for (int i = 0; i < numEmprestimos; i++) {
        if (strcmp(emprestimos[i].usuario, usuarioLogado->nome) == 0) {
            printf("Título: %s | Autor: %s | Data e Hora: %s\n", 
                   emprestimos[i].livro, emprestimos[i].autor, emprestimos[i].dataHora);
        }
    }
}
