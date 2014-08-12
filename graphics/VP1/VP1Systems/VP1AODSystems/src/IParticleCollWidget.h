/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleCollWidget                     //
//                                                            //
//  Description: Collection widget. Specialisation which      //
//               imposes desired section ordering.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLECOLLWIDGET_H
#define IPARTICLECOLLWIDGET_H

#include "VP1Base/VP1CollectionWidget.h"

class IParticleCollWidget : public VP1CollectionWidget {

  Q_OBJECT

public:

  IParticleCollWidget(QWidget * parent = 0) : VP1CollectionWidget(parent) {}
  virtual ~IParticleCollWidget(){}

protected:

  void sortSections(QList<QString>&);

private:
  class Imp;

};


#endif
