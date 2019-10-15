/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IsolationSelection_IsolationLowPtPLVTool_H
#define IsolationSelection_IsolationLowPtPLVTool_H

#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <IsolationSelection/IIsolationLowPtPLVTool.h>

#include <IsolationSelection/Defs.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODBase/IParticleContainer.h>

#include <vector>
#include <string>

// TMVA
#include "TMVA/Reader.h"

namespace CP {
    class IsolationLowPtPLVTool: public asg::AsgTool, public virtual IIsolationLowPtPLVTool {

        public:
            IsolationLowPtPLVTool(const std::string& name);
	    ASG_TOOL_CLASS(IsolationLowPtPLVTool, IIsolationLowPtPLVTool)
            virtual StatusCode initialize() override;
            StatusCode AugmentPLV(xAOD::IParticle* Particles) override;
	    
        private:
	    IntAccessor m_acc_TrackJetNTrack;
	    FloatAccessor m_acc_DRlj;
	    FloatAccessor m_acc_PtRel;
	    FloatAccessor m_acc_PtFrac;
	    FloatAccessor m_acc_topoetcone20;
	    FloatAccessor m_acc_ptvarcone20;
	    FloatAccessor m_acc_ptvarcone30;
	    FloatDecorator m_dec_iso_PLT;	    

	    std::unique_ptr<TMVA::Reader> m_TMVAReader_Muon;
	    std::unique_ptr<TMVA::Reader> m_TMVAReader_Elec;
	    std::unique_ptr<float []> m_varTMVA_Muon;
	    std::unique_ptr<float []> m_varTMVA_Elec;
    };

}
#endif
