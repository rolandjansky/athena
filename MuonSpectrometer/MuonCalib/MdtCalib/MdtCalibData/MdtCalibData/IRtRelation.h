/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IRTRELATION_H
#define MUONCALIB_IRTRELATION_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for a rt-relation */

    class IRtRelation : public CalibFunc {
    public:
        explicit IRtRelation(const CalibFunc::ParVec& vec) : CalibFunc(vec), m_tmax_diff(9e9){};
        virtual ~IRtRelation() = default;
        virtual std::string typeName() const { return "IRtRelation"; }

        /** returns drift radius for a given time */
        virtual double radius(double t) const = 0;
        virtual double driftvelocity(double t) const = 0;
        virtual double tLower(void) const = 0;
        virtual double tUpper(void) const = 0;

        /** return the difference in total dirft time between the two multilayers (ML1 - ML2) */
        float GetTmaxDiff() const { return HasTmaxDiff() ? m_tmax_diff : 0.0; }

        bool HasTmaxDiff() const { return (m_tmax_diff < 8e8); }

        /** set the difference in total drift time betwene the two multilayers (ML1 - ML2) */
        void SetTmaxDiff(const float& d) { m_tmax_diff = d; }

        float m_tmax_diff;
    };

}  // namespace MuonCalib

#endif
