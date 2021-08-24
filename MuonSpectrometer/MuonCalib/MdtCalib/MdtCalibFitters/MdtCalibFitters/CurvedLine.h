/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibMath/Legendre_polynomial.h"

namespace MuonCalib {

    class CurvedLine {
    public:
        // Constructors //
        CurvedLine();
        ///< Default constructor: a straight line through (0,0,0) pointing in
        ///< in the local x direction of the chambers is created.

        CurvedLine(std::vector<Amg::Vector3D> &points);
        ///< Constructor.
        ///< \param points A straight line in the xz and a parabolic line in the
        ///<               yz plane is fitter through the given points.
        ///<  All points get equal weight.

        CurvedLine(std::vector<Amg::Vector3D> &points, std::vector<Amg::Vector3D> &x_and_y_errors);
        ///< Constructor.
        ///< \param points A straight line in the xz and a parabolic line in the
        ///<               yz plane is fitter through the given points.
        ///< \param x_and_y_errors Errors of the x and y coordinates of the given
        ///<               points

        // Methods //
        // get-methods //
        Amg::Vector3D getPointOnLine(const double &loc_z) const;
        ///< get the point on the line a the local
        ///< z coordinate "loc_z"

        MTStraightLine getTangent(const double &loc_z) const;
        ///< get the tangent to the line a the local
        ///< z coordinate "loc_z"

        /// Cache the chi2
        void setChi2(double chi2);
        double chi2() const;

        ///  cache the number of track hits
        void setNumberOfTrackHits(unsigned int n_hits);
        unsigned int numberOfTrackHits() const;
        /// Return chi2 / number of TrackHits - 3
        double chi2PerDegreesOfFreedom() const;

        void setUsedHits(const std::vector<const MdtCalibHitBase *> &hits);
        const std::vector<const MdtCalibHitBase *> &trackHits() const;

    private:
        // parameters of the curved line //
        const Legendre_polynomial *m_Legendre;  // pointer to the Legendre polynomial
                                          // describing the curved line
        Amg::VectorX m_coeff_xz;          // coefficients of the straight line in the local
                                          // xz plane
        Amg::VectorX m_coeff_yz;          // coefficients of the curved line in the local
                                          // yz plane

        double m_chi2{-1};
        unsigned int m_numTrkHits{0};
        std::vector<const MdtCalibHitBase *> m_used_hits;

        // private methods //
        void init(std::vector<Amg::Vector3D> &points, std::vector<Amg::Vector3D> &x_and_y_errors);
        // initialization routine
    };

}  // namespace MuonCalib

#endif
