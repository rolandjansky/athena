/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITOOLBASE_TAU_H
#define ITOOLBASE_TAU_H

#include "AsgTools/IAsgTool.h"

#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"

#ifndef XAOD_ANALYSIS
#include "CaloEvent/CaloCellContainer.h"
#endif

/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 * @author Justin Griffiths
 * Thanks to Lianyou Shan, Lorenz Hauswald
 */

class ITauToolBase : virtual public asg::IAsgTool
{
 public:

  ASG_TOOL_INTERFACE(ITauToolBase)    

  virtual ~ITauToolBase() {}

  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize() = 0;

  //-----------------------------------------------------------------
  //! Event initializer - called at the beginning of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventInitialize() = 0;

  //-----------------------------------------------------------------
  //! Execute - called for each tau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute(xAOD::TauJet& pTau) const = 0;
  virtual StatusCode executeVertexFinder(xAOD::TauJet& pTau, 
                                         const xAOD::VertexContainer* vertexContainer = nullptr, 
                                         const xAOD::TrackParticleContainer* trackContainer = nullptr) const = 0;
  virtual StatusCode executeTrackFinder(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackContainer, const xAOD::TrackParticleContainer* trackContainer = nullptr) const = 0;
  virtual StatusCode executeTrackClassifier(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackContainer) const = 0;
  virtual StatusCode executeRNNTrackClassifier(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackContainer) const = 0;
  virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& shotClusterContainer, xAOD::PFOContainer& PFOContainer ) const = 0;
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer, 
  					      xAOD::PFOContainer& hadronicPFOContainer, 
					      xAOD::CaloClusterContainer& caloClusterContainer, 
					      const xAOD::CaloClusterContainer& pCaloClusterContainer ) const = 0;
  virtual StatusCode executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& vertexContainer ) const = 0;  
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer, xAOD::PFOContainer& chargedPFOContainer ) const = 0;  
  virtual StatusCode executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer) const = 0;
  virtual StatusCode executePanTau(xAOD::TauJet& pTau, xAOD::ParticleContainer& particleContainer) =0;

#ifdef XAOD_ANALYSIS
  // non-const version is needed in THOR
  virtual StatusCode executeDev(xAOD::TauJet& pTau) = 0;
#else
  // CaloCellContainer not available in AnalysisBase
  virtual StatusCode executePi0CreateROI(xAOD::TauJet& pTau, CaloCellContainer& caloCellContainer, std::vector<CaloCell*>& map ) = 0;
#endif
  
  //-----------------------------------------------------------------
  //! Event finalizer - called at the end of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventFinalize() = 0;

  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize() = 0;

  //make pure
  virtual StatusCode readConfig() = 0;

};

#endif // TOOLBASE_TAU_H
