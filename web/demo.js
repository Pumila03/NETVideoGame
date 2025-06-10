// demo.js

Module.onRuntimeInitialized = () => { start(); } 

const EMPTY = 0;      // empty shape
const ENDPOINT = 1;   // endpoint shape
const SEGMENT = 2;    // segment shape
const CORNER = 3;     // corner shape
const TEE = 4;        // tee shape
const CROSS = 5;      // cross shape
const NB_SHAPES = 6;  // nb of shapes
const BACKGROUND_IND = 11; // background image
const BORDER_IND = 12; // background image
const CORNER_IND = 13; // background image
const WRAPPING_ON_IND = 14; // background image
const WRAPPING_OFF_IND = 15; // background image
const Z_BUTTON_IND = 16; // background image
const Y_BUTTON_IND = 18; // background image
const S_BUTTON_IND = 20; // background image
const K_BUTTON_IND = 25; // background image
const L_BUTTON_IND = 24; // background image
const R_BUTTON_IND = 22; // background image


const NORTH = 0;      // north
const EAST = 1;       // east
const SOUTH = 2;      // south
const WEST = 3;       // west 
const NB_DIRS = 4;    // nb of directions

const CELL_SIZE = 40;  
const BUTTON_SIZE = 60;

const IMG_BACKGROUND = "res/BACKGROUNDV2.png";
const IMG_BACKGROUND_PIECES = "res/background_pieces.png";
const IMG_BORDER = "res/BORDER.png";
const IMG_BORDER_CORNER = "res/BORDER_CORNER.png";
const IMG_WRAPPING_ON = "res/WRAPPING_ON.png";
const IMG_WRAPPING_OFF = "res/WRAPPING_OFF.png";

const IMG_EMPTY = "res/EMPTY.png";
const IMG_ENDPOINT = "res/ENDPOINT.png";
const IMG_SEGMENT = "res/SEGMENT.png";
const IMG_CORNER = "res/CORNER.png";
const IMG_TEE = "res/TEE.png";
const IMG_CROSS = "res/CROSS.png";

const IMG_CONNECTED_ENDPOINT = "res/ENDPOINT_T_C.png";
const IMG_CONNECTED_SEGMENT = "res/SEGMENT_T_C.png";
const IMG_CONNECTED_CORNER = "res/CORNER_T_C.png";
const IMG_CONNECTED_TEE = "res/TEE_T_C.png";
const IMG_CONNECTED_CROSS = "res/CROSS_T_C.png";

const IMG_Z_BBUTTON = "res/Z_BLUE_BUTTON.png";
const IMG_Y_BBUTTON = "res/Y_BLUE_BUTTON.png";
const IMG_Z_PBUTTON = "res/Z_PINK_BUTTON.png";
const IMG_Y_PBUTTON = "res/Y_PINK_BUTTON.png";
const IMG_S_BBUTTON = "res/S_BLUE_BUTTON.png";
const IMG_R_BBUTTON = "res/R_BLUE_BUTTON.png";
const IMG_S_PBUTTON = "res/S_PINK_BUTTON.png";
const IMG_R_PBUTTON = "res/R_PINK_BUTTON.png";
const IMG_L_PBUTTON = "res/L_PINK_BUTTON.png";
const IMG_K_PBUTTON = "res/K_PINK_BUTTON.png";
const IMG_L_BBUTTON = "res/L_BLUE_BUTTON.png";
const IMG_K_BBUTTON = "res/K_BLUE_BUTTON.png";




var env = {
    g: null, 
    coord_tab: [], 
    canvas: null,
    ctx: null,
    IMG_SHAPES: [IMG_EMPTY, IMG_ENDPOINT, IMG_SEGMENT, IMG_CORNER, IMG_TEE, IMG_CROSS,IMG_CONNECTED_ENDPOINT, IMG_CONNECTED_SEGMENT, IMG_CONNECTED_CORNER, IMG_CONNECTED_TEE, IMG_CONNECTED_CROSS,IMG_BACKGROUND_PIECES, IMG_BORDER, IMG_BORDER_CORNER, IMG_WRAPPING_ON, IMG_WRAPPING_OFF, IMG_Z_PBUTTON,IMG_Z_BBUTTON, IMG_Y_PBUTTON,IMG_Y_BBUTTON,IMG_S_PBUTTON,IMG_S_BBUTTON,IMG_R_PBUTTON,IMG_R_BBUTTON, IMG_L_PBUTTON, IMG_K_PBUTTON, IMG_L_BBUTTON, IMG_K_BBUTTON],
    PIECE_TAB: [],
    imageCache: {},
    Z_pressed : 0,
    Y_pressed : 0,
    K_pressed : 0,
    R_pressed : 0,
    S_pressed : 0,
    L_pressed : 0,
    isAnimating: false,
    movingPieces: [],
    solving: false,
    solution_not_found: false,
    timer: null
}; 
let _square2str = [
    [" ", " ", " ", " "],  // empty
    ["^", ">", "v", "<"],  // endpoint
    ["|", "-", "|", "-"],  // segment
    ["└", "┌", "┐", "┘"],  // corner
    ["┴", "├", "┬", "┤"],  // tee
    ["+", "+", "+", "+"],  // cross
];

function square2str(s, o) {
    return _square2str[s][o];
}

//#define GAME2TAB(i, j, g) (game_nb_cols(g) * (i) + (j))    // To find index x with the two position (i,j)

function GAME2TAB(i, j, g) {
    var nb_cols = Module._nb_cols(g);
    return (nb_cols * i + j);
}

function has_half_edge(env,i,j) {
    for (let d = NORTH; d < NB_DIRS; d++) {
        if (Module._check_edge(env.g, i, j, d) == 2) {
            return true;
        }
    }
    return false;
}


function printGameGraphic(env) {
    env.coord_tab = [];
    var nb_rows = Module._nb_rows(env.g);
    var nb_cols = Module._nb_cols(env.g);

    const background = new Image();
    background.onload = function () {
        env.ctx.drawImage(background, 0, 0, env.canvas.width, env.canvas.height);

    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {

            let s = Module._get_piece_shape(env.g, row, col);
            if (has_half_edge(env, row, col) && s != EMPTY) {
                s += NB_SHAPES - 1;
            }
            let o = Module._get_piece_orientation(env.g, row, col);
            let angle;
            if (env.movingPieces[2] == true && row == env.movingPieces[0] && col == env.movingPieces[1]) {
                angle = (o * 90 + env.movingPieces[3] - 90*env.movingPieces[4]) * Math.PI / 180;
            } else {
                angle = (o * 90) * Math.PI / 180;
            }

            let x = (env.canvas.width - nb_rows * CELL_SIZE) / 2 + (CELL_SIZE * col);
            let y = (env.canvas.height - nb_cols * CELL_SIZE) / 2 + (CELL_SIZE * row);
            env.coord_tab.push([x, y]);
            if (row == 0 && col == 0) {
                loadCornerImage(env,x - CELL_SIZE,y - CELL_SIZE,0);
            } if (row == 0 && col == nb_cols - 1) {
                var is_wrapping = Module._is_wrapping(env.g);
                loadCornerWrappingImage(env,x + CELL_SIZE,y - CELL_SIZE,is_wrapping);
            } if (row == nb_rows - 1 && col == 0) {
                loadCornerImage(env,x - CELL_SIZE,y + CELL_SIZE,3);
            }
            if (row == nb_rows - 1 && col == nb_cols - 1) {
                loadCornerImage(env,x + CELL_SIZE,y + CELL_SIZE,2);
            }  
            if (row == 0) {
                loadBorderImage(env,x,y - CELL_SIZE,0);
            } if (row == nb_rows - 1) {
                loadBorderImage(env,x,y + CELL_SIZE,2);
            } if (col == 0) {
                loadBorderImage(env,x - CELL_SIZE,y,3);
            } if (col == nb_cols - 1) {
                loadBorderImage(env,x + CELL_SIZE,y,1);
            }
            loadBackgroundImage(env, x, y);
            loadPieceImage(env, s, x, y, angle);
            }
        }
    const button_y= (env.canvas.height - nb_cols * CELL_SIZE) / 2 - BUTTON_SIZE*1.5;
    const Zbutton_x=(env.canvas.width)/2-BUTTON_SIZE*3;

    const Zbutton = env.imageCache[Z_BUTTON_IND + env.Z_pressed];
    env.ctx.drawImage(Zbutton, Zbutton_x, button_y, BUTTON_SIZE, BUTTON_SIZE);

    const Ybutton = env.imageCache[Y_BUTTON_IND + env.Y_pressed];
    env.ctx.drawImage(Ybutton, Zbutton_x+BUTTON_SIZE, button_y, BUTTON_SIZE, BUTTON_SIZE);

    const Kbutton = env.imageCache[K_BUTTON_IND + env.K_pressed];
    env.ctx.drawImage(Kbutton, Zbutton_x+BUTTON_SIZE*2, button_y, BUTTON_SIZE, BUTTON_SIZE);

    const Rbutton = env.imageCache[R_BUTTON_IND + env.R_pressed];
    env.ctx.drawImage(Rbutton, Zbutton_x+BUTTON_SIZE*3, button_y, BUTTON_SIZE, BUTTON_SIZE);

    const LButton = env.imageCache[L_BUTTON_IND + env.L_pressed];
    env.ctx.drawImage(LButton, Zbutton_x+BUTTON_SIZE*4, button_y, BUTTON_SIZE, BUTTON_SIZE);

    const SButton = env.imageCache[S_BUTTON_IND + env.S_pressed];
    env.ctx.drawImage(SButton, Zbutton_x+BUTTON_SIZE*5, button_y, BUTTON_SIZE, BUTTON_SIZE);

    if (Module._won(env.g)) {
        document.fonts.load("50px 'Pixelify Sans'").then(() => {
            env.ctx.font = "50px 'Pixelify Sans'";
            const text = "YOU WIN";
            const metrics = env.ctx.measureText(text);
            const textWidth = metrics.width;
            var x_text = (env.canvas.width - textWidth) / 2;
            var y_text = (env.canvas.height + (nb_cols + 2) * CELL_SIZE + CELL_SIZE *2) / 2;
            env.ctx.fillStyle = "#ff33e9";
            env.ctx.fillRect(x_text - 10, y_text - 50, textWidth + 20, 50 + 10);
            env.ctx.fillStyle = " #ffecfd  ";
            env.ctx.fillText(text,x_text,y_text);
        });
    }
    if(env.solving && !Module._won(env.g)) {
        env.solving = false;
        document.fonts.load("30px 'Pixelify Sans'").then(() => {
            env.ctx.font = "30px 'Pixelify Sans'";
            env.ctx.fillStyle = " #ffecfd  ";
            let text = "Solving the game...";
            let metrics = env.ctx.measureText(text);
            let textWidth = metrics.width;
            env.ctx.fillText(text, (env.canvas.width - textWidth) / 2, (env.canvas.height + (Module._nb_cols(env.g) + 2) * CELL_SIZE + CELL_SIZE) / 2);
        });
    }
    
    if(env.solution_not_found) {
        env.solution_not_found = false;
        let text = "Game with no apparent solution...";
        metrics = env.ctx.measureText(text);
        let textWidth = metrics.width;
        env.ctx.fillText(text, (env.canvas.width - textWidth) / 2, (env.canvas.height + (Module._nb_cols(env.g) + 2) * CELL_SIZE + CELL_SIZE) / 2);
    }

    };

    background.src = 'res/BACKGROUNDV2.png';
}

function preloadImages(env, onReady) {
    let imagesToLoad = env.IMG_SHAPES.length;
    let loaded = 0;

    for (let i = 0; i < env.IMG_SHAPES.length; i++) {
        const img = new Image();
        img.src = env.IMG_SHAPES[i];
        loaded++;
        img.onload = () => {
            if (loaded === imagesToLoad) {
                onReady();
            }
        };
        env.imageCache[i] = img; 
    }
    printGameGraphic(env);
}


function loadPieceImage(env, shapeIndex, x, y, angle) {
    const img = env.imageCache[shapeIndex];
    env.ctx.save();
    env.ctx.translate(x + CELL_SIZE / 2, y + CELL_SIZE / 2);
    env.ctx.rotate(angle);
    env.ctx.drawImage(img, -CELL_SIZE / 2, -CELL_SIZE / 2, CELL_SIZE, CELL_SIZE);
    env.ctx.restore();
}

function loadCornerWrappingImage(env, x, y,state) {
    var corner_img
    if (state) {
        corner_img = env.imageCache[WRAPPING_ON_IND];
    } else {
        corner_img = env.imageCache[WRAPPING_OFF_IND];
    }
    const corner = corner_img;
        env.ctx.drawImage(corner, x, y, CELL_SIZE, CELL_SIZE);
}

function loadBorderImage(env, x, y, d) {
    const border = env.imageCache[BORDER_IND];
    env.ctx.save();
    env.ctx.translate(x + CELL_SIZE / 2, y + CELL_SIZE / 2);
    env.ctx.rotate(d * 90 * Math.PI / 180);
    env.ctx.drawImage(border, -CELL_SIZE / 2, -CELL_SIZE / 2, CELL_SIZE, CELL_SIZE);
    env.ctx.restore();
}

function loadCornerImage(env, x, y, d) {
    const corner = env.imageCache[CORNER_IND];
        env.ctx.save();
        env.ctx.translate(x + CELL_SIZE / 2, y + CELL_SIZE / 2);
        env.ctx.rotate(d * 90 * Math.PI / 180);
        env.ctx.drawImage(corner, -CELL_SIZE / 2, -CELL_SIZE / 2, CELL_SIZE, CELL_SIZE);
        env.ctx.restore();
}



function loadBackgroundImage(env,x,y) {
    const background = env.imageCache[BACKGROUND_IND];
    env.ctx.drawImage(background, x, y, CELL_SIZE, CELL_SIZE);
};



function gridToInd(x,y, env) {
    var nb_rows = Module._nb_rows(env.g);
    var nb_cols = Module._nb_cols(env.g);

    for (let row = 0; row < nb_rows; row++) {
        for (let col = 0; col < nb_cols; col++) {
            var index = GAME2TAB(row, col, env.g);
            var cell = env.coord_tab[index];
            if (cell[0] <= x && x < cell[0]  + CELL_SIZE && cell[1] <= y && y < cell[1]  + CELL_SIZE ) {
                return { success: true, i: row, j: col };
            }
        }
    }
    return { success: false, i: null, j: null };
}

function c(row, col) {
    Module._play_move(env.g, row, col, 1);
    env.movingPieces[0] = row;
    env.movingPieces[1] = col;
    env.movingPieces[2] = true;
    env.movingPieces[3] = 0;
    env.movingPieces[4] = 1;

    const animation = setInterval(() => {
        if (env.movingPieces[3] >= 90) {
            clearInterval(animation);
            env.movingPieces[2] = false;
            printGameGraphic(env);
            return;
        }

        printGameGraphic(env);
        env.movingPieces[3] += 1;
    }, 1);
}



function a(row, col) {
    Module._play_move(env.g, row, col, -1);
    env.movingPieces[0] = row;
    env.movingPieces[1] = col;
    env.movingPieces[2] = true;
    env.movingPieces[3] = 0;
    env.movingPieces[4] = -1;

    const animation = setInterval(() => {

        if (env.movingPieces[3] <= -90) {
            clearInterval(animation);
            env.movingPieces[2] = false;
            printGameGraphic(env);
            return;
        }

        printGameGraphic(env);
        env.movingPieces[3] -= 1;
    }, 1);
}
function ctrl_z(){
    Module._undo(env.g);
    printGameGraphic(env);
}
function ctrl_y(){
    Module._redo(env.g);
    printGameGraphic(env);
}

function jeuAvecString(string) {
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

    return g;
}

function solveur(env) {
    if(env.timer != null) {
        clearTimeout(env.timer);
    }
    env.solving = true;
    printGameGraphic(env);
    const worker = new Worker('./worker.js');
    const jeu_serialise = Module._jeu_en_string(env.g);
    const json = UTF8ToString(jeu_serialise);

    const timeout = setTimeout(() => {
        worker.terminate();
        env.solution_not_found = true;
        printGameGraphic(env);
    }, 10000);
    env.timer = timeout
    worker.onmessage = (e) => {
        clearTimeout(env.timer);
        delete_game(env.g);
        env.g = jeuAvecString(e.data);
        printGameGraphic(env);
    };
    worker.postMessage(json);

}

function loadGame(env) {
    const input = document.createElement('input');
    input.type = 'file';
    input.style.display = 'none';
    input.addEventListener('change', (event) => {
        const file = event.target.files[0];
        if (!file) {
            input.remove();
            return;
        }
        const reader = new FileReader();
        reader.onload = (e) => {
            const content = e.target.result;
            const load = () => {
                FS.writeFile("/jeu_load.txt", content);
                const jeu = Module._load(allocateUTF8("/jeu_load.txt"));
                if (env.g != null) {
                    delete_game(env.g);
                }
                env.g = jeu;
                printGameGraphic(env);
                input.remove();
            };

            if (Module.calledRun) {
                load();
            } else {
                Module.onRuntimeInitialized = load;
            }
        };
        reader.readAsText(file);
    });

    document.body.appendChild(input);
    input.click();
}


function saveGame(env) {
    const filename = '/jeu_a_save.txt';
    const filenamePtr = allocateUTF8(filename);
    const userInput = prompt('Please enter the name:');

    if(Module.calledRun && (userInput != null)) {
        Module._save(env.g, filenamePtr);
        Module._free_alive(filenamePtr);
        const content = FS.readFile(filename, { encoding: 'utf8' });
        const blob = new Blob([content], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);    
        const a = document.createElement('a');
        a.href = url;
        a.download = userInput;
        a.click();
        URL.revokeObjectURL(url);
    } else if (userInput != null) {
        Module.onRuntimeInitialized = () => {
            Module._save(env.g, filenamePtr);
            Module._free_alive(filenamePtr);
            const content = FS.readFile(filename, { encoding: 'utf8' });
            const blob = new Blob([content], { type: 'text/plain' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = userInput;
            a.click();
            URL.revokeObjectURL(url);
        };
    }
}


function startGame() {
    env.coord_tab =  [];
    env.canvas = document.getElementById('mycanvas');
    let size = Math.min(window.innerWidth, window.innerHeight) * 0.8;
    env.canvas.width = size;
    env.canvas.height = size;
    env.PIECE_TAB = [];
    env.ctx = env.canvas.getContext('2d'),
    preloadImages(env, () => {
        printGameGraphic(env);
    });
}

function clickButton(button, env) {
    if (env.isAnimating) return;
    env.isAnimating = true;
    if (button == "Z") {
        env.Z_pressed = 1;
        printGameGraphic(env);
        setTimeout(() => {
            env.Z_pressed = 0;
            printGameGraphic(env);
            env.isAnimating = false;
        }, 300);
        ctrl_z();
    } else if (button == "Y") {
        env.Y_pressed = 1;
        printGameGraphic(env);
        setTimeout(() => {
            env.Y_pressed = 0;
            printGameGraphic(env);
            env.isAnimating = false;
        }, 300);
        ctrl_y();
    } else if (button == "K") {
        env.isAnimating = false;
        solveur(env);
    } else if (button == "R") {
        env.isAnimating = false;
        restart(env);
    } else if (button == "S") {
        env.isAnimating = false;
        saveGame(env);
    } else if (button == "L") {
        env.isAnimating = false;
        loadGame(env);
    }

}

function restart(env) {
    if(env.timer != null) {
        clearTimeout(env.timer);
    }
    const randInt = Math.floor(Math.random() * Number.MAX_SAFE_INTEGER);
    let screenWidth = window.innerWidth;
    var value;
    if (screenWidth <= 768) {
        value = 3;
    } else {
        value = 8;
    }
    cols = randInt % value + 2;
    rows = randInt % value + 2;
    wrapping = randInt % 2;
    delete_game(env.g);
    env.g = Module._new_random(rows, cols, wrapping, 0, 0);
    Module._restart(env.g);
    startGame();
}

function start() {
    env.g = Module._new_default();
    startGame();
    window.addEventListener('resize', function() {
        let size = Math.min(window.innerWidth, window.innerHeight) * 0.8;
        env.canvas.width = size;
        env.canvas.height = size;
        printGameGraphic(env); 
    });
    
    env.canvas.addEventListener('click', function(event) {
        const rect = env.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        let Zbutton_x =(env.canvas.width)/2-BUTTON_SIZE*3;

        let button_y=(env.canvas.height - Module._nb_cols(env.g) * CELL_SIZE) / 2 - BUTTON_SIZE*1.5;

        if (gridToInd(x, y, env).success) {
            const i = gridToInd(x, y, env).i;
            const j = gridToInd(x, y, env).j;
            c(i, j);
        }
        else if (x >= Zbutton_x && x <= Zbutton_x + BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("Z", env);
        }
        else if (x >= Zbutton_x + BUTTON_SIZE && x <= Zbutton_x + 2*BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("Y", env);
        }
        else if (x >= Zbutton_x + 2*BUTTON_SIZE && x <= Zbutton_x + 3*BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("K", env);
        }
        else if (x >= Zbutton_x + 3*BUTTON_SIZE && x <= Zbutton_x + 4*BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("R", env);
        }
        else if (x >= Zbutton_x + 4*BUTTON_SIZE && x <= Zbutton_x + 5*BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("L", env);
        }
        else if (x >= Zbutton_x + 5*BUTTON_SIZE && x <= Zbutton_x + 6*BUTTON_SIZE && y >= button_y && y <= button_y + BUTTON_SIZE) {
            clickButton("S", env);
        }
    });
    env.canvas.addEventListener('contextmenu', function(event) {
        event.preventDefault();
        const rect = env.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        let Zbutton_x =(env.canvas.width)/2-BUTTON_SIZE;
        let button_y=50;

        if (gridToInd(x, y, env).success) {
            const i = gridToInd(x, y, env).i;
            const j = gridToInd(x, y, env).j;
            a(i, j);
        }
    });
    document.addEventListener("keydown", function(event) {
        document.fonts.load("30px 'Pixelify Sans'").then(() => {
            env.ctx.font = "30px 'Pixelify Sans'";
            env.ctx.fillStyle = "white";
            if((event.key == "k" || event.key == "K") && !Module._won(env.g)) {
                solveur(env);
            }
            if(event.key == "m" || event.key == "M") {
                Module._restart(env.g);
                printGameGraphic(env);
            }
            if (event.key == "z" || event.key == "Z") {
                clickButton("Z", env);
            }
            if (event.key == "y" || event.key == "Y") {
                clickButton("Y", env);
            }
            if(event.key == "r" || event.key == "R") {
                restart(env)
            }
        })
    });    
}
function delete_game() {
    Module._delete(env.g);
}

