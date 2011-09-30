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

#include <model.h>

#include <algorithm>
using std::remove_if;
#include <exception>
using std::exception;
#include <iostream>
using std::cerr;

model::model(map<string, shape*> Shapes, vector<object*> objs, vector<light> lights, map<string, material> mats)
    : _surfaces(), _lights(lights), _materials(mats) {
  for(auto iter = objs.begin(); iter != objs.end(); iter++) {
    shape* base = Shapes[(*iter)->shape()];
    Matrix<4, 4> transform = identity<4>();
    Vector<3> tmp;

    for(auto tran = (*iter)->rbegin(); tran != (*iter)->rend(); tran++) {
      transform *= **tran;
    }

    for(auto poly = base->pbegin(); poly != base->pend(); poly++) {
      polygon* newPoly = new polygon();

      for(auto v = poly->begin(); v != poly->end(); v++) {
        tmp = transform * (*v);
        newPoly->add_vertex(tmp);
      }

      newPoly->normal() = transform * poly->normal();
      newPoly->material() = (*iter)->material();

      auto found = _surfaces.end();
      sphere s(newPoly->center(), newPoly->radius());
      for(auto get = _surfaces.begin(); get != _surfaces.end(); get++) {
        if(**get == s) {
          found = get;
          break;
        }
      }

      if(found == _surfaces.end()) {
        sphere* newSphere = new sphere();
        newSphere->center() = newPoly->center();
        newSphere->radius() = newPoly->radius();
        newSphere->push_back(newPoly);
        _surfaces.push_back(newSphere);
      } else {
        (*found)->push_back(newPoly);
      }
    }
    for(auto siter = ((shape*)base)->sbegin(); siter != ((shape*)base)->send(); siter++) {
      sphere* newSphere = new sphere();

      newSphere->center()   = transform * siter->center();
      newSphere->radius()   = siter->radius() * (*iter)->scale();
      newSphere->material() = (*iter)->material();
      _surfaces.push_back(newSphere);
    }

    delete *iter;
  }

  /* clean up memory passed to Model::Model() */
  for(auto iter = Shapes.begin(); iter != Shapes.end(); iter++) {
    delete iter->second;
  }
}

model::~model() {
  for(auto iter = _surfaces.begin(); iter != _surfaces.end(); iter++) {
    delete *iter;
  }
}

material& model::mat(const string& name) {
  if(_materials.find(name) == _materials.end())
    throw exception();
  return _materials[name];
}

material model::mat(const string& name) const {
  if(_materials.find(name) == _materials.end())
    throw exception();
  return _materials.find(name)->second;
}

point light::direction(point src) const {
  point ret;

  ret[0] = _position[0];
  ret[1] = _position[1];
  ret[2] = _position[2];

  if(_position[3] == 0) {
    ret.negate();
  } else {
    ret = ret - src;
  }

  return ret;
}

lexer& operator>>(lexer& istr, material& m) {
  string t1, t2;
  
  // read in the material name and "Lambertian"
  istr >> m.name() >> t1;
  if(t1 != "Lambertian") {
    throw exception();
  }
  
  // read in the lamberitan numbers and "EndLambertian" and "Specular"
  istr >> m.diffuse()[0][0] >> m.diffuse()[1][1] >> m.diffuse()[2][2] >> t1 >> t2;
  if(t1 != "EndLambertian" || t2 != "Specular") {
    throw exception();
  }
  
  // read in the specular numbers and "EndSpecular" and "Translucence"
  istr >> m.ks() >> m.alpha() >> t1 >> t2;
  if(t1 != "EndSpecular" || t2 != "Translucence") {
    throw exception();
  }
  
  // read in the translucence numbers "EndTranslucence" and "EndMaterial"
  istr >> m.kt() >> m.density() >> t1 >> t2;
  if(t1 != "EndTranslucence" || t2 != "EndMaterial") {
    throw exception();
  }
  
  return istr;
}

lexer& operator>>(lexer& istr, light& l) {
  string t1, t2;

  // read the initial type
  istr >> t1;
  if(t1 != "Illuminant") {
    throw exception();
  }

  // read in the illumination
  istr >> l.illumination()[0] >> l.illumination()[1] >> l.illumination()[2] >> t1 >> t2;
  if(t1 != "EndIlluminant" || t2 != "Position") {
    throw exception();
  }

  // read in the position information
  istr >> l.position()[0] >> l.position()[1] >> l.position()[2] >> l.position()[3] >> t1 >> t2;
  if(t1 != "EndPosition" || t2 != "EndLightSource") {
    throw exception();
  }

  return istr;
}
