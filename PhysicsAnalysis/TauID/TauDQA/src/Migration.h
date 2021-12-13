/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_MIGRATION_H
#define TAUDQA_MIGRATION_H

#include "TrkValHistUtils/PlotBase.h"
#include "ParamPlots.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"

namespace Tau{

    enum DecayMode {
	t10r10,
	t11r11,
	t1xr1x,
	t30r30,
	t3xr3x,
	t10r11,
	t10r1x,
	t11r10,
	t11r1x,
	t1xr10,
	t1xr11,
	t30r3x,
	t3xr30,
	t1r3,
	t3r1,
	DECAYSIZE
    };
    class Migration: public PlotBase {
    public:
	Migration(PlotBase *pParent, const std::string& sDir, std::string sTauJetContainerName);
	virtual ~Migration();

	/* void fill(const xAOD::TauJet& tau,int nProng, int nNeu); */
	void fill(const xAOD::TauJet& tau, xAOD::TauJetParameters::DecayMode trueMode, float weight);
	
	const char *m_lable[DECAYSIZE] = {
	    "t10r10",
	    "t11r11",
	    "t1xr1x",
	    "t30r30",
	    "t3xr3x",
	    "t10r11",
	    "t10r1x",
	    "t11r10",
	    "t11r1x",
	    "t1xr10",
	    "t1xr11",
	    "t30r3x",
	    "t3xr30",
	    "t1r3",
	    "t3r1"
	};
	
	TH1* m_migration_panTau;
	TH1* m_migration_panTauProto;
	TH1* m_migration_cellBased;
    private:
	void initializePlots();
	std::string m_sTauJetContainerName;
	void decayModeFill(int trueMode, int recMode, TH1 *histo, float weight);
	void decayModeFill(int trueMode, int recP, int recN, TH1 *histo, float weight);
    };
    
}

#endif
