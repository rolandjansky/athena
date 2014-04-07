/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollWidget                     //
//                                                            //
//  Description: Collection widget. Specialisation which      //
//               imposes desired section ordering.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLWIDGET_H
#define TRACKCOLLWIDGET_H

#include "VP1Base/VP1CollectionWidget.h"

class TrackCollWidget : public VP1CollectionWidget {

  Q_OBJECT

public:

  TrackCollWidget(QWidget * parent = 0) : VP1CollectionWidget(parent) {}
  virtual ~TrackCollWidget(){}

protected:

  void sortSections(QList<QString>&);

private:
  class Imp;

};


#endif
