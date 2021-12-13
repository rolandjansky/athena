/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IRECIMuonTrackToSegmentAssociationTool_H
#define IRECIMuonTrackToSegmentAssociationTool_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegmentContainer.h"


namespace MuonCombined {

  
    /** @class IMuonTrackToSegmentAssociationTool
        @brief Interface to associate the Muon segments to the muon track
     */

    class IMuonTrackToSegmentAssociationTool : virtual public IAlgTool {
    public:
        
       
        virtual ~IMuonTrackToSegmentAssociationTool() = default;
        static const InterfaceID& interfaceID(){
            static const InterfaceID IID_IMuonTrackToSegmentAssociationTool("MuonCombined::IMuonTrackToSegmentAssociationTool", 1, 0);
            return IID_IMuonTrackToSegmentAssociationTool;
        }

        /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */
        virtual bool associatedSegments(const Trk::Track& track, const xAOD::MuonSegmentContainer* segments,
                                std::vector<ElementLink<xAOD::MuonSegmentContainer> >& assocSegmentVec) const = 0;

        
        virtual bool associatedSegments(const xAOD::Muon& muon, const xAOD::MuonSegmentContainer* segments,
                                std::vector<ElementLink<xAOD::MuonSegmentContainer> >& assocSegmentVec) const = 0;
    };

 
}  // namespace MuonCombined

#endif
