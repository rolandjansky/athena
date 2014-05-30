/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1AvailEvtsHttp                    //
//                                                            //
//  Description: Implementation of VP1AvailEvents which       //
//               monitors and retrieves events from a web     //
//               directory (mainly from point1)               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AVAILEVTSHTTP_H
#define VP1AVAILEVTSHTTP_H

#include "VP1Gui/VP1AvailEvents.h"

class VP1AvailEvtsHttp : public VP1AvailEvents {

  Q_OBJECT

public:

  VP1AvailEvtsHttp( QString fileinfoUrl,
		    int updateInterval,
		    int timeCutForNew,
		    QString tmpcopydir,
		    int maxLocalFilesToKeep = -1,
		    QObject * parent = 0 );
  virtual ~VP1AvailEvtsHttp();

  virtual void init();

private slots:
  void fileInfoChanged();
  void fileInfoDownloadSuccessful( const QString& urltofile, const QString& localtargetfile );
  void fileInfoDownloadFailed( const QString& error, const QString& urltofile,const QString& localtargetfile );
  void eventFileDownloadSuccessful( const QString& urltofile, const QString& localtargetfile, const QString& data );
  void eventFileDownloadFailed( const QString& error, const QString& urltofile,const QString& localtargetfile );
  void examineEvtsOnServer();

private:

  class Imp;
  Imp * d;

};

#endif
