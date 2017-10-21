

#include "menu.h"

using namespace std;

// Displays app menu to users and gets inputs
int prompt_main_menu()
{
	cout << "\n---------------------------------"
		 << "\n| BMP IMAGE MASTER - MAIN MENU: |"
		 << "\n---------------------------------\n";

	cout << "[1] Encrypt message \n"
		 << "[2] Decrypt message \n"
		 << "[3] Export image grid \n"
		 << "[0] Exit \n"
		 << "-------------------------------------- \n"
		 << "Please Enter a menu number :: e.g: 1 \n"
		 << "Menu Option:: ";

	int option = 0;
	cin >> option;

	// input validation
	while (cin.fail() || option < 0 || option > 3)
	{
		cout << "\n Error: Please enter an integer between (0 - 3): ";
		cout << "\n Menu Option: ";

		cin.clear();
		cin.ignore(256, '\n');
		cin >> option;
	}
	cout << endl;

	return option;
}


// Display importing/exporting message-options 
int prompt_msg_menu()
{
	cout << "\nWould you like to\n"
		<< "\t [4] Read/Write your message directly using the terminal \n" 
		<< "\t [5] Import/Export the message from/to a (.txt) text file \n"
		<< "-------------------------------------- \n"
		<< "Please Enter a menu number :: e.g: 4 \n"
		<< "Menu Option:: ";

	int option = 0;
	cin >> option;

	// input validation
	while (cin.fail() || option < 4 || option > 5)
	{
		cout << "\n Error: Please enter an integer between (4 - 5): ";
		cout << "\n Menu Option: ";

		cin.clear();
		cin.ignore(256, '\n');
		cin >> option;
	}
	cout << endl;

	return option;
}


// Get (.bmp) image name from the user and load its bitmap
void get_img(BMP& image, string src_filename)
{
	// get the name of the text file that has the message, and read in its content
	cout << "\n\t Please enter your image name (ex: cats.bmp):\t";
	cin >> src_filename;

	bool data_was_read = image.loadBitmap(&src_filename[0]);

	// input validation
	while (data_was_read != true)
	{
		cout << "\n\t Please enter a valid filename: ";
		cin.clear();
		cin >> src_filename;

		data_was_read = image.loadBitmap(&src_filename[0]);
	}
}


// Get (.bmp) image name for the image that has the encrypted message and make its bitmap
void make_img(BMP& image, string dest_filename)
{
	// get the name of the desired file to output the encrypted message to
	cout << "\n\t Please enter a image name for the encrypted message (ex: msg.bmp):\t";
	cin >> dest_filename;

	image.buildBitmap(&dest_filename[0]);
}


// Reads data from a (.txt) file, and loads it into a string
bool read_data(string& src_filename, string& str)
{
	ifstream input;
	input.open(src_filename);

	string line;
	if (input.good())
	{
		while (!input.eof())
		{
			getline(input, line);
			str += line;
		}
	}
	else
	{
		cout << "\nError: File Doesn't exit. . . \n";
		return false;
	}

	input.close();

	return true;
}


// Get (.txt) file name that contains the message and load its content
void get_msg(string& src_filename, string& msg)
{
	// get the name of the text file that has the message, and read in its content
	cout << "\n\t Please enter file name that contains your message (ex: msg.txt):\t";
	cin >> src_filename;

	bool data_was_read = read_data(src_filename, msg);

	// input validation
	while (data_was_read != true)
	{
		cout << "\n\t Please enter a valid filename: ";
		cin.clear();
		cin >> src_filename;

		data_was_read = read_data(src_filename, msg);
	}
}


// Writes out text to the specified (.txt) file	           			      
void write_data(string& dest_filename, string& text)
{
	// get the name of the desired file to output the encrypted message to
	cout << "\n\t Please enter a file name to save your decrypted message (ex: d_msg.txt):\t";
	cin >> dest_filename;

	ofstream output;
	output.open(dest_filename);

	output << text;

	output.close();
}


// to export all pixels and colors information of an image to a (.csv) file
void export_image_grid_to_csv_file(BMP& image)
{
	RGBQUAD** grid = image.createImageGrid();

	image.exportImageGrid(grid);

	// free memory
	for (int i = 0; i < image.getHeight(); i++)
	{
		delete[] grid[i];
		grid[i] = 0;
	}
	delete[] grid;
	grid = 0;
}