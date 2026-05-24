//------- Ignorar esto (Rutas de C++17) ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------------------------

#include<iostream>
#include<vector>               // Para guardar los puntos de la curva de Bézier
#include<glad/glad.h>          // Para cargar las funciones modernas de OpenGL
#include<GLFW/glfw3.h>         // Para la ventana y los inputs
#include<stb/stb_image.h>      // Para cargar la imagen de la textura
#include<glm/glm.hpp>          // Matemáticas de vectores y matrices
#include<glm/gtc/matrix_transform.hpp> // Para las transformaciones (Translate, Rotate, Scale)
#include<glm/gtc/type_ptr.hpp> // Para pasar las matrices de GLM a los shaders

// Mis clases del motor/tutorial
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"

// Tamaño de la ventana
const unsigned int width = 800;
const unsigned int height = 800;

// El número áureo para que el icosaedro (D20) quede con las proporciones perfectas
float phi = 1.61803398875f;

// Los 12 vértices base del dado
GLfloat vertices[] =
{ // Estructura: X, Y, Z (Posición) | R, G, B (Color) | U, V (Coordenadas de textura)
	-1.0f,  phi, 0.0f,  0.8f, 0.7f, 0.4f, 0.0f, 0.0f,
	1.0f,  phi, 0.0f,  0.8f, 0.7f, 0.4f, 1.0f, 0.0f,
	-1.0f, -phi, 0.0f,  0.8f, 0.7f, 0.4f, 0.0f, 1.0f,
	1.0f, -phi, 0.0f,  0.8f, 0.7f, 0.4f, 1.0f, 1.0f,

	0.0f, -1.0f,  phi,  0.8f, 0.7f, 0.4f, 0.5f, 0.0f,
	0.0f,  1.0f,  phi,  0.8f, 0.7f, 0.4f, 0.5f, 1.0f,
	0.0f, -1.0f, -phi,  0.8f, 0.7f, 0.4f, 0.5f, 0.0f,
	0.0f,  1.0f, -phi,  0.8f, 0.7f, 0.4f, 0.5f, 1.0f,

	phi, 0.0f, -1.0f,  0.8f, 0.7f, 0.4f, 0.0f, 0.5f,
	phi, 0.0f,  1.0f,  0.8f, 0.7f, 0.4f, 1.0f, 0.5f,
	-phi, 0.0f, -1.0f,  0.8f, 0.7f, 0.4f, 0.0f, 0.5f,
	-phi, 0.0f,  1.0f,  0.8f, 0.7f, 0.4f, 1.0f, 0.5f
};

// El orden para conectar los vértices y armar los 20 triángulos del dado
GLuint indices[] =
{
	0, 11, 5,   0, 5, 1,   0, 1, 7,   0, 7, 10,  0, 10, 11,
	1, 5, 9,    5, 11, 4,  11, 10, 2,  10, 7, 6,   7, 1, 8,
	3, 9, 4,    3, 4, 2,   3, 2, 6,   3, 6, 8,    3, 8, 9,
	4, 9, 5,    2, 4, 11,  6, 2, 10,  8, 6, 7,    9, 8, 1
};

// Función para calcular los puntos de la curva de Bézier cúbica usando el parámetro t (de 0 a 1)
glm::vec3 bezier(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	float u = 1.0f - t;
	return (u * u * u * p0) + (3.0f * u * u * t * p1) + (3.0f * u * t * t * p2) + (t * t * t * p3);
}

int main()
{
	// Inicializar GLFW y configurar OpenGL 3.3 en perfil Core
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear la ventana del juego/App
	GLFWwindow* window = glfwCreateWindow(width, height, "Icosaedro con Bézier", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Cargar GLAD para mapear las funciones de la GPU
	gladLoadGL();
	glViewport(0, 0, width, height);

	// Shaders para el dado texturizado
	Shader shaderProgram("default.vert", "default.frag");

	// --- Configuración de los buffers del dado ---
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices)); // Sube los vértices
	EBO EBO1(indices, sizeof(indices));   // Sube el orden de los triángulos

	// Configurar los Layouts del Vertex Shader:
	// Atributo 0: Posición (3 floats: X, Y, Z)
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	// Atributo 1: Color (3 floats: R, G, B)
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Atributo 2: UVs (2 floats: U, V)
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Desvincular todo para no romper nada por accidente
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Cargar la textura del dado en la ranura TEXTURE0
	Texture brickTex("pngegg.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	// Activar la prueba de profundidad (así las caras de atrás no se ven adelante)
	glEnable(GL_DEPTH_TEST);

	// Crear la cámara (un poco alejada para ver toda la escena)
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

	// Variables para controlar la rotación por tiempo (independiente de los FPS)
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	// Los 4 puntos de control para darle forma a la curva de Bézier
	glm::vec3 p0 = glm::vec3(-2.0f, 0.0f, 0.0f);
	glm::vec3 p1 = glm::vec3(-1.0f, 2.0f, 0.0f);
	glm::vec3 p2 = glm::vec3(1.0f, -2.0f, 0.0f);
	glm::vec3 p3 = glm::vec3(2.0f, 0.0f, 0.0f);

	// Calcular 50 puntos intermedios para poder dibujar la línea guía
	const int numPuntos = 50;
	std::vector<glm::vec3> lineaPuntos;

	for (int i = 0; i <= numPuntos; i++) {
		float t = (float)i / (float)numPuntos;
		lineaPuntos.push_back(bezier(t, p0, p1, p2, p3));
	}

	// --- Configuración de buffers para la línea guía ---
	VAO VAOLinea;
	VAOLinea.Bind();
	// Pasamos los datos del vector a float plano con reinterpret_cast para que el VBO los entienda
	VBO VBOLinea(reinterpret_cast<GLfloat*>(lineaPuntos.data()), lineaPuntos.size() * sizeof(glm::vec3));
	// Atributo 0: Solo posiciones (X, Y, Z)
	VAOLinea.LinkAttrib(VBOLinea, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAOLinea.Unbind();
	VBOLinea.Unbind();

	// Shaders exclusivos para la línea (sin texturas, solo color plano)
	Shader shaderLinea("Linea.vert", "Linea.frag");

	// --- Bucle principal de renderizado ---
	while (!glfwWindowShouldClose(window))
	{
		// Limpiar pantalla (Fondo gris oscuro) y el buffer de profundidad
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Controlar el tiempo para que la rotación vaya a 60 FPS estables en cualquier PC
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1.0 / 60.0) {
			rotation += 0.5f;
			prevTime = crntTime;
		}

		// 't' oscila entre 0.0 y 1.0 usando la función seno para que el dado vaya y vuelva por la curva
		float t = (sinf((float)glfwGetTime()) + 1.0f) / 2.0f;
		// Sacar la posición 3D actual del dado en la curva
		glm::vec3 posicionBezier = bezier(t, p0, p1, p2, p3);

		// Actualizar la cámara con el teclado/mouse
		camera.Inputs(window);

		// --------------------------------------------------------
		// 1. Renderizar la línea de la trayectoria
		// --------------------------------------------------------
		shaderLinea.Activate();
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderLinea, "camMatrix");

		VAOLinea.Bind();
		glLineWidth(2.5f); // Grosor de la línea
		glDrawArrays(GL_LINE_STRIP, 0, numPuntos + 1); // Dibujar los puntos conectados
		VAOLinea.Unbind();

		// --------------------------------------------------------
		// 2. Renderizar el dado moviéndose
		// --------------------------------------------------------
		shaderProgram.Activate();
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Armar la matriz de modelo para aplicar las transformaciones al dado
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, posicionBezier); // Moverlo a la posición actual de la curva
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotarlo en el eje Y

		// Enviar la matriz de modelo al Vertex Shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Activar textura, enlazar el VAO del dado y dibujar los triángulos indexados
		brickTex.Bind();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		VAO1.Unbind();

		// Intercambiar buffers y procesar eventos de la ventana
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// --- Limpieza total de la GPU al cerrar el programa ---
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	VAOLinea.Delete();
	VBOLinea.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
	shaderLinea.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}