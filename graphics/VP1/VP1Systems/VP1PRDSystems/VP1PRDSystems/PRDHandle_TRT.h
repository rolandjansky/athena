/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_TRT_H
#define PRDHANDLE_TRT_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_TRT.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

class PRDHandle_TRT : public PRDHandleBase {
public:

  PRDHandle_TRT(PRDCollHandle_TRT*,const InDet::TRT_DriftCircle*);
  virtual ~PRDHandle_TRT() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();
  inline bool highLight() const;

  bool transformUsesSurfacePositionOnly() const { return true; }

  const InDet::TRT_DriftCircle * driftCircle() const { return m_driftcircle; }
  const Trk::PrepRawData * getPRD() const { return m_driftcircle; }

  bool isBarrel() const { return m_driftcircle->detectorElement()->type()==InDetDD::TRT_BaseElement::BARREL; }
  bool isPositiveZ() const { return m_driftcircle->detectorElement()->center().z() > 0.0; }
  bool highLevel() const { return m_driftcircle->highLevel(); }

  virtual QStringList clicked() const;


private:
  const InDet::TRT_DriftCircle* m_driftcircle;

};

///////////////
//  INLINES  //
///////////////

inline bool PRDHandle_TRT::highLight() const
{
  return highLevel() && static_cast<PRDCollHandle_TRT*>(collHandle())->highLightHighThreshold();
}


#endif
