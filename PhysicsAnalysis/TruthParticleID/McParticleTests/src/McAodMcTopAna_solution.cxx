///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McTopAna.cxx 
// Implementation file for class McTopAna
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// McParticleTests includes
#include "McAodMcTopAna_solution.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// mc tools
#include "McParticleKernel/IMcVtxFilterTool.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "McParticleUtils/McVtxFilter.h"

#include "EventKernel/PdtPdg.h"

// units
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT includes
#include "TH1F.h"

namespace McAod {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McTopAnaSolution::McTopAnaSolution( const std::string& name, 
                    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_vtxFilter("McVtxFilterTool/McVtxFilterTool", this),
  m_cnvTool("TruthParticleCnvTool/TruthParticleCnvTool", this),
  m_mcEventsName ("GEN_EVENT"),
  m_filteredMcEventsName ("MyGEN_EVENT"),
  m_filteredMcParticlesName ("MyTruthParticles"),
  m_h_mctop_mass(0),
  m_h_mcwqq_mass(0),
  m_h_mcq1_ene(0),
  m_h_mcq2_ene(0)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("FilterTool", m_vtxFilter, 
                  "handle to the mc-vtx filter tool");

  declareProperty("CnvTool", m_cnvTool, 
                  "handle to the truthparticle cnv tool");

  declareProperty("McEvent",
                  m_mcEventsName,
                  "input McEventCollection container name");

  declareProperty("FilteredMcEvent",
                  m_filteredMcEventsName,
                  "output filtered McEventCollection container name");

  declareProperty("FilteredMcParticles",
                  m_filteredMcParticlesName,
                  "output filtered TruthParticles container name");
}

// Destructor
///////////////
McTopAnaSolution::~McTopAnaSolution()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode McTopAnaSolution::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_vtxFilter.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve vtx filter tool");
    return StatusCode::FAILURE;
  } else {
    m_vtxFilter->displayOptions();
  }

  if (!m_cnvTool.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve truthparticle cnv tool");
    return StatusCode::FAILURE;
  }

  // handle to THistSvc
  ServiceHandle<ITHistSvc> hsvc("THistSvc", this->name());
  if (!hsvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve histogram svc");
    return StatusCode::FAILURE;
  }

  // register mc-top histogram
  m_h_mctop_mass = new TH1F("mctop_mass", "Mc Top mass",
                            200, 0.*CLHEP::GeV, 300.*CLHEP::GeV);
  if (!hsvc->regHist("/mcaod/McTopWb/mctop_mass", m_h_mctop_mass).isSuccess()) {
    ATH_MSG_ERROR("could not register histogram");
    return StatusCode::FAILURE;
  }

  // register mc-w histogram
  m_h_mcwqq_mass = new TH1F("mcwqq_mass", "Mc W mass",
                            100, 0.*CLHEP::GeV, 100.*CLHEP::GeV);
  if (!hsvc->regHist("/mcaod/McTopWb/mcwqq_mass", m_h_mcwqq_mass).isSuccess()) {
    ATH_MSG_ERROR("could not register histogram");
    return StatusCode::FAILURE;
  }

  // register mc-q1 histogram
  m_h_mcq1_ene = new TH1F("mcq1_ene", "Mc q1 energy",
                            100, 0.*CLHEP::GeV, 500.*CLHEP::GeV);
  if (!hsvc->regHist("/mcaod/McTopWb/mcq1_ene", m_h_mcq1_ene).isSuccess()) {
    ATH_MSG_ERROR("could not register histogram");
    return StatusCode::FAILURE;
  }

  // register mc-q2 histogram
  m_h_mcq2_ene = new TH1F("mcq2_ene", "Mc q2 energy",
                            100, 0.*CLHEP::GeV, 500.*CLHEP::GeV);
  if (!hsvc->regHist("/mcaod/McTopWb/mcq2_ene", m_h_mcq2_ene).isSuccess()) {
    ATH_MSG_ERROR("could not register histogram");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode McTopAnaSolution::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  // display some stats
  m_vtxFilter->stats();

  // histo stats
  ATH_MSG_INFO
    ("-- McTopMass --" << endmsg
     << "\tentries= " << m_h_mctop_mass->GetEntries() << endmsg
     << "\tmean=    " << m_h_mctop_mass->GetMean() / CLHEP::GeV << " GeV/c2" << endmsg
     << "\tRMS=     " << m_h_mctop_mass->GetRMS()  / CLHEP::GeV << " GeV/c2");

  return StatusCode::SUCCESS;
}

StatusCode McTopAnaSolution::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if (!doMcTopWb().isSuccess()) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode McTopAnaSolution::doMcTopWb()
{
  // retrieve input data
  const McEventCollection *mcevt = 0;
  if (!evtStore()->retrieve(mcevt, m_mcEventsName).isSuccess() ||
      0 == mcevt) {
    ATH_MSG_INFO("could not retrieve mc collection at [" 
                 << m_mcEventsName << "]!");
    return StatusCode::FAILURE;
  }

  if (mcevt->empty()) {
    ATH_MSG_INFO("mc collection at [" << m_mcEventsName << "] is empty");
    return StatusCode::SUCCESS;
  }

  // create filtered container and give ownership to evtstore
  McEventCollection *filteredMcEvt = new McEventCollection;
  if (!evtStore()->record(filteredMcEvt, m_filteredMcEventsName).isSuccess()) {
    ATH_MSG_INFO("could not record filtered mc collection at ["
                 << m_filteredMcEventsName << "]!");
    // clean-up...
    delete filteredMcEvt; filteredMcEvt = 0;
    return StatusCode::FAILURE;
  }

  // Tell StoreGate that other algorithms won't be able to change the content of this container
  if (evtStore()->setConst(filteredMcEvt).isFailure() ) {
    ATH_MSG_WARNING("Could not lock the McEventCollection at ["
                    << m_filteredMcEventsName << "] !!"
                    << endmsg
                    << "Downstream algorithms will be able to alter it...");
  }

  // Filter the input collection and fill the output one
  m_vtxFilter->filterMcEventCollection(mcevt, filteredMcEvt);

  const HepMC::GenEvent * evtAod = filteredMcEvt->front();
  for ( HepMC::GenEvent::particle_const_iterator itr = evtAod->particles_begin();
        itr != evtAod->particles_end();
        ++itr ) {
    ATH_MSG_DEBUG("Part. id: " << (*itr)->pdg_id() << endmsg
                  << "E= "        << (*itr)->momentum().e()
                  << "\tpx= "     << (*itr)->momentum().px());

    // retrieve the decay vertex of the current particle
    const HepMC::GenVertex * decayVtx = (*itr)->end_vertex();

    if (PDG::t == (*itr)->pdg_id() && //> select top
        0      !=  decayVtx        && //> check that we have a valid vtx pointer
        2      <=  decayVtx->particles_out_size() ) { //> isn't necessary, just to exercize the GenVertex interface
      m_h_mctop_mass->Fill( (*itr)->momentum().m() );
    }//> top
  }//> end loop over particles


  // create an output TruthParticleContainer
  TruthParticleContainer *mcparts = new TruthParticleContainer;
  if (!evtStore()->record(mcparts, m_filteredMcParticlesName).isSuccess()) {
    ATH_MSG_INFO("could not record filtered truthparticles at ["
                 << m_filteredMcParticlesName << "]!");
    // clean-up...
    delete mcparts; mcparts = 0;
    return StatusCode::FAILURE;
  }

  // Tell StoreGate that other algorithms won't be able to change the content of this container
  if (evtStore()->setConst(mcparts).isFailure() ) {
    ATH_MSG_WARNING("Could not lock the TruthParticleContainer at ["
                    << m_filteredMcParticlesName << "] !!"
                    << endmsg
                    << "Downstream algorithms will be able to alter it...");
  }
  
  // perform the conversion
  if (!m_cnvTool->convert(filteredMcEvt, mcparts).isSuccess()) {
    ATH_MSG_WARNING("could not convert [" << filteredMcEvt << "] collection "
                    "into a TruthParticleContainer at [" 
                    << m_filteredMcParticlesName << "] !");
    return StatusCode::FAILURE;
  }

  McVtxFilter wqqFilter;
  wqqFilter.setDecayPattern( "24 -> 1|2|3|4|5|-1|-2|-3|-4|-5 + 1|2|3|4|5" );

  for ( TruthParticleContainer::const_iterator itr = mcparts->begin();
        itr != mcparts->end();
        ++itr ) {
    const TruthParticle* mc = *itr;
    if ( PDG::W_plus  != mc->pdgId() &&
         PDG::W_minus != mc->pdgId() ) {
      // not interested in... Skip it
      continue;
    }
    const HepMC::GenParticle* hepPart = mc->genParticle();
    const HepMC::GenVertex* vtx = hepPart->end_vertex();
    // Get the W boson
    if ( 0 != vtx &&
         wqqFilter.isAccepted( vtx ) ) {
      m_h_mcwqq_mass->Fill( mc->m() );
      const TruthParticle * q1 = mc->child(0);
      const TruthParticle * q2 = mc->child(1);
      // note that further treatment should be done
      // in case of gluons
      m_h_mcq1_ene->Fill( q1->e() );
      m_h_mcq2_ene->Fill( q2->e() );

    }//> is a W->qq'
  }//> end loop over TruthParticles

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

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace McAod
