/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcFit.h"
#include "gsl/gsl_statistics.h"
#include "gsl/gsl_fit.h"
#include <float.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <set>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcFit::TgcFit(const std::string& type,
			     const std::string& name,
                             const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcFit::setFitParameters(double CHI2_TEST,
					    unsigned MIN_WIRE_POINTS,
					    unsigned MIN_STRIP_POINTS)
{
  m_CHI2_TEST        = CHI2_TEST;
  m_MIN_WIRE_POINTS  = MIN_WIRE_POINTS;
  m_MIN_STRIP_POINTS = MIN_STRIP_POINTS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::TgcFit::LinStats::eval(double fX) const
{
  double fY, fYerr;
  gsl_fit_linear_est(fX, fIntercept, fSlope, fCov00, fCov01, fCov11, &fY, &fYerr);
  return fY;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcFit::SimpleStatistics(TrigL2MuonSA::TgcFit::PointArray& points, TrigL2MuonSA::TgcFit::SimpleStats& stats)
{
  double *y = new double[points.size()];
  double *w = new double[points.size()];
  for (;;)
  {
    stats.clear();
    for (const TrigL2MuonSA::TgcFit::Point& Pt : points) 
    {
      // Exclude outliers.
      if (!Pt.bOutlier)
      {
        y[stats.n] = Pt.fY;
        w[stats.n] = Pt.fW;
        stats.n++;
      }
    }
    if (stats.n == 0)
      break;
    // Calculate mean and standard deviation.
    stats.fMean = gsl_stats_wmean(w, 1, y, 1, stats.n);
    if (stats.n == 1)
      break;
    stats.fStd  = gsl_stats_wsd  (w, 1, y, 1, stats.n);
    double fMaxChi2 = 0.0;
    int iPtMax = -1;
    for (unsigned iPt = 0; iPt < points.size(); iPt++)
    {
      if (!points[iPt].bOutlier)
      {
        double fDiff = points[iPt].fY - stats.fMean;
        points[iPt].fChi2 = points[iPt].fW * fDiff * fDiff;
        stats.fChi2 += points[iPt].fChi2;
        if (fMaxChi2 < points[iPt].fChi2)
        {
          fMaxChi2 = points[iPt].fChi2;
          iPtMax = iPt;
        }
      }
    }
    // Print results.
    ATH_MSG_DEBUG( "SimpleStatistics:"
        << " n=" << stats.n << " Mean=" << stats.fMean
        << " Std=" << stats.fStd << " Chi2=" << stats.fChi2);

    if (iPtMax == -1 || fMaxChi2 < m_CHI2_TEST)
      break;
    points[iPtMax].bOutlier = true;
  }
  delete [] y;
  delete [] w;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcFit::linReg(TrigL2MuonSA::TgcFit::PointArray& points, TrigL2MuonSA::TgcFit::LinStats& stats)
{
  double *x = new double[points.size()];
  double *y = new double[points.size()];
  double *w = new double[points.size()];
  for (;;)
  {
    stats.clear();
    for (const TrigL2MuonSA::TgcFit::Point& Pt : points)
    {
      if (!Pt.bOutlier)
      {
        x[stats.n] = Pt.fX;
        y[stats.n] = Pt.fY;
        w[stats.n] = Pt.fW;
        stats.n++;
      }
    }
    if (stats.n < 2)
      break;

    gsl_fit_wlinear(x, 1,
        w, 1,
        y, 1,
        stats.n,
        &stats.fIntercept, &stats.fSlope,
        &stats.fCov00, &stats.fCov01, &stats.fCov11,
        &stats.fChi2);
    ATH_MSG_DEBUG("Y=" << stats.fIntercept << "+X*" << stats.fSlope
        << ", N=" << stats.n << ", Chi2=" << stats.fChi2);

    double fMaxChi2 = 0.0;
    int iPtMax = -1;
    for (unsigned iPt = 0; iPt < points.size(); iPt++)
    {
      if (!points[iPt].bOutlier)
      {
        double fDiff = points[iPt].fY - stats.eval(points[iPt].fX);
        points[iPt].fChi2 = points[iPt].fW * fDiff * fDiff;
        if (fMaxChi2 < points[iPt].fChi2)
        {
          fMaxChi2 = points[iPt].fChi2;
          iPtMax = iPt;
        }
      }
    }
    if (iPtMax == -1 || fMaxChi2 < m_CHI2_TEST || stats.n <= 2)
      break;
    points[iPtMax].bOutlier = true;
  }

  delete [] x;
  delete [] y;
  delete [] w;

  // Print results
  for (const TrigL2MuonSA::TgcFit::Point& Pt : points)
  {
    if (!Pt.bOutlier)
    {
      ATH_MSG_DEBUG("Idx=" << Pt.nIdx
          << ", x=" << Pt.fX
          << ", y=" << Pt.fY
          << ", w=" << Pt.fW
          << ", Y=" << stats.eval(Pt.fX)
          << ", chi2=" << Pt.fChi2);
    }
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcFit::Status TrigL2MuonSA::TgcFit::runTgcMiddle(TrigL2MuonSA::TgcFit::PointArray& stripPoints,
    TrigL2MuonSA::TgcFit::PointArray& wirePoints,
    TrigL2MuonSA::TgcFitResult& tgcFitResult)
{
  ATH_MSG_DEBUG("TrigL2MuonSA::TgcFit::runTgcMiddle stripPoints=" << stripPoints.size()
      << " wirePoints=" << wirePoints.size());

  tgcFitResult.tgcMidRhoNin = wirePoints.size();
  tgcFitResult.tgcMidPhiNin = stripPoints.size();

  if (stripPoints.empty() || wirePoints.empty())
    return FIT_NONE;

  Status status = FIT_NONE;
  LinStats stripStats;
  LinStats wireStats;
  std::set<int> wireStations, stripStations;

  for (const TrigL2MuonSA::TgcFit::Point& wirePt : wirePoints)
  {
    wireStations.insert(wirePt.nStation);
  }
  for (const TrigL2MuonSA::TgcFit::Point& stripPt : stripPoints)
  {
    stripStations.insert(stripPt.nStation);
  }

  if (wireStations.size() == 1 || wirePoints.size() < m_MIN_WIRE_POINTS)
  {
    status = FIT_POINT;
    printDebug("runTgcMiddle: Wires fit point");
    SimpleStats stats;
    SimpleStatistics(wirePoints, stats);
    wireStats.fIntercept = stats.fMean;
    wireStats.n = stats.n;
    wireStats.fChi2 = stats.fChi2;

    printDebug("runTgcMiddle: Strips fit point");
    SimpleStatistics(stripPoints, stats);
    stripStats.fIntercept = stats.fMean;
    stripStats.n = stats.n;
    stripStats.fChi2 = stats.fChi2;
  }
  else
  {
    status = FIT_LINE;
    printDebug("runTgcMiddle: Wires fit line");
    linReg(wirePoints, wireStats);
    if (stripStations.size() == 1 || stripPoints.size() < m_MIN_STRIP_POINTS)
    {
      printDebug("runTgcMiddle: Strips fit point");
      SimpleStats stats;
      SimpleStatistics(stripPoints, stats);
      stripStats.fIntercept = stats.fMean;
      stripStats.n = stats.n;
      stripStats.fChi2 = stats.fChi2;
    }
    else
    {
      printDebug("runTgcMiddle: Strips fit line");
      linReg(stripPoints, stripStats);
    }
  }

  tgcFitResult.intercept = wireStats.fIntercept;
  tgcFitResult.slope     = wireStats.fSlope;

  // Create low and high points
  double lowZ = 30000.0, highZ = 0.0;
  int iLow = -1, iHigh = -1;
  for (unsigned iPt = 0; iPt < wirePoints.size(); iPt++)
  {
    if (!wirePoints[iPt].bOutlier)
    {
      if (lowZ > fabs(wirePoints[iPt].fX))
      {
        lowZ = fabs(wirePoints[iPt].fX);
        iLow = iPt;
      }
      if (highZ < fabs(wirePoints[iPt].fX))
      {
        highZ = fabs(wirePoints[iPt].fX);
        iHigh = iPt;
      }
    }
  }
  if (iLow > -1 && iLow < (int)wirePoints.size()) lowZ  = wirePoints[iLow].fX;
  if (iHigh > -1 && iHigh < (int)wirePoints.size()) highZ = wirePoints[iHigh].fX;
  Amg::Vector3D p1(wireStats.eval(lowZ), 0.0, lowZ);
  double phi = stripStats.eval(lowZ);
  if( phi >  M_PI ) phi -= M_PI*2;
  if( phi < -M_PI ) phi += M_PI*2;
  Amg::setPhi(p1, phi);
  Amg::Vector3D p2(wireStats.eval(highZ), 0.0, highZ);
  phi = stripStats.eval(highZ);
  if( phi >  M_PI ) phi -= M_PI*2;
  if( phi < -M_PI ) phi += M_PI*2;
  Amg::setPhi(p2, phi);
  {
    ATH_MSG_DEBUG("runTgcMiddle: Point1 eta=" << p1.eta()
        << ",phi=" << p1.phi() << ",perp=" << p1.perp() << ",z=" << p1.z());
  }
  {
    ATH_MSG_DEBUG("runTgcMiddle: Point2 eta=" << p2.eta()
        << ",phi=" << p2.phi() << ",perp=" << p2.perp() << ",z=" << p2.z());
  }
  tgcFitResult.tgcMid1[0] = p1.eta();
  tgcFitResult.tgcMid1[1] = p1.phi();
  tgcFitResult.tgcMid1[2] = p1.perp();
  tgcFitResult.tgcMid1[3] = p1.z();
  tgcFitResult.tgcMid2[0] = p2.eta();
  tgcFitResult.tgcMid2[1] = p2.phi();
  tgcFitResult.tgcMid2[2] = p2.perp();
  tgcFitResult.tgcMid2[3] = p2.z();
  tgcFitResult.tgcMidRhoChi2 = wireStats.fChi2;
  tgcFitResult.tgcMidRhoN = wireStats.n;
  tgcFitResult.tgcMidPhiChi2 = stripStats.fChi2;
  tgcFitResult.tgcMidPhiN = stripStats.n;

  return status;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcFit::Status TrigL2MuonSA::TgcFit::runTgcInner(TrigL2MuonSA::TgcFit::PointArray& stripPoints,
    TrigL2MuonSA::TgcFit::PointArray& wirePoints,
    TrigL2MuonSA::TgcFitResult& tgcFitResult)
{
  Status status = FIT_NONE;
  SimpleStats stripStats;
  SimpleStats wireStats;    

  tgcFitResult.tgcInnRhoNin = wirePoints.size();
  tgcFitResult.tgcInnPhiNin = stripPoints.size();

  ATH_MSG_DEBUG("runTgcInner: stripPoints.size()=" << stripPoints.size()
      << ", wirePoints.size()=" << wirePoints.size());

  if (stripPoints.size() > 1 && wirePoints.size() > 1)
  {
    status = FIT_POINT;
    printDebug("runTgcInner: Wires fit point");
    SimpleStatistics(wirePoints, wireStats);

    printDebug("runTgcInner: Strips fit point");
    SimpleStatistics(stripPoints, stripStats);

    Amg::Vector3D p(wireStats.fMean, 0.0, wirePoints[0].fX);
    double phi = stripStats.fMean;
    if( phi >  M_PI ) phi -= M_PI*2;
    if( phi < -M_PI ) phi += M_PI*2;
    Amg::setPhi(p, phi);

    ATH_MSG_DEBUG("runTgcInner: Point eta=" << p.eta()
        << ",phi=" << p.phi() << ",perp=" << p.perp() << ",z=" << p.z());

    tgcFitResult.tgcInn[0] = p.eta();
    tgcFitResult.tgcInn[1] = p.phi();
    tgcFitResult.tgcInn[2] = p.perp();
    tgcFitResult.tgcInn[3] = p.z();
    tgcFitResult.tgcInnRhoStd = wireStats.fStd;
    tgcFitResult.tgcInnRhoN   = wireStats.n;
    tgcFitResult.tgcInnPhiStd = stripStats.fStd;
    tgcFitResult.tgcInnPhiN   = stripStats.n;
  }
  return status;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
