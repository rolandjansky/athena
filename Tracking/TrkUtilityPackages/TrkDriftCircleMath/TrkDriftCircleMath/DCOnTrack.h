/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_DCONTRACK_H
#define DCMATH_DCONTRACK_H

#include <ostream>

#include "GaudiKernel/MsgStream.h"
#include "TrkDriftCircleMath/DriftCircle.h"

namespace TrkDriftCircleMath {

    /** class representing a drift circle meaurement on segment */
    class DCOnTrack : public DriftCircle {
    public:
        /** enum to identify different DC states */
        enum DCOnTrackState {
            OnTrack,    /** used in fit */
            Delta,      /** delta electron */
            OutOfTime,  /** too large drift time */
            CloseDC,    /** inside road but too far to be on track */
            OutsideRoad /** outside road */
        };

        /** minimal constructor
            @param dc DriftCircle
            @param res residual
            @param err error of track prediction
        */
        DCOnTrack(const DriftCircle& dc, double res = 0., double err = 1e9) : DriftCircle{dc}, m_residual{res}, m_errorTrack{err} {}

        /** set residual */
        void residual(double res) { m_residual = res; }

        /** access residual */
        double residual() const { return m_residual; }

        /** set track error */
        void errorTrack(double error) { m_errorTrack = error; }

        /** access track error */
        double errorTrack() const { return m_errorTrack; }

        /** set DCOnTrack state */
        void state(DCOnTrackState st) { m_state = st; }

        /** access DCOnTrack state */
        DCOnTrackState state() const { return m_state; }

    private:
        double m_residual{0.};
        double m_errorTrack{1.e9};
        // cppcheck-suppress duplInheritedMember
        DCOnTrackState m_state{OnTrack};  // Masks DriftCircle::m_state which is a DriftState
    };

    typedef std::vector<DCOnTrack> DCOnTrackVec;
    typedef DCOnTrackVec::iterator DCOnTrackIt;
    typedef DCOnTrackVec::const_iterator DCOnTrackCit;

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::DCOnTrack& dc);

    MsgStream& operator<<(MsgStream& os, const TrkDriftCircleMath::DCOnTrack& dc);

}  // namespace TrkDriftCircleMath

#endif
