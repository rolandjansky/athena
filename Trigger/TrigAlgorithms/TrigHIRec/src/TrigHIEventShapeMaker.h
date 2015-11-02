/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigHIClusterMaker.h
PACKAGE:  Trigger/TrigAlgorithms/TrigHIRec

AUTHOR:  martin.spousta@cern.ch
         This is an HLT::Fex analogue of Reconstruction/HeavyIonRec/HIGlobal/HIEventShapeMaker.h

********************************************************************/

#ifndef TrigHLTJetRec_TrigHIEventShapeMaker_h
#define TrigHLTJetRec_TrigHIEventShapeMaker_h

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/ToolHandle.h"
#include "HIGlobal/IHIEventShapeFiller.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>


//class StoreGateSvc;

class TrigHIEventShapeMaker : public HLT::FexAlgo
{
public:
  TrigHIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHIEventShapeMaker();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize(); 
  
private:
  bool        m_use_calo_cell      ;//use cells instead of towers to fill the EventShape
  std::string m_HIEventShapeContainerKey;
  std::string m_cell_container_key;
  std::string m_tower_container_key;
  ToolHandle<IHIEventShapeFiller> m_fillerTool;
};


#endif // TrigHIHypo_TrigHIEventShapeMaker_h
