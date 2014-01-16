/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibMath/DataPoint.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS DataPoint ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataPoint::DataPoint(void): m_ref_comp(0){
}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataPoint::DataPoint(const unsigned int & length, const double & fill) {

    m_vec = Amg::VectorX(length);
    for (unsigned int k=0; k<length; k++) {
        m_vec[k] = fill;
    }
    m_ref_comp = 0;

}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataPoint::DataPoint(const Amg::VectorX & vec, const unsigned int ref_comp) {

    m_vec = vec;
    m_ref_comp = ref_comp;

}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

DataPoint::DataPoint(const CLHEP::HepVector & vec, const unsigned int ref_comp) {

    m_vec = Amg::VectorX(vec.num_row());
    for(int i=0; i<vec.num_row(); i++)
    	{
        m_vec[i]=vec[i];
        }
        
    m_ref_comp = ref_comp;
}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD dataVector ::
//:::::::::::::::::::::::

const Amg::VectorX & DataPoint::dataVector(void) const {

    return m_vec;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD dataVector ::
//:::::::::::::::::::::::

Amg::VectorX & DataPoint::dataVector(void) {

    return m_vec;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD referenceComponent ::
//:::::::::::::::::::::::::::::::

unsigned int DataPoint::referenceComponent(void) const {

    return m_ref_comp;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::
//:: METHOD setReferenceComponent ::
//::::::::::::::::::::::::::::::::::

void DataPoint::setReferenceComponent(const unsigned int & ref_comp) {

    m_ref_comp = ref_comp;

}
