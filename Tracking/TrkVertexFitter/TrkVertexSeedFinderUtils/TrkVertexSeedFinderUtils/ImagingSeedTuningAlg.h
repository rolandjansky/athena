///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ImagingSeedTuningAlg.h 
// Header file for class ImagingSeedTuningAlg
// Author: D.Casper<dcasper@uci.edu>
/////////////////////////////////////////////////////////////////// 
#ifndef TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H
#define TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H 1

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

#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include "TH1F.h"
#include "TTree.h"

class IBeamCondSvc;

namespace InDet
{
  class IInDetTrackSelectionTool;
}

namespace Trk
{
  class Track;
  class ITrackLink;
  class IVertexSeedFinder;
  class IImpactPoint3dEstimator;

class ImagingSeedTuningAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ImagingSeedTuningAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ImagingSeedTuningAlg(); 

  // Assignment operator: 
  //ImagingSeedTuningAlg &operator=(const ImagingSeedTuningAlg &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private:
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  // compute track distance of approach to vertex
  double distanceAndError(const Trk::TrackParameters* params, const Amg::Vector3D * vertex, double & err) const;

  // routine to identify visible truth vertices
  StatusCode findTruth(const std::vector<Trk::ITrackLink*> & trackVector, std::vector<Amg::Vector3D>& truth) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  void selectTracks(const xAOD::TrackParticleContainer* trackParticles, std::vector<Trk::ITrackLink*> & trackVector);

  void analyzeTracks(const std::vector<Trk::ITrackLink*> & trackVector, std::vector<const Trk::TrackParameters*>& perigeeList);

  void analyzeSeeds(std::string conditions, const std::vector<Amg::Vector3D>& seeds, const std::vector<Amg::Vector3D>& truth);

  StatusCode initializeConditions(std::string& conditions);

  StatusCode iterateConditions(std::string& conditions, bool& done);

  StatusCode setupConditions(std::string& conditions);
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  // Properties
  // Reco track must pass within this many sigma of interaction vertex to be "good"
  double m_significanceTruthCut;

  // This many good reco tracks required for truth vertex to be "good"
  int m_truthVertexTracks;

  // Window around truth vertex for a seed to be "good"
  double m_truthWindow;

  // Use beam spot?
  bool m_useBeamConstraint;

  // Input parameter ranges
  std::vector<float> m_angularCutoffParameterValues;
  std::vector<float> m_clusterWindowXYValues;
  std::vector<float> m_mergeParameterValues;
  std::vector<float> m_weightThresholdValues;
  std::vector<bool>  m_refineZValues;
  std::vector<bool>  m_gaussianWindowValues;

  // Tools
  ToolHandle< InDet::IInDetTrackSelectionTool > m_trackFilter;
  ToolHandle< Trk::IVertexSeedFinder > m_seedFinder;
  ToolHandle< Trk::IImpactPoint3dEstimator > m_impactPoint3dEstimator;

  // Non-property private data
  
  // Results
  std::map<std::string, int> m_allSeeds;
  std::map<std::string, int> m_allTruth;
  std::map<std::string, int> m_goodSeeds;
  std::map<std::string, int> m_goodTruth;
  std::map<std::string, int> m_nFake;
  std::map<std::string, int> m_nLost;
  std::map<std::string, int> m_nSplit;
  std::map<std::string, int> m_nMerge;
  // IProperty handles
  SmartIF<IProperty> m_imageMakerProperty;
  SmartIF<IProperty> m_clusterFinderProperty;

  ServiceHandle< IBeamCondSvc > m_iBeamCondSvc;
  ServiceHandle< ITHistSvc > m_iTHistSvc;

  std::vector<float>::const_iterator m_i_angularCutoffParameter;
  std::vector<float>::const_iterator m_i_clusterWindowXY;
  std::vector<float>::const_iterator m_i_mergeParameter;
  std::vector<float>::const_iterator m_i_weightThreshold;
  std::vector<bool>::const_iterator  m_i_refineZ;
  std::vector<bool>::const_iterator  m_i_gaussianWindow;

  /// Default constructor: 
  ImagingSeedTuningAlg();

  /// Containers

  /// Histograms and trees

  TH1* m_h_nTruthVertices;
  TH1* m_h_zTruthVertices;

  TTree* m_t_seeds;
  int m_b_nTruth;
  int m_b_nConditions;
  int m_b_nSeeds[5000];
  int m_b_nGoodSeeds[5000];
  int m_b_nGoodTruth[5000];
  int m_b_nFake[5000];
  int m_b_nLost[5000];
  int m_b_nMerge[5000];
  int m_b_nSplit[5000];
  float m_b_angularCutoffParameter[5000];
  float m_b_weightThreshold[5000];
  float m_b_mergeParameter[5000];
  float m_b_clusterWindowXY[5000];
  int m_b_refineZ[5000];
  int m_b_gaussianWindow[5000];

  int m_iCondition;

}; // class
}  // namespace
// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRKVERTEXSEEDFINDERUTILS_IMAGINGSEEDTUNINGALG_H
