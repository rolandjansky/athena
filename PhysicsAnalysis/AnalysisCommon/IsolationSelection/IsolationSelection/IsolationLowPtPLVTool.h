/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IsolationSelection_IsolationLowPtPLVTool_H
#define IsolationSelection_IsolationLowPtPLVTool_H

#include <IsolationSelection/IIsolationLowPtPLVTool.h>
#include <IsolationSelection/Defs.h>

#include <AsgTools/AsgTool.h>

// TMVA
#include "TMVA/Reader.h"

#include <memory>

namespace CP {
    class IsolationLowPtPLVTool: public asg::AsgTool, public virtual IIsolationLowPtPLVTool {

        public:
            IsolationLowPtPLVTool(const std::string& name);
	    ASG_TOOL_CLASS(IsolationLowPtPLVTool, IIsolationLowPtPLVTool)
            virtual StatusCode initialize() override;
	    virtual StatusCode augmentPLV( const xAOD::IParticle& particle ) override;
	    
        private:
	    std::string m_muonCalibFile = "";
	    std::string m_elecCalibFile = "";
	    std::string m_muonMethodName = "";
	    std::string m_elecMethodName = "";
	    std::unique_ptr<TMVA::Reader> m_TMVAReader_Muon;
	    std::unique_ptr<TMVA::Reader> m_TMVAReader_Elec;
	    static const int N_VARIABLES = 6;
	    float m_varTMVA_Muon[ N_VARIABLES ];
	    float m_varTMVA_Elec[ N_VARIABLES ];

	    static const int N_MSG_COUNTS = 100;
	    unsigned int varMissingMsgCounter = 0;
    };

}
#endif
