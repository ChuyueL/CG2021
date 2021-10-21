#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define WIDTH 320
#define HEIGHT 240

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

	for (size_t i = 0; i < numberOfValues-1; i++)
	{
		currentVec += division;
		values.push_back(currentVec);
	}

	return values;

}

void draw(DrawingWindow& window) {
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

void drawGreyscale(DrawingWindow &window) {
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

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {

	std::vector<glm::vec3> result;
	glm::vec3 from(1, 4, 9.2);
	glm::vec3 to(4, 1, 9.8);
	result = interpolateThreeElementValues(from, to, 4);
	for (size_t i = 0; i < result.size(); i++) std::cout << glm::to_string(result[i]) << std::endl;
	std::cout << std::endl;

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		drawColour(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}

	

}
