/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_DataBinH
#define MuonCalib_DataBinH

//:::::::::::::::::::
//:: CLASS DataBin ::
//:::::::::::::::::::

/// \class DataBin
///
/// This class provides a bin of data points. The bin is a half-open
/// multdimensional interval ([x1min, x1max)x...x[xnmin, xnmax)).
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 07.06.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MuonCalibMath/DataPoint.h"

namespace MuonCalib {

class DataBin {

public:
// Constructors //
    DataBin(void);
    ///< Default constructor. Give a bin with 0 content and no extensions.

    DataBin(const std::vector<DataPoint> points, const double & epsilon);
    ///< Constructor.
    ///< \param points Data points occupying the bin.
    ///< \param epsilon The upper bin boundaries are shifted by epsilon to
    ///<        create a half-open intervall containing all data points.

    DataBin(const Amg::VectorX & lower_boundaries,
                                        const Amg::VectorX & upper_boundaries);
    ///< Constructor.
    ///< Create an empty bin with the given boundaries.
    ///< \param lower_boundaries Lower boundaries of the bin.
    ///< \param upper_boundaries Upper boundaries of the bin.

// Methods //
// get-methods //
    double density(void) const;
                                    ///< get the data point density in the bin
    const Amg::VectorX & centreOfBin(void) const;
                                    ///< get the centre of the bin
    const Amg::VectorX & lowerBinBoundaries(void) const;
                                    ///< get the lower boundaries of the bin
    const Amg::VectorX & upperbinBoundaries(void) const;
                                    ///< get the upper boundaries of the bin
    const Amg::VectorX & centreOfGravity(void) const;
                                    ///< get the centre of gravity of the data
                                    ///< points
    const Amg::VectorX & standardDeviations(void) const;
                                    ///< get the standard deviations of the
                                    ///< data points from the centre of gravity
                                    ///< in all dimensions
    unsigned int numberOfDataPoints(void) const;
                                    ///< get the number of data points
    const std::vector<DataPoint> & dataPoints(void) const;
                                    ///< get the data points filling this bin
    DataBin * splitBin(const unsigned int & ref_coord);
                                    ///< divide the bin into two of equal
                                    ///< content; splitting is done along the
                                    ///< coordinate ref_coord; the method resize
                                    ///< the current bin and return the other
                                    ///< half (at least 4 points are required
                                    ///< for splitting)

// set-methods //
    bool addPoint(const DataPoint & point);
                                    ///< add the data point to the bin if
                                    ///< possible; returns true if the point
                                    ///< can be added, false otherwise
    void addPointAndResize(const DataPoint & point, const double & epsilon);
                                    ///< add the data point to the bin; the bin
                                    ///< will be resized if the point does not
                                    ///< fit into the bin; the upper bin 
                                    ///< boundaries are shifted by epsilon to
                                    ///< create a half-open intervall containing
                                    ///< all data points
    void setPoints(const std::vector<DataPoint> & points);
                                    ///< fill the bin with the given points;
                                    ///< the use of this method is highly
                                    ///< discouraged, it is needed for the
                                    ///< splitting whithout changing the bin
                                    ///< boundaries

private:
// bin parameters //
    std::vector<DataPoint> m_points; // data points in the bin
    Amg::VectorX m_bin_centre; // centre of the bin
    Amg::VectorX m_lower_boundaries; // lower bin boundaries
    Amg::VectorX m_upper_boundaries; // upper bin boundaries
    Amg::VectorX m_centre_of_gravity; // center of gravity of the data points in
                                   // the bin
    Amg::VectorX m_standard_deviations; // standard deviations from the centre of
                                     // gravity

};

}

#endif
