/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual bool cut(PRDHandleBase*) override;

  virtual void setupSettingsFromControllerSpecific(PRDSystemController*) override;
  virtual float lodArea() const override { return 800.0f*800.0f; }
  virtual QColor defaultColor() const override;
  virtual DETAIL defaultDetailLevel() const override { return AUTO; }
  //  virtual DETAIL defaultDetailLevel() const { return SIMPLE; }

private:

  class Imp;
  Imp * m_d;

};

#endif
