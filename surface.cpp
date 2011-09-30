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

#include <surface.h>
#include <matrix.tpp>
#include <camera.h>

#include <algorithm>
using std::min;
using std::max;
#include <cmath>
#include <exception>
using std::exception;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <limits>
using std::numeric_limits;

int surface::id_gen = 0;

/**
 * Check if two spheres are close enough together that we can justify placing them under the
 * same super sphere. A Sphere is close to this sphere if the distance between their centers
 * is smaller than their summed radius's.
 *
 * @param oth the sphere to compare this sphere to
 * @return true if they are close, false otherwise
 */
bool sphere::close(const sphere& oth) const {
  return (oth.center().distance(center()) < oth.radius() + radius());
}

/**
 * Calculate the intersection between a ray and a sphere.
 *
 * @param U
 * @param L
 * @param skip
 * @return
 */
tuple<point, double, const surface*> sphere::intersection(const Vector<3>& U, const point& L, const surface* skip) const {
  tuple<point, double, const surface*> i(point(0), numeric_limits<double>::infinity(), (const surface*)NULL), tmp;
  double s, t_sq, r_sq, m_sq, q;
  Vector<3> T = center() - L;

  if(skip == this && U.dot(normal(L)) > 0) {
    return i;
  }

  /* perform first check to see if we hit the circle */
  s = T.dot(U);
  t_sq = T.dot(T);
  r_sq = radius() * radius();
  if(s < 0 && t_sq > r_sq) {
    return i;
  }

  /* second easy rejection check */
  m_sq = t_sq - s*s;
  if(m_sq > r_sq) {
    return i;
  }

  /* we now know that the ray will intersect the sphere, fork for subsurfaces */
  if(_subsurfaces.size() == 0) {
    q = sqrt(r_sq - m_sq);
    if(t_sq > r_sq && (this != skip || U.dot(normal(L)) >= 0)) {
      s -= q;
    } else {
      s += q;
    }

    return tuple<point, double, const surface*>(L + s*U, s, this);
  }

  /* this sphere does have subsurfaces, find the closest and return its intersection */
  for(auto iter = _subsurfaces.begin(); iter != _subsurfaces.end(); iter++) {
    if(*iter != skip) {
      tmp = (*iter)->intersection(U, L, skip);

      if(get<1>(tmp) >= 0 && get<1>(tmp) < get<1>(i)) {
        i = tmp;
      }
    }
  }

  return i;
}

tuple<point, double, const surface*> polygon::intersection(const Vector<3>& U, const point& L, const surface* skip) const {
  Matrix<3, 4> m;
  Vector<3> A(_vertices[0]), B, C;

  if(skip != this) {
    for(int i = 1; i < size() - 1; i++) {
      B = _vertices[i];
      C = _vertices[i + 1];


      m[0][0] = A[0] - B[0]; m[0][1] = A[0] - C[0]; m[0][2] = U[0]; m[0][3] = A[0] - L[0];
      m[1][0] = A[1] - B[1]; m[1][1] = A[1] - C[1]; m[1][2] = U[1]; m[1][3] = A[1] - L[1];
      m[2][0] = A[2] - B[2]; m[2][1] = A[2] - C[2]; m[2][2] = U[2]; m[2][3] = A[2] - L[2];
      m.gaussian_elimination();

      if(m[0][3] >= 0 && m[1][3] >= 0 && m[2][3] >= 0 && m[0][3] + m[1][3] < 1) {
        return tuple<point, double, const surface*>(L + m[2][3]*U, m[2][3], this);
      }
    }
  }

  return tuple<point, double, const surface*>(point(), -1, (const surface*)NULL);
}

point polygon::center() const {
  point center(0);

  for(auto iter = _vertices.begin(); iter != _vertices.end(); iter++) {
    center += *iter;
  }

  center /= _vertices.size();
  return center;
}

double polygon::radius() const {
  double rad = 0, d;
  point c = center();

  for(auto iter = begin(); iter != end(); iter++) {
    d = c.distance(*iter);
    rad = max(d, rad);
  }

  return rad;
}

lexer& operator>>(lexer& istr, pre_polygon& poly) {
  Vector<3> n, z, curr;
  string tmp;

  for(istr >> tmp; tmp == "Vertex" && !istr.fail(); istr >> tmp) {
    Vector<4> v;
    istr >> v;
    v[3] = 1;
    poly.add_vertex(v);
  }

  if(poly.size() < 3) {
    throw exception();
  }

  /* make sure that the polygon is coplanar and convext */
  poly.normal() = (poly[1] - poly[0]).cross(poly[2] - poly[0]);
  n = poly.normal();
  n.normalize();
  bool colinear = true;
  for(int i = 2; i < poly.size(); i++) {
    curr = (poly[i] - poly[i - 1]).cross(poly[(i + 1)%poly.size()] - poly[i - 1]);
    curr.normalize();
    if(curr != n) {
      throw exception();
    } else if(curr != z) {
      colinear = false;
    }
  }

  if(tmp != "EndPolygon" || colinear) {
    throw exception();
  }

  return istr;
}

lexer& operator>>(lexer& istr, pre_sphere& sphere) {
  string tmp;

  istr >> tmp;
  if(tmp != "Vertex") {
    throw exception();
  }

  istr >> sphere.center() >> tmp;
  if(tmp != "Radius") {
    throw exception();
  }

  istr >> sphere.radius();

  istr >> tmp;
  if(tmp != "EndSphere") {
    throw exception();
  }

  sphere.center()[3] = 1;
  return istr;
}
