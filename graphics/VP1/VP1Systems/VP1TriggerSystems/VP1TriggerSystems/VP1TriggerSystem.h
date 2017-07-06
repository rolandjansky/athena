/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERSYSTEM_H
#define VP1TRIGGERSYSTEM_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerSystem
 *
 * @brief   : Trigger Data System (Header) - access and store trigger data
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

#ifndef Q_MOC_RUN
//Local includes
#include "VP1TriggerSystems/VP1TriggerHandleEF.h"
#include "VP1TriggerSystems/VP1TriggerHandleL2.h"
#include "VP1TriggerSystems/VP1TriggerHandleL1.h"
#include "VP1TriggerSystems/VP1TriggerProcessor.h"
#include "VP1TriggerSystems/Logger.h"

//Trigger includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#endif

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ObjectVector.h"

//Other includes
#include "StoreGate/StoreGateSvc.h"
#include <QStringList>

namespace VP1Trig {
  class VP1TriggerSystem : public virtual Logger
  {
  public:
    VP1TriggerSystem();
    virtual ~VP1TriggerSystem();
    
  private:
    //Granted Access
    friend class VP1TriggerProcessor;
    
    //Mains
    bool loadTriggerHandles(StoreGateSvc* m_storeGate, QString triglvl);
    void clearTriggerHandles();
    
    //Tool Handles
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;
    ToolHandle<TrigMatchTool> m_trigMatch;
    
    //Trigger data vectors
    std::vector<VP1Trig::VP1TriggerHandleEF> m_handleEF;
    std::vector<VP1Trig::VP1TriggerHandleL2> m_handleL2;
    std::vector<VP1Trig::VP1TriggerHandleL1> m_handleL1;
    
    //Trigger data access handles (granted access only)
    std::vector<VP1Trig::VP1TriggerHandleEF> getHandleEF() { return m_handleEF; };
    std::vector<VP1Trig::VP1TriggerHandleL2> getHandleL2() { return m_handleL2; };
    std::vector<VP1Trig::VP1TriggerHandleL1> getHandleL1() { return m_handleL1; };
  };
}
#endif
