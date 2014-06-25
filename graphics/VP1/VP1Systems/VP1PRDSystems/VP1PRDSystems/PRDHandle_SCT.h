/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_SCT_H
#define PRDHANDLE_SCT_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "InDetPrepRawData/SCT_Cluster.h"

class PRDCollHandle_SCT;

class PRDHandle_SCT : public PRDHandleBase {
public:

  PRDHandle_SCT(PRDCollHandle_SCT*,const InDet::SCT_Cluster*);
  virtual ~PRDHandle_SCT() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const InDet::SCT_Cluster * cluster() const { return m_cluster; }
  const Trk::PrepRawData * getPRD() const { return m_cluster; }
  bool isBarrel() const { return m_cluster->detectorElement()->isBarrel(); }
  bool isPositiveZ() const { return m_cluster->detectorElement()->center().z() > 0.0; }

private:
  const InDet::SCT_Cluster* m_cluster;

};

#endif
