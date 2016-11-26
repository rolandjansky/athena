///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonElossTool.cxx 
// Implementation file for class METMuonElossTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METMuonElossTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Muon EDM
#include "xAODMuon/Muon.h"

// Calo EDM
#include "xAODCaloEvent/CaloCluster.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Muon;
  //
  using xAOD::CaloCluster;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METMuonElossTool::METMuonElossTool(const std::string& name) : 
    AsgTool(name),
    METRefinerTool(name)
  {
  }

  // Destructor
  ///////////////
  METMuonElossTool::~METMuonElossTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METMuonElossTool::initialize()
  {
    ATH_CHECK( METRefinerTool::initialize() );
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METMuonElossTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode METMuonElossTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::Tags::correctedTerm(MissingETBase::Status::Nominal,
								      MissingETBase::Status::PileupTrack));

    // Extract the component corresponding to the Cluster SoftTerms
    MissingETBase::Types::bitmask_t src_ST_clus = MissingETBase::Source::softEvent() | MissingETBase::Source::cluster();
    MissingETComponentMap::const_iterator citer = MissingETComposition::find(metMap,src_ST_clus);
    if(citer==metMap->end()) {
      ATH_MSG_WARNING("Could not find soft cluster component in MET Map!");
      return StatusCode::SUCCESS;
    }
    vector<const IParticle*> clusterList = (*citer)->objects();

    // Extract the component corresponding to the Muons term
    citer = MissingETComposition::find(metMap,MissingETBase::Source::muon());
    if(citer==metMap->end()) {
      ATH_MSG_WARNING("Could not find Muons component in MET Map!");
      return StatusCode::SUCCESS;
    }
    vector<const IParticle*> muonList = (*citer)->objects();

    metTerm->setSource(MissingETBase::Source::muon());

    // Loop over the tracks and select only good ones
    MissingETBase::Types::weight_t unitWeight(1.,1.,1.);
    for( const auto& obj1 : muonList ) {
      if(obj1->type() != xAOD::Type::Muon) {
        ATH_MSG_WARNING("MuonEloss given an object of type " << obj1->type());
        continue;
      }
      const Muon* muon = static_cast<const Muon*>(obj1);
      double clusterSum = 0.;
      for( const auto& obj2 : clusterList ) {
	// search for clusters in the vicinity of the muon
	// (really, we should use the track extrapolation)
	// to place a cap on the correction.
	if(obj2->type()==xAOD::Type::CaloCluster) {
	const CaloCluster* cl = static_cast<const CaloCluster*>(obj2);
	  if(xAOD::P4Helpers::isInDeltaR(*cl,*muon,0.1,m_useRapidity)) {
	    clusterSum += cl->e();
	  }
        }
        else { ATH_MSG_WARNING("MuonEloss given an object of type " << obj2->type()); }
      }
      ATH_MSG_DEBUG("Muon with pt " << muon->pt() << " passes through clusters with total E = " << clusterSum);
    }

    // not sure what to add here
    // for( vector<const IParticle*>::const_iterator iPar=filteredTrackList.begin();
    // 	 iPar!=filteredTrackList.end(); ++iPar ) {
    //   MissingETComposition::insert(metMap,metTerm,muon,filteredTrackList,unitWeight);
    // }
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}

