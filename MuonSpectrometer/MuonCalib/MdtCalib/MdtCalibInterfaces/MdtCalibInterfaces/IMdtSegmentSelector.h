/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMdtSegmentSelector.h
//   Header file for class IMdtSegmentSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_IMDTSEGMENTSELECTOR_H
#define MUONCALIB_IMDTSEGMENTSELECTOR_H

namespace MuonCalib {
    /**
       @class IMdtSegmentSelector
       Interface class for MdtSegmentSelectors.

       @author Niels.Van.Eldik@cern.ch
    */

    class MuonCalibSegment;

    class IMdtSegmentSelector {
    public:
        /** destructor */
        virtual ~IMdtSegmentSelector() {}

        /** return true if segment satisfies selection criteria */
        virtual bool select(const MuonCalibSegment& seg) const = 0;

        /** set print level */
        virtual void setPrintLevel(int level) = 0;
    };

}  // namespace MuonCalib

#endif
