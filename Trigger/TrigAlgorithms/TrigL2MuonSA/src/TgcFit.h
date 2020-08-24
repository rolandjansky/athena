/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCFIT_H
#define  TRIGL2MUONSA_TGCFIT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TgcFitResult.h"

namespace TrigL2MuonSA {

/*
 *  Statistical routines and fit information (TGC points, statistics, etc.).
 */
class TgcFit: public AthAlgTool
{
 public:
  enum Status
  {
    FIT_NONE,
    FIT_POINT,
    FIT_LINE
  };
  /*
   *  A 2D point used in statistics and fits.
   * X is always the Z-value of a digit and Y is either phi or eta.
   */
  struct Point
  {
    int nIdx;       /**< Index of point in original list of digits. */
    int nStation;   /**< Index of station: 0-3 for TGC. */
    double fX;      /**< X coordinate. */
    double fY;      /**< Y coordinate. */
    bool bOutlier;  /**< Indicates an outlier (excluded from all calculations). */
    double fW;      /**< Weight. */
    double fChi2;   /**< Chi2 contribution. */
    
    /*
     *  Default constructor.
     */
  Point() : nIdx(0), nStation(0), fX(0.0), fY(0.0), bOutlier(false), fW(0.0), fChi2(0.0)
    {}
    /*
     *  Constructor.
     */
  Point(int nIdx, int nStation, double fX, double fY, double fW = 1.0) :
    nIdx(nIdx), nStation(nStation), fX(fX), fY(fY), bOutlier(false), fW(fW), fChi2(0.0)
    {}
    /*
     *  Add a constant to the coordinate.
     */
    void operator+=(double fD)
    {
      fX += fD;
      fY += fD;
    }
    /*
     *  Add one point to another.
     */
    void operator+=(const Point& p)
    {
      fX += p.fX;
      fY += p.fY;
    }
    /*
     *  Devide the coordiante by a constant.
     */
    void operator/=(double fD)
    {
      if (fD != 0.0)
	{
	  fX /= fD;
	  fY /= fD;
	}
    }
  };
  /*
   *  A list of points.
   */
  typedef std::vector<Point> PointArray;
  
  /**
   * A structure to hold simple statisitcs
   */
  struct SimpleStats
  {
    int n;              /**< Number of valid points */
    double fMean;       /**< Mean */
    double fStd;        /**< Standard deviation */
    double fChi2;       /**< Chi2 of all valid points */

    /**
     * Constructor
     */
  SimpleStats() :
    n(0), fMean(0.0), fStd(0.0), fChi2(0.0)
    {}
    
    /**
     * Clear statistics before a new fit
     */
    void clear()
    {
      n = 0;
      fMean = fStd = fChi2 = 0.0;
    }
  };
  
  /*
   *  A structure to hold linear fit statistics.
   */
  struct LinStats
  {
    int n;              /**< Number of points. */
    double fIntercept;  /**< Intercept of the fit line. */
    double fSlope;      /**< Slope of the fit line. */
    double fR2;         /**< R-squared of the fit. */
    double fAdjR2;      /**< R-squared adjusted for small samples. */
    double fChi2;       /**< Total Chi2. */
    double fCov00, fCov01, fCov11;
    
    /*
     *  Default constructor.
     */
  LinStats() :
    n(0), fIntercept(0.0), fSlope(0.0), fR2(0.0), fAdjR2(0.0), fChi2(0.0),
      fCov00(0.0), fCov01(0.0), fCov11(0.0)
    {}
    /*
     *  Clear statistics before a new fit.
     */
    void clear()
    {
      n = 0;
      fIntercept = fSlope = fR2 = fAdjR2 = fChi2 = fCov00 = fCov01 = fCov11 = 0.0;
    }
    /*
     *  Calculate projected values given fit results.
     * @parma fX The X value of the point to project.
     * @return The projected Y value.
     */
    double eval(double fX) const;
  };
  
 public:

  /*
   *  Default constuctor.
   */
  TgcFit(const std::string& type, 
	 const std::string& name,
	 const IInterface*  parent);

  // not used
  void setFitParameters(double CHI2_TEST,
			unsigned MIN_WIRE_POINTS,
			unsigned MIN_STRIP_POINTS);


  /*
   *  Get the fit data for strip (phi) fit;
   */
  const LinStats& getStripLinStats() const;
  /*
   *  Get the fit data for wire (eta) fit;
   */
  const LinStats& getWireLinStats() const;
  /*
   *  Fit a straight line throuh points. Ignore outliers.
   * @param points The input list of data points.
   * @param stats The statistics struct to fill with fit results.
   */
  void linReg(PointArray& points, LinStats& stats);
  /*
   *  Calculate simple statistics for the Y values of a set of points.
   * @param points The input list of data points.
   * @param stats The statistics struct to fill with fit results.
   */
  void SimpleStatistics(PointArray& points, SimpleStats& stats);
  /*
   *  Fit data to TGC. Calculate outliers and fit lines to strip and wire points.
   */
  Status runTgcMiddle(PointArray& stripPoints, PointArray& wirePoints, TgcFitResult& fitResult);
  Status runTgcInner(PointArray& stripPoints, PointArray& wirePoints, TgcFitResult& fitResult);
  
 protected:
  PointArray m_superPoints;           /**< List of wire (eta) super-points. */
  
  double m_CHI2_TEST { 10.0 };        /** Test for outliers: w * (value - mean)^2 > CHI2_TEST. */
  unsigned m_MIN_WIRE_POINTS { 4 };   /**< Minimum number of wire points for linear fit. */
  unsigned m_MIN_STRIP_POINTS { 3 };  /**< Minimum number of strip points for linear fit. */
  
  void printDebug(const std::string& str){ ATH_MSG_DEBUG(str); };
};
 
}

#endif
