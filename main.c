#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

// Bar width to display the graph of time taken by total number of threads
#define MAX_BAR_WIDTH 60


void *multiply(void *arg);
void set_matrix_values(int *arr, int rows, int columns);
void print_matrix_values(int *arr, int rows, int columns);
void print_line(int n);
void display_message(int n);
void print_bar_graph(const unsigned long long values[], int size);
void press_to_contnue();


// Structure containing arguments to be passed in multiply function for matrix multiplication
struct multiply_data {
    // address of matirces
    int *matrix_1;
    int *matrix_2;
    int *matrix_3;
    int row_s; // Rows start
    int row_e; // Row end
    int columns; // no of columns in matrix 3
    int common; // dimension shared by matrix 1 and 2 for multiplication
};



int main() {
    // file to store time taken by different number of threads
    FILE *file;
    file = fopen("log.txt", "w");

    // prints welcome message
    display_message(1);

    ///////////////////////////////////
    // MATRICES ROWS AND COLUMN VALUES;
    ///////////////////////////////////

    // Stores rows and column values temporarily for initial matrices
    int temp_array_values[4];

    // Getting rows and columns values for Multiplicant Matrices
    for (int i = 0; i < 2; i++) {
        printf("ENTER NUMBER OF ROWS IN MATRIX %d ::: ", (i + 1));
        scanf("%d", &temp_array_values[i * 2]);
        printf("ENTER NUMBER OF COLUMNS IN MATRIX %d ::: ", (i + 1));
        scanf("%d", &temp_array_values[i * 2 + 1]);
    }


    print_line(1);


    // Checks if matrix multiplication is possible or not via comparing rows and column values
    if (temp_array_values[1] != temp_array_values[2]) {
        printf("MATRIX MULTIPLICATION NOT POSSIBLE, C2 != R1 ....exiting\n");
        exit(0); // exits if matrix multiplication not possible
    }

    // Initialising Multiplicant and product matrices;
    int matrix_1_rows = temp_array_values[0];
    int matrix_1_columns = temp_array_values[1];
    int matrix_2_rows = temp_array_values[2];
    int matrix_2_columns = temp_array_values[3];
    int matrix_3_rows = matrix_1_rows;
    int matrix_3_columns = matrix_2_columns;

    // Dynamic allocation of Matrices
    int *matrix_1 = (int *)malloc(matrix_1_rows * matrix_1_columns * sizeof(int));
    int *matrix_2 = (int *)malloc(matrix_2_rows * matrix_2_columns * sizeof(int));
    int *matrix_3 = (int *)malloc(matrix_3_rows * matrix_3_columns * sizeof(int));

    // exit if any matrix is not inititalised correctly
    if (matrix_1 == NULL || matrix_2 == NULL || matrix_3 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    printf("MATRICES CREATED SUCCESSFULLY....\n");
    print_line(1);

    ///////////////////////////////////
    // DEFINING MATRICES
    ///////////////////////////////////

    printf("MATRIX WILL BE ASSIGNED WITH RANDOM NUMBERS...\n");
    print_line(1);

    printf("ENTER VALUES FOR MATRIX-1\n");
    set_matrix_values(matrix_1, matrix_1_rows, matrix_1_columns); // assigning values for multiplicant matrix - 1
    print_line(1);
    
    printf("ENTER VALUES FOR MATRIX-2\n");
    set_matrix_values(matrix_2, matrix_2_rows, matrix_2_columns); // assigning values for multiplicant matrix - 2
    print_line(1);
    

    // Asking user whether to print created Matrices or not
    int choice = 0;
    printf("DO YOU WANT TO PRINT THE MATRICES GENERATED...\n");
    printf("1 -> YES\n2-> NO (recommended)\n");
    scanf("%d", &choice);

    if (choice == 1) {
        print_line(1);
        printf("MATRIX-1\n");
        print_matrix_values(matrix_1, matrix_1_rows, matrix_1_columns);
        print_line(1);
        printf("MATRIX-2\n");
        print_matrix_values(matrix_2, matrix_2_rows, matrix_2_columns);
        print_line(1);
    }

    ///////////////////////////////////
    // THREADS CREATION AND TIMING
    ///////////////////////////////////

    // total no of threads to be used for checking time taken.
    int max_threads;
    printf("ENTER THE TOTAL NUMBER OF THREADS TO CREATE (<= %d) ::: ", matrix_3_rows);
    scanf("%d", &max_threads);

    // to make sure that max_threads isn't greater than rows to avoid errors on later stages.
    if (max_threads > matrix_3_rows) {
        printf("Number of threads exceeds the number of rows. Adjusting to %d threads.\n", matrix_3_rows);
        max_threads = matrix_3_rows;
    }

    printf("\n\n\n");
    
    // stores time taken by n threads for completion of a process.
    unsigned long long times[max_threads];

    // stores time taken by each thread for completion of a process.
    unsigned long time;

    // to calculate time taken by n number of threads for Matrix Multiplication
    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {

        // dynamic array of n number of threads for execution
        pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

        // dynamic array of n number of structures (values to be passed in function)
        struct multiply_data *thread_args = (struct multiply_data *)malloc(num_threads * sizeof(struct multiply_data));

        // exits if dynamic array aren't created successfully
        if (threads == NULL || thread_args == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        // stores time of execution (beginning and ending time)
        struct timeval stop, start;

        // rows to be handeled by each thread
        int rows_per_thread = matrix_3_rows / num_threads;
        // remainder no of rows (case when rows % num_threads != 0) so that remaining rows doesn't miss out
        int remainder_rows = matrix_3_rows % num_threads;

        // current time before execution
        gettimeofday(&start, NULL);

        // Assigns value to each structure and each thread for execution
        for (int i = 0; i < num_threads; i++) {
            thread_args[i].matrix_1 = matrix_1;
            thread_args[i].matrix_2 = matrix_2;
            thread_args[i].matrix_3 = matrix_3;
            thread_args[i].row_s = i * rows_per_thread + (i < remainder_rows ? i : remainder_rows);
            thread_args[i].row_e = (i + 1) * rows_per_thread + (i + 1 <= remainder_rows ? 1 : 0);
            thread_args[i].columns = matrix_2_columns;
            thread_args[i].common = matrix_1_columns;
            pthread_create(&threads[i], NULL, multiply, (void *)&thread_args[i]);
        }

        // Join all threads to ensure the main process waits for their completion
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // current time after execution
        gettimeofday(&stop, NULL);

        // Calculation of time taken by each nthreads process (stop_time - start_time)
        time = ( (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

        // saving time value in array to be used later and write in file
        times[num_threads - 1] = time;

        print_line(1);
        // Prints the time taken by current number of threads for multiplication process
        printf("NUMBER OF THREADS %d TOOK %lu us\t\t\t|\n", num_threads,time);

        // Free allocated memory
        free(threads);
        free(thread_args);
    }
    
    // Writing time values to the file seperated by newline
    for (int i = 0; i < max_threads ; i++ ) {
        fprintf(file, "%lld\n", times[i]);
    }

    getchar();
    press_to_contnue();

    // Printing time values as a graph for better understanding for user
    printf("\n\n\n");
    print_bar_graph(times, max_threads);
    press_to_contnue();
    
    // Free final matrix memory
    free(matrix_1);
    free(matrix_2);
    free(matrix_3);
    display_message(2);
    fclose(file);
    return 0;
}




/*
 *   Function : set_matrix_values
 *   Purpose : assigns matrices indices with random values as per user requirements
 *   Parameters :
 *           *arr - address of a matrix
 *           rows - No of rows in matrix
 *           columns - No of columns in matrix
 *   Returns: void
*/
void set_matrix_values(int *arr, int rows, int columns) {
    int max = 0, min = 0;
    printf("ENTER MIN VALUE FOR A MATRIX ELEMENT ::: ");
    scanf("%d", &min);
    printf("ENTER MAX VALUE FOR A MATRIX ELEMENT ::: ");
    scanf("%d", &max);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            arr[i * columns + j] = (rand() % (max - min + 1) + min);
        }
    }
}

/*
 *   Function : set_matrix_values
 *   Purpose : prints each element of matrix with their row and column values
 *   Parameters :
 *           *arr - address of a matrix
 *           rows - No of rows in matrix
 *           columns - No of columns in matrix
 *   Returns: void
*/
void print_matrix_values(int *arr, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("[%d][%d]%d\t",i,j, arr[i * columns + j]);
        }
        printf("\n");
    }
}

/*
 *   Function : print_line
 *   Purpose : Prints a horizontal line using hyphon (-) symbo
 *   Parameters :
 *           n - choice to print a newline or not (1 - newline, 2 - no newline)
 *   Returns: void
*/
void print_line(int n) {
    if (n == 1)
        printf("________________________________________________________________\n");
    else if (n == 2)
        printf("________________________________________________________________");
}

/*
 *   Function : multiply
 *   Purpose : performs multiplication on matrices and store the result in another matrix.
 *   Parameters :
 *           *arg - contains address of matrices and values for rows and columns
 *   Returns: void
*/
void *multiply(void *arg) {
    struct multiply_data *data = (struct multiply_data *)arg;
    for (int i = data->row_s; i < data->row_e; i++) {
        for (int j = 0; j < data->columns; j++) {
            data->matrix_3[i * data->columns + j] = 0;
            for (int k = 0; k < data->common; k++) {
                data->matrix_3[i * data->columns + j] += data->matrix_1[i * data->common + k] * data->matrix_2[k * data->columns + j];
            }
        }
    }
    return NULL;
}

/*
 *   Function : display_message
 *   Purpose : Prints a specific message based on provided parameter n
 *   Parameters :
 *           n - choice of pre-defined message
 *   Returns: void
*/
void display_message(int n) {
    if (n == 1) {
    // TO BE PRINTED AT START OF THE PROGRAM
        print_line(1);
        printf("\n");
        printf("PROGRAM TO DEMONSTRATE HOW USING DIFFERENT NUMBER OF THREADS FOR A\n");
        printf("SAME PROCESS AFFECTS IT'S TIME COMPLEXITY\n");
        print_line(1);
    } else if (n == 2) {
    // TO BE PRINTED AT END OF PROGRAM
        printf("--SUMMARY");
        printf("\n\n");
        printf("--Increasing Threads--\n--Generally decreases time to a point, but benefits decrease with more threads due to overhead and contention.\n");
        printf("\n");
        printf("--Optimal Thread Count--\n--Often close to the number of physical CPU cores. Hyper-threading can allow more threads, but with diminishing returns.\n");
        printf("\n");
        printf("--Application Dependency--\n--The best number of threads can vary depending on the nature of the application (CPU-bound vs. I/O-bound) and the specific workload\n");
        printf("\n");
    }
}


/*
 *   Function : print_bar_graph
 *   Purpose : Prints bar graph for time taken by each number of threads
 *   Parameters :
 *           values[] - list of time taken by threads
 *           size - size of list
 *   Returns: void
*/
void print_bar_graph(const unsigned long long values[], int size) {
    unsigned long long max_value = 0;
    for (int i = 0; i < size; i++) {
        if (values[i] > max_value) {
            max_value = values[i];
        }
    }

    for (int i = 0; i < size; i++) {
        int bar_width = (int)(((double)values[i] / max_value) * MAX_BAR_WIDTH);
        
        printf("nTHREADS [%d] | ", i + 1);
        
        for (int j = 0; j < bar_width; j++) {
            printf("#");
        }
        
        printf(" %llu\n", values[i]);
    }
}

/*
 *   Function : display_message
 *   Purpose : Allows program to wait for user input to proceed to next step
 *   Parameters : (None)
 *   Returns: void
*/
void press_to_contnue() {
    printf("\nPRESS ENTER TO CONTINUE....\n");
    getchar();
}