/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ITrigMuonEFInfoToMuonCnvTool.h
//
//  Created by Mark Owen on 12/03/2014.
//

#ifndef xAODTrigMuonCnv_ITrigMuonEFInfoToMuonCnvTool_h
#define xAODTrigMuonCnv_ITrigMuonEFInfoToMuonCnvTool_h

// framework
#include "GaudiKernel/IAlgTool.h"

// needed edm classes
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

class TrigMuonEFInfoContainer;
class TrigMuonEFInfo;

static const InterfaceID IID_XAODEFMUONCNV_ITrigMuonEFInfoToMuonCnvTool("ITrigMuonEFInfoToMuonCnvTool",1,0);

class ITrigMuonEFInfoToMuonCnvTool :virtual public IAlgTool {
    
public:
    virtual ~ITrigMuonEFInfoToMuonCnvTool() {}
    
    static const InterfaceID& interfaceID();
    
    /// Convert a single TrigMuonEFInfo object into (maybe multiple) xAOD::Muon objects.
    virtual StatusCode convertTrigMuonEFInfo(const TrigMuonEFInfo& efinfo,
                                                 xAOD::MuonContainer& muoncontainer,
                                                 xAOD::TrackParticleContainer* combParticleContainer,
                                                 xAOD::TrackParticleContainer* extrapParticleContainer) = 0;
    
    /// Convert full TrigMuonEFInfoContainer into xAOD::MuonContainer
    virtual StatusCode convertTrigMuonEFInfoContainer(const TrigMuonEFInfoContainer& efinfocont,
                                                          xAOD::MuonContainer& muoncontainer,
                                                          xAOD::TrackParticleContainer* combParticleContainer,
                                                          xAOD::TrackParticleContainer* extrapParticleContainer)=0;
}; // end ITrigMuonEFInfoToMuonCnvTool class definition

inline const InterfaceID& ITrigMuonEFInfoToMuonCnvTool::interfaceID() {
    return IID_XAODEFMUONCNV_ITrigMuonEFInfoToMuonCnvTool;
}

#endif
