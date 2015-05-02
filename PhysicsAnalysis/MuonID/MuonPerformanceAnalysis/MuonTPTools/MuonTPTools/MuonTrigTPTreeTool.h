/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGTPTreeTOOL_H_
#define MUONTRIGTPTreeTOOL_H_

#include "MuonTPTools/IMuonTPTreeTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
#include "AsgTools/AsgTool.h"
#include <map>

class MuonTrigTPTreeTool:
        public MuonTPTreeTool {         // for now, a 1:1 copy
        ASG_TOOL_CLASS(MuonTrigTPTreeTool, IMuonTPTreeTool)

public:
    MuonTrigTPTreeTool(std::string name);

    virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
    virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

protected:
    float m_runNumber;
    float m_eventNumber;
    float m_mu;
    float m_mcEventWeight;
    float m_pt;
    float m_eta;
    float m_phi;
    float m_ptcone40;
    float m_etcone40;
    int   m_fineEtaPhi;
    int   m_q;
    float m_tagPt;
    float m_tagEta;
    float m_tagPhi;
    float m_mll;
    float m_d0;
    float m_d0err;
    float m_z0;

    fineEtaPhiBinning m_fepb;
        
};

#endif /* MUONTRIGTPTreeTOOL_H_ */
