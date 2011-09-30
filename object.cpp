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

#include <object.h>

#include <exception>
using std::exception;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

object::~object() {
  for(auto iter = _transforms.begin(); iter != _transforms.end(); iter++) {
    delete *iter;
  }
}

lexer& operator>>(lexer& istr, object& obj) {
  string curr;

  istr >> obj.shape();
  istr >> obj.material();

  for(istr >> curr; curr != "EndObject" && !istr.fail(); istr >> curr) {
    if(curr == "Scale") {
      scale* s = new scale();
      istr >> *s;
      obj.addTransform(s);
      if(obj.uniform()) {
        if(s->uniform()) {
          obj.scale() *= (*s)[0][0];
        } else {
          obj.scale() = -1;
        }
      }
    } else if(curr == "Translate") {
      translate* t = new translate();
      istr >> *t;
      obj.addTransform(t);
    } else if(curr == "Rotate") {
      rotate* r = new rotate();
      istr >> *r;
      obj.addTransform(r);
    } else {
      throw exception();
    }
  }

  if(curr != "EndObject") {
    throw exception();
  }

  return istr;
}
