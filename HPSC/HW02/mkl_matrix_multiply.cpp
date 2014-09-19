#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <mkl.h>

using namespace std;

typedef double matrix_type;

void parse_matrix_row_major(string filename, matrix_type* matrix, const int m, const int n);
void row_col(string filename, int& m, int& n);
matrix_type* allocate_matrix(const int m, const int n);
void deallocate_matrix(matrix_type* matrix, const int m, const int n);
void write_matrix(string filename, matrix_type* matrix, int m, int n);

int main(int argc, char** argv)
{
    // make sure number of arguments is correct
    assert argc == 4;
    
    // read in matrix A, B, and C filenames
    string A_file = string(argv[1]);
    string B_file = string(argv[2]);
    string C_file = string(argv[3]);
    
    // read in rows and cols of A, B, and C
    int A_m, A_n, B_m, B_n, C_m, C_n;
    row_col(A_file, A_m, A_n);
    row_col(B_file, B_m, B_n);
    row_col(C_file, C_m, C_n);
    
    // allocate matrices and initialize matrices
    A = allocate_matrix(A_m, A_n);
    B = allocate_matrix(B_m, B_n);
    C = allocate_matrix(C_m, C_n);
    
    parse_matrix_row_major(A_file, A, A_m, A_n);
    parse_matrix_row_major(B_file, B, B_m, B_n);
    
    // multiply matrices A and B using dgemm
    char* Atran = 'n';
    char* Btran = 'n';
    dgemm(Atran, Btran, A_m, B_n, A_n, 1.0, A, A_m, B, B_m, 0.0, C, C_m); // C= AXB
    
    // write C out to file
    write_matrix_row_major(C_file, C, C_m, C_n);
    
    // deallocate matrices A,B,C
    deallocate_matrix(A, A_m, A_n);
    deallocate_matrix(B, B_m, B_n);
    deallocate_matrix(C, C_m, C_n);
    
    return 0;
}

// parses matrix from file
void parse_matrix_row_major(string filename, matrix_type* matrix, const int m, const int n)
{
    ifstream matrix_file (filename);
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
                matrix[i+j*n] = str2d(num);
            }
            j++;
        }
    }
}

// row_col reads in rows and columns of matrix file and saves them to m and n
void row_col(string filename, int& m, int& n)
{
    int dot_pos1 = filename.find('.', 0);
    int x_pos = filename.find('x', dot_pos1);
    int dot_pos2 = filename.find('.', x_pos);
    *m = atoi(filename.substr(dot_pos1 + 1, x_pos - dot_pos1 - 1));
    *n = atoi(filename.substr(x_pos + 1, dot_pos2 - x_pos - 1));
}

// allocates matrix array
matrix_type* allocate_matrix(const int m, const int n)
{
    int size = m*n;
    new_matrix = new matrix_type[size];
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
    ofstream file ( filename );
    if(file.isopen())
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