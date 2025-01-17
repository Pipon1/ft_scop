#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main(void)
{
    glfwInit(); //Fonction pour ini opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Définir la version mineur et majeur qu'on va use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Définir qu'on va utiliser que les fonctions de "base" et pas les fonctions de retrocompatibilité

    GLFWwindow  *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ft_scop", NULL, NULL); //On crée la fenêtre, on donne la taille et le nom

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); //Défini la window qu'on vient de créer en context principal de ce thread
                                    //Ce qui veut dire que tous les commandes qu'on va faire sont implicitement fait sur cette window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //On définit un callback pour pouvoir resize la window

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to ini GLAD" << std::endl;
        return -1;
    }

    //On crée le shader en précisent le type
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //On link la string du shader à l'objet qu'on vient de créer
    //1) l'objet, 2) nombre de string qu'on donne, 3) la string, 4) longueur, mais on s'en balec
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //On compile le shader et on vérifie si ça a marché
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    /*
        C'est pas giga dur les programmes aussi
        pour allez vite et simple :
        c'est comme ça que tu fusionnes des shaders ensemble et les prépares a être render
        ici on met le shader de couleur et de forme ensemble
        et on link le tout puis on vérifie.
        Et pour finir on delete les shaders vu que maintenant ils sont linké.
    */
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*  Pour comprendre les vertices : 

        pour un triangle :
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        }; 

        Chaque ligne représente un point dans le triangle.
        Chaque ligne est composée de 3 positions : x, y et z
        vu qu'on est en 2d, le z n'est pas utilisé et il est toujours set à 0.

        Pour avoir une idée plus précise :
        -0.5f, -0.5f, 0.0f : coin en bas à gauche
        0.5f, -0.5f, 0.0f : coin en bas à droite
        0.0f,  0.5f, 0.0f : coin en haut au milieu
    */
    
    float vertices[] = {
        0.5f, 0.5f, 0.0f, //en haut droit
        0.5f, -0.5f, 0.0f, // en bas droit
        -0.5f, -0.5f, 0.0f, // en bas gauche
        -0.5f, 0.5f, 0.0f //en haut gauche
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    /*
        VBO :
        VBO (vertex buffer objects) c'est un buffer qui sert a stocker une grosse quantité de vertex
        dans le but de pouvoir tous les envoyer d'un coup à la carte graphique.
        Basiquement c'est giga lent et cringe de balancer les vertex un par un à la carte graphique
        donc on les buffer dans un VBO et on envoie le VBO, c'est bcp plus rapide et efficace.

        VAO :
        VAO (vertex array objects) c'est un buffer qui sert a stocker les configs des vertex.
        l'idées c'est que tu fais des VBO et tu fais ta config qui dit à la carte graphique/opengl comment interprêter
        ces vertex qui sont dans le VBO. Une fois que ça c'est fait, tu stock les configs dans un VAO qui peut être bind/debind 
        et qui refait direct les bonnes configs une fois appelé. Ce qui te permet d'avoir un énorme VBO et des VAO qui appelle interprête correctement
        les vertex qui sont dedans.

        EBO :
        EBO (element buffer objects) c'est une buffer qui sert à stocker des "indices" 
        les indices c'est un moyen de dessiner des triangles sans overhead, c'est un peu zinzin
        mais en gros, si on fait dessiner un carré, faut 2 triangles et on doit préciser 2 fois les même vertex, ce qui est giga cringe
        mais avec des indices, on peut donner que 4 vertex et les indices qui indique la forme des triangles et pouf, ça fait un carré.
    */

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    //Les "glGenBuffers" ça sert juste a générer des objets buffer
    //2 arguments : 1) le nombre d'objet a générer, 2) le tableau où contenir les ID des objets générés
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    //Les "glBindBuffer" sont aussi vachement simple
    //c'est simplement le moyen de dire à opengl queltype de buffer ont vu que nos objets soit
    //deux arguments : 1) type de buffer, 2) tableau des ids pour define le type de buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Les "glBufferData" c'est vachement easy aussi
    //c'est comme ça qu'on copie les datas de notre array "vertices" dans l'objet VBO
    //y'a 4 arguments :
    //1) type de buffer, 2) taille du tableau d'origine, 3) le tableau d'origine, 4) type stockage dans la carte graphique
    //y'a 3 type :
    //GL_STREAM_DRAW : que quelques draw et le contenu est pas modifié
    //GL_STATIC_DRAW : une grosse quantitée de draw et le contenu est pas modifié
    //GL_DYNAMIC_DRAW : une grosse quantitée de draw et le contenu est souvent modifié
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    /*
        ici on define pour opengl comment les vertex qu'on lui a donné doivent être interprêté.
        1) la location (ici définit dans la shader à 0)
        2) le nombre d'argument par vertex, ici y'en a 3 (x, y, z)
        3) le type des arguments, en glsl, les vec* sont toujours des floats
        4) si on converti les floats en int (non)
        5) Le plus complex, ça c'est le stride c'est l'espace dans la mémoire entre 2 vertex
        ici, c'est 3 * sizeof(float) car y'a 3 arguments de type float entre chaque vertex
        6) C'est l'offset de la première valeur dans le tableau, obligatoirement cast en void*
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //On active les arrays de vertex qui pointe sur le shader location 0
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) //Loop de rendering, simple check de si on doit close la window ou pas
    {
        processInput(window); //fonction pour gérer les inputs

        glClearColor(0.2f, 0.3f, 0.7f, 1.0f); //couleur du background pour le clear de l'écran
        glClear(GL_COLOR_BUFFER_BIT); //clear de l'écran avec les couleurs choisis
        
        glUseProgram(shaderProgram); //Ici on dit quels shaders on doit utiliser
        glBindVertexArray(VAO); //Ici on dit quel VAO on doit utiliser
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //simple, on veut draw des triangles, avec 6 vertex au total

        glfwSwapBuffers(window); //Un peu complex, mais pour faire simple :
                                 //En gros, quand on draw sur l'écran, on draw pas une image d'un coup, mais pixel par pixel
                                 //ce qui peut mener a pas avoir le temps de finir de draw avant que la prochaine image doivent être chargée
                                 //pour éviter ça on fait du "double buffering". Ce qu'il y'a dans le buffer 1 est draw sur l'écran et tout ce qu'on va draw est mis dans le buffer 2
                                 //qui va construire l'image avant dans la draw. Quand on appelle cette function, le buffer 1 devient le buffer 2 et vice-versa.
                                 //Ca nous permet de preload les images pour éviter les soucis cité avant.

        glfwPollEvents(); //Fonction pour register les events et qui call les bonnes fonctions en callback.
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); //Ferme le programme et free la mémoire de opengl
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //Check quelle key est pressée
        glfwSetWindowShouldClose(window, true); //Dit à la fenêtre que elle doit close

    GLint debug = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        glGetIntegerv(GL_POLYGON_MODE, &debug);
        if (debug == GL_FILL)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height); //resize le viewport en fonction de la taille de la fenêtre
                                     //ATTENTION
                                     //Le viewport c'est pas la taille de la fenêtre
                                     //C'est la taille de la zone de dessin dans la fenêtre
                                     //Tu peux avoir multiple viewport dans une fenêtre
                                     //Mais là vu que y'en a que un, ça resize la window
}
