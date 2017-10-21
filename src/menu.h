

#ifndef MENU_H
#define MENU_H

#include "bmp.h"
#include "encrypter.h"

enum MENU_OPTIONS
{
	EXIT	 = 0,
	ENCRYPT  = 1,
	DECRYPT  = 2,
	EXPORT	 = 3,
	TERMINAL = 4,
	TEXTFILE = 5
};

int prompt_main_menu();

int prompt_msg_menu();

void get_img(BMP& image, std::string src_filename);

void make_img(BMP& image, std::string dest_filename);

bool read_data(std::string& src_filename, std::string& str);

void write_data(std::string& dest_filename, std::string& text);

void get_msg(std::string& src_filename, std::string& msg);

void export_image_grid_to_csv_file(BMP& image);

#endif 
