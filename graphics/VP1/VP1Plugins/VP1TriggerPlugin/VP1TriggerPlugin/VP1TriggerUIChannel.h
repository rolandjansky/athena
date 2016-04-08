/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERUICHANNEL_H
#define VP1TRIGGERUICHANNEL_H

/***********************************************************************************
 * @Package : VP1TriggerPlugins
 * @Class   : VP1TriggerUIChannel
 *
 * @brief   : Trigger UI to query trigger data utilizing the VP1TriggerTool (Header)
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Base include
#include "VP1Base/IVP1ChannelWidget.h"

//GUI includes
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTreeWidgetItemIterator>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

//Other includes
#include <QList>

//Classes
class VP1TriggerUISystem;
namespace VP1Trig {
  class VP1TriggerTool;
}

class VP1TriggerUIChannel : public IVP1ChannelWidget, public virtual VP1Trig::Logger
{
  Q_OBJECT
    
public:
  VP1TriggerUIChannel();
  virtual ~VP1TriggerUIChannel();
  void init();
  Type type() const { return NUMBERS; };
  
  //System calls:
  void create();
  void systemRefreshed(IVP1System*);
  void systemErased(IVP1System*);
  
protected slots:
  void filterDisplayUpdate(QString);
  void setTrigLvl(QString);
  void toggleDisplay();
  
private:
  //VP1Trig::VP1TriggerTool * m_triggertool;
  VP1TriggerUISystem * sys;
  
  //GUI components
  QTreeWidget  * m_treeWidget;
  QTextBrowser * m_filterDisplay;
  QLineEdit    * m_searchBar;
  QComboBox    * m_comboBox;
  QPushButton  * m_switchButton;
  QLabel       * m_browserLabel;
  
  //Processing methods
  void loadTrigData(QString);
  bool loadDataPattern();
  void searchData(QString);
  void printToScreen(QString);
      
  //Formated trigger data for search purposes
  QList<QString> trigNode;
  std::vector<std::vector<QString> > trigData;
};
#endif
