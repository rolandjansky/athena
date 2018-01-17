///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTauTool.cxx 
// Implementation file for class METTauTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METTauTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Tau EDM
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrack.h"

// Calo EDM
#include "xAODCaloEvent/CaloClusterContainer.h"

// Calo helpers
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Jet;
  using xAOD::JetConstituentVector;
  //
  using xAOD::TauJet;
  using xAOD::TauTrack;
  using xAOD::TauJetContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
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
  METTauTool::METTauTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {
    declareProperty( "MinPt",         m_tau_minPt     = 20e3  ); // use GeV const?
    declareProperty( "MaxEta",        m_tau_maxEta    = 2.5   );
    declareProperty( "IsTauFlag",     m_tau_isTauFlag = xAOD::TauJetParameters::JetBDTSigMedium );
    declareProperty( "DoElVeto",      m_tau_doElVeto  = true  );
    declareProperty( "ElVeto",        m_tau_elVeto    = xAOD::TauJetParameters::EleBDTMedium );
    declareProperty( "DoMuVeto",      m_tau_doMuVeto  = true  );
    declareProperty( "MuVeto",        m_tau_muVeto    = xAOD::TauJetParameters::MuonVeto );
    declareProperty( "MinWet",        m_tau_minWet    = 0.5   );
    declareProperty( "DoMinWetPtCut", m_tau_doMinWetPtCut = false );
    declareProperty( "DoTracks",      m_tau_doTracks  = true  );
  }

  // Destructor
  ///////////////
  METTauTool::~METTauTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTauTool::initialize()
  {
    ATH_CHECK( METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // Provide parser of input data string here? 
    // Or take a list of inputs?

    return StatusCode::SUCCESS;
  }

  StatusCode METTauTool::finalize()
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

  bool METTauTool::accept(const xAOD::IParticle* object) const
  {
    if(object->type() != xAOD::Type::Tau) {
      ATH_MSG_WARNING("METTauTool::accept given an object of type " << object->type());
      return false;
    }
    const TauJet* tau = static_cast<const TauJet*>(object);

    ATH_MSG_VERBOSE("Testing tau with pt " << tau->pt() << ", eta " << tau->eta());
    ATH_MSG_VERBOSE("Tau ID discriminants:"
		    << " jet " << tau->discriminant(xAOD::TauJetParameters::BDTJetScore)
		    << " el " << tau->discriminant(xAOD::TauJetParameters::BDTEleScore));
		    //<< " mu " << tau->flag(xAOD::TauJetParameters::MuonFlag)); // ASM 18/4/2016 - not in Tau EDM anymore

    if(tau->pt()<m_tau_minPt || fabs(tau->eta())>m_tau_maxEta) return false;
    // need to accommodate more than one of these?
    if(!tau->isTau( xAOD::TauJetParameters::IsTauFlag(m_tau_isTauFlag) )) return false;
    // for now, do jet, el, mu discrimination separately.
    if(m_tau_doElVeto)
      if(tau->isTau( xAOD::TauJetParameters::IsTauFlag(m_tau_elVeto) )) return false;
    if(m_tau_doMuVeto)
      if(tau->isTau( xAOD::TauJetParameters::IsTauFlag(m_tau_muVeto) )) return false;

    return true;
  }

  bool METTauTool::resolveOverlap(const xAOD::IParticle* object,
				  xAOD::MissingETComponentMap* metMap,
				  std::vector<const xAOD::IParticle*>& acceptedSignals,
				  MissingETBase::Types::weight_t& objWeight) const
  {
    if(object->type() != xAOD::Type::Tau) {
      ATH_MSG_WARNING("METTauTool::resolveOverlap given an object of type " << object->type());
      return false;
    }
    const TauJet* tau = static_cast<const TauJet*>(object);

    ATH_MSG_VERBOSE("Retrieving tau constituents.");
    /////////////////////////////////////////// TO-BE REMOVED!!!
    /////////////////////////////////////////// TO-BE REMOVED!!!
    ////// <<<===== OLD TAU EDM : ASM 19/4/2016
    //const Jet* jet = *tau->jetLink();
    //JetConstituentVector constit = jet->getConstituents();
    //ATH_MSG_VERBOSE("Current tau has " << constit.size() << " constituents.");
    // first get the topoclusters
    // for now take them from the linked jet and apply a dR cone of 0.2
    // test for used topoclusters, and retrieve unused ones (ok until/unless we use PFlow taus)
    // only use clusters for computing the overlap removal relative to other objects
    //double sumE_allclus = 0.;
    //std::vector<const IParticle*> constit_vec;
    //CaloClusterChangeSignalStateList stateHelperList;
    //for(JetConstituentVector::const_iterator iClus = constit.begin();
    //	iClus!=constit.end(); ++iClus) {
    //  // TEMP: use jet seed axis
    //  //       taus will provide an accessor
    //  if(!xAOD::P4Helpers::isInDeltaR(*jet,*(*iClus)->rawConstituent(),0.2,m_useRapidity)) continue;
    //  // skip cluster if dR>0.2
    //  sumE_allclus += (*iClus)->e();
    //  if((*iClus)->rawConstituent()->type() != xAOD::Type::CaloCluster) {
	//ATH_MSG_WARNING("Unexpected jet constituent type " << (*iClus)->rawConstituent()->type() << " received! Skip.");
	//continue;
    //  }
    //  const CaloCluster* pClus = static_cast<const CaloCluster*>( (*iClus)->rawConstituent() );
    //  // create a helper to change the signal state and retain it until the end of the execute
    //  // signal state will be reset when it goes out of scope
    //  //CaloClusterChangeSignalState stateHelper(pClus, CaloCluster::State(m_signalstate));
    //  stateHelperList.add(pClus, CaloCluster::State(m_signalstate));
    //  constit_vec.push_back(pClus);
    //} // loop over jet constituents
    ////// <<<===== OLD TAU EDM : ASM 19/4/2016
    /////////////////////////////////////////// TO-BE REMOVED!!!
    /////////////////////////////////////////// TO-BE REMOVED!!!
    const CaloClusterContainer* modClusCont(0);
    if(m_useModClus) {
      ATH_CHECK( evtStore()->retrieve(modClusCont,m_mod_clus_key), false );
    }
    double sumE_allclus = 0.;
    std::vector<const IParticle*> constit_vec;
    CaloClusterChangeSignalStateList stateHelperList;
    for( ElementLink< xAOD::IParticleContainer > cluster_link : tau->clusterLinks() ){
      const xAOD::IParticle* ipart = *cluster_link;
      sumE_allclus += ipart->e();
      if(ipart->type() != xAOD::Type::CaloCluster) {
    	ATH_MSG_WARNING("Unexpected jet constituent type " << ipart->type() << " received! Skip.");
    	continue;
      }      
      // Link set in Reconstruction/tauRecTools/src/TauAxisSetter.cxx
      // Internal defaults are m_clusterCone = 0.2, m_doCellCorrection = false, m_doAxisCorrection = True
      const CaloCluster* pClus = static_cast<const CaloCluster*>( ipart );
      if(m_useModClus && modClusCont) {
	// replace with modified cluster
	pClus = (*modClusCont)[pClus->index()];
      }
      stateHelperList.add(pClus, CaloCluster::State(m_signalstate));
      constit_vec.push_back(pClus);
    }

    ATH_MSG_VERBOSE( "Tau E = " << tau->e() << ", cluster energy sum = " << sumE_allclus );
    bool clustersUsed = metMap->checkUsage(constit_vec,MissingETBase::UsageHandler::OnlyCluster);
    if(clustersUsed) { // true implies some cluster has been used
      double sumE_unique = 0.; // sumE or sumet?
      for(vector<const IParticle*>::const_iterator iClus = constit_vec.begin();
    	  iClus!=constit_vec.end(); ++iClus) {
    	sumE_unique += (*iClus)->e();
    	acceptedSignals.push_back(*iClus);
      } // loop over tau unique constituents
      double scalef = sumE_unique / sumE_allclus;
      // weight as an entire object with the unused E fraction
      // decision about what to do with the weight later
      if(scalef>1.) ATH_MSG_WARNING( "Computed a scale factor " << scalef << " larger than 1!" );
      ATH_MSG_VERBOSE( "Tau unique cluster energy sum = " << sumE_unique );
      objWeight = MissingETBase::Types::weight_t(scalef,scalef,scalef);
    } else {

      ATH_MSG_VERBOSE( "Tau has no overlaps." );
      // weight as an entire object with the unused E fraction
      // decision about what to do with the weight later
      acceptedSignals = constit_vec;
      objWeight = MissingETBase::Types::weight_t(1.,1.,1.);
    }

    if(m_tau_doTracks) {
      // now find associated tracks
      // TEMP: get all tau core tracks 
      //       add other tracks (from seed jet) in 0.2 cone
      vector<const IParticle*> tautracks;
      /////////////////////////////////////////// TO-BE REMOVED!!!
      /////////////////////////////////////////// TO-BE REMOVED!!!
      ////// <<<===== OLD TAU EDM : ASM 18/4/2016
      //for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
      //  tautracks.push_back(tau->track(iTrk));
      //}
      //for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
      //  const xAOD::TrackParticle* trk = tau->otherTrack(iTrk);
      //  if(xAOD::P4Helpers::isInDeltaR(*jet,*trk,0.2,m_useRapidity)) tautracks.push_back(trk);
      //}
      ////// <<<===== OLD TAU EDM : ASM 18/4/2016
      /////////////////////////////////////////// TO-BE REMOVED!!!
      /////////////////////////////////////////// TO-BE REMOVED!!!
      for( const TauTrack* ttrk : tau->tracks(xAOD::TauJetParameters::coreTrack) ){//all tracks dR < 0.2 regardless of quality
        tautracks.push_back(ttrk->track());
      }
      // test for used tracks, and retrieve unused ones
      metMap->checkUsage(tautracks,MissingETBase::UsageHandler::OnlyTrack);
      for(vector<const IParticle*>::const_iterator iTrk = tautracks.begin();
        iTrk!=tautracks.end(); ++iTrk) {
        acceptedSignals.push_back(*iTrk);
      } // loop over tau tracks
    }

    return !clustersUsed; // return true if the tau shares no clusters with another object
  }

  StatusCode METTauTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const TauJetContainer* tauCont = 0;
    if( evtStore()->retrieve(tauCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input tau container");
      return StatusCode::SUCCESS;
    }

    MissingETBase::Types::bitmask_t source = MissingETBase::Source::tau();
    metTerm->setSource(source);

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::contributedHardTerm());

    ATH_MSG_DEBUG("Retrieved tau collection: " << m_input_data_key);

    std::vector<const IParticle*> signalList;
    signalList.reserve(15*tauCont->size());
    for(TauJetContainer::const_iterator iTau=tauCont->begin();
	iTau!=tauCont->end(); ++iTau) {
      if ( this->accept(*iTau) ) {
	ATH_MSG_VERBOSE("Accepted tau with "
			<< "pT = " << (*iTau)->pt()
			<< " eta = " << (*iTau)->eta()
			<< " phi = " << (*iTau)->phi());
	signalList.clear();
	MissingETBase::Types::weight_t objWeight;
	if ( this->resolveOverlap(*iTau,metMap,signalList,objWeight) ) {
	  // add if not overlapping anything
	  this->addToMET(*iTau,signalList,metTerm,metMap,objWeight);
	} else { // no overlaps
	  if( m_tau_doMinWetPtCut ) {
            if( (*iTau)->pt()*objWeight.wet() > m_tau_minPt ) {
              ATH_MSG_VERBOSE("Tau weighted energy is above threshold -- add to MET");
	      this->addToMET(*iTau,signalList,metTerm,metMap,objWeight);
	      //ATH_MSG_VERBOSE("Tau px = " << (*iTau)->px()
              //             << " , weighted px = " << (*iTau)->px()*objWeight.wpx()
              //             << " ,MET px = " << metTerm->mpx() );
            } // if passes weighted pt cut
          } else {
	    if ( objWeight.wet() > m_tau_minWet ) {
              ATH_MSG_VERBOSE("Tau weighted energy is above threshold -- add to MET");
	      this->addToMET(*iTau,signalList,metTerm,metMap,objWeight);
	      //ATH_MSG_VERBOSE("Tau px = " << (*iTau)->px()
              //             << " , weighted px = " << (*iTau)->px()*objWeight.wpx()
              //             << " ,MET px = " << metTerm->mpx() );
            } // if passes weight cut
          } // end if do weighted pt cut
	} // overlapping
      } // if passing selection
    } // loop on tau container
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
