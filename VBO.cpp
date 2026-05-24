#include"VBO.h"

// Constructor para crear el Vertex Buffer Object y subir los vértices de una
VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
	// Generamos un ID único para el buffer en la GPU
	glGenBuffers(1, &ID);
	// Lo enlazamos como un buffer de arreglos/vértices (ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	// Le mandamos los datos planos de los vértices a la GPU (posiciones, colores, UVs). 
	// Usamos STATIC_DRAW porque la geometría base no va a estar cambiando en tiempo real
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

// Para activar/vincular este VBO cuando toque usarlo o enlazar sus atributos
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Para desvincular el VBO y dejar todo limpio para otras configuraciones
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Para borrar el buffer de la GPU y liberar memoria limpia antes de cerrar el programa
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}