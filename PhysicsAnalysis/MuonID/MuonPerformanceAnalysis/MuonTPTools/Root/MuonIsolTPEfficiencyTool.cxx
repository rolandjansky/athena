/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonIsolTPEfficiencyTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc / a.m.
 */

#include "MuonTPTools/MuonIsolTPEfficiencyTool.h"

MuonIsolTPEfficiencyTool::MuonIsolTPEfficiencyTool(std::string myname)
:  MuonTPEfficiencyTool(myname)
{
}



//---------------------------------------------------------
void MuonIsolTPEfficiencyTool::matchProbes(ProbeContainer* probes, const xAOD::IParticleContainer* ) const
{
    float sf = 1.;
    bool match = false;
    for(auto probe : *probes)
    {
        sf = 1.;
        probe->sf_isol(1.);
        match = false;
        const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*>(&(probe->probeTrack()));
        if (probemu){
            match = (probemu && m_isolTool->accept(*probemu));
            if (m_do_sf &&m_isol_sf_tool->getEfficiencyScaleFactor(*probemu,sf) == CP::CorrectionCode::Ok){
                probe->sf_isol(sf);
            }
            else {
                if (m_do_sf) ATH_MSG_DEBUG("Unable to retrieve isolation SF for "<<m_efficiencyFlag);
                probe->sf_isol(1.);
            }
            
        }
        else {
            const xAOD::TrackParticle* probetrk = dynamic_cast<const xAOD::TrackParticle*>(&(probe->probeTrack()));
            if (!probetrk){

            }
            if(m_IDtrack_isol_tool->decorate(probetrk).isFailure()){
                ATH_MSG_ERROR("Unable to decorate ID track during ID isolation matching");
            }
            match = m_isolTool->accept(*probetrk);
        }
        probe->isMatched(match);
    }
}

