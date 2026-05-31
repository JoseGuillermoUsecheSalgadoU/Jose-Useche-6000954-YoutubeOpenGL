//------- Ignorar esto (Rutas de C++17) ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------------------------

#include<iostream>
#include<vector>               // Para guardar los puntos de la trayectoria en forma de 8
#include<cmath>                // Necesario para que funcionen las matemáticas de sin() y cos()
#include<glad/glad.h>          // Para cargar las funciones modernas de OpenGL
#include<GLFW/glfw3.h>         // Para la ventana y los inputs
#include<stb/stb_image.h>      // Para cargar la imagen de la textura
#include<glm/glm.hpp>          // Matemáticas de vectores y matrices
#include<glm/gtc/matrix_transform.hpp> // Para las transformaciones (Translate, Rotate, Scale)
#include<glm/gtc/type_ptr.hpp> // Para pasar las matrices de GLM a los shaders

// Mis clases del motor
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"

// Tamaño de la ventana del programa
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

// Función para calcular la trayectoria en forma de 8 (Lemniscata de Bernoulli)
// Recibe el parámetro t (de 0 a 1) y devuelve la posición 3D correspondiente
glm::vec3 obtenerPosicionInfinito(float t) {
	float angulo = t * 2.0f * 3.14159f; // Mapeamos t (0 a 1) a un ángulo en radianes (0 a 2*PI)
	float a = 2.5f;                     // Define el tamaño o escala general del "8"
	float denominador = 1.0f + sin(angulo) * sin(angulo); // Ecuación base de la Lemniscata

	// Calculamos las coordenadas paramétricas de la curva
	float x = (a * cos(angulo)) / denominador;
	float y = (a * sin(angulo) * cos(angulo)) / denominador;

	return glm::vec3(x, y, 0.0f); // Retornamos la posición en el plano XY (Z en 0)
}

// Función para actualizar el viewport si el usuario llega a cambiar el tamaño de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main()
{
	// Inicializar GLFW y configurar OpenGL 3.3 en perfil Core
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear la ventana principal de la aplicación
	GLFWwindow* window = glfwCreateWindow(width, height, "Icosaedro en Trayectoria de Infinito", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Cargar GLAD para poder usar las funciones modernas de la GPU
	gladLoadGL();
	glViewport(0, 0, width, height);

	// Registrar el callback para redimensionar la ventana de forma dinámica
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Cargar los Shaders para pintar el dado texturizado
	Shader shaderProgram("default.vert", "default.frag");

	// --- Configuración de los buffers (VAO, VBO, EBO) para el dado ---
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices)); // Sube los datos de los vértices a la GPU
	EBO EBO1(indices, sizeof(indices));   // Sube el orden de dibujo de las caras

	// Configurar los Layouts correspondientes al Vertex Shader del dado:
	// Atributo 0: Posición en el espacio (X, Y, Z)
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	// Atributo 1: Color base del vértice (R, G, B)
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Atributo 2: Coordenadas de texturizado (U, V)
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Desvincular para evitar modificaciones accidentales en los buffers
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Cargar y configurar la textura del dado en la unidad TEXTURE0
	Texture brickTex("pngegg.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	// Habilitar el buffer de profundidad para que las caras traseras no se superpongan
	glEnable(GL_DEPTH_TEST);

	// Crear el objeto cámara y ubicarla un poco retrasada en el eje Z para visualizar todo
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

	// Variables de control de tiempo y rotación del icosaedro
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	// --- Pre-cálculo de la línea guía para la trayectoria del 8 ---
	const int numPuntos = 100; // Usamos 100 puntos para que la curva del infinito se vea súper fluida
	std::vector<glm::vec3> lineaPuntos;

	// Generamos los puntos del 8 evaluando la función a lo largo de un ciclo completo (de 0 a 1)
	for (int i = 0; i <= numPuntos; i++) {
		float t = (float)i / (float)numPuntos;
		lineaPuntos.push_back(obtenerPosicionInfinito(t));
	}

	// --- Configuración de buffers exclusivos para renderizar la línea de guía ---
	VAO VAOLinea;
	VAOLinea.Bind();
	// Pasamos los datos dinámicos del vector a memoria plana de la GPU usando reinterpret_cast
	VBO VBOLinea(reinterpret_cast<GLfloat*>(lineaPuntos.data()), lineaPuntos.size() * sizeof(glm::vec3));
	// Atributo 0: Solo nos importan las posiciones (X, Y, Z) de la línea
	VAOLinea.LinkAttrib(VBOLinea, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAOLinea.Unbind();
	VBOLinea.Unbind();

	// Cargar Shaders específicos de la línea guía (dibujo simple y plano)
	Shader shaderLinea("Linea.vert", "Linea.frag");

	// --- Bucle principal de la aplicación (Game Loop) ---
	while (!glfwWindowShouldClose(window))
	{
		// Limpiar la pantalla con un fondo gris oscuro y resetear el buffer de profundidad
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Obtener las dimensiones actuales de la ventana para recalcular el aspecto de la cámara
		int currentWidth, currentHeight;
		glfwGetWindowSize(window, &currentWidth, &currentHeight);
		camera.width = currentWidth;
		camera.height = currentHeight;

		// Control de rotación basado en tiempo real para asegurar 60 FPS estables en cualquier PC
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1.0 / 60.0) {
			rotation += 0.5f;
			prevTime = crntTime;
		}

		// 't' oscila entre 0.0 y 1.0 gracias al seno matemático. Esto hace que el dado vaya y regrese continuamente.
		// El "* 0.5f" ralentiza un poco el recorrido para que se aprecie mejor el movimiento en 8.
		float t = (sinf((float)glfwGetTime() * 0.5f) + 1.0f) / 2.0f;

		// Obtener las coordenadas dinámicas del objeto en la curva para este frame
		glm::vec3 posActual = obtenerPosicionInfinito(t);

		// Escuchar eventos de entrada de la cámara (teclado/ratón)
		camera.Inputs(window);

		// -------------------------------------------------------------------------
		// PASO 1: Renderizar la línea de guía de la trayectoria
		// -------------------------------------------------------------------------
		shaderLinea.Activate();
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderLinea, "camMatrix");

		VAOLinea.Bind();
		glLineWidth(2.5f); // Grosor en píxeles de la línea guía
		glDrawArrays(GL_LINE_STRIP, 0, numPuntos + 1); // Dibujar los puntos en cadena continua
		VAOLinea.Unbind();

		// -------------------------------------------------------------------------
		// PASO 2: Renderizar el icosaedro (Dado D20) moviéndose por la escena
		// -------------------------------------------------------------------------
		shaderProgram.Activate();
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Construcción de la matriz de modelo para aplicar transformaciones espaciales
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, posActual); // Trasladar el dado a la posición actual del infinito
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotarlo continuamente en el eje Y

		// Enviar la matriz de transformación calculada al Vertex Shader correspondiente
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Activar textura enlazada, enlazar el VAO principal y mandar a pintar las caras del dado indexado
		brickTex.Bind();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		VAO1.Unbind();

		// Intercambiar buffers de renderizado y procesar eventos de ventana
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// --- Sección de limpieza: Liberar memoria de la GPU al cerrar el programa ---
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	VAOLinea.Delete();
	VBOLinea.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
	shaderLinea.Delete();

	// Destruir instancias de GLFW y dar por finalizado el programa de forma limpia
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}