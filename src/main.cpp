#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    while (!glfwWindowShouldClose(window)) //Loop de rendering, simple check de si on doit close la window ou pas
    {
        processInput(window); //fonction pour gérer les inputs

        glClearColor(0.2f, 0.3f, 0.7f, 1.0f); //couleur du background pour le clear de l'écran
        glClear(GL_COLOR_BUFFER_BIT); //clear de l'écran avec les couleurs choisis

        glfwSwapBuffers(window); //Un peu complex, mais pour faire simple :
                                 //En gros, quand on draw sur l'écran, on draw pas une image d'un coup, mais pixel par pixel
                                 //ce qui peut mener a pas avoir le temps de finir de draw avant que la prochaine image doivent être chargée
                                 //pour éviter ça on fait du "double buffering". Ce qu'il y'a dans le buffer 1 est draw sur l'écran et tout ce qu'on va draw est mis dans le buffer 2
                                 //qui va construire l'image avant dans la draw. Quand on appelle cette function, le buffer 1 devient le buffer 2 et vice-versa.
                                 //Ca nous permet de preload les images pour éviter les soucis cité avant.

        glfwPollEvents(); //Fonction pour register les events et qui call les bonnes fonctions en callback.
    }

    glfwTerminate(); //Ferme le programme et free la mémoire de opengl
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //Check quelle key est pressée
        glfwSetWindowShouldClose(window, true); //Dit à la fenêtre que elle doit close
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