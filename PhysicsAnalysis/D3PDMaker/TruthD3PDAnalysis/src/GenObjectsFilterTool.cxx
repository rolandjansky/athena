/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenObjectsFilterTool.cxx 466446 2011-11-04 15:58:28Z aad $

// System include(s):
#include <cmath>

// General include(s):
#include "AthenaKernel/errorcheck.h"
#include "HepPDT/ParticleDataTable.hh"

// Helper functors:
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenInteracting.h"

// EDM include(s):
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

// Local include(s):
#include "GenObjectsFilterTool.h"

#include "ParticleJetTools/HadronUtils.h"

namespace D3PD {

  
GenObjectsFilterTool::GenObjectsFilterTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_partPropSvc( "PartPropSvc", name ) {

   // Declare which interface(s) this tool implements:
   declareInterface< IGenObjectsFilterTool >( this );

   // Declare the handle(s) as property:
   declareProperty( "PartPropSvc", m_partPropSvc,
                    "Handle to the particle property service" );

   // Declare the properties of the tool:
   declareProperty( "PtMin" , m_ptMinCut = 100.,
                    "Minimum track pT to be selected (in MeV)" );
   declareProperty( "EtaMax" , m_etaCut = 5.,
                    "Maximum track eta. Set to negative not to apply the cut" );
   declareProperty( "RemoveEmptyEvents" , m_removeEmptyEvents = true );
   declareProperty( "RemoveDummyEvents" , m_removeDummyEvents = false );
   declareProperty( "RemoveInTimePileUp" , m_removeInTimePileUp = false );
   declareProperty( "Remove2BCPileUp" , m_remove2BCPileUp = false );
   declareProperty( "Remove800nsPileUp" , m_remove800nsPileUp = false );
   declareProperty( "RemoveCavernBkg" , m_removeCavernBkg = false );
   declareProperty( "SelectTruthTracks", m_selectTruthTracks = false,
                    "Only select stable charged particles" );
   declareProperty( "AddOnlyFirstVertex", m_addOnlyFirstVertex=false,
		    "add only first vertex per gen event");

   declareProperty( "RemoveUnrequestedParticles",m_removeUnrequestedParticles=false);
   declareProperty( "KeepBCQuarks",m_keepbcQuarks=false); 
   declareProperty( "KeepBCQuarkAncestors",m_keepbcQuarkAncestors=false); 
   declareProperty( "KeepBCHadrons",m_keepbcHadrons=false);
   declareProperty( "KeepBCHadronDecayChain",m_keepbcHadronDecayChain=false);
   declareProperty( "BCHadronsDescendantsPtCut",m_bcHadronsDescendantsPtCut=-1);
   declareProperty( "BCHadronsDescendantsBarcodeCut",m_bcHadronsDescendantsBarcodeCut=-1); /// -1 for no cut
   declareProperty( "BCHadronsDescendantsEtaCut",m_bcHadronsDescendantsEtaCut=-1);/// -1 for no cut
   declareProperty( "KeepParticleWithPdgId",m_keepParticleWithPdgId);
   declareProperty( "KeepFirstParticles", m_keepFirstParticles=-1);

   declareProperty( "KeepLeptonicWZBosons",m_keepLeptonicWZBosons=false);
   declareProperty( "KeepLeptonicWZBosonDecayChains",m_keepLeptonicWZBosonDecayChains=false);
   declareProperty( "KeepLeptonicWZBosonsParents",m_keepLeptonicWZBosonsParents=false);
   declareProperty( "KeepStatusOneLeptonsFromWZ",m_keepStatusOneLeptonsFromWZ=false);


}

StatusCode GenObjectsFilterTool::initialize() {

   // Let the user know that the tool is alive:
   ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

   // Initialize the base class:
   CHECK( AthAlgTool::initialize() );
   // Retrieve the particle property service:
   CHECK( m_partPropSvc.retrieve() );

   return StatusCode::SUCCESS;
}

bool GenObjectsFilterTool::pass( const HepMC::GenEvent* evt,
                                 const McEventCollection* coll ) const {

   bool isEmpty = ( evt->particles_size() == 0 );
   bool isDummy = ( ( evt->event_number() == -1 ) &&
                    ( evt->signal_process_id() == 0 ) );
   if( isDummy ) isEmpty = false;

   if( m_removeEmptyEvents && isEmpty ) return false;
   if( m_removeDummyEvents && isDummy ) return false;

   // We can't do the further selection without the full truth record:
   if( ! coll ) return true;

   McEventCollection::const_iterator iter = coll->begin();
   McEventCollection::const_iterator end = coll->end();

   if( *iter == evt ) return true; /// always keep signal

   int gotzero = 1;
   for( ; iter != end; ++iter ) {
      if( ( ( ( *iter )->event_number() == -1 ) &&
            ( ( *iter )->signal_process_id() == 0 ) ) ) {
         ++gotzero;
      }
      if( evt == *iter ) break;
   }

   if( ( gotzero == 1 ) && m_removeInTimePileUp ) return false;
   if( ( gotzero == 2 ) && m_remove2BCPileUp ) return false;
   if( ( gotzero == 3 ) && m_remove800nsPileUp ) return false;
   if( ( gotzero == 4 ) && m_removeCavernBkg ) return false;

   return true;
}



bool GenObjectsFilterTool::isBCHadron(const HepMC::GenParticle* part) const{

  if(part->barcode() >= 200000) return false;
  int type = HadronClass::type(part->pdg_id()).second;
  if(type == 5 || type == 4)  return true;

  return false;

}

bool GenObjectsFilterTool::isBC(int pdg) const{
  if(abs(pdg)==5 || abs(pdg)==4)return true;
  return false;
}

bool GenObjectsFilterTool::isKeep(int pdg) const{
  for(unsigned int i=0; i<m_keepParticleWithPdgId.size(); ++i){
    if(abs(pdg)==m_keepParticleWithPdgId[i])return true;
  }

  return false;
}

bool GenObjectsFilterTool::isLeptonicWZ(const HepMC::GenParticle* part) const{

  int pdg = part->pdg_id();

  if(abs(pdg)!=24 && abs(pdg)!=23) return false;
  
  /// take into account W/Z->W/Z->lnu/ll

  HepMC::GenVertex* end = part->end_vertex();
  if(end){
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(;firstChild!=endChild; ++firstChild){
      int cpdg = (*firstChild)->pdg_id();
      if(abs(cpdg) == 11 || abs(cpdg) == 13 || abs(cpdg) == 15){
	return true;
      }
      if(cpdg == pdg){
	if(isLeptonicWZ(*firstChild)) return true;
      }
    }
  }

  return false;

}

bool GenObjectsFilterTool::isRequested( const HepMC::GenParticle* part) const{


   const HepMC::FourVector& p4 = part->momentum();
   double pt = p4.perp();
   double eta = p4.eta();

   int barcode = part->barcode();
   int pdg = part->pdg_id();
   int status = part->status();

   //// keep first m_keepFirstParticles particles
   if(barcode<m_keepFirstParticles) return true;

   /// always keep these pdgids
   if(isKeep(pdg)) return true;

   if(m_keepbcQuarks){
     /// keep b/c quarks
     if(isBC(pdg)){
       /// not from hadron
       bool isfromhadron=false;
       HepMC::GenVertex* prod = part->production_vertex();
       if(prod){
	 HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
	 HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
	 for(;firstParent!=endParent; ++firstParent){
	   if( isBCHadron(*firstParent) ){
	     isfromhadron=true;
	     break;
	   }
	 }
       }
       if(!isfromhadron)return true;
     }
   }

   if(m_keepbcHadrons){
     /// keep b/c quarks/hadrons
     if(isBCHadron(part)) return true;
   }


   if(m_keepbcQuarkAncestors){
     /// keep parents of b/c quarks
     HepMC::GenVertex* end = part->end_vertex();
     if(end){
       HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::descendants);
       HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::descendants);
       for(;firstChild!=endChild; ++firstChild){
	 if( isBC((*firstChild)->pdg_id()) ){
	   /// check that this quark is not from hadron (HERWIG)
	   bool isfromhadron=false;
	   HepMC::GenVertex* prod = (*firstChild)->production_vertex();
	   if(prod){
	     HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
	     HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
	     for(;firstParent!=endParent; ++firstParent){
	       if( isBCHadron(*firstParent) ){
		 isfromhadron=true;
		 break;
	       }
	     }
	   }
	   if(!isfromhadron)return true;
	 }
       }
     }
   }

   if(m_keepbcHadronDecayChain){
     //// keep particles from b/c hadron decay

     if(pt>m_bcHadronsDescendantsPtCut){
       if(m_bcHadronsDescendantsEtaCut<0 || fabs(eta)<m_bcHadronsDescendantsEtaCut){
	 if(barcode < m_bcHadronsDescendantsBarcodeCut || m_bcHadronsDescendantsBarcodeCut<0){

	   bool isfromhadron=false;
	   HepMC::GenVertex* prod = part->production_vertex();
	   if(prod){
	     HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
	     HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);

	     for(;firstParent!=endParent; ++firstParent){
	       if( isBCHadron(*firstParent) ){
		 isfromhadron=true;
		 break;
	       };
	     }
	   }
	   if(isfromhadron) return true;
	 }
       }
     }
   }


   if(m_keepLeptonicWZBosons){
     if(abs(pdg)==24 || abs(pdg)==23){
       if(isLeptonicWZ(part)) return true;
     }
   }

   if(m_keepLeptonicWZBosonDecayChains){
     if(barcode < 200000){

       bool isleptonicWZ=false;
       HepMC::GenVertex* prod = part->production_vertex();
       if(prod){
	 HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
	 HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);

	 for(;firstParent!=endParent; ++firstParent){
	   if( abs((*firstParent)->pdg_id())==24 || abs((*firstParent)->pdg_id())==23){
	     if( isLeptonicWZ(*firstParent) ){
	       isleptonicWZ=true;
	       break;
	     }
	   }
	 }
       }
       if(isleptonicWZ) return true;
     }
   }

   if(m_keepLeptonicWZBosonsParents){

     bool isleptonicWZ=false;
     HepMC::GenVertex* end = part->end_vertex();
     if(end){
       HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
       HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);

       for(;firstChild!=endChild; ++firstChild){
	 if( abs((*firstChild)->pdg_id())==24 || abs((*firstChild)->pdg_id())==23){
	   if( isLeptonicWZ(*firstChild) ){
	     isleptonicWZ=true;
	     break;
	   }
	 }
       }
     }
     if(isleptonicWZ) return true;
   }


   if(m_keepStatusOneLeptonsFromWZ){
     if(abs(pdg) == 11 || abs(pdg) == 13 || abs(pdg) == 15 ){
       if(barcode < 200000 && status == 1){

	 bool isleptonicWZ=false;
	 HepMC::GenVertex* prod = part->production_vertex();
	 if(prod){
	   HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
	   HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);

	   for(;firstParent!=endParent; ++firstParent){
	     if( abs((*firstParent)->pdg_id())==24 || abs((*firstParent)->pdg_id())==23){
	       if( isLeptonicWZ(*firstParent) ){
		 isleptonicWZ=true;
		 break;
	       }
	     }
	   }
	 }
	 if(isleptonicWZ) return true;
       }
     }
   }




   return false;
}

bool GenObjectsFilterTool::passParticleCuts( const HepMC::GenParticle* part) const{

  if(isRequested(part)) return true;
  if(m_removeUnrequestedParticles) return false;

   // Execute the 4-momenum cuts:
  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = p4.eta();

  if( pt < m_ptMinCut ) return false;
  if( ( fabs( eta ) > m_etaCut ) && ( m_etaCut >= 0. ) ) return false;

  return true;

}


bool GenObjectsFilterTool::pass( const HepMC::GenParticle* part,
                                 const McEventCollection* coll ) const {

   // Check if the particle is coming from a "good" GenEvent:
   if( ! pass( part->parent_event(), coll ) ) return false;

   if(!passParticleCuts(part)) return false;
 

   // If we don't want to specifically select charged truth tracks, then this
   // is already good enough:
   if( ! m_selectTruthTracks ) return true;
   if (part->barcode() < 200000) {
     if( ! TruthHelper::IsGenStable()( part ) ) return false;
     if( ! TruthHelper::IsGenInteracting()( part ) ) return false;
   }

   int pdg = part->pdg_id();
   /// remove gluons and quarks of status 2 that pass IsGenStable!!!
   if( abs(pdg) < 7 || abs(pdg) == 21 ) return false;

   const HepPDT::ParticleData* pd = m_partPropSvc->PDT()->particle( abs( pdg ) );
   if( ! pd ) {
     ATH_MSG_DEBUG( "Could not get particle data for pdg = " << pdg 
		      << " status " << part->status() << " barcode " <<part->barcode()
		      << " process id " <<part->parent_event()->signal_process_id());
      return false;
   }
   float charge = pd->charge();

   if( fabs( charge ) < 1E-5 ) return false;

   return true;
}

bool GenObjectsFilterTool::pass( const HepMC::GenVertex* vtx,
                                 const McEventCollection* coll ) const {

  HepMC::GenEvent* event = vtx->parent_event();

   // Check if the vertex is coming from a "good" GenEvent:
   if( ! pass(event , coll ) ) return false;

   /// always add first vertex in event
   HepMC::GenEvent::vertex_const_iterator vtxfirst =  event->vertices_begin(); 
   ///can't be invalid - already have vertices in event
   if(*vtxfirst == vtx){
     return true;
   }
   
   if(m_addOnlyFirstVertex) return false;

   // Check if any of the incoming particles pass our selection cuts:
   std::vector< HepMC::GenParticle* >::const_iterator iter =
      vtx->particles_in_const_begin();
   std::vector< HepMC::GenParticle* >::const_iterator end =
      vtx->particles_in_const_end();
   for( ; iter != end; ++iter ) {
      if( pass( *iter ) ) return true;
   }

   // Check if any of the outgoing particles pass our selection cuts:
   iter = vtx->particles_out_const_begin();
   end = vtx->particles_out_const_end();
   for( ; iter != end; ++iter ) {
      if( pass( *iter ) ) return true;
   }

   // If there aren't any good particles associated with the vertex, then
   // it shouldn't be selected:
   return false;
}


} // namespace D3PD
