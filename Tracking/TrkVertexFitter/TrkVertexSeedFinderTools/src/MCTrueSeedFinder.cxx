/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          MCTrueSeedFinder.cxx - Description in header file
*********************************************************************/

#include <utility>



#include "TrkVertexSeedFinderTools/MCTrueSeedFinder.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GaudiKernel/IPartPropSvc.h"

// Helper functors:
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenInteracting.h"
#include "HepPDT/ParticleDataTable.hh"

namespace {
    /// Auxiliary class for sorting interactions info
    class Interactions_pair {
    public:
      float first; ///< weight
      Amg::Vector3D second; ///< vertex position
      Interactions_pair(float p1, Amg::Vector3D p2)
	: first(p1), second(std::move(p2)) {};
      /// define order: note that we sort inversed in intensity (highest first)
      bool operator> (const Interactions_pair& other) const
      {return first < other.first;}
      /// define order: note that we sort inversed in intensity (highest first)
      bool operator< (const Interactions_pair& other) const
      {return first > other.first;}
    };
}

namespace Trk
{

  MCTrueSeedFinder::MCTrueSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_partPropSvc( "PartPropSvc", n ),
    m_removeInTimePileUp(false),
    m_removeHardScattering(false)
  {   
    declareProperty("RemoveHardScattering", m_removeHardScattering, "Do not consider hard-scattering");
    declareProperty("RemoveInTimePileUp", m_removeInTimePileUp, "Do not consider in-time pile-up");
    declareProperty( "PartPropSvc", m_partPropSvc, "Handle to the particle property service" );
  }
  
  
  MCTrueSeedFinder::~MCTrueSeedFinder()
  {
  }


  StatusCode MCTrueSeedFinder::initialize() 
  { 
    ATH_CHECK( m_mcEventCollectionKey.initialize() );
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode MCTrueSeedFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D
  MCTrueSeedFinder::findSeed(const std::vector<const Trk::Track*>& perigeeList,
                             const xAOD::Vertex* constraint) const
  {
    std::vector<Amg::Vector3D> seeds = findMultiSeeds (perigeeList, constraint);
    if (seeds.empty()) {
      return Amg::Vector3D(0.,0.,0.);
    }
    return seeds[0];
  }
  

  Amg::Vector3D
  MCTrueSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*>& vectorTrk,
                             const xAOD::Vertex* constraint) const
  {
    std::vector<Amg::Vector3D> seeds = findMultiSeeds (vectorTrk, constraint);
    if (seeds.empty()) {
      return Amg::Vector3D(0.,0.,0.);
    }
    return seeds[0];
  }


  std::vector<Amg::Vector3D> MCTrueSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) const
  {
    std::vector<Amg::Vector3D> seeds;
    if( retrieveInteractionsInfo (seeds).isFailure() ) {
      seeds.clear();
    }

    return seeds;
  }


  std::vector<Amg::Vector3D> MCTrueSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) const
  {
    std::vector<Amg::Vector3D> seeds;
    if( retrieveInteractionsInfo (seeds).isFailure() ) {
      seeds.clear();
    }

    return seeds;
  }

  StatusCode
  MCTrueSeedFinder::retrieveInteractionsInfo (std::vector<Amg::Vector3D>& interactions) const
  {
    ATH_MSG_DEBUG("Retrieving interactions information");
    msg(MSG::DEBUG) << "StoreGate Step: MCTrueSeedFinder retrieves -- " << m_mcEventCollectionKey.key() << endmsg;
    SG::ReadHandle<McEventCollection> mcEventCollection( m_mcEventCollectionKey );
    if ( !mcEventCollection.isValid() ) {
      msg(MSG::DEBUG)
	<< "Could not retrieve McEventCollection/" << m_mcEventCollectionKey.key() << " from StoreGate."  << endmsg;
      return StatusCode::FAILURE;
    }

    interactions.clear();    
    std::vector<Interactions_pair> interactionsColl;
    McEventCollection::const_iterator itr = mcEventCollection->begin();
    for ( ; itr != mcEventCollection->end(); ++itr ) {
      const HepMC::GenEvent* myEvent=(*itr);
      if(!pass( myEvent, mcEventCollection.cptr())) continue; //checked if events is acceptable
      
      
      //get "intensity" (scalar sum ot p_T^2)
      double sum_pt2(0.0);
      HepMC::GenEvent::particle_const_iterator pitr;
      for (pitr = myEvent->particles_begin(); pitr != myEvent->particles_end(); ++pitr ) {
	HepMC::GenParticle *part = (*pitr);
	if(!pass(part, mcEventCollection.cptr())) continue; //select stable charged particles
	sum_pt2 += part->momentum().perp2();
      }
      ATH_MSG_DEBUG("Calculated Sum P_T^2 = " << sum_pt2);
      
      //get position of interaction from first non-zero vertex
      Amg::Vector3D vtxPosition;
      HepMC::GenEvent::vertex_const_iterator Vert = myEvent->vertices_begin();
      msg(MSG::DEBUG) << "Retrieved position  x: " << (*Vert)->position().x()  << 
	" y: " << (*Vert)->position().y() << 
	" z: " << (*Vert)->position().z() << endmsg;
      vtxPosition = Amg::Vector3D((*Vert)->position().x(),
				  (*Vert)->position().y(),
				  (*Vert)->position().z());
      
      //now store info about position and "intensity" (here: scalar sum of p_T^2)
      interactionsColl.emplace_back(sum_pt2, vtxPosition );
    } // end loop over GenEvent

    //now sort the container and store results to interactions
    std::sort(interactionsColl.begin(), interactionsColl.end());
    std::vector<Interactions_pair>::iterator itIp = interactionsColl.begin();
    ATH_MSG_DEBUG("Sorted collection:");
    for (; itIp < interactionsColl.end(); ++itIp) {
      interactions.push_back(itIp->second);
      ATH_MSG_DEBUG("(" << itIp->second.x() << ", " << itIp->second.y() << ", " 
		    << itIp->second.z() << "), SumPt2 = " << itIp->first);
    }

    ATH_MSG_DEBUG("New interactions info stored successfully: " << interactions.size() << " interactions found.");
    return StatusCode::SUCCESS;

  }

  bool MCTrueSeedFinder::pass( const HepMC::GenEvent* evt,
			       const McEventCollection* coll ) const {

    //we select in-time pile-up interactions and hard-scattering, if valid


    bool isEmpty = ( evt->particles_size() == 0 );
    bool isDummy = ( ( evt->event_number() == -1 ) &&
		     ( evt->signal_process_id() == 0 ) );
    if( isDummy ) isEmpty = false;

    if( isEmpty ) return false;
    if( isDummy ) return false;

    // We can't do the further selection without the full truth record:
    if( ! coll ) return true;

    McEventCollection::const_iterator iter = coll->begin();
    McEventCollection::const_iterator end = coll->end();

    if( *iter == evt ) {
      //this is the hard-scattering event (first of the collection)
      return ! m_removeHardScattering;
    }

    int gotzero = 1;
    for( ; iter != end; ++iter ) {
      if( ( ( ( *iter )->event_number() == -1 ) &&
            ( ( *iter )->signal_process_id() == 0 ) ) ) {
	++gotzero;
      }
      if( evt == *iter ) break;
    }

    if( gotzero == 1 && m_removeInTimePileUp ) return false;
    if( gotzero == 2 ) return false; //remove2BCPileUp
    if( gotzero == 3 ) return false; //remove800nsPileUp
    if( gotzero == 4 ) return false; //removeCavernBkg

    return true;
  }

  bool MCTrueSeedFinder::pass( const HepMC::GenParticle* part,
			       const McEventCollection* coll ) const {

    // Check if the particle is coming from a "good" GenEvent:
    if( ! pass( part->parent_event(), coll ) ) return false;

    // Now check for stable particles
    if (part->barcode() < 200000) {
      if( ! TruthHelper::IsGenStable()( part ) ) return false;
      if( ! TruthHelper::IsGenInteracting()( part ) ) return false;
    }

    // finally require it to be charged
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

    return std::abs( charge ) >= 1E-5;
  }


}
