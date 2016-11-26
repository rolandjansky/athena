///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaTool.cxx 
// Implementation file for class METEgammaTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <algorithm>

// METReconstruction includes
#include "METReconstruction/METEgammaTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Egamma EDM
#include "xAODEgamma/EgammaContainer.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Egamma;
  using xAOD::EgammaContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::TrackParticle;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  static bool greaterPt(const xAOD::IParticle* part1, const xAOD::IParticle* part2) {
    return part1->pt()>part2->pt();
  }

  // Constructors
  ////////////////
  METEgammaTool::METEgammaTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {

    declareProperty( "PIDSel",            m_eg_pid         = ""     ); // Selection string to be determined
    declareProperty( "AuthorSel",         m_eg_author = xAOD::EgammaParameters::AuthorALL ); // default to accept Electron, Photon or Ambiguous
    declareProperty( "MinPt",             m_eg_minPt       = 10e3   ); // use GeV const?
    declareProperty( "MaxEta",            m_eg_maxEta      = 2.47   );
    declareProperty( "RejectCrack",       m_eg_rejectCrack = false  );
    declareProperty( "CrackEtaLow",       m_eg_crkEtaLo    = 1.37   ); // range covered by the Tile Gap scintillator
    declareProperty( "CrackEtaHigh",      m_eg_crkEtaHi    = 1.56   );
    declareProperty( "ClusOQ",            m_eg_clusOQ      = 0x0    );
    declareProperty( "TestClusOQ",        m_eg_testClusOQ  = false  ); // could e.g. veto BADCLUSELECTRON

    declareProperty( "TopoClusKey",       m_tcCont_key = "CaloCalTopoClusters" );
    declareProperty( "TCMatchDeltaR",     m_tcMatch_dR        = 0.1 );
    declareProperty( "TCMatchMaxRat",     m_tcMatch_maxRat    = 1.5 );
    declareProperty( "TCMatchTolerance",  m_tcMatch_tolerance = 0.2 );
    declareProperty( "TCMatchMethod",     m_tcMatch_method    = 0   );

    declareProperty( "DoTracks",          m_eg_doTracks  = true     );
  }

  // Destructor
  ///////////////
  METEgammaTool::~METEgammaTool()
  {} 

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METEgammaTool::initialize()
  {
    ATH_CHECK(  METBuilderTool::initialize() );
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }


  StatusCode METEgammaTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  bool METEgammaTool::accept(const xAOD::IParticle* object) const
  {

    if(object->type() != xAOD::Type::Electron && object->type() != xAOD::Type::Photon) {
      ATH_MSG_WARNING("METEgammaTool::accept given an object of type " << object->type());
      return false;
    }
    const Egamma* eg = static_cast<const Egamma*>(object);

    ATH_MSG_VERBOSE("Test egamma quality." 
		    << " pT = " << eg->pt()
		    << " eta = " << eg->eta()
		    << " phi = " << eg->phi());

    bool testPID = 0;
    eg->passSelection(testPID,m_eg_pid);
    ATH_MSG_VERBOSE("Egamma PID " << m_eg_pid << " tests " << (testPID ? " GOOD" : "BAD") );
    if( !testPID ) return false;

    /////////////////////////////////////////////////////////////////
    // FIXME
    // Test that egamma author satisfies one of the permitted algorithms
    // Maybe doesn't need to be configurable
    // For now default to author 1, i.e. standard egamma alg
    ATH_MSG_VERBOSE("Egamma author = " << eg->author() << " test " << (eg->author()&m_eg_author));
    if( !(eg->author()&m_eg_author) ) return false;
    /////////////////////////////////////////////////////////////////

    if( eg->pt()<m_eg_minPt ) return false;
    if( fabs(eg->eta())>m_eg_maxEta ) return false;

    /////////////////////////////////////////////////////////////////
    // FIXME
    // We could potentially avoid calibrating crack egammas
    if( m_eg_rejectCrack ) {
      if( fabs(eg->eta())>m_eg_crkEtaLo &&
	  fabs(eg->eta())<m_eg_crkEtaHi ) return false;
    }
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    // FIXME
    // We could veto egammas with poor cluster quality
    if( m_eg_testClusOQ ) {
      if( !eg->isGoodOQ(m_eg_clusOQ) ) return false;
    }
    /////////////////////////////////////////////////////////////////

    ATH_MSG_VERBOSE("Accepted this egamma");

    return true;
  }

  void METEgammaTool::matchTopoClusters(const xAOD::Egamma* eg, std::vector<const xAOD::IParticle*>& tclist,
					const xAOD::CaloClusterContainer* tcCont) const
  {
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    vector<const xAOD::CaloCluster*> nearbyTC;
    nearbyTC.reserve(10);
    for(CaloClusterContainer::const_iterator iClus=tcCont->begin();
	iClus!=tcCont->end(); ++iClus) {
      // this can probably be done more elegantly
      if(xAOD::P4Helpers::isInDeltaR(*swclus,**iClus,0.1,m_useRapidity) && (*iClus)->e()>0) {
	// could consider also requirements on the EM fraction or depth
	nearbyTC.push_back(*iClus);
      } // match TC in a cone around SW cluster
    }
    ATH_MSG_VERBOSE("Found " << nearbyTC.size() << " nearby topoclusters");

    bool goodmatch = false;
    bool doSum = true;
    double sumE_tc = 0.;
    const CaloCluster* bestbadmatch = 0;
    std::sort(nearbyTC.begin(),nearbyTC.end(),greaterPt);
    for(vector<const xAOD::CaloCluster*>::const_iterator iClus=nearbyTC.begin();
	iClus!=nearbyTC.end(); ++iClus) {
      double tcl_e = (*iClus)->e();
      // skip cluster if it's above our bad match threshold
      if(tcl_e>m_tcMatch_maxRat*eg_cl_e) {
	ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (tcl_e/eg_cl_e));
	if( !bestbadmatch || (fabs(tcl_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = *iClus;
	continue;
      }

      switch(m_tcMatch_method) {
      case 0:
	// sum clusters until the next cluster to be added will make the energy match worse
	doSum = ( fabs(sumE_tc+tcl_e - eg_cl_e) < fabs(sumE_tc - eg_cl_e) );
	ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
	break;
      case 1:
	// sum clusters until we either find one very good cluster match
	// or reach a specified cutoff
	doSum = (!goodmatch && (sumE_tc+tcl_e) < m_tcMatch_maxRat*eg_cl_e );
	break;
      }

      if(doSum) {
	tclist.push_back(*iClus);
	sumE_tc += tcl_e;
	if(tclist.size()==1) goodmatch = fabs(tcl_e/eg_cl_e-1)<m_tcMatch_tolerance;
	ATH_MSG_VERBOSE("Accept topocluster with pt " << (*iClus)->pt() << ", e " << (*iClus)->e() << " in sum.");
	ATH_MSG_VERBOSE("Energy ratio of TC to eg: " << tcl_e / eg_cl_e);
	ATH_MSG_VERBOSE("Do we have a good match? " << (goodmatch ? "YES" : "NO"));
      } // if we will retain the topocluster
    } // loop over nearby clusters
    if(sumE_tc<1e-9 && bestbadmatch) {
      tclist.push_back(bestbadmatch);
      sumE_tc += bestbadmatch->e();
    }
    ATH_MSG_VERBOSE("Egamma links " << eg->nCaloClusters() << " clusters");
    ATH_MSG_VERBOSE("Identified " << tclist.size() << " matched topoclusters");
    ATH_MSG_VERBOSE("Egamma energy: " << eg->e());
    ATH_MSG_VERBOSE("Egamma cluster energy: " << swclus->e());
    ATH_MSG_VERBOSE("Sum of nearby topocluster energies: " << sumE_tc);
    ATH_MSG_VERBOSE("Energy ratio of TC sum to eg: " << sumE_tc / swclus->e());
  }

  // In case any common treatment is needed for egammas in addition to the electron/photon specialised versions
  void METEgammaTool::matchExtraTracks(const xAOD::Egamma* /*eg*/, std::vector<const xAOD::IParticle*>& trklist) const
  {
    ATH_MSG_VERBOSE("Egamma has " << trklist.size() << " linked tracks");
  }

}
