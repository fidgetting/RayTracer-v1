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

#ifndef VECTOR_TPP_INCLUDE
#define VECTOR_TPP_INCLUDE

#include <lexer.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
using std::exception;
#include <iostream>
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
#include <limits>
#include <string>
using std::string;
#include <vector>
using std::vector;

/**
 *
 */
template<int S, typename type = double>
class Vector {
  public:

    typedef type* iterator;
    typedef const type* const_iterator;

    Vector(type t = type()) : data() { for(int i = 0; i < S; i++) data[i] = t; }
    ~Vector() { }

    template<typename cast>
    const Vector<S, cast>& operator=(const Vector<S, cast>& asn);
    const Vector<S, type>& operator=(const Vector<S, type>& asn);
    const Vector<S + 1, type>& operator=(const Vector<S + 1, type>& asn);

    /* ********** getters and setters ********** */
    inline type& operator[](int i) { return data[i]; }
    inline type operator[](int i) const { return data[i]; }
    inline int size() const { return S; }
    inline void clear() { for(auto iter = begin(); iter != end(); iter++) *iter = 0; }

    inline iterator begin() { return data; }
    inline const_iterator begin() const { return data; }
    inline iterator end() { return data + S; }
    inline const_iterator end() const { return data + S; }

    /* ********** Math Operators ********** */
    Vector<S, type> cross(const Vector<S, type>& rhs) const;
    double distance(const Vector<S, type>& rhs) const;
    double dot(const Vector<S, type>& rhs) const;
    double length() const;
    void negate();
    void normalize();
    Vector<S, type>& operator+=(const Vector<S, type>& v);
    Vector<S, type>& operator+=(const type& d);
    Vector<S, type>& operator/=(const type& d);

    template<typename cast>
    Vector<S, type>& operator+=(const Vector<S, cast>& v);

  protected:
    type data[S];
};

typedef Vector<3> point;

/**
 *
 * @param asn
 * @return
 */
template<int S, typename type>
template<typename cast>
const Vector<S, cast>& Vector<S, type>::operator=(const Vector<S, cast>& asn) {
  for(int i = 0; i < S; i++) {
    data[i] = asn[i];
  }
  return asn;
}

/**
 *
 * @param asn
 * @return
 */
template<int S, typename type>
const Vector<S, type>& Vector<S, type>::operator=(const Vector<S, type>& asn) {
  memcpy(data, asn.data, sizeof(data));
  return asn;
}

/**
 *
 * @param asn
 * @return
 */
template<int S, typename type>
const Vector<S + 1, type>& Vector<S, type>::operator=(const Vector<S + 1, type>& asn) {
  for(int i = 0; i < S; i++) {
    data[i] = asn[i];
  }
  return asn;
}

/**
 * Takes the cross products of two vectors. The cross products will produce a
 * vector that is perpendicular to both the input vectors and has a magnitude
 * relative to their magnitudes and the angle between them.
 *
 * @param rhs the vector to take the cross product with
 * @return a new vector that is the cross product of this and rhs
 */
template<int S, typename type>
Vector<S, type> Vector<S, type>::cross(const Vector<S, type>& rhs) const {
  Vector<S, type> ret;
  ret[0] = data[1]*rhs[2] - data[2]*rhs[1];
  ret[1] = data[2]*rhs[0] - data[0]*rhs[2];
  ret[2] = data[0]*rhs[1] - data[1]*rhs[0];
  return ret;
}

/**
 * Calculate the distance between this point and another.
 *
 * @param rhs the point to calculate the distance to
 * @return the distance between the points
 */
template<int S, typename type>
double Vector<S, type>::distance(const Vector<S, type>& rhs) const {
  double ret = 0;

  for(auto l = begin(), r = rhs.begin(); l != end(); l++, r++) {
    ret += pow(*l - *r, 2);
  }

  return sqrt(ret);
}

/**
 * Calculate the dot product of this Vector and another Vector
 *
 * @param rhs the Vector to calculate the dot product with
 * @return the dot product
 */
template<int S, typename type>
double Vector<S, type>::dot(const Vector<S, type>& rhs) const {
  double ret = 0;

  for(auto l = begin(), r = rhs.begin(); l != end(); l++, r++) {
    ret += (*l)*(*r);
  }

  return ret;
}

/**
 * Calculates the length of this Vector in the coordinate system of the
 * Vector
 *
 * @return the length of the Vector
 */
template<int S, typename type>
double Vector<S, type>::length() const {
  double ret = 0;

  for(auto iter = begin(); iter != end(); iter++) {
    ret += pow(*iter, 2);
  }

  return sqrt(ret);
}

/**
 * reverse the direction of the Vector
 */
template<int S, typename type>
void Vector<S, type>::negate() {
  for(auto iter = begin(); iter != end(); iter++) {
    *iter = -*iter;
  }
}

/**
 * In place normalization of the Vector
 */
template<int S, typename type>
void Vector<S, type>::normalize() {
  double mag = length();
  for(auto iter = begin(); iter != end() && mag != 0; iter++) {
    *iter /= mag;
  }
}

/**
 * Add a Vector to this Vector. This will add each element of the provided
 * Vector to the corresponding element of this Vector
 *
 * @param v the Vector to add
 * @return this Vector
 */
template<int S, typename type>
Vector<S, type>& Vector<S, type>::operator+=(const Vector<S, type>& v) {
  for(auto l = begin(), r = v.begin(); l != end(); l++, r++) {
    *l += *r;
  }
  return *this;
}

/**
 * Add a number to this Vector. This will add the number to each element in
 * this Vector
 *
 * @param d the number to add
 * @return this Vector
 */
template<int S, typename type>
Vector<S, type>& Vector<S, type>::operator+=(const type& d) {
  for(auto l = begin(); l != end(); l++) {
    *l += d;
  }
  return *this;
}

/**
 *
 * @param d
 * @return
 */
template<int S, typename type>
Vector<S, type>& Vector<S, type>::operator/=(const type& d) {
  for(auto l = begin(); l != end(); l++) {
    *l /= d;
  }
  return *this;
}

/**
 *
 * @param v
 * @return
 */
template<int S, typename type>
template<typename cast>
Vector<S, type>& Vector<S, type>::operator+=(const Vector<S, cast>& v) {
  for(int i = 0; i < S; i++) {
    data[i] = std::min(int(std::numeric_limits<type>::max()), data[i] + type(v[i]));
  }
  return *this;
}

/**
 * check if two Verteces are equal
 *
 * @param lhs the left hand side of the operator
 * @param rhs the right hand side of the operator
 * @return if the two are equal
 */
template<int S, typename type>
bool operator==(const Vector<S, type>& lhs, const Vector<S, type>& rhs) {
  for(auto l = lhs.begin(), r = rhs.begin(); l != lhs.end(); l++, r++) {
    if(*l != *r) {
      return false;
    }
  }
  return true;
}

/**
 * check if two Verteces are not equal
 *
 * @param lhs the left hand side of the operator
 * @param rhs the right hand side of the operator
 * @return if the two are equal
 */
template<int S, typename type>
bool operator!=(const Vector<S, type>& lhs, const Vector<S, type>& rhs) {
  for(auto l = lhs.begin(), r = rhs.begin(); l != lhs.end(); l++, r++) {
    if(*l != *r) {
      return true;
    }
  }
  return false;
}

/**
 * Subtract one Vector from another
 *
 * @param lhs the left hand side of the subtraction
 * @param rhs the right hand side of the subtraction
 * @return the first minus the second
 */
template<int S, typename type>
Vector<S, type> operator-(const Vector<S, type>& lhs, const Vector<S, type>& rhs) {
  Vector<S, type> ret;

  for(auto l = lhs.begin(), r = rhs.begin(), d = ret.begin(); l != lhs.end(); l++, r++, d++) {
    *d = *l - *r;
  }

  return ret;
}

/**
 * Add one Vector to another
 *
 * @param lhs the left hand side of the addition
 * @param rhs the right hand side of the addition
 * @return the first plus the second
 */
template<int S, typename type>
Vector<S, type> operator+(const Vector<S, type>& lhs, const Vector<S, type>& rhs) {
  Vector<S, type> ret;

  for(auto l = lhs.begin(), r = rhs.begin(), d = ret.begin(); l != lhs.end(); l++, r++, d++) {
    *d = *l + *r;
  }

  return ret;
}

/**
 * Multiply a Vector by a scaler
 *
 * @param lhs the scaler that this Vector will be scaled by
 * @param rhs the Vector that will be scaled
 * @return a new Vector that is the other Vector scaled by lhs
 */
template<int S, typename type>
Vector<S, type> operator*(const double& lhs, const Vector<S, type>& rhs) {
  Vector<S, type> ret;

  for(auto d = ret.begin(), r = rhs.begin(); d != ret.end(); d++, r++) {
    *d = *r * lhs;
  }

  return ret;
}

/**
 * Multiply a Vector by a scaler
 *
 * @param lhs the Vector that will be scaled
 * @param rhs the scaler that the Vector will be scaled by
 * @return a new Vector that is the other Vector scaled by rhs
 */
template<int S, typename type>
Vector<S, type> operator*(const Vector<S, type>& lhs, const double& rhs) {
  Vector<S, type> ret;

  for(auto d = ret.begin(), l = lhs.begin(); d != ret.end(); d++, l++) {
    *d = *l * rhs;
  }

  return ret;
}

/**
 *
 *
 * @param istr
 * @param v
 * @return
 */
template<int S, typename type>
lexer& operator>>(lexer& istr, Vector<S, type>& v) {
  string tmp;

  for(int i = 0; i < 3; i++) {
    istr >> v[i];
  }

  istr >> tmp;
  if(tmp != "EndVertex") {
    throw exception();
  }

  return istr;
}

/**
 *
 *
 * @param ostr
 * @param v
 * @return
 */
template<int S, typename type>
ostream& operator<<(ostream& ostr, const Vector<S, type>& v) {
  for(auto iter = v.begin(); iter != v.end(); iter++) {
    ostr << *iter << " ";
  }
  return ostr;
}

#endif /* VECTOR_TPP_INCLUDE */
