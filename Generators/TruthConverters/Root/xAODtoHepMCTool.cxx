/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TruthConverters/xAODtoHepMCTool.h"

xAODtoHepMCTool::xAODtoHepMCTool( const std::string& name ) 
  : asg::AsgTool( name ),
  m_momFac(1.), //<-- MeV/GeV conversion factor
  m_lenFac(1.), //<-- length conversion factor
  m_signalOnly(true),
  m_maxCount(0)
{
}


StatusCode xAODtoHepMCTool::initialize() {
  ATH_MSG_INFO ("Initializing xAODtoHepMCTool "<< name() << "...");
  ATH_MSG_INFO ("SignalOnly         = " << m_signalOnly);
  m_evtCount   = 0;
  m_badSuggest = 0;
  m_noProdVtx  = 0;
  m_badBeams   = 0;
  return StatusCode::SUCCESS;
}



StatusCode xAODtoHepMCTool :: finalize ()
{
  ATH_MSG_INFO ("==============================================================");
  ATH_MSG_INFO ("==========    xAOD -> HepMC Tool :: Run Summary     ==========");
  ATH_MSG_INFO ("==============================================================");
  if( m_badSuggest ){
    ATH_MSG_INFO ("Number of suggest_barcode failures = " << m_badSuggest);
    ATH_MSG_INFO (" ... check input particle/vertex barcodes.");
  } else {
    ATH_MSG_INFO ("No suggest_barcode failures");
  }
  if ( m_noProdVtx ) {
    ATH_MSG_INFO ("Number of events that have a missing production vertex = " << m_noProdVtx);
  }else{
    ATH_MSG_INFO ("No missing production vertices");
  }
  if ( m_badBeams ){
    ATH_MSG_INFO ("Number events with improperly defined beamparticles = " << m_badBeams);
    ATH_MSG_INFO ("Inconsistencies with the beam particles storage in the event record!");
    ATH_MSG_INFO ("... check xAOD::TruthEvent record ...");
  }else{
    ATH_MSG_INFO ("No events with undefined beams.");
  }
  ATH_MSG_INFO ("==============================================================");
  ATH_MSG_INFO ("===================    End Run Summary     ===================");
  ATH_MSG_INFO ("==============================================================");
  return StatusCode::SUCCESS;
}

std::vector<HepMC::GenEvent> xAODtoHepMCTool :: getHepMCEvents(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::EventInfo* eventInfo)
{
  ++m_evtCount;
  bool doPrint = m_evtCount < m_maxCount;
  // CREATE HEPMC EVENT COLLECTION
  std::vector<HepMC::GenEvent> mcEventCollection;
  // Loop over xAOD truth container
  // Signal event is always first, followed by pileup events
  ATH_MSG_DEBUG("Start event loop");
  for (const xAOD::TruthEvent* xAODEvent : *xTruthEventContainer) {
    if( doPrint ) ATH_MSG_DEBUG("XXX Printing xAOD Event");
    if( doPrint ) printxAODEvent( xAODEvent,eventInfo );
    // Create GenEvent for each xAOD truth event
    ATH_MSG_DEBUG("Create new GenEvent");
    HepMC::GenEvent hepmcEvent = createHepMCEvent(xAODEvent,eventInfo);
    // Insert into McEventCollection
    mcEventCollection.push_back(hepmcEvent);    
    if( doPrint ) ATH_MSG_DEBUG("XXX Printing HepMC Event");
    if( doPrint ) hepmcEvent.print();
    // Quit if signal only
    if( m_signalOnly ) break;
  }  
  return mcEventCollection;
}


const HepMC::GenEvent xAODtoHepMCTool::createHepMCEvent(const xAOD::TruthEvent* xEvt, const xAOD::EventInfo* eventInfo) {
  
  ///EVENT LEVEL
  HepMC::GenEvent genEvt;

  long long int evtNum = eventInfo->eventNumber();    
  genEvt.set_event_number(evtNum);
  ATH_MSG_DEBUG("Start createHepMCEvent for event " <<evtNum);
  
  //Beam particles
  /* commenting out for now as the particle links are broken in TEvent 
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
  */


  // a similar hack... but protects against (counts) events where there are < 2 incoming protons ... 
  // not sure why this happens but it does happen various Higgs AODs from different decay channels.
  // JIRA ticket: 
  /*
  const xAOD::TruthParticle* beam1 = xEvt->truthParticle(0);
  const xAOD::TruthParticle* beam2 = xEvt->truthParticle(1);
  std::pair<HepMC::GenParticle*,HepMC::GenParticle*> HepMC_beamParticles;
  if (beam1) 
    HepMC_beamParticles.first =  new HepMC::GenParticle(
							HepMC::FourVector(
									  m_momFac * beam1->px(),
									  m_momFac * beam1->py(),
									  m_momFac * beam1->pz(),
									  m_momFac * beam1->e() ),
							beam1->pdgId(),
							beam1->status()); 
  if (beam2)
    HepMC_beamParticles.second = new HepMC::GenParticle(
							HepMC::FourVector(
									  m_momFac * beam2->px(),
									  m_momFac * beam2->py() ,
									  m_momFac * beam2->pz(),
									  m_momFac * beam2->e() ),
							beam2->pdgId(),
							beam2->status());
  genEvt.set_beam_particles(HepMC_beamParticles);a
  */

  /*
  std::pair< HepMC::GenParticle*, HepMC::GenParticle* >  beams = genEvt.beam_particles ();  
  if ( beams.first->pdg_id()!=2212 || beams.second->pdg_id()!=2212 ) { 
    genEvt.set_event_number(-evtNum); 
    if ( ++m_badBeams < m_maxCount ) ATH_MSG_WARNING("xAOD->HepMC :: inconsistent truth event record >> event does not have 2 beam protons!"); 
    ATH_MSG_WARNING("Beam one PDGID "<<beams.first->pdg_id());
  }
  ATH_MSG_WARNING("Number of bad beams "<<m_badBeams);
  */

  /*
    xAOD::TruthEvent::PdfInfo pdfInfo = xEvt->pdfInfo();
  HepMC::PdfInfo pdfInfo_hepmc;
  pdfInfo_hepmc.set_id1( pdfInfo.pdgId1 );
  pdfInfo_hepmc.set_id2( pdfInfo.pdgId2 );
  pdfInfo_hepmc.set_pdf_id1( pdfInfo.pdfId1 );
  pdfInfo_hepmc.set_pdf_id2( pdfInfo.pdfId2 );
  pdfInfo_hepmc.set_x1( (double)pdfInfo.x1 );
  pdfInfo_hepmc.set_x2( (double)pdfInfo.x2 );
  pdfInfo_hepmc.set_pdf1( (double)pdfInfo.xf1 );
  pdfInfo_hepmc.set_pdf2( (double)pdfInfo.xf2 );
  //pdfInfo_hepmc.set_scalePDF( m_momFac * (double)pdfInfo.Q );
  genEvt.set_pdf_info( pdfInfo_hepmc );
  */

  //Cross section <- Can't see this read in Reader
  // Need to look in xTruthEvent
  //HepMC::GenCrossSection* xsec = new HepMC::GenCrossSection();
  //xsec->set_cross_section((double)xEvt->crossSection());
  //xsec->set_cross_section_error((double)xEvt->crossSectionError());
  //genEvt.set_cross_section( *xsec ); // this is currently always -1 ... something wrong

  //Weights
  // const std::vector<float> weights = xEvt->weights();
  // for ( std::vector<float>::const_iterator wgt = weights.begin(); wgt != weights.end(); ++wgt ) 
  //   genEvt.weights().push_back(*wgt);
  

  // PARTICLES AND VERTICES  
  // Map of existing vertices - needed for the tree linking
  std::map<const xAOD::TruthVertex*,HepMC::GenVertex*> vertexMap;

  // Loop over all of the particles in the event, call particle builder
  // Call suggest_barcode only after insertion!  
  for (auto tlink:xEvt->truthParticleLinks()) {
    if (!tlink.isValid()) continue;
    const xAOD::TruthParticle* xPart = *tlink;
    
    // sanity check
    if (xPart == nullptr) {
      ATH_MSG_WARNING("xAOD TruthParticle is equal to NULL. This should not happen!");
      continue;
    }

    if( !xPart->hasProdVtx() && !xPart->hasDecayVtx() ){
      ATH_MSG_WARNING("xAOD particle with no vertices, bc = "<<xPart->barcode());
      continue;
    }

    // skip particles with barcode > 200000 --> Geant4 secondaries 
    if ( xPart->barcode() > 200000 ) continue; 

    // Create GenParticle
    HepMC::GenParticle* hepmcParticle( createHepMCParticle(xPart) );
    int bcpart = xPart->barcode();
    
    // status 10902 should be treated just as status 2
    if ( hepmcParticle->status() == 10902 ) hepmcParticle->set_status(2);

    // Get the production and decay vertices
    if( xPart->hasProdVtx() ) {
      const xAOD::TruthVertex* xAODProdVtx = xPart->prodVtx();
      // skip production vertices with barcode > 200000 --> Geant4 secondaries 
      if ( std::abs(xAODProdVtx->barcode()) > 200000 ) continue; 
      bool prodVtxSeenBefore(false); // is this new?
      HepMC::GenVertex* hepmcProdVtx = vertexHelper(xAODProdVtx,vertexMap,prodVtxSeenBefore);
      // Set the decay/production links
      hepmcProdVtx->add_particle_out(hepmcParticle);
      // Insert into Event
      if (!prodVtxSeenBefore){ 
	genEvt.add_vertex(hepmcProdVtx);
	if( !hepmcProdVtx->suggest_barcode(xAODProdVtx->barcode()) ){
	  ATH_MSG_WARNING("suggest_barcode failed for vertex "<<xAODProdVtx->barcode());
	  ++m_badSuggest;
	}
            }
      if( !hepmcParticle->suggest_barcode(bcpart) ){
	ATH_MSG_DEBUG("suggest_barcode failed for particle " <<bcpart);
	++m_badSuggest;
      }
      bcpart = 0;
    } else {
      ATH_MSG_DEBUG("No production vertex found for particle "<<xPart->barcode());
    }
    
    if( xPart->hasDecayVtx() ){
      const xAOD::TruthVertex* xAODDecayVtx = xPart->decayVtx();
      // skip decay vertices with barcode > 200000 --> Geant4 secondaries 
      if ( fabs(xAODDecayVtx->barcode()) > 200000 ) continue; 
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
HepMC::GenVertex* xAODtoHepMCTool::vertexHelper(const xAOD::TruthVertex* xaodVertex,
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
HepMC::GenParticle* xAODtoHepMCTool::createHepMCParticle(const xAOD::TruthParticle* particle) {
  ATH_MSG_VERBOSE("Creating GenParticle for barcode " <<particle->barcode());
  const HepMC::FourVector fourVec( m_momFac * particle->px(), m_momFac * particle->py(), m_momFac * particle->pz(), m_momFac * particle->e() );
  HepMC::GenParticle* hepmcParticle = new HepMC::GenParticle(fourVec, particle->pdgId(), particle->status());
  hepmcParticle->set_generated_mass( m_momFac * particle->m());
  return hepmcParticle;
}

// Create the HepMC GenVertex
// Call suggest_barcode after insertion!
HepMC::GenVertex* xAODtoHepMCTool::createHepMCVertex(const xAOD::TruthVertex* vertex) {
  ATH_MSG_VERBOSE("Creating GenVertex for barcode " <<vertex->barcode());
  HepMC::FourVector prod_pos( m_lenFac * vertex->x(), m_lenFac * vertex->y(),m_lenFac * vertex->z(), m_lenFac * vertex->t() );
  HepMC::GenVertex* genVertex = new HepMC::GenVertex(prod_pos);
  return genVertex;
}

// Print xAODTruth Event. The printout is particle oriented, unlike the
// HepMC particle/vertex printout. Geant and pileup particles with
// barcode>100000 are omitted.
void xAODtoHepMCTool::printxAODEvent(const xAOD::TruthEvent* event, const xAOD::EventInfo* eventInfo) const{
  
  std::vector<int> bcPars;
  std::vector<int> bcKids;

  long long int evtNum = eventInfo->eventNumber();

  std::cout <<"======================================================================================" <<std::endl;
  std::cout <<"xAODTruth Event " << evtNum <<std::endl;
  std::cout <<"   Barcode      PDG Id  Status   px(GeV)   py(GeV)   pz(GeV)    E(GeV)   Parent: Decay" <<std::endl;
  std::cout <<"   -----------------------------------------------------------------------------------" <<std::endl;

  int nPart = event->nTruthParticles();
  for(int i=0; i<nPart; ++i){
    const xAOD::TruthParticle* part = event->truthParticle(i);
    if (part==nullptr) continue;
    int bc = part->barcode();
    if( bc > 100000 ) continue;
    int id = part->pdgId();
    if ( id != 25 ) continue;
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
