/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_DataPointH
#define MuonCalib_DataPointH

//:::::::::::::::::::::
//:: CLASS DataPoint ::
//:::::::::::::::::::::

/// \class DataPoint
///
/// This class defines a multidimensional data point.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 07.06.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "EventPrimitives/EventPrimitives.h"
#include "CLHEP/Matrix/Vector.h"
namespace MuonCalib {

class DataPoint {

public:
// Constructors //
    DataPoint(void);
    ///< Default constructor. Gives the vector of length 0.

    DataPoint(const unsigned int & length, const double & fill);
    ///< Constructor.
    ///< \param length Length of the vector.
    ///< \param fill Sets the value of the components of the vector to fill.

    DataPoint(const Amg::VectorX & vec, const unsigned int ref_comp);
    ///< Constructor.
    ///< \param vec Vector to be made the data point.
    ///< \param ref_comp Component used for ordering and comparison (>=0).
 
    DataPoint(const CLHEP::HepVector & vec, const unsigned int ref_comp);
    ///< Constructor.
    ///< \param vec Vector to be made the data point.
    ///< \param ref_comp Component used for ordering and comparison (>=0).

// Methods //
// get-methods //
    const Amg::VectorX & dataVector(void) const;
                                    ///< get the data vector
    Amg::VectorX & dataVector(void);
                                    ///< get the data vector
    unsigned int referenceComponent(void) const;
                                    ///< get the ordering component (>=0)

// set-methods //
    void setReferenceComponent(const unsigned int & ref_comp);
                                    ///< set the ordering component to ref_comp
                                    ///< (>=0)

// operators (all operators are applied to the comparison component) //
    inline bool operator < (const DataPoint & point) const;
    inline bool operator > (const DataPoint & point) const;
    inline bool operator <= (const DataPoint & point) const;
    inline bool operator >= (const DataPoint & point) const;
    inline bool operator == (const DataPoint & point) const;

private:
    Amg::VectorX m_vec; // data vector
    unsigned int m_ref_comp; // component used for ordering and comparison

};

//////////////////////////
// INCLUDE HEADER FILES //
//////////////////////////

#include "MuonCalibMath/DataPoint.ixx"

}

#endif
