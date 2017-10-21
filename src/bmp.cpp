
#include "bmp.h"
using namespace std;

//Constructor
BMP::BMP()
{
	width = 0;
	height = 0;
	dataSize = 0;
	linePaddingSize = 0;
}


//Destructor
BMP :: ~BMP()
{
	delete[] bitMap;
	bitMap = 0;

	delete[] pixels_array;
	pixels_array = 0;
}


// read a BMP image
bool BMP::loadBitmap(char* imgName)
{
	FILE* image;

	// opining the image to read in data using binary mode ('r' read in 'b' binary mode)
	fopen_s(&image, imgName, "rb"); 

	if (image == NULL)
	{
		cout << "Error!, could not open the image...";
		return false;
	}

	// reading the header information into an unsigned char array
	/* fread( pointer to a block of memory,
			  size of each element to be read,
			  number of elements to be read,
			  a pointer to a FILE object stream )
	*/
	fread(header, sizeof(unsigned char), 54, image);	
	

	unsigned char* header_address = &header[0];
	int* width_address = &width;
	int* height_address = &height;
	int* dataSize_address = &dataSize;

	__asm
	{
		mov		esi, header_address

	// Extracting the width of the image from the header array
	/*******************************************/
		mov		edi, width_address

		movzx	eax, BYTE PTR[esi + 18]
		mov		BYTE PTR[edi], al

		movzx	eax, BYTE PTR[esi + 19]
		mov		BYTE PTR[edi + 1], al

		movzx	eax, BYTE PTR[esi + 20]
		mov		BYTE PTR[edi + 2], al

		movzx	eax, BYTE PTR[esi + 21]
		mov		BYTE PTR[edi + 3], al

	// Extracting the height of the image from the header array
	/*******************************************/
		mov		edi, height_address

		movzx	eax, BYTE PTR[esi + 22]
		mov		BYTE PTR[edi], al

		movzx	eax, BYTE PTR[esi + 23]
		mov		BYTE PTR[edi + 1], al

		movzx	eax, BYTE PTR[esi + 24]
		mov		BYTE PTR[edi + 2], al

		movzx	eax, BYTE PTR[esi + 25]
		mov		BYTE PTR[edi + 3], al

	// Extracting the size of the image from the header array
	/*******************************************/
		mov		edi, dataSize_address

		movzx	eax, BYTE PTR[esi + 34]
		mov		BYTE PTR[edi], al

		movzx	eax, BYTE PTR[esi + 35]
		mov		BYTE PTR[edi + 1], al

		movzx	eax, BYTE PTR[esi + 36]
		mov		BYTE PTR[edi + 2], al

		movzx	eax, BYTE PTR[esi + 37]
		mov		BYTE PTR[edi + 3], al
	}

	// padding bytes might be added to the end of each line since the number of bytes
	// in a row will always be added up to the next multiple of 4.
	int* padding_address = &linePaddingSize;
	 
	__asm
	{	// line padding = [(width * 3) + 3] & (~3)	
		mov		esi, width_address
		mov		eax, [esi]
		mov		ebx, 3	 
		mul		ebx		// (width * 3) because each pixel will hold 3 colors values(Red, Green, Blue)
		add		eax, 3
		not		ebx
		and		eax, ebx
		mov		ecx, padding_address
		mov		DWORD PTR [ecx], eax
	}

	// dynamically allcocating an array to hold the raw binary bitmap including the image header
	bitMap = new unsigned char[linePaddingSize]; 

	// dynamically allcocating an array to hold all pixels and colors information without the image header
	pixels_array = new RGBQUAD[width*height];


	int pixel = 0;	// pixel counter
	for (int row = 0; row < height; row++)
	{
		fread(bitMap, sizeof(unsigned char), linePaddingSize, image);
	/*
		Each pixel consists of three color values (r,g,b), however, they're are going to be read 
		in reverse order, like (b, g, r) instead of (r, g, b) - becuase we're reading the bitmap 
		from the bottom-left cornor of the image to the top as follows

		***********************************  **********************************  
		*		      Pexiel #1			  *	 *		      Pexiel #2			  *	 
		*	--------  ---------  -------  *	 *	--------  ---------  -------  *	 ...........
		*	| blue |  | green |  | Red |  *	 *	| blue |  | green |  | Red |  *	 
		*	--------  ---------	 -------  *	 *	--------  ---------	 -------  *  
		***********************************  **********************************  
	*/

		for (int col = 0; col < width * 3; col += 3)
		{
			pixels_array[pixel].blue = bitMap[col];
			pixels_array[pixel].green = bitMap[col + 1];
			pixels_array[pixel].red = bitMap[col + 2];

			//cout << " R: " << (int)pixels_array[pixel].red 
			//	   << " G: " << (int)pixels_array[pixel].green
			//	   << " B: " << (int)pixels_array[pixel].blue << endl;

			pixel++;	// moving to the next pixel
		}
	}

	fclose(image); // closeing the input file

	return true;
}


// create a copy of the image
void BMP::buildBitmap(char* imgName)
{
	unsigned char* body = NULL;

	// an array to hold all the bitmap information before creating the new image
	body = new unsigned char[dataSize];

	// the case where there's no padding needed (when the width is a multiple of 4)
	if (width % 4 == 0)	
	{
		// to keep track of every pixel in the body array in order to move to the next pixel properly
		int px = 0;		

		for (int pixel = 0; pixel < height * width; pixel++)
		{
			// getting colors information for each pixel
			body[px]	 = pixels_array[pixel].blue;
			body[px + 1] = pixels_array[pixel].green;
			body[px + 2] = pixels_array[pixel].red;

			px += 3;	// moving to the next pixel
		}
	}
	else	
	{ // the case where padding is needed
	  
	    // to keep track of every pixel in the body array in order to move to the next pixel properly
		int px = 0;
		int line = 0;

		for (int pixel = 0; pixel < height * width; pixel++)
		{	
			if (pixel == ( width * (line + 1) ) )
			{
				line++;		// moving to the next line in the image file
				px = 0;		// reset pixels' counter to skip padding bits
			}

			// getting colors information for each pixel
			body[(linePaddingSize * line) + (px)]	  = pixels_array[pixel].blue;
			body[(linePaddingSize * line) + (px + 1)] = pixels_array[pixel].green;
			body[(linePaddingSize * line) + (px + 2)] = pixels_array[pixel].red;
		
			px += 3;	// moving to the next pixel	
		}

	}

	// writing the image out
	FILE* newImage;

	// opining the image to write in data using binary mode ('w' write in 'b' binary mode)
	fopen_s(&newImage, imgName, "wb");
	
	// writing the header to the new file
	fwrite(&header, sizeof(unsigned char), 54, newImage);

	// writing the bitmap to the new file
	fwrite(body, sizeof(unsigned char), dataSize, newImage);

	fclose(newImage); // closeing the input file

  // getting rid of the body array
	delete[] body;
	body = 0;
}


// to create a 2D array that represents all pixels and colors information of the image
RGBQUAD** BMP::createImageGrid()
{
	// Dynamically allocating memory for the image grid 
	RGBQUAD** grid = new RGBQUAD*[height];
	for (int i = 0; i < height; i++)
	{
		grid[i] = new RGBQUAD[width];
	}

	// loading pixels into the grid
	int px = 0;
	for (int x = 0; x < height; x++)
	{
		for (int y = 0; y < width; y++)
		{
			grid[x][y] = pixels_array[px];
			px++;
		}
	}

	return grid;
}


// to export all pixels and colors information of an image to a (.csv) file
void BMP::exportImageGrid(RGBQUAD** grid)
{
	ofstream csvFile;

	csvFile.open("image_grid.csv");
	
	// loading pixels into the grid
	for (int x = 0; x < height; x++)
	{
		for (int y = 0; y < width; y++)
		{			 
			csvFile << "("	  << (int)grid[x][y].red
					<< " :: " << (int)grid[x][y].green
					<< " :: " << (int)grid[x][y].blue 
					<< ")" << ", ";
		}

		csvFile << "\n";
	}

	csvFile.close();
}
