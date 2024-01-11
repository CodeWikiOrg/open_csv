/**
 * Author(s):           Arda T. Kersu
 * File name:           open_csv.c
 * Date:                1st November 2023
 *
 * Description: Source code for the library "open_csv.h" that provides easier handling and manipulation of
 *              '.csv' files with the use of C programming language. See inline comments for further details
 *              regarding any specific function of interest.
 *
 * Disclaimer: This open-source project is provided "as is" without any warranty, expressed or implied.
 *             The contributors and maintainers disclaim any an all unintended consequences or issues
 *             that may arise from its use. Users are encouraged to review and understand the software's
 *             characteristics before implementation.
 *
 *
 * Copyright @ [Arda T. Kersu]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "open_csv.h"

FILE *csvPtr = NULL;


/**
 * @brief Close a file safely and report the status.
 *
 * This function attempts to close the specified file pointed to by 'filePtr'. It checks if the
 * file pointer is NULL and reports whether the file was closed successfully or if it could
 * not be found.
 *
 * @param filePtr A pointer to the file to be closed.
 *
 * @note This function reports the status of file closure using standard output and error streams.
 *
 * @code
 *   // Example usage:
 *   FILE *file = fopen("example.txt", "r");
 *   closeFile(file);
 *   // Attempt to close the file and report the status...
 * @endcode
 */
void closeFile(FILE *filePtr)
{
    if(filePtr == NULL)
    {
        fprintf(stderr, "File could not be found, hence could not be closed.\n");
    }
    else
    {
        fprintf(stdout, "File has been closed safely.\n");
        fclose(filePtr);
    }
}

/**
 * @brief Get the size of a data frame from a '.csv' file.
 *
 * This function reads a '.csv' file pointed to by 'filePtr' and determines the number of rows and columns
 * in the data frame. It skips the first row (usually containing feature names) and counts the rows
 * and columns in the dataset.
 *
 * @param filePtr A pointer to the '.csv' file to analyze.
 * @return An integer array containing the number of rows and columns, or NULL if an error occurs.
 *
 * @note This function dynamically allocates memory for the integer array 'retVal,' which should be
 *       freed by the caller when no longer needed to avoid memory leaks.
 *
 * @code
 *   // Example usage:
 *   FILE *file = fopen("data.csv", "r");
 *   int *size = getDFsize(file);
 *   if (size != NULL) {
 *       printf("Number of rows: %d\n", size[0]);
 *       printf("Number of columns: %d\n", size[1]);
 *       free(size); // Free the allocated memory
 *   }
 *   else
 *   {
 *       puts("Error occurred while getting data frame size.");
 *   }
 *   // Analyze the '.csv' file and retrieve the data frame size...
 * @endcode
 */
int *getDFsize(FILE *filePtr)
{
    char buffer[1024];      //buffer
    int skipFirstRow = 0;   //skips the "feature names" row, first row of the file
    int *retVal = (int *)malloc(sizeof(int) * 2), dfRows = 0, dfCols = 0; //return value
    filePtr = fopen(CSV_PATH, CSV_MODE);    //open the file

    if(filePtr == NULL) //check file validity
    {
        puts("Could not open the file.");
        return NULL;
    }
    else
    {
        printf("File has been opened.\n");
    }

    bool_t lock = FALSE;
    while(fgets(buffer, 1024, filePtr)) //pull file contents row by row
    {
        char *tokens = strtok(buffer, CSV_DELIM); //split them into tokens separated by deliminator

        if(skipFirstRow == 0) //this condition locks itself out once it has executed for the value 0
        {
            skipFirstRow = 1;
            continue;
        }

        if(lock == FALSE) //also runs once and locks itself out. counts the number of columns in dataset
        {
            while(tokens) //count tokens
            {
                tokens = strtok(NULL, CSV_DELIM);
                dfCols++;
            }
            lock = TRUE;
        }

        dfRows++; //count rows
    }

    retVal[0] = dfRows;
    retVal[1] = dfCols;

    closeFile(filePtr);

    return retVal;
}

/**
 * @brief Create a CSV data frame structure based on file information.
 *
 * This function analyzes a '.csv' file pointed to by 'filePtr' to determine its size and creates
 * a CSV data frame structure accordingly. The data frame structure includes the number of rows,
 * columns, delimiter, and feature parameters.
 *
 * @param filePtr A pointer to the '.csv' file to create the data frame from.
 * @return A pointer to a dynamically allocated 'csvData_t' structure representing the data frame.
 *
 * @note The caller is responsible for freeing the memory allocated for the 'csvData_t' structure
 *       and its members when it is no longer needed to avoid memory leaks.
 *
 * @code
 *   // Example usage:
 *   FILE *file = fopen("data.csv", "r");
 *   csvData_t *dataFrame = createDataFrame(FILE);
 *   if (dataFrame != NULL) {
 *       // Use the data frame...
 *       free(dataFrame->delim);
 *       free(dataFrame->params);
 *       free(dataFrame);
 *   }
 *   else
 *   {
 *       puts("Error occurred while creating the data frame.");
 *   }
 *   // Create a data frame based on the '.csv' file...
 * @endcode
 */
csvData_t *createDataFrame(FILE *filePtr)
{
    int *dfSize = getDFsize(filePtr); //get dataframe size info
    csvData_t *dataFrame = (csvData_t *)malloc(sizeof(csvData_t)); //allocate memory for row/column numbers
    dataFrame->rows = dfSize[0];
    dataFrame->cols = dfSize[1];

    dataFrame->delim = (char *)malloc(sizeof(char) * 2); //allocate memory for deliminator
    strncpy(dataFrame->delim, CSV_DELIM, sizeof(char *));

    dataFrame->params = (char *)malloc(sizeof(char) * (dataFrame->cols + 1));//allocate memory for features
    dataFrame->params[dataFrame->cols] = '\0';

    return dataFrame;
}

/**
 * @brief Trim a token by removing non-alphanumeric characters.
 *
 * This function takes a token as input and removes any non-alphanumeric characters from it.
 * It returns a dynamically allocated string containing the trimmed token.
 *
 * @param token A null-terminated string representing the token to be trimmed.
 * @return A dynamically allocated string containing the trimmed token, or NULL if an error occurs.
 *
 * @note The caller is responsible for freeing the memory allocated for the returned trimmed token
 *       when it is no longer needed to avoid memory leaks.
 *
 * @code
 *   // Example usage:
 *   char *originalToken = "abc!@123";
 *   char *trimmed = trimToken(originalToken);
 *   if (trimmed != NULL) {
 *       printf("Original Token: %s\n", originalToken);
 *       printf("Trimmed Token: %s\n", trimmed);
 *       free(trimmed); // Free the allocated memory
 *   }
 *   else
 *   {
 *       puts("Error occurred while trimming the token.");
 *   }
 *   // Trim a token by removing non-alphanumeric characters...
 * @endcode
 */
char *trimToken(char *token)
{
    char *trimmedToken = (char *)malloc(strlen(token) + 1);
    trimmedToken[0] = '\0';

    int loop = 0, innerLoop = 0;
    while(*(token + loop) != '\0')
    {
        if( ! isalnum(*(token + loop)))
        {
            loop++;
            continue;
        }
        else
        {
            *(trimmedToken + innerLoop) = *(token + loop);
            innerLoop++;
            loop++;
        }
    }

    return trimmedToken;
}

/**
 * @brief Load data from a '.csv' file into a CSV data frame.
 *
 * This function opens a '.csv' file pointed to by 'filePtr', reads the data from the file,
 * and loads it into a CSV data frame. It extracts feature names from the first row and stores
 * them in the data frame's 'params' member. Data points are read and stored in the 'dataFrame'
 * member of the data frame.
 *
 * @param filePtr A pointer to the '.csv' file to load data from.
 * @return A pointer to a dynamically allocated 'csvData_t' structure representing the loaded data frame.
 *
 * @note The caller is responsible for freeing the memory allocated for the returned data frame
 *       when it is no longer needed to avoid memory leaks.
 *
 * @code
 *   // Example usage:
 *   FILE *file = fopen("data.csv", "r");
 *   csvData_t *dataFrame = loadCsv(file);
 *   if (dataFrame != NULL)
 *   {
 *       // Use the loaded data frame...
 *       // Don't forget to free the allocated memory when done.
 *       free(dataFrame->params);
 *       for (int row = 0; row < dataFrame->rows; row++)
 *       {
 *           free(dataFrame->dataFrame[row]);
 *       }
 *       free(dataFrame->dataFrame);
 *       free(dataFrame);
 *   }
 *   else
 *   {
 *       puts("Error occurred while loading the '.csv' file.");
 *   }
 *   // Load data from a '.csv' file into a CSV data frame...
 * @endcode
 */

csvData_t *loadCsv(FILE *filePtr)
{
    char buffer[1024];

    filePtr = fopen(CSV_PATH, CSV_MODE);

    if(filePtr != NULL)
    {
        puts("the file has been opened\n");
    }

    csvData_t *df = createDataFrame(filePtr); //create and initialize dataframe

    //EXTRACT FEATURE NAMES ---------------------------------------------------

    {
        fgets(buffer, 1024, filePtr);       //get the first line of csv file
        char *tokens = strtok(buffer, df->delim);   //split into multiple tokens

        while(tokens) //
        {
            char *label = trimToken(tokens); //trim token of unwanted characters
            strncat(df->params, label, sizeof(char) * strlen(label)); //write into dataframe
            printf("\"%s\", \n", trimToken(tokens)); //print dataset features, can be commented out
            tokens = strtok(NULL, df->delim); //split the next token from source
        }
    }

    //EXTRACT DATA POINTS------------------------------------------------------

    df->dataFrame = (float **)malloc(sizeof(float *) * df->rows); //allocate ...
    for(int row=0; row<df->rows; row++)                                //... memory
    {
        df->dataFrame[row] = (float *)malloc(sizeof(float) * df->cols);
    }

    {
        int row = 0;
        while(fgets(buffer, 1024, filePtr)) //get data from dataset row by row
        {
            int col = 0;
            char *tokens = strtok(buffer, df->delim); //split into tokens

            while(tokens)
            {
                df->dataFrame[row][col] = atof(tokens); //feed data into dataframe
                tokens = strtok(NULL, df->delim); //further break into tokens
                col++;
            }
            //(void)puts(" ");

            row++;
        }
    }

    fclose(filePtr);

    return df;
}
