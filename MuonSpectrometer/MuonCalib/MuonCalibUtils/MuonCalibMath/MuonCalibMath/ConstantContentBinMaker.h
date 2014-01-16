/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 08.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_ConstantContentBinMakerH
#define MuonCalib_ConstantContentBinMakerH

//:::::::::::::::::::::::::::::::::::
//:: CLASS ConstantContentBinMaker ::
//:::::::::::::::::::::::::::::::::::

/// \class ConstantContentBinMaker
///
/// This class allows the user to group his data points into bins
/// (multidimensional half-open intervals) of equal data content.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 08.06.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MuonCalibMath/DataBin.h"

namespace MuonCalib {

class ConstantContentBinMaker {

public:
// Constructor //
    ConstantContentBinMaker(const std::vector<DataPoint> & points,
                            const double & epsilon);
    ///< Constructor.
    ///< \param points Data points to be grouped into bins.
    ///< \param epsilon The upper boundaries of the seed bin are shifted by
    ///<        epsilon to create a half-open intervall containing all data
    ///<        points.
    ~ConstantContentBinMaker();
// Methods //
    bool binDataPoints(const unsigned int & bin_content,
                                        std::vector<unsigned int> & ref_coord);
                                    ///< group the data points into bins of
                                    ///< equal content "bin_content";
                                    ///< returns true in case of success, false
                                    ///< otherwise;
                                    ///< splitting is done along the coordinate
                                    ///< axes stored in ref_coord successively;
                                    ///< if ref_coord is of size 0, splitting
                                    ///< will be done in all axes successively
    const std::vector<DataBin *> & getBins(void) const;
                                    ///< get the bins determined by the method
                                    ///< "binDataPoints"

private:
    std::vector<DataPoint> m_points; // data points
    std::vector<DataBin *> m_bins; // bins containing the data points
    double m_epsilon; // the same as epsilon in constructor

};

}

#endif
