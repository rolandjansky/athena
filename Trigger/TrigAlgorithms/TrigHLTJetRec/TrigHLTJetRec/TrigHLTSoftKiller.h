/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTSOFTKILLER_H
#define TRIGHLTJETREC_TRIGHLTSOFTKILLER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "JetRecTools/JetConstituentModSequence.h"

class TrigHLTSoftKiller : public HLT::FexAlgo
{

    public:
        TrigHLTSoftKiller(const std::string& name, ISvcLocator* pSvcLocator);
        ~TrigHLTSoftKiller();

        HLT::ErrorCode hltInitialize();
        HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                                        HLT::TriggerElement* outputTE);
        HLT::ErrorCode hltFinalize();

    private:

       ToolHandle<IJetConstituentModifier> m_skWeightTool;
       ToolHandle<IJetExecuteTool> m_skclustModSeqTool; 
       std::string m_outputCollectionLabel;
       std::string m_clusterCalib;
        
};

#endif
