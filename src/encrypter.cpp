
#include "encrypter.h"

using namespace std;


// save the raw binary representation of the character into the pixel object as follows:
// - character = aabb ccdd
// - pixel=>red = xxxx ccdd
// - pixel=>green = xxxx aabb
extern "C" void encode_bits(int& pixelAddress, int& charAdress)
{
	__asm
	{
		mov		esi, charAdress
		mov		edi, pixelAddress

		// get the low four bits of the character
		movzx	eax, BYTE PTR[esi]
		and 	eax, 0Fh

		// set the low four of the pixel=>red 
		movzx	ebx, BYTE PTR[edi]
		and 	ebx, 0F0h
		or 		eax, ebx

		// override the character into the pixels_array
		mov		BYTE PTR[edi], al

		/************************************************/

		// get the high four bits of the character
		movzx	eax, BYTE PTR[esi]
		and 	eax, 0F0h

		// set the high four of the pixel=>green 
		shr		eax, 4
		movzx	ebx, BYTE PTR[edi + 1]
		and 	ebx, 0F0h
		or 		eax, ebx

		// override the character into the pixels_array
		mov		BYTE PTR[edi + 1], al
	}
}


// take the raw binary representation of the pixel object and then reconstruct the character as follows:
// - pixel=>red = xxxx ccdd
// - pixel=>green = xxxx aabb
// - character = aabb ccdd
extern "C" void decode_bits(int& pixelAddress, int& charAdress)
{
	__asm
	{
		mov		esi, pixelAddress
		mov		edi, charAdress

		// get the low four bits of the pixel=>red  
		movzx	eax, BYTE PTR[esi]
		and 	eax, 0Fh

		// set the low four bits of the character
		movzx	ebx, BYTE PTR[edi]
		and 	ebx, 0F0h
		or 		eax, ebx

		// override the low four bits of the character 
		mov		BYTE PTR[edi], al

		/************************************************/

		// get the high four bits of the pixel=>green  
		movzx	eax, BYTE PTR[esi + 1]
		and 	eax, 0Fh

		// set the high four bits of the character 
		shl		eax, 4
		movzx	ebx, BYTE PTR[edi]
		and 	ebx, 0Fh
		or 		eax, ebx

		// override the high four bits of the character 
		mov		BYTE PTR[edi], al
	}
}


// Encrypt a text using in-line assembly 
void encrypt_message(BMP& image, string msg)
{
	char* msg_address = &msg[0];
	int msg_len = msg.length();

	RGBQUAD* pixels_array_address = image.pixels_array;
	int pixels_array_size = image.getSize();

	__asm
	{
		xor		ecx, ecx	// counter for the message string
		xor		edx, edx	// counter for the pixels_array

		// replace the first four bytes in the pixel_array 
		// with the length of the message that is going to be encrpyted
		mov		edi, pixels_array_address
		mov		ebx, msg_len
		mov		DWORD PTR[edi], ebx

		// skip the first two pixels
		add		edx, 6

		// itrate through the pixels map
		startLoop:
			cmp		ecx, msg_len
			jge		endLoop

			mov		esi, msg_address
			mov		edi, pixels_array_address

			// load address of the pixel to EAX 
			mov		eax, edi
			add		eax, edx

			// load address of the character to EBX
			mov		ebx, esi
			add		ebx, ecx

			// save current registers values on stack
			push	ecx
			push	edx

			// encodes character into the pixel
			push	ebx
			push	eax
			call	encode_bits
			add		esp, 8

			// restore registers values from stack
			pop		edx
			pop		ecx


			inc		ecx				// move to the next letter in the message
			add		edx, 3			// move to the next pixle in the pixels_array 
			jmp		startLoop

		endLoop :
			// exit loop
	}
}


// Decrypts a text using in-line assembly 
char* decrypt_message(BMP& image)
{
	RGBQUAD* pixels_array_address = image.pixels_array;
	int pixels_array_size = image.getSize();

	char* msg_address = new char[pixels_array_size];
	int msg_len;

	__asm
	{
		xor		ecx, ecx	// counter for the pixels_array
		xor		edx, edx	// counter for the message string

		// copy the length of the encrypted message stored in the image 
		// from the first four bytes in the pixel_array
		mov		esi, pixels_array_address
		mov		ebx, DWORD PTR[esi]
		mov		msg_len, ebx

		// skip the first two pixels
		add		ecx, 6

		// itrate through the pixels array
		startLoop :
			cmp		edx, msg_len
			jge		endLoop

			mov		esi, pixels_array_address
			mov		edi, msg_address

			// load address of the character to EBX
			mov		ebx, edi
			add		ebx, edx

			// load address of the pixel to EAX 
			mov		eax, esi
			add		eax, ecx

			// save current registers values on stack
			push	ecx
			push	edx

			// decodes character
			push	ebx
			push	eax
			call	decode_bits
			add		esp, 8

			// restore registers values from stack
			pop		edx
			pop		ecx

			inc		edx				// move to the next letter in the message
			add		ecx, 3			// move to the next pixle in the pixels_array 
			jmp		startLoop

		endLoop :
			mov		BYTE PTR[edi + edx], 0
	}

	return msg_address;
}
