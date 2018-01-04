/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_SCT_H
#define PRDCOLLHANDLE_SCT_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDCommonFlags.h"

class PRDCollHandle_SCT : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_SCT(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_SCT();

public slots:
  void setPartsFlags(PRDCommonFlags::InDetPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC
  void setExcludeIsolatedClusters(bool);

protected:
  virtual PRDHandleBase * addPRD(const Trk::PrepRawData*) override;
  bool cut(PRDHandleBase*);

  void eraseEventDataSpecific();
  void postLoadInitialisation();
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 800.0f*800.0f; }
  QColor defaultColor() const;

private:

  class Imp;
  Imp * m_d;

};

#endif
