/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  TrigMuonEFInfoToMuonCnvTool.h
//
//  Created by Mark Owen on 12/03/2014.
//

#ifndef xAODTrigMuonCnv_TrigMuonEFInfoToMuonCnvTool_h
#define xAODTrigMuonCnv_TrigMuonEFInfoToMuonCnvTool_h

#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTrigMuonCnv/ITrigMuonEFInfoToMuonCnvTool.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <string>

class TrigMuonEFInfoToMuonCnvTool : public AthAlgTool, public virtual ITrigMuonEFInfoToMuonCnvTool {
    
public:
    
    /// Standard constructor
    TrigMuonEFInfoToMuonCnvTool(const std::string& type, const std::string& name, const IInterface* parent);
    
    /// Destructor
    virtual ~TrigMuonEFInfoToMuonCnvTool()=default;
    
    /// initialize the tool
    virtual StatusCode initialize() override;
    
    /// Convert a single TrigMuonEFInfo object into (maybe multiple) xAOD::Muon objects.
    virtual StatusCode convertTrigMuonEFInfo(const TrigMuonEFInfo& efinfo,
                                                 xAOD::MuonContainer& muoncontainer,
                                                 xAOD::TrackParticleContainer* combParticleContainer,
                                                 xAOD::TrackParticleContainer* extrapParticleContainer) const override;
    
    /// Convert full TrigMuonEFInfoContainer into xAOD::MuonContainer
    virtual StatusCode convertTrigMuonEFInfoContainer(const TrigMuonEFInfoContainer& efinfocont,
                                                          xAOD::MuonContainer& muoncontainer,
                                                          xAOD::TrackParticleContainer* combParticleContainer,
                                                          xAOD::TrackParticleContainer* extrapParticleContainer) const override;
    
protected:
    
    /// Convert TrigMuonEFTrack into a xAOD::TrackParticle
    StatusCode convertTrigMuonEFTrack(const TrigMuonEFTrack& eftrack, xAOD::TrackParticle& trackpart) const;
    
private:
    
    /// Flag to decide if we want to make element link to EF ID xAOD tracks - only turn on once EF ID conversion is done
    bool m_makeIDTrackLink;
    
    /// Name of container that holds the EF ID xAOD track particles
    std::string m_inDetTrackParticles;
    
};// end class definition for TrigMuonEFInfoToMuonCnvTool

#endif
