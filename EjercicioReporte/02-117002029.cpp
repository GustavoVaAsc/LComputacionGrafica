#include <stdio.h>
#include <string.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <glew.h>
#include <glfw3.h>
//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(0.65f,0.3f,0.8f,1.0f);	 			\n\
}";

void agregarCuadrado(std::vector<GLfloat>& vertices, float x, float y, float tam){
	float mitad = tam / 2.0f;
	vertices.insert(vertices.end(),{
		x - mitad, y - mitad, 0.0f,
		x + mitad, y - mitad, 0.0f,
		x + mitad, y + mitad, 0.0f,

		x - mitad, y - mitad, 0.0f,
		x + mitad, y + mitad, 0.0f,
		x - mitad, y + mitad, 0.0f
		});
}

int CrearLetras(){
	std::vector<GLfloat> vertices;

	const float tam = 0.05f;

	// Letra G
	// Rectangulo horizontal superior 
	agregarCuadrado(vertices, -0.8, 0.5, tam);
	agregarCuadrado(vertices, -0.75, 0.5, tam);
	agregarCuadrado(vertices, -0.70, 0.5, tam);
	agregarCuadrado(vertices, -0.65, 0.5, tam);
	agregarCuadrado(vertices, -0.60, 0.5, tam);

	// Rectangulo vertical izquierdo
	agregarCuadrado(vertices, -0.8, 0.45, tam);
	agregarCuadrado(vertices, -0.8, 0.40, tam);
	agregarCuadrado(vertices, -0.8, 0.35, tam);
	agregarCuadrado(vertices, -0.8, 0.30, tam);
	agregarCuadrado(vertices, -0.8, 0.25, tam);
	agregarCuadrado(vertices, -0.8, 0.20, tam);

	// Rectangulo horizontal superior

	agregarCuadrado(vertices, -0.60, 0.2, tam);
	agregarCuadrado(vertices, -0.65, 0.2, tam);
	agregarCuadrado(vertices, -0.70, 0.2, tam);
	agregarCuadrado(vertices, -0.75, 0.2, tam);
	agregarCuadrado(vertices, -0.80, 0.2, tam);

	// Rectangulo vertical derecho
	agregarCuadrado(vertices, -0.6, 0.35, tam);
	agregarCuadrado(vertices, -0.6, 0.30, tam);
	agregarCuadrado(vertices, -0.6, 0.25, tam);
	agregarCuadrado(vertices, -0.6, 0.20, tam);

	// Rectangulo horizontal medio

	agregarCuadrado(vertices, -0.60, 0.35, tam);
	agregarCuadrado(vertices, -0.65, 0.35, tam);
	agregarCuadrado(vertices, -0.70, 0.35, tam);


	// V

	// Rectangulo vertical superior izquierdo

	agregarCuadrado(vertices, -0.20, 0.5, tam);
	agregarCuadrado(vertices, -0.20, 0.45, tam);
	agregarCuadrado(vertices, -0.20, 0.40, tam);
	agregarCuadrado(vertices, -0.20, 0.35, tam);
	agregarCuadrado(vertices, -0.20, 0.30, tam);
	agregarCuadrado(vertices, -0.15, 0.30, tam);
	agregarCuadrado(vertices, -0.15, 0.25, tam);
	agregarCuadrado(vertices, -0.10, 0.20, tam);
	agregarCuadrado(vertices, -0.05, 0.20, tam);
	agregarCuadrado(vertices,  0.00, 0.25, tam);
	agregarCuadrado(vertices,  0.00, 0.30, tam);
	agregarCuadrado(vertices,  0.05, 0.30, tam);
	agregarCuadrado(vertices,  0.05, 0.35, tam);
	agregarCuadrado(vertices,  0.05, 0.40, tam);
	agregarCuadrado(vertices,  0.05, 0.45, tam);
	agregarCuadrado(vertices,  0.05, 0.50, tam);


	// A

	agregarCuadrado(vertices, 0.60, 0.50, tam);
	agregarCuadrado(vertices, 0.55, 0.45, tam);
	agregarCuadrado(vertices, 0.55, 0.40, tam);
	agregarCuadrado(vertices, 0.5, 0.35, tam);
	agregarCuadrado(vertices, 0.5, 0.30, tam);
	agregarCuadrado(vertices, 0.5, 0.25, tam);
	agregarCuadrado(vertices, 0.5, 0.20, tam);

	agregarCuadrado(vertices, 0.65, 0.50, tam);
	agregarCuadrado(vertices, 0.70, 0.45, tam);
	agregarCuadrado(vertices, 0.70, 0.40, tam);
	agregarCuadrado(vertices, 0.75, 0.35, tam);
	agregarCuadrado(vertices, 0.75, 0.30, tam);
	agregarCuadrado(vertices, 0.75, 0.25, tam);
	agregarCuadrado(vertices, 0.75, 0.20, tam);

	// Linea media
	
	agregarCuadrado(vertices, 0.55, 0.35, tam);
	agregarCuadrado(vertices, 0.60, 0.35, tam);
	agregarCuadrado(vertices, 0.65, 0.35, tam);
	agregarCuadrado(vertices, 0.70, 0.35, tam);
	agregarCuadrado(vertices, 0.75, 0.35, tam);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return (int)vertices.size() / 3;
}



void CrearTriangulo()
{
	GLfloat vertices[] = {
		-0.35f,0.0f,0.0f,
		-0.35f,0.35f,0.0f,
		 0.0f, 0.35f,0.0f,
		 -0.35f,0.0f,0.0f,
		 0.0f, 0.35f,0.0f,
		 0.0f,0.0f,0.0f,
		 0.5f,0.0f,0.0f,
		 0.8f,0.0f,0.0f,
		 0.65f,0.3f,0.0f,
		 0.5f,0.0f,0.0f,
		 0.8f,0.0f,0.0f,
		 0.65f,-0.3f,0.0f,
	};
	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es est�tico pues no se modificar�n los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a v�rtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Funci�n para agregar los shaders a la tarjeta gr�fica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el c�digo
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevenci�n de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el c�digo a la tarjeta gr�fica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gr�fica
	//verificaciones y prevenci�n de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}
int main()
{
	//Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fall� inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 L�NEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSI�N DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tama�o de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fall� inicializaci�n de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	int cantidadVertices = CrearLetras();
	CompileShaders();

	// Arreglo de 3 posiciones con colores aleatorios 
	std::vector<double> colores_aleatorios(3,0.0f);
	//Loop mientras no se cierra la ventana

	// Usamos dos variables time_t para calcular 2 segundos
	time_t t1, t2;
	time(&t1);

	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		// Generamos numeros aleatorios y los casteamos a double
		for (double& color : colores_aleatorios) {
			color = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		}

		// Medimos el tiempo
		time(&t2);

		// Si la diferencia de tiempo es mayor a dos segundos, cambiamos de color
		if (difftime(t2, t1) >= 2) {
			// Asignamos los colores aleatorios
			glClearColor(colores_aleatorios[0], colores_aleatorios[1], colores_aleatorios[2], 0.0f);
			// Incrementamos 2 segundos t1 para mantener la ventana de tiempo
			t1 += 2;
		}

		
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, cantidadVertices);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA L�NEA DESPU�S DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}