///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
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

  StatusCode findTruth(double mode,
                       const std::vector<Trk::ITrackLink*>& trackVector,
                       std::vector<Amg::Vector3D>& truth,
                       TH1* h_truthDensity,
                       TH1* h_modeCheck) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  GaussianDensityTestAlg();

  void selectTracks(const xAOD::TrackParticleContainer* trackParticles, std::vector<Trk::ITrackLink*> & trackVector);

  static void analyzeTracks(const std::vector<Trk::ITrackLink*> & trackVector, std::vector<const Trk::TrackParameters*>& perigeeList);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  // Properties
  Gaudi::Property<double> m_significanceTruthCut { this, 
                                                   "SignificanceTruthCut", 
                                                   3.0, 
                                                   "Reco track must pass within this many sigma of pp vertex to be good" };

  Gaudi::Property<int> m_truthVertexTracks { this, 
                                             "MinTruthVertexTracks", 
                                             2, 
                                             "Minimum associated reconstructed tracks for vertex to be considered visible" };

  bool m_useBeamConstraint;
  bool m_firstEvent;

  // Tools
  ToolHandle< InDet::IInDetTrackSelectionTool > m_trackFilter { this, "TrackSelector", 
                                                               "InDet::InDetTrackSelectionTool/InDetDetailedTrackSelectionTool", 
                                                               "Track selection tool" } ;

  ToolHandle< Trk::IVertexTrackDensityEstimator > m_estimator { this, "Estimator", "Trk::GaussianTrackDensity", 
                                                                "Track density function" };

  ToolHandle< Trk::ITrackToVertexIPEstimator > m_ipEstimator { this, "IPEstimator", "Trk::TrackToVertexIPEstimator",
                                                                 "Impact point estimator" };

  // Non-property private data
  
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  ServiceHandle< ITHistSvc > m_iTHistSvc;

  /// Data handle keys
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticlesKey  { this, "TrackParticles", "InDetTrackParticles", 
                                                                         "Input track particle collection" };

  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventsKey { this, "TruthEvents", "TruthEvents",
                                                                  "Key for truth event collection" };

  SG::ReadHandleKey<xAOD::TruthPileupEventContainer> m_pileupEventsKey { this, "TruthPileupEvents", "TruthPileupEvents",
                                                                         "Key for truth pileup event collection" };
  
  /// Histograms and trees

  TH1* m_h_density{};
  TH1* m_h_truthDensity{};
  TH1* m_h_truthVertices{};
  TH1* m_h_modeCheck{};

}; // class
}  // namespace
// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H
