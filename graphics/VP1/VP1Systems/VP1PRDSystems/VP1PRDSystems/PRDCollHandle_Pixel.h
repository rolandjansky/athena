/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_PIXEL_H
#define PRDCOLLHANDLE_PIXEL_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDCommonFlags.h"

class PRDCollHandle_Pixel : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_Pixel(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_Pixel();

public slots:
  void setPartsFlags(PRDCommonFlags::InDetPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC
  void setMinNRDOPerCluster(unsigned);

protected:
  virtual PRDHandleBase * addPRD(const Trk::PrepRawData*) override;
  bool cut(PRDHandleBase*);

  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 800.0f*800.0f; }
  QColor defaultColor() const;
  virtual DETAIL defaultDetailLevel() const override { return AUTO; }
  //  virtual DETAIL defaultDetailLevel() const { return SIMPLE; }

private:

  class Imp;
  Imp * m_d;

};

#endif
