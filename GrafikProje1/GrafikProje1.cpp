#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>   
#include <sstream>     
#include <string>     
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Ekran boyutları
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Hata kontrolü
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compile error:\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Bağlantı kontrolü
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader link error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void drawCube(GLuint shaderProgram, GLuint VAO, glm::mat4 model, glm::vec3 color) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), color.r, color.g, color.b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawTree(GLuint shaderProgram, GLuint VAO, glm::vec3 position) {
    // Gövde
    glm::mat4 trunkModel = glm::mat4(1.0f);
    trunkModel = glm::translate(trunkModel, position);
    trunkModel = glm::scale(trunkModel, glm::vec3(0.5f, 1.5f, 0.5f)); 
    drawCube(shaderProgram, VAO, trunkModel, glm::vec3(0.55f, 0.27f, 0.07f)); 

    glm::mat4 leafModel = glm::mat4(1.0f);
    leafModel = glm::translate(leafModel, position + glm::vec3(0.0f, 1.2f, 0.0f)); 
    leafModel = glm::scale(leafModel, glm::vec3(1.5f, 1.2f, 1.5f)); 
    drawCube(shaderProgram, VAO, leafModel, glm::vec3(0.0f, 0.6f, 0.0f)); 
}


void drawHammock(GLuint shaderProgram, GLuint VAO, glm::vec3 position, float length) {

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
    model = glm::scale(model, glm::vec3(length, 0.1f, 0.5f));

    drawCube(shaderProgram, VAO, model, glm::vec3(0.2f, 0.2f, 1.0f));  
}

void drawPerson(GLuint shaderProgram, GLuint VAO, glm::vec3 pos) {
    // Gövde
    glm::mat4 body = glm::mat4(1.0f);
    body = glm::translate(body, pos + glm::vec3(0.0f, 0.5f, 0.0f));
    body = glm::scale(body, glm::vec3(0.4f, 1.0f, 0.2f));
    drawCube(shaderProgram, VAO, body, glm::vec3(0.2f, 0.2f, 0.8f));

    // Kafa
    glm::mat4 head = glm::mat4(1.0f);
    head = glm::translate(head, pos + glm::vec3(0.0f, 1.3f, 0.0f));
    head = glm::scale(head, glm::vec3(0.4f, 0.4f, 0.4f));
    drawCube(shaderProgram, VAO, head, glm::vec3(1.0f, 0.8f, 0.6f));

    // Sol kol (çapraz, sola açılı)
    glm::mat4 armL = glm::mat4(1.0f);
    armL = glm::translate(armL, pos + glm::vec3(-0.25f, 1.0f, 0.0f)); // omuzdan başla
    armL = glm::rotate(armL, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // saat yönünün tersine döndür
    armL = glm::scale(armL, glm::vec3(0.15f, 0.6f, 0.15f));
    drawCube(shaderProgram, VAO, armL, glm::vec3(0.2f, 0.2f, 0.8f));

    // Sağ kol (çapraz, sağa açılı)
    glm::mat4 armR = glm::mat4(1.0f);
    armR = glm::translate(armR, pos + glm::vec3(0.25f, 1.0f, 0.0f)); // omuzdan başla
    armR = glm::rotate(armR, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // saat yönünde döndür
    armR = glm::scale(armR, glm::vec3(0.15f, 0.6f, 0.15f));
    drawCube(shaderProgram, VAO, armR, glm::vec3(0.2f, 0.2f, 0.8f));
}




float cubeVertices[] = {
    // positions           // normals
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  0.0f
};



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Pencere oluştur
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Benim Dunyan", NULL, NULL);
    if (window == NULL) {
        std::cout << "Pencere olusturulamadi!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD yükle
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD yuklenemedi!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);


    GLuint shaderProgram = createShaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    glUseProgram(shaderProgram);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // pozisyon (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Ana döngü
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 1.5f, 0.5f));

        glm::mat4 view = glm::lookAt(
            glm::vec3(0, 2.0f, 8.0f),  
            glm::vec3(0, 1.0f, 0),    
            glm::vec3(0, 1, 0)
        );

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Uniformları shader'a gönder
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Işık ve renk
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.5f, 2.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 1.5f, 4.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.55f, 0.27f, 0.07f); 

        glBindVertexArray(VAO);

        drawTree(shaderProgram, VAO, glm::vec3(-2.0f, 0.0f, 0.0f));
        drawTree(shaderProgram, VAO, glm::vec3(2.0f, 0.0f, 0.0f)); 
        glm::vec3 hammockPos = glm::vec3(0.0f, 0.1f, 0.0f);  
        drawHammock(shaderProgram, VAO, hammockPos, 3.5f);  
        drawPerson(shaderProgram, VAO, glm::vec3(0.0f, 0.0f, 5.5f));



        // Frame göster
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // GLFW temizle
    glfwTerminate();
    return 0;
}
