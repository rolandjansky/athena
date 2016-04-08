/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonRecoTPEfficiencyTool.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef ZMUMUMUONTPEFFICIENCYTOOL_H_
#define ZMUMUMUONTPEFFICIENCYTOOL_H_

#include "MuonTPTools/MuonTPEfficiencyTool.h"
class MuonRecoTPEfficiencyTool : public MuonTPEfficiencyTool{
  ASG_TOOL_CLASS(MuonRecoTPEfficiencyTool, IMuonTPEfficiencyTool)
    public:
  MuonRecoTPEfficiencyTool(std::string name);

  /// Match probes for efficiency calculation
  virtual void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const;
  void dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const;
  void ptrMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const;
  bool GoodMatchMuonType(const xAOD::IParticle* probe) const;
 private:

  bool m_ptrMatching;
  bool m_match_MS;
  bool m_match_CB;
  bool m_match_Loose;
  bool m_match_Loose_noCT;
  bool m_match_Medium;
  bool m_match_Tight;
  bool m_match_HighPt;
  bool m_match_ID;
  bool m_match_CaloTag;
  bool m_match_MuidCB;
  bool m_match_STACO;
  bool m_match_MuTag;
  bool m_match_MuTagIMO;
  bool m_match_MuidSA;
  bool m_match_MuGirl;
  bool m_match_MuGirlLowBeta;
  bool m_match_CaloLikelihood;
  bool m_match_ExtrapolateToIP;
  bool m_do_IDHits;

};

#endif /* ZMUMUMUONTPEFFICIENCYTOOL_H_ */
