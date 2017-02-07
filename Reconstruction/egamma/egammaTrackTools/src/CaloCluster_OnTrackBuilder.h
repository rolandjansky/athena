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

namespace Trk{
  class CaloCluster_OnTrack;
  class Surface;
  class LocalParameters;
}

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCellContainer;
class CaloDetDescrManager;
class ICaloSurfaceBuilder;

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
  ToolHandle<ICaloSurfaceBuilder>  m_calosurf;
  
  bool m_useClusterEnergy;
  bool m_useClusterPhi;
  bool m_useClusterEta;


  std::string                          m_caloCellContainerName;
  mutable const CaloCellContainer*     m_cellContainer;
  
  /** @brief (eta,phi) around which estimate the shower shapes */
  mutable double m_eta;
  mutable double m_phi;
  /** @brief (deta,dphi) granularity*/
  mutable double m_deta;
  mutable double m_dphi;
  // Calo variables
  const CaloDetDescrManager* m_calo_dd;
  const LArEM_ID* m_emid;
  /** @brief CaloSample */
  mutable CaloSampling::CaloSample m_sam;
  mutable CaloCell_ID::SUBCALO m_subcalo;
  mutable bool m_barrel;
  
};

#endif //CALOCLUSTER_ONTRACKBUILER_H
