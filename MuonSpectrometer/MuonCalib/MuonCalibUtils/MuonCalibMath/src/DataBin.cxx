/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibMath/DataBin.h"
#include <algorithm>
#include <iostream>
#include "cmath"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS DataBin ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataBin::DataBin(void) {
}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataBin::DataBin(const std::vector<DataPoint> points, const double & epsilon) {

    for (unsigned int k=0; k<points.size(); k++) {
        addPointAndResize(points[k], epsilon);
    }

}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataBin::DataBin(const Amg::VectorX & lower_boundaries,
                                        const Amg::VectorX & upper_boundaries) {

////////////////////////////////////////////////////////
// CHECK WHETHER BOUNDARIES DIMENSIONS ARE CONSISTENT //
////////////////////////////////////////////////////////

    if (lower_boundaries.rows()!=upper_boundaries.rows()) {
        cerr << "Class MuonCalibMath/DataBin, constructor: ERROR!\n"
             << "Dimensions of lower and upper bin boundaries disagree.\n";
        exit(1);
    }

//////////////////////////////////////////////////////
// COPY THE BOUNDARIES AND CALCULATE THE BIN CENTRE //
//////////////////////////////////////////////////////

    m_lower_boundaries = lower_boundaries;
    m_upper_boundaries = upper_boundaries;

    m_bin_centre = 0.5*(m_lower_boundaries+m_upper_boundaries);

}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD DataBin ::
//::::::::::::::::::::

double DataBin::density(void) const {

    double volume(1.0);
    for (int k=0; k<m_lower_boundaries.cols(); k++) {
        volume = volume*(m_upper_boundaries[k]-m_lower_boundaries[k]);
    }
    return m_points.size()/volume;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD centreOfBin ::
//::::::::::::::::::::::::

const Amg::VectorX & DataBin::centreOfBin(void) const {

    return m_bin_centre;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD lowerBinBoundaries ::
//:::::::::::::::::::::::::::::::

const Amg::VectorX & DataBin::lowerBinBoundaries(void) const {

    return m_lower_boundaries;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD upperbinBoundaries ::
//:::::::::::::::::::::::::::::::

const Amg::VectorX & DataBin::upperbinBoundaries(void) const {

    return m_upper_boundaries;

}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD centreOfGravity ::
//::::::::::::::::::::::::::::

const Amg::VectorX & DataBin::centreOfGravity(void) const {

    return m_centre_of_gravity;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD standardDeviations ::
//:::::::::::::::::::::::::::::::

const Amg::VectorX & DataBin::standardDeviations(void) const {

    return m_standard_deviations;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD numberOfDataPoints ::
//:::::::::::::::::::::::::::::::

unsigned int DataBin::numberOfDataPoints(void) const {

    return m_points.size();

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD dataPoints ::
//:::::::::::::::::::::::

const std::vector<DataPoint> & DataBin::dataPoints(void) const {

    return m_points;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD splitBin ::
//:::::::::::::::::::::

DataBin * DataBin::splitBin(const unsigned int & ref_coord) {

////////////////////////////////////////////////////////////////////
// CHECK WHETHER THE REFERENCE COORDINATE IS IN THE ALLOWED RANGE //
////////////////////////////////////////////////////////////////////

	if (ref_coord>=static_cast<unsigned int >(m_bin_centre.rows())) {
        cerr << "Class MuonCalibMath/DataBin, method splitBin: ERROR!\n"
             << "Reference coordinate out of range!\n";
        exit(1);
    }

////////////////////////////////////////////////////
// CHECK WHETHER THERE ARE AT LEAST 4 DATA POINTS //
////////////////////////////////////////////////////

    if (m_points.size()<4) {
        cerr << "Class MuonCalibMath/DataBin, method splitBin: ERROR!\n"
             << "Less than 4 points in the bin!\n";
        exit(1);
    }

/////////////////////////////////////////////////////////////
// SORT THE DATA POINTS ALONG THE SELECTED COORDINATE AXIS //
/////////////////////////////////////////////////////////////

    for (unsigned int k=0; k<m_points.size(); k++) {
        m_points[k].setReferenceComponent(ref_coord);
    }

   sort(m_points.begin(), m_points.end());

///////////////////////
// MAKE TWO NEW BINS //
///////////////////////

// event dividing the splitting position //
    unsigned int k_split((m_points.size()/2)+m_points.size()%2);

// bin 1 data //
    Amg::VectorX bin_1_low = m_lower_boundaries;
    Amg::VectorX bin_1_up = m_upper_boundaries;
    bin_1_up[ref_coord] = (m_points[k_split]).dataVector()[ref_coord];
    vector<DataPoint> bin_1_points(k_split);
    for (unsigned k=0; k<k_split; k++) {
        bin_1_points[k] = m_points[k];
    }

// bin 2 data //
    Amg::VectorX bin_2_low = m_lower_boundaries;
    bin_2_low[ref_coord] = (m_points[k_split]).dataVector()[ref_coord];
    Amg::VectorX bin_2_up = m_upper_boundaries;
    vector<DataPoint> bin_2_points(m_points.size()-k_split);
	for (unsigned int k=k_split; k<m_points.size(); k++) {
        bin_2_points[k-k_split] = m_points[k];
    }

// create the second bin //
    DataBin *bin_2 = new DataBin(bin_2_low, bin_2_up);
    bin_2->setPoints(bin_2_points);

// resize the current bin //
    m_lower_boundaries = bin_1_low;
    m_upper_boundaries = bin_2_up;
    setPoints(bin_1_points);

    return bin_2;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD addPoint ::
//:::::::::::::::::::::

bool DataBin::addPoint(const DataPoint & point) {

// check whether the point can be added //
    for (int k=0; k<point.dataVector().rows(); k++) {
        if (point.dataVector()[k]<m_lower_boundaries[k] ||
                                point.dataVector()[k]>=m_upper_boundaries[k]) {
            return false;
        }
    }

// add the point //
    m_points.push_back(point);

// recalculate the centre of gravity and the standard deviations //
    m_centre_of_gravity = point.dataVector();
	m_standard_deviations = point.dataVector();
	for (int k=0; k<m_centre_of_gravity.rows(); k++) {
        m_centre_of_gravity[k] = 0.0;
        m_standard_deviations[k] = 0.0;
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_centre_of_gravity[k] = m_centre_of_gravity[k]+
                                              m_points[l].dataVector()[k];
        }
        m_centre_of_gravity[k] = m_centre_of_gravity[k]/
                            			static_cast<double>(m_points.size());
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_standard_deviations[k] = m_standard_deviations[k]+
                    std::pow(m_points[l].dataVector()[k]-m_centre_of_gravity[k], 2);
        }
        if (m_points.size()>2) {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size()-1);
        } else {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size());
        }
        m_standard_deviations[k] = sqrt(m_standard_deviations[k]);
    }

    return true;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD addPointAndResize ::
//::::::::::::::::::::::::::::::

void DataBin::addPointAndResize(const DataPoint & point, const double & epsilon) {

// add the point //
    m_points.push_back(point);

// recalculate the centre of gravity and the standard deviations //
    m_centre_of_gravity = point.dataVector();
	m_standard_deviations = point.dataVector();
	for (int k=0; k<m_centre_of_gravity.rows(); k++) {
        m_centre_of_gravity[k] = 0.0;
        m_standard_deviations[k] = 0.0;
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_centre_of_gravity[k] = m_centre_of_gravity[k]+
                                              m_points[l].dataVector()[k];
        }
        m_centre_of_gravity[k] = m_centre_of_gravity[k]/
                            			static_cast<double>(m_points.size());
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_standard_deviations[k] = m_standard_deviations[k]+
                    std::pow(m_points[l].dataVector()[k]-m_centre_of_gravity[k], 2);
        }
        if (m_points.size()>2) {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size()-1);
        } else {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size());
        }
        m_standard_deviations[k] = sqrt(m_standard_deviations[k]);
    }

// determine the bin boundaries //
    if (m_points.size()==1) {
		m_lower_boundaries = point.dataVector();
		m_upper_boundaries = point.dataVector();
		for (int k=0; k<m_lower_boundaries.rows(); k++) {
        	m_lower_boundaries[k]=point.dataVector()[k];
			m_upper_boundaries[k]=point.dataVector()[k]+epsilon;
    	}
	} else {
		for (int k=0; k<m_lower_boundaries.rows(); k++) {
        	if (m_lower_boundaries[k]<point.dataVector()[k]) {
            	m_lower_boundaries[k]=point.dataVector()[k];
        	}
        	if (m_upper_boundaries[k]>=point.dataVector()[k]) {
            	m_upper_boundaries[k]=point.dataVector()[k]+epsilon;
        	}
    	}
	}

// calculate bin centre //
	m_bin_centre = 0.5*(m_lower_boundaries+m_upper_boundaries);

    return;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD setPoints ::
//::::::::::::::::::::::

void DataBin::setPoints(const std::vector<DataPoint> & points) {

    m_points = points;

	if (m_points.size()==0) {
		return;
	}

// recalculate the centre of gravity and the standard deviations //
    m_centre_of_gravity = m_points[0].dataVector();
	m_standard_deviations = m_points[0].dataVector();
	for (int k=0; k<m_centre_of_gravity.rows(); k++) {
        m_centre_of_gravity[k] = 0.0;
        m_standard_deviations[k] = 0.0;
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_centre_of_gravity[k] = m_centre_of_gravity[k]+
                                              m_points[l].dataVector()[k];
        }
        m_centre_of_gravity[k] = m_centre_of_gravity[k]/
                            			static_cast<double>(m_points.size());
        for (unsigned int l=0; l<m_points.size(); l++) {
            m_standard_deviations[k] = m_standard_deviations[k]+
                    std::pow(m_points[l].dataVector()[k]-m_centre_of_gravity[k], 2);
        }
        if (m_points.size()>2) {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size()-1);
        } else {
            m_standard_deviations[k] = m_standard_deviations[k]/
                            			static_cast<double>(m_points.size());
        }
        m_standard_deviations[k] = sqrt(m_standard_deviations[k]);
    }

    return;

}
