///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// TrackVertexAssoTestAlg.h
// Header file for class TrackVertexAssoTestAlg
// Author: Rachid Mazini, Bo Liu
///////////////////////////////////////////////////////////////////

#ifndef TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H
#define TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AnaAlgorithm/AnaAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Tools
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

// xAOD
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// ROOT
#include "TEfficiency.h"

class TrackVertexAssoTestAlg : public EL::AnaAlgorithm
{ 

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////
public: 

  /// Constructor with parameters:
  TrackVertexAssoTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TrackVertexAssoTestAlg();

  // Athena algorithm's Hooks
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

///////////////////////////////////////////////////////////////////
// Private data:
///////////////////////////////////////////////////////////////////
private:

  /// Default constructor:
  TrackVertexAssoTestAlg();

  ToolHandle<CP::ITrackVertexAssociationTool> m_TVATool;

  /// Containers
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trkContname { this, "TrackContainer", "InDetTrackParticles", "Input track particles" };
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContname { this, "VertexContainer", "PrimaryVertices", "Input vertices" };

  int   m_nbins = 26;
  float m_xmin  = -2.5;
  float m_xmax  = +2.5;
  TEfficiency* m_h_eff = nullptr; //!
  TEfficiency* m_h_imp = nullptr; //!

};

#endif //> !TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H
