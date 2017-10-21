

#ifndef BMP_H
#define BMP_H

#include <iostream>
#include <fstream>


struct RGBQUAD
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

/**********************************************************
- defining a (bmp) class to map the image to a 2D pixel-array  

************** Start BMP Class Definition **************/
class BMP
{
	private:
	/****************************** Private Members ***************************************/
		unsigned char header[54];				// an array to store header of the image 
												// File-Header(14), and Info-Header(40)
		unsigned char* bitMap = NULL;			// an array holding raw binary bitmap including the image header

		int width;								// width of the image
		int height;								// height of the image
		int dataSize;							// size of the image 
		int linePaddingSize;					// size of line padding into the image file 

	public:
	/****************************** Public Members ***************************************/

		// an array to hold all pixels and colors information without the image header
		struct RGBQUAD* pixels_array = NULL;		

		BMP();			// Constructor
		~BMP();			// Destructor

		// getters
		int getWidth()	{ return width; };
		int getHeight() { return height; };
		int getSize()	{ return dataSize; };

		// setters
		void setWidth(int w)  { width = w; };
		void setHeight(int h) { height = h; };

		// to read an image from a file
		bool loadBitmap(char* imgName);	 
		
		// to create a copy an image
		void buildBitmap(char* imgName);

		// to create a 2D array that represents all pixels and colors information of the image
		RGBQUAD** createImageGrid();

		// to export all pixels and colors information of an image to a (.csv) file
		void exportImageGrid(RGBQUAD** grid);
};
/************** END BMP Class Definition **************/

#endif 
