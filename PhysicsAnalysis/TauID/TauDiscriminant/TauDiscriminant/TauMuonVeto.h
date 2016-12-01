/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUMUONVETO_H
#define TAUMUONVETO_H

#include "tauRecTools/TauRecToolBase.h"
#include <string>

class TauMuonVeto: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauMuonVeto, TauRecToolBase, ITauToolBase)
    public:

        TauMuonVeto(const std::string& name = "TauMuonVeto"):
            TauRecToolBase(name)
        {
	}

        virtual ~TauMuonVeto() {}

	virtual StatusCode initialize(){ return StatusCode::SUCCESS; }
	
        virtual StatusCode execute(xAOD::TauJet&);

	virtual StatusCode finalize(){ return StatusCode::SUCCESS; }
        
};

#endif // TAUMUONVETO_H
