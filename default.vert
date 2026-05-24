#version 330 core // Versión 3.3 de GLSL en perfil Core

// Atributos que entran desde el VBO en C++ (El Layout que configuramos)
layout (location = 0) in vec3 aPos;   // Atributo 0: Posición del vértice (X, Y, Z)
layout (location = 1) in vec3 aColor; // Atributo 1: Color del vértice (R, G, B)
layout (location = 2) in vec2 aTex;   // Atributo 2: Coordenadas UV para la textura (U, V)

// Datos que le vamos a mandar al Fragment Shader para que pinte
out vec3 color;    // Pasamos el color
out vec2 texCoord; // Pasamos las UVs

// Traemos las matrices matemáticas desde el main en C++
uniform mat4 camMatrix; // Matriz de la cámara (Vista + Proyección)
uniform mat4 model;     // Matriz del objeto (Maneja la traslación en la curva y la rotación sobre su eje)

void main()
{
	// Calculamos la posición final en el mundo 3D.
	// Ojo al orden de multiplicación de matrices: primero Cámara, luego Modelo y al final el Vértice (convertido a vec4).
	gl_Position = camMatrix * model * vec4(aPos, 1.0);
	
	// Pasamos los datos limpios al Fragment Shader sin hacerles cambios
	color = aColor;
	texCoord = aTex;
}