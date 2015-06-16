/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "DataModel/ElementLink.h"

// EDM include(s):
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "DerivationFrameworkMCTruth/xAODtoHepMCCnvAlg.h"

namespace DerivationFramework {
    
    xAODtoHepMCCnvAlg::xAODtoHepMCCnvAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_momFac(1.), //<-- MeV/GeV conversion factor
    m_lenFac(1.), //<-- length conversion factor
    m_xaodTruthEventKey("TruthEvent"),
    m_outKey("GEN_EVENT"),
    m_signalOnly(true),
    m_maxCount(0)
    {
        declareProperty("xAODTruthEventKey", m_xaodTruthEventKey);
        declareProperty("HepMCTruthEventKey", m_outKey);
        declareProperty("SignalOnly", m_signalOnly);
        declareProperty("MaxCount", m_maxCount);
    }
    

    StatusCode xAODtoHepMCCnvAlg::initialize() {
        
        ATH_MSG_INFO("Initializing xAODtoHepMCCnvAlg");
        ATH_MSG_INFO("xAODTruthEventKey  = " <<m_xaodTruthEventKey );
        ATH_MSG_INFO("HepMCTruthEventKey = " <<m_outKey);
        ATH_MSG_INFO("SignalOnly         = " <<m_signalOnly);

        m_evtCount = -1;
        m_badSuggest = 0;
        m_noProdVtx = 0;

        return StatusCode::SUCCESS;
    }
    

    StatusCode xAODtoHepMCCnvAlg::finalize() {

        if( m_badSuggest ){
          ATH_MSG_WARNING("suggest_barcode failure count " <<m_badSuggest);
          ATH_MSG_WARNING("Check input particle/vertex barcodes.");
        } else {
          ATH_MSG_INFO("No suggest_barcode failures");
        }
        ATH_MSG_INFO("Missing production vertex count " <<m_noProdVtx);
        ATH_MSG_INFO("xAODTruthCnvAlg misses initial vertices(?)");

        return StatusCode::SUCCESS;
    }
    
    
    StatusCode xAODtoHepMCCnvAlg::execute() {
        
        ++m_evtCount;
        bool doPrint = m_evtCount < m_maxCount;

        // Retrieve the xAOD truth
        const xAOD::TruthEventContainer* xTruthEventContainer = NULL;
        CHECK( evtStore()->retrieve( xTruthEventContainer, m_xaodTruthEventKey));
        
        // CREATE HEPMC EVENT COLLECTION
        std::unique_ptr<McEventCollection> mcEventCollection(new McEventCollection());
        
        // Loop over xAOD truth container
        // Signal event is always first, followed by pileup events
        ATH_MSG_DEBUG("Start event loop");
        for (const xAOD::TruthEvent* xAODEvent : *xTruthEventContainer) {

            if( doPrint ) ATH_MSG_DEBUG("XXX Printing xAOD Event");
            if( doPrint ) printxAODEvent( xAODEvent );
            // Create GenEvent for each xAOD truth event
            ATH_MSG_DEBUG("Create new GenEvent");
            HepMC::GenEvent* hepmcEvent = new HepMC::GenEvent(createHepMCEvent(xAODEvent));
            // Insert into McEventCollection
            mcEventCollection->push_back(hepmcEvent);

            if( doPrint ) ATH_MSG_DEBUG("XXX Printing HepMC Event");
            if( doPrint ) hepmcEvent->print();

            // Quit if signal only
            if( m_signalOnly ) break;
        }
        
        // SAVE TO STOREGATE
        CHECK(evtStore()->record(move(mcEventCollection),m_outKey));
        
        return StatusCode::SUCCESS;
        
    }
    
}


    const HepMC::GenEvent DerivationFramework::xAODtoHepMCCnvAlg::createHepMCEvent(
                          const xAOD::TruthEvent* xEvt) {

    ///EVENT LEVEL
    HepMC::GenEvent genEvt;

    // Hard-coded EventInfo key to get event number.
    const xAOD::EventInfo* eventInfo = 0;
    if( !evtStore()->retrieve( eventInfo, "EventInfo").isSuccess() ){
      ATH_MSG_ERROR("No xAOD::EventInfo found with name EventInfo");
      return StatusCode::FAILURE;
    }
    long long int evtNum = eventInfo->eventNumber();    
    genEvt.set_event_number(evtNum);
    ATH_MSG_DEBUG("Start createHepMCEvent for event " <<evtNum);
    
    //Beam particles
    std::pair<const xAOD::TruthParticle*,const xAOD::TruthParticle*> beamParticles = xEvt->beamParticles();
    if( beamParticles.first && beamParticles.second ) {
        std::pair<HepMC::GenParticle*,HepMC::GenParticle*> HepMC_beamParticles;
        HepMC_beamParticles.first =  new HepMC::GenParticle(
                                     HepMC::FourVector(
                                       m_momFac * beamParticles.first->px(), 
                                       m_momFac * beamParticles.first->py(),
                                       m_momFac * beamParticles.first->pz(), 
                                       m_momFac * beamParticles.first->e() ),
                                     beamParticles.first->pdgId(), 
                                     beamParticles.first->status());
        HepMC_beamParticles.second = new HepMC::GenParticle(
                                     HepMC::FourVector( 
                                       m_momFac * beamParticles.second->px(), 
                                       m_momFac * beamParticles.second->py() ,
                                       m_momFac * beamParticles.second->pz(), 
                                       m_momFac * beamParticles.second->e() ),
                                     beamParticles.second->pdgId(), 
                                     beamParticles.second->status());
        
        genEvt.set_beam_particles(HepMC_beamParticles);
    }
    
    // These are NOT set by xAODTruthCnvAlg -- FIXME
    //genEvt.set_event_scale((double)xEvt->eventScale());
    //genEvt.set_alphaQED((double)xEvt->alphaQED());
    //genEvt.set_alphaQCD((double)xEvt->alphaQCD());

    // PDF information accessors not in xAOD::EventInfo -- FIXME

    // Heavy ion information not in xAOD::EventInfo -- FIXME
    
    //Cross section <- Can't see this read in Reader
    // Need to look in xTruthEvent
    HepMC::GenCrossSection* xsec = new HepMC::GenCrossSection();
    xsec->set_cross_section((double)xEvt->crossSection());
    xsec->set_cross_section_error((double)xEvt->crossSectionError());
    
    //Weights
    const std::vector<float> weights = xEvt->weights();
    for ( std::vector<float>::const_iterator wgt = weights.begin();
        wgt != weights.end(); ++wgt ) { genEvt.weights().push_back(*wgt); }
    
    
    // PARTICLES AND VERTICES
    
    // Map of existing vertices - needed for the tree linking
    std::map<const xAOD::TruthVertex*,HepMC::GenVertex*> vertexMap;

    // Loop over all of the particles in the event, call particle builder
    // Call suggest_barcode only after insertion!

    int nTruthParticles = xEvt->nTruthParticles();
    ATH_MSG_DEBUG("Number of particles " <<nTruthParticles);

    for (int i=0; i<nTruthParticles; ++i) {
        const xAOD::TruthParticle* xPart = xEvt->truthParticle(i);
        if( !xPart->hasProdVtx() && !xPart->hasDecayVtx() ){
          ATH_MSG_WARNING("xAOD particle with no vertices, bc = "
                          <<xPart->barcode());
          continue;
        }

        // Create GenParticle
        HepMC::GenParticle* hepmcParticle( createHepMCParticle(xPart) );
        int bcpart = xPart->barcode();

        // Get the production and decay vertices
        if( xPart->hasProdVtx() ) {
            const xAOD::TruthVertex* xAODProdVtx = xPart->prodVtx();
            bool prodVtxSeenBefore(false); // is this new?
            HepMC::GenVertex* hepmcProdVtx = vertexHelper(xAODProdVtx,vertexMap,prodVtxSeenBefore);
            // Set the decay/production links
            hepmcProdVtx->add_particle_out(hepmcParticle);
            // Insert into Event
            if (!prodVtxSeenBefore){ 
                genEvt.add_vertex(hepmcProdVtx);
                if( !hepmcProdVtx->suggest_barcode(xAODProdVtx->barcode()) ){
                  ATH_MSG_WARNING("suggest_barcode failed for vertex "
                                  <<xAODProdVtx->barcode());
                  ++m_badSuggest;
                }
            }
            if( !hepmcParticle->suggest_barcode(bcpart) ){
                ATH_MSG_DEBUG("suggest_barcode failed for particle " <<bcpart);
                ++m_badSuggest;
            }
            bcpart = 0;
        } else {
            ATH_MSG_DEBUG("No production vertex found for particle "
                          <<xPart->barcode());
        }

        if( xPart->hasDecayVtx() ){
            const xAOD::TruthVertex* xAODDecayVtx = xPart->decayVtx();
            bool decayVtxSeenBefore(false); // is this new?
            HepMC::GenVertex* hepmcDecayVtx = vertexHelper(xAODDecayVtx,vertexMap,decayVtxSeenBefore);
            // Set the decay/production links
            hepmcDecayVtx->add_particle_in(hepmcParticle);
            // Insert into Event
            if (!decayVtxSeenBefore){ 
                genEvt.add_vertex(hepmcDecayVtx);
                if( !hepmcDecayVtx->suggest_barcode(xAODDecayVtx->barcode()) ){
                  ATH_MSG_WARNING("suggest_barcode failed for vertex "
                                  <<xAODDecayVtx->barcode());
                  ++m_badSuggest;
                }
            }
            if( bcpart != 0 ){
                if( !hepmcParticle->suggest_barcode(bcpart) ){
                    ATH_MSG_DEBUG("suggest_barcode failed for particle " <<bcpart);
                    ++m_badSuggest;
                }
                bcpart = 0;
            }
        }
    } // end of particle loop

    const HepMC::GenEvent constGenEvt(genEvt);
    ATH_MSG_DEBUG("Returning const GenEvent");
    return constGenEvt;

}

// Helper to check whether a vertex exists or not using a map; 
// calls createHepMCVertex if not

HepMC::GenVertex* DerivationFramework::xAODtoHepMCCnvAlg::vertexHelper(
                            const xAOD::TruthVertex* xaodVertex,
                            std::map<const xAOD::TruthVertex*,HepMC::GenVertex*> &vertexMap,
                            bool &seenBefore) {

    HepMC::GenVertex* hepmcVertex;
    std::map<const xAOD::TruthVertex*,HepMC::GenVertex*>::iterator vMapItr;
    vMapItr=vertexMap.find(xaodVertex);
    // Vertex seen before?
    if (vMapItr!=vertexMap.end()) {
            // YES: use the HepMC::Vertex already in the map
            hepmcVertex = (*vMapItr).second;
            seenBefore = true;
    } else {
            // NO: create a new HepMC::Vertex
            hepmcVertex = createHepMCVertex(xaodVertex);
            vertexMap[xaodVertex] = hepmcVertex;
            seenBefore = false;
    }

    return hepmcVertex;

}

// Create the HepMC GenParticle
// Call suggest_barcode after insertion!

HepMC::GenParticle* DerivationFramework::xAODtoHepMCCnvAlg::createHepMCParticle(const xAOD::TruthParticle* particle) {

    ATH_MSG_VERBOSE("Creating GenParticle for barcode " <<particle->barcode());
    const HepMC::FourVector fourVec( m_momFac * particle->px(), m_momFac * particle->py(), m_momFac * particle->pz(), m_momFac * particle->e() );
    HepMC::GenParticle* hepmcParticle = new HepMC::GenParticle(fourVec, particle->pdgId(), particle->status());
     hepmcParticle->set_generated_mass( m_momFac * particle->m());

     return hepmcParticle;
}

// Create the HepMC GenVertex
// Call suggest_barcode after insertion!

HepMC::GenVertex* DerivationFramework::xAODtoHepMCCnvAlg::createHepMCVertex(const xAOD::TruthVertex* vertex) {

    ATH_MSG_VERBOSE("Creating GenVertex for barcode " <<vertex->barcode());
    HepMC::FourVector prod_pos( m_lenFac * vertex->x(), m_lenFac * vertex->y(),m_lenFac * vertex->z(), m_lenFac * vertex->t() );
    HepMC::GenVertex* genVertex = new HepMC::GenVertex(prod_pos);

    return genVertex;
}




// Print xAODTruth Event. The printout is particle oriented, unlike the
// HepMC particle/vertex printout. Geant and pileup particles with
// barcode>100000 are omitted.

void DerivationFramework::xAODtoHepMCCnvAlg::printxAODEvent(
                          const xAOD::TruthEvent* event) const{

  std::vector<int> bcPars;
  std::vector<int> bcKids;

  std::cout <<"======================================================================================" <<std::endl;
  std::cout <<"xAODTruth Event " <<"666666" <<std::endl;
  std::cout <<"   Barcode      PDG Id  Status   px(GeV)   py(GeV)   pz(GeV)    E(GeV)   Parent: Decay" <<std::endl;
  std::cout <<"   -----------------------------------------------------------------------------------" <<std::endl;

  int nPart = event->nTruthParticles();
  for(int i=0; i<nPart; ++i){
    const xAOD::TruthParticle* part = event->truthParticle(i);
    int bc = part->barcode();
    if( bc > 100000 ) continue;
    int id = part->pdgId();
    int stat = part->status();
    float px = part->px()/1000.;
    float py = part->py()/1000.;
    float pz = part->pz()/1000.;
    float e = part->e()/1000.;
    bcPars.clear();
    bcKids.clear();

    if( part->hasProdVtx() ){
      const xAOD::TruthVertex* pvtx = part->prodVtx();
      if( pvtx ) bcPars.push_back(pvtx->barcode());
    }

    if( part->hasDecayVtx() ){
      const xAOD::TruthVertex* dvtx = part->decayVtx();
      if( dvtx ) bcKids.push_back(dvtx->barcode());
    }

    std::cout <<std::setw(10)<<bc <<std::setw(12)<<id
              <<std::setw(8)<<stat
              <<std::setprecision(2)<<std::fixed
              <<std::setw(10)<<px <<std::setw(10)<<py
              <<std::setw(10)<<pz <<std::setw(10)<<e <<"   ";
    std::cout <<"P: ";
    for(unsigned int k=0; k<bcPars.size(); ++k){
      std::cout <<bcPars[k] <<" ";
    }
    std::cout <<"  D: ";
    for(unsigned int k=0; k<bcKids.size(); ++k){
      std::cout <<bcKids[k] <<" ";
    }
    std::cout <<std::endl;
  }
  std::cout <<"======================================================================================" <<std::endl;
}
