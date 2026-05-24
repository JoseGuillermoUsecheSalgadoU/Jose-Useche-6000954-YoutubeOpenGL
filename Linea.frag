#version 330 core // Le digo a OpenGL que use la versión 3.3 de GLSL en su perfil Core

out vec4 FragColor; // Esta es la variable de salida que va a escupir el color final del píxel (RGBA)

void main()
{
	// Acá defino el color para pintar la línea guía. 
	// Ahora mismo está en blanco puro (R=1.0, G=1.0, B=1.0, Alpha=1.0)
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}