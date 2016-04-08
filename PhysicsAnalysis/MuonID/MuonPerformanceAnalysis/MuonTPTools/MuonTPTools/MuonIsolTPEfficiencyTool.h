/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonIsolTPEfficiencyTool.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc / a.m.
 */

#ifndef MUONISOLTPEFFICIENCYTOOL_H_
#define MUONISOLTPEFFICIENCYTOOL_H_

#include "MuonTPTools/MuonTPEfficiencyTool.h"


class MuonIsolTPEfficiencyTool : public MuonTPEfficiencyTool{
	  ASG_TOOL_CLASS(MuonIsolTPEfficiencyTool, IMuonTPEfficiencyTool)

public:
    MuonIsolTPEfficiencyTool(std::string name);

    // Match probes for efficiency calculation
    void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const;
     
private:
    

};

#endif /* MUONISOLTPEFFICIENCYTOOL_H_ */
