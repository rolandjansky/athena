/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1LINES_H
#define VP1LINES_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoSeparator;

class VP1Lines : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1Lines( SoSeparator * attachsep,//where the grid separator will attach itself when visible
		    IVP1System * sys,QObject * parent = 0);
  virtual ~VP1Lines();

public slots:

  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setColourAndTransp(const SbColor4f&);
  void setDirection(const SbVec3f&);

private:
  class Imp;
  Imp * m_d;
};

#endif
