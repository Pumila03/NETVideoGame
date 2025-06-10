// Nous avons crée ce worker pour pouvoir gérer le solveur:
// Notre solveur n'étais pas assez performant, donc pour certain jeu ça prend un temps trop élevé pour la résolution.
// Comme dans notre implémentation SDL on a décidé d'avoir un timeout après 10 secondes, le problème est que EMSCRIPTEN fonctionne sur un thread unique
// donc on pouvait pas arrêter l'éxécution de la fonction game_solve, donc après des recherches nous avons trouvé le 'worker', qui est comme une sandbox
// qu'on peut utiliser pour générer un thread secondaire, et donc plus facile à gérer car après les 10 secondes de timer on pouvait directement kill ce thread.
// Mais après ça on a eu un autre problème: le pointeur du jeu dans notre env (donc env.g), le thread ne peut pas accéder à la mémoire d'un autre thread
// (donc le thread du worker ne peut pas accéder à celui de demo.js), donc on a du parser notre jeu en JSON, échanger les données du jeu et modifier le jeu ainsi.
// C'est pour ça que nous avons beaucoup de alloc et free (notre version de EMSCRIPTEN ne donne pas accés directement à malloc et free donc on a dû utiliser le wrapper pour ça).
// C'est peut-être pas l'idéal, mais nous avons pas trouvé mieux.

// Nota bene: Le "Module.HEAP32.buffer" est utilisé pour accéder à la mémoire de emiscripten (donc celle suite au Module._alloc_game)

self.importScripts("game.js");
function jeuAvecString(string) {
    return new Promise((solve) => {
        Module.onRuntimeInitialized = () => {
                const data = JSON.parse(string);
                const size = data.rows*data.cols;
                const shapes = Module._alloc(size*4);
                const orientations = Module._alloc(size*4);
                const shapes_tab = new Int32Array(Module.HEAP32.buffer, shapes, size);
                const orientations_tab = new Int32Array(Module.HEAP32.buffer, orientations, size);
                for (let i = 0; i < size; i++) {
                    shapes_tab[i] = data.shapes[i];
                    orientations_tab[i] = data.orientations[i];
                }
                const g = Module._new_game_from_data(data.rows, data.cols, data.wrap, shapes, orientations);
                Module._free_alive(shapes);
                Module._free_alive(orientations);
                solve(g);

        };
    });
}

onmessage = function(e) {
    const gameData = e.data;
    jeuAvecString(gameData)
        .then((game) => {
            if(Module._solve(game)) {
                const resultat = Module._jeu_en_string(game);
                const json = UTF8ToString(resultat);
                Module._free_alive(resultat);
                postMessage(json);
            } else {
                postMessage("erreur");
            }
        });
};

