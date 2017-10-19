/*
============================================================================
Name        : Lab5C.c
Author      : Spencer Stapf
Version     :
Copyright   : Your copyright notice
Description : Hello World in C, Ansi-style
============================================================================
*/
//-n, -o,-s,-r,-h are all the functions
#include <stdio.h>
#include <stdlib.h>

void printHelp();
void do_offset(double*, int, double, char*);
void do_scale(double*, int, double, char*);

double getmean(double*, int);
double getmax(double*, int);

void do_statistics(double*, int, int);
void write_output(char*, double*, int, double);

double *loadArray(char*, int*, double*);

int main(int argc, char **argv)
{
	char* filename = (char *)malloc(sizeof(char) * 50);
	int rename_file = 0;
	char* new_filename;

	int input_filenumber;
	double input_scale_factor = 1.00;
	double input_offset_factor = 0.00;

	int output_statistics = 0;
	int output_center = 0;
	int output_normalized = 0;

	double signal_maxvalue;
	int signal_length;
	double* signal_values;

	// Look for errors in the input

	if (argc <= 2)
	{
		printf("Error. You didn't enter appropriate command line arguments.");
		printHelp();
		return 0;
	}

	// Loop through and process all the arguments
	int arg_count = 1;
	char* invalid_msg = (char *)malloc(sizeof(char) * 100);
	int invalid_inputs = 1;
	while (arg_count < argc)
	{
		// File Number - this one has to be there or it won't run
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'n'))
		{
			arg_count++;
			if (argv[arg_count][0] != '-')
			{
				input_filenumber = atoi(argv[arg_count]);
				sprintf(filename, "Raw_Data_%02d.txt", input_filenumber);
				invalid_inputs = 0;
				arg_count++;
				if (arg_count >= argc)
					break;
			}
			else
			{
				invalid_msg = "-n given without number";
				invalid_inputs = 1;
				break;
			}
		}

		// Offset value
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'o'))
		{
			arg_count++;
			input_offset_factor = atof(argv[arg_count]);
			arg_count++;
			if (arg_count >= argc)
				break;

			if (input_offset_factor == 0.00)
			{
				invalid_msg = "-o given without offset factor";
				invalid_inputs = 1;
				break;
			}
		}

		// Scale value
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 's'))
		{
			arg_count++;
			input_scale_factor = atof(argv[arg_count]);
			arg_count++;
			if (arg_count >= argc)
				break;

			if (input_scale_factor == 0.00)
			{
				invalid_msg = "-s given without scale factor";
				invalid_inputs = 1;
				break;
			}
		}

		// Rename Files
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'r'))
		{
			arg_count++;
			if (arg_count <= argc)
			{
				new_filename = (char *)malloc(sizeof(char) * 50);
				sprintf(new_filename, "%s.txt", argv[arg_count]);
				rename_file = 1;
				arg_count++;
				if (arg_count >= argc)
					break;
			}
			else
			{
				invalid_msg = "-r given without new filename";
				invalid_inputs = 1;
				break;
			}
		}


		// Help
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'h'))
		{
			arg_count++;
			printHelp();
			if (arg_count >= argc)
				break;
		}

		// Statistics
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'S'))
		{
			arg_count++;
			output_statistics = 1;
			if (arg_count >= argc)
				break;
		}

		// Normalized
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'N'))
		{
			arg_count++;
			output_normalized = 1;
			if (arg_count >= argc)
				break;
		}

		// Centered
		if ((argv[arg_count][0] == '-') && (argv[arg_count][1] == 'C'))
		{
			arg_count++;
			output_center = 1;
			if (arg_count >= argc)
				break;
		}
	}


	// exit the program if an error occurred in the inputs
	if (invalid_inputs)
	{
		printf("You entered invalid arguments:%s\n", invalid_msg);
		printHelp();
		return 0;
	}

	// looks like we have a good set of inputs, so proceed
	signal_values = loadArray(filename, &signal_length, &signal_maxvalue);

	// Scale
	if (input_scale_factor != 1.00)
	{
		char* filename = (char *)malloc(sizeof(char) * 50);
		sprintf(filename, "Scaled_data_%02d.txt", input_filenumber);
		do_scale(signal_values, signal_length, input_scale_factor, filename);
		free(filename);
	}

	// Offset
	if (input_offset_factor != 0.00)
	{
		char* filename = (char *)malloc(sizeof(char) * 50);
		sprintf(filename, "Offset_data_%02d.txt", input_filenumber);
		do_offset(signal_values, signal_length, input_offset_factor, filename);
		free(filename);
	}

	// Center
	if (output_center != 0)
	{
		char* filename = (char *)malloc(sizeof(char) * 50);
		sprintf(filename, "Centered_data_%02d.txt", input_filenumber);
		do_offset(signal_values, signal_length, getmean(signal_values, signal_length), filename);
		free(filename);
	}

	// Normalize
	if (output_center != 0)
	{
		char* filename = (char *)malloc(sizeof(char) * 50);
		sprintf(filename, "Normalized_data_%02d.txt", input_filenumber);
		do_scale(signal_values, signal_length, 1.00 / signal_maxvalue, filename);
		free(filename);
	}

	// recalculate the maximum value
	signal_maxvalue = getmax(signal_values, signal_length);

	// output the file
	if (rename_file != 0)
		write_output(new_filename, signal_values, signal_length, signal_maxvalue);
}


void printHelp(void)
{
	printf("The program should be called through use of commmand line arguments. ");
}

double *loadArray(char* file, int* length, double* max_val)
{
	// Open the file and check for empty failure case
	FILE* filePointer = fopen(file, "r");
	if (filePointer == NULL)
	{
		printf("Error opening input file. File pointer was equal to NULL.");
		return 0;
		//terminates program
	}

	// Read the length and max from the first line
	fscanf(filePointer, "%d %lf", length, max_val);

	// read the signal data into an array
	double* return_array = (double *)malloc(sizeof(double) * *length);
	int i = 0;
	for (i = 0; i < *length; i++)
	{
		fscanf(filePointer, "%lf", (return_array + i));
	}
	fclose(filePointer);

	return return_array;
}

// do_offset should take in the array of doubles, the length of the array and the offset value,
// then it should make a new array of doubles, fill it with the sum of and return it
void do_offset(double* array, int length, double offset_value, char* filename)
{
	double* array_changed = (double *)malloc(sizeof(double) * length);

	int i;
	for (i = 0; i < length; i++)
	{
		*(array_changed + i) = (*(array + i) + offset_value);//adds the value to every term
	}

	write_output(filename, array_changed, length, offset_value);

	return;
}

// do_scale should take and array of doubles, the length of the array and the scaling value
// it will return a new array of doubles that is the product of the original array an the scaling value
void do_scale(double* array, int length, double scale_value, char* filename)
{
	double* array_changed = (double *)malloc(sizeof(double) * length);

	int i;
	for (i = 0; i < length; i++)
	{
		*(array_changed + i) = (*(array + i) * scale_value);//multiplies the value by every term
	}

	write_output(filename, array_changed, length, scale_value);
	return;
}

void do_statistics(double* array, int length, int file_number)
{
	char* filename = (char *)malloc(sizeof(char) * 50);
	sprintf(filename, "Statistics_data_%d.txt", file_number);

	FILE* filePointer = fopen(filename, "w");

	if (filePointer == NULL) //making sure the input file exists
	{
		freopen(filename, "w", filePointer);
	}

	fprintf(filePointer, "%.02lf %lf", getmean(array, length), getmax(array, length)); //writes to file
	fclose(filePointer);
	free(filename);

	return;
}

void write_output(char* output_filename, double* array, int length, double factor)
{
	FILE* filePointer = fopen(output_filename, "w");

	if (filePointer == NULL) //making sure the input file exists
	{
		freopen(output_filename, "w", filePointer);
	}

	fprintf(filePointer, "%d %6.4f\n", length, factor); //writes to file
	int i;
	for (i = 0; i < length; i++)
	{
		fprintf(filePointer, "%6.4f\n", array[i]);
	}

	fclose(filePointer);

	return;
}

double getmean(double* array, int length)
{
	int i = 0;
	double add = 0;
	double mean = 0;

	for (i = 0; i < length; i++)//sums up all the numbers in the array
	{
		add += array[i];
	}

	mean = (double)(add / (double)length);

	return mean;
}

double getmax(double* array, int length)
{
	int i = 0;
	double max = -99999999999.00;

	for (i = 0; i < length; i++) //iterates through the array
	{
		if (array[i] > max)//finds the max value in the for loop
			max = array[i];
	}
	return max;
}
