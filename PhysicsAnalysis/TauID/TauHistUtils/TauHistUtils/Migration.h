/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_MIGRATION_H
#define TAUHISTUTILS_MIGRATION_H

#include "TrkValHistUtils/PlotBase.h"
#include "TauHistUtils/ParamPlots.h"
#include "xAODTau/TauDefs.h" 
#include "xAODTau/TauJet.h"
#include "xAODPFlow/versions/PFO_v1.h"

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
	Migration(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
	virtual ~Migration();

	void fill(const xAOD::TauJet& tau,int nProng, int nNeu);
	
	//Tau::ParamPlots m_oParamPlots;
	
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
	/*
	    "t10r10",
	    "t10r11",
	    "t10r1x",
	    "t10r30",
	    "t10r3x",
	    "t11r10",
	    "t11r11",
	    "t11r1x",
	    "t11r30",
	    "t11r3x",
	    "t1xr10",
	    "t1xr11",
	    "t1xr1x",
	    "t1xr30",
	    "t1xr3x",
	    "t30r10",
	    "t30r11",
	    "t30r1x",
	    "t30r30",
	    "t30r3x",
	    "t3xr10",
	    "t3xr11",
	    "t3xr1x",
	    "t3xr30",
	    "t3xr3x"*/
	
	TH1* m_migration_tauRec;
	TH1* m_migration_eflow;
	TH1* m_migration_cellBased;
    private:
	void initializePlots();
	std::string m_sTauJetContainerName;
	void decayModeFill(int trueP,int trueN,int recP, int recN,TH1 *histo);
    };
    
}

#endif
