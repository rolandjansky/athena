/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MissingEtCollWidget              //
//                                                            //
//  Description: Collection widget. Specialisation which      //
//               imposes desired section ordering.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MISSINGETCOLLWIDGET_H
#define VP1MISSINGETCOLLWIDGET_H

#include "VP1Base/VP1CollectionWidget.h"

class VP1MissingEtCollWidget : public VP1CollectionWidget {
public:

  VP1MissingEtCollWidget(QWidget *parent = 0);// : VP1CollectionWidget(parent) {}
  virtual ~VP1MissingEtCollWidget();//{}

protected:

  void sortSections(QList<QString>& sections);/* {
    //Order: standard -> truth -> ... -> calo
    ensureFirst("*truth*",sections);
    ensureFirst("*standard*",sections);
    ensureLast("*calo*",sections);
  }*/

private slots:
 void setShape(bool);

};


#endif
