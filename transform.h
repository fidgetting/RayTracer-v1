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

#ifndef TRANSFORM_H_INCLUDE
#define TRANSFORM_H_INCLUDE

#include <matrix.tpp>

#include <iostream>
using std::ostream;
using std::cout;
using std::endl;

class scale : public Matrix<4, 4> {
  public:
    scale() : Matrix<4, 4>(identity<4>()), _uniform(false) { }
    virtual ~scale() { }

    inline bool& uniform() { return _uniform; }
    inline bool uniform() const { return _uniform; }

  protected:

    bool _uniform;
};

class translate : public Matrix<4, 4> {
  public:
    translate() : Matrix<4, 4>(identity<4>()) { }
    virtual ~translate() { }
};

class rotate : public Matrix<4, 4> {
  public:

    rotate() : Matrix<4, 4>(0) { }
    virtual ~rotate() { }
};

lexer& operator>>(lexer& istr, scale& s);
lexer& operator>>(lexer& istr, translate& t);
lexer& operator>>(lexer& istr, rotate& r);

ostream& operator<<(ostream& ostr, const scale& s);
ostream& operator<<(ostream& ostr, const translate& t);
ostream& operator<<(ostream& ostr, const rotate& r);

#endif /* TRANSFORM_H_INCLUDE  */
