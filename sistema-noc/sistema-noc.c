#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Garante compatibilidade multiplataforma para rotinas de delay visual
#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#define MAX_SERVIDORES 4
#define MAX_ALERTAS 100

typedef enum { STATUS_OK, STATUS_ALERTA, STATUS_CRITICO, STATUS_DOWN } StatusServidor;
#define STATUS_UP STATUS_OK

// Estrutura atualizada com limites de hardware individualizados
typedef struct {
    int id;
    char nome[30];
    char ip[15];
    float usoCPU;       
    float usoMemoria;   
    float latenciaMs;   
    StatusServidor status;
    float limiteCPU;     // Limite customizado para este servidor
    float limiteMemoria; // Limite customizado para este servidor
} Servidor;

typedef struct {
    char timestamp[20];
    int idServidor;
    char componente[15]; 
    char descricao[60];
} AlertaLog;

// Função de atualização inteligente respeitando limites individuais e indisponibilidade
void AtualizarTelemetria(Servidor *s, AlertaLog *historico, int *totalAlertas) {
    // Se o servidor foi derrubado manualmente pelo administrador, ele não gera métricas normais
    if (s->status == STATUS_DOWN) {
        s->usoCPU = 0.0f;
        s->usoMemoria = 0.0f;
        s->latenciaMs = 9999.0f; // Latência infinita (Timeout)
        return;
    }

    // Flutuação randômica realista de carga de trabalho
    s->usoCPU = 15.0f + (rand() % 81);      
    s->usoMemoria = 30.0f + (rand() % 66);  
    s->latenciaMs = 5.0f + (rand() % 146);  

    // Validação de anomalias baseada nos limites específicos configurados para cada hardware
    if (s->usoCPU > s->limiteCPU || s->usoMemoria > s->limiteMemoria || s->latenciaMs > 120.0f) {
        s->status = STATUS_CRITICO;
    } else if (s->usoCPU > (s->limiteCPU - 15.0f) || s->usoMemoria > (s->limiteMemoria - 15.0f) || s->latenciaMs > 80.0f) {
        s->status = STATUS_ALERTA;
    } else {
        s->status = STATUS_OK;
    }

    // Registro automático de logs em caso de quebra de SLA crítico
    if (s->status == STATUS_CRITICO && *totalAlertas < MAX_ALERTAS) {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        strftime(historico[*totalAlertas].timestamp, 20, "%d/%m %H:%M:%S", tm_info);
        
        historico[*totalAlertas].idServidor = s->id;
        
        if (s->usoCPU > s->limiteCPU) {
            strcpy(historico[*totalAlertas].componente, "CPU");
            sprintf(historico[*totalAlertas].descricao, "Uso (%.1f%%) excedeu o limite customizado de %.1f%%", s->usoCPU, s->limiteCPU);
        } else if (s->usoMemoria > s->limiteMemoria) {
            strcpy(historico[*totalAlertas].componente, "MEMORIA");
            sprintf(historico[*totalAlertas].descricao, "Uso (%.1f%%) excedeu o limite customizado de %.1f%%", s->usoMemoria, s->limiteMemoria);
        } else {
            strcpy(historico[*totalAlertas].componente, "REDE");
            sprintf(historico[*totalAlertas].descricao, "Latencia critica detectada: %.1fms", s->latenciaMs);
        }
        (*totalAlertas)++;
    }
}

void RetornarStatusTexto(StatusServidor status, char *buffer) {
    switch (status) {
        case STATUS_OK:       strcpy(buffer, "OK"); break;
        case STATUS_ALERTA:   strcpy(buffer, "ALERTA"); break;
        case STATUS_CRITICO:  strcpy(buffer, "CRITICO"); break;
        case STATUS_DOWN:     strcpy(buffer, "DOWN"); break;
    }
}

int main() {
    srand(time(NULL));

    // Configuração dos Servidores com Limites Customizados (Banco de dados e API são mais rígidos)
    Servidor infraestrutura[MAX_SERVIDORES] = {
        {101, "Servidor de Producao", "192.168.1.10", 0, 0, 0, STATUS_OK, 85.0f, 85.0f}, // Limite padrão
        {102, "Banco de Dados SQL",   "192.168.1.20", 0, 0, 0, STATUS_OK, 75.0f, 70.0f}, // Muito rígido (Alerta cedo)
        {103, "API Gateway Central",  "192.168.1.30", 0, 0, 0, STATUS_OK, 80.0f, 80.0f}, // Rígido
        {104, "Servidor de Backup",   "192.168.1.40", 0, 0, 0, STATUS_OK, 95.0f, 95.0f}  // Flexível (Suporta alta carga)
    };

    AlertaLog painelAlertas[MAX_ALERTAS];
    int totalAlertas = 0;
    int opcao = 0;
    int idBusca, encontrado;

    while (opcao != 6) {
        printf("\n=========================================================================");
        printf("\n           SISTEMA NOC AVANCADO - CONTROLE E TELEMETRIA DE TI            ");
        printf("\n=========================================================================");
        printf("\n1. Dashboard de Infraestrutura (Com Limites Customizados)");
        printf("\n2. Executar Varredura Geral e Atualizar Sensores");
        printf("\n3. Visualizar Painel de Incidentes e Histórico de SLA");
        printf("\n4. Simular DOWNTIME / Desligamento Manual de Ativo");
        printf("\n5. Executar BOOT / Reativação de Ativo em Falha");
        printf("\n6. Encerrar Painel de Controle NOC");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                printf("\n--- DASHBOARD CENTRAL DE INFRAESTRUTURA ---\n");
                printf("%-4s | %-20s | %-13s | %-11s | %-11s | %-7s | %-8s\n", 
                       "ID", "Nome do Ativo", "IP", "CPU (Lim)", "MEM (Lim)", "LAT", "STATUS");
                printf("-----------------------------------------------------------------------------------------\n");
                
                char txtStatus[15];
                for (int i = 0; i < MAX_SERVIDORES; i++) {
                    RetornarStatusTexto(infraestrutura[i].status, txtStatus);
                    
                    char cpuStr[15], memStr[15];
                    if (infraestrutura[i].status == STATUS_DOWN) {
                        strcpy(cpuStr, "0.0% (--%)");
                        strcpy(memStr, "0.0% (--%)");
                    } else {
                        sprintf(cpuStr, "%4.1f%% (%2.0f%%)", infraestrutura[i].usoCPU, infraestrutura[i].limiteCPU);
                        sprintf(memStr, "%4.1f%% (%2.0f%%)", infraestrutura[i].usoMemoria, infraestrutura[i].limiteMemoria);
                    }

                    printf("%-4d | %-20s | %-13s | %-11s | %-11s | %5.1fms | %-8s\n",
                           infraestrutura[i].id,
                           infraestrutura[i].nome,
                           infraestrutura[i].ip,
                           cpuStr,
                           memStr,
                           infraestrutura[i].latenciaMs,
                           txtStatus);
                }
                break;
            }

            case 2:
                printf("\n[NOC] Requisitando dados dos agentes de monitoramento...");
                fflush(stdout);
                for (int i = 0; i < 3; i++) {
                    SLEEP_MS(300);
                    printf(".");
                    fflush(stdout);
                }
                for (int i = 0; i < MAX_SERVIDORES; i++) {
                    AtualizarTelemetria(&infraestrutura[i], painelAlertas, &totalAlertas);
                }
                printf("\n[SUCESSO] Telemetria atualizada de acordo com as regras de SLA individuais!\n");
                break;

            case 3:
                printf("\n--- LOG DE INCIDENTES CRITICOS REGISTRADOS ---\n");
                if (totalAlertas == 0) {
                    printf("Nenhuma quebra de SLA detectada na infraestrutura.\n");
                } else {
                    printf("%-15s | %-8s | %-10s | %-45s\n", "Data/Hora", "ID Ativo", "Hardware", "Detalhes do Incidente");
                    printf("-----------------------------------------------------------------------------------------\n");
                    for (int i = 0; i < totalAlertas; i++) {
                        printf("%-15s | %-8d | %-10s | %-45s\n",
                               painelAlertas[i].timestamp,
                               painelAlertas[i].idServidor,
                               painelAlertas[i].componente,
                               painelAlertas[i].descricao);
                    }
                }
                break;

            case 4:
                // IMPLEMENTAÇÃO: Simulação manual de queda (Downtime)
                printf("\nDigite o ID do servidor que deseja derrubar/desligar: ");
                scanf("%d", &idBusca);
                encontrado = 0;

                for (int i = 0; i < MAX_SERVIDORES; i++) {
                    if (infraestrutura[i].id == idBusca) {
                        encontrado = 1;
                        if (infraestrutura[i].status == STATUS_DOWN) {
                            printf("\n[AVISO] O servidor %s ja se encontra em estado offline (DOWN).\n", infraestrutura[i].nome);
                        } else {
                            infraestrutura[i].status = STATUS_DOWN;
                            infraestrutura[i].usoCPU = 0.0f;
                            infraestrutura[i].usoMemoria = 0.0f;
                            infraestrutura[i].latenciaMs = 9999.0f;

                            // Registra a queda forçada no histórico de incidentes
                            if (totalAlertas < MAX_ALERTAS) {
                                time_t t = time(NULL);
                                struct tm *tm_info = localtime(&t);
                                strftime(painelAlertas[totalAlertas].timestamp, 20, "%d/%m %H:%M:%S", tm_info);
                                painelAlertas[totalAlertas].idServidor = idBusca;
                                strcpy(painelAlertas[totalAlertas].componente, "SISTEMA");
                                sprintf(painelAlertas[totalAlertas].descricao, "DOWNTIME FORCADO: Desligamento manual pelo administrador");
                                painelAlertas[totalAlertas].timestamp[19] = '\0';
                                totalAlertas++;
                            }
                            printf("\n[ALERTA NOC] O servidor %s foi desligado! Status alterado para DOWN.\n", infraestrutura[i].nome);
                        }
                        break;
                    }
                }
                if (!encontrado) printf("\n[ERRO] Nenhum ativo de TI localizado com o ID %d.\n", idBusca);
                break;

                case 5:
                //IMPLEMENTAÇÃO: Comando manual de boot para recuperar o serviço 
                printf("\nDigite o ID do servidor que deseja reativar (Boot): ");
                scanf("%d", &idBusca);
                encontrado = 0;

                for (int i = 0; i < MAX_SERVIDORES; i++) {
                    if (infraestrutura[i].id == idBusca) {
                        encontrado = 1;
                        if (infraestrutura[i].status != STATUS_DOWN) {
                            printf("\n[AVISO] O servidor %s ja esta ativo e operando em rede\n", infraestrutura[i].nome);
                        } else {
                            infraestrutura[i].status = STATUS_OK;
                            printf("\n[NOC] Enviando o sinal de boot para a maquina..."); 
                            fflush(stdout);
                            SLEEP_MS(800);

                            // Força uma atualização imediata pós-boot para sair do zero
                            AtualizarTelemetria(&infraestrutura[i], painelAlertas, &totalAlertas);
                            printf("\n[SUCESSO] O servidor %s foi reiniciado com sucesso e esta online!\n", infraestrutura[i].nome);
                        }
                        break;
                    }
                }
                if (!encontrado) printf("\n[ERRO] Nenhum ativo de TI localizado com o ID %d.\n", idBusca);
                break;

                case 6:
                printf("\nDesconectando do barramento corporativo. Painel NOC desativado.\n");
                break;

                default:
                printf("\nComando invalido! Tente novamente.\n");
            }
    }
    return 0;
}