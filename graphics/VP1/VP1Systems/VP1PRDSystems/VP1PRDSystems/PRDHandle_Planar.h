/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_PLANAR_H
#define PRDHANDLE_PLANAR_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "ISF_FatrasEvent/PlanarCluster.h"

class PRDCollHandle_Planar;
class SoGroup;

class PRDHandle_Planar : public PRDHandleBase {
public:

  PRDHandle_Planar(PRDCollHandle_Planar*, const iFatras::PlanarCluster*);
  virtual ~PRDHandle_Planar() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const iFatras::PlanarCluster * cluster() const { return m_cluster; }
  const Trk::PrepRawData * getPRD() const { return m_cluster; }
  bool isBarrel() const { return m_cluster->detectorElement()->isBarrel(); }
  bool isPositiveZ() const { return m_cluster->detectorElement()->center().z() > 0.0; }
  bool isSane() const; //!< returns false if anything seems weird about the Planarcluster

private:
  const iFatras::PlanarCluster* m_cluster;

  SoGroup* createErrorAtPlanarCluster( const double& sigmaScale,
				      const int& numPoints ) const;
	

};

#endif // PRDHANDLE_PLANAR_H
