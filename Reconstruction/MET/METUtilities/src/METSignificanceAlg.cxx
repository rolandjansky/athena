/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSignificanceAlg.cxx

#include "METSignificanceAlg.h"
#include "METInterface/IMETSignificance.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"

using std::string;
using namespace xAOD;

namespace met {

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");

  //**********************************************************************

  METSignificanceAlg::METSignificanceAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator )
 {
    declareProperty( "Significance",   m_metsignif                       );
    declareProperty( "METMapName",     m_mapname   = "METAssoc"          );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty( "METName",        m_outname   = "MET_Reference"     );

    declareProperty( "METSoftClName",  m_softclname  = "SoftClus"        );
    declareProperty( "METSoftTrkName", m_softtrkname = "PVSoftTrk"       );

    declareProperty( "InputJets",      m_jetColl   = "AntiKt4LCTopoJets" );
    declareProperty( "InputElectrons", m_eleColl   = "Electrons"         );
    declareProperty( "InputPhotons",   m_gammaColl = "Photons"           );
    declareProperty( "InputTaus",      m_tauColl   = "TauJets"           );
    declareProperty( "InputMuons",     m_muonColl  = "Muons"             );

    declareProperty( "DoTruthLeptons", m_doTruthLep = false              );

  }

  //**********************************************************************

  METSignificanceAlg::~METSignificanceAlg() { }

  //**********************************************************************

  StatusCode METSignificanceAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // retrieve tools
    if( m_metsignif.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_metsignif->name());
      return StatusCode::FAILURE;
    };

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METSignificanceAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METSignificanceAlg::execute() {
    ATH_MSG_VERBOSE("Executing " << name() << "...");

    // Create a MissingETContainer with its aux store
    MissingETContainer* newMet = new MissingETContainer();
    if( evtStore()->record(newMet, m_outname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_outname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_outname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_outname+"Aux.");
      return StatusCode::SUCCESS;
    }
    newMet->setStore(metAuxCont);

    const MissingETAssociationMap* metMap = 0;
    if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    metMap->resetObjSelectionFlags();

    // Retrieve containers ***********************************************

    /// MET
    const MissingETContainer* coreMet(0);
    if( evtStore()->retrieve(coreMet, m_corename).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_corename);
      return StatusCode::SUCCESS;
    }

    MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
    if((*newMet)[m_softtrkname]) trksource = (*newMet)[m_softtrkname]->source();
    MissingETBase::Types::bitmask_t clsource = MissingETBase::Source::LCTopo;
    if((*newMet)[m_softclname]) clsource = (*newMet)[m_softclname]->source();

    return StatusCode::SUCCESS;
  }

  //**********************************************************************
}
