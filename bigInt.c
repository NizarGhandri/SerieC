// This file requires at least C99 to compile

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset()

/* ==========================================================================
 *
 *  L'ÉTUDIANT DEVRAIT ÊTRE EN MESURE DE COMPRENDRE LE CODE CI-DESSOUS,
 *  ET DOIT COMPLÉTER LE CODE MANQUANT (MARQUÉ PAR « TODO »).
 *
 * ========================================================================== */

// -------------------------------------------------------------------------- //
// Déclarations
// Type d'entier non-signé utilisé
typedef unsigned long long bi_block;

// Structure d'un « gros entier »
#define BIGINT_LENGTH 5
typedef struct
{
    size_t length;
    // bi_block blocks[BIGINT_LENGTH];
    bi_block *blocks;
} BigInt;

// Nombre de LSB utilisés: la moitié (donc 4 bits/octet).
#define bi_block_used (4 * sizeof(bi_block))

// Base utilisée (doit être une puissance de 2).
bi_block const bi_base = ((bi_block)1) << bi_block_used;

// Résultat de comparaison
enum bi_cmp
{
    bi_below = -1,
    bi_equal = 0,
    bi_above = 1
};

/** Initialise un nombre avec les blocs donnés.
 * @param bi Nombre à initialiser, devra être finalisé
 * @param val Blocs à affecter, dans l'ordre où ils sont donné, NULL pour représenter 0
 * @param len Nombre d'éléments dans le tableau 'val', ignoré si 'val' est NULL
 * @return Vrai si l'initialisation est un succès, faux sinon (le nombre est alors finalisé)
**/
bool bi_init(BigInt *bi, bi_block const *val, size_t len);

/** Finalise un nombre. (= équivalent d'un destructeur)
 * @param Nombre à finaliser, doit avoir été initialisé
**/
void bi_cleanup(BigInt *);

/** Comparaison de deux BigInts.
 * @param Nombre A, doit avoir été initialisé
 * @param Nombre B, doit avoir été initialisé
 * @return 'bi_below' si A < B, 'bi_above' si A > B, 'bi_equal' si A == B
**/
enum bi_cmp bi_compare(BigInt const *A, BigInt const *B);
static enum bi_cmp cmp_blocks(const bi_block *a, const bi_block *b)
{
    if (*a < *b)
        return bi_below;
    else if (*a == *b)
        return bi_equal;
    else
        return bi_above;
}

/** Modulo d'un BigInt par un bloc.
 * @param Nombre A, doit avoir été initialisé
 * @param Module m, strictement positif
 * @return A % m
**/
bi_block bi_mod(BigInt const *A, bi_block m);

/** Exponentiation modulaire.
 * @param Base B, doit avoir été initialisé
 * @param Exposant E, doit avoir été initialisé
 * @param Module m, strictement positif (retourne 0 sur m est nul)
 * @return B^E % m
**/
bi_block bi_expmod(BigInt const *B, BigInt const *E, bi_block m);

/** Addition de deux BigInts, initialize un troisième avec le résultat.
 * @param Nombre A, doit avoir été initialisé
 * @param Nombre B, doit avoir été initialisé
 * @param Nombre R à initialiser avec A + B, devra être finalisé
 * @return Vrai si l'opération est un succès, faux sinon (alors R doit avoir été finalisé)
**/
bool bi_sum_init(BigInt const *A, BigInt const *B, BigInt *R);

/** Addition de deux BigInts, place le résultat dans le premier.
 * @param Nombre A à remplacer par A + B, doit avoir été initialisé
 * @param Nombre B, doit avoir été initialisé
 * @return Vrai si l'opération est un succès, faux sinon (alors A doit avoir été finalisé)
**/
bool bi_sum_over(BigInt *A, BigInt const *B);

/** Multiplication de deux BigInts, initialize un troisième avec le résultat.
 * @param Nombre A, doit avoir été initialisé
 * @param Nombre B, doit avoir été initialisé
 * @param Nombre R à initialiser avec A * B, devra être finalisé
 * @return Vrai si l'opération est un succès, faux sinon (alors R doit avoir été finalisé)
**/
bool bi_mul_init(BigInt const *A, BigInt const *B, BigInt *R);

/** Multiplication de deux BigInts, place le résultat dans le premier.
 * @param Nombre A à remplacer par A * B, doit avoir été initialisé
 * @param Nombre B, doit avoir été initialisé
 * @return Vrai si l'opération est un succès, faux sinon (alors A doit avoir été finalisé)
**/
bool bi_mul_over(BigInt *A, BigInt const *B);

// -------------------------------------------------------------------------- //
// Opérations fondamentales (= opérations sur des 'bi_block')

/** Avec b la base utilisée, calcule x modulo base.
 * @param x Valeur de x
 * @return x % b
**/
bi_block bi_lower(bi_block x)
{
    return x % bi_base;
}

/** Avec b la base utilisée, calcule x / b.
 * @param x Valeur de x
 * @return x / b
**/
bi_block bi_upper(bi_block x)
{
    return x / bi_base;
}

// -------------------------------------------------------------------------- //
// Affichage d'un nombre (fonction outil fournie)

/** Print le nombre donné.
 * @param p Chaîne de caractères en préfixe
 * @param x Nombre donné, doit avoir été initialisé
**/
void bi_println(const char *p, BigInt const *x)
{
    printf("%s{", p);
    for (size_t i = 0; i < x->length; ++i)
        printf(" 0x%08llx", x->blocks[i]);
    puts(" }");
}

/* ===========================================================================
 * définitions des fonctions demandées
 * =========================================================================== */

// TODO : définir ici les fonctions demandées et supprimer cette ligne (TODO)

bool bi_init(BigInt *bi, bi_block const *val, size_t len)
{
    if (len == 0 || val == NULL)
    {
        bi_cleanup(bi);
        return true;
    }
    else if (bi != NULL && val != NULL && (bi->blocks = calloc(len, sizeof(bi_block))) != NULL)
    {
        bi->length = len;
        for (size_t i = 0; i < len; i++)
            bi->blocks[i] = val[i];
        return true;
    }
    return false;
}

void bi_cleanup(BigInt *b)
{
    if (b != NULL)
    {
        b->length = 0;
        free(b->blocks);
        b->blocks = NULL;
    }
}

enum bi_cmp bi_compare(BigInt const *A, BigInt const *B)
{
    if (A != NULL && B != NULL)
    {
        BigInt const *s = A->length < B->length ? A : B;
        BigInt const *b = A->length >= B->length ? A : B;
        int i = b->length - 1;
        int j = s->length - 1;
        while (i > j)
        {
            if (s->blocks[i] != 0)
            {
                enum bi_cmp a = A->length < B->length ? bi_below : bi_above;
                return a;
            }
            i--;
        }
        while (A->blocks[i] == B->blocks[i] && i > 0)
        {
            i--;
        }
        return cmp_blocks(A->blocks + i, B->blocks + i);
    }
}

bi_block bi_mod(BigInt const *A, bi_block m)
{
    if (A != NULL && m != 0)
    {
        bi_block baseIncrementer = bi_base % m;
        bi_block modBase = 1;
        bi_block sum = 0;
        for (int i = 0; i < A->length; i++)
        {
            sum = (sum + ((A->blocks[i] % m) * modBase % m)) % m;
            modBase = (baseIncrementer * modBase) % m;
        }
        return sum;
    }
}

bi_block bi_expmod(BigInt const *B, BigInt const *E, bi_block m)
{
    if (B != NULL && E != NULL && m != 0)
    {
        bi_block p = 1;
        bi_block b = bi_mod(B, m);
        bi_block copy[E->length];
        for(size_t i = 0; i < E->length; i++)
        {
            copy[i]=E->blocks[i]; 
        }
        for (size_t i = 0; i < E->length; i++)
        {
            for (size_t j = 0; j < bi_block_used; j++)
            {
                if (copy[i] & 1)
                    p = (p * b) % m;
                copy[i] /= 2;
                b = (b * b) % m;
            }
        }
        return p;
    }
}

bool bi_sum_init(BigInt const *A, BigInt const *B, BigInt *R)
{
    if (A != NULL && B != NULL && R != NULL)
    {
        BigInt const *s = A->length < B->length ? A : B;
        BigInt const *b = A->length < B->length ? B : A;
        R->length = b->length + 1;
        if (R->length <= __SIZE_MAX__/ sizeof(bi_block) && (R->blocks = realloc(R->blocks, (R->length) * sizeof(bi_block))) != NULL)
        {
            bi_block carryIn = 0;
            for (int i = 0; i < s->length; i++)
            {
                R->blocks[i] = bi_lower(s->blocks[i] + b->blocks[i] + carryIn);
                carryIn = bi_upper(s->blocks[i] + b->blocks[i] + carryIn);
            }
            for (int i = s->length; i < b->length; i++)
            {
                R->blocks[i] = bi_lower(b->blocks[i] + carryIn);
                carryIn = bi_upper(b->blocks[i] + carryIn);
            }
            R->blocks[R->length - 1] = carryIn;
            return true;
        }
    }
    return false;
}

bool bi_sum_over(BigInt *A, BigInt const *B)
{
    if (A != NULL && B != NULL)
    {
        BigInt const *s = A->length < B->length ? A : B;
        BigInt const *b = A->length < B->length ? B : A;
        A->length = b->length + 1;
        if (A->length <= __SIZE_MAX__/ sizeof(bi_block) && (A->blocks = realloc(A->blocks, (A->length) * sizeof(bi_block))) != NULL)
        {   
            bi_block carryIn = 0;
            for (int i = 0; i < s->length; i++)
            {
                bi_block previous = A->blocks[i];
                A->blocks[i] = bi_lower(previous + B->blocks[i] + carryIn);
                carryIn = bi_upper(previous + B->blocks[i] + carryIn);
            }
             for (int i = s->length; i < b->length; i++)
            {
                bi_block previous = b->blocks[i];
                A->blocks[i] = bi_lower(previous + carryIn);
                carryIn = bi_upper(previous + carryIn);
            }
            A->blocks[A->length - 1] = carryIn;
            return true;
        }
    }
    return false;
}

bool bi_mul_init(BigInt const *A, BigInt const *B, BigInt *R)
{
    R->length = B->length + A->length - 1;
    BigInt const *s = A->length < B->length ? A : B;
    BigInt const *b = A->length < B->length ? B : A;
    R->blocks = realloc(R->blocks, (R->length) * sizeof(bi_block));
    for(size_t j = 0; j < R->length; j++)
        {
            R->blocks[j]=0;
        }
    BigInt a;
    bi_init(&a, R->blocks, R->length);
    bi_block carryIn = 0;
    for (size_t i = 0; i < s->length; i++)
    {
        for(size_t j = 0; j < i; j++)
        {
            a.blocks[j]=0;
        }
        for (size_t j = 0; j < b->length; j++)
        {
            a.blocks[j + i] = bi_lower(s->blocks[i] * b->blocks[j] + carryIn);
            carryIn = bi_upper(s->blocks[i] * b->blocks[j] + carryIn);
        }
        bi_sum_over(R, &a);
    }
    bi_cleanup(&a);

    return true;
}

// -------------------------------------------------------------------------- //
// Tests sommaires

/** Tests sommaires.
 * @return 0 si succès, 1 sinon
**/
int main(void)
{
    { // Tests sommaires de comparaisons et d'exponentiation modulaire
        bi_block m = 373;
        BigInt x, y, z, a;
        memset(&x, 0, sizeof(x));
        memset(&y, 0, sizeof(y));
        memset(&z, 0, sizeof(z));
        bi_block const vx[] = {0xffffffffull, 0xffffffffull, 0xffffffffull, 0xffffffffull};
        bi_block const vy[] = {0x0000ffffull, 0x0000ffffull, 0x0000ffffull, 0x0000ffffull, 0x0000ffffull, 0x0000ffffull};
        bi_block const vz[] = {0xffffffffull, 0xffffffffull, 0xffffffffull, 0xffffffffull, 0x00000000ull};
        bi_block const va[] = {0x1ull};
        if (!bi_init(&x, vx, sizeof(vx) / sizeof(*vx)))
        {
            puts("Erreur: bi_init(&x, ...)");
            return 1;
        }
        if (!bi_init(&y, vy, sizeof(vy) / sizeof(*vy)))
        {
            bi_cleanup(&x);
            puts("Erreur: bi_init(&y, ...)");
            return 1;
        }
        if (!bi_init(&z, vz, sizeof(vz) / sizeof(*vz)))
        {
            bi_cleanup(&y);
            bi_cleanup(&x);
            puts("Erreur: bi_init(&z, ...)");
            return 1;
        }
        if (!bi_init(&a, va, sizeof(va) / sizeof(*va)))
        {
            bi_cleanup(&y);
            bi_cleanup(&x);
            bi_cleanup(&z);
            puts("Erreur: bi_init(&z, ...)");
            return 1;
        }
        bi_println("x = ", &x);
        bi_println("y = ", &y);
        bi_println("z = ", &z);
        printf("bi_compare(&x, &y): %s\n", bi_compare(&x, &y) == bi_below ? "ok" : "erreur");
        printf("bi_compare(&x, &z): %s\n", bi_compare(&x, &z) == bi_equal ? "ok" : "erreur");
        printf("bi_compare(&y, &z): %s\n", bi_compare(&y, &z) == bi_above ? "ok" : "erreur");
        printf("bi_mod(&y, %llu): %s\n", m, bi_mod(&y, m) == 135ull ? "ok" : "erreur");
        printf("bi_expmod(&y, &z, %llu): %s\n", m, bi_expmod(&y, &z, m) == 18ull ? "ok" : "erreur");
        bi_mul_init(&a, &a, &y);
        bi_println("z = ", &y );

        bi_cleanup(&z);
        bi_cleanup(&y);
        bi_cleanup(&x);
        bi_cleanup(&a);
    }

    /* TODO : Optionnel (non noté en soi) mais chaudement recommandé :
     * Tester vos implémentations de 'bi_sum_init', 'bi_sum_over', 'bi_mul_init'
     *     et 'bi_mul_over'.
     */

    return 0;
}
