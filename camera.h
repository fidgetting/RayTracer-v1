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

#ifndef CAMERA_H_INCLUDE
#define CAMERA_H_INCLUDE

#include <lexer.h>
#include <model.h>
#include <queue.tpp>
#include <Vector.tpp>

#include <utility>
using std::pair;

typedef unsigned char uc;
class camera;

/**
 * the ray class is a key aspect of the ray tracing process. A ray is a vector
 * with the addition of source location for the vector. My implementation of a
 * ray also holds pointers the model and camera that are used in the ray tracing
 * process, and a reference to the pixel that is effects in the destination
 * image.
 *
 * @file camera.h
 */
class ray {
  public:

    /**
     * Basic constructor for the ray class.
     *
     * @param _m     the model that is being rendered
     * @param _gen   the camera that is taking a picture of the model
     * @param _src_p the location that the ray is extending from
     * @param _dir   the direction that the ray is pointing in
     * @param _pixel the pixel is the destination image that this ray changes
     */
    ray(const model* _m, const camera* _gen, const point& _src_p,
        const Vector<3>& _dir, Vector<3, uc>& _pixel) :
      _m(_m),     _generator(_gen), _src_point(_src_p), _direction(_dir), _pixel(_pixel),
      _src(NULL), _cont(1.0),       _depth(0),          _density(1.0) { }

    /**
     * Destructor, virtual in case someone could think of a reason to extend ray
     */
    virtual ~ray() { }

    bool operator()();

    /* ********************************************************************** */
    /* *** Getters and Setters ********************************************** */
    /* ********************************************************************** */

    inline const model*    world()   const { return _m;         }
    inline point&          src()           { return _src_point; }
    inline point           src()     const { return _src_point; }
    inline Vector<3>&      dir()           { return _direction; }
    inline Vector<3>       dir()     const { return _direction; }
    inline const surface*& surf()          { return _src;       }
    inline const surface*  surf()    const { return _src;       }
    inline double&         cont()          { return _cont;      }
    inline double          cont()    const { return _cont;      }
    inline int&            depth()         { return _depth;     }
    inline int             depth()   const { return _depth;     }
    inline double&         density()       { return _density;   }
    inline double          density() const { return _density;   }

    static concurrent_queue<ray> rays;

  protected:

    const model*    _m;         ///< model that is begin rendered
    const camera*   _generator; ///< camera taking a picture of the model
    point           _src_point; ///< the origin of the ray
    Vector<3>       _direction; ///< the direction the ray travels in
    Vector<3, uc>&  _pixel;     ///< refernce to the pixel this ray effects
    const surface*  _src;       ///< the surface this ray bounced off of
    double          _cont;      ///< how much the ray effects the pixel
    int             _depth;     ///< the number of bounces before this ray
    double          _density;   ///< ???
};

/**
 * The camera class contains almost all of the work-horse functions for the ray
 * tracing process. To used, create and read a camera and a model. Simply call
 * click() on the model and a file will be produced that is the ray traced
 * image.
 *
 * @file camera.h
 */
class camera {
  public:
    camera() : fp(4), _n(4), _u(4), _v(4) { };
    virtual ~camera() { };

    inline point& focal_point() { return fp; }
    inline point focal_point() const { return fp; }
    inline point& vrp() { return _vrp; }
    inline point vrp() const { return _vrp; }
    inline Vector<3>& n() { return _n; }
    inline Vector<3> n() const { return _n; }
    inline Vector<3>& u() { return _u; }
    inline Vector<3> u() const { return _u; }
    inline Vector<3>& v() { return _v; }
    inline Vector<3> v() const { return _v; }
    inline double& focal_length() { return fl; }
    inline double focal_length() const { return fl; }
    inline int& umin() { return _umin; }
    inline int umin() const { return _umin; }
    inline int& umax() { return _umax; }
    inline int umax() const { return _umax; }
    inline int& vmin() { return _vmin; }
    inline int vmin() const { return _vmin; }
    inline int& vmax() { return _vmax; }
    inline int vmax() const { return _vmax; }

    void click(const model* m);
    Vector<3> ray_color(ray* r) const;

#ifdef DEBUG
    static bool print;
#endif

    static int running;

  protected:

    Vector<3> reflectance(ray* r, point p, Vector<3> n, const material& mat, const surface* s) const;
    bool shadowed(const point& pt, const Vector<3>& dir, const model* m, const surface* s) const;

    point fp, _vrp;
    Vector<3> _n, _u, _v;
    double fl;
    int _umin, _umax;
    int _vmin, _vmax;
};

lexer& operator>>(lexer& istr, camera& c);

#endif /* CAMERA_H_INCLUDE */
