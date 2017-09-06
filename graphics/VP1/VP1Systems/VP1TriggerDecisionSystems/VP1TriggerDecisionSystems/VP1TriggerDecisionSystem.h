/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERDECISIONSYSTEM_H
#define VP1TRIGGERDECISIONSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1TriggerDecisionSystem                             //
//  Weiyi Zhang <wyizhang@cern.ch>                                     //
//  June 18 2007                                      		     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
#include "VP1Base/IVP1System.h"
#include <QStringList>

class VP1TriggerDecisionSystem : public IVP1System 
{
  	Q_OBJECT

public:
  	VP1TriggerDecisionSystem();
  	virtual ~VP1TriggerDecisionSystem();

  	void create( StoreGateSvc* detstore );
  	void refresh( StoreGateSvc* storegate );
  	void erase();

  	QStringList getInfoForEntry(QString key,QString type);
	void getInfo(const std::string& key,const std::string&,std::ostringstream& os);

signals:
  	void entriesChanged(const QStringList& entry_key,const QStringList& entry_type);

	//  protected slots:
	//  void checkboxChanged();

private:

 	class Clockwork;
   	Clockwork* m_clock;
};

#endif
