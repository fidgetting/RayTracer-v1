/* ****************************************************************************
 * Copyright (C) 2010 Alex Norton                                             *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify it    *
 * under the terms of the BSD 2-Clause License.                               *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRENTY; without even the implied warranty of                 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 **************************************************************************** */

#ifndef MATRIX_H_INCLUDE
#define MATRIX_H_INCLUDE

#include <Vector.tpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>

/**
 * The basic matrix class. This uses doubles for the data representation and the
 * number of rows/columns is templated so that this can be optimzed by the
 * compiler.
 *
 * @file matrix.tpp
 */
template<int R, int C>
class Matrix {
  public:

    // deep constructor
    Matrix(double d = 0) { for(int i = 0; i < R; i++) for(int j = 0; j < C; j++) data[i][j] = d; }
    // deep destructor
    virtual ~Matrix() { }

    /* ********** other matrix operations ********** */
    void gaussian_elimination();

    /* ********** accessor methods ********** */
    inline int width() const { return C; }
    inline int height() const { return R; }
    inline double* operator[](int row) { return data[row]; }
    inline const double* operator[](int row) const { return data[row]; }

    /* ********** other operators ********** */
    template<int Ra, int Ca>
    Matrix<R, C> operator*=(const Matrix<Ra, Ca>& rhs);

  protected:

    /* instace variables */
    double data[R][C];
};

/**
 * Gets the identity matrix for any given size. Calling identity<4>() will
 * return a new matrix that looks like this:
 *
 * [ 1  0  0  0 ]
 * [ 0  1  0  0 ]
 * [ 0  0  1  0 ]
 * [ 0  0  0  1 ]
 *
 * @return
 */
template<int size>
Matrix<size, size> identity() {
  Matrix<size, size> ret(0);

  for(int i = 0; i < size; i++) {
    ret[i][i] = 1;
  }

  return ret;
}

/**
 * Performs gaussian elimination on a Matrix. This is used to solve
 * the standard Ax = b. The Matrix that this is called on must be an
 * augmented Matrix and the reuslt will be stored in the final column
 * of the Matrix.
 */
template<int R, int C>
void Matrix<R, C>::gaussian_elimination() {
  int i, j, k, max;

  for(i = 0; i < R; i++) {
    max = i;
    for(j = i + 1; j < R; j++) {
      if(abs(data[j][i]) > abs(data[max][i])) {
        max = j;
      }
    }

    for(j = 0; j <= R; j++) {
      std::swap(data[max][j], data[i][j]);
    }

    for(j = R; j >= i; j--) {
      for(k = i + 1; k < R; k++) {
        data[k][j] -= data[k][i]/data[i][i] * data[i][j];
      }
    }
  }

  for(i = R - 1; i >= 0; i--) {
    data[i][R] /= data[i][i];
    data[i][i] = 1;
    for(j = i - 1; j >= 0; j--) {
      data[j][R] -= data[j][i] * data[i][R];
      data[j][i] = 0;
    }
  }
}

/**
 * The output stream operator for the matrix class. This will print in this
 * format:
 *  [ A1,1 A1,2 ... A1,n ]
 *  [ A2,1 A2,2 ... A2,n ]
 *  [ .      .        .  ]
 *  [ .       .       .  ]
 *  [ .        .      .  ]
 *  [ Am,1 Am,2 ... Am,n ]
 *
 * @param ostr the stream that will be printed to
 * @param mat the matrix that this will print
 * @return the output stream that is being printed to
 */
template<int R, int C>
ostream& operator<<(ostream& ostr, const Matrix<R, C> mat) {
  for(int i = 0; i < mat.height(); i++) {
    ostr << "[ ";
    for(int j = 0; j < mat.width(); j++) {
      ostr << std::setprecision(5) << std::setfill(' ') << std::setw(7)
           << mat[i][j] << " ";
    }
    ostr << "]\n";
  }
  ostr << std::flush;

  return ostr;
}

/**
 * Standard matrix multiplication. Since the Matrix class is templated to its
 * size, this will do all the size checks at compile time.
 *
 * @param lhs the matrix on the left hand side of the multiplication
 * @param rhs the matrix on the right hand side of the multiplication
 * @return a new matrix that is the multiplication of the other two
 */
template<int R, int RC, int C>
Matrix<R, C> mult(const Matrix<R, RC>& lhs, const Matrix<RC, C>& rhs) {
  Matrix<R, C> ret;

  for(int i = 0; i < R; i++) {
    for(int j = 0; j < C; j++) {
      for(int k = 0; k < RC; k++) {
        ret[i][j] += lhs[i][k]*rhs[k][j];
      }
    }
  }

  return ret;
}

/**
 * Standard matrix multiplication. Since the Matrix class is templated to its
 * size, this will do all the size checks at compile time.
 *
 * @param lhs the matrix on the left had side of the multiplication
 * @param rhs the matrix on the right hand side of the multiplication
 * @return a new matrix that is the multiplication of the other two
 */
template<int R1, int C1, int R2, int C2>
Matrix<R1, C2> operator*(const Matrix<R1, C1>& lhs, const Matrix<R2, C2>& rhs) {
  return mult<R1, C1, C2>(lhs, rhs);
}

/**
 * Multiplies a matrix by a vector on the right hand of the matrix. This
 * is essentially the same a matrix multiplication, but one of the matrices
 * has only 1 column.
 *
 * @param lhs the matrix that will be multiplied
 * @param rhs the vector that will be multiplied
 * @return a vector that is the mutliplication of the matrix and vector
 */
template<int R, int S>
Vector<S> operator*(const Matrix<R, S>& lhs, const Vector<S>& rhs) {
  Vector<S> ret;

  for(int i = 0; i < lhs.height(); i++) {
    for(int k = 0; k < S; k++) {
      ret[i] += lhs[i][k]*rhs[k];
    }
  }

  return ret;
}

/**
 * Member function matrix multiply used for the *= operator.
 *
 * @param rhs the matrix to multiply this matrix by
 * @return this matrix
 */
template<int R,  int C>
template<int Ra, int Ca>
Matrix<R, C> Matrix<R, C>::operator*=(const Matrix<Ra, Ca>& rhs) {
  *this = mult<R, C, Ca>(*this, rhs);
  return *this;
}

#endif /* MATRIX_H_INCLUDE */
