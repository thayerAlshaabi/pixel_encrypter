# STEGANOGRAPHY
The ultimate goal of the project is to have a program that would allow users to encrypt/decrypt a message into an (.bmp) image. Fairly
big part of the program is written in C++, where highly oriented structure is appropriate due to the complexity of bitmap's underlying
architecture, like storing the image in a map of pixel-objects for both easier access to data, and to facilitate encryption/decryption process. On the other hand, the program also includes in-line assembly written in Intel-syntax for low-level programming, like bits manipulations, and encrypting/decrypting text into the image. Generally, the application will rely on the C++ implementation to drive the flow of program providing a User interface(UI), and a menu system, which would interact with the rest of functions to accomplish the required tasks.


## Building & Running
This program only works on windows, to test the program please download the repo, and run `steganography.exe`

 
## Implementation

#### Bitmap-File Format

Generally, (.bmp) file format { which is also commonly known as device-independent bitmap(DIB) { is the standard format for storing bitmap images throughout most applications, particularly on Windows, but also many other
operating systems. Unfortunately, there are many diferent implementations of BMP-file formats
due to the fact that bitmap-format was not initially designed to be portable, but rather easy and quick to use with Windows API until Microsoft made it almost fully compatible with most platforms in the 1990's. Hence, it is
relatively common to and some (.bmp) images, or bitmap-files in general, that can not be read on all platforms.

#### Customized Bitmap Structure
As an attempt to keep the project cross-platform I neither used the Windows-API, nor any structures from Windows MSDN or other implementations. Instead, I have designed a collection of structures just for the propose and
scope of this project. They are mainly inspired by the offcial Windows documentation of bitmap-formats.
The customized bitmap class contains a [header array], and a [pixels array]. The header array combines both the original bitmap-file header (14-bytes) and bitmap-information header(40-bytes). Further, the pixels array is a dynamic one dimensional array of pixel-objects where each pixel consists of three bytes, and each byte represents a color in the RGB-QUAD(Red, Green, Blue) stored in memory as unsigned characters.

![figure1](https://github.com/thaer275/steganography/blob/master/figure1.PNG)
![figure2](https://github.com/thaer275/steganography/blob/master/figure2.PNG)

#### Loading Bitmaps Images In Memory
Originally, BMP-format was designed to be incorporated with Intel processors, thus it uses Little-Endian format by default to order the individual bytes internally in memory. That being said, we need to be a bit more careful when implementing bitmaps on different computer architecture as it would not be an issue when working on Intel processors, however, it would be certainly a critical factor when working with other systems that may use Big-Endian format by default. 

The best way to read bitmap-files is to open them in binary mode. Thereafter, the first section that has to be read is the header, which is the most important part of the file as it contains all necessary information about the image like size, width, height, and the colors table, etc... The header could be read using the C standard function and saved into the header array in the BMP class. Then, using in-line Assembly, we may extract the size, width, height of the image from the header array, and save them into the appropriate variable to use them later on. 

After reading the header, there is a very crucial step that needs to be done, which is calculating the padding as the pixel structure is often stored with an extra byte of padding throughout the colors table since the number of bytes in a row will always be added up to the next multiple of 4.

The code below shows some in-line Assembly to calculate the padding value:
```
  __asm
  {	// line padding = [(width * 3) + 3] & (~3)	
    mov		esi, width_address
    mov		eax, [esi]
    mov		ebx, 3	 
    mul		ebx	
    add		eax, 3
    not		ebx
    and		eax, ebx
    mov		ecx, padding_address
    mov		DWORD PTR [ecx], eax
  }
```

Next step is to read the colors table, or the actual image grid of pixels and save it into memory. It is worth noting that pixels are, in fact, saved in reversed order in the bitmap, meaning that the first pixel that is going to be read is actually the pixel located on the lower-left corner of the image.

![figure3](https://github.com/thaer275/steganography/blob/master/figure3.png)

#### Re-creating Bitmaps Images
Not surprisingly, the process of re-creating the image using its raw binary data is simpler process than loading the image { as far as the exporting operations get executed properly in the right order. However, while padding bytes get abstracted away when loading bitmap images into memory, because they are not needed in the importing process as they only represent empty spaces, they are very important in the exporting process. There is an extra step, which involves appending the padding bytes to the colors table in order to re-construct the image correctly. More importantly, padding will only be needed when the width of the image is not a multiple of 4.


#### Encrypting Messages
The idea behind the project was to develop a program that allows users to hide some text inside the bitmap of an image without distorting or destroying the image visually.

My first intuition was to replace one of the colors inside each pixel with the ASCII value of each character from the message benefiting from the fact that both characters and color's values are stored internally in memory as a value ranges between (0) to (255). However, it turned out that even changing only one of the colors value in a pixel consisting of RGB values would damage the image signifcantly as the viewer would immediately realize the diference in colors where characters were encoded in the pixels of image even with spreading the characters throughout the image. 

Thus, i used a slightly different approach, yet more complected as it involves some bit manipulations at the very low level. I decided to store each character into two different bytes within each pixel by splitting of the character bits into two sets. The low four bits of the character would be stored in the low four bits of the first color value in the pixel, which is Red in this case. And, the high four bits of the character would be stored in the low four bits of the second color value in the pixel, which is Green. The figure below illustrates the encoding process showing how bits are manipulated, and shifted before storing them accordingly into the bitmap:

![figure4](https://github.com/thaer275/steganography/blob/master/figure4.png)


The code below shows some in-line Assembly copying the ASCII value of one character from the message string, and storing it within two bytes of a single pixel in the image:

```
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
```

The results showed drastic improvements as the two images were visually identical, except the fact that one of the images has an encrypted message encoded within the bitmap structure. Perhaps, the only restriction of this methodology is that the message's length would be limited to the total number of pixels available in the image as each character is going to be encoded within a single pixel. However, with an image big enough you could certainly store a very long letter of multiple pages.
		
For demonstration purposes, i ran the program on the image shown on the left side below. The image on the right side is indeed an identical copy of the original image, with the exception that it has an encrypted message -- which is approximately a (1,000) character long -- encoded carefully within its layout without distorting the image, or causing any damage to the image visually.
			

![figure5](https://github.com/thaer275/steganography/blob/master/figure5.png)


#### Decrypting Messages
Intentionally, the length of the encrypted message is actually stored within the first two pixels of the image to facilitate the decoding process. This technique would certainly save both time and computational power as we only need to import the bytes that have encoded bites in order to decrypt the message, rather than loading the entire bitmap of the image. Similar to the encoding process, in the decoding process we need to retrieve the low four bits of the character from the low four bits of the first color value in the pixel, and the high four bits of the character from the low four bits of the second color value in the pixel.
			
The figure below illustrates the decoding process showing how bits get retrieved from the bitmap:

![figure6](https://github.com/thaer275/steganography/blob/master/figure6.png)

The code below shows some in-line Assembly describing the decrypting process: 
```
// Retrieving The Length of The Encrypted Message
  mov		esi, pixels_array_address
  mov		ebx, DWORD PTR[esi]
  mov		msg_len, ebx	
  
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

// move to the next letter in the message
    inc		edx	

// move to the next pixle in the pixels_array		
    add		ecx, 3		
    jmp		startLoop

  endLoop :
    // add null to the end of the string
    mov		BYTE PTR[edi + edx], 0
```

### Conclusion
The UI of the program is very intuitive and self-explanatory including a menu system driven by the main function. Additionally, I included an extra function that gives the user the ability to export two dimensional pixels array into a Comma-Separated Values (CSV) file just in case if the user wants to look at all pixels and color values cleanly without using the terminal. The program also gives the user the option of reading and writing messages directly using the terminal, or via (.txt) text files. 

Ultimately, the goal of the project was not necessarily to create a super secured encryption, but rather an experiment of using a combination of highly oriented language like C++, and a very low level programming language like Assembly to achieve a particular task efficiently. It was a very challenging experiment that required comprehensive knowledge of memory management and bits manipulations, as well as, a clear understanding of BMP-file format, specifically, with very few sources and documentations online for operations on bitmaps for different reasons discussed early in this technical report.


## License
Licensed under the MIT License (see `LICENSE`).
