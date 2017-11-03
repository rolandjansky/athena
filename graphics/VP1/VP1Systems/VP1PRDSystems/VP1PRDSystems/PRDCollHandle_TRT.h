/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_TRT_H
#define PRDCOLLHANDLE_TRT_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDCommonFlags.h"
#include "VP1Utils/InDetProjFlags.h"

class PRDCollHandle_TRT : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_TRT(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_TRT();

  bool highLightHighThreshold() { return m_highlightHT; }
  bool project() const { return m_project; }
  InDetProjFlags::InDetProjPartsFlags appropriateProjections() const { return m_appropriateProjections; }

public slots:
  void setHighLightHighThresholds(bool);
  void setPartsFlags(PRDCommonFlags::InDetPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC
  void setMinToT(unsigned);
  void setMaxToT(unsigned);
  void setMinLE(unsigned);
  void setMaxLE(unsigned);
  void setRequireHT(bool);
  void setProject(bool);
  void setAppropriateProjection(InDetProjFlags::InDetProjPartsFlags);

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);
  bool cut(PRDHandleBase*);

  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 700.0f*700.0f; }
  QColor defaultColor() const;

private:

  class Imp;
  Imp * m_d;

  //Here for inlines:
  bool m_highlightHT;
  bool m_project;
  InDetProjFlags::InDetProjPartsFlags m_appropriateProjections;


};

#endif
