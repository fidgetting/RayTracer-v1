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

#ifndef MODEL_H_INCLUDE
#define MODEL_H_INCLUDE

#include <matrix.tpp>
#include <shape.h>
#include <surface.h>
#include <object.h>
#include <lexer.h>
#include <Vector.tpp>

#include <map>
using std::map;
#include <tuple>
using std::tuple;
#include <vector>
using std::vector;

class material {
  public:
    
    material() { }
    virtual ~material() { }
    
    inline string& name() { return _name; }
    inline string name() const { return _name; }
    inline double& ks() { return _ks; }
    inline double ks() const { return _ks; }
    inline double& alpha() { return _alpha; }
    inline double alpha() const { return _alpha; }
    inline double& kt() { return _kt; }
    inline double kt() const { return _kt; }
    inline double& density() { return _density; }
    inline double density() const { return _density; }
    inline Matrix<3, 3>& diffuse() { return _diffuse; }
    inline Matrix<3, 3> diffuse() const { return _diffuse; }
  
  protected:
  
    string       _name;
    double       _ks, _alpha;
    double       _kt, _density;
    Matrix<3, 3> _diffuse;
};

class light {
  public:

    light() : _illumination(3), _position(4) { }
    virtual ~light() { }

    inline Vector<3>& illumination() { return _illumination; }
    inline Vector<3> illumination() const { return _illumination; }
    inline Vector<4>& position() { return _position; }
    inline Vector<4> position() const { return _position; }

    point direction(point src) const;

  protected:
    Vector<3> _illumination;
    Vector<4> _position;

};

class model {
  public:

    typedef vector<sphere*>::iterator iterator;
    typedef vector<sphere*>::const_iterator const_iterator;
    typedef vector<light>::iterator literator;
    typedef vector<light>::const_iterator const_literator;

    model(map<string, shape*> Shapes, vector<object*> objs, vector<light> lights, map<string, material> mats);
    virtual ~model();

    inline iterator begin() { return _surfaces.begin(); }
    inline const_iterator begin() const { return _surfaces.begin(); }
    inline iterator end() { return _surfaces.end(); }
    inline const_iterator end() const { return _surfaces.end(); }
    inline literator lbegin() { return _lights.begin(); }
    inline const_literator lbegin() const { return _lights.begin(); }
    inline literator lend() { return _lights.end(); }
    inline const_literator lend() const { return _lights.end(); }
    inline vector<light>& lights() { return _lights; }
    inline const vector<light>& lights() const { return _lights; }
    inline int  size() const { return _surfaces.size(); }

    material& mat(const string& name);
    material mat(const string& name) const;

  protected:

    vector<sphere*>       _surfaces;
    vector<light>         _lights;
    vector<int>           _illumination;
    map<string, material> _materials;
};

lexer& operator>>(lexer& istr, material& m);
lexer& operator>>(lexer& istr, light& l);

#endif /* MODEL_H_INCLUDE */
