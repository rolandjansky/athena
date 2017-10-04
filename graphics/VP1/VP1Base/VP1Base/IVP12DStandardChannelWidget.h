/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DStandardChannelWidget      //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DSTANDARDCHANNELWIDGET_H
#define IVP12DSTANDARDCHANNELWIDGET_H

#include "VP1Base/IVP1ChannelWidget.h"
class IVP12DSystem;
class IVP12DSystemSimple;

class IVP12DStandardChannelWidget : public IVP1ChannelWidget {

  Q_OBJECT

public:
  IVP12DStandardChannelWidget(const QString & name, const QString & information, const QString & contact_info);
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
  //Call the following in constructor of a derived class. Do not delete the systems in the derived class.
  void addSystem(IVP12DSystem*, const SystemOptions& options = AllowAll );

  //////////////////// The following is for internal use ///////////////////////

  //Convenience so users dont need to cast themselves (needed due to private inheritance in IVP12DSystemSimple):
  void addSystem(IVP12DSystemSimple*, const SystemOptions& options = AllowAll );
public:
  Type type() const { return TWODIMENSIONAL; };

  void create();
  void systemRefreshed(IVP1System*);
  void systemErased(IVP1System*);
  virtual ~IVP12DStandardChannelWidget();
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

Q_DECLARE_OPERATORS_FOR_FLAGS(IVP12DStandardChannelWidget::SystemOptions)

#endif
