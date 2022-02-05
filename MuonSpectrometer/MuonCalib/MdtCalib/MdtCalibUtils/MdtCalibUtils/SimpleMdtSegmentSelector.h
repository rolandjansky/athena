/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleMdtSegmentSelector.h
//   Header file for class SimpleMdtSegmentSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_SIMPLEMDTSEGMENTSELECTOR_H
#define MUONCALIB_SIMPLEMDTSEGMENTSELECTOR_H

#include "MdtCalibInterfaces/IMdtSegmentSelector.h"

namespace MuonCalib {

    class MuonCalibSegment;

    /**@class SimpleMdtSegmentSelector
       Basic MdtSegments selector. Gets MdtSegments and tests if they satisfy the selection criteria:

       - chi2

       - number of hits

       Returns true if they do.

       @author Niels.Van.Eldik@cern.ch
    */

    class SimpleMdtSegmentSelector : public IMdtSegmentSelector {
    public:
        /** constructor */
        SimpleMdtSegmentSelector(double chi2_max, unsigned int hits_min);

        /** test MdtSegment on selection criteria */
        virtual bool select(const MuonCalibSegment& seg) const;

        /** set print level */
        void setPrintLevel(int level) { m_printLevel = level; }

    private:
        /** selection criteria */
        double m_chi2_max;        //!< maximum chi2 of segment
        unsigned int m_hits_min;  //!< minimum numbers of hits on segment

        /** print level */
        int m_printLevel;
    };

}  // namespace MuonCalib

#endif
