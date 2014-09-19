#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <mkl.h>
#include <iomanip>
#include <omp.h>

using namespace std;

typedef double matrix_type;

void parse_matrix_row_major(string filename, matrix_type* matrix, const int m, const int n);
void row_col(string filename, int* m, int* n);
matrix_type* allocate_matrix(const int m, const int n);
void deallocate_matrix(matrix_type* matrix, const int m, const int n);
void write_matrix_row_major(string filename, matrix_type* matrix, int m, int n);
matrix_type* rearrange_to_col_major(matrix_type* matrix, int m, int n);
void matrix_multiply(matrix_type* A, int A_m, int A_n, matrix_type* B, int B_m, int B_n, C);
void zero_matrix_init(matrix_type* matrix, int m, int n);

int main(argc, argv)
{
    // make sure number of arguments is correct
    assert(argc == 4);
    
    // read in matrix A, B, and C filenames
    string A_file = string(argv[1]);
    string B_file = string(argv[2]);
    string C_file = string(argv[3]);
    
    // read in rows and cols of A, B, and C
    int A_m, A_n, B_m, B_n, C_m, C_n;
    row_col(A_file, &A_m, &A_n);
    row_col(B_file, &B_m, &B_n);
    row_col(C_file, &C_m, &C_n);
    
    // allocate matrices and initialize matrices
    matrix_type* A = allocate_matrix(A_m, A_n);
    matrix_type* B = allocate_matrix(B_m, B_n);
    matrix_type* C = allocate_matrix(C_m, C_n);
    
    parse_matrix_row_major(A_file, A, A_m, A_n);
    parse_matrix_row_major(B_file, B, B_m, B_n);
    
    B = rearrange_to_col_major(B, B_m, B_n);
    zero_matrix_init(C, C_m, C_n);

    // multiply
    matrix_multiply(A, A_m, A_n, B, B_n, B_m, C);
    
}

void parse_matrix_row_major(string filename, matrix_type* matrix, const int m, const int n)
{
    ifstream matrix_file (filename.c_str());
    string line;
    
    int is_first = -1;
    int j = 0;
    if (matrix_file.is_open())
    {
        while(getline(matrix_file,line))
        {
            if(is_first < 0)
            {
                is_first++;
                continue;
            }
            
            string temp_line = line;
            int iter1, iter2 = 0;
            
            for(int i = 0; i < n - 1; i++)
            {
                iter1 = iter2 + 1;
                if(i == 0) iter1 = 0;
                iter2 = temp_line.find(' ', iter1);
                string num = temp_line.substr(iter1, iter2 - iter1 - 1);
                matrix[i+j*n] = strtod(num.c_str(), NULL);
            }
            j++;
        }
    }
}

// row_col reads in rows and columns of matrix file and saves them to m and n
void row_col(string filename, int* m, int* n)
{
    int dot_pos1 = filename.find('.', 0);
    int x_pos = filename.find('x', dot_pos1);
    int dot_pos2 = filename.find('.', x_pos);
    *m = atoi(filename.substr(dot_pos1 + 1, x_pos - dot_pos1 - 1).c_str());
    *n = atoi(filename.substr(x_pos + 1, dot_pos2 - x_pos - 1).c_str());
}

// allocates matrix array
matrix_type* allocate_matrix(const int m, const int n)
{
    int size = m*n;
    matrix_type* new_matrix = new matrix_type[size];
    return new_matrix;
}

// deallocates matrix array
void deallocate_matrix(matrix_type* matrix, const int m, const int n)
{
    delete[] matrix;
}

// write matrix to file
void write_matrix_row_major(string filename, matrix_type* matrix, int m, int n)
{
    ofstream file (filename.c_str());
    if(file.is_open())
    {
        file << m << " " << n << "\n";
        for(int i = 0; i < m - 1; i++) {
            for(int j = 0; j < n; j++) {
                file << setprecision(5) << fixed << matrix[i+n*j] << " ";
            }
            file << '\n';
        }
    }
    else {
        cout << "File could not be opened" << endl;
    }
}

// reaggange to colomn major for memory locality
matrix_type* rearrange_to_col_major(matrix_type* matrix, int m, int n)
{
    matrix_type* new_matrix = allocate_matrix(m, n);
    for(int j = 0; j < m; j++)
    {
        for(int i = 0; i < n; i++)
        {
            new_matrix[j+i*m] = matrix[i+j*n];
        }
    }
    deallocate_matrix(matrix, m, n);
    return new_matrix;
}

void matrix_multiply(matrix_type* A, int A_m, int A_n, matrix_type* B, int B_m, int B_n, C)
{
    for(int c_i = 0; c_i < B_n; c_i++)
    {
        for(int c_j = 0; c_j < A_m; c_j++)
        {
            for(int a_i = 0; a_i < A_n; a_i++)
            {
                for(int a_j = 0; a_j < A_m; a_j++)
                {
                    C[c_i + c_j * B_n] += A[a_i + a_j * A_n] * B[a_i + a_j * B_m];
                }
            }
        }
    }
}

void zero_matrix_init(matrix_type* matrix, int m, int n)
{
    int mn = m * n;
    for(int i = 0; i < mn; i++) matrix[i] = 0;
}
















