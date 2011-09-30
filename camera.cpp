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

#include <camera.h>
#include <lexer.h>

#include <algorithm>
using std::for_each;
using std::min;
using std::max;
#include <boost/bind.hpp>
#include <cmath>
#include <exception>
using std::exception;
#include <iostream>
using std::cout;
using std::endl;
using std::flush;
#include <limits>
using std::numeric_limits;
#include <sstream>
using std::ostringstream;
#include <string>
using std::string;
#include <thread>
#include <tuple>
using std::tuple;
using std::get;

#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>

#define MAX_DEPTH 700
#define X_PRINT 0
#define Y_PRINT 0

/* intialize statics */
#ifdef DEBUG
bool camera::print = false;
#else
concurrent_queue<ray> ray::rays;
int camera::running = 0;
std::condition_variable_any wait_on;
std::mutex                  lock_on;
unsigned int                numb_on;
int                         block_on;

/**
 * Simple wrapper function passed into the creation of threads
 */
void wrapper() {
  ray::rays.worker();
  camera::running--;
}

#endif

/**
 * Entry function for the ray tracing process. This takes a model and uses it to
 * generate an images and save it to a file named output.png
 *
 * @param m
 * @return
 */
void camera::click(const model* m) {
  /* locals */
  cv::Mat raw_image(vmax() - vmin() + 1, umax() - umin() + 1, CV_8UC3);
  vector<std::thread*> threads;
  Vector<3> U;
  point L;

  /* two different versions of this function can be compiled.
   *   1. A debugging version that runs purely in the main thread. This has
   *      the advantage that it can print all the information for a specific
   *      pixel. Simply set X_PRINT and Y_PRINT before compiling, and this
   *      if one checks the print variable, they can get any information
   *      about a particular pixel
   *
   *   2. The standard threaded version. This has evolved over time into
   *      the current version. Currently this will display an image to the
   *      screen and show the rendering process in real time. It will check
   *      the number of threads available on the system. One thread will be
   *      reserved for the display (the main thread). The other threads will
   *      be allocated to the rendering process.
   */
  for(int x = umin(); x <= umax(); x++) {
    for(int y = vmin(); y <= vmax(); y++) {
      L = vrp() + x*u() + y*v();
      U = L - focal_point(); U.normalize();
#ifdef DEBUG
      if(x == X_PRINT && y == Y_PRINT)
        print = true;
      ray(m, this, L, U, raw_image.at<Vector<3, uc> >(vmax() - y, x - umin()))();
      if(x == X_PRINT && y == Y_PRINT)
        print = false;
    }
  }

  Vector<3, uc> fill(255);
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() - 1, Y_PRINT - vmin() - 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() - 1, Y_PRINT - vmin() + 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() - 1, Y_PRINT - vmin()    ) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin()    , Y_PRINT - vmin() - 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin()    , Y_PRINT - vmin() + 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() + 1, Y_PRINT - vmin() - 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() + 1, Y_PRINT - vmin() + 1) = fill;
  raw_image.at<Vector<3, uc> >(X_PRINT - umin() + 1, Y_PRINT - vmin()    ) = fill;
#else
      ray::rays.push(new ray(m, this, L, U, raw_image.at<Vector<3, uc> >(vmax() - y, x - umin())));
    }
  }

  numb_on = 1;
  block_on = 1;

  int n_thread = std::max(int(std::thread::hardware_concurrency()- 1), 1);
  for(int i = 0; i < n_thread; i++) {
    running++;
    threads.push_back(new std::thread(wrapper));
  }

  while(running) {
    cv::imshow("win", raw_image);
    if(numb_on == std::thread::hardware_concurrency()) {
      cv::waitKey(0);
      block_on++;
      wait_on.notify_all();
      while(numb_on != 1)
        usleep(100);
    }

    cv::waitKey(30);
  }

  for(int i = 0; i < n_thread; i++) {
    threads[i]->join();
  }

  std::cout << "hello?" << std::endl;
  cv::imshow("win", raw_image);
  cv::waitKey(-1);
#endif

  /* create the output image */
  cv::imwrite("output.png", raw_image);
}

/**
 * Calculates the color of a specific ray. This will check what object (if any)
 * the ray intersects with. Once found this will call the function to calculate
 * the color of the light effecting the pixel and return it.
 *
 * @param r the ray that needs the color calculated
 * @return the color change based upon the input ray
 */
Vector<3> camera::ray_color(ray* r) const {
  tuple<point, double, const surface*> i, t;
  get<1>(i) = numeric_limits<double>::infinity();
  double cont = r->cont();

  for(auto iter = r->world()->begin(); iter != r->world()->end(); iter++) {
    t = (*iter)->intersection(r->dir(), r->src(), r->surf());

    if(get<1>(t) > 0 && get<1>(t) < get<1>(i)) {
      i = t;
    }
  }

  if(get<2>(i) != NULL) {
    return cont * reflectance(
        r,
        get<0>(i),
        get<2>(i)->normal(get<0>(i)),
        r->world()->mat(get<2>(i)->material()),
        get<2>(i));
  }

  r->cont() = 0;
  return Vector<3>(0);
}

/**
 * Calculates the color that a particular ray of light is. This will calculate
 * the color based upon all non-shadowed light sources, creates a new ray that
 * is the bounce of the original, and clip the color to within the 0-255 range
 * of a standard image.
 *
 * @param r pointer to the ray that the color is being calculated for
 * @param p the location on the surface that the ray intersected
 * @param n the normal to the surface at the point of intersection
 * @param mat the material of the surface
 * @param s the surface that it intersected
 * @return Vector<3> that is the color of the ray
 */
Vector<3> camera::reflectance(ray* r, point p, Vector<3> n, const material& mat, const surface* s) const {
  Vector<3> Lp, Rp(4), Rl(4);
  Vector<3> ret;
  Vector<3> v = r->dir();
  v.normalize();
  n.normalize();
  v.negate();

  if(v.dot(n) < 0) {
    n.negate();
  }

  /* add each light the red, green and blue values */
  for(auto light = r->world()->lbegin(); light != r->world()->lend(); light++) {
    /* calculate the direction of the light source and angle of reflectance*/
    Lp = light->direction(p); Lp.normalize();
    /* calculate the actual reflectance values */
    if(Lp.dot(n) < 0 || shadowed(p, light->direction(p), r->world(), s)) {
      continue;
    }

    Rl = (2 * (Lp.dot(n))) * n - Lp; Rl.normalize();
    ret += (mat.diffuse() * light->illumination() * Lp.dot(n)) + (light->illumination() * mat.ks() * pow(max(0.0, v.dot(Rl)), mat.alpha()));
  }

  /* recursively calculate new rays */
  Rp = (2 * (v.dot(n))) * n - v; Rp.normalize();
  r->dir()  = Rp;
  r->src()  = p;
  r->surf() = s;
  r->cont() = mat.ks() * r->cont();
  r->depth()++;

  /* clip the colors */
  ret[0] = min(int(ret[0]), 255);
  ret[1] = min(int(ret[1]), 255);
  ret[2] = min(int(ret[2]), 255);

  return ret;
}

/**
 * Checks if a particular light source is shadowed.
 *
 * @param pt the location of the surface that the ray intersected
 * @param U the direction of the light from the point pt
 * @param m the model that is being rendered
 * @param s the surface that the current ray bounced off of
 * @return true if the light source is shadowed for point pt
 */
bool camera::shadowed(const point& pt, const Vector<3>& U, const model* m, const surface* s) const {
  tuple<point, double, const surface*> inter;
  Vector<3> tmp = U;
  tmp.normalize();

  for(auto iter = m->begin(); iter != m->end(); iter++) {
    inter = (*iter)->intersection(tmp, pt, s);

    if(get<1>(inter) > 0 && get<1>(inter) < U.length()) {
      return true;
    }
  }

  return false;
}

/**
 * calculates the color of a ray and adds it to the ray pixel. This is the
 * () operator because the ray is being executed by one of the worker threads
 *
 * @return true if ray is still valid, false if it has finished calculation
 */
bool ray::operator()() {
  /*if(_depth == block_on) {
    std::unique_lock<std::mutex> lock(lock_on);
    numb_on++;
    wait_on.wait(lock);
    numb_on--;
  }*/
  _pixel += _generator->ray_color(this);
  _pixel[0] = min(int(_pixel[0]), 255);
  _pixel[1] = min(int(_pixel[1]), 255);
  _pixel[2] = min(int(_pixel[2]), 255);

#ifdef DEBUG
  if(!(_cont < 0.0039 || _depth > MAX_DEPTH ||
      (_pixel[0] == 255 && _pixel[1] == 255 && _pixel[2] == 255)))
    return this->operator()();
  return false;
#else
  return !(_cont < 0.0039 || _depth > MAX_DEPTH ||
        (_pixel[0] == 255 && _pixel[1] == 255 && _pixel[2] == 255));
#endif
}

/**
 * Reads a camera from the input object file.
 *
 * @param istr the lexer to read the camera from
 * @param c the camera to read the information into
 * @return the lexer that was given as a parameter
 */
lexer& operator>>(lexer& istr, camera& c) {
  string tmp;
  Vector<3> look_at;
  Vector<3> up;

  istr >> look_at[0] >> look_at[1] >> look_at[2];
  istr >> up[0] >> up[1] >> up[2];
  istr >> c.focal_point()[0] >> c.focal_point()[1] >> c.focal_point()[2];
  istr >> c.focal_length();
  istr >> c.umin() >> c.umax();
  istr >> c.vmin() >> c.vmax();
  istr >> tmp;

  if(tmp != "EndCamera") {
    throw exception();
  }

  c.n() = look_at;
  c.n().normalize();
  c.u() = up.cross(look_at);
  c.u().normalize();
  c.v() = c.n().cross(c.u());

  c.vrp() = c.focal_point() + (-c.focal_length())*c.n();
  return istr;
}
