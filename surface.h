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

#ifndef SURFACE_H_INCLUDE
#define SURFACE_H_INCLUDE

/* local includes */
#include <Vector.tpp>
#include <lexer.h>

/* std library includes */
#include <iostream>
using std::ostream;
#include <string>
using std::string;
#include <tuple>
using std::tuple;
using std::get;
#include <vector>
using std::vector;

class surface {
  public:
    surface() : _id(id_gen++) { };
    virtual ~surface() { };

    virtual Vector<3> normal(const Vector<3>& v) const = 0;
    virtual tuple<point, double, const surface*> intersection(const Vector<3>& U, const point& L, const surface* skip) const = 0;
    virtual point center() const = 0;
    virtual double radius() const = 0;

    inline string& material() { return _material; }
    inline string material() const { return _material; }
    inline int id() const { return _id; }
    inline bool& src() { return _src; }
    inline bool src() const { return _src; }

  protected:

    string _material;
    int _id;
    bool _src;

    static int id_gen;
};

class sphere : public surface {
  public:

    typedef vector<surface*>::iterator iterator;
    typedef vector<surface*>::const_iterator const_iterator;

    sphere() { }
    sphere(const point& _center, double _radius) : _center(_center), _radius(_radius) { }
    virtual ~sphere() { for(auto iter = _subsurfaces.begin(); iter != _subsurfaces.end(); iter++) delete *iter; }

    inline point& center() { return _center; }
    inline double& radius() { return _radius; }
    inline unsigned int size() { return _subsurfaces.size(); }
    inline void clear() { _subsurfaces.clear(); }
    inline void push_back(surface* s) { _subsurfaces.push_back(s); }
    inline void insert(const iterator& pos, surface* s) { _subsurfaces.insert(pos, s); }
    inline void insert(const iterator& s, const iterator& e) { _subsurfaces.insert(end(), s, e); }
    bool close(const sphere& oth) const;

    inline iterator begin() { return _subsurfaces.begin(); }
    inline const_iterator begin() const { return _subsurfaces.begin(); }
    inline iterator end() { return _subsurfaces.end(); }
    inline const_iterator end() const { return _subsurfaces.end(); }

    virtual inline Vector<3> normal(const Vector<3>& v) const { return v - _center; }
    virtual tuple<point, double, const surface*> intersection(const Vector<3>& ray, const point& src, const surface* skip) const;
    virtual inline point center() const { return _center; }
    virtual inline double radius() const { return _radius; }

  protected:

    point            _center;       ///< the index of the center of the sphere within a shape
    double           _radius;       ///< the radius of the sphere
    vector<surface*> _subsurfaces;  ///< list of surfaces that are contained within this sphere
};

class polygon : public surface {
  public:

    typedef vector<point>::iterator iterator;
    typedef vector<point>::const_iterator const_iterator;

    polygon() : _vertices() { }
    virtual ~polygon() { }

    inline void add_vertex(Vector<3> v) { _vertices.push_back(v); }
    inline int size() const { return _vertices.size(); }
    inline Vector<3>& normal() { return _n; }
    inline string& material() { return _material; }
    inline string material() const { return _material; }
    inline iterator begin() { return _vertices.begin(); }
    inline const_iterator begin() const { return _vertices.begin(); }
    inline iterator end() { return _vertices.end(); }
    inline const_iterator end() const { return _vertices.end(); }
    inline point& operator[](int i) { return _vertices[i]; }
    inline point operator[](int i) const { return _vertices[i]; }

    virtual inline Vector<3> normal(const Vector<3>& v) const { if(v == v) return _n; return _n; }
    virtual tuple<point, double, const surface*> intersection(const Vector<3>& U, const point& L, const surface* skip) const;
    virtual point center() const;
    virtual double radius() const;

  protected:

    vector<point>             _vertices;  ///< the verticies that represent this polygon
    Vector<3>                 _n;         ///< TODO
};

class pre_sphere {
  public:
    pre_sphere() { }
    virtual ~pre_sphere() { }

    inline Vector<4>& center() { return _center; }
    inline Vector<4> center() const { return _center; }
    inline double& radius() { return _radius; }
    inline double radius() const { return _radius; }

  protected:

    Vector<4> _center;
    double    _radius;
};

class pre_polygon {
  public:

    typedef vector<Vector<4> >::iterator iterator;
    typedef vector<Vector<4> >::const_iterator const_iterator;

    pre_polygon() { }
    virtual ~pre_polygon() { }

    inline void add_vertex(Vector<4> v) { vertices.push_back(v); }
    inline int size() const { return vertices.size(); }
    inline Vector<4>& normal() { return n; }
    inline string& material() { return _material; }
    inline string material() const { return _material; }
    inline iterator begin() { return vertices.begin(); }
    inline const_iterator begin() const { return vertices.begin(); }
    inline iterator end() { return vertices.end(); }
    inline const_iterator end() const { return vertices.end(); }
    inline Vector<4>& operator[](int i) { return vertices[i]; }
    inline Vector<4> operator[](int i) const { return vertices[i]; }

  protected:

    vector<Vector<4> > vertices;
    Vector<4> n;
    string _material;
};

inline bool operator==(const sphere& lhs, const sphere& rhs) {
  return lhs.center() == rhs.center() && lhs.radius() == rhs.radius();
}

lexer& operator>>(lexer& istr, pre_polygon& poly);
lexer& operator>>(lexer& istr, pre_sphere& sphere);

#endif /* SURFACE_H_INCLUDE */
