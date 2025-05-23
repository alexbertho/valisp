#include <stdio.h>

#include "allocateur.h"
#include "erreur.h"
#include "types.h"
#include "lifo.h"
#include "environnement.h"


void ramasse_miette_parcourir_et_marquer(sexpr s) {
    if (s == NULL) return;
    if (ramasse_miette_lire_marque(s)) return;
    
    ramasse_miette_poser_marque(s);
    
    if (cons_p(s)) {
        ramasse_miette_parcourir_et_marquer(car(s));
        ramasse_miette_parcourir_et_marquer(cdr(s));
    } else if (string_p(s) || symbol_p(s)) {
        char *str = string_p(s) ? get_string(s) : get_symbol(s);
        if (str != NULL) {
            ramasse_miette_poser_marque(str);
        }
    }
}

void ramasse_miette_parcourir_pile() {
    pile_parcourir(ramasse_miette_parcourir_et_marquer);
}

void valisp_ramasse_miettes(sexpr env) {
    reinitialiser_marques(); 
    ramasse_miette_parcourir_et_marquer(env);
    ramasse_miette_parcourir_pile();
    ramasse_miette_liberer();
}

void *valisp_malloc(size_t size) {
    void *ptr = allocateur_malloc(size);

    if (ptr == NULL) {
        valisp_ramasse_miettes(environnement_global());
        ptr = allocateur_malloc(size);
        if (ptr == NULL) {
            erreur(MEMOIRE, "malloc", "Allocation de mémoire échouée",NULL);
        }
    }

    pile_ajout(ptr);
    
    return ptr;
}

void valisp_free(void *ptr) {
    if (ptr == NULL) return;
    allocateur_free(ptr);
}