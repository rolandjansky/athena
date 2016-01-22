/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AODCollWidget                     //
//                                                            //
//  Description: Collection widget. Specialisation which      //
//               imposes desired section ordering.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef AODCOLLWIDGET_H
#define AODCOLLWIDGET_H

#include "VP1Base/VP1CollectionWidget.h"

class AODCollWidget : public VP1CollectionWidget {

  Q_OBJECT

public:

  AODCollWidget(QWidget * parent = 0) : VP1CollectionWidget(parent) {}
  virtual ~AODCollWidget(){}

protected:

  void sortSections(QList<QString>&);

private:
  class Imp;

};


#endif
