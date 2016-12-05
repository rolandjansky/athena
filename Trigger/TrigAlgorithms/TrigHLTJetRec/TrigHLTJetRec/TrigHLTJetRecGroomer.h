/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********
//
// NAME:    TrigHLTJetRecGroomer.h
// PACKAGE: Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:  Steven Schramm
//
// Description: FexAlgo for building a jet container and trimming it
//
// **********


#ifndef TRIGHLTJETREC_TRIGHLTJETRECGROOMER_H
#define TRIGHLTJETREC_TRIGHLTJETRECGROOMER_H

#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"

class TrigHLTJetRecGroomer : public TrigHLTJetRecFromCluster
{
    public:
        TrigHLTJetRecGroomer(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~TrigHLTJetRecGroomer();

        virtual HLT::ErrorCode hltInitialize();

    protected:
        const xAOD::JetContainer* build() const override;
    
    private:
        // properties configurable via python
        ToolHandle<IJetBuildTool> m_jetTrimTool;

};


#endif
