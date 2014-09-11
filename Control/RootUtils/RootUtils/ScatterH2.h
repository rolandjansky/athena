// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScatterH2.h,v 1.5 2008-01-17 20:56:37 ssnyder Exp $

/**
 * @file  RootUtils/ScatterH2.h
 * @author scott snyder
 * @date Jan, 2008, from earlier analysis code.
 * @brief A 2-D histogram that can draw a proper scatter plot.
 */


#ifndef ROOTUTILS_SCATTERH2_H
#define ROOTUTILS_SCATTERH2_H


#include "TH2.h"
#include <vector>
class TArrayD;
class TMemberInspector;


namespace RootUtils {


/**
 * @brief A 2-D histogram that can draw a proper scatter plot.
 *
 * This is a variation on @c TH2F that can draw itself as a proper
 * scatter plot --- that is, each point really corresponds
 * exactly to one @c Fill() call.  (The default behavior of @c TH2F
 * bins the histogram, and then dithers in random points when
 * asked to draw a scatter plot.)
 *
 * This is, in essence, a combination of @c TGraph2D and @c TH2F.
 *
 * Use @c scatter() to control whether we plot as a real scatter plot,
 * or if we get the default root behavior.
 *
 * Note that the implementation requires saving the values
 * of all the points.  Beware if you're trying to make
 * plots with a really huge number of points.
 *
 * If scatter plotting is on, it will be drawn by putting the default
 * polymarker at each point.  Note that once the plot starts getting dense,
 * this can hide the structure of the distribution: if all points are
 * set, then adding a new point makes no visible difference.  To help
 * with this, shading can also be used to show the distribution density.
 * This is controlled by the parameter @c shadestep.
 *
 * If @c shadestep is zero, no shading is done, and markers are drawn
 * with the default color.
 *
 * If @c shadestep is greater than zero, then the color used for each
 * pixel is ((N-1)*shadestep, 0, 0), (as (R,G,B)), where N is the number of
 * data points plotted on the pixel.
 *
 * If @c shadestep is less then zero, the shading is done similarly,
 * except that the scale factor is chosen to that the densest pixel
 * will have a R channel of 255.
 */
class ScatterH2
  : public TH2F
{
public:
  /**
   * @brief Default constructor.
   */
  ScatterH2 ();


  /**
   * @brief Constructor.
   * @param name     Histogram name.
   * @param title    Histogram title.
   * @param nbinsx   Number of bins on x-axis.
   * @param xlow     x-axis lower limit.
   * @param xup      x-axis upper limit.
   * @param nbinsy   Number of bins on y-axis.
   * @param ylow     y-axis lower limit.
   * @param yup      y-axis upper limit.
   */
  ScatterH2 (const char *name, const char *title,
             Int_t nbinsx, Axis_t xlow, Axis_t xup,
             Int_t nbinsy, Axis_t ylow, Axis_t yup);


  /**
   * @brief Constructor.
   * @param name     Histogram name.
   * @param title    Histogram title.
   * @param nbinsx   Number of bins on x-axis.
   * @param xbins    Array of bin boundaries.
   * @param nbinsy   Number of bins on y-axis.
   * @param ylow     y-axis lower limit.
   * @param yup      y-axis upper limit.
   */
  ScatterH2 (const char *name, const char *title,
             Int_t nbinsx, const Double_t* xbins,
             Int_t nbinsy, Axis_t ylow, Axis_t yup);


  /**
   * @brief Constructor.
   * @param name     Histogram name.
   * @param title    Histogram title.
   * @param xbins    Array of bin boundaries.
   * @param nbinsy   Number of bins on y-axis.
   * @param ylow     y-axis lower limit.
   * @param yup      y-axis upper limit.
   */
  ScatterH2 (const char *name, const char *title,
             const TArrayD& xbins,
             Int_t nbinsy, Axis_t ylow, Axis_t yup);


  /**
   * @brief Standard ROOT paint method.
   * @param option Paint options.  See @c TH2.
   *
   * This overrides the @c TH2 Paint method.
   * It suppresses the normal histogram plot if scatter mode is on.
   */
  virtual void Paint (Option_t* option = "");


  /**
   * @brief Standard ROOT reset method.
   * @param option Reset options.  See @c TH2.
   *
   * This overrides the @c TH2 Reset method.
   * We may need to clear the saved points, too.
   */
  virtual void Reset (Option_t *option = "");


  /**
   * @brief Standard ROOT fill method.
   * @param x x-coordinate.
   * @param y y-coordinate.
   * @param w Weight.
   *
   * This overrides the @c TH2 Fill method.
   * The data point is saved in our list.
   */
  virtual Int_t Fill (Axis_t x, Axis_t y, Stat_t w);


  /**
   * @brief Standard ROOT fill method.
   * @param x x-coordinate.
   * @param y y-coordinate.
   */
  virtual Int_t   Fill(Axis_t x, Axis_t y);


  /**
   * @brief Get the current value of the scatter flag.
   *
   * If it's true, then this histogram will draw as a real scatter
   * plot.  Otherwise, the default root behavior is used.
   */ 
  bool scatter () const;


  /**
   * @brief Set the scatter flag.
   * @param flag The new value of the scatter flag.
   * @return The previous value of the scatter flag.
   *
   * If it's true, then this histogram will draw as a real scatter
   * plot.  Otherwise, the default root behavior is used.
   */ 
  bool scatter (bool flag);


  /**
   * @brief Get the current value of shadestep.
   *
   * If zero, no shading is done.
   * If greater than zero, pixel brightness is increased by this much
   * for each additional data point.
   * If less than zero, pixel brightness is scaled so that pixels
   * with the most data points have a brightness of 255.
   */
  int shadestep () const;


  /**
   * @brief Set value of shadestep.
   * @param shadestep The new value for shadestep.
   * @return The previous value of shadestep.
   *
   * If zero, no shading is done.
   * If greater than zero, pixel brightness is increased by this much
   * for each additional data point.
   * If less than zero, pixel brightness is scaled so that pixels
   * with the most data points have a brightness of 255.
   */
  int shadestep (int shadestep);


  /**
   * @brief Return a new plot with all data points multiplied by a constant.
   * @param name Name for the new plot.
   * @param xscale x-axis scale factor.
   * @param yscale y-axis scale factor.
   */
  ScatterH2* rescale (const char* name, double xscale, double yscale) const;

  
private:
  /// The scatter flag.
  /// If true, plot as a scatter plot.
  bool m_scatter;

  /// shadestep option. 
  /// If zero, no shading is done.
  /// If greater than zero, pixel brightness is increased by this much
  /// for each additional data point.

  /// If less than zero, pixel brightness is scaled so that pixels
  /// with the most data points have a brightness of 255.
  int m_shadestep;

  /// One point.  Avoid std::pair so that we don't have duplicate dicts.
public:
  struct Pair
  {
    float first;
    float second;
    Pair (float x=0, float y=0) : first (x), second (y) {}
  };
private:
  /// The collection of points that have been plotted.
  std::vector<Pair> m_vals;


private:
  // These Fill overloads don't make sense for this class.
  // Make them private here.
  virtual Int_t   Fill(Axis_t, const char*, Stat_t) { return -1; }
  virtual Int_t   Fill(const char*, Axis_t, Stat_t) { return -1; }
  virtual Int_t   Fill(const char*, const char*, Stat_t) { return -1; }
  Int_t   Fill(Axis_t) {return -1;}
  Int_t   Fill(const char*, Stat_t) {return -1;}


  ClassDef (RootUtils::ScatterH2, 1);
};


} // namespace RootUtils


#endif // not ROOTUTILS_SCATTERH2_H
