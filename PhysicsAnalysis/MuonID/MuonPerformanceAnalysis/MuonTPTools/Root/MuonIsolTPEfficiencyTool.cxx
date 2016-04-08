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
    for(auto probe : *probes)
    {
      // check if the probe is isolated
      // then if it is:
        probe->isMatched(true);
    }
}

