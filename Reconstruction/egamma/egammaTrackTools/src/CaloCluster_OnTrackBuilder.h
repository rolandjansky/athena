/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTER_ONTRACKBUILER_H
#define CALOCLUSTER_ONTRACKBUILER_H

#include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "EventPrimitives/EventPrimitives.h"

#include "StoreGate/ReadHandleKey.h"

namespace Trk{
  class CaloCluster_OnTrack;
  class Surface;
  class LocalParameters;
}

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

class CaloDetDescrManager;

class CaloCluster_OnTrackBuilder : public AthAlgTool, virtual public ICaloCluster_OnTrackBuilder
{
 public:
  /** Constructor with AlgTool parameters */
  CaloCluster_OnTrackBuilder(const std::string&, const std::string&, const IInterface*);

  CaloCluster_OnTrackBuilder();
  ~CaloCluster_OnTrackBuilder();

  // standard Athena methods
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual Trk::CaloCluster_OnTrack*
  buildClusterOnTrack(const xAOD::CaloCluster* cl,
                      int charge = 0) const override;

private:

  const Trk::Surface*           getCaloSurface( const xAOD::CaloCluster* cluster ) const ;
  const Trk::LocalParameters*   getClusterLocalParameters( const xAOD::CaloCluster* cluster,
                                                           const Trk::Surface* surf,
                                                           int   charge) const;

  const Amg::MatrixX*      getClusterErrorMatrix( const xAOD::CaloCluster* cluster,
                                                  const Trk::Surface* surf,
                                                  int charge) const;

  static double getClusterPhiError( const xAOD::CaloCluster* cluster ) ;

  /** @brief Tool to build calorimeter layer surfaces */
  ToolHandle<ICaloSurfaceBuilder>  m_calosurf {this,
      "CaloSurfaceBuilder", "CaloSurfaceBuilder", "Tool to build calorimeter layer surfaces"};

  Gaudi::Property<bool> m_useClusterEnergy{this, "UseClusterEnergy", true};
  Gaudi::Property<bool> m_useClusterPhi{this, "UseClusterPhi", true};
  Gaudi::Property<bool> m_useClusterEta{this, "UseClusterEta", true};

  /** @brief (deta,dphi) granularity*/
  const LArEM_ID* m_emid;

};

#endif //CALOCLUSTER_ONTRACKBUILER_H
