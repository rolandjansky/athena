///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// GaussianDensityTestAlg.h 
// Header file for class GaussianDensityTestAlg
// Author: D.Casper<dcasper@uci.edu>
/////////////////////////////////////////////////////////////////// 
#ifndef TRKVERTEXSEEDFINDERUTILS_GAUSSIANDENSITYTESTALG_H
#define TRKVERTEXSEEDFINDERUTILS_GAUSSIANDENSITYTESTALG_H 1

// STL includes
#include <string>
#include <vector>
#include <map>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include "TH1F.h"
#include "TTree.h"

namespace Trk
{
  class Track;
  class ITrackLink;

class GaussianDensityTestAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  GaussianDensityTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~GaussianDensityTestAlg(); 

  // Assignment operator: 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private:
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  double ipSignificance(const Trk::TrackParameters* params, const Amg::Vector3D * vertex) const;

  StatusCode findTruth(const std::vector<Trk::ITrackLink*>& trackVector, std::vector<Amg::Vector3D>& truth) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  GaussianDensityTestAlg();

  void selectTracks(const xAOD::TrackParticleContainer* trackParticles, std::vector<Trk::ITrackLink*> & trackVector);

  void analyzeTracks(const std::vector<Trk::ITrackLink*> & trackVector, std::vector<const Trk::TrackParameters*>& perigeeList);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  // Properties
  double m_significanceTruthCut;

  int m_truthVertexTracks;

  bool m_useBeamConstraint;
  bool m_firstEvent;

  // Tools
  ToolHandle< InDet::IInDetTrackSelectionTool > m_trackFilter { "InDet::InDetTrackSelectionTool/InDetDetailedTrackSelectionTool" }; 
  ToolHandle< Trk::IVertexTrackDensityEstimator > m_estimator { "Trk::GaussianTrackDensity"}; 

  ToolHandle< Trk::ITrackToVertexIPEstimator > m_ipEstimator { "Trk::TrackToVertexIPEstimator" };

  // Non-property private data
  
  ServiceHandle< IBeamCondSvc > m_iBeamCondSvc;
  ServiceHandle< ITHistSvc > m_iTHistSvc;

  /// Data handle keys
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticlesKey  { "InDetTrackParticles" };

  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventsKey { "TruthEvents" };

  SG::ReadHandleKey<xAOD::TruthPileupEventContainer> m_pileupEventsKey { "TruthPileupEvents" };
  
  /// Histograms and trees

  TH1* m_h_density;
  TH1* m_h_truthDensity;
  TH1* m_h_truthVertices;
  TH1* m_h_modeCheck;

}; // class
}  // namespace
// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H
