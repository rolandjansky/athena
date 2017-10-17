/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDCollHandle_SpacePoints           //
//                                                            //
//  Description: Collection handles for space points.         //
//               For historical reasons this inherits from    //
//               the PRD class.                               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDCOLLHANDLE_SPACEPOINTS_H
#define PRDCOLLHANDLE_SPACEPOINTS_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDCommonFlags.h"

class PRDCollHandle_SpacePoints : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_SpacePoints(PRDSysCommonData*,const QString& key);
  virtual ~PRDCollHandle_SpacePoints();

protected:
  PRDHandleBase* addPRD(Trk::PrepRawData*) { return 0; }
  bool load();
  bool cut(PRDHandleBase*);
  void eraseEventDataSpecific();
  void postLoadInitialisation();
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 800.0f*800.0f; }
  QColor defaultColor() const;

public slots:
  void setPartsFlags(PRDCommonFlags::InDetPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC

private:

  class Imp;
  Imp * m_d;

};

#endif
