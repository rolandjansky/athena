/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTrigTPEfficiencyTool.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc / a.m.
 */

#ifndef MUONTRIGTPEFFICIENCYTOOL_H_
#define MUONTRIGTPEFFICIENCYTOOL_H_

#include "MuonTPTools/MuonTPEfficiencyTool.h"


// FrameWork includes
//#include "GaudiKernel/ServiceHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"



class MuonTrigTPEfficiencyTool : public MuonTPEfficiencyTool{
	  ASG_TOOL_CLASS(MuonTrigTPEfficiencyTool, IMuonTPEfficiencyTool)

public:
    MuonTrigTPEfficiencyTool(std::string name);

    // Match probes for efficiency calculation
    void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const;
    
private:

    float m_dR_L1;
    float m_dR_HLT;

};

#endif /* MUONTRIGTPEFFICIENCYTOOL_H_ */
