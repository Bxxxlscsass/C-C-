#include <stdio.h>
#include <string.h>

#define MAX_CLIENTES 4
#define MAX_HISTORICO 50
#define VALOR_PARA_DESCONTO 200.0f  
#define TAXA_DESCONTO 0.10f         
#define LIMITE_NEGATIVO -100.0f     

typedef struct {
    int id;
    char nome[50];
    float saldo;
} Cliente;

typedef struct {
    int idCliente;
    char item[50];
    float valorOriginal;
    float valorFinal;
} RegistroVenda;

int main() {
    Cliente listaClientes[MAX_CLIENTES];
    RegistroVenda historico[MAX_HISTORICO];
    int totalVendas = 0;

    // Variáveis para o relatório de faturamento do dia
    float faturamentoTotal = 0.0f;
    float totalDescontosConcedidos = 0.0f;

    int opcao = 0;
    int idBusca;
    float valor;
    int encontrado;

    // Cadastro Inicial
    printf("=== CADASTRO INICIAL DE CLIENTES ===\n");
    for (int i = 0; i < MAX_CLIENTES; i++) {
        printf("\nCliente %d de %d:\n", i + 1, MAX_CLIENTES);
        
        printf("Digite o ID (Numero inteiro): ");
        scanf("%d", &listaClientes[i].id);
        getchar(); 

        printf("Digite o Nome: ");
        fgets(listaClientes[i].nome, sizeof(listaClientes[i].nome), stdin);
        listaClientes[i].nome[strcspn(listaClientes[i].nome, "\n")] = '\0'; 

        printf("Digite o Saldo Inicial: R$ ");
        scanf("%f", &listaClientes[i].saldo);
    }

    // Menu Principal
    while (opcao != 6) {
        printf("\n=== SISTEMA CAIXA AVANCADO ===");
        printf("\n1. Visualizar todos os clientes");
        printf("\n2. Consultar saldo por ID");
        printf("\n3. Realizar pagamento/venda");
        printf("\n4. Adicionar/Depositar saldo (Com quitacao de divida)");
        printf("\n5. Visualizar historico de vendas");
        printf("\n6. Sair do sistema e Gerar Relatorio");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("\n--- LISTA DE CLIENTES ---");
                for (int i = 0; i < MAX_CLIENTES; i++) {
                    printf("\nID: %d | Nome: %-20s | Saldo: R$ %.2f (Limite min: R$ %.2f)", 
                            listaClientes[i].id, 
                            listaClientes[i].nome, 
                            listaClientes[i].saldo,
                            LIMITE_NEGATIVO);
                }
                printf("\n-------------------------\n");
                break;

            case 2:
                printf("\nDigite o ID do cliente: ");
                scanf("%d", &idBusca);
                encontrado = 0;

                for (int i = 0; i < MAX_CLIENTES; i++) {
                    if (listaClientes[i].id == idBusca) {
                        printf("\nCliente: %s", listaClientes[i].nome);
                        printf("\nSaldo atual: R$ %.2f\n", listaClientes[i].saldo);
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) printf("\nCliente nao encontrado.\n");
                break;

            case 3:
                printf("\nDigite o ID do cliente para a venda: ");
                scanf("%d", &idBusca);
                encontrado = 0;

                for (int i = 0; i < MAX_CLIENTES; i++) {
                    if (listaClientes[i].id == idBusca) {
                        encontrado = 1;
                        printf("\nCliente: %s | Saldo: R$ %.2f", listaClientes[i].nome, listaClientes[i].saldo);
                        
                        printf("\nDigite o valor da venda: R$ ");
                        scanf("%f", &valor);
                        getchar(); 

                        if (valor <= 0) {
                            printf("\nValor invalido.\n");
                            break;
                        }

                        float valorFinal = valor;
                        float desconto = 0.0f;
                        if (valor > VALOR_PARA_DESCONTO) {
                            desconto = valor * TAXA_DESCONTO;
                            valorFinal = valor - desconto;
                            printf("\n[DESCONTO] Compra acima de R$ %.2f! Voce ganhou 10%% de desconto (R$ %.2f).", VALOR_PARA_DESCONTO, desconto);
                            printf("\nValor final com desconto: R$ %.2f", valorFinal);
                        }

                        if (listaClientes[i].saldo - valorFinal >= LIMITE_NEGATIVO) {
                            if (totalVendas < MAX_HISTORICO) {
                                printf("\nDigite a descricao do produto: ");
                                fgets(historico[totalVendas].item, sizeof(historico[totalVendas].item), stdin);
                                historico[totalVendas].item[strcspn(historico[totalVendas].item, "\n")] = '\0';
                                
                                historico[totalVendas].idCliente = idBusca;
                                historico[totalVendas].valorOriginal = valor;
                                historico[totalVendas].valorFinal = valorFinal;
                                totalVendas++; 

                                listaClientes[i].saldo -= valorFinal; 
                                
                                // Acumula os dados para o faturamento diário
                                faturamentoTotal += valorFinal;
                                totalDescontosConcedidos += desconto;
                                
                                printf("\nVenda realizada com sucesso!");
                                if (listaClientes[i].saldo < 0) {
                                    printf("\n[ALERTA] O saldo ficou negativo! Saldo atual: R$ %.2f", listaClientes[i].saldo);
                                } else {
                                    printf("\nNovo saldo de %s: R$ %.2f", listaClientes[i].nome, listaClientes[i].saldo);
                                }
                                printf("\n");
                            } else {
                                printf("\nErro: Historico cheio.\n");
                            }
                        } else {
                            printf("\n[REJEITADO] Venda nao autorizada! O valor de R$ %.2f ultrapassa o limite de credito negativo permitido (Max: R$ %.2f).\n", valorFinal, LIMITE_NEGATIVO);
                        }
                        break;
                    }
                }
                if (!encontrado) printf("\nCliente nao encontrado.\n");
                break;

            case 4:
                printf("\nDigite o ID do cliente para deposito: ");
                scanf("%d", &idBusca);
                encontrado = 0;

                for (int i = 0; i < MAX_CLIENTES; i++) {
                    if (listaClientes[i].id == idBusca) {
                        encontrado = 1;
                        printf("\nCliente: %s (Saldo atual: R$ %.2f)", listaClientes[i].nome, listaClientes[i].saldo);
                        printf("\nDigite o valor a ser depositado: R$ ");
                        scanf("%f", &valor);

                        if (valor <= 0) {
                            printf("\nValor invalido.\n");
                        } else {
                            if (listaClientes[i].saldo < 0) {
                                float divida = -listaClientes[i].saldo; 
                                printf("\n[PROCESSANDO] O cliente possui uma divida ativa de R$ %.2f.", divida);
                                if (valor >= divida) {
                                    printf("\n[QUITADA] A divida foi totalmente paga!");
                                    if (valor > divida) {
                                        printf(" O restante (R$ %.2f) foi adicionado como saldo positivo.", valor - divida);
                                    }
                                } else {
                                    printf("\n[PARCIAL] A divida foi reduzida de R$ %.2f para R$ %.2f.", divida, divida - valor);
                                }
                            }

                            listaClientes[i].saldo += valor; 
                            printf("\nDeposito realizado! Novo saldo: R$ %.2f\n", listaClientes[i].saldo);
                        }
                        break;
                    }
                }
                if (!encontrado) printf("\nCliente nao encontrado.\n");
                break;

            case 5:
                printf("\n--- HISTORICO DE VENDAS ---");
                if (totalVendas == 0) {
                    printf("\nNenhuma venda registrada.");
                } else {
                    for (int i = 0; i < totalVendas; i++) {
                        printf("\nVenda #%d | ID Cliente: %d | Item: %-12s | Valor Orig: R$ %.2f | Valor Pago: R$ %.2f", 
                                i + 1, 
                                historico[i].idCliente, 
                                historico[i].item, 
                                historico[i].valorOriginal,
                                historico[i].valorFinal);
                    }
                }
                printf("\n---------------------------\n");
                break;

            case 6:
                // MODIFICAÇÃO: Fechamento de caixa detalhado ao sair
                printf("\n=========================================");
                printf("\n      RELATORIO DE FECHAMENTO DE CAIXA   ");
                printf("\n=========================================");
                printf("\n Total de Vendas Realizadas: %d", totalVendas);
                printf("\n Total de Descontos Concedidos: R$ %.2f", totalDescontosConcedidos);
                printf("\n FATURAMENTO BRUTO TOTAL: R$ %.2f", faturamentoTotal + totalDescontosConcedidos);
                printf("\n FATURAMENTO LIQUIDO RECEBIDO: R$ %.2f", faturamentoTotal);
                
                if (totalVendas > 0) {
                    printf("\n Ticket Medio por Venda: R$ %.2f", faturamentoTotal / totalVendas);
                } else {
                    printf("\nTicket Medio por Venda:R$0.00");
                }
                printf("\n=========================================\n");
                printf("\nEncerrando o sistema. Ate logo!\n");
                break;

                default:
                printf("\nOpcao invalida\n");
            }
    }
    return 0;
}