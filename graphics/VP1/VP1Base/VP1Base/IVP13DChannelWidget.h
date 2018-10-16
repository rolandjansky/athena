/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP13DChannelWidget              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP13DCHANNELWIDGET_H
#define IVP13DCHANNELWIDGET_H

#include "VP1Base/IVP1ChannelWidget.h"

// fwd declarations
//class SoQtRenderArea;
class VP1ExaminerViewer;
class QPixmap;

class IVP13DChannelWidget : public IVP1ChannelWidget {

protected:

  //All derived classes must register their render areas with this method:
  //Their render areas will then be deleted automatically when needed (amongst other things).
//  void registerRenderArea(SoQtRenderArea*);
  void registerRenderArea(VP1ExaminerViewer*);

public:

  IVP13DChannelWidget(const QString & name, const QString & information, const QString & contact_info);

  virtual ~IVP13DChannelWidget();

  Type type() const { return THREEDIMENSIONAL; };

  //We override this so we can also take care of the render areas.
  void setUpdatesEnabled ( bool enable );

  virtual QPixmap getSnapshot(bool transp = false, int width = 0, bool batch = false);

  virtual QByteArray saveState();
  virtual void restoreFromState(QByteArray);

  virtual void goingToNextEvent();//We abort all current zooms here.


private:
  IVP13DChannelWidget( const IVP13DChannelWidget & );
  IVP13DChannelWidget & operator= ( const IVP13DChannelWidget & );
  class Imp;
  Imp * m_d;

};


#endif
