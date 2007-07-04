/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegPatAssMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONSEGMENT_MUONSEGPATASSMAP_H
#define MUONSEGMENT_MUONSEGPATASSMAP_H

#include "DataModel/AssociationMap.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "CLIDSvc/CLASS_DEF.h"


namespace Muon {

// See doc/mainpage.h for usage instructions
  
/**
   Class stores associations between a MuonPatternCombination and a MuonSegmentCombination
   @author Niels van Eldik <niels.van.eldik@cern.ch>
   
   @section MuonSegPatAssMapUsage MuonSegPatAssMap Usage

   Creating a new map:

   @code
   // create a new association map to store the link between pattern and segment combinations
   Muon::MuonSegPatAssMap* assMap = new Muon::MuonSegPatAssMap();


   // pointer to the MuonPatternCombinationCollection...
   const MuonPatternCombinationCollection* patCombiCol = ....;

   // pointer to the MuonSegmentCombinationCollection
   MuonSegmentCombinationCollection* segCombiCol = .....;

   // calculate the position of the current MuonPatternCombination and MuonSegmentCombination
   int patCombiIndex = ....;
   int segCombiIndex = ....;

   // create link between pattern and segment combination
   assMap->addAssociation( patterns, patIndex , combiCol, segCombiIndex );

   // storing the map for further usage
   if( m_storeGate->record(assMap,"MyMuonSegPatAssociations").isSuccess() ){
     log << MSG::DEBUG << "stored MuonSegPatAssociations "  << endreq;
   }else{
     log << MSG::DEBUG << "Failed to store MuonSegPatAssociations " << endreq;
   }
   @endcode

   Using the map:

   @code
   // retieve your map
   const Muon::MuonSegPatAssMap* assMap = 0;
   StatusCode sc = m_storeGate->retrieve(assMap,"MyMuonSegPatAssociations");
   if (sc.isFailure() ) {
   log << MSG::ERROR << " Could not find MuonSegPatAssociations at " << "MyMuonSegPatAssociations" <<endreq;
     return StatusCode::SUCCESS;
   }else{
     log << MSG::INFO << " Retrieved MuonSegPatAssociations " << endreq;
   }



   // the map supports the assocation of multiple patCombis with a given segCombi and visa versa

   // find associated patCombis for a given segCombi

   // get pointer to your combi..
   const Muon::MuonSegmentCombination* segCombi = ...;

   // look up
   Muon::MuonSegPatAssMap::object_list assos; 
   assMap->getObjects( combi, assos );
   std::cout << " found association " <<  assos.size() << std::endl;
   Muon::MuonSegPatAssMap::object_list::iterator assit = assos.begin();
   for( ; assit != assos.end(); ++ assit ){
      std::cout << " segCombi " << segCombi << "   patCombi " << *assit << std::endl;
   }

   // find associated segCombis for a given patCombi

   // get pointer to your combi..
   const Muon::MuonPatternCombination* patCombi = ...;

   Muon::MuonSegPatAssMap::asso_list assos; 
   assMap->getAssociations( patCombi, assos );
   std::cout << " found association " <<  assos.size() << std::endl;
   Muon::MuonSegPatAssMap::asso_list::iterator assit = assos.begin();
   for( ; assit != assos.end(); ++ assit ){
      std::cout << " patCombi " << patCombi << "   segCombi " << *assit << std::endl;
   }
   @endcode
   
*/
  class MuonSegPatAssMap : public AssociationMap< MuonPatternCombinationCollection, MuonSegmentCombinationCollection > {
  public:
    MuonSegPatAssMap() : AssociationMap< MuonPatternCombinationCollection, MuonSegmentCombinationCollection >()
      { 
      }
      
      virtual ~MuonSegPatAssMap() {}
      
      
      
  private:

  };



}

CLASS_DEF( Muon::MuonSegPatAssMap, 102492935, 1 )


#endif
