/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERHANDLEEF_H
#define VP1TRIGGERHANDLEEF_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerHandleEF
 *
 * @brief   : EF Trigger Data Handle (Header)
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Other includes
#include <QTreeWidgetItem>
#include <QString>

//Classes
class TrigMuonEFInfo;
class TrigMuonEFTrack;
class TrigMuonEFCbTrack;

namespace VP1Trig {
  class VP1TriggerHandleEF : public virtual Logger 
  {
  public:
    VP1TriggerHandleEF(TrigMuonEFInfo* containerEF, QString trigID, QList<QString> chainIDs)
      : Logger("TriggerHandleEF"),
	m_containerEF(containerEF),
	m_trigID(trigID),
	m_chainIDs(chainIDs),
	m_qtrigData(0),
	m_trigData(0),
	m_qtrigstatus(false),
	m_vtrigstatus(false) {};
    
  private:
    //Granted Access
    friend class VP1TriggerProcessor;
    
    //EF Handle data -------------------------------->
    //++ Note: Set by VP1TriggerSystem ++
    TrigMuonEFInfo* m_containerEF;
    QString m_trigID;
    QList<QString> m_chainIDs;
    //-----------------------------------------------<
    
    //Handle item processing ------------------------>
    //**** Run by: VP1TriggerProcessor ****
    bool processQTrigItem();
    bool processVTrigItem();
    
    //**** Run by: Class Internal ****
    bool loadTrackContainer();
    void loadMuonTrack(int trackNo, QString type);
    double muonTrackData(QString type, int id);
    
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
