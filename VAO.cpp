#include"VAO.h"

// Constructor para crear el VAO y generar su ID único en la GPU
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Función clave para enlazar un atributo del VBO (como posición, color o UVs) con el VAO
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind(); // Primero activo el VBO con el que voy a trabajar

	// Le digo a OpenGL cómo debe interpretar el bloque de memoria de los vértices:
	// (qué layout usa, cuántos datos son, el tipo de dato, si va normalizado, el tamaño total del bloque y dónde empieza)
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);

	// Activo el atributo usando su número de layout para que el shader lo pueda leer
	glEnableVertexAttribArray(layout);

	VBO.Unbind(); // Desvinculo el VBO para dejar todo limpio
}

// Para activar/vincular este VAO cuando toque dibujar la malla en el bucle principal
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Para desvincular el VAO y no alterar su configuración sin querer en otra parte del código
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Para borrar el VAO de la GPU y liberar memoria limpia antes de cerrar la aplicación
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}