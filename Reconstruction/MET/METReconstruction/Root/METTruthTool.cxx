///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTruthTool.cxx 
// Implementation file for class METTruthTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METTruthTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Truth EDM
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

// Truth Utilities
#include "TruthUtils/TruthParticleHelpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::TruthParticle;
  using xAOD::TruthParticleContainer;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METTruthTool::METTruthTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name),
    m_truth_type(0)
  {
    // NonInt, Int, IntMuons, IntOut
    declareProperty( "InputComposition", m_inputType = "NonInt" ); // Truth type
    declareProperty( "MaxEtaDet",        m_det_maxEta = 5.      ); // Nominal max detector eta
    declareProperty( "MinPtMu"  ,        m_truthmu_minPt = 6e3  ); // Nominal min muon pt
    declareProperty( "MaxEtaMu",         m_truthmu_maxEta = 2.7  ); // Nominal max MS eta
  }

  // Destructor
  ///////////////
  METTruthTool::~METTruthTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTruthTool::initialize()
  {
    ATH_CHECK( METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    if(m_inputType=="NonInt") {
      m_truth_type = MissingETBase::Source::truthNonInt();
    } else if(m_inputType=="Int") {
      m_truth_type = MissingETBase::Source::truthInt();
    } else if(m_inputType=="IntOut") {
      m_truth_type = MissingETBase::Source::truthIntOut();
    } else if(m_inputType=="IntMuons") {
      m_truth_type = MissingETBase::Source::truthMuons();
    } else {
      ATH_MSG_FATAL("Invalid input type provided");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METTruthTool::finalize()
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

  bool METTruthTool::accept(const xAOD::IParticle* object) const
  {
    // Apply cuts 
    ATH_MSG_VERBOSE("Check if truth particle is accepted");

    if(object->type() != xAOD::Type::TruthParticle) { 
      ATH_MSG_WARNING("METTruthTool::accept given an object of type" << object->type());
      return false;
    }
    const xAOD::TruthParticle* truth = static_cast<const xAOD::TruthParticle*>(object);

    if(truth->pt()<1e-9) return false; // reject particles with no pt
    ATH_MSG_VERBOSE("My truth type: " << m_truth_type);
    switch(m_truth_type) {
    case MissingETBase::Source::NonInt:
      return accept_nonint(truth);
    case MissingETBase::Source::Int:
      return accept_int(truth);
    case MissingETBase::Source::IntOut:
      return accept_intout(truth);
    case MissingETBase::Source::TruthMuons:
      return accept_intmuons(truth);
    default:
      return false;
    }
    // should not get here
    return false;
  }

  bool METTruthTool::accept_nonint(const xAOD::TruthParticle* truth) const
  {
    ATH_MSG_VERBOSE("Check nonint");
    // stable and non-interacting
    if (MC::isGenStable(truth->status(),truth->barcode()) && MC::isNonInteracting(truth->pdgId())) return true;

    return false;
  }

  bool METTruthTool::accept_int(const xAOD::TruthParticle* truth) const
  {
    ATH_MSG_VERBOSE("Check int");
    // not muon
    if(truth->isMuon()) return false;
    // stable
    if(!MC::isGenStable(truth->status(),truth->barcode())) return false;
    // interacting
    if(MC::isNonInteracting(truth->pdgId())) return false;
    // in acceptance
    if(fabs(truth->eta())>m_det_maxEta) return false;

    return true;
  }
  
  bool METTruthTool::accept_intout(const xAOD::TruthParticle* truth) const
  {
    ATH_MSG_VERBOSE("Check intout");
    // not in acceptance (calo or MS)
    if( (truth->isMuon() && fabs(truth->eta())<m_truthmu_maxEta) ||
	(fabs(truth->eta())<m_det_maxEta) ) return false;
    // stable
    if(!MC::isGenStable(truth->status(),truth->barcode())) return false;
    // interacting
    if(MC::isNonInteracting(truth->pdgId())) return false;

    return true;
  }

  bool METTruthTool::accept_intmuons(const xAOD::TruthParticle* truth) const
  {
    ATH_MSG_VERBOSE("Check intmuon");
    // muon
    if(!truth->isMuon()) return false;
    // stable
    if(!MC::isGenStable(truth->status(),truth->barcode())) return false;
    // in acceptance
    if(truth->pt()<m_truthmu_minPt && fabs(truth->eta())>m_truthmu_maxEta) return false;

    return true;
  }

  // don't really need this for anything
  bool METTruthTool::resolveOverlap(const xAOD::IParticle* /*object*/,
				    xAOD::MissingETComponentMap* /*metMap*/,
				    std::vector<const xAOD::IParticle*>& /*acceptedSignals*/,
				    MissingETBase::Types::weight_t& /*objWeight*/)
  {

    return true;
  }

  StatusCode METTruthTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const TruthParticleContainer* truthCont = 0;

    metTerm->setSource(m_truth_type);

    // Retrieve the truth container
    if ( evtStore()->retrieve(truthCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input truth particle container");
      return StatusCode::SUCCESS;
    }

    vector<const IParticle*> signalList;
    signalList.reserve(truthCont->size());
    // Loop over all truth particles
    for( TruthParticleContainer::const_iterator iTruth=truthCont->begin(); iTruth!=truthCont->end(); ++iTruth ) {
      // Check if truth particles satisfies the requirements
      if( this->accept(*iTruth) ) {
	// Add the selected truth particles to the list
	signalList.push_back(*iTruth);
      }
    } // end loop over truths

    // Loop over the content and add to MET 
    MissingETBase::Types::weight_t unitWeight(1.,1.,1.);
    MissingETBase::Types::weight_t minusWeight(-1.,-1.,1.);
    vector<const IParticle*> dummyList;
    for( vector<const IParticle*>::const_iterator iPart=signalList.begin();
	 iPart!=signalList.end(); ++iPart) {
      if(m_truth_type==MissingETBase::Source::NonInt) {
	// flip direction for nonint
	this->addToMET(*iPart,dummyList,metTerm,metMap,minusWeight);
      } else {
	this->addToMET(*iPart,dummyList,metTerm,metMap,unitWeight);
      }
    }

    ATH_MSG_DEBUG( "Map contains " << (*MissingETComposition::find(metMap,metTerm))->objects().size() << " truth particles" );
 
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // // TEMPORARILY recopy some helper from TruthHelper and GeneratorUtils packages
  // //  *** via JetSimTools ***
  // // We'll have to use this package when they work properly with xAOD.

  // inline bool isStable(const xAOD::TruthParticle* p) {
  //   if (p->barcode() >= 200000) return false; // This particle is from G4
  //   if (p->pdgId() == 21 && p->p4().E() == 0) return false; //< Workaround for a gen bug?
  //   return ((p->status() % 1000 == 1) || //< Fully stable, even if marked that way by G4
  // 	    (p->status() % 1000 == 2 && p->decayVtx() != NULL && p->decayVtx()->barcode() < -200000)); //< Gen-stable with G4 decay
  //   /// @todo Add a no-descendants-from-G4 check?
  // }
}
