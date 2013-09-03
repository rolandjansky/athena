/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SlicesInfoTool_H
#define SlicesInfoTool_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TriggerRateTools/ISlicesInfoTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "InDetBCM_RawData/BCM_RawData.h"

#include "TriggerRateSlices/TrigEFMissingEtObject.h"
#include "TriggerRateSlices/TrigL2MissingEtObject.h"
#include "TriggerRateSlices/TrigL1MissingEtObject.h"
#include "TriggerRateSlices/TrigMissingEtObject.h"
#include "TriggerRateSlices/TrigBCMObject.h"

#include "TROOT.h"
#include <TSystem.h>
#include "GaudiKernel/NTuple.h"
#include "TBranch.h"
#include "TTree.h"
#include "TFile.h" 
#include "TH1.h"
#include "TH2.h"

namespace TrigConf {
    class ILVL1ConfigSvc;
}

#include <vector>
#include <string> 
#include <map>

class MsgStream;

static const InterfaceID IID_SlicesInfoTool("SlicesInfoTool", 1, 0);

class SlicesInfoTool : public AlgTool, public virtual ISlicesInfoTool {

    public:

        SlicesInfoTool(const std::string & type, const std::string & name, const IInterface* parent); 
        ~SlicesInfoTool();

        static const InterfaceID& interfaceID( ) { return IID_SlicesInfoTool; };

        StatusCode initialize();
        StatusCode finalize();
        
        StatusCode bookSlices();
        StatusCode fillSlices();
        
        StatusCode bookMet();
        StatusCode fillMet();
        
        StatusCode bookBCM();
        StatusCode fillBCM();
        
            
    private:
 
        MsgStream* m_log;
        ITHistSvc* m_histSvc;
        ServiceHandle<StoreGateSvc> m_storeGate;
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;
        ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

        Int_t evt_num;
        TTree* tree;
        
        bool m_doMet;
        bool m_doBCM;

        // BCM
        
        TrigBCMObject* bcm;
        
        std::string m_bcmRdoContainerName;

        // MET
        
        TrigL1MissingEtObject* etMissL1;
        TrigL2MissingEtObject* etMissL2;
        TrigEFMissingEtObject* etMissEF;
};

#endif
