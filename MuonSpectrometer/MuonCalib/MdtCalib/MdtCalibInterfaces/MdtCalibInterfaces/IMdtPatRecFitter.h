/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONCALIB_IMDTPATRECFITTER_H
#define MUONCALIB_IMDTPATRECFITTER_H

// MdtCalibInterfaces
#include "IMdtSegmentFitter.h"

namespace MuonCalib {
    class MdtCalibHitBase;

    /** @class IMdtPatRecFitter
    Interface class for MdtSegment fitters performing a pattern recognition

    @author rauscher@cern.ch
    */

    class IMdtPatRecFitter : public IMdtSegmentFitter {
    public:
        /** constructor */
        IMdtPatRecFitter() : IMdtSegmentFitter(), m_refine_segment(false), m_refit(false) {}
        /** destructor */
        virtual ~IMdtPatRecFitter() = default;

        /** number of hits selected for track */
        /// virtual unsigned int numberOfTrackHits() const = 0;

        /** get selected track hits */
        /// virtual const std::vector<const MdtCalibHitBase *> &trackHits() const = 0;

        /** set refine segment flag
         @param flag if true the hit selection is changed in the segment
        */
        void SetRefineSegmentFlag(const bool flag) { m_refine_segment = flag; }

        /** get refine segment flag */
        bool RefineSegmentFlag() const { return m_refine_segment; }

        /** switch on/off chi^2 refit after hit selection */
        void switchOnRefit() { m_refit = true; }
        void switchOffRefit() { m_refit = false; }
        /** return refit flag */
        bool refit() const { return m_refit; }
        /** set road width */
        virtual void setRoadWidth(const double &) = 0;

    protected:
        //! flags
        bool m_refine_segment;
        bool m_refit;
    };

}  // namespace MuonCalib

#endif
