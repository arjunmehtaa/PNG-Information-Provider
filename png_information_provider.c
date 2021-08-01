/* PNG Information Provider */
/* Author : Arjun Mehta */

/* Include required header files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png_additional_methods.c"
#include <arpa/inet.h>

/* Define constants. */
#define LEN_TYPE_BYTE_COUNT 8

char* get_file_name(char* path_name){ /* Returns file name from path. */

	/* Check if path_name is just the file name. */
	if(strrchr(path_name,'/')!=NULL){

		/* If it is the path, return the trimmed file name. */
		return strrchr(path_name,'/')+1;
	}

	/* If it is already just the file name, return it. */
	return path_name;
}

void find_expected_crc(FILE* file,int offset,int *count,int index,int *result){ /* Sets result to expected crc value in index-th chunk. */

	/* Loop three times for three chunks, IHDR, IDAT and IEND. */
	while((*count)<=3){

		/* Initialize variables. */
		int chunk_data_len = get_four_byte_val(file,offset);
		int updatedoffset = offset+chunk_data_len+LEN_TYPE_BYTE_COUNT;
		int crc_exp = get_four_byte_val(file,updatedoffset);

		/* If crc value is from expected chunk, set it to result. */
		if((*count) == index){
			(*result) = crc_exp;
		}

		/* Increase chunk count and update offset. */
		(*count)++;
		updatedoffset+=4;

		/*Call find_expected_crc() with updated offset. */
		find_expected_crc(file,updatedoffset,count,index,result);
	}
}

void find_calculated_crc(FILE* file,int offset,int *count,int index,int *result){ /* Sets result to calculated crc value for index-th chunk. */

	/* Loop three times for three chunks, IHDR, IDAT and IEND. */	
	while((*count)<=3){

		/* Initialize variables. */
		int chunk_data_len = get_four_byte_val(file,offset);
		int updatedoffset = offset + 4;
		int bytes_to_read = 4 + chunk_data_len;
		U8 buf[bytes_to_read];

		/* Read the type and data bytes into buf. */
		fseek(file,updatedoffset,SEEK_SET);
		fread(buf,bytes_to_read,1,file);

		/* Set calculated crc value by passing buf in crc(). */
		U32 crc_calc  = crc(buf,bytes_to_read);
		if((*count)==index){
			(*result) = crc_calc;
		}

		/* Increase chunk count and update offset. */
		(*count)++;
		updatedoffset+=(bytes_to_read+4);

		/*Call find_calculated_crc() with updated offset. */
		find_calculated_crc(file,updatedoffset,count,index,result);
	}
}

void check_crc(FILE* file){ /* Compares computed and expected crc values. */

	/* Initialize variables. */
	int count = 1;
	int exp_result = 0;
	int calc_result = 0;
	char arr[3][5] = {"IHDR","IDAT","IEND"};

	/* Loop three times for three chunks, IHDR, IDAT and IEND. */
	for(int i=0;i<3;i++){

		/* Find expected crc value for i-th chunk, reset count. */
		find_expected_crc(file,8,&count,(i+1),&exp_result);
		count=1;

		/* Find calculated crc value for i-th chunk, reset count. */
		find_calculated_crc(file,8,&count,(i+1),&calc_result);
		count = 1;

		/* If computed and expected values do not match, print error and break from loop. */
		if(exp_result!=calc_result){
			printf("%s chunk CRC error: computed %x, expected %x\n",arr[i],calc_result,exp_result);
			break;
		}
	}
}

int main (int argc, char **argv){ /* Main function. */

	/* Open the file passed in the argument. */
	FILE* file = fopen(argv[1],"rb");

	/* Check if the file is PNG. */
	if(is_png(file,PNG_SIG_SIZE)==0){

		/* If it is PNG, print width and height. */
		if(file!=NULL){
			printf("%s: %d x %d\n", get_file_name(argv[1]), get_png_width(file), get_png_height(file));
	
			/* Check crc in case of corrupted file. */
			check_crc(file);
		}
	} else{

		/* If it is not a PNG file, output accordingly. */
		printf("%s: Not a PNG file\n",get_file_name(argv[1]));
	}

	/* Close the opened file and return. */
	fclose(file);
	return 0;
}
