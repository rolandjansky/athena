/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLinearSegmentMakerUtilities/Fit2D.h"
#include "gsl/gsl_statistics.h"
#include "gsl/gsl_fit.h"
#include <sstream>

namespace Muon
{

std::string Fit2D::SimpleStats::toString() const
{
    std::ostringstream oss;
    oss << "Mean=" << fMean
    << ", Std=" << fStd
    << ", N=" << n
    << ", Chi2=" << fChi2;

    return oss.str();
}

void Fit2D::LinStats::eval(double fX, double& fY, double& fYerr) const
{
    gsl_fit_linear_est(fX, fIntercept, fSlope, fCov[0][0], fCov[0][1], fCov[1][1], &fY, &fYerr);
}

std::string Fit2D::LinStats::toString() const
{
    std::ostringstream oss;
    oss << "Y=" << fIntercept << "+X*" << fSlope
    << ", N=" << n
    << ", Cov=[[" << fCov[0][0] << "," << fCov[0][1] << "],["
    << fCov[1][0] << "," << fCov[1][1] << "]]"
    << ", Chi2=" << fChi2;

    return oss.str();
}

void Fit2D::SimpleStatistics(const Fit2D::PointArray& points, SimpleStats& stats)
{
    stats.clear();
    double *y = new double[points.size()];
    double *w = new double[points.size()];
    for (unsigned iPt = 0; iPt < points.size(); iPt++)
    {
        // Exclude outliers.
        if (!points[iPt]->bExclude)
        {
            y[stats.n] = points[iPt]->fY;
            w[stats.n] = points[iPt]->fW;
            stats.n++;
        }
    }
    if (stats.n == 0) goto done;
    // Calculate mean and standard deviation.
    stats.fMean = gsl_stats_wmean(w, 1, y, 1, stats.n);
    if (stats.n == 1) goto done;
    stats.fStd  = gsl_stats_wsd  (w, 1, y, 1, stats.n);
    for (int i = 0; i < stats.n; i++)
    {
        double fDiff = y[i] - stats.fMean;
        stats.fChi2 += w[i] * fDiff * fDiff;
    }
done:
    delete [] y;
    delete [] w;
}

void Fit2D::fitPoint(PointArray& points, double fExclChi2, bool bDump, SimpleStats& stats)
{
    double *y = new double[points.size()];
    double *w = new double[points.size()];
    for (;;)
    {
        stats.clear();
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            // Exclude outliers.
            if (!points[iPt]->bExclude)
            {
                y[stats.n] = points[iPt]->fY;
                w[stats.n] = points[iPt]->fW;
                stats.n++;
            }
        }
        // Calculate mean and standard deviation.
        if (stats.n == 0) goto done;
        stats.fMean = gsl_stats_wmean(w, 1, y, 1, stats.n);
        if (stats.n == 1) goto done;
        stats.fStd = gsl_stats_wsd(w, 1, y, 1, stats.n);
        double fMaxChi2 = 0;
        int i = 0, iPtMax = 0;
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            if (!points[iPt]->bExclude)
            {
                double fDiff = y[i] - stats.fMean;
                points[iPt]->fChi2 = w[i] * fDiff * fDiff;
                stats.fChi2 += points[iPt]->fChi2;
                if (fMaxChi2 < points[iPt]->fChi2)
                {
                    fMaxChi2 = points[iPt]->fChi2;
                    iPtMax = iPt;
                }
                if (bDump)
                {
                    std::cout << "idx=" << points[iPt]->nIdx
                    << ", y=" << y[i]
                    << ", w=" << w[i]
                    << ", chi2=" << points[iPt]->fChi2
                    << std::endl;
                }
                i++;
            }
        }
        if (bDump)
            std::cout << stats.toString() << std::endl;
        if (fMaxChi2 < fExclChi2 || stats.n <= 2)
            break;
        points[iPtMax]->bExclude = true;
    }
done:
    delete [] y;
    delete [] w;
}

void Fit2D::fitLine(Fit2D::PointArray& points, double fExclChi2, bool bDump, LinStats& stats)
{
    double *x = new double[points.size()];
    double *y = new double[points.size()];
    double *w = new double[points.size()];
    for (;;)
    {
        stats.clear();
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            if (!points[iPt]->bExclude)
            {
                x[stats.n] = points[iPt]->fX;
                y[stats.n] = points[iPt]->fY;
                w[stats.n] = points[iPt]->fW;
                stats.n++;
            }
        }
        if (stats.n < 2) goto done;

        gsl_fit_wlinear(x, 1,
                        w, 1,
                        y, 1,
                        stats.n,
                        &stats.fIntercept, &stats.fSlope,
                        &stats.fCov[0][0], &stats.fCov[0][1], &stats.fCov[1][1],
                        &stats.fChi2);
        stats.fCov[1][0] = stats.fCov[0][1];
        if (bDump)
            std::cout << stats.toString() << std::endl;

        double fY, fYerr, fMaxChi2 = 0;
        int i = 0, iPtMax = 0;
        for (unsigned iPt = 0; iPt < points.size(); iPt++)
        {
            if (!points[iPt]->bExclude)
            {
                stats.eval(x[i], fY, fYerr);
                double fDiff = y[i] - fY;
                points[iPt]->fChi2 = w[i] * fDiff * fDiff;
                if (fMaxChi2 < points[iPt]->fChi2)
                {
                    fMaxChi2 = points[iPt]->fChi2;
                    iPtMax = iPt;
                }
                if (bDump)
                {
                    std::cout << "idx=" << points[iPt]->nIdx
                    << ", x=" << x[i]
                    << ", y=" << y[i]
                    << ", w=" << w[i]
                    << ", Y=" << fY
                    << ", chi2=" << points[iPt]->fChi2
                    << std::endl;
                }
                i++;
            }
        }
        if (fMaxChi2 < fExclChi2 || stats.n <= 2)
            break;
        points[iPtMax]->bExclude = true;
    }
done:
    delete [] x;
    delete [] y;
    delete [] w;
}

}
