#version 330 core // Versión 3.3 de GLSL en perfil Core

// Atributo de entrada 0: para una línea simple solo nos importa su posición (X, Y, Z)
layout (location = 0) in vec3 aPos;

// Traemos la matriz de la cámara (Vista + Proyección) que calculamos en C++
uniform mat4 camMatrix;

void main()
{
	// Calculamos la posición final en la pantalla.
	// Convertimos el vec3 de la posición a vec4 poniéndole un 1.0 en la W para las matemáticas 3D.
	// Ojo: solo multiplicamos por camMatrix porque la línea se queda quieta en su lugar en el mapa (no usa matriz 'model').
	gl_Position = camMatrix * vec4(aPos, 1.0f);
}