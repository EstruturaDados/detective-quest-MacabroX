#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TAM 10

/* ---------------- ESTRUTURAS ---------------- */

typedef struct Sala {
    char nome[30];
    char pista[50];     // pista opcional
    struct Sala *esq;
    struct Sala *dir;
} Sala;

typedef struct BST {
    char pista[50];
    struct BST *esq;
    struct BST *dir;
} BST;

/* Hash: pista → suspeito */
typedef struct {
    char pista[50];
    char suspeito[50];
    int usado;
} Hash;

/* ---------------- PROTÓTIPOS ---------------- */

Sala* criarSala(const char *nome, const char *pista);
void explorarSala(Sala *s, BST **arvore, Hash hash[]);
BST* inserirPista(BST *root, const char *pista);
void adicionarPista(BST **root, const char *pista);
void inserirNaHash(Hash hash[], const char *pista, const char *suspeito);
int hashFunc(const char *str);
void encontrarSuspeito(Hash hash[], BST *root);
void verificarSuspeitoFinal(Hash hash[], BST *root);

/* ---------------- CRIAÇÃO DO MAPA ---------------- */

Sala* montarMapa() {
    /* Árvore fixa (simplificação solicitada) */

    Sala *hall     = criarSala("Hall", "luvas sujas");
    Sala *cozinha  = criarSala("Cozinha", "faca ensanguentada");
    Sala *quarto   = criarSala("Quarto", "perfume caro");
    Sala *garagem  = criarSala("Garagem", "chave inglesa");
    Sala *jardim   = criarSala("Jardim", "");

    hall->esq = cozinha;
    hall->dir = quarto;
    cozinha->esq = garagem;
    cozinha->dir = jardim;

    return hall;
}

/* ---------------- FUNÇÕES DE CRIAÇÃO ---------------- */

Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = malloc(sizeof(Sala));
    strcpy(s->nome, nome);
    strcpy(s->pista, pista);
    s->esq = s->dir = NULL;
    return s;
}

/* ---------------- BST DE PISTAS ---------------- */

BST* inserirPista(BST *root, const char *pista) {
    if (root == NULL) {
        BST *novo = malloc(sizeof(BST));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, root->pista) < 0)
        root->esq = inserirPista(root->esq, pista);
    else if (strcmp(pista, root->pista) > 0)
        root->dir = inserirPista(root->dir, pista);

    return root;
}

void adicionarPista(BST **root, const char *pista) {
    if (strlen(pista) == 0) return; // sala sem pista
    *root = inserirPista(*root, pista);
}

/* ---------------- HASH ---------------- */

int hashFunc(const char *str) {
    int soma = 0;
    for (int i = 0; str[i]; i++) soma += str[i];
    return soma % HASH_TAM;
}

void inserirNaHash(Hash hash[], const char *pista, const char *suspeito) {
    int h = hashFunc(pista);

    /* Inserção linear */
    for (int i = 0; i < HASH_TAM; i++) {
        int pos = (h + i) % HASH_TAM;
        if (!hash[pos].usado) {
            strcpy(hash[pos].pista, pista);
            strcpy(hash[pos].suspeito, suspeito);
            hash[pos].usado = 1;
            return;
        }
    }
}

/* -------------- EXPLORAÇÃO DAS SALAS --------------- */

void explorarSala(Sala *s, BST **arvore, Hash hash[]) {
    if (!s) return;

    printf("\nVocê entrou na sala: %s\n", s->nome);

    if (strlen(s->pista) > 0) {
        printf("Você encontrou uma pista: \"%s\"\n", s->pista);
        adicionarPista(arvore, s->pista);
    } else {
        printf("Nenhuma pista encontrada aqui.\n");
    }

    printf("\nMover (e) esquerda, (d) direita, (s) sair exploração: ");
}

/* -------------- ENCONTRAR SUSPEITO ------------- */

void encontrarSuspeito(Hash hash[], BST *root) {
    if (!root) return;

    int h = hashFunc(root->pista);

    for (int i = 0; i < HASH_TAM; i++) {
        int pos = (h + i) % HASH_TAM;

        if (hash[pos].usado && strcmp(hash[pos].pista, root->pista) == 0) {
            printf("Pista \"%s\" aponta para o suspeito: %s\n",
                   root->pista, hash[pos].suspeito);
        }
    }

    encontrarSuspeito(hash, root->esq);
    encontrarSuspeito(hash, root->dir);
}

/* ----------- VERIFICAÇÃO FINAL DO ACUSADO ------------ */

void verificarSuspeitoFinal(Hash hash[], BST *root) {
    char suspeitoMaisProvavel[50] = "";
    int contagemMax = 0;

    /* Varredura por cada suspeito na hash */
    for (int i = 0; i < HASH_TAM; i++) {
        if (!hash[i].usado) continue;

        char atual[50];
        strcpy(atual, hash[i].suspeito);

        int cont = 0;

        /* Conta quantas pistas na BST apontam para este suspeito */
        BST *pilha[50];
        int topo = -1;
        BST *p = root;

        /* Percorrer BST (DFS manual) */
        while (p || topo >= 0) {
            while (p) {
                pilha[++topo] = p;
                p = p->esq;
            }
            p = pilha[topo--];

            /* Se pista corresponde ao suspeito */
            if (strcmp(hash[i].suspeito, atual) == 0 &&
                hashFunc(p->pista) == hashFunc(hash[i].pista)) {
                cont++;
            }
            p = p->dir;
        }

        if (cont > contagemMax) {
            contagemMax = cont;
            strcpy(suspeitoMaisProvavel, atual);
        }
    }

    printf("\nCom base nas pistas, o suspeito mais provável é: %s\n",
           suspeitoMaisProvavel);
}

/* -------------------------- MAIN -------------------------- */

int main() {
    Sala *atual = montarMapa();
    BST *pistasColetadas = NULL;
    Hash hash[HASH_TAM] = {0};

    /* Inserção fixa das pistas → suspeitos (simplificação) */
    inserirNaHash(hash, "luvas sujas", "Carlos");
    inserirNaHash(hash, "faca ensanguentada", "Maria");
    inserirNaHash(hash, "perfume caro", "Ana");
    inserirNaHash(hash, "chave inglesa", "João");

    char cmd;

    printf("=== Mansão Misteriosa ===\n");

    do {
        explorarSala(atual, &pistasColetadas, hash);
        scanf(" %c", &cmd);

        if (cmd == 'e') atual = atual->esq;
        else if (cmd == 'd') atual = atual->dir;

    } while (cmd != 's');

    printf("\n=== Analisando pistas coletadas ===\n");
    encontrarSuspeito(hash, pistasColetadas);

    printf("\n=== Julgamento Final ===\n");
    verificarSuspeitoFinal(hash, pistasColetadas);

    return 0;
}