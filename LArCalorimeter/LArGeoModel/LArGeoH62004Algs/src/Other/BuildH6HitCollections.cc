/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// BuildH6HitCollections
// 10-Nov-2003 Bill Seligman

// Transform the internal LArG4 hit collections into the external form
// needed by AtlasG4Sim.

// AtlasG4Sim hit classes
#include "LArGeoH62004Algs/BuildH6HitCollections.h"
#include "LArSimEvent/LArHit.h"
#include "CaloSimEvent/CaloCalibrationHit.h"

// LArG4 hit classes
#include "LArG4Code/LArG4Hit.h"
#include "LArG4Code/CalibrationHit.h"
#include "LArG4Code/LArG4Identifier.h"

// ID classes...
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "Identifier/Identifier.h"

// Athena Service classes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "StoreGate/StoreGateSvc.h"

// Geant4 classes
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "globals.hh"

#include <iostream>
#include <sstream>
#include <vector>

#define DEBUG_HITS

namespace LArG4 {

  BuildH6HitCollections::BuildH6HitCollections()
  {
    // Constructor: Initialize the identifier helper classes.

    // Fetch the Athena services.  Save the message service pointer, so
    // we don't have to continually re-fetch it.
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
    m_msgSvc = 0;
    StatusCode result = svcLocator->service("MessageSvc", m_msgSvc);

    if ( !result.isSuccess()  ||  m_msgSvc == 0 )
      {
        std::cerr << "LArGeoH62004Algs  BuildH6HitCollections(): could not initialize MessageSvc!" << std::endl;
        throw GaudiException("Could not initalize MessageSvc","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    // Now fetch the detector store from StoreGate.
    StoreGateSvc* detectorStore = 0;
    result = svcLocator->service("DetectorStore", detectorStore);

    if ( !result.isSuccess()  ||  detectorStore == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not access detector store - " << result << endmsg;
        throw GaudiException("Could not get DetectorStore","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    const CaloIdManager* caloIdManager;
    result = detectorStore->retrieve(caloIdManager);

    if ( !result.isSuccess()  ||  caloIdManager == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not access Calo ID manager" << endmsg;
        throw GaudiException("Could not get CaloIdManager","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    // Finally (whew!) get the ID helpers themselves.
    m_larEmID = caloIdManager->getEM_ID();
    if ( m_larEmID == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not get LAr EM ID helper" << endmsg;
        throw GaudiException("Invalid LAr EM ID helper","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    m_larFcalID  = caloIdManager->getFCAL_ID();
    if ( m_larFcalID == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not get LAr FCAL ID helper" << endmsg;
        throw GaudiException("Invalid LAr FCAL ID helper","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    m_larHecID   = caloIdManager->getHEC_ID();
    if ( m_larHecID == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not get LAr HEC ID helper" << endmsg;
        throw GaudiException("Invalid LAr HEC ID helper","LArGeoH62004Algs",StatusCode::FAILURE);
      }

    m_caloDmID   = caloIdManager->getDM_ID();
    if ( m_caloDmID == 0 )
      {
        MsgStream log (m_msgSvc,"LArGeoH62004Algs");
        log << MSG::FATAL << "BuildH6HitCollections(): could not get Calo DM ID helper" << endmsg;
        throw GaudiException("Invalid Calo DM ID helper","LArGeoH62004Algs",StatusCode::FAILURE);
      }
  }

  BuildH6HitCollections::~BuildH6HitCollections()
  {;}



  // This looks a little confusing: The CreateHitCollections method
  // returns a vector of hitCollection_t*.

  void BuildH6HitCollections::ChangeHitCollections(LArHitContainer* emecHitContainer, LArHitContainer* hecHitContainer)
  {

#ifdef DEBUG_HITS
    MsgStream log (m_msgSvc,"LArGeoH62004Algs");

    log << MSG::DEBUG << "BuildH6HitCollections::ChangeHitCollections emec has "
	<< emecHitContainer->size()
	<< " hits nad hec has "
	<< hecHitContainer->size() << " hits "
	<< endmsg;
#endif

/*

		// First loop over emec
		    // Get the "i-th" hit in the collection.
		    LArG4Hit* larG4hit = (*collection)[i];

		    // Get the internal LArG4 Identifier used for the
		    // hit.
		    LArG4Identifier larG4Ident = larG4hit->getIdentifier();

		    // Try to convert the LArG4 Identifier into an
		    // Athena Identifier.
		    if(larG4Ident[0] == 10) continue; // Skip the DmId in standard containers
		    // Hack to get the correct id in EMEC and HEC
		    Identifier id;
		    if(larG4Ident[1] == 1)  { // EMEC Module
#ifdef DEBUG_HITS
		          log << MSG::DEBUG << "BuildH6HitCollections: adding 20 to phi" << endmsg;
#endif
                          LArG4Identifier idc;
                          idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
                             << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
                             << larG4Ident[6] + 20;
			  id = ConvertID( idc );
		    } else if (larG4Ident[1] == 2) { // HEC module
		           LArG4Identifier idc;
		           // Correct also for hits outside the dictionary
		           if(larG4Ident[4] == 0) {
			         if(larG4Ident[3]==0 && larG4Ident[5] < 6) larG4Ident[5]=6;
			         if(larG4Ident[3]==2 && larG4Ident[5] < 8) larG4Ident[5]=8;
			         idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
			            << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
			            << larG4Ident[6] + 16;
			    } else {
			         if(larG4Ident[3]==1 && larG4Ident[5] > 2) larG4Ident[5]=2;
			         idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
			            << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
			            << larG4Ident[6] + 8;
			    }

                            id = ConvertID( idc );
		    } else {
		            id = ConvertID( larG4Ident );
		    }

		    // Did we successfully create an Identifier?
		    if ( id.is_valid() ) {
#ifdef DEBUG_HITS
			log << MSG::VERBOSE
			    << "BuildH6HitCollections: hit is valid - "
			    << m_larEmID->show_to_string(id)
			    << endmsg;
#endif
			// Build the hit itself.  Note that we scale
			// whatever internal units Geant4 uses to the
			// official ATLAS units of MeV and ns.  (As it
			// happens, these are also Geant4's internal
			// units, so we're just dividing by 1.0).
			LArHit* hit = new LArHit( id,
						  larG4hit->energy() / MeV,
						  larG4hit->time()   / ns );

			// Put the hit into the appropriate
			// Athena-style hit collection.  Note that for
			// regular hits, there are no "dead-material"
			// hits; those only occur in calibration studies.

			if ( m_larEmID->is_lar_em( id ) )
			  {
			    if ( m_larEmID->is_em_endcap(id) )
			      {
#ifdef DEBUG_HITS
				log << MSG::VERBOSE << "   Add to EMEC" << endmsg;
#endif
				emecHitCollection->push_back( hit );
			      }
			  }
			else if ( m_larFcalID->is_lar_fcal( id ) )
			  {
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to FCAL" << endmsg;
#endif
			    fcalHitCollection->push_back( hit );
			  }
			else if ( m_larHecID->is_lar_hec( id ) )
			  {
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to HEC" << endmsg;
#endif
			    hecHitCollection->push_back( hit );
			  }
		      }
		    else
		      {
			MsgStream log (m_msgSvc,"LArGeoH62004Algs");
			log << MSG::ERROR 
			    << "Run=" << runNumber
			    << " Event=" << eventNumber
			    << ", Hit " << i
			    << " in collection '"
			    << collection->GetName()
			    << "' is invalid; id=" 
			    << std::string(larG4Ident)
			    << endmsg;

			// What do we do if we can't build the hit?  Keep running, or
			// crash and burn?

		      }
*/
    return; 
  }




  std::vector<BuildH6HitCollections::calibCollection_t*> BuildH6HitCollections::CreateCalibrationHitCollections()
  {
    // Start by creating the hit collections that will be returned by
    // this routine.  Memory management note: the pointers we create
    // here will be adopted by the calling routine.
    calibCollection_t* activeHitCollection   = new calibCollection_t("LArCalibrationHitActive");
    calibCollection_t* inactiveHitCollection = new calibCollection_t("LArCalibrationHitInactive");
    calibCollection_t* deadHitCollection     = new calibCollection_t("LArCalibrationHitDeadMaterial");

    // Get the current Geant4 event.
    G4RunManager* rm = G4RunManager::GetRunManager();
    const G4Event* event = rm->GetCurrentEvent();
    G4int runNumber = rm->GetCurrentRun()->GetRunID();
    G4int eventNumber = event->GetEventID();

#ifdef DEBUG_HITS
    MsgStream log (m_msgSvc,"BuildH6HitCollections");

    log << MSG::DEBUG << "CreateCalibrationHitCollections Run "
	<< runNumber
	<< " Event "
	<< eventNumber
	<< endmsg;
#endif

    // Access the hits of this event.  First, get the pointer to the
    // set of hit collections of this event.
    G4HCofThisEvent* HCofEvent = event->GetHCofThisEvent();

    // It's possible (especially if detectors are turned off for
    // testing) that there are no hits in this event at all.

    if ( HCofEvent != 0)
      {
	// Loop through all the hit collections and convert all the
	// hits.

	// This is the maximum number of hit collections that might
	// have been stored; it is not nessarily the actual number of
	// hit collections in this event:
	G4int maxNumberHitCollections = HCofEvent->GetCapacity();

	for ( G4int h = 0; h != maxNumberHitCollections; h++ )
	  {
	    // Check to see if a hit collection with the ID number "h"
	    // exists.
	    G4VHitsCollection* hc = HCofEvent->GetHC(h);

	    // If hc==0, it means that a hit collection with ID
	    // number h does not exist.  (It can also mean that the user
	    // turned off the sensitivity of a particular detector
	    // interactively with a "/hits/inactivate" command.)

	    CalibrationHitsCollection* collection = 0;
	    if ( hc != 0 )
	      // See if this is a LArG4 calibration hits collection.
	      collection = dynamic_cast<CalibrationHitsCollection*>( hc );

	    if (collection != 0 )
	      {
		// We want to loop through all the hits.  Get the number of hits
		// in this hit collection.
		G4int numberHits = collection->entries();

		// A number of tests and messages use the collection name.
		G4String collectionName = collection->GetName();

#ifdef DEBUG_HITS
		log << MSG::DEBUG << "Collection '"
		    << collectionName
		    << "' has "
		    << numberHits
		    << " hits"
		    << endmsg;
#endif

		for (int i = 0; i < numberHits; i++)
		  {
		    // Get the "i-th" hit in the collection.
		    CalibrationHit* larG4hit = (*collection)[i];

		    // Get the internal LArG4 Identifier used for the
		    // hit.
		    LArG4Identifier larG4Ident = larG4hit->identifier();

		    // Try to convert the LArG4 Identifier into an
		    // Athena Identifier.
		    Identifier id;
		    // Hack to get the correct id in EMEC and HEC
                    if((larG4Ident[0] != 10) && (larG4Ident[1] == 1) ){

                       LArG4Identifier idc;
                       idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
                          << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
                          << larG4Ident[6] + 20;

                       id = ConvertID( idc );

                    } else if ((larG4Ident[0] != 10) && (larG4Ident[1] == 2) ) {
                       LArG4Identifier idc;
                       // Correct also for hits outside the dictionary
                       if(larG4Ident[4] == 0) {
                          if(larG4Ident[3]==0 && larG4Ident[5] < 6) larG4Ident[5]=6;
                          if(larG4Ident[3]==2 && larG4Ident[5] < 8) larG4Ident[5]=8;
                          idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
                          << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
                          << larG4Ident[6] + 16;
                       } else {
                          if(larG4Ident[3]==1 && larG4Ident[5] > 2) larG4Ident[5]=2;
                          idc<< larG4Ident[0]<<larG4Ident[1]<<larG4Ident[2]
                          << larG4Ident[3]<<larG4Ident[4]<<larG4Ident[5]
                          << larG4Ident[6] + 8;
                       }

                       id = ConvertID( idc );

                    } else {

		       id = ConvertID( larG4Ident );

		    }

		    // Debug hit problems.
		    Identifier::value_type compactID = Identifier::value_type(id);
		    Identifier::value_type shiftedID = compactID / 04000000;
		    if ( shiftedID == 07777 || shiftedID == 07774 )
		      {
			std::ostringstream octID;
			octID << std::oct << compactID << "[" << shiftedID << "]" << std::dec;
			MsgStream log (m_msgSvc,"LArGeoH62004Algs");
			log << MSG::DEBUG
			    << "***BHC invalid hit ID="
			    << octID.str()
			    << " (oct), hit "
			    << i
			    << " in collection '"
			    << collectionName
			    << "'" << endmsg;
			log << MSG::DEBUG
			    << "***   LArG4 hit ID="
			    << std::string(larG4Ident)
			    << endmsg;
		      }


		    // Did we successfully create an Identifier?
		    if ( id.is_valid() )
		      {
#ifdef DEBUG_HITS
			log << MSG::VERBOSE
			    << "hit is valid - "
			    << m_larEmID->show_to_string(id)
			    << endmsg;
#endif
			// Build the hit itself.  Note that we scale
			// whatever internal units Geant4 uses to the
			// official ATLAS units of MeV.  (As it
			// happens, these are also Geant4's internal
			// units, so we're just dividing by 1.0).
			CaloCalibrationHit* hit
			  = new CaloCalibrationHit( id,
						    larG4hit->energy(0) / MeV,
						    larG4hit->energy(1) / MeV,
						    larG4hit->energy(2) / MeV,
						    larG4hit->energy(3) / MeV );


			// Put the hit into the appropriate
			// Athena-style hit collection.  Get the name
			// of the collection to see if it should
			// contain active-, inactive-, or
			// dead-material hits.

			// 26-Apr-2005 WGS: Because of the way the
			// some calculators are designed, it's
			// possible for the active or inactive hit
			// containers to contain dead hits.  So let's
			// always put dead hits in the correct
			// container.

			if ( m_caloDmID->is_lar_dm(id) || m_caloDmID->is_tile_dm(id) )
			  {
			    deadHitCollection->push_back( hit );
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to Dead" << endmsg;
#endif
			  }
			else if ( collectionName.contains("::Inactive") )
			  {
			    inactiveHitCollection->push_back( hit );
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to Inactive" << endmsg;
#endif
			  }
			else if ( collectionName.contains("::Dead") )
			  {
			    // If we get here, then we have a 'live'
			    // hit in a dead-hit container.

			    // This can happen if a "dead" volume
			    // pokes into a region that's
			    // geometrically part of a readout cell.

			    // If this happens, place the hit in the
			    // inactive container.

			    inactiveHitCollection->push_back( hit );
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to Inactive" << endmsg;
#endif
			  }
			else
			  {
			    // If it's not dead or inactive, then it's active.
			    activeHitCollection->push_back( hit );
#ifdef DEBUG_HITS
			    log << MSG::VERBOSE << "   Add to Active" << endmsg;
#endif
			  }
		      } // id is valid
		    else
		      {
			MsgStream log (m_msgSvc,"LArGeoH62004Algs");
			log << MSG::ERROR 
			    << "Run=" << runNumber
			    << " Event=" << eventNumber
			    << ", Hit " << i
			    << " in collection '"
			    << collection->GetName()
			    << "' is invalid; id=" 
			    << std::string(larG4Ident)
			    << endmsg;

			// What do we do if we can't build the hit?  Keep running, or
			// crash and burn?

			// Here's the code for crash and burn.  For now, hope that someone
			// is smart enough to read the errors, and keep going.
			/*
			  {
			  log << MSG::FATAL
			  << "BuildH6HitCollections: Invalid hit identifier.  Aborting."
			  << endmsg;
			  throw GaudiException("Could not convert hit","LArGeoH62004Algs",StatusCode::FAILURE);
			  }
			*/
		      } 
		  } // for each hit in G4 hit collection
	      } // endif: G4 collection turned on
	  } // end: G4 collection loop
      } // endif: there are any LArG4 hit collections

    // We've filled all the Athena LAr calibration hit collections.
    // Put them into a vector, and return them.
    std::vector<calibCollection_t*> calibCollectionVector;

    // 29-Mar-05 WGS: It turns out that Digitization chokes if any
    // collections it expects are not written to POOL.  On the other
    // hand, if it's not a calibration run, there's no point in
    // writing three empty hit containers for every event.
    // Compromise: Write all the calibration hit collections (even
    // zero-length ones), but only if at least one of them is
    // non-zero-length.

    if ( activeHitCollection->size()   != 0   ||
         inactiveHitCollection->size() != 0   ||
         deadHitCollection->size()     != 0 )
      {
	calibCollectionVector.push_back( activeHitCollection  );
	calibCollectionVector.push_back( inactiveHitCollection );
	calibCollectionVector.push_back( deadHitCollection );
      }

    return calibCollectionVector;
  }



  Identifier BuildH6HitCollections::ConvertID( const LArG4Identifier& a_ident )
  {
    // This is real meat of the hit-conversion
    // procedure: change a LArG4Identifier (a vector
    // of short ints) into a full-fledged Athena
    // Identifier.

    // The LArG4Identifier is basically set in the
    // various calculators in the simulation.  If the
    // structure of the ATLAS readout channels
    // changes, both the calculators and the following
    // code may have to be changed.

#ifdef DEBUG_HITS
    // Debug output:
    MsgStream log (m_msgSvc,"LArGeoH62004Algs");
    log << MSG::VERBOSE << "ID=/";
    for (unsigned short int f=0; f < a_ident.fields(); f++)
      {
	log << a_ident[f] << "/";
      }
    log << endmsg;
#endif

    // Define a new Athena Identifier object.  Note
    // that since we're not providing an argument to
    // the constructor, this Identifier is in an invalid
    // state.
    Identifier id;

    // Once we've figured out the type of the
    // identifier, create its compact form.

    if ( a_ident[0] == 4 )
      {
#ifdef DEBUG_HITS
	log << MSG::VERBOSE << "is_lar" << endmsg;
#endif

	if (a_ident[1] == 1 )
	  {
#ifdef DEBUG_HITS
	    log << MSG::VERBOSE  << "is_lar_em" << endmsg;
#endif
	    // Use the Athena classes to create the
	    // Identifier, handling any exceptions
	    // along the way.
	    try {
	      id = m_larEmID->channel_id(a_ident[2],  // barrel_ec
					 a_ident[3],  // sampling
					 a_ident[4],  // region
					 a_ident[5],  // eta
					 a_ident[6]); // phi
	    }
	    catch ( LArID_Exception& e )
	      {
		MsgStream log (m_msgSvc,"LArGeoH62004Algs");
		log << MSG::ERROR
		    << "BuildH6HitCollections: LArEM_ID error code " << e.code() << " "
		    << (std::string) e
		    << endmsg;
	      }

	  }
	else if ( a_ident[1] == 2 )
	  {
#ifdef DEBUG_HITS
	    log << MSG::VERBOSE  << "is_em_hec" << endmsg;
#endif
	    try {
	      id = m_larHecID->channel_id(a_ident[2],  // zSide
					  a_ident[3],  // sampling
					  a_ident[4],  // region
					  a_ident[5],  // eta
					  a_ident[6]); // phi
	    }
	    catch ( LArID_Exception& e )
	      {
		MsgStream log (m_msgSvc,"LArGeoH62004Algs");
		log << MSG::ERROR
		    << "BuildH6HitCollections: LArHEC_ID error code " << e.code() << " "
		    << (std::string) e
		    << endmsg;
	      }

	  }
	else if ( a_ident[1] == 3 )
	  {
#ifdef DEBUG_HITS
	    log << MSG::VERBOSE  << "is_em_fcal" << endmsg;
#endif
	    try {
	      id = m_larFcalID->channel_id(a_ident[2],  // zSide
					   a_ident[3],  // sampling
					   a_ident[4],  // eta
					   a_ident[5]); // phi
	    }
	    catch ( LArID_Exception& e )
	      {
		MsgStream log (m_msgSvc,"LArGeoH62004Algs");
		log << MSG::ERROR
		    << "BuildH6HitCollections: LArFCAL_ID error code " << e.code() << " "
		    << (std::string) e
		    << endmsg;
	      }
	  }
      }
    else if ( a_ident[0] == 10 )
      {
	// This is a dead-material identifier
#ifdef DEBUG_HITS
	log << MSG::VERBOSE  << "is_calo_dm" << endmsg;
#endif
	try {
	  id = m_caloDmID->zone_id(a_ident[1],  // zSide
				   a_ident[2],  // type
				   a_ident[3],  // sampling
				   a_ident[4],  // region
				   a_ident[5],  // eta
				   a_ident[6]); // phi
	}
	catch ( CaloID_Exception& e )
	  {
	    MsgStream log (m_msgSvc,"LArGeoH62004Algs");
	    log << MSG::ERROR
		<< "BuildH6HitCollections: CaloDM_ID error code " << e.code() << " "
		<< (std::string) e
		<< endmsg;
	  }
      }

    return id;
  }

} // namespace LArG4
