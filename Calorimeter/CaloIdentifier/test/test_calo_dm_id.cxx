/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/Calorimeter/CaloIdentifier/test/test_calo_dm_id.cxx,v 1.6 2006-05-18 11:38:04 fledroit Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/IdentifierHash.h" 
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include <iostream> 
 

static void
check_lar_dm_id_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_lar_dm_id_decoding" << std::endl;
    std::cout << "=========>  check_lar_dm_id_decoding" << std::endl;

    CaloDM_ID dm_id;
    if(dm_id.initialize_from_dictionary (idd)) {
	std::cout << "check_calo_dm_id_decoding - cannot init em from dict" << std::endl;
    }
    const CaloDM_ID* dm_p = &dm_id;

    bool verif = dm_id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    dm_id.set_do_checks(true);
    verif = dm_id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of lar hash calculation

    // zones
    std::vector<Identifier>::const_iterator itId = dm_p->lar_zone_begin();
    std::vector<Identifier>::const_iterator itIdEnd = dm_p->lar_zone_end();
  
    int nZon=0;

    bool error = false;
  
    for(; itId!=itIdEnd;++itId){
	Identifier zoneId = *itId;
	nZon++;
	
	IdentifierHash hashId = dm_p->lar_zone_hash ( zoneId );

	Identifier zoneId2 = dm_p->lar_zone_id ( hashId );
	
	if ( zoneId2 != zoneId ) {
	  std::cout
	    << " lar zoneId2 incorrect: " <<  dm_p->show_to_string(zoneId2) 
	    << "         should be " <<  dm_p->show_to_string(zoneId) 
	    << "           hashId = " << hashId
	    << std::endl;
	  error = true;
	}
	
	int detz  = dm_p->pos_neg_z(zoneId);
	int typ  = dm_p->dmat(zoneId);
	int samp = dm_p->sampling(zoneId);
	int reg  = dm_p->region(zoneId);
       	int eta  = dm_p->eta(zoneId);
	int phi  = dm_p->phi(zoneId);
	
	Identifier zoneId3 = dm_p->zone_id (detz,typ,samp,reg, eta, phi);
	if ( zoneId3 != zoneId ) {
	  std::cout
	    << " zoneId3 incorrect: " <<  dm_p->show_to_string(zoneId3) 
	    << "         should be " <<  dm_p->show_to_string(zoneId) 
	    << std::endl;
	  error = true;
	}

	Identifier regId = dm_p->region_id (zoneId);
	Identifier zoneId4 = dm_p->zone_id (regId, eta, phi);
	if ( zoneId4 != zoneId ) {
	  std::cout << "lar zoneId4 incorrect: " <<  dm_p->show_to_string(zoneId4) 
		    << "         should be " <<  dm_p->show_to_string(zoneId) 
		    << "         regId     " <<  dm_p->show_to_string(regId) 
		    << " input val of eta, phi= " << eta << " " << phi
		    << std::endl;
	  error = true;
	}
    }
    std::cout << "total number of zones (ref=81794) = " << nZon 
	      << ", lar_zone_hash_max= " << dm_p->lar_zone_hash_max() 
	      << std::endl;
    std::cout << " ====================================== " << std::endl;


    // lar regions
    itId = dm_p->lar_region_begin();
    itIdEnd = dm_p->lar_region_end();
  
    int nReg=0;
  
    for(; itId!=itIdEnd;++itId){

      nReg++;
      Identifier regId = *itId;
      
      IdentifierHash hashId = dm_p->lar_region_hash ( regId );

      std::cout << " lar region " << hashId << " etaMin, etaMax, phiMin, phiMax= " 
		<< dm_p ->eta_min(regId) << ", " 
		<< dm_p ->eta_max(regId) << ", " 
		<< dm_p ->phi_min(regId) << ", " 
		<< dm_p ->phi_max(regId) 
		<< std::endl;

      
      Identifier regId2 = dm_p->lar_region_id ( hashId );
      
      if ( regId2 != regId ) {
	std::cout
	  << " lar regId2 incorrect: " <<  dm_p->show_to_string(regId2) 
	  << "         should be "     <<  dm_p->show_to_string(regId) 
	  << "           hasId = "     << hashId
	  << std::endl;
	error = true;
      }
      
     int detz = dm_p->pos_neg_z(regId);
     int typ  = dm_p->dmat(regId);
     int samp = dm_p->sampling(regId);
     int reg  = dm_p->region(regId);
     Identifier regId3 = dm_p->region_id (detz,typ,samp,reg);
     if ( regId3 != regId ) {
       std::cout
	 << " regId3 incorrect: " <<  dm_p->show_to_string(regId3) 
	 << "         should be " <<  dm_p->show_to_string(regId) 
	 << std::endl;
       error = true;
     }
    }
    std::cout << "number of LAr regions (ref=94)= " << nReg 
	      << ", lar_region_hash_max= " << dm_p->lar_region_hash_max() 
	      << std::endl;

    if(!error) {
	std::cout << "LAr DM decoding tests ok" << std::endl;
    }

}


static void
check_tile_dm_id_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_tile_dm_id_decoding" << std::endl;
    std::cout << "=========>  check_tile_dm_id_decoding" << std::endl;

    CaloDM_ID dm_id;
    if(dm_id.initialize_from_dictionary (idd)) {
	std::cout << "check_calo_dm_id_decoding - cannot init em from dict" << std::endl;
    }
    const CaloDM_ID* dm_p = &dm_id;

    bool verif = dm_id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    dm_id.set_do_checks(true);
    verif = dm_id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of tile hash calculation

    // zones
    std::vector<Identifier>::const_iterator itId = dm_p->tile_zone_begin();
    std::vector<Identifier>::const_iterator itIdEnd = dm_p->tile_zone_end();
  
    int nZon=0;

    bool error = false;
  
    for(; itId!=itIdEnd;++itId){
	Identifier zoneId = *itId;
	nZon++;
	
	IdentifierHash hashId = dm_p->tile_zone_hash ( zoneId );

	Identifier zoneId2 = dm_p->tile_zone_id ( hashId );
	
	if ( zoneId2 != zoneId ) {
	  std::cout
	    << " tile zoneId2 incorrect: " <<  dm_p->show_to_string(zoneId2) 
	    << "         should be " <<  dm_p->show_to_string(zoneId) 
	    << "           hashId = " << hashId
	    << std::endl;
	  error = true;
	}
	
	int detz  = dm_p->pos_neg_z(zoneId);
	int typ  = dm_p->dmat(zoneId);
	int samp = dm_p->sampling(zoneId);
	int reg  = dm_p->region(zoneId);
       	int eta  = dm_p->eta(zoneId);
	int phi  = dm_p->phi(zoneId);
	
	Identifier zoneId3 = dm_p->zone_id (detz,typ,samp,reg, eta, phi);
	if ( zoneId3 != zoneId ) {
	  std::cout
	    << " zoneId3 incorrect: " <<  dm_p->show_to_string(zoneId3) 
	    << "         should be " <<  dm_p->show_to_string(zoneId) 
	    << std::endl;
	  error = true;
	}

	Identifier regId = dm_p->region_id (zoneId);
	Identifier zoneId4 = dm_p->zone_id (regId, eta, phi);
	if ( zoneId4 != zoneId ) {
	  std::cout << "tile zoneId4 incorrect: " <<  dm_p->show_to_string(zoneId4) 
		    << "         should be " <<  dm_p->show_to_string(zoneId) 
		    << "         regId     " <<  dm_p->show_to_string(regId) 
		    << " input val of eta, phi= " << eta << " " << phi
		    << std::endl;
	  error = true;
	}
    }
    std::cout << "total number of zones (ref:7426)= " << nZon << ", tile_zone_hash_max= " << dm_p->tile_zone_hash_max() << std::endl;
    std::cout << " ====================================== " << std::endl;


    // tile regions
    itId = dm_p->tile_region_begin();
    itIdEnd = dm_p->tile_region_end();
  
    int nReg=0;
  
    for(; itId!=itIdEnd;++itId){

      nReg++;
      Identifier regId = *itId;
      
      IdentifierHash hashId = dm_p->tile_region_hash ( regId );

      std::cout << " tile region " << hashId << " etaMin, etaMax, phiMin, phiMax= " 
		<< dm_p ->eta_min(regId) << ", " 
		<< dm_p ->eta_max(regId) << ", " 
		<< dm_p ->phi_min(regId) << ", " 
		<< dm_p ->phi_max(regId) 
		<< std::endl;

      
      Identifier regId2 = dm_p->tile_region_id ( hashId );
      
      if ( regId2 != regId ) {
	std::cout
	  << " tile regId2 incorrect: " <<  dm_p->show_to_string(regId2) 
	  << "         should be "     <<  dm_p->show_to_string(regId) 
	  << "           hasId = "     << hashId
	  << std::endl;
	error = true;
      }
      
     int detz = dm_p->pos_neg_z(regId);
     int typ  = dm_p->dmat(regId);
     int samp = dm_p->sampling(regId);
     int reg  = dm_p->region(regId);
     Identifier regId3 = dm_p->region_id (detz,typ,samp,reg);
     if ( regId3 != regId ) {
       std::cout
	 << " regId3 incorrect: " <<  dm_p->show_to_string(regId3) 
	 << "         should be " <<  dm_p->show_to_string(regId) 
	 << std::endl;
       error = true;
     }
    }
    std::cout << "number of Tile regions(ref:20)= " << nReg  << ", tile_region_hash_max= " << dm_p->tile_region_hash_max() << std::endl;

    if(!error) {
	std::cout << "Tile DM decoding tests ok" << std::endl;
    }

    std::cout << "finally check the exception throwing (lar and tile)... " << std::endl;

    try {
      /*Identifier wrongRegionId =*/ dm_p->region_id (-4,-99,0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongRegionId =*/ dm_p->region_id (5,-99,0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongRegionId =*/ dm_p->region_id (0,-99,0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }

    try {
      /*Identifier wrongZoneId =*/ dm_p->zone_id (4,99,0,0, 0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongZoneId =*/ dm_p->zone_id (-5,99,0,0, 0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongZoneId =*/ dm_p->zone_id (0,99,0,0, 0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }

    try {
      Identifier goodRegionId = dm_p->region_id (4,1,0,0); 
      /*Identifier wrongZoneId =*/ dm_p->zone_id (goodRegionId,0,-99); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    std::cout << "... which is ok " << std::endl;
}



int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  
    IdDictParser parser;  
    IdDictMgr& idd = parser.parse (argv[1]);  
    std::cout << "got dict mgr " << std::endl;

    check_lar_dm_id_decoding(idd);
    check_tile_dm_id_decoding(idd);
     
    return 0;  
}  
  
 
 
 
 
 
