/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLINEARSEGMENTMAKERUTILITIES_FIT2D_H
#define MUONLINEARSEGMENTMAKERUTILITIES_FIT2D_H

#include "GaudiKernel/MsgStream.h"
#include <vector>
#include <cstring>
#include <string>

namespace Muon
{

    /**
     * A 2D linear regression calculator.
     *
     * The class accepts a list of X/ points and computes the least-squares straight
     * line fit. It reports the intercept, slope and goodness of fit.
     *
     * Points can be excluded from the calculation, and can also have a weight
     * associated with them.
     *
     * The class also computes simple statistics (meand, standard deviation) of the
     * Y values.
     */
    class Fit2D
    {
    public:
        /**
         * A 2D point used in statistics and fit
         */
        struct Point
        {
            int    nIdx;        /**< Index of the point in the original list */
            double fX;          /**< The X coordinate */
            double fY;          /**< The Y coordinate */
            double fW;          /**< Weight */
            double fChi2;       /**< Contribution to the Chi2 */
            bool   bExclude;    /**< If set, exclude the point from all calculations */
            void*  pData;       /**< Any external data provided by the caller */

            /**
             * Default constructor
             */
            Point();

            /**
             * Constructor
             * \param nIdx The original index of the point
             * \param fX The X coordinate
             * \param fY The Y coordinate
             * \param fW The weight
             */
            Point(int nIdx, double fX, double fY, double fW = 1.0, void* pData = NULL);
        };

        typedef std::vector<Point*> PointArray;     /**< A vector of points */

        struct SimpleStats
        {
            int n;
            double fMean;
            double fStd;
            double fChi2;

            SimpleStats();
            void clear();
            std::string toString() const;
        };

        /**
         * A structure to hold linear fit statistics.
         */
        struct LinStats
        {
            int    n;           /**< Number of points */
            double fIntercept;  /**< Intercept of the fit line */
            double fSlope;      /**< Slope of the fit line */
            double fCov[2][2];   /**< The parameter covariance matrix */
            double fChi2;       /**< Chi-squared of the fit */

            /**
             * Constructor
             */
            LinStats();

            /**
             * Clear the statistics before a new fit
             */
            void clear();

            /**
             * Evaluate a point along the fitted line
             * \param fX The X value of the point to project
             * \param fY [output] The projected Y value
             * \param fYerr [output] The error of the projected Y value
             */
            void eval(double fX, double& fY, double& fYerr) const;

            /**
             * Get a string representation of the fit parameters
             */
            std::string toString() const;
        };

        /**
         * Constructor
         */
        Fit2D();

        /**
         * Calculate simple statistics for the Y values of a set of points
         * \param points The list of data points
         * \param stats [output] The statistics results
         */
        void SimpleStatistics(const PointArray& points, SimpleStats& stats);

        /**
         * Fit a straight line through the given points
         * \param points The list of data points
         * \param fExclChi2 CHi2 value for excluding outliers
         * \param bDump Write details to log
         * \param stats [output] The fit results
         */
        void fitLine(PointArray& points, double fExclChi2, bool bDump, LinStats& stats);

        /**
         * Estimate a new point from the given points
         * \param points The list of data points
         * \param fExclChi2 CHi2 value for excluding outliers
         * \param bDump Write details to log
         * \param stats [output] The fit results
         */
        void fitPoint(PointArray& points, double fExclChi2, bool bDump, SimpleStats& stats);
    };

    inline Fit2D::Point::Point() :
    nIdx(0), fX(0.0), fY(0.0), fW(1.0), fChi2(0.0), bExclude(false), pData(NULL)
    {
    }

    inline Fit2D::Point::Point(int nIdx, double fX, double fY, double fW, void* pData) :
    nIdx(nIdx), fX(fX), fY(fY), fW(fW), fChi2(0.0), bExclude(false), pData(pData)
    {
    }

    inline Fit2D::SimpleStats::SimpleStats() :
    n(0),
    fMean(0.0),
    fStd(0.0),
    fChi2(0.0)
    {
    }

    inline void Fit2D::SimpleStats::clear()
    {
        n = 0;
        fMean = fStd = fChi2 = 0.0;
    }

    inline Fit2D::LinStats::LinStats() :
    n(0),
    fIntercept(0.0),
    fSlope(0.0),
    fChi2(0.0)
    {
        memset(fCov, 0, sizeof(fCov));
    }

    inline void Fit2D::LinStats::clear()
    {
        n = 0;
        fIntercept = fSlope = fChi2 = 0.0;
        memset(fCov, 0, sizeof(fCov));
    }

    inline Fit2D::Fit2D()
    {
    }

}

#endif //MUONLINEARSEGMENTMAKERUTILITIES_FIT2D_H
