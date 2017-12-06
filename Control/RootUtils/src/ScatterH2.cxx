/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScatterH2.cxx,v 1.5 2008-01-17 20:56:38 ssnyder Exp $
/**
 * @file  RootUtils/src/ScatterH2.cxx
 * @author scott snyder
 * @date Jan, 2008, from earlier analysis code.
 * @brief A 2-D histogram that can draw a proper scatter plot.
 */


#include "RootUtils/ScatterH2.h"
#include "TVirtualPad.h"
#include "TPolyMarker.h"
#include "TColor.h"
#include "TArrayD.h"
#include "TList.h"
#include "TROOT.h"
#include "TClass.h"
#include "TMemberInspector.h"

#include <vector>
#include <cassert>


namespace RootUtils {


/**
 * @brief Default constructor.
 */
ScatterH2::ScatterH2 ()
  : m_scatter (true),
    m_shadestep (0)
{
}


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
ScatterH2::ScatterH2 (const char *name, const char *title,
                      Int_t nbinsx, Axis_t xlow, Axis_t xup,
                      Int_t nbinsy, Axis_t ylow, Axis_t yup)
  : TH2F (name, title, nbinsx, xlow, xup, nbinsy, ylow, yup),
    m_scatter (true),
    m_shadestep (0)
{
}


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
ScatterH2::ScatterH2 (const char *name, const char *title,
                      Int_t nbinsx, const Double_t* xbins,
                      Int_t nbinsy, Axis_t ylow, Axis_t yup)
  : TH2F (name, title, nbinsx, xbins, nbinsy, ylow, yup),
    m_scatter (true),
    m_shadestep (0)
{
}



/**
 * @brief Constructor.
 * @param name     Histogram name.
 * @param title    Histogram title.
 * @param xbins    Array of bin boundaries.
 * @param nbinsy   Number of bins on y-axis.
 * @param ylow     y-axis lower limit.
 * @param yup      y-axis upper limit.
 */
ScatterH2::ScatterH2 (const char *name, const char *title,
                      const TArrayD& xbins,
                      Int_t nbinsy, Axis_t ylow, Axis_t yup)
  : TH2F (name, title, xbins.GetSize()-1, xbins.GetArray(),
          nbinsy, ylow, yup),
    m_scatter (true),
    m_shadestep (0)
{
}


/**
 * @brief Standard ROOT paint method.
 * @param option Paint options.  See @c TH2.
 *
 * This overrides the @c TH2 Paint method.
 * It suppresses the normal histogram plot if scatter mode is on.
 */
void ScatterH2::Paint (Option_t* option /*= ""*/)
{
  if (!m_scatter) {
    TH2F::Paint (option);
    return;
  }

  // Muck with things to prevent the normal histogram output.
  double oldmax = GetMaximumStored ();
  double oldmin = GetMinimumStored ();
  SetMaximum (0);
  SetMinimum (0);

  // Get the frame, etc., drawn.
  TH2F::Paint (option);

  SetMaximum (oldmax);
  SetMinimum (oldmin);

  // Gotta to this to get the coordinates set up properly.
  gPad->Update();

  // Work around a root bug.
  // THistPainter::PaintTable2 will add a TPaveStats object
  // to the function list.  However, if we then try to paint
  // the same histo again, then there is code in THistPainter
  // that expects the things in the function list to derive
  // from TF1.  There's no checking on the downcast, so root
  // crashes...
  GetListOfFunctions()->Clear();

  // Get coordinate bounds.
  Double_t u, v;
  Double_t umin = gPad->GetUxmin();
  Double_t umax = gPad->GetUxmax();
  Double_t vmin = gPad->GetUymin();
  Double_t vmax = gPad->GetUymax();

  int pxmin = gPad->XtoAbsPixel (umin);
  int pxmax = gPad->XtoAbsPixel (umax);
  int pymin = gPad->YtoAbsPixel (vmin);
  int pymax = gPad->YtoAbsPixel (vmax);
  if (pxmin > pxmax) std::swap (pxmin, pxmax);
  if (pymin > pymax) std::swap (pymin, pymax);
  int pxsize = pxmax - pxmin + 1;
  int pysize = pymax - pymin + 1;
  std::vector<unsigned int> counts (pxsize * pysize);

  TVirtualPad* pad = gPad;

  // Count the number of points on each pixel.
  unsigned int maxcount = 0;
  unsigned int n = m_vals.size();
  for (unsigned int i=0; i<n; i++) {
    u = pad->XtoPad (m_vals[i].first);
    v = pad->YtoPad (m_vals[i].second);
    if (u < umin) u = umin;
    if (u > umax) u = umax;
    if (v < vmin) v = vmin;
    if (v > vmax) v = vmax;

    int px = pad->XtoAbsPixel (u);
    int py = pad->YtoAbsPixel (v);
    if (px < pxmin || px > pxmax || py < pymin || py > pymax) continue;
    unsigned ndx = (px-pxmin) + (py-pymin) * pxsize;
    assert (ndx < counts.size());
    ++counts[ndx];
    if (counts[ndx] > maxcount)
      maxcount = counts[ndx];
  }

  const unsigned int NCOLOR = 255;

  // Figure out shading.
  float step = 0;
  if (m_shadestep) {
    if (m_shadestep > 0)
      step = m_shadestep;
    else
      step = static_cast<float>(NCOLOR-1)/(maxcount-1);
  }

  // Count the number of points for each color.
  unsigned int ccounts[NCOLOR];
  std::uninitialized_fill (ccounts, ccounts+NCOLOR, 0);
  size_t ndxmax = counts.size();
  for (unsigned int i=0; i < ndxmax; i++) {
    unsigned int count = counts[i];
    if (count > 0) {
      unsigned int color = static_cast<unsigned int> ((count-1)*step);
      if (color >= NCOLOR) color = NCOLOR-1;
      ++ccounts[color];
    }
  }

  TPolyMarker* pms[NCOLOR];
  std::uninitialized_fill (pms, pms+NCOLOR, (TPolyMarker*)0);

  // Fill in the polymarkers.
  for (int py = pymin; py <= pymax; ++py) {
    for (int px = pxmin; px <= pxmax; ++px) {
      unsigned int ndx = (px-pxmin) + (py-pymin) * pxsize;
      assert (ndx < ndxmax);
      unsigned int count = counts[ndx];
      if (count > 0) {
        Double_t u = pad->AbsPixeltoX (px);
        Double_t v = pad->AbsPixeltoY (py);
        unsigned int color = static_cast<unsigned int> ((count-1)*step);
        if (color >= NCOLOR) color = NCOLOR-1;

        TPolyMarker* pm = pms[color];
        if (!pm) {
          pm = new TPolyMarker (ccounts[color]);
          ccounts[color] = 0;
          pm->SetMarkerStyle (GetMarkerStyle());
          pm->SetMarkerSize (GetMarkerSize());
          if (color != 0)
            pm->SetMarkerColor (TColor::GetColor (color, 0, 0));
          pms[color] = pm;
        }
        pm->SetPoint (ccounts[color]++, u, v);
      }
    }
  }

  // Draw the points.
  for (unsigned int i = 0; i < NCOLOR; i++) {
    if (pms[i]) {
      pms[i]->Paint();
      delete pms[i];
    }
  }
}


/**
 * @brief Standard ROOT reset method.
 * @param option Reset options.  See @c TH2.
 *
 * This overrides the @c TH2 Reset method.
 * We may need to clear the saved points, too.
 */
void ScatterH2::Reset (Option_t *option /*= ""*/)
{
  TString opt = option;
  if (!opt.Contains("ICE")) {
    // Clear out m_vals too, releasing memory.
    std::vector<Pair> tmp;
    m_vals.swap (tmp);
  }
  TH2F::Reset (option);
}


/**
 * @brief Standard ROOT fill method.
 * @param x x-coordinate.
 * @param y y-coordinate.
 * @param w Weight.
 *
 * This overrides the @c TH2 Fill method.
 * The data point is saved in our list.
 */
Int_t ScatterH2::Fill (Axis_t x, Axis_t y, Stat_t w)
{
  // Remember the point in m_vals.
  m_vals.push_back (Pair (x, y));
  return TH2F::Fill (x, y, w);
}


/**
 * @brief Standard ROOT fill method.
 * @param x x-coordinate.
 * @param y y-coordinate.
 */
Int_t ScatterH2::Fill(Axis_t x, Axis_t y)
{
  return Fill (x, y, 1.0);
}


/**
 * @brief Get the current value of the scatter flag.
 *
 * If it's true, then this histogram will draw as a real scatter
 * plot.  Otherwise, the default root behavior is used.
 */ 
bool ScatterH2::scatter () const
{
  return m_scatter;
}


/**
 * @brief Set the scatter flag.
 * @param flag The new value of the scatter flag.
 * @return The previous value of the scatter flag.
 *
 * If it's true, then this histogram will draw as a real scatter
 * plot.  Otherwise, the default root behavior is used.
 */ 
bool ScatterH2::scatter (bool flag)
{
  bool x = m_scatter;
  m_scatter = flag;
  return x;
}


/**
 * @brief Get the current value of shadestep.
 *
 * If zero, no shading is done.
 * If greater than zero, pixel brightness is increased by this much
 * for each additional data point.
 * If less than zero, pixel brightness is scaled so that pixels
 * with the most data points have a brightness of 255.
 */
int ScatterH2::shadestep() const
{
  return m_shadestep;
}


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
int ScatterH2::shadestep (int shadestep)
{
  int x = m_shadestep;
  m_shadestep = shadestep;
  return x;
}


ScatterH2*
ScatterH2::rescale (const char* name, double xscale, double yscale) const
{
  int nx = GetXaxis()->GetNbins();
  double xlo = GetXaxis()->GetXmin();
  double xhi = GetXaxis()->GetXmax();

  int ny = GetYaxis()->GetNbins();
  double ylo = GetYaxis()->GetXmin();
  double yhi = GetYaxis()->GetXmax();

#if 0
  ScatterH2* hnew = new ScatterH2 (name, GetTitle(),
                                   nx, xscale*xlo, xscale*xhi,
                                   ny, yscale*ylo, yscale*yhi);
#endif
  ScatterH2* hnew = dynamic_cast<ScatterH2*> (this->Clone (name));
  if (hnew == 0)
    return 0;
  hnew->GetXaxis()->Set (nx, xscale*xlo, xscale*xhi);
  hnew->GetYaxis()->Set (ny, yscale*ylo, yscale*yhi);
  size_t n = hnew->m_vals.size();
  for (size_t i = 0; i < n; i++) {
    hnew->m_vals[i].first *= xscale;
    hnew->m_vals[i].second *= yscale;
  }

  return hnew;
}


} // namespace RootUtils
