/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPEfficiencyTool.h
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#ifndef JPSIMUONTPEFFICIENCYTOOL_H_
#define JPSIMUONTPEFFICIENCYTOOL_H_

#include "MuonTPTools/MuonTPEfficiencyTool.h"
class JPsiMuonTPEfficiencyTool : public MuonTPEfficiencyTool{
	  ASG_TOOL_CLASS(JPsiMuonTPEfficiencyTool, IMuonTPEfficiencyTool)
public:
	JPsiMuonTPEfficiencyTool(std::string name);

    /// Match probes for efficiency calculation
	  void dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const;
	  bool GoodMatchMuonType(const xAOD::IParticle* probe) const;
private:
	 bool m_match_MS;
     bool m_match_CB;
     bool m_match_Medium;
     bool m_match_ID;
     bool m_match_CaloTag;
     bool m_do_IDHits;

};

#endif /* JPSIMUONTPEFFICIENCYTOOL_H_ */
