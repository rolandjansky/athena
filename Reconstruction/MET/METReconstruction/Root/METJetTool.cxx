///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetTool.cxx 
// Implementation file for class METJetTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METJetTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Jet EDM
#include "xAODJet/JetTypes.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAttributes.h"

// Calo EDM
#include "xAODCaloEvent/CaloCluster.h"

// Calo helpers
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Jet;
  using xAOD::JetContainer;
  using xAOD::JetAttribute;
  using xAOD::JetConstituentVector;
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
  METJetTool::METJetTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {
    // don't select on pt now, we will calibrate later.
    declareProperty( "MinPt",         m_jet_minPt          = 0     );
    declareProperty( "MaxEta",        m_jet_maxEta         = 5     );
    declareProperty( "DoQualCut",     m_jet_doQualCut      = false );
    declareProperty( "JetBadQual",    m_jet_badQual        = "isBadLoose" ); // don't like string comparisons, but the jet guys do
    declareProperty( "MinWet",        m_jet_minWet         = 0.5   );
    declareProperty( "DoMinWetPtCut", m_jet_doMinWetPtCut  = false );
    declareProperty( "DoTracks",      m_jet_doTracks       = true  );
  }

  // Destructor
  ///////////////
  METJetTool::~METJetTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METJetTool::initialize()
  {
    ATH_CHECK( METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // Provide parser of input data string here? 
    // Or take a list of inputs?

    return StatusCode::SUCCESS;
  }

  StatusCode METJetTool::finalize()
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


  bool METJetTool::accept(const xAOD::IParticle* object) const {
    const Jet* jet = dynamic_cast<const Jet*>(object);

    if(!jet) {
      ATH_MSG_WARNING("Jet accept given an object of type " << object->type());
      return false;
    }

    if( jet->pt() < m_jet_minPt ) return false;
    if( fabs(jet->eta()) > m_jet_maxEta) return false;

    // analyses should probably still veto the event...
    if( m_jet_doQualCut ) {
      bool isBad = false;
      jet->getAttribute<bool>(m_jet_badQual,isBad);
      if( isBad ) return false;
    }

    return true;
  }

  bool METJetTool::resolveOverlap(const xAOD::IParticle* object,
				  xAOD::MissingETComponentMap* metMap,
				  std::vector<const xAOD::IParticle*>& acceptedSignals,
				  MissingETBase::Types::weight_t& objWeight) const
  {

    if(object->type() != xAOD::Type::Jet) {
      ATH_MSG_WARNING("Jet resolveOverlap given an object of type " << object->type());
      return false;
    }
    const Jet* jet = static_cast<const Jet*>(object);
    //m_signalstate = jet->getConstituentsSignalState();

    ATH_MSG_VERBOSE("Retrieving jet constituents.");
    // first get the topoclusters
    // is it better to immediately put the constituents into acceptedSignals?
    JetConstituentVector constit = jet->getConstituents();
    ATH_MSG_VERBOSE("Current jet has " << constit.size() << " constituents.");
    // test for used topoclusters, and retrieve unused ones (ok until/unless we use PFlow jets)
    // only use clusters for computing the overlap removal relative to other objects
    double sumE_allclus = 0.;
    vector<const IParticle*> constit_vec;
    constit_vec.reserve(jet->numConstituents());
    CaloClusterChangeSignalStateList stateHelperList;
    for(JetConstituentVector::const_iterator iClus = constit.begin();
	iClus!=constit.end(); ++iClus) {
      sumE_allclus += (*iClus)->e();
      const CaloCluster* pClus = dynamic_cast<const CaloCluster*>( (*iClus)->rawConstituent() );
      if(pClus) {
        stateHelperList.add(pClus, CaloCluster::State(m_signalstate));
        ATH_MSG_VERBOSE("Constit E = " << pClus->e());
        constit_vec.push_back(pClus);
      } else {
        ATH_MSG_WARNING("Cluster resolveOverlap given an object of type " << (*iClus)->type());
      }
    } // loop over jet constituents
    ATH_MSG_VERBOSE( "Jet E = " << jet->e() << ", cluster energy sum = " << sumE_allclus );
    ATH_MSG_VERBOSE( "Check signal states" );
    ATH_MSG_VERBOSE( "Jet pt (default) = " << jet->jetP4(xAOD::JetAssignedScaleMomentum).Pt()
		     << " jet pt (uncalconstit_scale) = " << jet->jetP4(xAOD::JetEMScaleMomentum).Pt()
		     << " jet pt (jetfinding_scale) = " << jet->jetP4(xAOD::JetConstitScaleMomentum).Pt() );
    bool clustersUsed = metMap->checkUsage(constit_vec,MissingETBase::UsageHandler::OnlyCluster);
    if(clustersUsed) { // true implies some cluster has been used
      double sumE_unique = 0.; // sumE or sumet?
      for(vector<const IParticle*>::const_iterator iClus = constit_vec.begin();
	  iClus!=constit_vec.end(); ++iClus) {
	sumE_unique += (*iClus)->e();
	acceptedSignals.push_back(*iClus);
	ATH_MSG_VERBOSE("Unique constit E = " << (*iClus)->e());
      } // loop over jet unique constituents
      double scalef = sumE_unique / sumE_allclus;
      // weight as an entire object with the unused E fraction
      // decision about what to do with the weight later
      if(scalef>1.) ATH_MSG_WARNING( "Computed a scale factor " << scalef << " larger than 1!" );
      ATH_MSG_VERBOSE( "Jet unique cluster energy sum = " << sumE_unique );
      objWeight = MissingETBase::Types::weight_t(scalef,scalef,scalef);
    } else {

      ATH_MSG_VERBOSE( "Jet has no overlaps." );
      // weight as an entire object with the unused E fraction
      // decision about what to do with the weight later
      acceptedSignals = constit_vec;
      objWeight = MissingETBase::Types::weight_t(1.,1.,1.);
    }

    if(m_jet_doTracks) {
      // now find associated tracks
      vector<const IParticle*> jettracks = jet->getAssociatedObjects<IParticle>(JetAttribute::GhostTrack);
      ATH_MSG_VERBOSE( "Jet contains " << jettracks.size() << " ghost-associated tracks." );
      // test for used tracks, and retrieve unused ones
      metMap->checkUsage(jettracks,MissingETBase::UsageHandler::OnlyTrack);
      ATH_MSG_VERBOSE( "Of these, " << jettracks.size() << " are unique." );
      for(vector<const IParticle*>::const_iterator iTrk = jettracks.begin();
	  iTrk!=jettracks.end(); ++iTrk) {
	acceptedSignals.push_back(*iTrk);
      } // loop over jet tracks
    }

    return !clustersUsed; // return true if the jet shares no clusters with another object
  }

  StatusCode METJetTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const JetContainer* jetCont = 0;
    if( evtStore()->retrieve(jetCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input jet container");
      return StatusCode::SUCCESS;
    }

    MissingETBase::Types::bitmask_t source = MissingETBase::Source::jet();
    metTerm->setSource(source);

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::contributedHardTerm());

    ATH_MSG_DEBUG("Retrieved jet collection: " << m_input_data_key);

    vector<const IParticle*> signalList;
    for(JetContainer::const_iterator iJet=jetCont->begin();
	iJet!=jetCont->end(); ++iJet) {
      if ( this->accept(*iJet) ) {
	ATH_MSG_VERBOSE("Accepted jet with "
			<< "pT = " << (*iJet)->pt()
			<< " eta = " << (*iJet)->eta()
			<< " phi = " << (*iJet)->phi());
	signalList.clear();
	signalList.reserve(2*(*iJet)->numConstituents());
	MissingETBase::Types::weight_t objWeight;
	if ( this->resolveOverlap(*iJet,metMap,signalList,objWeight) ) {
	  // add automatically if not overlapping anything
	  this->addToMET(*iJet,signalList,metTerm,metMap,objWeight);
	} else {
	  if( m_jet_doMinWetPtCut ) { 
            if( (*iJet)->pt()*objWeight.wet() > m_jet_minPt ) {
	      ATH_MSG_VERBOSE("Jet unique energy is above threshold -- add to MET.");
              this->addToMET(*iJet,signalList,metTerm,metMap,objWeight);
              ATH_MSG_VERBOSE("Jet px = " << (*iJet)->px()
			      << ", weighted px = " << (*iJet)->px()*objWeight.wpx()
			      << ", MET px = " << metTerm->mpx() );
              ATH_MSG_VERBOSE("Jet pt = " << (*iJet)->pt()
			      << ", weighted pt = " << (*iJet)->pt()*objWeight.wet()
			      << ", MET pt = " << metTerm->met() );
            }
	  } else {
	    if( objWeight.wet() > m_jet_minWet ) {
              ATH_MSG_VERBOSE("Jet weighted energy is above threshold -- add to MET");
	      this->addToMET(*iJet,signalList,metTerm,metMap,objWeight);
	      ATH_MSG_VERBOSE("Jet px = " << (*iJet)->px()
			      << ", weighted px = " << (*iJet)->px()*objWeight.wpx()
			      << ", MET px = " << metTerm->mpx() );
            }
          } // end if minWet
	}
      } // if passing selection
    } // loop on jet container

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
