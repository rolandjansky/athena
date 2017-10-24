/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode initialize();
  virtual StatusCode finalize();  
  
  virtual Trk::CaloCluster_OnTrack* buildClusterOnTrack( const xAOD::Egamma* eg, int charge=0) ;
  virtual Trk::CaloCluster_OnTrack* buildClusterOnTrack( const xAOD::CaloCluster* cl, int charge=0) ;


 private:
 
  const Trk::Surface*           getCaloSurface( const xAOD::CaloCluster* cluster ) ;
  const Trk::LocalParameters*   getClusterLocalParameters( const xAOD::CaloCluster* cluster, 
                                                           const Trk::Surface* surf,
                                                           int   charge) const;
                                                           
  const Amg::MatrixX*      getClusterErrorMatrix( const xAOD::CaloCluster* cluster,
                                                  const Trk::Surface* surf,
                                                  int charge) const;
                                                      
  double getClusterPhiError( const xAOD::CaloCluster* cluster ) const ;
  double electronPhiResolution(double eta, double energy) const;
  double electronPhiResoA(double eta) const;
  double electronPhiResoB(double eta) const;

  bool   FindPosition(const xAOD::CaloCluster* cluster) const;
  double CalculatePhis(const xAOD::CaloCluster* cluster) const;


  /** @brief Tool to build calorimeter layer surfaces */
  ToolHandle<ICaloSurfaceBuilder>  m_calosurf {this, 
      "CaloSurfaceBuilder", "CaloSurfaceBuilder", "Tool to build calorimeter layer surfaces"};
  
  Gaudi::Property<bool> m_useClusterEnergy{this, "UseClusterEnergy", true};
  Gaudi::Property<bool> m_useClusterPhi{this, "UseClusterPhi", true};
  Gaudi::Property<bool> m_useClusterEta{this, "UseClusterEta", true};


  SG::ReadHandleKey<CaloCellContainer> m_caloCellContainerKey {this,
      "InputCellContainerName", "AODCellContainer"};

  /** @brief (deta,dphi) granularity*/
  mutable double m_deta;
  mutable double m_dphi;
  // Calo variables
  const CaloDetDescrManager* m_calo_dd;
  const LArEM_ID* m_emid;
  /** @brief CaloSample */
  mutable CaloSampling::CaloSample m_sam;
  mutable CaloCell_ID::SUBCALO m_subcalo;
  
};

#endif //CALOCLUSTER_ONTRACKBUILER_H
