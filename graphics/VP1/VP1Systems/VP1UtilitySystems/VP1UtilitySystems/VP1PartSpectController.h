/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PARTSPECTCONTROLLER_H
#define VP1PARTSPECTCONTROLLER_H
 
/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1PartSpectController                       //
//                                                                     //
//                                                                     //
//  Initial version: September 2009                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Controller.h"
#include "VP1UtilitySystems/VP1PartSpectFlags.h"

class VP1PartSpectController : public VP1Controller
{
  Q_OBJECT

 public:
  VP1PartSpectController(IVP1System* sys);
  virtual ~VP1PartSpectController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  VP1PartSpect::ParticleType getParticleType();

 signals:
  void fileUpdated(const QString&);
  void particleTypeChanged(VP1PartSpect::ParticleType);

 private:
  class Imp;
  Imp * d;

  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); } //unhide base methods

 private slots:
  void openFile();
  void particleType();
};

#endif
