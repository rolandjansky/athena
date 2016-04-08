/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1TriggerDecisionChannel        //
//  Weiyi Zhang  <wyizhang@cern.ch>  			   //
//  June 18 2007                             		   //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1TRIGDECCHANNEL_H
#define VP1TRIGDECCHANNEL_H

#include "VP1Base/IVP1ChannelWidget.h"

class QTableWidget;
class VP1TriggerDecisionSystem;

class VP1TriggerDecisionChannel : public IVP1ChannelWidget
{
  Q_OBJECT

public:
  VP1TriggerDecisionChannel();
  virtual ~VP1TriggerDecisionChannel(){}
  void init();

  Type type() const { return NUMBERS; };
  void create();
  void systemErased(IVP1System*);
  void systemRefreshed(IVP1System*);

protected slots:
void entriesChanged(const QStringList& entry_key,const QStringList& entry_type);
  void cellActivated( int row, int column );

private:
  QTableWidget * tableWidget;
  void clearAll();
  VP1TriggerDecisionSystem * trigdecsystem;
};

#endif
