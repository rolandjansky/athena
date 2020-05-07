/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitterValidationUtils/TrkPriVxPurityTool.h"

// forward includes
#include "TrkVertexFitterValidationUtils/TrkPriVxPurity.h"
#include "VxVertex/VxCandidate.h"

// normal includes
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenEvent.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruth.h"

#include <map>
#include <vector>

namespace Trk {

    StatusCode TrkPriVxPurityTool::initialize() {
        return StatusCode::SUCCESS;
    }//end of initialize method

    StatusCode TrkPriVxPurityTool::finalize() {
        msg(MSG::INFO)  << "Finalize successful" << endmsg;
        return StatusCode::SUCCESS;
    }

    TrkPriVxPurityTool::TrkPriVxPurityTool ( const std::string& t, const std::string& n, const IInterface* p ) : AthAlgTool ( t,n,p ) {
//tolerances around the signal pp interaction  to create a primary vertex candidate:
//defaul values are: sigma z = 1um (0.001 mm), sigma r = 1um (0.001 mm)
        declareProperty ( "VertexRTolerance", m_r_tol = 0.001 );

        declareProperty ( "VertexZTolerance", m_z_tol = 0.001 );

//name of the rec <-> sim truth map to be read from the storegate
//default one is the output of the  InDetTrackTruthMaker (see InDetTruthAlgs in InnerDetector packasge)
        declareProperty ( "SimTrackMapName", m_trackParticleTruthCollName = "TrackParticleTruthCollection" );

//name of the Mc event collection. Required to get the signal event.
        declareProperty ( "MonteCarloCollection", m_mc_collection_name = "TruthEvent" );

// declaring the interface fo the tool itself
        declareInterface<TrkPriVxPurityTool> ( this );

    }//end of constructor

    TrkPriVxPurityTool::~TrkPriVxPurityTool()
    {}

//analysis method
    const TrkPriVxPurity * TrkPriVxPurityTool::purity ( const Trk::VxCandidate * vertex ) const {
//    std::cout<<" The purity method called"<<std::endl;
//protection and related
        if ( vertex != 0 ) {
            const std::vector<Trk::VxTrackAtVertex *> * tracks = vertex->vxTrackAtVertex();
            if ( tracks !=0 ) {

// first getting the Mc Event collection
                const McEventCollection * mcCollection ( 0 );
                StatusCode sc = evtStore()->retrieve ( mcCollection, m_mc_collection_name );
                if ( sc.isFailure() ) {
                    if (msgLvl(MSG::DEBUG))                                      {
                        msg() << "Unable to retrieve MC collection: " << m_mc_collection_name << endmsg;
                        msg() << "Zero pointer returned." << endmsg;
                    }
                    return 0;
                }

//getting the signal event itself
                McEventCollection::const_iterator it = mcCollection->begin();
                const HepMC::GenEvent* genEvent= ( *it );
//        std::cout<<"The ID of the first event of the collection: "<<genEvent->event_number()<<std::endl;
                if( genEvent->vertices_empty() ) {
                  ATH_MSG_DEBUG( "No vertices found in first GenEvent" );
                  return 0;
                }

//analysing the MC event to create PV candidate
//first finding the vertex of primary pp interaction
                HepMC::GenEvent::vertex_const_iterator pv = genEvent->vertices_begin();

//and storing its position
                CLHEP::HepLorentzVector pv_pos ( ( *pv )->position().x(),
                                          ( *pv )->position().y(),
                                          ( *pv )->position().z(),
                                          ( *pv )->position().t() );
                double pv_r = pv_pos.perp();
                double pv_z = pv_pos.z();
//    std::cout<<"Purity: Primary vertex position: "<<pv_pos<<std::endl;

// storing all the ids of vertices reasonably close to the primary one.
// here the region of interest is selected.
                std::map<int,HepMC::GenVertex *> vertex_ids;

                for ( HepMC::GenEvent::vertex_const_iterator i = genEvent->vertices_begin();
                      i != genEvent->vertices_end()  ;++i ) {
                    CLHEP::HepLorentzVector lv_pos ( ( *i )->position().x(),
                                              ( *i )->position().y(),
                                              ( *i )->position().z(),
                                              ( *i )->position().t() );
                    if ( fabs ( lv_pos.perp() - pv_r ) <m_r_tol  && fabs ( lv_pos.z() - pv_z ) <m_z_tol ) {
                        vertex_ids[ ( *i )->barcode() ]= ( *i );
                    }//end of accepted vertices check
                }//end  of loop over all the vertices

//    std::cout<<"Actual number of merged vertices "<<vertex_ids.size()<<std::endl;
                /*-------------------Some debug output-------------------------------------------------
                  std::cout<<"Actual number of merged vertices "<<vertex_ids.size()<<std::endl;
                  for( std::map<int,HepMC::GenVertex *>::const_iterator i = vertex_ids.begin();
                  i != vertex_ids.end(); ++i)
                  {
                  HepMC::GenVertex * cev =(*i).second;
                  std::cout<<"Selected GenVertex has event index: "<<cev->parent_event()->event_number() <<std::endl;
                  }//end of debug loop over all the vertices
                  //------------------ End of debug output ------------------------------------------------
                  */

//getting the track truth collection
                const TrackParticleTruthCollection * trackParticleTruthCollection ( 0 );
                sc = evtStore()->retrieve ( trackParticleTruthCollection, m_trackParticleTruthCollName );

                if ( sc.isFailure() ) {
                    if (msgLvl(MSG::DEBUG)) {
                        msg() << "Cannot retrieve " << m_trackParticleTruthCollName << endmsg;
                        msg() << "Zero pointer returned" << endmsg;
                    }
                    return 0;
                }

//looping over the tracks to find those matched to the GenParticle originating from signal PV
                std::vector<Trk::VxTrackAtVertex *>::const_iterator vt = tracks->begin();
                std::vector<Trk::VxTrackAtVertex *>::const_iterator ve = tracks->end();
//         unsigned int total_size = 0;
                unsigned int n_failed = 0;
                std::vector<double> in_weights ( 0 );
                std::vector<double> out_weights ( 0 );
                std::vector<double> pu_weights ( 0 );
                std::vector<double> no_correspondance ( 0 );
	
//        std::cout<< "Getting to the main loop"<<std::endl;

                for ( ;vt!=ve;++vt ) {
//original element link
//ugly so far, we'll correct the EDM later
                    if ( ( *vt ) !=0 ) {

                        ITrackLink * origLink = ( **vt ).trackOrParticleLink();

                        if ( origLink !=0 ) {
                            // get to the original track particle
                            LinkToTrackParticleBase * tr_part = dynamic_cast< LinkToTrackParticleBase * > ( origLink );
                            if ( tr_part !=0  && tr_part->isValid()) {
//                 ++total_size;
                
//		std::cout<< "Link non zero"<<std::endl;

                                std::map< Rec::TrackParticleTruthKey, TrackParticleTruth>::const_iterator ttItr = trackParticleTruthCollection->end();
                  
                                const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part);
                                if(tp) {
                                    const Rec::TrackParticleContainer* tpCont = dynamic_cast<const Rec::TrackParticleContainer*>(tr_part->getStorableObjectPointer());
                                    if (tpCont) {
                                        ElementLink<Rec::TrackParticleContainer> linkTruth;
                                        linkTruth.setElement(const_cast<Rec::TrackParticle*>(tp));
                                        linkTruth.setStorableObject(*tpCont);
                                        ttItr = trackParticleTruthCollection->find(Rec::TrackParticleTruthKey(linkTruth));
                                    }
                                }
                
//		std::cout<< "Find done"<<std::endl;
			
                                if (ttItr != trackParticleTruthCollection->end() ) {
//		std::cout<< "Found correspondance"<<std::endl;
                                    const HepMcParticleLink& particleLink = ttItr->second.particleLink();
                                    const HepMC::GenParticle* genParticle = particleLink.cptr();
//		 std::cout<<"GenParticle pointer: "<<  genParticle<<std::endl;
		 
                                    if(genParticle !=0) {
                                        HepMC::GenEvent * tpEvent = genParticle->parent_event();
		 
//		  std::cout<<"Parent event "<<  tpEvent<<std::endl;
		 
                                        if(tpEvent==genEvent) { 
                                            const HepMC::GenVertex * pVertex(0);
                                            if (genParticle!=0) pVertex = genParticle->production_vertex();
		
                                            if ( pVertex != 0 ) {
                                                int link_pid = genParticle->pdg_id();
                                                bool primary_track = false;
                                                bool secondary_track = false;
                  
//		    std::cout<< "Decision loop starts"<<std::endl;
//loop over the particles until decision is really taken
                                                do {
                                                    int tvrt_code = pVertex->barcode();
                                                    std::map<int, HepMC::GenVertex *>::const_iterator idf_res = vertex_ids.find ( tvrt_code );

//for the HepMcParticle Link, the signal event has an index 0.
// tagging on it
                                                    if ( idf_res != vertex_ids.end() ) {
//this is a primary track, storing its weight
                                                        primary_track = true;
                                                        in_weights.push_back ( ( *vt )->weight() );
                                                    }else {
//this vertex is not from the central region.
//checking whether it is a bremsstrahlung
//if so, propagating track to its origin, otherwise rejecting it completely.
                                                        if ( pVertex->particles_in_size() == 1 ) {
// one mother particle: is it a brem of some kind?
                                                            HepMC::GenVertex::particles_in_const_iterator inp = pVertex->particles_in_const_begin() ;
                                                            HepMC::GenVertex * tmpVertex_loc = ( *inp )->production_vertex();
                                                            if ( ( *inp )->pdg_id() == link_pid  && tmpVertex_loc) {
// seems like a brem (this can be generator/simulation dependent unfortunately)
// continue iterating
                                                                pVertex = tmpVertex_loc;
                                                            }else {
                                                                secondary_track = true;
                                                                out_weights.push_back ( ( *vt )->weight() );
                                                            }//end of "no id change" check
                                                        }else {

//has more than one mother particle; comes from reaction, not from central region
// it is an outlier
                                                            secondary_track = true;
                                                            out_weights.push_back ( ( *vt )->weight() );
                                                        }//end of reaction check
                                                    }//end of central region check
                                                }while ( !primary_track && !secondary_track );//end of loop over gen particles
                                            }else {
		  
// this track has no production vertex. Whatever it is, it does not
// come from the primary interaction, consider as pileup
                                                if (msgLvl(MSG::INFO)) {
                                                    msg() <<"A truth particle with no production vertex found."<<endmsg;
                                                    msg() <<"Since it does not come from PV, consider as PileUp"<<endmsg;
                                                }
                                                pu_weights.push_back ( ( *vt )->weight() );
                                            } //end of particle link without production vertex check.
                                        }else {
		   
//		  std::cout<<"Not equal events "<< std::endl; 
                                            pu_weights.push_back ( ( *vt )->weight() );
                                        }//end of event pointers comparison
                                    }else{
	     
//the corresponding link to the track particles is zero. 
//This seems to be a  fake (broken link in any case).	        
                                        no_correspondance.push_back ( ( *vt )->weight() );
                                    }//end of genParticle check 
                                }//end of search for correspondance in the trurth   collection
                            }else{
                                if (msgLvl(MSG::DEBUG)) msg() <<"This track at vertex has no valid link to its original trackparticle "<<endmsg;
                                ++ n_failed;
                            }//end of search for the truth link
                        }else{
                            if (msgLvl(MSG::DEBUG)) msg() <<"There is an empty pointer in the vector<VxTrackAtVertex *> for this vertex"<<endmsg;
                            ++n_failed;
                        }
                    }//end of valid link check
                } //end of check of the empty pointer in the vector

//debug
//       std::cout<<"Total number of tracks fitted to the vertex: "<<tracks->size() <<std::endl;
//       std::cout<<"Number of tracks w/o truth correspondance: "<<no_correspondance.size() <<std::endl;
//       std::cout<<"Number of correct tracks: "<<total_size<<std::endl;
//       std::cout<<"Number of tracks with broken origin links "<<n_failed<<std::endl;
//       std::cout<<"Number of fitted outliers "<<out_weights.size() <<std::endl;
//       std::cout<<"Number of fitted intliers "<<in_weights.size() <<std::endl;
      
                return new Trk::TrkPriVxPurity ( tracks->size(), pu_weights, no_correspondance,
                                                 n_failed, in_weights, out_weights );    
            } // end of if tracks != 0
        }else{
            if (msgLvl(MSG::ERROR)) {
                msg()<<"Empty vertex pointer received"<<endmsg;
                msg()<<"Empty pointer returned."<<endmsg;
            }
            return 0;
        }//end of empty vertex check
        return 0;
    }//end of purity method


}//end of namespace definitions
