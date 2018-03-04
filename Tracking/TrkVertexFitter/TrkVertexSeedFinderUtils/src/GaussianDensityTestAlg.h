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

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

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
  bool m_useBeamConstraint;

  // Tools
  ToolHandle< InDet::IInDetTrackSelectionTool > m_trackFilter { this, "TrackSelector", 
                                                               "InDet::InDetTrackSelectionTool/InDetDetailedTrackSelectionTool", 
                                                               "Track selection tool" } ;

  ToolHandle< Trk::IVertexTrackDensityEstimator > m_estimator { this, "Estimator", "Trk::GaussianTrackDensity", 
                                                                "Track density function" };

  // Non-property private data
  
  ServiceHandle< IBeamCondSvc > m_iBeamCondSvc;
  ServiceHandle< ITHistSvc > m_iTHistSvc;

  /// Data handle keys
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticlesKey  { this, "TrackParticles", "InDetTrackParticles", 
                                                                         "Input track particle collection" };

  /// Histograms and trees

  TH1* m_h_density;

}; // class
}  // namespace
// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H
