/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonAlignmentUncertTool
//    Interface tool to assess the muon alignment and scattering
//    uncertainties using calibration files
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUONALIGNMENTUNCERTTOOL_H
#define MUIDINTERFACES_IMUONALIGNMENTUNCERTTOOL_H

#include <GaudiKernel/IAlgTool.h>
#include <TrkTrack/TrackStateOnSurface.h>
#include <TrkTrack/Track.h>

namespace Muon {
  
    /** Interface ID for ICombinedMuonTrackBuilder*/  
    static const InterfaceID IID_IMuonAlignmentUncertTool("IMuonAlignmentUncertTool", 1, 0);
  
    /**@class IMuonAlignmentUncertTool
        Base class for MuonAlignmentUncertTool AlgTool
    */
    class IMuonAlignmentUncertTool : virtual public IAlgTool {
        public:
            /**Virtual destructor*/
            virtual ~IMuonAlignmentUncertTool()= default;
            
            /** AlgTool and IAlgTool interface methods */
            static const InterfaceID&	interfaceID() { return IID_IMuonAlignmentUncertTool; }

            /**IMuonAlignmentUncertTool interface: retrieve the associated uncertainties */
            virtual float get_uncertainty (const Trk::Track* trk) const = 0;
            
            /// Helper method to retrieve the last measurement of the ID, 
            /// the first and last scatterer in the calorimeter
            /// and finally to retrieve the first measurement inside the MS
            virtual void get_track_state_measures(const Trk::Track* track, const Trk::TrackStateOnSurface*& id_exit,
                                                  const Trk::TrackStateOnSurface*& calo_entrance,
                                                  const Trk::TrackStateOnSurface*& calo_exit,
                                                  const Trk::TrackStateOnSurface*& ms_entrance) const  = 0;
    };
 
}	// end of namespace

#endif


