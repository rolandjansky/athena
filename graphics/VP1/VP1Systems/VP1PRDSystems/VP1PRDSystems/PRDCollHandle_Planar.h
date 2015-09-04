/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_PLANAR_H
#define PRDCOLLHANDLE_PLANAR_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDCommonFlags.h"

class PRDCollHandle_Planar : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_Planar(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_Planar();

public slots:
  void setPartsFlags(PRDCommonFlags::InDetPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC
  void setMinNRDOPerCluster(unsigned);

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);
  bool cut(PRDHandleBase*);

  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 800.0f*800.0f; }
  QColor defaultColor() const;
  virtual DETAIL defaultDetailLevel() const { return AUTO; }

public:
  bool load();

private:

  class Imp;
  Imp * d;

};

#endif // PRDCOLLHANDLE_PLANAR_H
