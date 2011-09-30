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

#include <transform.h>

#include <cstdlib>
#include <cmath>
#include <exception>
using std::exception;
#include <iomanip>
using std::setprecision;
using std::setw;
#include <iostream>
using std::cerr;
using std::endl;
#include <string>
using std::string;

lexer& operator>>(lexer& istr, scale& s) {
  string tmp;

  istr >> s[0][0];
  istr >> tmp;
  if(tmp == "EndScale") {
    s[1][1] = s[2][2] = s[0][0];
  } else {
    s[1][1] = strtod(tmp.c_str(), NULL);
    istr >> s[2][2];
    istr >> tmp;
    if(tmp != "EndScale") {
      throw exception();
    }
  }

  if(s[0][0] == s[1][1] && s[1][1] == s[2][2]) {
    s.uniform() = true;
  }

  return istr;
}

lexer& operator>>(lexer& istr, translate& t) {
  string tmp;

  istr >> t[0][3];
  istr >> t[1][3];
  istr >> t[2][3];

  istr >> tmp;
  if(tmp != "EndTranslate") {
    throw exception();
  }

  return istr;
}

lexer& operator>>(lexer& istr, rotate& r) {
  string tmp;
  Vector<3> look_at(3);
  Vector<3> up(3);

  istr >> look_at[0];
  istr >> look_at[1];
  istr >> look_at[2];
  istr >> up[0];
  istr >> up[1];
  istr >> up[2];

  Vector<3> bz = look_at;
  bz.normalize();
  Vector<3> bx = up.cross(look_at);
  bx.normalize();
  Vector<3> by = bz.cross(bx);

  for(int i = 0; i < r.width() - 1; i++) {
    r[0][i] = bx[i];
    r[1][i] = by[i];
    r[2][i] = bz[i];
  }

  istr >> tmp;
  if(tmp != "EndRotate") {
    throw exception();
  }

  r[3][3] = 1;
  return istr;
}

ostream& operator<<(ostream& ostr, const scale& s) {
  ostr << "Scale:\n";
  return operator<<(ostr, *(const Matrix<4, 4>*)(&s));
}

ostream& operator<<(ostream& ostr, const translate& t) {
  ostr << "Translate:\n";
  return operator<<(ostr, *(const Matrix<4, 4>*)(&t));
}

ostream& operator<<(ostream& ostr, const rotate& r) {
  ostr << "Rotate:\n";
  return operator<<(ostr, *(const Matrix<4, 4>*)(&r));
}
