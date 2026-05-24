#include"Camera.h"

// Constructor para guardar el tamaño de la ventana y dónde arranca la cámara
Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

// Función para calcular y mandar la matriz de la cámara (Vista + Proyección) al shader
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	// Inicializamos las matrices como identidad para que no arranquen en nulo
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Matriz de Vista: Define desde dónde mira la cámara, hacia dónde apunta y cuál es el "arriba"
	view = glm::lookAt(Position, Position + Orientation, Up);

	// Matriz de Proyección: Le da la perspectiva 3D a la escena (hace que lo lejano se vea más pequeño)
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Multiplicamos Proyección * Vista y se la mandamos de una al uniform del Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

// Manejo de teclado y mouse para mover la cámara
void Camera::Inputs(GLFWwindow* window)
{
	// --- CONTROLES DE TECLADO (Movimiento WASD + Espacio/Ctrl) ---

	// W: Avanzar hacia adelante usando la orientación actual
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	// A: Moverse a la izquierda (saca el producto cruz entre la orientación y el eje 'Up' para saber cuál es el lado)
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	// S: Retroceder
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	// D: Moverse a la derecha (producto cruz positivo)
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	// Espacio: Subir en el eje Y global
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	// Control Izquierdo: Bajar en el eje Y global
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}

	// Shift Izquierdo: Sprint / Multiplicador de velocidad si se deja hundido
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.04f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.01f;
	}


	// --- CONTROLES DEL MOUSE (Mirar alrededor al hacer Clic Izquierdo) ---
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Escondemos el cursor para que no estorbe mientras arrastramos la vista
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Evitamos que la cámara meta un brinco feo la primera vez que hacemos clic
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Variables para guardar la posición del cursor en la pantalla
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Sacamos la distancia del mouse al centro de la ventana y la escalamos con la sensibilidad
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculamos cómo cambiaría la orientación vertical (arriba/abajo) rotando sobre el eje lateral
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Bloqueo de seguridad: Evita que la cámara dé la vuelta completa de cabeza (límite de 85 grados)
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotamos la orientación hacia los lados (izquierda/derecha) usando el eje 'Up' global
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Forzamos al mouse a volver al centro para poder seguir midiendo el movimiento de forma infinita
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	// Cuando soltamos el clic izquierdo
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Mostramos el cursor otra vez
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Reseteamos el flag para que el próximo clic no dé saltos locos
		firstClick = true;
	}
}