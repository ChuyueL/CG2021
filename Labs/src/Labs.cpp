#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ModelTriangle.h>
#include <unordered_map>

#define WIDTH 320
#define HEIGHT 240
#define MULTIPLIER 500


std::vector<ModelTriangle> modelTriangles;
const float scalingFactor = 0.17;
std::unordered_map<std::string, Colour> colourPalette;

const glm::vec3 CameraPos(0.0, 0.0, 4.0);
const float FocalLen = 1.0;


std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> values;
	float division = (to - from) / (float)(numberOfValues - 1); //subtract 1 because values are inclusive of this range

	float currentVal = from;
	values.push_back(currentVal);

	for (size_t i = 0; i < numberOfValues - 1; i++)
	{
		currentVal += division;
		values.push_back(currentVal);
	}

	return values;

}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
	std::vector<glm::vec3> values;
	std::vector<double> divisions;

	for (size_t i = 0; i < 3; i++)
	{
		double division = (to[i] - from[i]) / (double)(numberOfValues - 1);
		divisions.push_back(division);
	}

	glm::vec3 division(divisions[0], divisions[1], divisions[2]);

	glm::vec3 currentVec = from;
	values.push_back(currentVec);

	for (size_t i = 0; i < numberOfValues - 1; i++)
	{
		currentVec += division;
		values.push_back(currentVec);
	}

	return values;

}

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void drawGreyscale(DrawingWindow& window) {
	window.clearPixels();
	std::vector<float> values = interpolateSingleFloats(255, 0, window.width);
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = values[x], green = values[x], blue = values[x];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void drawColour(DrawingWindow& window) {
	window.clearPixels();
	glm::vec3 topLeft(255, 0, 0);        // red 
	glm::vec3 topRight(0, 0, 255);       // blue 
	glm::vec3 bottomRight(0, 255, 0);    // green 
	glm::vec3 bottomLeft(255, 255, 0);   // yellow

	std::vector<glm::vec3> firstCol = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	std::vector<glm::vec3> lastCol = interpolateThreeElementValues(topRight, bottomRight, window.height);

	for (size_t y = 0; y < window.height; y++) {
		std::vector<glm::vec3> row = interpolateThreeElementValues(firstCol[y], lastCol[y], window.width);
		for (size_t x = 0; x < window.width; x++) {
			float red = (row[x])[0], green = (row[x])[1], blue = (row[x])[2];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}

}

void drawLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour) {
	//window.clearPixels();
	float xDiff = (to.x - from.x);
	float yDiff = (to.y - from.y);
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff))*1.5; //*1.5 to avoid swiss cheese
	
	std::vector<float> xValues = interpolateSingleFloats(from.x, to.x, numberOfSteps+1); //need to add 1 because we want exactly numberOfSteps steps but interpolate would give us numberOfSteps-1.
	std::vector<float> yValues = interpolateSingleFloats(from.y, to.y, numberOfSteps+1);
	/*
	for (size_t i = 0; i < xValues.size(); i++) std::cout << "(" << xValues[i] << ", " << yValues[i] << std::endl;
	std::cout << "end";
	std::cout << std::endl;
	*/

	//float xStepSize = xDiff / numberOfSteps;
	//float yStepSize = yDiff / numberOfSteps;

	for (float i = 0.0; i < numberOfSteps; i++)
	{
		float x = xValues[i];
		float y = yValues[i];
		//float x = from.x + i * xStepSize;
		//float y = from.y + i * yStepSize;
		
		uint32_t col = (255 < 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;

		//std::cout << "(" << (float)x << ", " << (float)y << std::endl;

		window.setPixelColour(round(x), round(y), col);
	}
}

void drawStrokedTriangle(DrawingWindow& window, CanvasTriangle vertices, Colour colour) {
	//window.clearPixels();
	drawLine(window, vertices.v0(), vertices.v1(), colour);
	drawLine(window, vertices.v0(), vertices.v2(), colour);
	drawLine(window, vertices.v1(), vertices.v2(), colour);
}

std::array<CanvasPoint, 3> sortVertices(std::array<CanvasPoint, 3> vertices) {
	std::sort(vertices.begin(), vertices.end(),
		[](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	return vertices;
}

CanvasPoint interpolatePoint(std::array<CanvasPoint, 3> vertices) {
	CanvasPoint top = vertices[0];
	CanvasPoint side = vertices[1];
	CanvasPoint bottom = vertices[2];

	float y = side.y;
	float x;
	float x_y_ratio = abs((top.x - bottom.x) / (top.y - bottom.y));
	int xDiff = (int)(abs(top.y - side.y) * x_y_ratio);
	if (bottom.x < top.x) {
		x = top.x - xDiff;
	}
	else {
		x = top.x + xDiff;
	}

	CanvasPoint middlePoint(x, y);
	std::cout << "interpolated: " << middlePoint << std::endl;
	return middlePoint;

}

void fillFlatBottomTriangle(DrawingWindow& window, CanvasTriangle vertices, Colour colour) {
	//std::cout << "first func";
	std::array<CanvasPoint, 3> sortedVertices = sortVertices(vertices.vertices); //extra sort just to be sureee
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::sort(sortedVertices.begin(), sortedVertices.end(), [](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	CanvasPoint top = sortedVertices[0];
	CanvasPoint bottom = sortedVertices[1];
	CanvasPoint bottom1 = sortedVertices[2];
	float yDiff = abs(top.y -bottom.y);
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top.x, bottom1.x, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i);
		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i);
		drawLine(window, startPoint, endPoint, colour);
	}

}

void fillFlatTopTriangle(DrawingWindow& window, CanvasTriangle vertices, Colour colour) {
	
	std::array<CanvasPoint, 3> sortedVertices = sortVertices(vertices.vertices);
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	CanvasPoint top = sortedVertices[0];
	CanvasPoint top1 = sortedVertices[1];
	CanvasPoint bottom = sortedVertices[2];
	float yDiff = abs(top.y - bottom.y);
	//std::cout << "before interpolate";
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top1.x, bottom.x, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i);
		//std::cout << startPoint;
		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i);
		//std::cout << endPoint << std::endl;
		//std::cout << "before drawing";
		drawLine(window, startPoint, endPoint, colour);
		//std::cout << "after drawing";

	}
}

void drawFilledTriangle(DrawingWindow& window, CanvasTriangle vertices, Colour colour) {
	//std::cout << vertices.vertices[0].x << ", " << vertices.vertices[0].y << std::endl;
	std::array<CanvasPoint, 3> sortedVertices = sortVertices(vertices.vertices);
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::sort(sortedVertices.begin(), sortedVertices.end(), [](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	std::cout << sortedVertices[0] << sortedVertices[1] << sortedVertices[2] << std::endl;
	//std::cout << sortedVertices[0] << std::endl;
	CanvasPoint interpolatedPoint = interpolatePoint(sortedVertices);
	float middleY = interpolatedPoint.y;

	CanvasPoint top = sortedVertices[0];
	CanvasPoint side = sortedVertices[1];
	CanvasPoint bottom = sortedVertices[2];

	CanvasTriangle topTriangle(top, interpolatedPoint, side);
	CanvasTriangle bottomTriangle(interpolatedPoint, side, bottom);

	fillFlatBottomTriangle(window, topTriangle, colour);
	fillFlatTopTriangle(window, bottomTriangle, colour);

	Colour white(255, 255, 255);

	//drawStrokedTriangle(window, vertices, white);
}

glm::vec3 parseVector(std::string line) {
	char delimiter = ' ';
	std::vector<std::string> tokens = split(line, delimiter);
	/*for (size_t i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << std::endl;
	}*/
	glm::vec3 newVec(stof(tokens[1]) * scalingFactor, stof(tokens[2]) * scalingFactor, stof(tokens[3]) * scalingFactor);
	return newVec;
}

std::vector<int> parseFacet(std::string line) {
	char delimiter = ' ';
	
	std::vector<std::string> tokens = split(line, delimiter);
	/*for (size_t i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << std::endl;
	}*/

	std::vector<int> indices = { stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]) };

	for (size_t i = 0; i < indices.size(); i++)
	{
		std::cout << indices[i] << " ";
	}
	std::cout << std::endl;

	return indices;
}

void parseOBJ(std::string filename) {
	std::string line;
	std::ifstream readFile(filename);
	std::vector<glm::vec3> vertices;
	Colour currentCol;
	while (getline(readFile, line)) {
		// Output the text from the file
		if (line[0] == 'v') {
			glm::vec3 vec = parseVector(line);
			vertices.push_back(vec);
		}
		else if (line[0] == 'f') {
			std::vector<int> facetIndices = parseFacet(line);
			ModelTriangle newTriangle(vertices[facetIndices[0] - 1], vertices[facetIndices[1] - 1], vertices[facetIndices[2] - 1], currentCol);
			modelTriangles.push_back(newTriangle);
			
		}
		else if (line.find("usemtl") != std::string::npos) {
			std::vector<std::string> tokens = split(line, ' ');
			std::string colName = tokens[1];
			currentCol = colourPalette[colName];
		}
	}

	readFile.close();
}

void parseMTL(std::string filename) {
	std::string line;
	std::ifstream rf(filename);
	char delimiter = ' ';
	std::string currentName;

	while (getline(rf, line)) {
		if (line.find("newmtl") != std::string::npos) {
			std::vector<std::string> tokens = split(line, delimiter);
			currentName = tokens[1];
			std::cout << currentName << std::endl;
		}
		else if (line.find("Kd") != std::string::npos) {
			std::vector<std::string> tokens = split(line, delimiter);
			float red = stof(tokens[1]);
			float green = stof(tokens[2]);
			float blue = stof(tokens[3]);
			std::cout << red << " " << green << " " << blue << std::endl;
			Colour newCol(round(red * 255), round(green * 255), round(blue * 255));
			colourPalette[currentName] = newCol;
		}
	}
	
}

glm::vec3 getVertexRelativeToCamera(glm::vec3 cameraPosition, glm::vec3 vertexPosition) {
	glm::vec3 relative = vertexPosition - cameraPosition;
	return relative;
}

CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength) {
	glm::vec3 relativePoint = getVertexRelativeToCamera(cameraPosition, vertexPosition);
	float x = relativePoint.x;
	float y = relativePoint.y;
	float z = relativePoint.z;
	float u = focalLength * (x / z) * MULTIPLIER;
	float v = focalLength * (y / z) * MULTIPLIER;
	u += WIDTH / 2;
	v += HEIGHT / 2;
	u = WIDTH - u; //jank
	CanvasPoint pt(u, v);
	std::cout << pt << std::endl;
	return pt;
}

std::vector<std::vector<float>> populateDepthBuffer() {

}

void drawPointCloud(DrawingWindow& window) {
	for (ModelTriangle triangle : modelTriangles) {
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(CameraPos, vertex, FocalLen);
			

			window.setPixelColour(point.x, point.y, UINT32_MAX);
		}
	}
}

void drawWireframe(DrawingWindow& window) {
	for (ModelTriangle triangle : modelTriangles) {
		std::vector<CanvasPoint> relativeVertices;
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(CameraPos, vertex, FocalLen);
			relativeVertices.push_back(point);
		}
		CanvasTriangle relativeTriangle(relativeVertices[0], relativeVertices[1], relativeVertices[2]);
		Colour white(255, 255, 255);
		drawStrokedTriangle(window, relativeTriangle, white);
	}
}

void drawRasterised(DrawingWindow& window) {
	for (ModelTriangle triangle : modelTriangles) {
		std::vector<CanvasPoint> relativeVertices;
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(CameraPos, vertex, FocalLen);
			relativeVertices.push_back(point);
		}
		CanvasTriangle relativeTriangle(relativeVertices[0], relativeVertices[1], relativeVertices[2]);
		Colour colour = triangle.colour;
		drawFilledTriangle(window, relativeTriangle, colour);
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) {
			std::cout << "u" << std::endl;
			CanvasPoint v0(rand() % 320, rand() % 240);
			CanvasPoint v1(rand() % 320, rand() % 240);
			CanvasPoint v2(rand() % 320, rand() % 240);
			CanvasTriangle triangle(v0, v1, v2);
			Colour colour(rand() % 255, rand() % 255, rand() % 255);
			drawStrokedTriangle(window, triangle, colour);
		}
		else if (event.key.keysym.sym == SDLK_f) {
			std::cout << "f" << std::endl;
			CanvasPoint v0(rand() % 320, rand() % 240);
			CanvasPoint v1(rand() % 320, rand() % 240);
			CanvasPoint v2(rand() % 320, rand() % 240);
			CanvasTriangle triangle(v0, v1, v2);
			Colour colour(rand() % 255, rand() % 255, rand() % 255);
			drawFilledTriangle(window, triangle, colour);
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	/*
	std::vector<glm::vec3> result;
	glm::vec3 from(1, 4, 9.2);
	glm::vec3 to(4, 1, 9.8);
	result = interpolateThreeElementValues(from, to, 4);
	for (size_t i = 0; i < result.size(); i++) std::cout << glm::to_string(result[i]) << std::endl;
	std::cout << std::endl;
	*/

	parseMTL("cornell-box.mtl");
	parseOBJ("cornell-box.obj");
	/*for (size_t i = 0; i < modelTriangles.size(); i++)
	{
		std::cout << modelTriangles[i] << std::endl;
		
	}*/
	
	std::vector<std::vector<float>> depthBuffer;
	

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		//drawStrokedTriangle(window, triangle, colour);
		//drawFilledTriangle(window, triangle, colour);
		drawRasterised(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
