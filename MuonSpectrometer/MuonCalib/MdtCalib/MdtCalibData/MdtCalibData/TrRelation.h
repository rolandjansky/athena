/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 21.08.2009, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MUONCALIB_TrRelationH
#define MUONCALIB_TrRelationH

//::::::::::::::::::::::
//:: CLASS TrRelation ::
//::::::::::::::::::::::

/// \class TrRelation
///
/// This class provides the user with the inverse of the space
/// drift-relationship r(t). It should be initialized with an object of the
/// IRtRelation class, but it is also possible to provide a vector of
/// (r, t) pairs.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 21.08.2009

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibData/IRtRelation.h"

namespace MuonCalib {

    class TrRelation {
    public:
        ///< Constructor. The user has to provide the input r-t
        ///< relationship to be inverted.
        TrRelation(const IRtRelation &input_rt);

        ///< Constructor. The user has to provide (r, t) pairs stored in the
        ///< vectors r_values and t_values. The r and t values must be in
        ///< increasing order.
        TrRelation(const std::vector<double> &r_values, const std::vector<double> &t_values);

        // Methods //
        ///< Get t(r). out_of_bound_flag is set to true if r is out of bounds.
        double tFromR(const double &r, bool &out_of_bound_flag) const;
        ///< Get r(t). out_of_bound_flag is set to true if t is out of bounds.
        double rFromT(const double &t, bool &out_of_bound_flag) const;

    private:
        // (r, t) pairs
        std::vector<double> m_r;  // drift radii
        std::vector<double> m_t;  // drift times

        // r(t) inversion method
        static double getTFromR(const double &r, const IRtRelation &input_rt);
    };

}  // namespace MuonCalib

#endif
