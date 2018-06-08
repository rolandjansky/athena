/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPROCESSORALG_H
#define TAUREC_TAUPROCESSORALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "tauRecTools/ITauToolExecBase.h"
#include "tauRecTools/ITauToolBase.h"

#include "tauRecTools/TauEventData.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

/**
 * @brief       Main class for tau candidate processing.
 */

class TauProcessorAlg: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauProcessorAlg( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauProcessorAlg();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    private:
       
	ToolHandleArray<ITauToolBase>  m_tools;

	double m_maxEta; //!< only build taus with eta_seed < m_maxeta
	double m_minPt;  //!< only build taus with pt_seed > m_minpt

	bool m_doCreateTauContainers;

        //ToolHandleArray<ITauToolExecBase>  m_tools;
	TauEventData m_data;

	SG::ReadHandleKey<xAOD::JetContainer> m_jetInputContainer{this,"Key_jetInputContainer","AntiKt4LCTopoJets","input jet key"};
	SG::WriteHandleKey<xAOD::TauJetContainer> m_tauOutputContainer{this,"Key_tauOutputContainer","TauJets","output tau data key"};
	SG::WriteHandleKey<xAOD::TauTrackContainer> m_tauTrackOutputContainer{this,"Key_tauTrackOutputContainer","TauTracks","output tau tracks data key"};

	
};

#endif // TAUREC_TAUPROCESSORALG_H
