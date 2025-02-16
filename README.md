```
                               ,----, 
         ,--.                ,/   .`| 
       ,--.'|    ,---,.    ,`   .'  : 
   ,--,:  : |  ,'  .' |  ;    ;     / 
,`--.'`|  ' :,---.'   |.'___,/    ,'  
|   :  :  | ||   |   .'|    :     |   
:   |   \ | ::   :  |-,;    |.';  ;   
|   : '  '; |:   |  ;/|`----'  |  |   
'   ' ;.    ;|   :   .'    '   :  ;   
|   | | \   ||   |  |-,    |   |  '   
'   : |  ; .''   :  ;/|    '   :  |   
|   | '`--'  |   |    \    ;   |.'    
'   : |      |   :   .'    '---'      
;   |.'      |   | ,'                 
'---'        `----'                   
```                 

## Présentation 
Voici le jeu Net,

Ce jeu utilise une grille de 5 cases sur 5, où chaque case contient des pièces représentant des tuyaux ou des câbles. Ces pièces peuvent être pivotées d'un quart de tour à droite ou à gauche. Elles se déclinent en différentes formes : des terminaisons, des segments droits, des coudes, des jonctions en T ou des croix, avec des orientations vers le nord, l'est, le sud ou l'ouest. Chaque pièce est construite à partir de bras partant du centre vers ses bords, appelés demi-arêtes.
Bon courage !

## Jouer

Il vous ai proposé différente manière de jouer au jeu. Afin de pouvoir obtenir des executable pour jouer il faudra executer les commandes suivantes :
```
mkdir build
cd build
cmake ..
make
```
A partir de la deux éxecutables seront créer :

- Pour créer le jeu de base :

```./game_text [game_to_load]```

- Pour créer un jeu aléatoire :

```./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]```

## Les commandes

Les différentes commandes sont :
- ```c I J``` Tourne dans le sens horaire la pièce à la ligne I et à la colonne J
- ```a I J``` Tourne dans le sens anti-horaire la pièce à la ligne I et à la colonne J
- ```r``` Permet de réinitialiser la grille de jeu
- ```z``` Revenir en arrière(Undo)
- ```y``` Revenir en avant(Redo)
- ```q``` Permet de quitter à votre grande honte
- ```h``` Permet d'afficher l'aide

## Les auteurs(A17) :

- Siméon Descorps
- Aala Ajmaime
- Eliott Peaudeau