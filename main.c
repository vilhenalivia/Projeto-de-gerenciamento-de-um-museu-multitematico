#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Constatante para senha de administrador
#define SENHA_ADMIN "admin"
// Constantes para valores dos ingressos
#define VALOR_ENTRADA_INTEIRA 50
#define VALOR_MEIA_ENTRADA 25
#define VALOR_ISENCAO 0

// Estrutura do Tema
typedef struct {
    int id;
    char descricao[200];
} Tema;

// Estrutura das Perguntas
typedef struct {
    char pergunta[200];
    char opcoes[3][50];
} Pergunta;

// Estrutura dos Ingressos
typedef struct {
    char nomeCompleto[100];
    int idTema;
    int tipoIngresso;  // 0 para INTEIRA, 1 para MEIA, 2 para GRATIS
} Ingresso;

typedef struct {
    char resposta;
} Resposta;

// Fun��o para verificar se o arquivo temas.txt est� vazio
int arquivoVazio(FILE *file) {
    fseek(file, 0, SEEK_END);
    return ftell(file) == 0;
}

// Contador adicionado para manter o CMD limpo na primeira execu��o do programa
int pularInicio = 0;
void adicionarTema(FILE *temasFile, Tema novoTema) {
    if (verificarIdExistente(temasFile, novoTema.id)) {
        printf("ID do tema j� em uso. Escolha outro ID.\n");
    } else {
        fprintf(temasFile, "%d %s\n", novoTema.id, novoTema.descricao);

        pularInicio++;

        if (pularInicio > 3) {
            printf("Novo tema adicionado com sucesso!\n");

            fflush(temasFile);
        }
    }
}

// Fun��o para verificar se um ID do tema j� est� em uso
int verificarIdExistente(FILE *temasFile, int id) {
    rewind(temasFile);
    int idExistente;

    while (fscanf(temasFile, "%d", &idExistente) == 1) {
        if (idExistente == id) {
            // ID j� existe, retornar 1 (verdadeiro)
            return 1;
        }
        // Consumir a descri��o do tema
        char descricao[200];
        fgets(descricao, sizeof(descricao), temasFile);
    }

    // ID n�o encontrado, retornar 0 (falso)
    return 0;
}


// Fun��o para exibir temas
void exibirTemas(FILE *temasFile) {
    rewind(temasFile);
    int id;
    char descricao[200];
    while (fscanf(temasFile, "%d %[^\n]", &id, descricao) == 2) {
        printf("ID: %d\nDescri��o: %s\n", id, descricao);
        printf("-------------------------\n");
    }
}

// Fun��o para obter o n�mero da �ltima quest�o no arquivo de perguntas
int obterNumeroUltimaQuestao(FILE *perguntasFile) {
    int numeroQuestao = 0;
    char linha[200];

    rewind(perguntasFile);

    // Percorre o arquivo para encontrar a �ltima quest�o
    while (fgets(linha, sizeof(linha), perguntasFile) != NULL) {
        if (linha[0] >= '0' && linha[0] <= '9') {
            int numeroAtual = atoi(&linha[0]);
            if (numeroAtual > numeroQuestao) {
                numeroQuestao = numeroAtual;
            }
        }
    }

    return numeroQuestao;
}

// Fun��o para adicionar a Pergunta ao arquivo
void adicionarPergunta(FILE *perguntasFile, Pergunta novaPergunta) {
    int numeroUltimaQuestao = obterNumeroUltimaQuestao(perguntasFile);

    fprintf(perguntasFile, "\n%d. %s", numeroUltimaQuestao + 1, novaPergunta.pergunta);

    for (int i = 0; i < 3; ++i) {
        fprintf(perguntasFile, "%c) %s", 'a' + i, novaPergunta.opcoes[i]);
    }
    fflush(perguntasFile);  // Descarrega o buffer para garantir a grava��o imediata
}

// Fun��o para exibir as Perguntas
void exibirPerguntas(FILE *perguntasFile) {
    rewind(perguntasFile);
    char pergunta[200];
    while (fgets(pergunta, sizeof(pergunta), perguntasFile) != NULL) {
        printf("%s", pergunta);
    }
}

// Fun��o para adicionar a resposta ao arquivo
void adicionarResposta(FILE *respostasFile, Resposta novaResposta) {
    fprintf(respostasFile, "%c\n", novaResposta.resposta);
    fflush(respostasFile);
}

void calcularFeedbacks(FILE *respostasFile, FILE *feedbacksFile, FILE *perguntasFile) {
    int totalPerguntas = 0;
    char linha[200];

    // Contar o n�mero total de perguntas
    rewind(perguntasFile);
    while (fgets(linha, sizeof(linha), perguntasFile) != NULL) {
        if (linha[0] >= '0' && linha[0] <= '9') {
            totalPerguntas++;
        }
    }

    // Reiniciar o contador de respostas
    int contagem[3] = {0};  // �ndice 0 para 'a', 1 para 'b', 2 para 'c'

    char resposta;
    fseek(respostasFile, 0, SEEK_SET); // Coloca o cursor no in�cio do arquivo de respostas

    // L� cada resposta e atualiza os contadores
    while (fscanf(respostasFile, " %c", &resposta) == 1) {
        switch (resposta) {
            case 'a':
                contagem[0]++;
                break;
            case 'b':
                contagem[1]++;
                break;
            case 'c':
                contagem[2]++;
                break;
            default:
                // Op��o inv�lida
                break;
        }
    }

    // Calcular a porcentagem e salvar no arquivo de feedbacks
    fprintf(feedbacksFile, "Feedbacks:\n");
    for (int i = 0; i < 3; i++) {
        int totalRespostas = contagem[0] + contagem[1] + contagem[2];
        if (totalRespostas == 0) {
            fprintf(feedbacksFile, "%c) 0.00%%\n", 'a' + i);
        } else {
            fprintf(feedbacksFile, "%c) %.2f%%\n", 'a' + i, (float)contagem[i] / totalRespostas * 100);
        }
    }
    fflush(respostasFile);
    fflush(feedbacksFile);

}

// Fun��o para vender ingressos
void venderIngresso(FILE *ingressosFile) {
    Ingresso novoIngresso;

    // Solicitar informa��es ao visitante
    printf("\nPara cadastrar seu Ingresso precisamos do seu Nome e Id da apresenta��o\n");
    printf("Digite seu nome: ");
    getchar(); // Limpar o buffer do teclado
    fgets(novoIngresso.nomeCompleto, sizeof(novoIngresso.nomeCompleto), stdin);

    printf("Digite o ID do tema que gostaria de visitar: ");
    scanf("%d", &novoIngresso.idTema);

    printf("Escolha o tipo de ingresso:\n");
    printf("0 - Inteira (R$%d)\n", VALOR_ENTRADA_INTEIRA);
    printf("1 - Meia entrada (R$%d)\n", VALOR_MEIA_ENTRADA);
    printf("2 - Isen��o (R$%d)\n", VALOR_ISENCAO);
    printf("Digite o n�mero correspondente ao tipo de ingresso: ");
    scanf("%d", &novoIngresso.tipoIngresso);

    // Calcular o valor total do ingresso
    int valorTotal;
    switch (novoIngresso.tipoIngresso) {
        case 0:
            valorTotal = VALOR_ENTRADA_INTEIRA;
            break;
        case 1:
            valorTotal = VALOR_MEIA_ENTRADA;
            break;
        case 2:
            valorTotal = VALOR_ISENCAO;
            break;
        default:
            printf("Op��o inv�lida. Ingresso n�o vendido.\n");
            return;
    }

    // Salvar as informa��es no arquivo de ingressos
    fprintf(ingressosFile, "Nome: %sID do Tema: %d\nTipo de Ingresso: %d\nValor Total: R$%d\n\n",novoIngresso.nomeCompleto, novoIngresso.idTema, novoIngresso.tipoIngresso, valorTotal);
    fflush(ingressosFile);
    printf("Ingresso vendido com sucesso. Valor total: R$%d\n", valorTotal);
}

// Fun��o para gerar temas iniciais se o arquivo temas.txt estiver vazio
void TemasIniciais(FILE *temasFile) {
    if (arquivoVazio(temasFile)) {
        Tema tema1 = {1, "Edi��o especial em comemora��o aos 100 anos da semana da arte moderna."};
        Tema tema2 = {2, "Edi��o especial em comemora��o aos 150 anos de Santos Dumont, criador do 14-Bis, Rel�gio de pulso e Chuveiro aquecido"};
        Tema tema3 = {3, "Edi��o dedicada aos jogos ol�mpicos de 2024 que ir�o acontecer em Paris no per�odo de 26 de julho at� 11 de agosto"};

        adicionarTema(temasFile, tema1);
        adicionarTema(temasFile, tema2);
        adicionarTema(temasFile, tema3);

        fflush(temasFile);
    }
}

void PerguntasIniciais(FILE *perguntasFile) {
    if (arquivoVazio(perguntasFile)) {
        fprintf(perguntasFile, "1. O que voc� achou da tematica do museu ?\n");
        fprintf(perguntasFile, "a) Bom.\n");
        fprintf(perguntasFile, "b) Mediana.\n");
        fprintf(perguntasFile, "c) Ruim.\n\n");

        fprintf(perguntasFile, "2. Voc� voltaria ao museu novamente?\n");
        fprintf(perguntasFile, "a) Sim.\n");
        fprintf(perguntasFile, "b) N�o.\n");
        fprintf(perguntasFile, "c) Talvez.\n\n");

        fprintf(perguntasFile, "3. Voc� recomendaria o museu a um amigo(a)\n");
        fprintf(perguntasFile, "a) Sim.\n");
        fprintf(perguntasFile, "b) N�o.\n");
        fprintf(perguntasFile, "c) N�o sei.\n\n");

        fprintf(perguntasFile, "4. Como voc� considera a recep��o e atendimento do museu?\n");
        fprintf(perguntasFile, "a) Bom.\n");
        fprintf(perguntasFile, "b) Mediano.\n");
        fprintf(perguntasFile, "c) Ruim.\n");

        fflush(perguntasFile);
    }
}


// Fun��o para responder perguntas
void responderPerguntas(FILE *perguntasFile, FILE *respostasFile) {
    char linha[200];
    char resposta;

    rewind(perguntasFile);

    while (fgets(linha, sizeof(linha), perguntasFile) != NULL) {
        // Verifica se a linha come�a com um n�mero, indicando uma pergunta
        if (linha[0] >= '0' && linha[0] <= '9') {
            printf("%s", linha);  // Exibe a pergunta

            // Exibe as op��es
            for (int i = 0; i < 3; ++i) {
                fgets(linha, sizeof(linha), perguntasFile);
                printf("%s", linha);
            }

            printf("Escolha a op��o (a, b ou c): ");
            scanf(" %c", &resposta);

            Resposta novaResposta;
            novaResposta.resposta = resposta;

            // Adiciona a resposta ao arquivo
            adicionarResposta(respostasFile, novaResposta);
        }
    }

    system("cls");
    printf("Suas respostas foram salvas. Obrigado!\n");
}

void verificarAcesso(FILE *ingressosFile, int idTema, char *nomeCompleto) {
    FILE *tempFile;
    tempFile = fopen("temp.txt", "w"); // Arquivo tempor�rio para armazenar os ingressos que n�o correspondem � entrada desejada

    rewind(ingressosFile);

    char nomeCompletoIngresso[100];
    int idTemaIngresso;
    int tipoIngresso;
    int valorTotal;
    int encontrado = 0;

    while (fscanf(ingressosFile, "Nome: %[^\n]\nID do Tema: %d\nTipo de Ingresso: %d\nValor Total: R$%d\n\n",nomeCompletoIngresso, &idTemaIngresso, &tipoIngresso, &valorTotal) == 4) {
        if (idTema == idTemaIngresso && (nomeCompleto == nomeCompletoIngresso) == 0) {
            encontrado = 1;
            printf("Acesso permitido. Seja bem-vindo!\n");
        } else {
            // Armazenar os ingressos que n�o correspondem � entrada desejada no arquivo tempor�rio
            fprintf(tempFile, "Nome: %s\nID do Tema: %d\nTipo de Ingresso: %d\nValor Total: R$%d\n\n",nomeCompletoIngresso, idTemaIngresso, tipoIngresso, valorTotal);
        }
    }

    fclose(ingressosFile);
    fclose(tempFile);

    // Substituir o arquivo original pelo tempor�rio
    remove("ingressos.txt");
    rename("temp.txt", "ingressos.txt");

    if (!encontrado) {
        system("cls");
        printf("Acesso negado. Verifique o ID do tema e o nome cadastrado no ingresso.\n");
    }
}

// Fun��o principal para visitantes
void menuVisitante(FILE *temasFile, FILE *perguntasFile, FILE *respostasFile, FILE *ingressosFile) {
    int opcao;

    do {
        printf("\nMenu Visitante:\n");
        printf("1. Exibir temas\n");
        printf("2. Responder perguntas\n");
        printf("3. Comprar ingressos\n");
        printf("4. Acessar Tema\n");
        printf("0. Menu anterior\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                system("cls");
                exibirTemas(temasFile);
                break;
            }
            case 2: {
                system("cls");
                responderPerguntas(perguntasFile, respostasFile);
                break;
            }
            case 3: {
                venderIngresso(ingressosFile);
                break;
            }
            case 4: {
                int idTemaEscolhido;
                printf("Digite o ID do tema que gostaria de visitar: ");
                scanf("%d", &idTemaEscolhido);

                getchar();

                printf("Digite seu nome completo: ");
                char nomeCompletoVisitante[100];
                fgets(nomeCompletoVisitante, sizeof(nomeCompletoVisitante), stdin);

                // Verificar o acesso do visitante
                verificarAcesso(ingressosFile, idTemaEscolhido, nomeCompletoVisitante);
                break;
            }
            case 0: {
                system("cls");
                printf("Retornando ao menu anterior.\n");
                break;
            }
            default: {
                printf("Op��o inv�lida. Tente novamente.\n");
                break;
            }
        }

    } while (opcao != 0);
}

// Fun��o principal para administradores
void menuAdministrador(FILE *temasFile, FILE *perguntasFile, FILE *respostasFile, FILE *ingressosFile, FILE *feedbacksFile) {
    int opcao;
    char senha[20];

    printf("Digite a senha de administrador: ");
    scanf("%s", senha);
    system("cls");

    // Verifica a senha
    if (strcmp(senha, SENHA_ADMIN) != 0) {
        printf("Senha incorreta. Acesso negado.\n");
        return;
    }

    do {
        printf("\nMenu Administrador:\n");
        printf("1. Adicionar tema\n");
        printf("2. Exibir temas\n");
        printf("3. Adicionar pergunta\n");
        printf("4. Exibir perguntas\n");
        printf("5. Calcular Feedback\n");
        printf("0. Menu anterior\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                Tema novoTema;
                printf("ID do tema: ");
                scanf("%d", &novoTema.id);

                while ((getchar()) != '\n');

                // Verificar se o ID j� existe
                if (verificarIdExistente(temasFile, novoTema.id)) {
                    printf("ID do tema j� em uso. Escolha outro ID.\n");
                } else {
                    printf("Descri��o do tema: ");
                    fgets(novoTema.descricao, sizeof(novoTema.descricao), stdin);
                    adicionarTema(temasFile, novoTema);
                }
                break;
            }
            case 2: {
                system("cls");
                exibirTemas(temasFile);
                break;
            }
            case 3: {
                Pergunta novaPergunta;

                // Solicitar a nova pergunta ao usu�rio
                printf("Digite a nova pergunta: ");
                getchar();
                fgets(novaPergunta.pergunta, sizeof(novaPergunta.pergunta), stdin);

                for (int i = 0; i < 3; ++i) {
                    printf("Op��o %c: ", 'a' + i);
                    fgets(novaPergunta.opcoes[i], sizeof(novaPergunta.opcoes[i]), stdin);
                }

                // Adicionar a pergunta ao arquivo
                adicionarPergunta(perguntasFile, novaPergunta);
                if (fflush(perguntasFile) == 0) {
                    printf("Pergunta adicionada com sucesso!\n");
                } else {
                    printf("Erro ao salvar a pergunta.\n");
                }
                break;
            }
            case 4: {
                exibirPerguntas(perguntasFile);
                break;
            }
            case 5: {
                calcularFeedbacks(respostasFile, feedbacksFile, perguntasFile);
                system("cls");
                printf("C�lculo de Feedback conclu�do, verifique o arquivo feedbacks.txt.\n");
                break;
            }
            case 0: {
                system("cls");
                printf("Retornando ao menu anterior.\n");
                break;
            }
            default: {
                printf("Op��o inv�lida. Tente novamente.\n");
                break;
            }
        }

    } while (opcao != 0);
}

// Fun��o principal
int main() {
    setlocale(LC_ALL,"");
    FILE *temasFile, *perguntasFile, *respostasFile, *ingressosFile, *feedbacksFile;
    int opcao;

    temasFile = fopen("temas.txt", "a+");
    perguntasFile = fopen("perguntas.txt", "a+");
    respostasFile = fopen("respostas.txt", "a+");
    ingressosFile = fopen("ingressos.txt", "a+");
    feedbacksFile = fopen("feedbacks.txt", "a+");

    if (temasFile == NULL || perguntasFile == NULL || respostasFile == NULL || ingressosFile == NULL || feedbacksFile == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    TemasIniciais(temasFile);
    PerguntasIniciais(perguntasFile);

    do {
        printf("\nMenu Inicial:\n");
        printf("1. Visitante\n");
        printf("2. Administrador\n");
        printf("3. Sair\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                system("cls");
                menuVisitante(temasFile, perguntasFile, respostasFile, ingressosFile);
                break;
            }
            case 2: {
                menuAdministrador(temasFile, perguntasFile, respostasFile, ingressosFile, feedbacksFile);
                break;
            }
            case 3: {
                printf("Encerrando o programa.\n");
                break;
            }
            default: {
                printf("Op��o inv�lida. Tente novamente.\n");
                break;
            }
        }

    } while (opcao != 3);

    // Feche os arquivos
    fclose(temasFile);
    fclose(perguntasFile);
    fclose(respostasFile);
    fclose(ingressosFile);
    fclose(feedbacksFile);

    return 0;
}
