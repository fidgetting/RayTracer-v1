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

#ifndef SHAPE_H_INCLUDE
#define SHAPE_H_INCLUDE

/* local includes */
#include <surface.h>
#include <object.h>
#include <lexer.h>

/* library includes */
#include <iostream>
using std::ostream;
using std::cout;
using std::endl;
#include <map>
using std::map;
#include <string>
using std::string;
#include <vector>
using std::vector;

class shape {
  public:

    typedef vector<pre_polygon>::iterator piterator;
    typedef vector<pre_polygon>::const_iterator const_piterator;
    typedef vector<pre_sphere>::iterator siterator;
    typedef vector<pre_sphere>::const_iterator const_siterator;

    shape(const string& name = "") :
      _spheres(), _polygons(), _name(name) { }
    virtual ~shape() { }

    void add_polygon(const pre_polygon& poly) { _polygons.push_back(poly); }
    void add_sphere(const pre_sphere& sphere) { _spheres.push_back(sphere); }

    inline string& name() { return _name; }
    inline string name() const { return _name; }
    inline unsigned int ssize() const { return  _spheres.size(); }
    inline unsigned int psize() const { return _polygons.size(); }

    inline piterator pbegin() { return _polygons.begin(); }
    inline const_piterator pbegin() const { return _polygons.begin(); }
    inline siterator sbegin() { return _spheres.begin(); }
    inline const_siterator sbegin() const { return _spheres.begin(); }

    inline piterator pend() { return _polygons.end(); }
    inline const_piterator pend() const { return _polygons.end(); }
    inline siterator send() { return _spheres.end(); }
    inline const_siterator send() const { return _spheres.end(); }

  protected:

    vector<pre_sphere>  _spheres;           ///< the list of spheres that belong to the Shape
    vector<pre_polygon> _polygons;          ///< the list of polygons belonging to this Shape
    string _name;                           ///< the string name used to access the Shape
};

lexer& operator>>(lexer& istr, shape& shape);

#endif /* SHAPE_H_INCLUDE */
