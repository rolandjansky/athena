/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ZMUMUMUONTPISOLATIONTreeTOOL_H_
#define ZMUMUMUONTPISOLATIONTreeTOOL_H_
#include "MuonTPTools/IMuonTPTreeTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
#include "AsgTools/AsgTool.h"
#include <map>

class ZmumuMuonTPIsolationTreeTool:
        public MuonTPTreeTool {         // for now, a 1:1 copy
        ASG_TOOL_CLASS(ZmumuMuonTPIsolationTreeTool, IMuonTPTreeTool)

public:
        ZmumuMuonTPIsolationTreeTool(std::string name);

    virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
    virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

protected:
    float m_runNumber;
    float m_eventNumber;
    float m_mu;
    float m_mcEventWeight;
    float m_probePt;
    float m_probeEta;
    float m_probePhi;
    int m_fineEtaPhi;
    int m_probe_q;
    int m_tag_q;

    float m_tagPt;
    float m_tagEta;
    float m_tagPhi;

    float m_mll;
    float m_d0;
    float m_d0err;
    float m_z0;

// isolation value
  float m_tagiso_neflowisol20;
  float m_tagiso_topoetcone20;
  float m_tagiso_ptcone20;
  float m_tagiso_etcone20;
  float m_tagiso_ptvarcone20;

  float m_tagiso_neflowisol30;
  float m_tagiso_topoetcone30;
  float m_tagiso_ptcone30;
  float m_tagiso_etcone30;
  float m_tagiso_ptvarcone30;

  float m_tagiso_neflowisol40;
  float m_tagiso_topoetcone40;
  float m_tagiso_ptcone40;
  float m_tagiso_etcone40;
  float m_tagiso_ptvarcone40;

  float m_probeiso_neflowisol20;
  float m_probeiso_topoetcone20;
  float m_probeiso_ptcone20;
  float m_probeiso_etcone20;
  float m_probeiso_ptvarcone20;

  float m_probeiso_neflowisol30;
  float m_probeiso_topoetcone30;
  float m_probeiso_ptcone30;
  float m_probeiso_etcone30;
  float m_probeiso_ptvarcone30;

  float m_probeiso_neflowisol40;
  float m_probeiso_topoetcone40;
  float m_probeiso_ptcone40;
  float m_probeiso_etcone40;
  float m_probeiso_ptvarcone40;

  int m_Npvtx;
  float m_dPhi;

    fineEtaPhiBinning m_fepb;
        
};

#endif /* ZMUMUMUONTPISOLATIONTreeTOOL_H_ */
