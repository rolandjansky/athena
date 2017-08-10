/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERHANDLEL1_H
#define VP1TRIGGERHANDLEL1_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerHandleL1
 *
 * @brief   : L1 Trigger Data Handle (Header)
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 * @author  : Edward Moyse    <edward.moyse@cern.ch>
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//L1 includes

// check the include of the Chain.h file here below if it's still used or it's obsolete
//#include "TrigSteering/Chain.h" // OLD include
#include "TrigSteeringEvent/Chain.h" // NEW include - Requested by Francesca Pastore, 13 Sep 2013

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"

//Other includes
#include <QTreeWidgetItem>
#include <QString>
#include <QList>
#include <vector>

//Classes
class MuonFeatureDetails;
class FeatureContainer;

namespace VP1Trig {
  class VP1TriggerHandleL1 : public virtual Logger 
  {
  public:
    VP1TriggerHandleL1(Trig::FeatureContainer containerL1, QString trigID) 
      : Logger("TriggerHandleL1"),
        m_containerL1(containerL1),
        m_trigID(trigID),
        m_qtrigitem(0),
        m_trigData(0),
        m_qtrigstatus(false),
        m_vtrigstatus(false) {};
    
  private:
    //Granted Access
    friend class VP1TriggerProcessor;
    
    //L1 Handle data -------------------------------->
    //++ Note: Set by VP1TriggerSystem ++
    Trig::FeatureContainer m_containerL1;
    QString m_trigID;
    //-----------------------------------------------<
    
    //Handle item processing ------------------------>
    //**** Run by: VP1TriggerProcessor ****
    bool processQTrigItem();
    bool processVTrigItem();
    
    //**** Run by: Class Internal ****
    bool processitem();
    bool processFeature(int processId, QString roiId, QString feature);

    //**** Data: QTree format ****
    QTreeWidgetItem* m_qtrigitem;
    
    //**** Data: Vector format ****
    std::vector<std::vector<QString> > m_trigData;
    
    //**** Data: Status flags ****
    bool m_qtrigstatus;
    bool m_vtrigstatus;
    //-----------------------------------------------<
  };
}
#endif
