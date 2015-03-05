/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class is the basis for all Physics-Analysis-Classes
///////////////////////////////////////////////////////////////////////////

#ifndef MuonPhysicsAnalysisMonitoring_H
#define MuonPhysicsAnalysisMonitoring_H


// Gaudi Tools
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/Service.h"

// Storegate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>

#include "Particle/TrackParticle.h"

//vertexing method
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "VxVertex/VxTrackAtVertex.h"


// Defining Cuts for Zmumu-Analysis
struct cutDefinitions
{
  double	Pt_loose;
  double	Pt_tight;
  double        TrigMu_Pt;
  double	diMuonMass;
  double        ZInvmMassWnd;
  double	deltaPhi;
  double	deltaR;
  double	Jet_InHalo_Loose;
  double	NTracks_InHalo_Loose;
  double	PTTracks_InHalo_Loose;
  double	ET_InHalo_Loose;
  double	Jet_InHalo_Tight;
  double	NTracks_InHalo_Tight;
  double	PTTracks_InHalo_Tight;
  double	ET_InHalo_Tight;
};

class MuonPhysicsAnalysisMonitoring
{
 public :
  // Default Constructor - Empty
  MuonPhysicsAnalysisMonitoring();
  ~MuonPhysicsAnalysisMonitoring();

  void	setup(	std::string outputname,		// Name of the Root-File
		std::string RootDirName,	// Directory within the root-File where results should be written
		cutDefinitions cuts		// Which Cuts should be used?
		);

 protected:

  double dRand(double min, double max);

  // Vertex fitting
  Trk::ExtendedVxCandidate* doVertexing(const Rec::TrackParticle *trk1, const Rec::TrackParticle *trk2, ToolHandle<InDet::VertexPointEstimator> m_vertexEstimator,ToolHandle<InDet::ConversionFinderUtils> m_helpertool, ToolHandle < Trk::IVertexFitter > m_iVertexFitter,const Trk::TrackParticleBaseCollection* importedTrackCollection,bool &deleteExtVxCandidate);

  bool isElectron(const Rec::TrackParticle*);

  double CorrectInvMassTwoMuons(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2);

  std::string  m_Outputfile;
  std::string  m_RootDirName;

  cutDefinitions	  cutD;
};

#endif
 
