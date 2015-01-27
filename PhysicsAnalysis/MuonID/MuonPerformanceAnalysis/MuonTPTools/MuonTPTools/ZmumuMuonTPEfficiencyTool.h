/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPEfficiencyTool.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef ZMUMUMUONTPEFFICIENCYTOOL_H_
#define ZMUMUMUONTPEFFICIENCYTOOL_H_

#include "MuonTPTools/MuonTPEfficiencyTool.h"
class ZmumuMuonTPEfficiencyTool : public MuonTPEfficiencyTool{
	  ASG_TOOL_CLASS(ZmumuMuonTPEfficiencyTool, IMuonTPEfficiencyTool)
public:
	ZmumuMuonTPEfficiencyTool(std::string name);

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

#endif /* ZMUMUMUONTPEFFICIENCYTOOL_H_ */
