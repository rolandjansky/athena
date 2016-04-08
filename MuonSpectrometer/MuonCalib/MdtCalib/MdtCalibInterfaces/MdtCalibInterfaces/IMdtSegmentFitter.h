/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMdtSegmentFitter.h
//   Header file for class IMdtSegmentFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_IMDTSEGMENTFITTER_H
#define MUONCALIB_IMDTSEGMENTFITTER_H

#include <vector>

namespace MuonCalib {

  /**
     @class IMdtSegmentFitter
     Interface for MdtSegment fitters.
   
     @author Niels.Van.Eldik@cern.ch
  */

  class MuonCalibSegment;

  class IMdtSegmentFitter {
  public:
    typedef std::vector<unsigned int> HitSelection;

    /** destructor */
    virtual ~IMdtSegmentFitter() {}

    /** fit using all hits */
    virtual bool fit( MuonCalibSegment& seg ) const = 0;

    /** fit subset of the hits.
	If the HitSelection vector contains a 0 for a given hit the hit is used
	else the hit is not included in the fit.
	The size of the HitSelection vector should be equal to the number of hits on track else
	no fit is performed.
    */
    virtual bool fit( MuonCalibSegment& seg, HitSelection selection ) const = 0;

    virtual void printLevel(int) = 0;
  };

 
}

#endif
