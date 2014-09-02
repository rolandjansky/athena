/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// MdtDriftCirleStatus.h
//   Header file for class CscClusterStatus
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement MdtDriftCirleStatus for Muon - MDT
///////////////////////////////////////////////////////////////////
// Niels van Eldik
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_MDTPREPDATASTATUS_H
#define MUONPREPRAWDATA_MDTPREPDATASTATUS_H

namespace  Muon {

    /** @brief Enum to represent the 'status' of Mdt measurements e.g. whether the tube was masked at the time. 
    See enum values for more details*/
    enum MdtDriftCircleStatus
    {
        /** A hit during a previous bunch crossing occured which rendered 
        the tube dead for this bunch crossing.  
        It is not excluded that a particle belonging to the current event 
        passed the tube. No time was recorded.
        */
        MdtStatusMasked = 0,
        
        /** The tube produced a vaild measurement */
        MdtStatusDriftTime = 1,

        /** The tube produced a hit that is inconsistent with the drift time spectrum, the drift time is smaller than t0  */
        MdtStatusBeforeSpectrum = 2,

        /** The tube produced a hit that is inconsistent with the drift time spectrum, the drift time is larger than tmax  */
        MdtStatusAfterSpectrum = 3,

        /** Undefined. This shouldn't happen, and almost certainly indicates a problem! */
        MdtStatusUnDefined = 999
    };
    
}

#endif
