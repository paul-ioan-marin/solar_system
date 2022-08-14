#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<float> sphere(float R);
GLuint textura(char fisier[]);

const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1500;

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath;
        std::cerr << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model planetar", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

    GLuint vertexShaderEarth = glCreateShader( GL_VERTEX_SHADER );
        if( 0 == vertexShaderEarth )
        {
            std::cout << "Error creating vertex shader." << std::endl;
            exit(1);
        }

    std::string shaderCode = readFile("earth.vert");
    const char *codeArray = shaderCode.c_str();
    glShaderSource( vertexShaderEarth, 1, &codeArray, NULL );

    glCompileShader(vertexShaderEarth);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShaderEarth, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShaderEarth, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

    unsigned int fragmentShaderEarth = glCreateShader(GL_FRAGMENT_SHADER);

    shaderCode = readFile("earth.frag");
    codeArray = shaderCode.c_str();
    glShaderSource( fragmentShaderEarth, 1, &codeArray, NULL );


    glCompileShader(fragmentShaderEarth);

        glGetShaderiv(fragmentShaderEarth, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShaderEarth, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderEarth);
    glAttachShader(shaderProgram, fragmentShaderEarth);
    glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    glDeleteShader(vertexShaderEarth);
    glDeleteShader(fragmentShaderEarth);

    std::vector<float> vertices;
    vertices = sphere(1);

    unsigned int VBO, VAO;
    GLuint texturi[4] = {textura("sun.jpg"),textura("earth.bmp"), textura("moon.jpg"), textura("jupiter.jpg")};
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int texLoc = glGetUniformLocation(shaderProgram, "ourTexture");

    float step = 1.0, angle = 0;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shaderProgram);

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        angle = angle + step;
        if (angle > 360)
            angle = angle - 360;

        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));
        view  = glm::rotate(view, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        view  = glm::rotate(view, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(-45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, texturi[0]);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLE_STRIP, 0 , vertices.size()/5);

        ////////////////

        model = glm::mat4(1.0f);

        model = glm::rotate(model, 2*glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.1f, 0.9f, 0.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));


        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, texturi[1]);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_STRIP, 0 , vertices.size()/5);

        ////////////////

        model = glm::mat4(1.0f);

        model = glm::rotate(model, 2*glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(-0.9f, 0.1f, 0.0f));
        model = glm::rotate(model, 2*glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f,0.0f,1.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, texturi[2]);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_STRIP, 0 , vertices.size()/5);

        /////////////////

        model = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(5.0f), glm::vec3(0.1f, 0.9f, 0.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));


        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, texturi[3]);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_STRIP, 0 , vertices.size()/5);

        glfwSwapBuffers(window);
        Sleep(10);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1,&texturi[0]);
    glDeleteTextures(1,&texturi[1]);
    glDeleteTextures(1,&texturi[2]);
    glDeleteTextures(1,&texturi[3]);



    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

std::vector <float> sphere(float R) {
    int nMeridiane = 200;
    int nParalele = nMeridiane;
    std::vector <float> vertex = {};
    double theta0, theta1, phi;
    int i,j;
    for(i = 1; i < nParalele ; i++) {
        theta0 = M_PI * ( (double) (i - 1) / nParalele);
        theta1 = M_PI * ( (double) i / nParalele);
        for(j = 0; j < nMeridiane; j++) {
            phi = 2 * M_PI * (double) (j - 1) / nMeridiane;
            vertex.push_back(R * cos(phi) * sin(theta0));
            vertex.push_back(R * sin(phi) * sin(theta0));
            vertex.push_back(R * cos(theta0));
            vertex.push_back( (double) (i - 1) / nParalele);
            vertex.push_back( (double) j / nMeridiane);
            vertex.push_back(R * cos(phi) * sin(theta1));
            vertex.push_back(R * sin(phi) * sin(theta1));
            vertex.push_back(R * cos(theta1));
            vertex.push_back( (double) i / nParalele);
            vertex.push_back( (double) j / nMeridiane);
            }
    }
    return vertex;
}

GLuint textura(char fisier[]) {
    GLuint rezTextura;
    int width, height, nrChannels;

    glGenTextures(1, &rezTextura);

    glBindTexture(GL_TEXTURE_2D, rezTextura);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(fisier, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    return rezTextura;

}
