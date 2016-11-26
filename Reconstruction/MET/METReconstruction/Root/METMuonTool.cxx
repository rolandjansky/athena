///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonTool.cxx 
// Implementation file for class METMuonTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METMuonTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Muon;
  using xAOD::MuonContainer;
  using xAOD::MuonSegmentContainer;
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
  METMuonTool::METMuonTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {
    declareProperty( "MinP",             m_mu_minP           = 4e3   );
    declareProperty( "MinPt",            m_mu_minPt          = 2.5e3 );
    declareProperty( "MaxEta",           m_mu_maxEta         = 2.5   );
    declareProperty( "QualCut",          m_mu_qualcut        = 2     ); // 0:Tight, 1:Medium, 2:Loose
    declareProperty( "MinNpixel",        m_mu_nPixHits       = 3     );
    declareProperty( "MinNsilicon",      m_mu_nSiHits        = 5     );
    declareProperty( "DoStandAlone",     m_mu_doStandAlone   = true  );
    declareProperty( "MinEtaStandAlone", m_mu_minEtaSA       = 2.5   );
    declareProperty( "MinNsegments",     m_mu_nSegments      = 0     );
    declareProperty( "MinNprecision",    m_mu_nPrecisionHits = 3     );
    declareProperty( "DoTracks",         m_mu_doTracks       = true  );
  }

  // Destructor
  ///////////////
  METMuonTool::~METMuonTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METMuonTool::initialize()
  {
    ATH_CHECK( METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // Provide parser of input data string here? 
    // Or take a list of inputs?

    return StatusCode::SUCCESS;
  }

  StatusCode METMuonTool::finalize()
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

  bool METMuonTool::accept(const xAOD::IParticle* object) const
  {

    if(object->type() != xAOD::Type::Muon) {
      ATH_MSG_WARNING("METMuonTool::accept given an object of type " << object->type());
      return false;
    }
    const Muon* mu = static_cast<const Muon*>(object);

    ATH_MSG_VERBOSE( "Test muon with pt " << mu->pt() << " eta " << mu->eta() );

    //    if(mu->quality()>m_mu_qualcut) return false;
    if(mu->pt()<m_mu_minPt || mu->pt()/cosh(mu->eta())<m_mu_minP) return false;
    if(mu->muonType()==Muon::MuonStandAlone) {
      if(!m_mu_doStandAlone) return false;
      else {
	// only take forward SA -- need a max eta cut?
	if(fabs(mu->eta())<m_mu_minEtaSA) return false;
	// cuts on number of muon segments and precision hits
	int nSeg=mu->muonSegmentLinks().size();
	if(nSeg<m_mu_nSegments) return false;
	uint8_t nPrecision=0;
	if(!mu->primaryTrackParticleLink().isValid()) return false;
	mu->primaryTrackParticle()->summaryValue(nPrecision,xAOD::numberOfPrecisionLayers);
	if(nPrecision<m_mu_nPrecisionHits) return false;
      } // only if we use SA muons
    } // selection for StandAlone muons
    else if(mu->muonType()==Muon::Combined || mu->muonType()==Muon::SegmentTagged) {
      if(fabs(mu->eta())>m_mu_maxEta) return false;

      // could add some error checking to make sure we successfully read the details
      uint8_t nPixHits(0), nSctHits(0);
      if(!mu->primaryTrackParticleLink().isValid()) return false;
      mu->primaryTrackParticle()->summaryValue(nPixHits,xAOD::numberOfPixelHits);
      mu->primaryTrackParticle()->summaryValue(nSctHits,xAOD::numberOfSCTHits);

      if(nPixHits<m_mu_nPixHits) return false;
      if(nPixHits+nSctHits<m_mu_nSiHits) return false;
    } // selection for SegmentTagged and Combined muons
    else {return false;} // don't accept forward muons or calo tagged
  
    return true;
  }

  bool METMuonTool::resolveOverlap(const xAOD::IParticle* object,
				   xAOD::MissingETComponentMap* metMap,
				   std::vector<const xAOD::IParticle*>& acceptedSignals,
				   MissingETBase::Types::weight_t& /*objWeight*/) const
  {

    if(object->type() != xAOD::Type::Muon) {
      ATH_MSG_WARNING("METMuonTool::resolveOverlap given an object of type " << object->type());
      return false;
    }
    const Muon* mu = static_cast<const Muon*>(object);

    if(m_mu_doTracks) {
      // retrieve tracks associated to the electron
      ATH_MSG_VERBOSE("Extract muon track.");
      if(mu->inDetTrackParticleLink().isValid())
	acceptedSignals.push_back(*mu->inDetTrackParticleLink());
    } // loop over muon-matched tracks
    metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyTrack);
  
    return true;
  }

  StatusCode METMuonTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const MuonContainer* muCont = 0;
    if( evtStore()->retrieve(muCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input muon container");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved muon collection");

    MissingETBase::Types::bitmask_t source = MissingETBase::Source::muon();
    metTerm->setSource(source);

    vector<const IParticle*> signalList;
    signalList.reserve(muCont->size());
    for(MuonContainer::const_iterator iMu=muCont->begin();
	iMu!=muCont->end(); ++iMu) {
      if ( this->accept(*iMu) ) {
	signalList.clear();
	MissingETBase::Types::weight_t objWeight;
	this->resolveOverlap(*iMu,metMap,signalList,objWeight);
	ATH_MSG_VERBOSE("Add this muon to MET.");
	this->addToMET(*iMu,signalList,metTerm,metMap,objWeight);
      } // if passing selection
    } // loop on mu container

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


}
