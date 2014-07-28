/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TgcFit.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuMessageInterface.h"
#include "CLHEP/Geometry/Point3D.h"
#include "gsl/gsl_statistics.h"
#include "gsl/gsl_fit.h"
#include <float.h>
#include <iostream>
#include <math.h>
#include <sstream>

void TgcFit::printDebug(const std::string& str)
{
    if (m_log != NULL)
    {
        *m_log << MSG::DEBUG << str << endreq;
    }
    if (m_dis != NULL)
    {
        *m_dis << str << std::endl;
    }
    if (m_msg != NULL)
    {
        m_msg->lock();
        m_msg->displayDebug(str);
        m_msg->unlock();
    }
}

double TgcFit::LinStats::eval(double fX) const
{
    double fY, fYerr;
    gsl_fit_linear_est(fX, fIntercept, fSlope, fCov00, fCov01, fCov11, &fY, &fYerr);
    return fY;
}

void TgcFit::SimpleStatistics(TgcFit::PointArray& points, SimpleStats& stats)
{
    double *y = new double[points.size()];
    double *w = new double[points.size()];
    for (;;)
    {
        stats.clear();
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            // Exclude outliers.
            if (!points[iPt].bOutlier)
            {
                y[stats.n] = points[iPt].fY;
                w[stats.n] = points[iPt].fW;
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
        std::ostringstream oss;
        oss  << "SimpleStatistics:"
        << " n=" << stats.n
        << " Mean=" << stats.fMean
        << " Std=" << stats.fStd
        << " Chi2=" << stats.fChi2;
        printDebug(oss.str());

        if (iPtMax == -1 || fMaxChi2 < m_CHI2_TEST)
            break;
        points[iPtMax].bOutlier = true;
    }
    delete [] y;
    delete [] w;
}

void TgcFit::linReg(TgcFit::PointArray& points, TgcFit::LinStats& stats)
{
    double *x = new double[points.size()];
    double *y = new double[points.size()];
    double *w = new double[points.size()];
    for (;;)
    {
        stats.clear();
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            if (!points[iPt].bOutlier)
            {
                x[stats.n] = points[iPt].fX;
                y[stats.n] = points[iPt].fY;
                w[stats.n] = points[iPt].fW;
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
        std::ostringstream oss;
        oss << "Y=" << stats.fIntercept << "+X*" << stats.fSlope
        << ", N=" << stats.n << ", Chi2=" << stats.fChi2;
        printDebug(oss.str());

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
    for (unsigned iPt = 0; iPt < points.size(); iPt++)
    {
        if (!points[iPt].bOutlier)
        {
            std::ostringstream oss;
            oss << "Idx=" << points[iPt].nIdx
            << ", x=" << points[iPt].fX
            << ", y=" << points[iPt].fY
            << ", w=" << points[iPt].fW
            << ", Y=" << stats.eval(points[iPt].fX)
            << ", chi2=" << points[iPt].fChi2;
            printDebug(oss.str());
        }
    }
}

TgcFit::Status TgcFit::runTgcMiddle(PointArray& stripPoints, PointArray& wirePoints, TriggerData& trigData)
{
    {
        std::ostringstream oss;
        oss << "TgcFit::runTgcMiddle stripPoints=" << stripPoints.size() << " wirePoints=" << wirePoints.size();
        printDebug(oss.str());
    }
    
    trigData.tgcMidRhoNin = wirePoints.size();
    trigData.tgcMidPhiNin = stripPoints.size();

    if (stripPoints.empty() || wirePoints.empty())
        return FIT_NONE;

    unsigned iPt;
    Status status = FIT_NONE;
    LinStats stripStats;
    LinStats wireStats;
    std::set<int> wireStations, stripStations;

    for (iPt = 0; iPt < wirePoints.size(); iPt++)
    {
        wireStations.insert(wirePoints[iPt].nStation);
    }
    for (iPt = 0; iPt < stripPoints.size(); iPt++)
    {
        stripStations.insert(stripPoints[iPt].nStation);
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

    trigData.intercept = wireStats.fIntercept;
    trigData.slope     = wireStats.fSlope;

    // Create low and high points
    double lowZ = 30000.0, highZ = 0.0;
    int iLow = -1, iHigh = -1;
    for (iPt = 0; iPt < wirePoints.size(); iPt++)
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
    lowZ  = wirePoints[iLow].fX;
    highZ = wirePoints[iHigh].fX;
    HepGeom::Point3D<double> p1(wireStats.eval(lowZ), 0.0, lowZ);
    double phi = stripStats.eval(lowZ);
    if( phi >  M_PI ) phi -= M_PI*2;
    if( phi < -M_PI ) phi += M_PI*2;
    p1.setPhi(phi);
    HepGeom::Point3D<double> p2(wireStats.eval(highZ), 0.0, highZ);
    phi = stripStats.eval(highZ);
    if( phi >  M_PI ) phi -= M_PI*2;
    if( phi < -M_PI ) phi += M_PI*2;
    p2.setPhi(phi);
    {
        std::ostringstream oss;
        oss << "runTgcMiddle: Point1 eta=" << p1.eta() << ",phi=" << p1.phi() << ",perp=" << p1.perp() << ",z=" << p1.z();
        printDebug(oss.str());
    }
    {
        std::ostringstream oss;
        oss << "runTgcMiddle: Point2 eta=" << p2.eta() << ",phi=" << p2.phi() << ",perp=" << p2.perp() << ",z=" << p2.z();
        printDebug(oss.str());
    }
    trigData.tgcMid1[0] = p1.eta();
    trigData.tgcMid1[1] = p1.phi();
    trigData.tgcMid1[2] = p1.perp();
    trigData.tgcMid1[3] = p1.z();
    trigData.tgcMid2[0] = p2.eta();
    trigData.tgcMid2[1] = p2.phi();
    trigData.tgcMid2[2] = p2.perp();
    trigData.tgcMid2[3] = p2.z();
    trigData.tgcMidRhoChi2 = wireStats.fChi2;
    trigData.tgcMidRhoN = wireStats.n;
    trigData.tgcMidPhiChi2 = stripStats.fChi2;
    trigData.tgcMidPhiN = stripStats.n;

    return status;
}

TgcFit::Status TgcFit::runTgcInner(PointArray& stripPoints, PointArray& wirePoints, TriggerData& trigData)
{
    Status status = FIT_NONE;
    SimpleStats stripStats;
    SimpleStats wireStats;    
    
    trigData.tgcInnRhoNin = wirePoints.size();
    trigData.tgcInnPhiNin = stripPoints.size();

    if (stripPoints.size() > 1 && wirePoints.size() > 1)
    {
        status = FIT_POINT;
        printDebug("runTgcInner: Wires fit point");
        SimpleStatistics(wirePoints, wireStats);
        
        printDebug("runTgcInner: Strips fit point");
        SimpleStatistics(stripPoints, stripStats);

        HepGeom::Point3D<double> p(wireStats.fMean, 0.0, wirePoints[0].fX);
	double phi = stripStats.fMean;
	if( phi >  M_PI ) phi -= M_PI*2;
	if( phi < -M_PI ) phi += M_PI*2;
        p.setPhi(phi);

        std::ostringstream oss;
        oss << "runTgcInner: Point eta=" << p.eta() << ",phi=" << p.phi() << ",perp=" << p.perp() << ",z=" << p.z();
        printDebug(oss.str());

        trigData.tgcInn[0] = p.eta();
        trigData.tgcInn[1] = p.phi();
        trigData.tgcInn[2] = p.perp();
        trigData.tgcInn[3] = p.z();
        trigData.tgcInnRhoStd = wireStats.fStd;
        trigData.tgcInnRhoN   = wireStats.n;
        trigData.tgcInnPhiStd = stripStats.fStd;
        trigData.tgcInnPhiN   = stripStats.n;
    }
    return status;
}
