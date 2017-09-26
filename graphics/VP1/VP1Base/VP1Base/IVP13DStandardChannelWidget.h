/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP13DStandardChannelWidget      //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP13DSTANDARDCHANNELWIDGET_H
#define IVP13DSTANDARDCHANNELWIDGET_H

#include "VP1Base/IVP13DChannelWidget.h"
class IVP13DSystem;
class IVP13DSystemSimple;
class QResizeEvent;
class QPaintEvent;

class IVP13DStandardChannelWidget : public IVP13DChannelWidget {

  Q_OBJECT

public:
  IVP13DStandardChannelWidget(const QString & name, const QString & information, const QString & contact_info, bool detectorViewButtons = true);
  //Fixme: Pass along default backgroundcolor as an option?

protected:
  enum SystemOption {
    AllowAll = 0x0000,
    DisallowExtraDisplayWidget = 0x0002,//Any extra display widget provided by this wont be present in the GUI
    DisallowSwitchable=0x0004,//User can not turn 3d objects from this system on/off.
    DisallowController = 0x0008,//Any custom controller provided by this wont be present in the GUI
    StartDisabled = 0x0010,
    DisallowCameraAccess = 0x0060
  };
public:  //public instead of protected due to compilation problems on SLC3
  Q_DECLARE_FLAGS(SystemOptions, SystemOption)
protected:

  //Call the following in constructor of a derived class. Do not delete the systems in the derived class.
  void addSystem(IVP13DSystem*, const SystemOptions& options = AllowAll );

  //////////////////// The following is for internal use ///////////////////////

  //Convenience so users dont need to cast themselves (needed due to private inheritance in IVP13DSystemSimple):
  void addSystem(IVP13DSystemSimple*, const SystemOptions& options = AllowAll );

public:
  void create();
  virtual ~IVP13DStandardChannelWidget();
  virtual QPixmap getSnapshot(bool transp = false, int width = 0, bool batch = false);
  //If overriding the following, remember to call this implementation:
  virtual void lastOfActiveSystemsRefreshed();
  virtual void systemRefreshed(IVP1System*);
  virtual void systemErased(IVP1System*);
  virtual QByteArray saveState();
  virtual void restoreFromState(QByteArray);
  void launchStereoEditor();
  bool isAntiAliasing();
  bool setAntiAliasing(bool);

private:
  IVP13DStandardChannelWidget( const IVP13DStandardChannelWidget & );
  IVP13DStandardChannelWidget & operator= ( const IVP13DStandardChannelWidget & );
  class Imp;
  Imp * m_d;

private slots:
  void toggleSystemActive();
  void updateSnapshotDim();
  void setImageFromPresets();

protected slots:
  void showControlsForSystem();
protected:
  void dockResized();
  void stopSpinning();

};

Q_DECLARE_OPERATORS_FOR_FLAGS(IVP13DStandardChannelWidget::SystemOptions)

#endif
