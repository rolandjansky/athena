/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CartesianGrid                    //
//                                                            //
//  Description: Helper class providing a cartesian grid.     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CARTESIANGRID_H
#define VP1CARTESIANGRID_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoSeparator;

class VP1CartesianGrid : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1CartesianGrid( SoSeparator * attachsep,//where the grid separator will attach itself when visible
		    IVP1System * sys,QObject * parent = 0);
  virtual ~VP1CartesianGrid();

public slots:

  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setColourAndTransp(const SbColor4f&);
  void setExtent(const double&);//A negative value means that negative parts will be shown as well
  void setSpacing(const double&);

private:
  class Imp;
  Imp * m_d;

};

#endif
