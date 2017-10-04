/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestMatrix.cxx
 * @brief Implementation of class AthenaPoolTestMatrix
 */
/***************************************************************************
 Athena Pool Test data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestMatrix.cxx,v 1.11 2006-08-30 07:49:43 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestMatrix.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

AthenaPoolTestMatrix::AthenaPoolTestMatrix()
    :
    m_smallMatrix( 4,4,0 ),
    m_bigMatrix( new CLHEP::HepMatrix(6,6,0) ),
    m_point(1, 2, 3),
    m_point1 (new HepGeom::Point3D<double>(4, 5, 6))

{
    double k = 7.;
    // Fill small matrix
    for (int i = 0; i < m_smallMatrix.num_row(); ++i) {
	for (int j = 0; j < m_smallMatrix.num_col(); ++j) {
	    m_smallMatrix[i][j] = k;
	    k *= 5.0;
	}
    }
    // Fill big matrix
    for (int i = 0; i < m_bigMatrix->num_row(); ++i) {
	for (int j = 0; j < m_bigMatrix->num_col(); ++j) {
	    (*m_bigMatrix)[i][j] = k;
	    k *= 5.0;
	}
    }
    // Fill matrix maps
    std::map<unsigned int, CLHEP::HepMatrix> m1;
    std::map<unsigned int, CLHEP::HepMatrix> m2;
    m1[5]  = m_smallMatrix;
    m1[10] = *m_bigMatrix;
    m2[16] = *m_bigMatrix;
    m2[35] = m_smallMatrix;
    m_matrixMaps.push_back(m1);
    m_matrixMaps.push_back(m2);

    // Fill vectors
    m_vint.push_back(6);
    m_vint.push_back(26);
    m_vint.push_back(46);
    m_vint.push_back(66);
    m_vfloat.push_back(7.);
    m_vfloat.push_back(17.);
    m_vfloat.push_back(37.);
    m_vfloat.push_back(57.);
}

AthenaPoolTestMatrix::~AthenaPoolTestMatrix()
{
    delete m_bigMatrix; m_bigMatrix=0;
    delete m_point1; m_point1=0;
}
