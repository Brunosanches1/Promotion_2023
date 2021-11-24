# TP2 de MACEDO SANCHES Bruno

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`

## Mandelbrot 

*Expliquer votre stratégie pour faire une partition équitable des lignes de l'image entre chaque processus*
       
Chaque processus va traiter H/nbp lignes           
par exemple avec 3 processus et 600 lignes, le processus 0 traite de 400 a 599,
le processus 1 traite de 200 a 399 e le 2 traite de 0 a 199 et après est faite un gather

           | Taille image : 800 x 600 | 
-----------+---------------------------
séquentiel |              
1          |    327.429   
2          |    181.380 
3          |    162.368
4          |    131.393
8          |    227.389


*Discuter sur ce qu'on observe, la logique qui s'y cache.*

*Expliquer votre stratégie pour faire une partition dynamique des lignes de l'image entre chaque processus*

Avec plus d'un processus le processus 0 envoye les numeros 0 a nbp-2 à les processus 1 a nbp-1            
et chacun va processer la ligne envoyée, envoyer la ligne résultat et attendre recevoir 
le proche valeur 

           | Taille image : 800 x 600 | 
-----------+---------------------------
séquentiel | 19.2889  
1          | 19.2889         
2          | 22.2126
3          | 12.4729
4          | 9.3428
8          | 7.6795       



## Produit matrice-vecteur



*Expliquer la façon dont vous avez calculé la dimension locale sur chaque processus, en particulier quand le nombre de processus ne divise pas la dimension de la matrice.*
Pour le cas des matrices colonnes la dimension locale est la meme, chaque processus va juste faire moins de calculs et apres
c'est necessaire de realiser un MPI_ReduceAll pour realiser la somme des valeurs, en cas d'avoir un numero de processus qui divise pas 
on peut calculer un numero additionnel de colones pour chaque processeus de forme a ne supercharger pas 

Pour le cas des matrices lignes la dimension change, on peut calculer un offset de meme facon que pour les colonnes mais maintenant il faut
utiliser le Reduce e un vecteur de taile N avec des zeros dans les valeurs pas calcules, si on assume qui le numero de processus est diviseur de N
il faut utiliser un gather qui peut reduire le traffic sur le réseaux
