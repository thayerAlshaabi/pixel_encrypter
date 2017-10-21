

/*
Author: Thaer Alshaabi
Class : CSI-370 - Computer Architecture

Description :
	The ultimate goal of the project is to have a program that would allow users
	to encrypt/decrypt a message into an (.bmp) image. Fairly big part of the
	program is written in C++, where highly oriented structure would
	be appropriate due to the complexity of bitmaps underlying architecture,
	like storing the image in a map of pixel-objects for both easier access to
	data, and to facilitate encryption/decryption process. On the other hand,
	the program also includes in-line Assembly {Intel-syntax} for low-level programming,
	like bits manipulations, and encrypting/decrypting text.
	Generally, the application relies on the C++ implementation to drive the flow of program 
	providing a (UI), and a menu system, which would interact with the rest of
	functions to accomplish the required tasks.
*/


#include "menu.h"

using namespace std;


// Main app driver 
int main()
{
	BMP image;
	string src_filename, dest_filename, msg;

	int option;
	int msg_option;

	do
	{
		option = prompt_main_menu();

		switch (option)
		{
			case ENCRYPT:
			{
				get_img(image, src_filename);
				
				msg_option = prompt_msg_menu();

				switch (msg_option)
				{
					case TERMINAL:
					{
						cout << "\nPlease enter your message:"
							<< "\n--------------------------------------\n";
						
						cin.ignore();
						getline(cin, msg);

						// just to ensure that the message is not too long to fit into the image
						while (msg.length() > (image.getWidth() * image.getHeight()) )
						{
							cout << "\nError: your message is too long to fit into the image!\n"
								<< "Please change your message or import a bigger image\n";

							get_img(image, src_filename);

							cout << "\nPlease enter your message:"
								<< "\n--------------------------------------\n";
							cin.ignore();
							msg.clear();
							getline(cin, msg);
						}

						cout << "\n--------------------------------------\n";

						break;
					}

					case TEXTFILE:
					{
						get_msg(src_filename, msg);
						
						// just to ensure that the message is not too long to fit into the image
						while (msg.length() > (image.getWidth() * image.getHeight()))
						{
							cout << "\nError: your message is too long to fit into the image!\n"
								<< "Please change your message or import a bigger image\n";

							msg.clear();
							get_img(image, src_filename);
							get_msg(src_filename, msg);
						}

						break;
					}

				} // end of msg_option switch

				// call the encryption function
				encrypt_message(image, msg);

				make_img(image, dest_filename);

				cout << "\nThe message was encrypted successfully" << endl;
				break;

			} // end of ENCRYPT case

			case DECRYPT:
			{
				get_img(image, src_filename);

				// call the decryption function
				string msg = decrypt_message(image);

				cout << "\nThe message was decrypted successfully" << endl;

				msg_option = prompt_msg_menu();

				switch (msg_option)
				{
					case TERMINAL:
					{
						cout << "\nMessage:"
							<< "\n--------------------------------------\n"
							<< msg
							<< "\n--------------------------------------\n";

						break;
					}

					case TEXTFILE:
					{
						write_data(dest_filename, msg);

						cout << "\nYour decrypted message was successfully saved to (" << dest_filename << ")\n";

						break;
					}
				}

				break;

			}  // end of DECRYPT case

			case EXPORT:
			{
				get_img(image, src_filename);

				export_image_grid_to_csv_file(image);

				cout << "\nThe image grid has been successfully exported to (image_grid.csv)" << endl;
			}

		} // end of option switch

	} while (option != EXIT);

	return 0;
}