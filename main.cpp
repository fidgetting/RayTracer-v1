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

/* local includes */
#include <shape.h>
#include <object.h>
#include <model.h>
#include <lexer.h>
#include <camera.h>

/* library includes */
#include <exception>
using std::exception;
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::cerr;
using std::flush;
#include <utility>
using std::pair;

pair<model*, camera*> parse(const char* filename) {
  lexer istr(filename);
  string curr;                        // the current type of object being loaded from the file
  map<string, shape*> shapes;         // the set of shape retrieved from the file
  map<string, material> materials;    // the set of materials for this model
  vector<object*> objects;            // the list of objects that will be in the model
  vector<light> lights;               // the list of lights for the model
  pair<model*, camera*> ret;          // the pair that will be returned by this function

  ret.first = NULL;
  ret.second = NULL;

  /* check if the file openned correctly */
  if(istr.fail()) {
    cerr << "ERROR: Could not open model file." << endl;
    cerr << "ERROR: filename was: " << filename << endl;
    return ret;
  }

  try {
    /* read the file into a model */
    for(istr >> curr; !istr.eof() && !istr.fail(); istr >> curr) {
      if(curr == "Shape") {
        shape* s = new shape();
        istr >> *s;
        shapes[s->name()] = s;
      } else if(curr == "Object") {
        object* obj = new object();
        istr >> *obj;
        objects.push_back(obj);
        if(shapes.find(obj->shape()) == shapes.end()) {
          throw exception();
        }
        if(materials.find(obj->material()) == materials.end()) {
          throw exception();
        }
        if(shapes[obj->shape()]->ssize() != 0) {
          if(!obj->uniform()) {
            throw exception();
          }
        }
      } else if(curr == "Camera" && ret.second == NULL) {
        ret.second = new camera();
        istr >> *(ret.second);
      } else if(curr == "LightSource"){
        light l;
        istr >> l;
        lights.push_back(l);
      } else if(curr == "Material") {
        material mat;
        istr >> mat;
        materials[mat.name()] = mat;
      } else {
        throw exception();
      }
    }
  } catch(exception& e) {
    cerr << "ERROR: invalid syntax found in model file." << endl;
    cerr << "ERROR: invalid syntax in: " << filename << endl;
    cerr << "ERROR: error found on line: " << istr.line_number() << endl;
    cerr << "ERROR: line reads: " << istr.line_stream().str() << endl;
    for(map<string, shape*>::iterator iter = shapes.begin();
        iter != shapes.end(); iter++)
      delete iter->second;
    for(vector<object*>::iterator iter = objects.begin();
        iter != objects.end(); iter++)
      delete *iter;
    return ret;
  }


  ret.first = new model(shapes, objects, lights, materials);
  return ret;
}

/* ************************************************************************** */
/* *** main function of ray tracer ****************************************** */
/* ************************************************************************** */

int main(int argc, char** argv) {
  for(int i = 1; i < argc; i++) {
    pair<model*, camera*> p = parse(argv[i]);
    if(p.second != NULL && p.first != NULL) {
      p.second->click(p.first);
    }
    delete p.first;
    delete p.second;
  }
  return 0;
}


