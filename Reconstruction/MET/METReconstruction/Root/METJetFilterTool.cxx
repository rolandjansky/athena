///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetFilterTool.cxx 
// Implementation file for class METJetFilterTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METJetFilterTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Jet EDM
#include "xAODJet/JetTypes.h"
#include "xAODJet/JetAttributes.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Jet;
  using xAOD::JetAttribute;
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
  METJetFilterTool::METJetFilterTool(const std::string& name) : 
    AsgTool(name),
    METRefinerTool(name)
  {
    declareProperty( "DoJVFCut",       m_jet_doJVFCut  = false );
    declareProperty( "MinAbsJVF",      m_jet_minAbsJVF = 0.25  );
    declareProperty( "MaxPtJVF",       m_jet_maxPtJVF  = 50e3  );
    declareProperty( "MaxEtaJVF",      m_jet_maxEtaJVF = 2.4   );
  }

  // Destructor
  ///////////////
  METJetFilterTool::~METJetFilterTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METJetFilterTool::initialize()
  {
    ATH_CHECK( METRefinerTool::initialize() );
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METJetFilterTool::finalize()
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

  bool METJetFilterTool::isHSJet(const xAOD::Jet* jet) const
  {

    // switch to more generic HS jet selector when available
    if( m_jet_doJVFCut ) {
      if( jet->pt() < m_jet_maxPtJVF &&
	  fabs(jet->eta()) < m_jet_maxEtaJVF ) {
	vector<float> jvf;
	jet->getAttribute<vector<float> >(JetAttribute::JVF,jvf);
	if(!jet->getAttribute<vector<float> >(JetAttribute::JVF,jvf)) {
	  ATH_MSG_WARNING("Jet JVF unavailable!");
	  return false;
	}
	ATH_MSG_VERBOSE("Jet JVF = " << jvf[0]);
	if( fabs(jvf[0]) < m_jet_minAbsJVF ) return false;
      }
    }

    return true;
  }

  StatusCode METJetFilterTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::Tags::correctedTerm(MissingETBase::Status::Nominal,
								      MissingETBase::Status::PileupJetVertex));

    // Extract the component corresponding to the Jet SoftTerms
    MissingETBase::Types::bitmask_t src_refJet = MissingETBase::Source::jet();
    MissingETComponentMap::const_iterator citer = MissingETComposition::find(metMap,src_refJet);
    if(citer==metMap->end()) {
      ATH_MSG_WARNING("Could not find uncorrected Jet component in MET Map!");
      return StatusCode::SUCCESS;
    }

    metTerm->setSource(src_refJet);

    vector<const IParticle*> jetList = (*citer)->objects();
    vector<const IParticle*> dummyList; // jet constituents are already in the map

    // Loop over the jets and select only good ones
    for( const auto& obj : jetList ) {
      MissingETBase::Types::weight_t jetWeight = (*citer)->weight(obj);
      if(obj->type() != xAOD::Type::Jet) {
        ATH_MSG_WARNING("Retrieved an object of type " << obj->type() << " while expecting xAOD::Jet");
        continue;
      }
      const Jet* jet = static_cast<const Jet*>(obj);
      ATH_MSG_VERBOSE("Filter jet with pt " << jet->pt());
      // Could/should use common implementation of addToMET here -- derive builder and refiner from a common base tool?
      bool passFilters = true;
      if(!isHSJet(jet)) passFilters = false;
      if(passFilters) {
	ATH_MSG_VERBOSE("Add to MET.");
	metTerm->add(jet->px()*jetWeight.wpx(),
		     jet->py()*jetWeight.wpy(),
		     jet->pt()*jetWeight.wet());
	MissingETComposition::insert(metMap,metTerm,jet,dummyList,jetWeight);
      }
    }

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}

