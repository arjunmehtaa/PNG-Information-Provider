/* PNG Additional Methods */
/* Author : Arjun Mehta */

/* Include required header files. */
#include <string.h>
#include <arpa/inet.h>

int is_png(FILE* file, size_t n){ /* Checks if inputted file is PNG.*/
  
	/* Declare required variables. */
	unsigned char data[PNG_SIG_SIZE];
	unsigned char png_identifier[PNG_SIG_SIZE] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
	U8 *buf = data;
  
	/* Read file data into buffer. */
	if(file != NULL){
		fread(buf, PNG_SIG_SIZE, 1, file);
	}
  
	/* Check if it matches PNG signature. */
	if (memcmp(png_identifier, buf, n)==0){
		return 0;
	} else{
		return -1;
	}
}

int get_png_height(FILE* file){ /* Returns height of inputted PNG file. */
  
	/* Declare variable and check file. */
	unsigned int height;
	if(file == NULL){
		return -1;
	}
  
	/* Set required position in file and read next four bytes. */
	fseek(file,20,SEEK_SET);
	fread((char*)&height,CHUNK_LEN_SIZE,1,file);
  
	/* Convert to host order and return. */
	return ntohl(height);
}

int get_png_width(FILE* file){ /* Returns width of inputted PNG file. */
  
	/* Declare variable and check file. */
	unsigned int width;
	if(file == NULL){
		return -1;
	}
  
	/* Set required position in file and read next four bytes. */
	fseek(file,16,SEEK_SET);
	fread((char*)&width,CHUNK_LEN_SIZE,1,file);
  
	/* Convert to host order and return. */
	return ntohl(width);
}

int get_four_byte_val(FILE* file,int offset){ /* Returns 4 byte value from inputted file at given offset. */
  
	/* Declare variable and check file. */
	unsigned int value;
	if(file == NULL){
		return -1;
	}
  
	/* Set required position in file and read next four bytes. */
	fseek(file,offset,SEEK_SET);
	fread((char*)&value,CHUNK_LEN_SIZE,1,file);
  
	/* Convert to host order and return. */
	return ntohl(value);
}
