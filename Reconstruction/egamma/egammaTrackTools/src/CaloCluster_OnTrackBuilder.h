/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTER_ONTRACKBUILER_H
#define CALOCLUSTER_ONTRACKBUILER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"

#include "StoreGate/ReadHandleKey.h"

namespace Trk {
class CaloCluster_OnTrack;
class Surface;
class LocalParameters;
}

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaFwd.h"

class CaloDetDescrManager;

class CaloCluster_OnTrackBuilder
  : public AthAlgTool
  , virtual public ICaloCluster_OnTrackBuilder
{
public:
  /** Constructor with AlgTool parameters */
  CaloCluster_OnTrackBuilder(const std::string&,
                             const std::string&,
                             const IInterface*);

  CaloCluster_OnTrackBuilder();
  ~CaloCluster_OnTrackBuilder();

  // standard Athena methods
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual Trk::CaloCluster_OnTrack* buildClusterOnTrack(
    const EventContext& ctx,
    const xAOD::CaloCluster* cl,
    int charge = 0) const override final;

private:
  std::unique_ptr<Trk::Surface> getCaloSurface(
    const xAOD::CaloCluster* cluster,
    const CaloDetDescrManager* caloDDMgr) const;

  Trk::LocalParameters getClusterLocalParameters(
    const xAOD::CaloCluster* cluster,
    const Trk::Surface* surf,
    int charge) const;

  Amg::MatrixX getClusterErrorMatrix(const xAOD::CaloCluster* cluster,
                                     const Trk::Surface& surf,
                                     int charge) const;

  /** @brief Tool to build calorimeter layer surfaces */
  ToolHandle<ICaloSurfaceBuilder> m_calosurf{
    this,
    "CaloSurfaceBuilder",
    "CaloSurfaceBuilder",
    "Tool to build calorimeter layer surfaces"
  };
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager"
  };

  Gaudi::Property<bool> m_useClusterEnergy{ this, "UseClusterEnergy", true };
  Gaudi::Property<bool> m_useClusterPhi{ this, "UseClusterPhi", true };
  Gaudi::Property<bool> m_useClusterEta{ this, "UseClusterEta", true };

  /** @brief (deta,dphi) granularity*/
  const LArEM_ID* m_emid;
};

#endif // CALOCLUSTER_ONTRACKBUILER_H
