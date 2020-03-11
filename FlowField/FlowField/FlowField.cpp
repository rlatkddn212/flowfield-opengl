#pragma warning(disable:4996)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")

#include <GL/glew.h>	
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <time.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

GLFWwindow* window;
double cursorX;
double cursorY;

GLuint gRectArrayID;
GLuint gLineArrayID;
GLuint gArrowArrayID;
GLuint gCircleArrayID;
GLuint gLoc;

int width = 1024;
int height = 768;
const int cntX = 10;
const int cntY = 10;
int tiles[cntY][cntX];
glm::vec2 direct[cntY][cntX];
double dist[cntY][cntX];
const double INF = 987654321;

double gapX = (double)width / cntX;
double gapY = (double)height / cntY;

int posX;
int posY;
bool isPos;

int dy[4] = { -1, 0 ,0, 1 };
int dx[4] = { 0, -1, 1, 0};

GLfloat green[] =
{
	0.0f, 1.0f, 0.0f, 1.0f
};

GLfloat red[] =
{
	1.0f, 0.0f, 0.0f, 1.0f
};

GLfloat black[] =
{
	0.0f, 0.0f, 0.0f, 1.0f
};

GLfloat white[] =
{
	1.0f, 1.0f, 1.0f, 1.0f
};

void initGLFW()
{
	cursorX = 0.0;
	cursorY = 0.0;
	// GLFW 초기화
	if (!glfwInit())
	{
		cerr << "ERROR : init GLFW\n";
		return exit(-1);
	}

	// GLFW 설정
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window 생성
	window = glfwCreateWindow(1024, 768, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		cerr << "Error : open GLFW\n";
		glfwTerminate();
		return exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void initGLEW()
{
	// GLEW 초기화
	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		cerr << "ERROR : init GLEW\n";
		return exit(-1);
	}
}

GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath)
{
	// 쉐이더들 생성
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// 버텍스 쉐이더 코드를 파일에서 읽기
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFilePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		//printf("파일 %s 를 읽을 수 없음.\n", vertexFilePath);
		getchar();
		return 0;
	}

	// 프래그먼트 쉐이더 코드를 파일에서 읽기
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentFilePath, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// 버텍스 쉐이더를 컴파일
	printf("Compiling shader : %s\n", vertexFilePath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// 버텍스 쉐이더를 검사
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// 프래그먼트 쉐이더를 컴파일
	printf("Compiling shader : %s\n", fragmentFilePath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// 프래그먼트 쉐이더를 검사
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// 프로그램에 링크
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// 프로그램 검사
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void DrawRect(int y, int x, int type)
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix = glm::translate(matrix, glm::vec3(-1.0 + (double)x / (cntX/2) + gapX / width,
		1.0 - (double)y / (cntY / 2) - gapY / height, 0.f));
	//glm::rotate(matrix, 90.f, glm::vec3(0.f, 0.f, 1.f));
	matrix= glm::scale(matrix, glm::vec3(1.0f / cntX, 1.0f / cntY, 1.f));

	glUniformMatrix4fv(gLoc, 1, GL_FALSE, (&matrix[0][0]));
	glBindVertexArray(gRectArrayID);

	if (type == 0)
	{
		glVertexAttrib4fv(1, green);
	}
	else
	{
		glVertexAttrib4fv(1, red);
	}
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawLine(int v, int type)
{
	glm::mat4 matrix = glm::mat4(1.0f);
	if (type == 1)
	{
		matrix = glm::translate(matrix, glm::vec3(-1.0f + v * gapX * 2 / width, 0.0f, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(0.f,0.0f, 1.0f));
	}
	else
	{
		matrix = glm::translate(matrix, glm::vec3(0.0f, 1.0f - v * gapY * 2 / height, 0.0f));
	}

	glUniformMatrix4fv(gLoc, 1, GL_FALSE, (&matrix[0][0]));
	glBindVertexArray(gLineArrayID);
	glVertexAttrib4fv(1, black);

	glDrawArrays(GL_LINES, 0, 2);
}

float GetAngle(const glm::vec2& a, const glm::vec2& b)
{
	float cosAngle = glm::acos(glm::dot(a, b) / glm::length(a) * glm::length(b));
	return (a.x * b.y - a.y * b.x > 0.0f) ? cosAngle : -cosAngle;
}

void DrawArrow(int y, int x, glm::vec2 dir)
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix = glm::translate(matrix, glm::vec3(-1.0 + (double)x / (cntX / 2) + gapX / width,
		1.0 - (double)y / (cntY / 2) - gapY / height, 0.f));
	
	matrix = glm::scale(matrix, glm::vec3(1.0f / cntX, 1.0f / cntY, 1.f));
	matrix = glm::rotate(matrix, GetAngle(dir, glm::vec2(1.0, 0.0)), glm::vec3(0.f, 0.0f, 1.0f));

	glUniformMatrix4fv(gLoc, 1, GL_FALSE, (&matrix[0][0]));
	glBindVertexArray(gArrowArrayID);

	glVertexAttrib4fv(1, black);

	glDrawArrays(GL_LINES, 0, 6);
}

// 배열 생성
// 배열에 대한 정보 cost, 
void update(double time)
{

}


void render()
{
	for (int y = 0; y < cntY; ++y) 
	{
		for (int x = 0; x < cntX; ++x)
		{
			DrawRect(y, x, tiles[y][x]);
		}
	}

	// 배열 랜더링
	for (int y = 1; y < cntY; ++y)
	{
		DrawLine(y, 0);
	}

	for (int x = 1; x < cntX; ++x)
	{
		DrawLine(x, 1);
	}

	for (int y = 0; y < cntY; ++y)
	{
		for (int x = 0; x < cntX; ++x)
		{
			if (tiles[y][x] != 1)
				DrawArrow(y, x, direct[y][x]);
		}
	}
}

bool CheckRange(int y, int x) 
{
	if (0 <= x && x < cntX&& 0 <= y && y < cntY)
	{
		return true;
	}

	return false;
}

// 다익스트라
void CreateFlowField(int y, int x)
{

	priority_queue<pair<double, pair<int, int>>> pq;

	fill(&dist[0][0], &dist[cntY - 1][cntX], INF);
	dist[y][x] = 0;
	pq.push(make_pair(0, make_pair(y, x)));

	while (!pq.empty())
	{
		double cost = -pq.top().first;
		int ty = pq.top().second.first;
		int tx = pq.top().second.second;
		pq.pop();

		if (dist[ty][tx] < cost) continue;

		for (int i = 0; i < 4; ++i)
		{
			int nextY = dy[i] + ty;
			int nextX = dx[i] + tx;
			// 1이라고 가정한다.
			double nextCost = cost + 1;
			if (CheckRange(nextY, nextX) && tiles[nextY][nextX] != 1 && dist[nextY][nextX] > nextCost)
			{
				dist[nextY][nextX] = nextCost;
				pq.push(make_pair(-nextCost, make_pair(nextY, nextX)));
			}
		}
	}
	
	for (int i = 0; i < cntY; ++i)
	{
		for (int j = 0; j < cntY; ++j)
		{
			double cost = dist[i][j];
			
			double dir[4];

			for (int d = 0; d < 4; ++d)
			{
				int nY = i + dy[d];
				int nX = j + dx[d];

				if (CheckRange(nY, nX) && tiles[nY][nX] != 1)
				{
					dir[d] = dist[nY][nX];
				}
				else
				{
					dir[d] = cost;
				}
			}

			direct[i][j] = glm::normalize(glm::vec2(dir[1] - dir[2], dir[0] - dir[3]));
		}
	}

	for (int y = 0; y < cntY; ++y)
	{
		for (int x = 0; x < cntY; ++x)
			printf("%d %d : %f %f\n", y, x, direct[y][x].x, direct[y][x].y);
	}
}

void HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
	cursorX = xPos;
	cursorY = yPos;
}

void HandleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		printf("유닛 이동 cursorX : %f, cursorX : %f\n", cursorX, cursorY);

		posX = cursorX / gapX;
		posY = cursorY / gapY;
		if (tiles[posY][posX] != 1)
		{
			isPos = true;
			CreateFlowField(posY, posX);
		}
	}
	// 왼쪽 마우스 클릭시
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{

	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// 배열에서 좌표를 구한다.
		int x = cursorX / gapX;
		int y = cursorY / gapY;

		printf("지형 생성/삭제 x : %d, y : %d\n", x, y);
		tiles[y][x] = tiles[y][x] ? 0 : 1;
		if (isPos)
			CreateFlowField(posY, posX);
	}
}

void initRect()
{
	static const GLfloat vertexRect[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenVertexArrays(1, &gRectArrayID);
	glBindVertexArray(gRectArrayID);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexRect), vertexRect, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void initLine()
{
	static const GLfloat vertexLine[] =
	{
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	glGenVertexArrays(1, &gLineArrayID);
	glBindVertexArray(gLineArrayID);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), vertexLine, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void initArrow()
{
	static const GLfloat vertexArrow[] =
	{
		0.0f, -0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,

		0.0f,  0.5f, 0.0f,
		0.5f,  0.0f, 0.0f,

		0.5f,  0.0f, 0.0f,
		-0.5f,  0.0f, 0.0f,
	};

	glGenVertexArrays(1, &gArrowArrayID);
	glBindVertexArray(gArrowArrayID);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertexArrow, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void initCircle()
{
	int steps = 10;
	float radius = 1.0;
	float* vertexCircle = new float[steps * 6];
	float t = 0;
	int pos = 0;
	
	for (int i = 0; i < steps; ++i) 
	{
		float x = radius * cos(t);
		float y = radius * sin(t);

		vertexCircle[pos++] = x;
		vertexCircle[pos++] = y;
		vertexCircle[pos++] = 0.0f;

		vertexCircle[pos++] = 0.0f;
		vertexCircle[pos++] = 0.0f;
		vertexCircle[pos++] = 0.0f;

		t += 360 / steps;
	}

	glGenVertexArrays(1, &gCircleArrayID);
	glBindVertexArray(gCircleArrayID);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, steps * 3 * sizeof(GLfloat), vertexCircle, GL_STATIC_DRAW);

	delete[] vertexCircle;
}

void initGeometry()
{
	initRect();
	initLine();
	initArrow();
	initCircle();
}

int main()
{
	isPos = false;
	initGLFW();
	initGLEW();
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2, height / 2);
	glfwSetCursorPosCallback(window, HandleMouse);
	glfwSetMouseButtonCallback(window, HandleMouseButton);

	initGeometry();
	
	// 공용으로 사용한다.
	GLuint programID = LoadShaders("vertex.glsl", "Image.glsl");
	glUseProgram(programID);
	gLoc = glGetUniformLocation(programID, "mMatrix");

	do
	{
		double t = glfwGetTime();
		update(t);
		// drawing
		static const GLfloat blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, blue);

		render();
		// swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}