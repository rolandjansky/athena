/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1EvtsOnServerInfo                 //
//                                                            //
//  Description: Class for parsing the fileinfo.txt file      //
//               from the http directory where event files    //
//               from point 1 are available                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1EVTSONSERVERINFO_H
#define VP1EVTSONSERVERINFO_H

#include <QString>
#include <QStringList>
#include "VP1Gui/VP1EventFile.h"

class VP1EvtsOnServerInfo {
public:

  VP1EvtsOnServerInfo(const QString& infofile);//the fileinfo.txt file from the server
  ~VP1EvtsOnServerInfo();

  bool isValid() const;//Check if infofile parsed succesfully.
  QString error() const;//When !isValid, this gives an explanation.

  QStringList print() const;

  //Access the results:
  unsigned long long numberOfEvents() const;
  VP1EventFile newestEvent() const;
  QList<VP1EventFile> events(int timecut = -1, bool requireNewestRunNumber = false ) const;
  //Returns events in order of increasing time.
  //
  //  * If timecut>0, only events within timecut seconds of newest
  //    event will be in list.
  //  * If requireNewestRunNumber is set, only events with same run
  //    number as the newest event will be in the list.

private:

  VP1EvtsOnServerInfo( const VP1EvtsOnServerInfo & );
  VP1EvtsOnServerInfo & operator= ( const VP1EvtsOnServerInfo & );

  class Imp;
  Imp * d;

};

#endif
