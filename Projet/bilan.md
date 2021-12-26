# Bilan pour le projet réalisé 

## Mesure du temps en sequentiel
Avec affichage = 115ms par pas de temp
Sans affichage = 26ms par pas de temp

Affichage correspond à 77% du temps de simulation

## Mesure du temps avec affichage synchrone et MPI
Avec affichage = 113ms par pas de temp
Sans affichage = 40ms par pas de temp

Sans modification de temps avec l'affichage, la simulation doit de même façon attendre l'affichage pour pouvoir envoyer les
données.

Sans affichage le temps est plus grande, ça c'est le resultat des échanges de message necessaires pour finalizer l'application correctement

