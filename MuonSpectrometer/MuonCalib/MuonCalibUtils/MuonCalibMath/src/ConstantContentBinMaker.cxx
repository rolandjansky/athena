/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 08.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibMath/ConstantContentBinMaker.h"
#include <iostream>
//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS ConstantContentBinMaker ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

ConstantContentBinMaker::ConstantContentBinMaker(
                            const std::vector<DataPoint> & points,
                            const double & epsilon) {

    m_points = points;
    m_bins.clear();
    m_epsilon = fabs(epsilon);

}


ConstantContentBinMaker::~ConstantContentBinMaker()
	{
	for(std::vector<DataBin *>::iterator it=m_bins.begin(); it<m_bins.end(); it++)
    	delete *it;
	}


//*****************************************************************************

//::::::::::::::::::::::::::
//:: METHOD binDataPoints ::
//::::::::::::::::::::::::::

bool ConstantContentBinMaker::binDataPoints(const unsigned int & bin_content,
                                       std::vector<unsigned int> & ref_coord) {

///////////////
// VARIABLES //
///////////////

    unsigned int n_bins(static_cast<unsigned int>(
                        log(m_points.size()/static_cast<double>(bin_content))
																	/log(2.0)));
    vector<unsigned int> splitting_axis(ref_coord);
    unsigned int axis_counter(0);

///////////
// RESET //
///////////
    for(std::vector<DataBin *>::iterator it=m_bins.begin(); it<m_bins.end(); it++)
    	delete *it;
    m_bins.clear();

/////////////////////////////////////////////////
// CHECK WHETHER THE IS SUFFICIENT DATA POINTS //
/////////////////////////////////////////////////

    if (m_points.size()==0 || n_bins<2) {
        cerr << endl
             << "Class ConstantContentBinMaker, method binDataPoints: WARNING!"
             << "\nToo few data points for binning!\n";
        return false;
    }

////////////////////////////////
// CHECK REFERENCE COORDINATE //
////////////////////////////////

    if (ref_coord.size()==0) {
        splitting_axis = vector<unsigned int>(
                                        m_points[0].dataVector().rows());
        for (unsigned int k=0; k<splitting_axis.size(); k++) {
            splitting_axis[k] = k;
        }
    }

//////////////////////////////////////////////////////////////////////////
// FILL A MOTHER BIN AND BIN THE DATA UNTIL THE REQUIRED BIN CONTENT IS //
// REACHED.                                                             //
//////////////////////////////////////////////////////////////////////////

// mother bin //
    m_bins.push_back(new DataBin(m_points, m_epsilon));

    for (unsigned int split=0; split<n_bins; split++) {
        unsigned int nb_bins_so_far(m_bins.size());
        for (unsigned int k=0; k<nb_bins_so_far; k++) {
                m_bins.push_back(m_bins[k]->splitBin(
                                            splitting_axis[axis_counter]));
                axis_counter++;
                if (axis_counter==splitting_axis.size()) {
                    axis_counter = 0;
                }
        }
    }

    return true;

}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD getBins ::
//::::::::::::::::::::

const std::vector<DataBin *> & ConstantContentBinMaker::getBins(void) const {

    return m_bins;

}
