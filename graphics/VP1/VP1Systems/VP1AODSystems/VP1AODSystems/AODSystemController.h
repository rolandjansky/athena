/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AODSystemController                 //
//                                                            //
//  Description: Controller widget for the AOD system         //
//               Keeps all nasty gui stuff internally, and    //
//               only presents the actual interesting data    //
//               with specialised access methods and signals. //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEMS_AODSYSTEMCONTROLLER_H
#define VP1AODSYSTEMS_AODSYSTEMCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include <QFlags>
#include "GaudiKernel/ServiceHandle.h"

class SoMaterial;
class SoDrawStyle;
class SoComplexity;
class AODSysCommonData;
class QTreeWidget;
class QTreeWidgetItem;

namespace Trk {
  class IExtrapolator;
  class ITrackFitter;
  class Track;
  class Volume;
}

class AODCollWidget;

class AODSystemController : public VP1Controller {

  Q_OBJECT

public:

  AODSystemController(IVP1System * sys);
  virtual ~AODSystemController();

  void actualRestoreSettings(VP1Deserialise&);
  int  currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  /// Return widget which fills the collection selection list in the GUI (i.e. which finds and lists the available collections).
  AODCollWidget * collWidget() const;
  bool cutOnlyVertexAssocTracks() const;

  //Interactions:
  bool showTotMomentumOnMultiTrackSelection() const;
  //No associated signals for the next four (only queried on-click):
  bool orientAndZoomOnSingleSelection() const;
  bool printInfoOnSingleSelection() const;
  bool printVerboseInfoOnSingleSelection() const;
  bool printTotMomentumOnMultiTrackSelection() const;
  
  QTreeWidget* objBrowser() const; //!< Returns a pointer to the Track Object Browser (if it exists)
  AODSysCommonData * common() const; //!< Returns a pointer to the common data (if it exists)
  void setCommonData(AODSysCommonData *) ; //!< set pointer to the common data
  
  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:

  
private:

  class Imp;
  Imp * d;

private slots:
  void objectBrowserClicked(QTreeWidgetItem * item, int);
};

#endif
