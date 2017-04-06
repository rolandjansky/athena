/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERHANDLEL2_H
#define VP1TRIGGERHANDLEL2_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerHandleL2
 *
 * @brief   : L2 Trigger Data Handle (Header)
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Other includes
#include <QTreeWidgetItem>
#include <QString>

//Classes
class MuonFeatureDetails;

namespace VP1Trig {
  class VP1TriggerHandleL2 : public virtual Logger 
  {
  public:
    VP1TriggerHandleL2(std::vector<const MuonFeatureDetails*> containerL2) 
      : Logger("TriggerHandleL2"),
        m_containerL2(containerL2),
        m_qtrigData(0),
        m_trigData(0),
        m_qtrigstatus(false),
        m_vtrigstatus(false) {};
    
  private:
    //Granted Access
    friend class VP1TriggerProcessor;
    
    //L2 Handle data -------------------------------->
    //++ Note: Set by VP1TriggerSystem ++
    std::vector<const MuonFeatureDetails*> m_containerL2;
    //-----------------------------------------------<
    
    //Handle item processing ------------------------>
    //**** Run by: VP1TriggerProcessor ****
    bool processQTrigItem();
    bool processVTrigItem();
    
    //**** Run by: Class Internal ****
    bool processitem();

    //**** Data: QTree format ****
    QTreeWidgetItem* m_qtrigData;
    
    //**** Data: Vector format ****
    std::vector<std::vector<QString> > m_trigData;
    
    //**** Data: Status flags ****
    bool m_qtrigstatus;
    bool m_vtrigstatus;
    //-----------------------------------------------<    
  };
}
#endif
