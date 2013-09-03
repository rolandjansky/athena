/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EvtVetoTool_H
#define EvtVetoTool_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TriggerRateTools/IEvtVetoTool.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

#include <vector>
#include <string> 
#include <map>

class MsgStream;

static const InterfaceID IID_EvtVetoTool("EvtVetoTool", 1, 0);

class EvtVetoTool : public AlgTool, public virtual IEvtVetoTool {

    public:

        EvtVetoTool(const std::string & type, const std::string & name, const IInterface* parent); 
        ~EvtVetoTool();

        static const InterfaceID& interfaceID( ) { return IID_EvtVetoTool; };

        StatusCode initialize();
        StatusCode finalize();
        bool isPassed();

    //private:
        

    private:
 
        MsgStream* m_log;
        ServiceHandle<StoreGateSvc> m_storeGate;
        std::string m_truthJetsContainerName;
        std::vector<const Jet*> m_truthJets;
        double m_vetoThreshold;
};

#endif
