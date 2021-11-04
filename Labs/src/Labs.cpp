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
#include <TextureMap.h>
#include <unordered_map>

#define WIDTH 640
#define HEIGHT 480
#define MULTIPLIER 700
#define PI 3.14159265

std::vector<CanvasTriangle> texturedTriangles;
std::vector<std::vector<uint32_t>> texturePixels2D;

std::vector<ModelTriangle> modelTriangles;
const float scalingFactor = 0.17;
std::unordered_map<std::string, Colour> colourPalette;

//glm::vec3 CameraPos(0.0, 0.0, 2.0);
//float FocalLen = 1.0;

std::vector<std::vector<float>> depthBuffer;

struct camera {
	glm::vec3 cameraPos;
	float focalLen;
	glm::mat3 orientation;
} camera;

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

glm::mat3 makeXRotationMatrix(float angleInDegrees) {
	float theta = angleInDegrees * (PI / 180.0); //angle in radians
	glm::vec3 col1(1.0, 0.0, 0.0);
	glm::vec3 col2(0.0, std::cos(theta), std::sin(theta));
	glm::vec3 col3(0.0, -std::sin(theta), std::cos(theta));
	glm::mat3 rotationMat(col1, col2, col3);
	return rotationMat;
}

glm::mat3 makeYRotationMatrix(float angleInDegrees) {
	float theta = angleInDegrees * (PI / 180.0);
	glm::vec3 col1(std::cos(theta), 0.0, -std::sin(theta));
	glm::vec3 col2(0.0, 1.0, 0.0);
	glm::vec3 col3(std::sin(theta), 0.0, std::cos(theta));
	glm::mat3 rotationMat(col1, col2, col3);
	return rotationMat;
}

glm::mat3 makeZRotationMatrix(float angleInDegrees) {
	float theta = angleInDegrees * (PI / 180);
	glm::vec3 col1(std::cos(theta), std::sin(theta), 0);
	glm::vec3 col2(-std::sin(theta), std::cos(theta), 0);
	glm::vec3 col3(0, 0, 1);
	glm::mat3 rotationMat(col1, col2, col3);
	return rotationMat;
}


void drawLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour) {
	//window.clearPixels();
	float xDiff = (to.x - from.x);
	float yDiff = (to.y - from.y);
	float zDiff = (to.depth - from.depth);
	//float max_X_Y = std::max(abs(xDiff), abs(yDiff));
	//float numberOfSteps = std::max(max_X_Y, abs(zDiff));
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff)) * 2.0; //*2.0 to avoid swiss cheese
	
	std::vector<float> xValues = interpolateSingleFloats(from.x, to.x, numberOfSteps+1); //need to add 1 because we want exactly numberOfSteps steps but interpolate would give us numberOfSteps-1.
	std::vector<float> yValues = interpolateSingleFloats(from.y, to.y, numberOfSteps+1);
	//std::vector<float> zValues = interpolateSingleFloats(from.depth, to.depth, numberOfSteps + 1);
	/*
	for (size_t i = 0; i < xValues.size(); i++) std::cout << "(" << xValues[i] << ", " << yValues[i] << std::endl;
	std::cout << "end";
	std::cout << std::endl;
	*/

	//float xStepSize = xDiff / numberOfSteps;
	//float yStepSize = yDiff / numberOfSteps;

	for (size_t i = 0; i < numberOfSteps; i++)
	{
		float x = xValues[i];
		float y = yValues[i];
		//float depth = 1.0 / abs(zValues[i]);
		//std::cout << zValues[i] << std::endl;
		//float x = from.x + i * xStepSize;
		//float y = from.y + i * yStepSize;
		
		uint32_t col = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;

		//std::cout << "(" << (float)x << ", " << (float)y << std::endl;

		window.setPixelColour(round(x), round(y), col);

	}
}

//guaranteed this will be a straight line
void drawLineWithDepth(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour) {
	float xDiff = (to.x - from.x);
	float yDiff = (to.y - from.y);
	//float numberOfSteps = abs(xDiff) *2.0;
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff)) * 2.0;
	std::vector<float> xValues = interpolateSingleFloats(from.x, to.x, numberOfSteps + 1);
	std::vector<float> zValues = interpolateSingleFloats(from.depth, to.depth, numberOfSteps + 1);
	std::vector<float> yValues = interpolateSingleFloats(from.y, to.y, numberOfSteps + 1);

	for (size_t i = 0; i < numberOfSteps; i++)
	{
		int x = round(xValues[i]);
		int y = round(yValues[i]);
		float depth = 1 / abs(zValues[i]);
		
		if (depth > depthBuffer[x][y]) {
			uint32_t col = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
			window.setPixelColour(x, y, col);
			depthBuffer[x][y] = depth;

		}

		

		//std::cout << "(" << (float)x << ", " << (float)y << std::endl;

		
		
		
	}
}

void drawStrokedTriangle(DrawingWindow& window, CanvasTriangle vertices, Colour colour) {
	//window.clearPixels();
	drawLine(window, vertices.v0(), vertices.v1(), colour);
	drawLine(window, vertices.v0(), vertices.v2(), colour);
	drawLine(window, vertices.v1(), vertices.v2(), colour);
}

std::vector<CanvasPoint> sortVertices(std::vector<CanvasPoint> vertices) {
	std::sort(vertices.begin(), vertices.end(),
		[](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	return vertices;
}

float interpolateDepth(CanvasPoint top, CanvasPoint side, CanvasPoint bottom) {
	float y = side.y;
	float z;
	float z_y_ratio = abs((top.depth - bottom.depth) / (top.y - bottom.y));
	float zDiff = abs(top.y - side.y) * z_y_ratio;
	if (bottom.depth < top.depth) {
		z = top.depth - zDiff;
	}
	else {
		z = top.depth + zDiff;
	}

	return z;
}


CanvasPoint interpolatePoint(std::vector<CanvasPoint> vertices) {
	CanvasPoint top = vertices[0];
	CanvasPoint side = vertices[1];
	CanvasPoint bottom = vertices[2];

	float y = side.y;
	float x;
	float x_y_ratio = abs((top.x - bottom.x) / (top.y - bottom.y));
	float xDiff = (abs(top.y - side.y) * x_y_ratio);
	if (bottom.x < top.x) {
		x = top.x - xDiff;
	}
	else {
		x = top.x + xDiff;
	}


	float depth = interpolateDepth(top, side, bottom);

	CanvasPoint middlePoint(x, y, depth);
	//std::cout << "interpolated: " << middlePoint << std::endl;
	return middlePoint;

}

void fillFlatBottomTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
	//std::cout << "first func";
	std::vector<CanvasPoint> verticesVector = { triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] };
	std::vector<CanvasPoint> sortedVertices = sortVertices(verticesVector); //extra sort just to be sureee
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::sort(sortedVertices.begin(), sortedVertices.end(), [](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	CanvasPoint top = sortedVertices[0];
	CanvasPoint bottom = sortedVertices[1];
	CanvasPoint bottom1 = sortedVertices[2];
	float yDiff = abs(top.y - bottom.y);
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top.x, bottom1.x, yDiff + 1);
	std::vector<float> firstEdgeZVals = interpolateSingleFloats(top.depth, bottom.depth, yDiff + 1);
	std::vector<float> secondEdgeZVals = interpolateSingleFloats(top.depth, bottom1.depth, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i, firstEdgeZVals[i]);
		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i, secondEdgeZVals[i]);
		//drawLine(window, startPoint, endPoint, colour);
		drawLineWithDepth(window, startPoint, endPoint, colour);
	}

}

void fillFlatTopTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
	std::vector<CanvasPoint> verticesVector = { triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] };
	std::vector<CanvasPoint> sortedVertices = sortVertices(verticesVector);
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	CanvasPoint top = sortedVertices[0];
	CanvasPoint top1 = sortedVertices[1];
	CanvasPoint bottom = sortedVertices[2];
	float yDiff = abs(top.y - bottom.y);
	//std::cout << "before interpolate";
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top1.x, bottom.x, yDiff + 1);
	std::vector<float> firstEdgeZVals = interpolateSingleFloats(top.depth, bottom.depth, yDiff + 1);
	std::vector<float> secondEdgeZVals = interpolateSingleFloats(top1.depth, bottom.depth, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i, firstEdgeZVals[i]);
		//std::cout << startPoint;
		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i, secondEdgeZVals[i]);
		//std::cout << endPoint << std::endl;
		//std::cout << "before drawing";
		//drawLine(window, startPoint, endPoint, colour);
		drawLineWithDepth(window, startPoint, endPoint, colour);
		//std::cout << "after drawing";

	}
}

void drawFilledTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
	//std::cout << vertices.vertices[0].x << ", " << vertices.vertices[0].y << std::endl;
	std::vector<CanvasPoint> verticesVector = { triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] };
	
	std::vector<CanvasPoint> sortedVertices = sortVertices(verticesVector);
	//std::cout << "depths1 " << sortedVertices[0].depth << sortedVertices[1].depth << sortedVertices[2].depth << std::endl;
	//std::array<CanvasPoint, 3> sortedVertices = vertices.vertices;
	//std::sort(sortedVertices.begin(), sortedVertices.end(), [](CanvasPoint v0, CanvasPoint v1) { return v0.y < v1.y; });
	//std::cout << sortedVertices[0] << sortedVertices[1] << sortedVertices[2] << std::endl;
	//std::cout << sortedVertices[0] << std::endl;
	CanvasPoint interpolatedPoint = interpolatePoint(sortedVertices);

	CanvasPoint top = sortedVertices[0];
	CanvasPoint side = sortedVertices[1];
	CanvasPoint bottom = sortedVertices[2];

	CanvasTriangle topTriangle(top, interpolatedPoint, side);
	CanvasTriangle bottomTriangle(interpolatedPoint, side, bottom);

	fillFlatBottomTriangle(window, topTriangle, colour);
	fillFlatTopTriangle(window, bottomTriangle, colour);

	//Colour white(255, 255, 255);

	//drawStrokedTriangle(window, triangle, white);
}

//Guaranteed to be a horizontal line in CanvasPoint space
void drawTextureLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to) {
	float xDiff = from.x - to.x;
	int numberOfSteps = round(abs(xDiff) * 1.5);
	std::vector<float> xValues = interpolateSingleFloats(from.x, to.x, numberOfSteps+1);
	int y = round(from.y);

	for (size_t i = 0; i < numberOfSteps; i++)
	{
		float x = xValues[i];
		float proportion = abs((from.x - x) / (from.x - to.x));
		float textureX = from.texturePoint.x + proportion * (to.texturePoint.x - from.texturePoint.x);
		float textureY = from.texturePoint.y + proportion * (to.texturePoint.y - from.texturePoint.y);

		uint32_t colour = texturePixels2D[round(textureY)][round(textureX)];
		window.setPixelColour(round(x), y, colour);
	}
}

void textureFlatBottomTriangle(DrawingWindow& window, CanvasTriangle triangle) {
	std::vector<CanvasPoint> verticesVector = { triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] };
	std::vector<CanvasPoint> sortedVertices = sortVertices(verticesVector); //to be sure to be sure to be sure....
	CanvasPoint top = sortedVertices[0];
	CanvasPoint bottom = sortedVertices[1];
	CanvasPoint bottom1 = sortedVertices[2];

	float yDiff = round(abs(top.y - bottom.y));
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top.x, bottom1.x, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i);
		float yProportionStart = abs((top.y - startPoint.y) / (top.y - bottom.y));
		float xProportionStart = abs((top.x - startPoint.x) / (top.x - bottom.x));
		float textureXStart = top.texturePoint.x + xProportionStart * (bottom.texturePoint.x - top.texturePoint.x);
		float textureYStart = top.texturePoint.y + yProportionStart * (bottom.texturePoint.y - top.texturePoint.y);
		TexturePoint startTexturePoint(textureXStart, textureYStart);
		startPoint.texturePoint = startTexturePoint;

		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i);
		float yProportionEnd = abs((top.y - endPoint.y) / (top.y - bottom1.y));
		float xProportionEnd = abs((top.x - endPoint.x) / (top.x - bottom1.x));
		float textureXEnd = top.texturePoint.x + xProportionEnd * (bottom1.texturePoint.x - top.texturePoint.x);
		float textureYEnd = top.texturePoint.y + yProportionEnd * (bottom1.texturePoint.y - top.texturePoint.y);
		TexturePoint endTexturePoint(textureXEnd, textureYEnd);
		endPoint.texturePoint = endTexturePoint;

		drawTextureLine(window, startPoint, endPoint);
	}
}

void textureFlatTopTriangle(DrawingWindow& window, CanvasTriangle triangle) {
	std::vector<CanvasPoint> verticesVector = { triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] };
	std::vector<CanvasPoint> sortedVertices = sortVertices(verticesVector); //to be sure to be sure to be sure....
	CanvasPoint top = sortedVertices[0];
	CanvasPoint top1 = sortedVertices[1];
	CanvasPoint bottom = sortedVertices[2];

	float yDiff = round(abs(top.y - bottom.y));
	std::vector<float> firstEdgeXPoints = interpolateSingleFloats(top.x, bottom.x, yDiff + 1);
	std::vector<float> secondEdgeXPoints = interpolateSingleFloats(top1.x, bottom.x, yDiff + 1);

	for (size_t i = 0; i < yDiff; i++)
	{
		CanvasPoint startPoint(firstEdgeXPoints[i], top.y + i);
		float yProportionStart = abs((top.y - startPoint.y) / (top.y - bottom.y));
		float xProportionStart = abs((top.x - startPoint.x) / (top.x - bottom.x));
		float textureXStart = top.texturePoint.x + xProportionStart * (bottom.texturePoint.x - top.texturePoint.x);
		float textureYStart = top.texturePoint.y + yProportionStart * (bottom.texturePoint.y - top.texturePoint.y);
		TexturePoint startTexturePoint(textureXStart, textureYStart);
		startPoint.texturePoint = startTexturePoint;

		CanvasPoint endPoint(secondEdgeXPoints[i], top.y + i);
		float yProportionEnd = abs((top1.y - endPoint.y) / (top1.y - bottom.y));
		float xProportionEnd = abs((top1.x - endPoint.x) / (top1.x - bottom.x));
		float textureXEnd = top1.texturePoint.x + xProportionEnd * (bottom.texturePoint.x - top1.texturePoint.x);
		float textureYEnd = top1.texturePoint.y + yProportionEnd * (bottom.texturePoint.y - top1.texturePoint.y);
		TexturePoint endTexturePoint(textureXEnd, textureYEnd);
		endPoint.texturePoint = endTexturePoint;

		drawTextureLine(window, startPoint, endPoint);
	}
}

//Interpolate the "middle" point for a triangle which has a TexturePoint
CanvasPoint interpolateTexturedPoint(CanvasPoint top, CanvasPoint side, CanvasPoint bottom) {
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
	
	float yProportion = abs((top.y - y) / (top.y - bottom.y));
	float xProportion = abs((x - top.x) / (top.x - bottom.x));

	float textureX = top.texturePoint.x + xProportion * (bottom.texturePoint.x - top.texturePoint.x);
	float textureY = top.texturePoint.y + yProportion * (bottom.texturePoint.y - top.texturePoint.y);

	CanvasPoint middlePoint(x, y);
	TexturePoint texturePt(textureX, textureY);
	middlePoint.texturePoint = texturePt;
	
	return middlePoint;
}

void drawTexturedTriangles(DrawingWindow& window) {
	for (CanvasTriangle texturedTriangle : texturedTriangles) {
		std::vector<CanvasPoint> vertices = { texturedTriangle.vertices[0], texturedTriangle.vertices[1], texturedTriangle.vertices[2] };
		std::vector<CanvasPoint> sortedVertices = sortVertices(vertices);
		CanvasPoint top = sortedVertices[0];
		CanvasPoint side = sortedVertices[1];
		CanvasPoint bottom = sortedVertices[2];

		CanvasPoint interpolatedPoint = interpolateTexturedPoint(top, side, bottom);
		CanvasTriangle topTriangle(top, side, interpolatedPoint);
		CanvasTriangle bottomTriangle(side, interpolatedPoint, bottom);

		textureFlatBottomTriangle(window, topTriangle);
		textureFlatTopTriangle(window, bottomTriangle);

		Colour white(255, 255, 255);
		drawStrokedTriangle(window, texturedTriangle, white);
	}
}

void parseTexture(std::string filename) {
	TextureMap texture(filename);
	std::vector<uint32_t> pixels_1D = texture.pixels;

	std::cout << texture.height << " " << texture.width << std::endl;

	for (size_t i = 0; i < texture.height; i++)
	{
		std::vector<uint32_t> row;
		for (size_t j = 0; j < texture.width; j++)
		{
			row.push_back(pixels_1D[i * texture.width + j]);

		}
		texturePixels2D.push_back(row);
	}
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

	/*for (size_t i = 0; i < indices.size(); i++)
	{
		std::cout << indices[i] << " ";
	}
	std::cout << std::endl;*/

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
			//std::cout << currentName << std::endl;
		}
		else if (line.find("Kd") != std::string::npos) {
			std::vector<std::string> tokens = split(line, delimiter);
			float red = stof(tokens[1]);
			float green = stof(tokens[2]);
			float blue = stof(tokens[3]);
			//std::cout << red << " " << green << " " << blue << std::endl;
			Colour newCol(round(red * 255), round(green * 255), round(blue * 255));
			colourPalette[currentName] = newCol;
		}
	}
	
}

glm::vec3 getVertexRelativeToCamera(glm::vec3 vertexPosition) {
	//glm::vec3 relative = vertexPosition - cameraPosition;
	glm::vec3 relative = (camera.cameraPos - vertexPosition) * camera.orientation;
	return relative;
}

CanvasPoint getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
	glm::vec3 relativePoint = getVertexRelativeToCamera(vertexPosition);
	float x = relativePoint.x;
	float y = relativePoint.y;
	float z = relativePoint.z;
	float u = camera.focalLen * (x / z) * MULTIPLIER;
	float v = camera.focalLen * (y / z) * MULTIPLIER;
	u += WIDTH / 2;
	v += HEIGHT / 2;
	u = WIDTH - u; //jank
	CanvasPoint pt(u, v, z);
	//std::cout << pt << std::endl;
	return pt;
}


void drawPointCloud(DrawingWindow& window) {
	for (ModelTriangle triangle : modelTriangles) {
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(vertex);
			

			window.setPixelColour(point.x, point.y, UINT32_MAX);
		}
	}
}

void drawWireframe(DrawingWindow& window) {
	for (ModelTriangle triangle : modelTriangles) {
		std::vector<CanvasPoint> relativeVertices;
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(vertex);
			relativeVertices.push_back(point);
		}
		CanvasTriangle relativeTriangle(relativeVertices[0], relativeVertices[1], relativeVertices[2]);
		Colour white(255, 255, 255);
		drawStrokedTriangle(window, relativeTriangle, white);
	}
}

void lookAt() {
	glm::vec3 forward = camera.cameraPos;
	glm::vec3 vertical(0, 1, 0);
	glm::vec3 right = glm::normalize(glm::cross(vertical, forward));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));
	glm::mat3 newOrientation(right, up, forward);
	camera.orientation = newOrientation;
	std::cout << glm::to_string(camera.orientation) << std::endl;
}

void drawRasterised(DrawingWindow& window) {
	window.clearPixels();
	glm::mat3 rotation = makeYRotationMatrix(0.5);
	camera.cameraPos = rotation * camera.cameraPos; //orbit
	lookAt();
	for (ModelTriangle triangle : modelTriangles) {
		std::vector<CanvasPoint> projectedVertices;
		for (glm::vec3 vertex : triangle.vertices) {
			CanvasPoint point = getCanvasIntersectionPoint(vertex);
			projectedVertices.push_back(point);
		}
		CanvasTriangle relativeTriangle(projectedVertices[0], projectedVertices[1], projectedVertices[2]);
		//std::cout << "depths " << projectedVertices[0].depth << projectedVertices[1].depth << projectedVertices[2].depth << std::endl;
		Colour colour = triangle.colour;
		drawFilledTriangle(window, relativeTriangle, colour);
	}
}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			std::cout << "LEFT" << std::endl;
			glm::vec3 left(0.01, 0, 0);
			camera.cameraPos += left;
			std::cout << glm::to_string(camera.cameraPos) << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			std::cout << "RIGHT" << std::endl;
			glm::vec3 right(-0.01, 0, 0);
			camera.cameraPos += right;
		}
		else if (event.key.keysym.sym == SDLK_UP) {
			std::cout << "UP" << std::endl;
			glm::vec3 up(0, -0.01, 0);
			camera.cameraPos += up;
		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			std::cout << "DOWN" << std::endl;
			glm::vec3 down(0, 0.01, 0);
			camera.cameraPos += down;
		}
		else if (event.key.keysym.sym == SDLK_w) {
			std::cout << "FORWARD" << std::endl;
			glm::vec3 forward(0, 0, -0.01);
			camera.cameraPos += forward;
		}
		else if (event.key.keysym.sym == SDLK_s) {
			std::cout << "BACKWARDS" << std::endl;
			glm::vec3 backward(0, 0, 0.01);
			camera.cameraPos += backward;
		}
		else if (event.key.keysym.sym == SDLK_x) {
			glm::mat3 rotationMat = makeXRotationMatrix(1.0);
			camera.cameraPos = rotationMat * camera.cameraPos;
		}
		else if (event.key.keysym.sym == SDLK_y) {
			glm::mat3 rotationMat = makeYRotationMatrix(1.0);
			camera.cameraPos = rotationMat * camera.cameraPos;
		}
		else if (event.key.keysym.sym == SDLK_t) { //tilt
			glm::mat3 rotationMat = makeXRotationMatrix(-1.0);
			camera.orientation = rotationMat * camera.orientation;
		}
		else if (event.key.keysym.sym == SDLK_p) { //pan
			glm::mat3 rotationMat = makeYRotationMatrix(1.0);
			camera.orientation = camera.orientation * rotationMat;
		}
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
		else if (event.key.keysym.sym == SDLK_l) {
			std::cout << "l" << std::endl;
			CanvasPoint v0(rand() % 320, rand() % 240);
			TexturePoint t0(round(rand() % 480), round(rand() % 395));
			v0.texturePoint = t0;
			CanvasPoint v1(rand() % 320, rand() % 240);	
			TexturePoint t1(round(rand() % 480), round(rand() % 395));
			v1.texturePoint = t1;
			CanvasPoint v2(rand() % 320, rand() % 240);
			TexturePoint t2(round(rand() % 480), round(rand() % 395));
			v2.texturePoint = t2;
			CanvasTriangle texturedTri(v0, v1, v2);
			texturedTriangles.push_back(texturedTri);

		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

void clearDepthBuffer() {
	for (size_t i = 0; i < depthBuffer.size(); i++)
	{
		for (size_t j = 0; j < depthBuffer[i].size(); j++)
		{
			depthBuffer[i][j] = 0.0;
		}
	}
}

int main(int argc, char *argv[]) {

	glm::vec3 camPos (0, 0, 2.0);
	camera.cameraPos = camPos;
	camera.focalLen = 1.0;
	glm::vec3 rightOrientation(1, 0, 0);
	glm::vec3 upOrientation(0, 1, 0);
	glm::vec3 forwardsOrientation(0, 0, 1);
	glm::mat3 cameraOrientation(rightOrientation, upOrientation, forwardsOrientation);
	camera.orientation = cameraOrientation;

	parseMTL("cornell-box.mtl");
	parseOBJ("cornell-box.obj");

	parseTexture("texture.ppm");

	
	/*for (size_t i = 0; i < texturePixels2D.size(); i++)
	{
		for (size_t j = 0; j < texturePixels2D[i].size(); j++)
		{
			std::cout << texturePixels2D[i][j] << " ";
		}
		std::cout << std::endl;
	}*/

	//std::vector<std::vector<float>> depthBuffer;
	

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	
	for (size_t i = 0; i < WIDTH; i++)
	{
		std::vector<float> row;
		for (size_t j = 0; j < HEIGHT; j++)
		{
			row.push_back(0.0);
		}
		depthBuffer.push_back(row);
	}
	

	CanvasPoint p1(160, 10);
	TexturePoint tp1(195, 5);
	p1.texturePoint = tp1;

	CanvasPoint p2(300, 230);
	TexturePoint tp2(395, 380);
	p2.texturePoint = tp2;

	CanvasPoint p3(10, 150);
	TexturePoint tp3(65, 330);
	p3.texturePoint = tp3;

	CanvasTriangle texturedTri(p1, p2, p3);
	texturedTriangles.push_back(texturedTri);

	while (true) {
		clearDepthBuffer();
		//std::cout << glm::to_string(camera.cameraPos) << std::endl;
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		//drawStrokedTriangle(window, triangle, colour);
		//drawFilledTriangle(window, triangle, colour);
		drawRasterised(window);
		//drawWireframe(window);
		//displayTexture(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		
		window.renderFrame();
	}
}
