/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DDetViewsChannelWidget      //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: July 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DDETVIEWSCHANNELWIDGET_H
#define IVP12DDETVIEWSCHANNELWIDGET_H

#include "VP1Base/IVP1ChannelWidget.h"


//This type of channel widget displays itemcollections in two
//different graphicsviews - one for each of XY and RZ
//projections.
//
//The normal way to add data from systems into these views is to add a
//number of IVP12DDetViewsSystem's to the channel (using the addSystem
//method) methods. Since IVP12DDetViewsSystem's specifically provide
//item collections for each of the different projections, everything
//should be taken care of automatically. It is, however, possible to
//add collections from any IVP12DSystem to the views. To do this, one
//must use the addGeneralSystem method and specify as a parameter in
//which of the two projections all of the item collections provided
//by this system should be presented.

//fixme/TODO: Actually use the z-factor!!

class IVP12DSystem;
class IVP12DDetViewsSystem;

class IVP12DDetViewsChannelWidget : public IVP1ChannelWidget {

  Q_OBJECT

public:
  IVP12DDetViewsChannelWidget(const QString & name, const QString & information, const QString & contact_info);
  //Fixme: Pass along default backgroundcolor as an option?

protected:
  enum SystemOption {
    AllowAll = 0x0000,
    DisallowSelections = 0x0100,//User can not interact with items from this system (movable though, is handled separately)
    DisallowSwitchable = 0x0400,//User can not turn items from this system on/off.
    DisallowController = 0x0800,//Custom controllers provided by this wont be present in the GUI
    DisallowMovable = 0x1000,//Items from this systems are never movable
    StartDisabled = 0x0010
  };
public:  //public instead of protected due to compilation problems on SLC3
  Q_DECLARE_FLAGS(SystemOptions, SystemOption)
protected:
  //Call the following in constructor of a derived class to add 2D systems. Never delete the systems in the derived class.

  //Zfactor determines which systems are on top of each other (higher values are in the foreground) //FIXME - NOT USED!!!

  void addSystem(IVP12DDetViewsSystem*, double zfactor, const SystemOptions& options = AllowAll );

  //Other types of systems can only be used by adding all of their collections into a single given projection view:
  enum PROJECTION { XY, RZ };
  void addGeneralSystem(IVP12DSystem*, const PROJECTION& projection, double zfactor, const SystemOptions& options = AllowAll );//FIXME: NOT IMPLEMENTED YET!!!!!!

  //////////////////// The following is for internal use ///////////////////////

public:
  Type type() const { return TWODIMENSIONAL; };

  void create();
  void systemRefreshed(IVP1System*);
  void systemErased(IVP1System*);
  virtual ~IVP12DDetViewsChannelWidget();
  virtual QByteArray saveState();
  virtual void restoreFromState(QByteArray);

protected slots:
  void showControlsForSystem();

private:
  class Imp;
  Imp * m_d;
private slots:
  void toggleSystemActive();
  void setBackgroundColor(const QColor &);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IVP12DDetViewsChannelWidget::SystemOptions)

#endif
