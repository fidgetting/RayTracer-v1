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

#include <shape.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <exception>
using std::exception;

/**
 * operator to read a shape from a Lexer. A Lexer is used here instead of an
 * input stream is used here since line numbers must be counted for error
 * reporting. This will read however many Polygons are listed in the Shape
 * declaration, and will read any spheres that are held in the shape.
 *
 * @param istr the Lexer that the shape will be read from
 * @param shape the shape that the information should be put into
 * @return stander >> operator, Lexer that is passed in will be returned
 */
lexer& operator>>(lexer& istr, shape& shape) {
  string curr;

  istr >> shape.name();

  for(istr >> curr; curr != "EndShape" && !istr.fail(); istr >> curr) {
    if(curr == "Polygon") {
      pre_polygon poly;
      istr >> poly;
      shape.add_polygon(poly);
    } else if(curr == "Sphere") {
      pre_sphere sphere;
      istr >> sphere;
      shape.add_sphere(sphere);
    } else {
      throw exception();
    }
  }

  if(curr != "EndShape") {
    throw exception();
  }

  return istr;
}
