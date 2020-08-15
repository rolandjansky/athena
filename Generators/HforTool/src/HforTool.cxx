/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      Tool for Alpgen heavy flavor overlap removal
//
//      authors: Dominic Hirschbuehl, Bergische Universitaet Wuppertal
//               Michiel Sanders, Ludwig-Maximilans-Universitaet Muenchen
//
/////////////////////////////////////////////////////////////
#include <cmath>
#include <set>

#include "Gaudi/Property.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TruthUtils/GeneratorName.h"

#include "HforTool/HforTool.h"

#include "JetEvent/JetCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"


HforTool::HforTool(const std::string& type, const std::string& name, const IInterface* parent)
	: AthAlgTool(type, name, parent),
	  m_schema(""), m_minjetpt(15000), m_matchingcone(0.4),
      m_evt(NULL), m_sampleType(""), m_isZinclusive(false), m_result(""),
	  m_McEventCollectionKey("")
{
         declareInterface<IHforTool>(this);
         declareProperty("RemovalSchema",	m_schema    = "angularbased");
         declareProperty("MinJetPt",    	m_minjetpt = 20000);
         declareProperty("MatchingCone",	m_matchingcone = 0.4);
         declareProperty("IsAtlfastData",   m_isAtlfast =  false);
         declareProperty("SampleType",      m_sampleType="");
         declareProperty("JetContainer",	m_JetContainer ="AntiKt4TruthJets");

	 declareProperty("McEventCollectionKeys", m_McEventCollectionKeys, "Vector of possible SGKey to retrieve the McEventCollection") ;
	 m_McEventCollectionKeys.push_back("GEN_EVENT") ; // EVNT files
	 m_McEventCollectionKeys.push_back("GEN_AOD") ;   // AOD files

	 // Treat event with this shower generator type if the type cannot
	 // be found in the event, eg in SHERPA
	 // if this is set to "UNKNOWN", label all HF quarks as "unknown"
	 declareProperty("DefaultShowerGenerator",m_DefaultShowerGenerator = "");
}


StatusCode HforTool::initialize() {
	ATH_MSG_INFO("Initializing Hfor Tool ");

	StatusCode sc = AthAlgTool::initialize();
	if(sc.isFailure()) return sc;


	//check if removal schema is supported
	if(m_schema != "jetbased" && m_schema != "angularbased" && m_schema != "strict" )	{
		ATH_MSG_ERROR("Requested removal schema is not supported, please use: jetbased, ptbased or strict");
		return StatusCode::FAILURE;
	}

	ATH_MSG_INFO(" Using " << m_schema << " removal schema");
	return StatusCode::SUCCESS;
}



StatusCode HforTool::execute() {

        ATH_MSG_INFO("Sample Type " << m_sampleType);
	std::string jet = "Jets";
	if ( m_isAtlfast ) jet = "Jet";
	ATH_MSG_DEBUG(" Execute Hfor Tool ");

        testSampleType();

 	if (m_sampleType == "") {
	  ATH_MSG_DEBUG("Used sample doesn't need any heavy flavor overlap removal!");
	  return StatusCode::FAILURE;
 	}

	ATH_MSG_DEBUG(" Sample Type "<<m_sampleType);

	findHFQuarks() ;

	ATH_MSG_DEBUG("ME() =============================================");
	ATH_MSG_DEBUG("GS(b) = " << m_Quarks_GS[5].size()
	    << " ME(b) = "<<m_Quarks_ME[5].size()
	    << " MPI(b) = "<<m_Quarks_MPI[5].size()
            << " TOP(b) = "<<m_Quarks_topdecay[5].size());

	ATH_MSG_DEBUG("GS(c) = " << m_Quarks_GS[4].size()
	    << " ME(c) = "<<m_Quarks_ME[4].size()
	    << " MPI(c) = "<<m_Quarks_MPI[4].size()
            << " TOP(c) = "<<m_Quarks_topdecay[4].size());

	if (m_schema == "jetbased")
	  jetBasedRemoval();
	else if (m_schema == "angularbased")
	  angularBasedRemoval();
	else
	  keepAllRemoval();


	return StatusCode::SUCCESS;
}


/// @todo Should these functions be inline?

const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_MPI() {
  findHFQuarks() ;
  return m_Quarks_MPI[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_GS() {
  findHFQuarks() ;
  return m_Quarks_GS[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_ME() {
  findHFQuarks() ;
  return m_Quarks_ME[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_MEin() {
  findHFQuarks() ;
  return m_Quarks_MEin[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_topdecay() {
  findHFQuarks() ;
  return m_Quarks_topdecay[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_PDF() {
  findHFQuarks() ;
  return m_Quarks_PDF[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_unknown() {
  findHFQuarks() ;
  return m_Quarks_unknown[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_MPI() {
  findHFQuarks() ;
  return m_Quarks_MPI[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_GS() {
  findHFQuarks() ;
  return m_Quarks_GS[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_ME() {
  findHFQuarks() ;
  return m_Quarks_ME[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_MEin() {
  findHFQuarks() ;
  return m_Quarks_MEin[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_topdecay() {
  findHFQuarks() ;
  return m_Quarks_topdecay[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_PDF() {
  findHFQuarks() ;
  return m_Quarks_PDF[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_unknown() {
  findHFQuarks() ;
  return m_Quarks_unknown[4] ;
}


std::string HforTool::getDecision(const std::string& schema) {
  if (schema == "") {
    // Using schema given in config file
    return m_result;
  } else if (schema == "jetbased") {
    jetBasedRemoval();
    return m_result;
  } else if (schema == "angularbased") {
    angularBasedRemoval();
    return m_result;
  } else {
    keepAllRemoval();
    return m_result;
  }
}




// Find the Heavy Flavour Quarks in this event
void HforTool::findHFQuarks() {
  // Get the event / run number from StoreGate
  const EventInfo * currentEvent(NULL) ;
  StatusCode sc = evtStore()->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
    ATH_MSG_INFO("Couldnt retrieve EventInfo from StoreGateSvc");
    return ;
  }

  // Make sure that we haven't processed this event before
  const EventType* eventType = currentEvent->event_type();
  EventID::number_type run_number(eventType->mc_channel_number()) ;
  //EventID::number_type run_number(currentEvent->event_ID()->run_number()) ;
  EventID::number_type event_number(currentEvent->event_ID()->event_number()) ;
  if ( run_number == m_prev_run_number &&
       event_number == m_prev_event_number )
    return ;

  m_prev_run_number = run_number ;
  m_prev_event_number = event_number ;

  // clean up from previous event
  m_Quarks_MPI.clear() ;
  m_Quarks_GS.clear() ;
  m_Quarks_ME.clear() ;
  m_Quarks_MEin.clear() ;
  m_Quarks_topdecay.clear() ;
  m_Quarks_PDF.clear() ;
  m_Quarks_unknown.clear() ;
  m_ShowerGenerator.clear() ;

  // get the McEventCollection from StoreGate

  // if we don't know yet which store gate key to use, find it here
  if ( !m_McEventCollectionKey.size() ) {
    for ( std::vector<std::string>::const_iterator ikey = m_McEventCollectionKeys.begin();
	  !m_McEventCollectionKey.size() && ikey != m_McEventCollectionKeys.end();
	  ikey++ ) {
      ATH_MSG_DEBUG("SG key " << (*ikey));
      const McEventCollection * mymcevent(NULL) ;
      sc = evtStore()->retrieve(mymcevent, (*ikey)) ;
      if (sc.isFailure())
	ATH_MSG_DEBUG("no McEventCollection found with key " << (*ikey));
      else {
	m_McEventCollectionKey = (*ikey) ;
	ATH_MSG_INFO("McEventCollection found with key " << m_McEventCollectionKey);
      }
    }
    //if we still don't know which key to use, return
    if ( !m_McEventCollectionKey.size() ) {
      ATH_MSG_INFO("McEventCollection key unknown");
      return ;
    }
  }

  // Get the McEventCollection
  const McEventCollection* myMcEventCollection(NULL);
  /// @todo Use CHECK macro
  sc = evtStore()->retrieve(myMcEventCollection,m_McEventCollectionKey);
  if (sc.isFailure()) {
    ATH_MSG_INFO("McEventCollection not found");
    return ;
  }
  const HepMC::GenEvent* evt = *(myMcEventCollection->begin());
  if(!evt){
    ATH_MSG_INFO("GenEvent in McEventCollection not found");
    return ;
  }
  // find out which generator was used for this event,
  int g_id(HepMC::signal_process_id(evt)) ;
  const std::string gen1 = generator_string(first_generator(g_id));
  const std::string gen2 = generator_string(second_generator(g_id));
  const std::string gen3 = generator_string(third_generator(g_id));
  ATH_MSG_DEBUG("Using generators " << gen1 << " " << gen2 << " " << gen3) ;
  if ( gen1 == "PYTHIA" || gen2 == "PYTHIA" || gen3 == "PYTHIA" )
    m_ShowerGenerator = "PYTHIA" ;
  else if ( gen1 == "HERWIG" || gen2 == "HERWIG" || gen3 == "HERWIG" )
    m_ShowerGenerator = "HERWIG" ;
  if ( !m_ShowerGenerator.size() ) {
    if ( m_DefaultShowerGenerator.size() ) {
      ATH_MSG_INFO("Shower generator type not found in the event; using "<< m_DefaultShowerGenerator << " instead") ;
      m_ShowerGenerator = m_DefaultShowerGenerator ;
    } else {
      ATH_MSG_WARNING("Shower generator type unknown ("<< gen1 << "," << gen2 << "," << gen3 << ")") ;
      return ;
    }
  }

  // vectors with the initial and final state b/c quarks; ie, initial or final
  // in the parton shower; ignore b/c quarks from b/c-hadron decays
  std::map< int,std::vector<HepMC::ConstGenParticlePtr> > finalstate_q ;

  // get the GenEvent, assume it's the first one in the McEventCollection
  for (auto  bcpart: *evt) {
    int pdg = bcpart->pdg_id();
    int apdg = std::abs(pdg) ;
    if (apdg == 5 || apdg == 4) { // b or c quark
      ATH_MSG_DEBUG("pdg = " << pdg << ": " << bcpart);

      // find the production vertex and parents
      auto prodvtx = bcpart->production_vertex() ;
      bool hasbchadronparent(false) ;
      bool hasmpiparent(false) ;
      bool hastopparent(false) ;
      bool hasWparent(false) ;
      if ( prodvtx ) {
	ATH_MSG_DEBUG("  prod vtx: " << prodvtx);
	// check that there is no b/c-hadron as a parent
	// also find mpi and top parents
#ifdef HEPMC3
	for (auto pin: prodvtx->particles_in()){
#else
	for (auto pin_it = prodvtx->particles_begin(HepMC::parents); pin_it != prodvtx->particles_end(HepMC::parents); pin_it++) {
          auto pin=pin_it;
#endif
          if(hasbchadronparent) break;
	  ATH_MSG_DEBUG("    incoming: " << *pin);
	  int pdgin(std::abs((*pin)->pdg_id())) ;

	  if ( (pdgin%10000)/1000 == apdg || (pdgin%1000)/100 == apdg )
	    hasbchadronparent = true ;
	  // also reject the c-quarks from a b-quark/hadron decay
	  if ( apdg == 4 && ( pdgin == 5 || (pdgin%10000)/1000 == 5 ||(pdgin%1000)/100 == 5 ) )
	    hasbchadronparent = true ;
	  // Herwig specific
	  if ( pdgin == 0 && (*pin)->status() == 120 )
	    hasmpiparent = true ;
	  if ( pdgin == 6 ) {
	    hastopparent = true ;
	    ATH_MSG_DEBUG("  b/c parton with a top parent");
	  }
	  if ( pdgin == 24 ) {
	    hasWparent = true ;
	    ATH_MSG_DEBUG("  b/c parton with a W parent");
	  }
	}
      } else {
	ATH_MSG_DEBUG("  b/c parton without production vertex");
      }

      if ( hasbchadronparent )
	ATH_MSG_DEBUG("  b/c parton with a b/c quark/hadron parent") ;

      // find the decay vertex and children
      auto decayvtx = bcpart->end_vertex() ;
      bool hasbcquarkdaughter(false) ;
      if ( !hasbchadronparent && decayvtx ) {
	ATH_MSG_DEBUG("  decay vtx: " << decayvtx);

	// check whether there are only non-b/c-quark daughters
	for (auto pout: *decayvtx)
        {
          if (hasbcquarkdaughter) break;
	  ATH_MSG_DEBUG("    outgoing: " << pout);
	  int pdgout(std::abs(pout->pdg_id())) ;
	    if ( pdgout == apdg )
	      hasbcquarkdaughter = true ;
	}
      } else if ( !decayvtx ) {
	ATH_MSG_DEBUG("  b/c parton without decay vertex");
      }

	// if no b/c-hadron parent and no b/c-quark daughter, keep it!
      if (!hasbchadronparent && !hasbcquarkdaughter) {
	ATH_MSG_DEBUG("  final state b/c-quark, barcode = "<< HepMC::barcode(bcpart));
	finalstate_q[apdg].push_back(bcpart) ;
      }

      // if no b/c-hadron parent, check it to see whether it comes from the ME
      // but ignore the ones with an MPI or top or W parent (these also have
      // status code 123 or 124); this is Herwig specific
      if (!hasbchadronparent && !hasmpiparent && !hastopparent && !hasWparent &&
	  ( bcpart->status() == 123 || bcpart->status() == 124 ) ) {
	  ATH_MSG_DEBUG("  b/c-quark from ME");
	  m_Quarks_MEin[apdg].push_back(bcpart->momentum()) ;
      }

    } // particle is a b or a c quark

  } // loop over all particles in the GenEvent
  
    // the rest of the classification depends on whether the shower
  // was generated with Pythia or with Herwig
  if ( m_ShowerGenerator == "HERWIG" )
    findHFQuarksHerwig(finalstate_q) ;
  else if ( m_ShowerGenerator == "PYTHIA" ) {
    findHFQuarksPythia(finalstate_q) ;
    // print out vtx -5 (the Alpgen ME process) if there are HF quarks and
    // only in DEBUG mode
    // Actually it does not make much sense.
    if ( msgLvl(MSG::DEBUG) && finalstate_q.size() ) {
      ATH_MSG_DEBUG("print out vertex -5") ;
      auto vtx5=HepMC::barcode_to_vertex(evt,-5) ;
      if (vtx5) {
#ifdef HEPMC3
	for ( auto pin = vtx5->particles_in().begin() ; pin != vtx5->particles_in().end(); pin++)      ATH_MSG_DEBUG("    incoming: " << (*pin));
	for ( auto pout = vtx5->particles_out().begin() ; pout != vtx5->particles_out().end(); pout++) ATH_MSG_DEBUG("    outgoing: " << (*pout));
#else
	for ( auto pin = vtx5->particles_begin(HepMC::parents) ; pin != vtx5->particles_end(HepMC::parents); pin++)      ATH_MSG_DEBUG("    incoming: " << (*pin));
	for ( auto pout = vtx5->particles_begin(HepMC::children) ; pout != vtx5->particles_end(HepMC::children); pout++) ATH_MSG_DEBUG("    outgoing: " << (*pout));
#endif
      }
    } // print out vtx -5 if there are HF quarks and in DEBUG mode
  } // Pythia shower
  else if ( m_ShowerGenerator == "UNKNOWN" ) {
    // simply loop over all the final state partons and call them "unknown"
    findHFQuarksUnknown(finalstate_q) ;
  }
  else
    // should never get here ...
    ATH_MSG_WARNING("Shower generator type unknown "<<m_ShowerGenerator<<")") ;

  // done
  return ;
}

////////////////////////////////////////////////////////////////////////
// Find the specifics for HF quarks in a Herwig shower
void HforTool::findHFQuarksHerwig
  (const std::map< int,std::vector<HepMC::ConstGenParticlePtr> >
   & finalstate_q )
////////////////////////////////////////////////////////////////////////
{
  ATH_MSG_DEBUG("findHFQuarksHerwig");

  // loop over all the final state b/c-quarks and find out where they come from
  // first loop over quarks flavours that were stored (b,c)
  for (auto ipdg: finalstate_q) {
    int apdg(ipdg.first) ;
    ATH_MSG_DEBUG("looking for ancestors of pdg " << apdg);

    // second loop over the final state quarks
    for ( auto bcpart: ipdg.second) {
      ATH_MSG_DEBUG("final state b/c " << bcpart);
      auto prodvtx=bcpart->production_vertex() ;
      bool isMPI(false) ;
      bool isGS(false) ;
      bool isME(false) ;
      bool isPDF(false) ;
      bool isTopDecay(false) ;
      bool isWDecay(false) ; // subset of top-decays, for hadronic top-decays
      bool iscquarkfromb(false) ;
      // if the final state quark is a PDF parton, ignore it
      // (in AOD, descendants of these partons may be filtered out)
      if ( bcpart->status() == 141 || bcpart->status() == 142 ) {
	ATH_MSG_DEBUG("PDF !!");
	isPDF = true ;
      }
      if ( !isPDF && prodvtx ) {
#ifdef HEPMC3
	for (auto  pin: HepMC::ancestor_particles(prodvtx)) {
#else
	HepMC::GenVertex::particle_iterator prodvtx_particles_begin = prodvtx->particles_begin(HepMC::ancestors) ;
	HepMC::GenVertex::particle_iterator prodvtx_particles_end =    prodvtx->particles_end(HepMC::ancestors) ;
	for (auto  pin_it=prodvtx_particles_begin;  pin_it!= prodvtx_particles_end ; pin_it++ ) {
          auto pin=*pin_it;
#endif
          if (iscquarkfromb || isPDF) break;
	  int apdgin = std::abs(pin->pdg_id()) ;
	  if (apdgin != apdg ) {
	    ATH_MSG_DEBUG("  non b/c parent " << pin);
	    // if MPI as a non-b parent, label it
	    if ( apdgin == 0 && pin->status() == 120 ) {
	      ATH_MSG_DEBUG("  MPI !!");
	      isMPI = true ;
	    }
	    // gluon splitting or ME origin: in evgen files,
	    // proton (id 2212) seems to be saved in all events; not so in
	    // AOD files... Thus look for non-HF origin with status 121 or 122
	    if ( pin->status() == 121 || pin->status() == 122 ) {
	      ATH_MSG_DEBUG("  GS !!");
	      isGS = true ;
	    }
	    // c quark from a b quark (in b-hadron decays)
	    if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 ||
				(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  c quark from b quark or b hadron");
	      iscquarkfromb = true ;
	    }
	    // b quark from a b-hadron decay
	    // (b directly from b-hadron already rejected)
	    if ( apdg == 5 && ( (apdgin%10000)/1000 == 5 ||
				(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  b quark from b hadron");
	      iscquarkfromb = true ;
	    }
	    // top quark decay
	    if ( apdgin == 6 ) {
	      ATH_MSG_DEBUG("  TOP !!") ;
	      isTopDecay = true ;
	    }
	    // W boson decay
	    if ( apdgin == 24 ) {
	      ATH_MSG_DEBUG("  W !!") ;
	      isWDecay = true ;
	    }
	  } else {
	    ATH_MSG_DEBUG("  b/c parent " << pin);
	    // if the status of a b-quark is 123 or 124, then it is a ME b-quark
	    if ( pin->status() == 123 || pin->status() == 124 ) {
	      ATH_MSG_DEBUG("  ME !!");
	      isME = true ;
	    }
	    // if status 141 or 142 then it came from the PDF, ignore those!!
	    if ( pin->status() == 141 || pin->status() == 142 ) {
	      ATH_MSG_DEBUG("  PDF !!");
	      isPDF = true ;
	    }
	  } // b/c or non-b/c quark as parent
	} // loop over all ancestors
      } // final state b/c-quark with a production vertex

      // TopDecay does not depend on status codes so it comes first
      // MPI output is also status 123,124 so MPI comes before the others
      // ME parents have status 121 or 122, so ME comes before GS
      if ( !iscquarkfromb && !isPDF ) {
	if ( isTopDecay ) {
	  // if a b or c appears in the shower of a b-parton from a top decay,
	  // it should be counted as coming from the top decay; however, the
	  // b or c should not come from a W decay in a top decay
	  if ( !isWDecay )
	    m_Quarks_topdecay[apdg].push_back( bcpart->momentum() ) ;
	}
	else if ( isMPI )
	  m_Quarks_MPI[apdg].push_back( bcpart->momentum() ) ;
	else if ( isME )
	  m_Quarks_ME[apdg].push_back( bcpart->momentum() ) ;
	else if ( isGS ) {
	  // in AOD, incoming ME partons may look like GS partons if
	  // their descendants are filtered out
	  if ( ! (bcpart->status() == 123 || bcpart->status() == 124) )
	    m_Quarks_GS[apdg].push_back( bcpart->momentum() ) ;
	  else
	    ATH_MSG_DEBUG("ME b/c-quark identified as GS");
	}
	else {
	  ATH_MSG_DEBUG("Unidentified b/c-quark");
	  m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
	}
      } // not a c-quark from a b decay or a PDF c-quark

      else if ( !iscquarkfromb && isPDF ) {
	m_Quarks_PDF[apdg].push_back(bcpart->momentum()) ;
      }

    } // loop over final state b/c-quarks
  }   // loop over quark flavours

} // end of HforTool::findHFQuarksHerwig(...)


////////////////////////////////////////////////////////////////////////
// Find the specifics for HF quarks in a Pythia shower
void HforTool::findHFQuarksPythia
  (const std::map< int,std::vector<HepMC::ConstGenParticlePtr> >
   & finalstate_q)
////////////////////////////////////////////////////////////////////////
{
  // loop over all the final state b/c-quarks and find out where they come from
  // first loop over quarks flavours that were stored (b,c)
  ATH_MSG_DEBUG("findHFQuarksPythia");

  for ( auto ipdg: finalstate_q) {
    int apdg(ipdg.first) ;
    ATH_MSG_DEBUG("looking for ancestors of pdg " << apdg);

    // assume the Alpgen input (initial-state -> final-state) is completely
    // included in the event record with stat=3 particles
    // the partons that we need are *not* these ones, but if
    // these stat=3 partons exist, then the correct partons are in the event too
    std::set<HepMC::ConstGenParticlePtr> MEParton ;
    std::set<HepMC::ConstGenParticlePtr> PDFParton ;
    // loop over the stat=3 final state quarks
    for ( auto bcpart: ipdg.second) {
      if ( bcpart->status() == 3 ) {
	ATH_MSG_DEBUG("final state b/c (stat=3) " << bcpart << ", m = " << bcpart->momentum().m() );
	// if this parton has no descendants, then it's a ME parton
	if ( !bcpart->end_vertex() ) {
	  ATH_MSG_DEBUG("  ME parton") ;
	  m_Quarks_MEin[apdg].push_back(bcpart->momentum()) ;
	  MEParton.insert(bcpart) ;
	  // if there is a direct stat=3 ancestor with the same flavour,
	  // then there is PDF parton too (eg, qc->q'Wc)
	  auto prodvtx=bcpart->production_vertex() ;
	  if ( prodvtx ) {
#ifdef HEPMC3
	    for (auto pin=prodvtx->particles_in().begin(); pin != prodvtx->particles_in().end(); pin++)
#else
	    for (auto pin=prodvtx->particles_begin(HepMC::parents); pin != prodvtx->particles_end(HepMC::parents); pin++) 
#endif
            {
	      ATH_MSG_DEBUG("    incoming: " << (*pin));
	      int pdgin((*pin)->pdg_id()) ;
	      if ( std::abs(pdgin) == apdg && (*pin)->status() == 3 ) {
		ATH_MSG_DEBUG("   PDF parton") ;
		PDFParton.insert((*pin)) ;
	      }
	    } // ME parton with a production vertex
	  } // ME final state parton
	} // final state parton with no decay vertex
	// else it's a PDF parton that gets annihilated
	else {
	  ATH_MSG_DEBUG("  PDF parton") ;
	  PDFParton.insert(bcpart) ;
	}
      } // stat=3 final state parton
    } // first loop over final state quarks
    int nMEPartons(MEParton.size()) ;
    int nPDFPartons(PDFParton.size()) ;

    // loop over the other final state quarks
    for ( auto bcpart: ipdg.second) {
      if ( bcpart->status() != 3 ) {
	ATH_MSG_DEBUG("final state b/c " << bcpart << ", m = " << bcpart->momentum().m() );
	auto prodvtx=bcpart->production_vertex() ;
	bool isTopDecay(false) ;
	bool isWDecay(false) ; // subset of top-decays, for hadronic top-decays
	bool iscquarkfromb(false) ;

	// separate GS/ME from MPI by looking at the ancestors:
	// one and only one proton ancestor -> MPI
	// two ancestors of which one a proton -> GS/ME
	bool hasPAncestor(false) ;
	int nAncestors(0) ;
	// if the parton has an origin, look at the ancestors
	if ( prodvtx ) {
	  // check whether there is a proton ancestor,
	  // and how many ancestors there are
#ifdef HEPMC3
          for ( auto pin: prodvtx->particles_in()) {
#else
          for ( auto pin_it = prodvtx->particles_begin(HepMC::ancestors); pin_it != prodvtx->particles_end(HepMC::ancestors) ; pin_it++ ) {
            auto pin=*pin_it;
#endif 
            if (iscquarkfromb ) break;
	    int apdgin = std::abs(pin->pdg_id()) ;
	    if ( apdgin != apdg ) {
	      ATH_MSG_DEBUG("  non b/c ancestor " << pin);
	      // proton parent
	      if ( apdgin == 2212 ) {
		hasPAncestor = true ;
	      }
	      // count number of ancestors
	      nAncestors += 1 ;

	      // c quark from a b quark (in b-hadron decays)
	      if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 || (apdgin%1000)/100 == 5 ) ) {
		ATH_MSG_WARNING("  c quark from b quark or b hadron");
		iscquarkfromb = true ;
	      }
	      // b quark from a b-hadron decay
	      // (b directly from b-hadron already rejected)
	      if ( apdg == 5 && ( (apdgin%10000)/1000 == 5 || (apdgin%1000)/100 == 5 ) ) {
		ATH_MSG_WARNING("  b quark from b hadron");
		iscquarkfromb = true ;
	      }
	      // top quark decay
	      if ( apdgin == 6 ) {
		ATH_MSG_DEBUG("  TOP !!") ;
		isTopDecay = true ;
	      }
	      // W boson decay
	      if ( apdgin == 24 ) {
		ATH_MSG_DEBUG("  W !!") ;
		isWDecay = true ;
	      }
	    } else {
	      ATH_MSG_DEBUG("  b/c or ME/PDF parent " << pin);
	    } // b/c or non-b/c quark as parent

	  } // loop over all ancestors

	  if ( isTopDecay || isWDecay || iscquarkfromb ) {
	  // M.P. Sanders  19-mar-2012
	  // do nothing for the time being ==> DON'T RUN THIS ON TTBAR EVENTS!
	  }
	  else if ( nAncestors == 1 && hasPAncestor ) {
	    ATH_MSG_DEBUG("MPI parton") ;
	    m_Quarks_MPI[apdg].push_back( bcpart->momentum() ) ;
	  }
	  else if ( nAncestors == 2 && hasPAncestor ) {
	    // this can come from GS, ME or PDF
	    // exact matching not possible, eg c cbar -> s W cbar
	    // cbar in the final state can be ME or PDF (from c in initial state)
	    // or g c -> W s c cbar, cbar in final state?
	    // HF pair from ME and HF pair from GS: don't know which one is which
	    if ( (nMEPartons + nPDFPartons) == 0 ) {
	      ATH_MSG_DEBUG("GS parton") ;
	      m_Quarks_GS[apdg].push_back( bcpart->momentum() ) ;
	    }
	    else {
	      ATH_MSG_DEBUG("  ME/PDF parton") ;
	      // need to check production vertices and compare them with
	      // prod. vertices of ME/PDF partons
	      int pdg(bcpart->pdg_id()) ;

	      // vertex -3 / -4 have as output showered ME parton and link
	      // to vertex -5 with stat=3 ME parton; identical pdgid
	      // -> prod vtx of showered ME parton ==
	      //    prod vtx of one of the parents of stat=3 ME parton

	      // vertex -3 / -4 have as output showerd PDF parton and link
	      // to vertex -5 wih stat=3 PDF anti-parton; opposite pdgid
	      // -> prod vtx of showered PDF parton ==
	      //    prod vtx of stat=3 PDF parton, and opposite pdgid

	      // first check that showered ME/PDF parton has prod. vtx -3 or -4
	      auto pvtx34=prodvtx ;
	      bool bc34=(HepMC::barcode(pvtx34)==-3 || HepMC::barcode(pvtx34)==-4) ;
	      if ( !bc34 ) {
#ifdef HEPMC3
                for (auto pin: HepMC::ancestor_particles(prodvtx) ) {
#else
                for (auto pin_it=prodvtx->particles_begin(HepMC::ancestors);pin_it!=prodvtx->particles_end(HepMC::ancestors); pin_it++ ) {
                 auto pin=*pin_it;
#endif
                  if (bc34) break;
		  int bcpv(-1) ;
		  if ( pin->production_vertex() )
		    bcpv = HepMC::barcode(pin->production_vertex()) ;
		  if ( pin->pdg_id() == pdg && (bcpv==-3 || bcpv==-4) ) {
		    pvtx34 = pin->production_vertex() ;
		    bc34 = true ;
		  }
		} // loop over ancestors
	      } // prod-vertex has not barcode -3 or -4
	      if ( bc34 ) {
		// number of times this parton gets identified as ME or PDF
		int nid(0) ;
		// PDF parton
		for ( auto ipdf: PDFParton) {
		  if ( ipdf->production_vertex() == pvtx34 &&
		       ipdf->pdg_id() == -pdg ) {
		    ATH_MSG_DEBUG("  -> PDF parton") ;
		    nid += 1 ;
		    m_Quarks_PDF[apdg].push_back( bcpart->momentum() ) ;
		  }
		} // loop over stat=3 PDF partons
		// ME parton
		bool isME(false) ;
		for ( auto ime:  MEParton) { if (isME) break;
		  auto mepvtx=ime->production_vertex() ;
		  // check the prod.vertices of the parents
		  if ( mepvtx ) {
#ifdef HEPMC3
                  for (auto  pin = mepvtx->particles_in().begin() ; pin != mepvtx->particles_in().end(); pin++) 
#else
                  for (auto  pin = mepvtx->particles_begin(HepMC::parents) ; pin != mepvtx->particles_end(HepMC::parents); pin++) 
#endif
                  {
		      if ( (*pin)->production_vertex() == pvtx34 && ime->pdg_id() == pdg ) {
			ATH_MSG_DEBUG("  -> ME parton") ;
			nid += 1 ;
			isME = true ;
			m_Quarks_ME[apdg].push_back( bcpart->momentum() ) ;
		      }
		    } // loop over parents
		  } // ME (stat=3) parton has a prod. vertex
		} // loop over ME (stat=3) partons
		ATH_MSG_DEBUG("this parton was id-d " << nid << " times") ;
		if ( !nid ) {
		  ATH_MSG_DEBUG("  -> GS parton") ;
		  m_Quarks_GS[apdg].push_back( bcpart->momentum() ) ;
		} // not identified as ME or PDF -> GS
	      } // prod-vertex with barcode -3 or -4
	      else {
		ATH_MSG_WARNING("parton with no -3 or -4 prod. vertex") ;
		m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
	      }
	    } // event with ME or PDF stat=3 partons

	  } // parton with 2 ancestor of which one is a proton
	  else {
	    ATH_MSG_WARNING("parton with strange set of ancestors") ;
	    m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
	  }
	} // final state b/c-quark with a production vertex

	// if the final state parton has no prod. vertex, then we
	// don't know what it is
	else {
	  ATH_MSG_DEBUG("b/c-quark without prod. vtx -> unidentified");
	  m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
	}

      } // stat!=3 final state b/c-quark

    } // loop over final state b/c-quarks

    // print out wrongly identified PDF/GS partons
    if ( msgLvl(MSG::WARNING) ) {
      if ( m_Quarks_PDF[apdg].size() != PDFParton.size() ) {
	ATH_MSG_WARNING("Mismatch in number of id-d PDF partons, apdg = "<< apdg ) ;
	ATH_MSG_DEBUG("PDF partons from ME:") ;
	for ( auto ipdf: PDFParton) {
	  const HepMC::FourVector & mom(ipdf->momentum()) ;
	  ATH_MSG_DEBUG(mom.px()<<", "<< mom.py()<<", "<<mom.pz()<<"  pT = "<<mom.perp()/1000.<<", eta = "<<mom.eta()) ;
	}
	ATH_MSG_DEBUG("PDF partons identified:") ;
	for ( auto mom:  m_Quarks_PDF[apdg]) {
	  ATH_MSG_DEBUG(mom.px()<<", "<< mom.py()<<", "<<mom.pz()<<"  pT = "<<mom.perp()/1000.<<", eta = "<<mom.eta());
	}

	// Could move the pdf parton with smaller eta to gs, but only if
	// some gs partons have been identified already
	// This is not fail-proof...

      }
    } // check number of identified PDF partons

  }   // loop over quark flavours

} // end of HforTool::findHFQuarksPythia(...)

////////////////////////////////////////////////////////////////////////
// Label all the final state partons to "unknown"
void HforTool::findHFQuarksUnknown
  (const std::map< int,std::vector<HepMC::ConstGenParticlePtr> >
   & finalstate_q )
////////////////////////////////////////////////////////////////////////
{
  ATH_MSG_DEBUG("findHFQuarksUnknown");

  // loop over all the final state b/c-quarks and call all of them "unknown"
  // this doesn't really work for, eg, Sherpa with a c in the initial state
  // or a c and a b in the same event
  // first loop over quarks flavours that were stored (b,c)
  for ( auto ipdg: finalstate_q ) {
    int apdg(ipdg.first) ;
    ATH_MSG_DEBUG("labeling final state partons with pdg " << apdg);

    // second loop over the final state quarks
    for ( auto  bcpart: ipdg.second) {
      ATH_MSG_DEBUG("final state b/c " << bcpart);
      // still filter out the b/c partons directly from a b/c/bhadron
      // or from top or W decay
      auto prodvtx=bcpart->production_vertex();
      bool isTopDecay(false) ;
      bool isWDecay(false) ; // subset of top-decays, for hadronic top-decays
      bool iscquarkfromb(false) ;
      if ( prodvtx ) {
#ifdef HEPMC3
	for ( auto pin: prodvtx->particles_in()) {
#else
	for ( auto pin_it = prodvtx->particles_begin(HepMC::ancestors); pin_it != prodvtx->particles_end(HepMC::ancestors) ; pin_it++ ) {
        auto pin=*pin_it;
#endif
        if (!!iscquarkfromb ) break;
	  int apdgin = std::abs(pin->pdg_id()) ;
	  if (apdgin != apdg ) {
	    ATH_MSG_DEBUG("  non b/c parent " << pin);
	    // c quark from a b quark (in b-hadron decays)
	    if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 ||(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  c quark from b quark or b hadron");
	      iscquarkfromb = true ;
	    }
	    // b quark from a b-hadron decay
	    // (b directly from b-hadron already rejected)
	    if ( apdg == 5 && ( (apdgin%10000)/1000 == 5 ||(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  b quark from b hadron");
	      iscquarkfromb = true ;
	    }
	    // top quark decay
	    if ( apdgin == 6 ) {
	      ATH_MSG_DEBUG("  TOP !!") ;
	      isTopDecay = true ;
	    }
	    // W boson decay
	    if ( apdgin == 24 ) {
	      ATH_MSG_DEBUG("  W !!") ;
	      isWDecay = true ;
	    }
	  } // non-b/c quark as parent
	} // loop over all ancestors
      } // final state b/c-quark with a production vertex

      if ( !iscquarkfromb ) {
	if ( isTopDecay && !isWDecay )
	  // if a b or c appears in the shower of a b-parton from a top decay,
	  // it should be counted as coming from the top decay; however, the
	  // b or c should not come from a W decay in a top decay
	  m_Quarks_topdecay[apdg].push_back( bcpart->momentum() ) ;
	else
	  m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
      } // not from b or c decay
    } // loop over final state b/c-quarks
  }   // loop over quark flavours

} // end of HforTool::findHFQuarksUnknown(...)

////////////////////////////////////////////////////////////////////////
// No overlap removal only migration of events
// to be used only Wbb samples with phase space cuts
void HforTool::keepAllRemoval()
////////////////////////////////////////////////////////////////////////
{

  if (m_Quarks_GS[5].size() + m_Quarks_ME[5].size() > 0)
    m_result = "isBB";
  else if (m_Quarks_GS[4].size() + m_Quarks_ME[4].size() > 0)
    m_result = "isCC";
  else
    m_result = "isLightFlavor";

}




////////////////////////////////////////////////////////////////////////
// Do the jetbased overlap removal
void HforTool::jetBasedRemoval()
////////////////////////////////////////////////////////////////////////
{

  // container to keep matched quarks
  // 0:GS, 1:ME
  bool hasCC[2] = {false,false};
  bool hasBB[2] = {false,false};

  int sum_match = 0;

  // default result
  m_result = m_sampleType ;

  // find the jet collection
  ATH_MSG_DEBUG(" Using "<<m_JetContainer<<" jet collection");
  const JetCollection * aod_jets = 0;
  StatusCode sc = evtStore()->retrieve( aod_jets, m_JetContainer );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD jet container found: key = " <<m_JetContainer);
    return ;
  }
  m_jets = aod_jets;
  ATH_MSG_DEBUG(" Size of Jet Collection " << aod_jets->size());

  ATH_MSG_DEBUG("There are " << m_jets->size()<<" jets in this event");
  for ( JetCollection::const_iterator jetItr  = m_jets->begin(); jetItr != m_jets->end(); jetItr++ ) {


    const Jet* thisjet = *jetItr;
    if (thisjet->pt() > m_minjetpt) {

      int match = 0;
      for (unsigned int i=0; i<m_Quarks_GS[5].size(); i++) {
	double dR = deltaR(m_Quarks_GS[5][i],(*thisjet));
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("quark pt = " << m_Quarks_GS[5][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched b quarks from GS = " << match);
      if (match > 1) hasBB[0] = true;

      sum_match += match;
      match = 0;
      for (unsigned int i=0; i<m_Quarks_ME[5].size(); i++) {
	double dR = deltaR(m_Quarks_ME[5][i],(*thisjet));
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("quark pt = " << m_Quarks_ME[5][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched b quarks from ME = " << match);
      if (match > 1) hasBB[1] = true;


      match = 0;
      for (unsigned int i=0; i<m_Quarks_GS[4].size(); i++) {
	double dR = deltaR(m_Quarks_GS[4][i],(*thisjet));
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("c quark pt = " << m_Quarks_GS[4][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched c quarks from GS = " << match);
      if (match > 1) hasCC[0] = true;

      sum_match += match;
      match = 0;
      for (unsigned int i=0; i<m_Quarks_ME[4].size(); i++) {
	double dR = deltaR(m_Quarks_ME[4][i],(*thisjet));
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("c quark pt = " << m_Quarks_ME[4][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched c quarks from ME = " << match);
      if (match > 1) hasCC[1] = true;


      sum_match += match;
    }
  }

  ATH_MSG_DEBUG("number of matched quarks = " << sum_match<<" # quarks = "<<m_Quarks_ME[5].size()+m_Quarks_GS[5].size()+m_Quarks_ME[4].size()+m_Quarks_GS[4].size());
  if (sum_match == 0) {
    if (m_Quarks_GS[5].size() == 2){
      double dR = deltaR(m_Quarks_GS[5][0],m_Quarks_GS[5][1]);
      ATH_MSG_DEBUG("GS b quarks dR = " << dR);
    }
    if (m_Quarks_ME[5].size() == 2){
      double dR = deltaR(m_Quarks_ME[5][0],m_Quarks_ME[5][1]);
      ATH_MSG_DEBUG("ME b quarks dR = " << dR);
    }

    if (m_Quarks_GS[4].size() == 2){
      double dR = deltaR(m_Quarks_GS[4][0],m_Quarks_GS[4][1]);
      ATH_MSG_DEBUG("GS c quarks dR = " << dR);
    }
    if (m_Quarks_ME[4].size() == 2){
      double dR = deltaR(m_Quarks_ME[4][0],m_Quarks_ME[4][1]);
      ATH_MSG_DEBUG("ME c quarks dR = " << dR);
    }

  }

  //light flavor samples
  if (m_sampleType == "isLightFlavor"){
    m_result = "isLightFlavor";

    //remove ME HF
    if ( (m_Quarks_ME[5].size()>0) || (m_Quarks_ME[4].size()>0) ) {
      m_result = "kill";
    } else if (m_Quarks_GS[5].size()>0){
      //remove unmatched HF from GS
      if (hasBB[0])
	m_result = "isBB";
      else
	m_result = "kill";
    } else if (m_Quarks_GS[4].size()>0){
      //remove unmatched HF from GS
      if (hasCC[0])
	m_result = "isCC";
      else
	m_result = "kill";
    }
  }


  // cc samples
  if (m_sampleType == "isCC"){
    m_result = "isCC";

     //remove matched ME HF
    if (hasCC[1])
      m_result = "kill";
    else if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // cc samples
  if (m_sampleType == "isC"){
    m_result = "isC";

    // only look if matched bb
    if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // bb samples -  we only "promote" events,
  // therefore no c-quarks have to be considered
  if (m_sampleType == "isBB"){
    m_result = "isBB";

    //remove matched ME HF
    if (hasBB[1])
      m_result = "kill";

  }
}

////////////////////////////////////////////////////////////////////////
// Do the angular based overlap removal
void HforTool::angularBasedRemoval()
////////////////////////////////////////////////////////////////////////
{

  // container to keep matched quarks
  // 0:GS, 1:ME
  bool hasCC[2] = {false,false};
  bool hasBB[2] = {false,false};


  ATH_MSG_DEBUG("Using angular matching method");

  int match_bGS = matchdR(&m_Quarks_GS[5]);
  ATH_MSG_DEBUG("number of matched (dR) b quarks from GS = " << match_bGS);
  if (match_bGS > 0) hasBB[0] = true;

  int match_bME = matchdR(&m_Quarks_ME[5]);
  ATH_MSG_DEBUG("number of matched (dR) b quarks from ME = " << match_bME);
  if (match_bME > 0) hasBB[1] = true;

  int match_cGS = matchdR(&m_Quarks_GS[4]);
  ATH_MSG_DEBUG("number of matched (dR) c quarks from GS = " << match_cGS);
  if (match_cGS > 0) hasCC[0] = true;

  int match_cME = matchdR(&m_Quarks_ME[4]);
  ATH_MSG_DEBUG("number of matched (dR) c quarks from ME = " << match_cME);
  if (match_cME > 0) hasCC[1] = true;



  //light flavor samples
  if (m_sampleType == "isLightFlavor") {
    m_result = "isLightFlavor";
    //remove ME HF
    if ( (m_Quarks_ME[5].size()>0) || (m_Quarks_ME[4].size()>0) ) {
      m_result = "kill";
    } else if (m_Quarks_GS[5].size()>0){
      //remove unmatched HF from GS
      if (hasBB[0])
	m_result = "isBB";
      else
	m_result = "kill";
    } else if (m_Quarks_GS[4].size()>0){
      //remove unmatched HF from GS
      if (hasCC[0])
	m_result = "isCC";
      else
	m_result = "kill";
    }
  }


  // cc samples
  if (m_sampleType == "isCC"){
    m_result = "isCC";

     //remove matched ME HF
    if (hasCC[1])
      m_result = "kill";
    else if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // cc samples
  if (m_sampleType == "isC"){
    m_result = "isC";

    // only look if matched bb
    if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // bb samples -  we only "promote" events,
  // therefore no c-quarks have to be considered
  if (m_sampleType == "isBB"){
    m_result = "isBB";

    //remove matched ME HF
    if (hasBB[1])
      m_result = "kill";

  }

  ATH_MSG_DEBUG("Result  = " << m_result);
}



////////////////////////////////////////////////////////////////////////
// Perform DeltaR matching between two quarks
int HforTool::matchdR(std::vector<HepMC::FourVector>* quarks)
////////////////////////////////////////////////////////////////////////
{
  int match = 0;
  if (quarks->size() > 1) {
    for (unsigned int i=0; i<quarks->size(); i++) {
      for (unsigned int j=i+1; j<quarks->size(); j++) {
	double dR = deltaR(quarks->at(i),quarks->at(j));
	ATH_MSG_DEBUG("delta R( "<<i<<","<<j<<") = " << dR);
	if (dR < m_matchingcone){
	  ++match;
	}
      }
    }
  }
  return match;
}


//wrapper function around DSID sample ID method - used to test the agreement between user definition and automatic method
void HforTool::testSampleType()

{

    m_userSampleType = m_sampleType;
    checkSampleType();
    if (m_userSampleType == m_sampleType){

        ATH_MSG_INFO("User defined sampleType  gives same value as DSID based identification");
    }else if(m_userSampleType == ""){

        ATH_MSG_INFO("User defined sampleType  is blank, falling back to DDSID based method");

    }
    else{

        ATH_MSG_INFO("User(" << m_userSampleType << ") and DSID(" << m_sampleType << ") method disagree on sampleType, using user defined value");
        m_sampleType = m_userSampleType;

    }

}
////////////////////////////////////////////////////////////////////////
// Check which sample we are running over
void HforTool::checkSampleType()
////////////////////////////////////////////////////////////////////////
{
  // Get the event / run number from StoreGate
  const EventInfo * currentEvent(NULL) ;
  StatusCode sc = evtStore()->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
    ATH_MSG_INFO("Couldnt retrieve EventInfo from StoreGateSvc");
    return ;
  }
  const EventType* eventType = currentEvent->event_type();
  EventID::number_type run_number(eventType->mc_channel_number()) ;

  // If running on EVGEN files, mc_channel_number may not be set yet
  if ( run_number == 0 )
    run_number = currentEvent->event_ID()->run_number() ;

  // poor man's solution for the time being
  m_isZinclusive = false;
  // W inclusive samples
  if ( (run_number >= 107680 && run_number <= 107685)
       || (run_number >= 107690 && run_number <= 107695)
       || (run_number >= 107700 && run_number <= 107705)
       || (run_number >= 144018 && run_number <= 144020)   // np5 excl.
       || (run_number >= 144022 && run_number <= 144024)   // np6 incl.
       || (run_number >= 144196 && run_number <= 144207)   // SUSY filtered
       || (run_number >= 105890 && run_number <= 105897)   // ttbar + Np
       || (run_number >= 117887 && run_number <= 117899)   // ttbar + Np
       || (run_number >= 164440 && run_number <= 164445)   // ttbar + Np for 8 TeV
       || (run_number >= 164450 && run_number <= 164455)   // ttbar + Np for 8 TeV
       || (run_number >= 117680 && run_number <= 117685)   // AlpgenPythia W+Np
       || (run_number >= 117690 && run_number <= 117695)   // AlpgenPythia W+Np
       || (run_number >= 117700 && run_number <= 117705)   // AlpgenPythia W+Np
       || (run_number >= 147025 && run_number <= 147030)   // AlpgenPythia Auto W+Np
       || (run_number >= 147033 && run_number <= 147038)   // AlpgenPythia Auto W+Np
       || (run_number >= 147041 && run_number <= 147046)   // AlpgenPythia Auto W+Np
       || (run_number >= 190001 && run_number <= 190005)   // akt10 filter
       || (run_number >= 190011 && run_number <= 190015)   // akt10 filter
       || (run_number >= 190021 && run_number <= 190025)   // akt10 filter
       || (run_number >= 201020 && run_number <= 201024)   // AlpgenAuto tt
       || (run_number >= 201030 && run_number <= 201034)   // AlpgenAuto tt
       || (run_number >= 201040 && run_number <= 201044)   // AlpgenAuto tt
       || (run_number >= 201220 && run_number <= 201224)   // AlpgenAuto tt
       || (run_number >= 201230 && run_number <= 201234)   // AlpgenAuto tt
       || (run_number >= 201240 && run_number <= 201244)   // AlpgenAuto tt
       )
    m_sampleType = "isLightFlavor";

  // Z inclusive sample
  if ( (run_number >= 107650 && run_number <= 107655)
       || (run_number >= 107660 && run_number <= 107665)
       || (run_number >= 107670 && run_number <= 107675)
       || (run_number >= 107710 && run_number <= 107715)
       || (run_number >= 144192 && run_number <= 144195)
       || (run_number == 144021)
       || (run_number == 147078)
       || (run_number == 147086)
       || (run_number == 147094)
       || (run_number >= 117650 && run_number <= 117655) // AlpgenPythia Z+Np
       || (run_number >= 117660 && run_number <= 117665) // AlpgenPythia Z+Np
       || (run_number >= 117670 && run_number <= 117675) // AlpgenPythia Z+Np
       || (run_number >= 147105 && run_number <= 147110) // AlpgenPythia Auto Z+Np
       || (run_number >= 147113 && run_number <= 147118) // AlpgenPythia Auto Z+Np
       || (run_number >= 147121 && run_number <= 147126) // AlpgenPythia Auto Z+Np
       || (run_number >= 156803 && run_number <= 156828) // AlpgenPythia Znunu+Np
       || (run_number >= 181310 && run_number <= 181315) // low mass DY with dilepton and VBF filter
       || (run_number >= 181320 && run_number <= 181325) // low mass DY with dilepton and VBF filter
       || (run_number >= 169450 && run_number <= 169455) // low mass DY with dilepton and VBF filter
       || (run_number >= 178354 && run_number <= 178383) // low mass DY with dilepton
       )
    {
      m_sampleType = "isLightFlavor";
      m_isZinclusive = true;
    }

  if ( (run_number >= 117288 && run_number <= 117297)
       || (run_number >= 126601 && run_number <= 126605)  // AlpgenPythia Wc
       || (run_number >= 200056 && run_number <= 200060)  // AlpgenPythia Auto Wc
       || (run_number >= 190030 && run_number <= 190034)  // akt10 filter
       )
    m_sampleType = "isC";

  if ( (run_number >= 117284 && run_number <= 117287)
       || (run_number == 116109)                           // ttbar + cc + Np
       || (run_number == 181108) || (run_number == 181109) // ttbar + cc + Np
       || (run_number == 181239) || (run_number == 181240) // ttbar + cc + Np
       || (run_number >= 126606 && run_number <= 126609)  // AlpgenPythia Wcc
       || (run_number >= 200156 && run_number <= 200159)  // AlpgenPythia Auto Wcc
       || (run_number >= 190040 && run_number <= 190043)  // akt10 filter
       || (run_number == 201027)                           // ttbar + cc
       )
    m_sampleType = "isCC";


  // Zcc samples
  if ( (run_number >= 126414 && run_number <= 126421)
       || (run_number >= 110805 && run_number <= 110816)  // AlpgenPythia Zcc
       || (run_number >= 117706 && run_number <= 117709)  // AlpgenPythia Zcc
       || (run_number >= 200432 && run_number <= 200435)  // AlpgenPythia Auto Zcc
       || (run_number >= 200440 && run_number <= 200443)  // AlpgenPythia Auto Zcc
       || (run_number >= 200448 && run_number <= 200451)  // AlpgenPythia Auto Zcc
       || (run_number >= 166990 && run_number <= 166993)  // AlpgenPythia Auto Znunucc
       || (run_number >= 178384 && run_number <= 178395)  // low mass DY with dilepton and VBF filter
       )
    m_sampleType = "isCC";


  // Wbb samples
  if ( (run_number >= 106280 && run_number <= 106283)
       || (run_number >= 107280 && run_number <= 107283)
       || (run_number >= 110801 && run_number <= 110804)  // AlpgenPythia Wbb MC12
       || (run_number >= 126530 && run_number <= 126533)  // AlpgenPythia Wbb
       || (run_number >= 200256 && run_number <= 200259)  // AlpgenPythia Auto Wbb
       || (run_number >= 190050 && run_number <= 190053)  // akt10 filter
       )
    m_sampleType = "isBB";


  // Zbb samples
  if ( (run_number >= 109300 && run_number <= 109313)
       || (run_number >= 118962 && run_number <= 118965)
       || (run_number >= 110817 && run_number <= 110828)  // AlpgenPythia Zbb MC12
       || (run_number >= 126560 && run_number <= 126563)  // AlpgenPythia Zbb
       || (run_number >= 200332 && run_number <= 200335)  // AlpgenPythia Auto Zbb
       || (run_number >= 200340 && run_number <= 200343)  // AlpgenPythia Auto Zbb
       || (run_number >= 200348 && run_number <= 200351)  // AlpgenPythia Auto Zbb
       || (run_number >= 167060 && run_number <= 167063)  // AlpgenPythia Auto Znunubb
       || (run_number >= 178396 && run_number <= 178407)  // low mass DY with dilepton and VBF filter
       || (run_number == 146983)
       || (run_number == 146988)
       )
    m_sampleType = "isBB";

  // ttbb samples
  if ( ( run_number == 116108 )
       || ( run_number == 181106 ) || ( run_number == 181107 )
       || ( run_number == 181237 ) || ( run_number == 181238 )
       || ( run_number == 201028 )
       )
    m_sampleType = "isBB";

}

HforTool::~HforTool() {
}

