/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.04.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_CurvedLineH
#define MuonCalib_CurvedLineH

//::::::::::::::::::::::
//:: CLASS CurvedLine ::
//::::::::::::::::::::::

/// \class CurvedLine
///
/// This class describes a curved line in an MDT chamber. Let x,y,z denote the
/// local coordinate frame of an MDT chamber. Then the class assumes a
/// straight line in the xz and a parabolicly curved line in the yz plane.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 05.04.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// CLHEP //
#include "MdtCalibFitters/MTStraightLine.h"

// MuonCalib //
#include "MuonCalibMath/Legendre_polynomial.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace MuonCalib {

class CurvedLine {

public:
// Constructors //
    CurvedLine(void);
    ///< Default constructor: a straight line through (0,0,0) pointing in
    ///< in the local x direction of the chambers is created.

    CurvedLine(std::vector<Amg::Vector3D> & points);
    ///< Constructor.
    ///< \param points A straight line in the xz and a parabolic line in the
    ///<               yz plane is fitter through the given points.
    ///<  All points get equal weight.

    CurvedLine(std::vector<Amg::Vector3D> & points,
                                std::vector<Amg::Vector3D> & x_and_y_errors);
    ///< Constructor.
    ///< \param points A straight line in the xz and a parabolic line in the
    ///<               yz plane is fitter through the given points.
    ///< \param x_and_y_errors Errors of the x and y coordinates of the given
    ///<               points

// Methods //
// get-methods //
    Amg::Vector3D getPointOnLine(const double & loc_z) const;
                                    ///< get the point on the line a the local
                                    ///< z coordinate "loc_z"

    MTStraightLine getTangent(const double & loc_z) const;
                                    ///< get the tangent to the line a the local
                                    ///< z coordinate "loc_z"

private:
// parameters of the curved line //
    Legendre_polynomial *m_Legendre; // pointer to the Legendre polynomial
                                     // describing the curved line
    Amg::VectorX m_coeff_xz; // coefficients of the straight line in the local
                          // xz plane
    Amg::VectorX m_coeff_yz; // coefficients of the curved line in the local
                          // yz plane

// private methods //
    void init(std::vector<Amg::Vector3D> & points,
                                std::vector<Amg::Vector3D> & x_and_y_errors);
                                // initialization routine

};

}

#endif
