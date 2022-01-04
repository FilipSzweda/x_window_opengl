#include <iostream>

#define GL3_PROTOTYPES 1
#include <glew.h>
#include <SDL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define WIDTH 800
#define HEIGHT 600


GLuint vbo[4];		//identyfikatory buforow wierzcholkow
GLuint vao[2];		//identyfikatory tablic wierzcholkow
GLuint ebo;		//identyfikator bufora elementow

GLuint shaderProgram;
GLint vertexShader;	//identyfikator shadera wierzcholkow
GLint fragmentShader;   //identyfikator shadera fragmentow
GLint posAttrib, colAttrib;	//wskazniki atrybutow wierzcholkow

glm::mat4 viewMatrix = glm::mat4(1.0f);  //macierz widoku
glm::mat4 projectionMatrix = glm::mat4(1.0f); //marzerz rzutowania
GLfloat fi = 0;


//-------------Atrybuty wierzcholkow------------------------------------------

GLfloat ver_triangle[] = {	//wspolrzedne wierzcholkow trojkata
	 0.0f,  1.0f, 0.0,
	 1.0f,  0.0f, 0.0,
	-1.0f,  0.0f, 0.0
};

GLfloat col_triangle[] = {	//kolory wierzcholkow trojkata
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};

GLfloat ver_rectangle[] = {	//wspolrzedne wierzcholkow prostokata
	-1.0f, -0.2f, 0.0f,
	 1.0f, -0.2f, 0.0f,
	-1.0f, -0.7f, 0.0f,
	 1.0f, -0.7f, 0.0f
};

GLfloat col_rectangle[] = {	//kolory wierzcholkow prostokata
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f
};

GLuint elements[] = { //prostokat skladamy z dwoch trojkatow
	0, 1, 2,		  //indeksy wierzcholkow dla pierwszego trojkata
	1, 2, 3			  //indeksy wierzcholkow dla drugiego trojkata
};



//------------- kod zrodlowy shadera wierzcholkow ----------------------------------
const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"in vec3 color;"
"out vec3 Color;"
"uniform mat4 transformMatrix;"
"void main()"
"{"
"    gl_Position =  transformMatrix * vec4(position, 1.0);"
"    Color = color;"
"}";

//------------- kod zrodlowy shadera fragmentow --------------------------
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"out vec4 outColor;"
"void main()"
"{"
"    outColor = vec4(Color, 1.0);"
"}";


//----------------------------------tworzenie, wczytanie, kompilacja shaderow-------------------------

int init_shaders()
{
	// tworzenie i kompilacja shadera wierzcholkow
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "Kompilacja shadera wierzcholkow NIE powiodla sie!\n";
		return 0;
	}

	// tworzenie i kompilacja shadera fragmentow
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "Kompilacja shadera fragmentow NIE powiodla sie!\n";
		return 0;
	}

	// dolaczenie programow przetwarzajacych wierzcholki i fragmenty do programu cieniujacego
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// wskazniki atrybutow wierzcholkow
	posAttrib = glGetAttribLocation(shaderProgram, "position");  //pobranie indeksu tablicy atrybutow wierzcholkow okreslajacych polozenie
	glEnableVertexAttribArray(posAttrib);
	colAttrib = glGetAttribLocation(shaderProgram, "color");  //pobranie indeksu tablicy atrybutow wierzcholkow okreslajacych kolor
	glEnableVertexAttribArray(colAttrib);

	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------

void create_objects()
{
	// generowanie obiektow
	glGenVertexArrays(2, vao);  // obiekt tablicy wierzcholkow, dla kazdego obiektu mamy jedna tablice
	glGenBuffers(4, vbo);		// obiekty buforow wierzcholkow, dla kazdego typu atrubutow kazdego obiektu mamy jeden bufor (np. bufor dla kolorow prostok¹ta, bufor dla wspolrzednych prostok¹ta itd.)
	glGenBuffers(1, &ebo);		// obiekt bufora elementow

	//trojkat vao[0]

	glBindVertexArray(vao[0]);  //wybor tablicy

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	// //powiazanie bufora wspolrzednych wierzcholkow trojkata z odpowiednim obiektem (wybor bufora) 
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_triangle), ver_triangle, GL_STATIC_DRAW); //skopiowanie danych do pamieci aktywnego bufora
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0); //okreslenie organizacji danych w tablicy wierzcholkow
	glEnableVertexAttribArray(posAttrib);  //wlaczanie tablicy

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);	// bufor kolorow wierzcholkow trojkata
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_triangle), col_triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	//prostokat vao[1]

	glBindVertexArray(vao[1]);  //wybor tablicy

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);	// bufor wspolrzednych wierzcholkow prostokata
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_rectangle), ver_rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);	// bufor kolorow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_rectangle), col_rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); //bufor indeksow wierzcholkow (wykorzystany bedzie podczas rysowania prostokata)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

//------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_Event windowEvent;

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "ERROR" << std::endl;
		return 0;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // GL_ALWAYS)


	if (!init_shaders())
		return 0;

	create_objects();

	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //macierz widoku (okresla polozenie kamery i kierunek, w ktorym jest skierowana) 

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);		 //macierz rzutowania perspektywicznego
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 transformMatrix;

	GLint transformMatrixUniformLocation = glGetUniformLocation(shaderProgram, "transformMatrix"); //pobranie polozenia macierzy bedacej zmienna jednorodna shadera

	GLfloat angle = 0;

	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_KEYUP &&
				windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
			if (windowEvent.type == SDL_QUIT) break;

		}

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	// szare tlo
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));						// macierz przesuniecia o zadany wektor
		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));		// macierz obrotu o zadany kat wokol osi y
		transformMatrix = projectionMatrix * viewMatrix * translationMatrix * rotationMatrix;				// wynikowa macierz transformacji
		glUniformMatrix4fv(transformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));	// zapisanie macierzy wejœciowej dla shadera wierzcholkow

		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);	//rysujemy trojkat

		glBindVertexArray(vao[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//rysujemy prostokat

		angle += -0.05;
		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(4, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(2, vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}