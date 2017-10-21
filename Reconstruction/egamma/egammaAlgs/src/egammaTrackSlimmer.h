/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATRACKSLIMMER_H
#define EGAMMAALGS_EGAMMATRACKSLIMMER_H
/**
  @class egammaTrackSlimmer Thinning of GSFTrackParticle collection
*/


// INCLUDE HEADER FILES:
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODEgamma/PhotonContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

class IThinningSvc;

class egammaTrackSlimmer : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  egammaTrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Destructor*/
  ~egammaTrackSlimmer();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

 private:


  /** @brief electron collection input name*/
  SG::ReadHandleKey<xAOD::ElectronContainer> m_InputElectronContainerKey {this, 
      "InputElectronContainerName", 
      "Electrons", 
      "Name of the input electron container"};

  /** @brief photon collection input name*/
  SG::ReadHandleKey<xAOD::PhotonContainer> m_InputPhotonContainerKey {this,
      "InputPhotonContainerName",
      "Photons",
      "Name of the input photon container"};
		
  /** @brief GSF Track Particle container to thin */
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticlesKey {this,
      "TrackParticleContainerName", 
      "",
      "Name of the Track Particle container to thin"};

  /** @brief  GSF vertex container to thin */
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertexKey {this,
      "VertexContainerName", 
      "",
      "Name of the Vertex container to thin"};

  /** @brief Bool to decide if we actually do the Thinning */
  Gaudi::Property<bool> m_doThinning{this, "doThinning", true, "Bool to do Thinning"};

  /** @brief The thinning service */
  ServiceHandle<IThinningSvc> m_thinningSvc;

};

#endif









