#version 330 core // Versión 3.3 de GLSL en perfil Core

// Variable de salida para el color final de cada píxel (RGBA)
out vec4 FragColor;

// Lo que nos llega desde el Vertex Shader
in vec3 color;    // El color de los vértices (aunque en este script no lo estemos usando)
in vec2 texCoord; // Las coordenadas UV que nos pasa el Vertex Shader para saber cómo acomodar la imagen

// La textura real (el archivo png) que configuramos en el main de C++
uniform sampler2D tex0;

void main()
{
	// La función texture() cruza la imagen (tex0) con las coordenadas UV (texCoord) 
	// para sacar el color exacto que le toca a este píxel del dado
	FragColor = texture(tex0, texCoord);
}