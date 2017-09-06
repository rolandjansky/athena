/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Letters                          //
//                                                            //
//  Description: Helper class providing a letters.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1LETTERS_H
#define VP1LETTERS_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>

class SoMaterial;
class SoSeparator;

class VP1Letters : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1Letters( SoMaterial * mat,
	      SoSeparator * attachsep,//where the letters separator will attach itself when visible
	      IVP1System * sys,QObject * parent = 0);
  virtual ~VP1Letters();

public slots:
  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setZPos(const double&);
  void setVerticalPosition(const double&);

private:
  class Imp;
  Imp * d;

};

#endif
