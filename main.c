#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

// Bar width to display the graph of time taken by
// total number of threads
#define MAX_BAR_WIDTH 50

// Function to perform matrix multiplication
void *multiply(void *arg);

// Function to assign matrices with a random value
void set_matrix_values(int *arr, int rows, int columns);

// Function to print MATRIX
void print_matrix_values(int *arr, int rows, int columns);

// Prints a horizontal line of (-)
void print_line(int n);

// Prints intro and outro text
void display_message(int n);

// Calculates the difference between time taken by consecutive threads,
// and find points where performance is diminished
void diminished_points(const unsigned long long times[], int size);

// Prints bar graph for time taken by each number of threads
void print_bar_graph(const unsigned long long values[], int size);

// Allows program to wait for user input to proceed to next step
void press_to_contnue();

// Arguments to be passed in pointer function for atrix multiplication
struct multiply_data {
    int *matrix_1;
    int *matrix_2;
    int *matrix_3;
    int row_s;
    int row_e;
    int columns;
    int common;
};



int main() {
    FILE *file;
    file = fopen("log.txt", "w");
    display_message(1);
    ///////////////////////////////////
    // MATRICES ROWS AND COLUMN VALUES;
    ///////////////////////////////////

    // Stores rows and column values temporarily for initial matrices
    int temp_array_values[4];

    for (int i = 0; i < 2; i++) {
        printf("ENTER NUMBER OF ROWS IN MATRIX %d ::: ", (i + 1));
        scanf("%d", &temp_array_values[i * 2]);
        printf("ENTER NUMBER OF COLUMNS IN MATRIX %d ::: ", (i + 1));
        scanf("%d", &temp_array_values[i * 2 + 1]);
    }

    print_line(1);

    // Checks if matrix multiplication is possible or not via
    // comparing rows and column values
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
    set_matrix_values(matrix_1, matrix_1_rows, matrix_1_columns);
    print_line(1);
    
    printf("ENTER VALUES FOR MATRIX-2\n");
    set_matrix_values(matrix_2, matrix_2_rows, matrix_2_columns);
    print_line(1);
    

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

    if (max_threads > matrix_3_rows) {
        printf("Number of threads exceeds the number of rows. Adjusting to %d threads.\n", matrix_3_rows);
        max_threads = matrix_3_rows;
    }

    printf("\n\n\n");
    
    // stores time taken by n threads for completion of a process.
    unsigned long long times[max_threads];
    // stores time taken by each thread for completion of a process.
    unsigned long time;

    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {

        // Allocate memory for threads and thread arguments

        // creates a dynamic array of n number of threads for execution
        pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

        // creates a dynamic array of n number of structures (values to be passed in function)
        struct multiply_data *thread_args = (struct multiply_data *)malloc(num_threads * sizeof(struct multiply_data));

        if (threads == NULL || thread_args == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        // stores time of execution (beginning and ending time)
        struct timeval stop, start;

        int rows_per_thread = matrix_3_rows / num_threads;
        int remainder_rows = matrix_3_rows % num_threads;

        // time before execution
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

        // starts the execution of each thread;
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // time after execution
        gettimeofday(&stop, NULL);

        // Calculation of time taken by each nthreads process
        time = ( (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

        times[num_threads - 1] = time;

        print_line(1);
        printf("NUMBER OF THREADS %d TOOK %lu us\t\t\t|\n", num_threads,time);

        // Free allocated memory
        free(threads);
        free(thread_args);
    }

    for (int i = 0; i < max_threads ; i++ ) {
        fprintf(file, "%lld\n", times[i]);
    }

    getchar();
    press_to_contnue();

    printf("\n\n\n");
    print_bar_graph(times, max_threads);
    press_to_contnue();
    
    printf("\n\n\n");
    diminished_points(times, max_threads);
    
    // Free final matrix memory
    free(matrix_1);
    free(matrix_2);
    free(matrix_3);
    display_message(2);
    fclose(file);
    return 0;
}

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

void print_matrix_values(int *arr, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("[%d][%d]%d\t",i,j, arr[i * columns + j]);
        }
        printf("\n");
    }
}

void print_line(int n) {
    if (n == 1)
        printf("________________________________________________________________\n");
    else if (n == 2)
        printf("________________________________________________________________");
}

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

void diminished_points(const unsigned long long times[], int size) {
    unsigned long long *differences = (unsigned long long *)malloc((size - 1) * sizeof(unsigned long long));

    for (int i = 1; i < size; i++) {
        differences[i - 1] = times[i - 1] - times[i];
    }
    printf("\nPoints where performance diminishes:\n");
    for (int i = 1; i < size - 1; i++) {
        if (differences[i - 1] > differences[i]) {
            print_line(1);
            printf("Performance diminishes between threads: %d and threads: %d)  |\n",
                    i + 1, i + 2);
        }
    }

    free(differences);
}
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
void press_to_contnue() {
    printf("\nPRESS ENTER TO CONTINUE....\n");
    getchar();
}