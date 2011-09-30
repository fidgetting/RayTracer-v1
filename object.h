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

#ifndef OBJECT_H_INCLUDE
#define OBJECT_H_INCLUDE

#include <transform.h>
#include <lexer.h>
#include <matrix.tpp>

#include <iostream>
#include <string>
using std::string;
#include <vector>
using std::vector;

class object {
  public:

    typedef vector<Matrix<4, 4>* >::iterator iterator;
    typedef vector<Matrix<4, 4>* >::const_iterator const_iterator;
    typedef vector<Matrix<4, 4>* >::reverse_iterator reverse_iterator;
    typedef vector<Matrix<4, 4>* >::const_reverse_iterator const_reverse_iterator;

    object() : _uniform_scale(1) { }
    virtual ~object();

    inline void addTransform(Matrix<4, 4>* trans) { _transforms.push_back(trans); }

    inline bool uniform() const { return _uniform_scale >= 0; }
    inline double& scale() { return _uniform_scale; }
    inline double scale() const { return _uniform_scale; }
    inline string& shape() { return _shape; }
    inline string shape() const { return _shape; }
    inline string& material() { return _material; }
    inline string material() const { return _material; }

    inline iterator begin() { return _transforms.begin(); }
    inline const_iterator begin() const { return _transforms.begin(); }
    inline iterator end() { return _transforms.end(); }
    inline const_iterator end() const { return _transforms.end(); }
    inline reverse_iterator rbegin() { return _transforms.rbegin(); }
    inline const_reverse_iterator rbegin() const { return _transforms.rbegin(); }
    inline reverse_iterator rend() { return _transforms.rend(); }
    inline const_reverse_iterator rend() const { return _transforms.rend(); }

    inline Matrix<4, 4>* operator[](int idx) { return _transforms[idx]; }
    inline const Matrix<4, 4>* operator[](int idx) const { return _transforms[idx]; }
    inline unsigned int size() const { return _transforms.size(); }

  protected:

    double                 _uniform_scale;
    string                 _shape;
    string                 _material;
    vector<Matrix<4, 4>* > _transforms;
};

lexer& operator>>(lexer& istr, object& obj);

#endif /* OBJECT_H_INCLUDE */
