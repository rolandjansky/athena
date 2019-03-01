/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// $Header: /build/atlas/cvs/atlas/offline/Calorimeter/CaloIdentifier/test/test_lvl1_id.cxx,v 1.7 2006-12-13 23:03:15 ssnyder Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/IdentifierHash.h" 
#include "CaloIdentifier/CaloLVL1_ID.h"
#include <iostream> 
#include <cassert>

static void
test_id(const CaloLVL1_ID& idh, Identifier id)
{
    if(idh.is_tile	(id)) std::cout << "is_tile" << std::endl;
    if(idh.is_indet    	(id)) std::cout << "is_indet" << std::endl;
    if(idh.is_lar 	(id)) std::cout << "is_lar" << std::endl;
    if(idh.is_muon	(id)) std::cout << "is_muon" << std::endl;
    if(idh.is_calo	(id)) std::cout << "is_calo" << std::endl;
    if(idh.is_lvl1_trig_towers(id)) std::cout << "is_lvl" << std::endl;
    if(idh.is_pixel	(id)) std::cout << "is_pixel" << std::endl;
    if(idh.is_sct	(id)) std::cout << "is_sct" << std::endl;
    if(idh.is_trt	(id)) std::cout << "is_trt" << std::endl;
    if(idh.is_lar_em 	(id)) std::cout << "is_lar_em" << std::endl;
    if(idh.is_lar_hec	(id)) std::cout << "is_lar_hec" << std::endl;
    if(idh.is_lar_fcal	(id)) std::cout << "is_lar_fcal" << std::endl;
    if(idh.is_lar_dm	(id)) std::cout << "is_lar_dm" << std::endl;
    if(idh.is_tile_dm	(id)) std::cout << "is_tile_dm" << std::endl;
    if(idh.is_mdt	(id)) std::cout << "is_mdt" << std::endl;
    if(idh.is_csc	(id)) std::cout << "is_csc" << std::endl;
    if(idh.is_tgc	(id)) std::cout << "is_tgc" << std::endl;
    if(idh.is_rpc	(id)) std::cout << "is_rpc" << std::endl;
}

static void
check_lvl1_id_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_lvl1_id_decoding" << std::endl;
    std::cout << "=========>  check_lvl1_id_decoding" << std::endl;

    CaloLVL1_ID lvl1_id;
    if(lvl1_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lvl1_id_decoding - cannot init lvl1 from dict" << std::endl;
    }

    // Test LVL1
    Identifier id = lvl1_id.layer_id(-1, 1, 2, 0, 5, 2);
    std::cout << "id                  " << lvl1_id.show_to_string(id) 
	      << " " <<  id  << std::endl;
    test_id(lvl1_id, id);


    // Test LVL1
    id = lvl1_id.lar_lvl1();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);


    // Test LAr DM
    id = lvl1_id.lar_dm();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    // Test Tile DM
    id = lvl1_id.tile_dm();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);


    id = lvl1_id.indet();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.lar();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.tile();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    std::cout << "is_tile " << lvl1_id.is_tile(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.muon();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.calo();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.pixel();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.sct();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.trt();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.lar_em();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.lar_hec();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.lar_fcal();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.mdt();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.csc();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.rpc();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);

    id = lvl1_id.tgc();
    std::cout << "id                  " << lvl1_id.show_to_string(id) << std::endl;
    test_id(lvl1_id, id);
}

static void
check_lvl1_helper(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_lvl1_helper" << std::endl;
    std::cout << "=========>  check_lvl1_helper" << std::endl;

    CaloLVL1_ID lvl1Id;
    if(lvl1Id.initialize_from_dictionary (idd)) {
	std::cout << "check_lvl1_helper - cannot init lvl1 from dict" << std::endl;
    }
    const CaloLVL1_ID* lvl1_id = &lvl1Id;


    bool verif = lvl1Id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    lvl1Id.set_do_checks(true);
    verif = lvl1Id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    IdContext towerContext = lvl1_id->tower_context();
    IdContext layerContext = lvl1_id->layer_context();
    IdContext regionContext = lvl1_id->region_context();

    // regions
    std::vector<Identifier>::const_iterator itId = lvl1_id->region_begin();
    std::vector<Identifier>::const_iterator itIdEnd = lvl1_id->region_end();
  
    bool error = false;
    int nreg=0;
  
    for(; itId!=itIdEnd;++itId){
	Identifier regId = *itId;
	//	std::cout << " ================= NREG = " << nreg << std::endl;
	nreg++;
	
	//
	// ... in tower context
	//
	int posNeg   = lvl1_id->pos_neg_z(regId);
	int sampling = lvl1_id->sampling(regId);
	int region   = lvl1_id->region(regId);
	
	Identifier regId2 = lvl1_id->region_id (posNeg, sampling, region) ;

	if (regId2 != regId) {
	  std::cout 
	      << "error in region_id "
	      << " regId  " << lvl1_id->show_to_string(regId) 
	      << " regId2 " << lvl1_id->show_to_string(regId2) 
	      << std::endl;
	  error = true;
	}
	std::cout << "region: " << lvl1_id->show_to_string(regId) << std::endl;
	std::cout << "etamin, etamax=  " << lvl1_id->eta_min(regId)
		  << " " << lvl1_id->eta_max(regId) << std::endl;
	std::cout << "phimax, laymax=  " << lvl1_id->phi_max(regId)
		  << " " << lvl1_id->layer_max(regId) << std::endl;
    }
    std::cout << "number of regions= " << nreg  << ", region_hash_max= " << lvl1_id->region_hash_max() << std::endl;


    // towers
    itId = lvl1_id->tower_begin();
    itIdEnd = lvl1_id->tower_end();
  
    int nChan=0;
  
    for(; itId!=itIdEnd;++itId){
	Identifier ttId = *itId;
	//	std::cout << " ================= NCHAN = " << nChan << std::endl;
	nChan++;
	
	//
	// ... in tower context
	//
	int posNeg   = lvl1_id->pos_neg_z(ttId);
	int sampling = lvl1_id->sampling(ttId);
	int region   = lvl1_id->region(ttId);
	int eta = lvl1_id->eta(ttId);
	int phi = lvl1_id->phi(ttId);
	
	Identifier regId = lvl1_id->region_id (posNeg, sampling, region) ;

	IdentifierHash regHash;
	int not_ok = lvl1_id->get_hash ( regId, regHash, &regionContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_hash"
	      << " regId " << lvl1_id->show_to_string(regId) 
	      << std::endl;
	  error = true;
	}
	Identifier regId2;
	not_ok = lvl1_id->get_id (regHash, regId2, &regionContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_id (region constext)"
	      << " regId2 " << lvl1_id->show_to_string(regId2) 
	      << " regHash " <<  regHash 
	      << std::endl;
		error = true;
	}
	Identifier regId3 = lvl1_id->region_id(regHash);
	if (regId3 != regId) {
	  std::cout 
	      << "error in region_id(regHash)"
	      << " regId  " << lvl1_id->show_to_string(regId) 
	      << " regId3 " << lvl1_id->show_to_string(regId3) 
	      << " regHash = " << regHash
	      << std::endl;
	  error = true;
	}


	Identifier ttId3 = lvl1_id->tower_id (regId, eta, phi) ;
	if (ttId3 != ttId) {
	  std::cout 
	      << "error in tower_id(regId, eta, phi)"
	      << " ttId  " << lvl1_id->show_to_string(ttId) 
	      << " ttId3 " << lvl1_id->show_to_string(ttId3) 
	      << "input fields = " << lvl1_id->show_to_string(regId) << " " << eta << " " << phi
	      << std::endl;
	  error = true;
	}
	Identifier ttId2 = lvl1_id->tower_id (posNeg, sampling, region, eta, phi) ;
	if (ttId2 != ttId) {
	  std::cout 
	      << "error in tower_id(posNeg, sampling, region, eta, phi)"
	      << " ttId  " << lvl1_id->show_to_string(ttId) 
	      << " ttId2 " << lvl1_id->show_to_string(ttId2) 
	      << "input fields = " << posNeg << " " << sampling << " " << region << " " << eta << " " << phi
	      << std::endl;
	  error = true;
	}

	//
	// ... now test 'return trip' to hash
	//
	// ... in tower context
	//
	IdentifierHash hashId;
	
	not_ok = lvl1_id->get_hash ( ttId, hashId, &towerContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_hash"
	      << " ttId " << lvl1_id->show_to_string(ttId) 
	      << std::endl;
	  error = true;
	}

	assert (lvl1_id->tower_hash(ttId) == hashId);
	      
	not_ok =  lvl1_id->get_id ( hashId, ttId2, &towerContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_id"
	      << " id " << lvl1_id->show_to_string(ttId2) 
	      << " hash " <<  hashId 
	      << std::endl;
		error = true;
	}
	assert (ttId == ttId2);
	
	ttId2 =  lvl1_id->tower_id(hashId);
	assert (ttId == ttId2);
	
    }
    std::cout << "number of towers= " << nChan  << ", tower_hash_max= " << lvl1_id->tower_hash_max() << std::endl;
    std::cout << ", region_hash_max= " << lvl1_id->region_hash_max() << std::endl;

    //
    // ... then in layer context
    //
    
    itId = lvl1_id->layer_begin();
    itIdEnd = lvl1_id->layer_end();
  
    nChan=0;
  
    for(; itId!=itIdEnd;++itId){
	Identifier layId = *itId;
	//	std::cout << " ================= NCHAN = " << nChan << std::endl;
	nChan++;
	
	//
	// ... in layer context
	//
	int posNeg   = lvl1_id->pos_neg_z(layId);
	int sampling = lvl1_id->sampling(layId);
	int region   = lvl1_id->region(layId);
	int eta = lvl1_id->eta(layId);
	int phi = lvl1_id->phi(layId);
	int lay = lvl1_id->layer(layId);
	
	Identifier ExtTtId ;
	try {
	  ExtTtId = lvl1_id->layer_id (posNeg, sampling, region, eta, phi, lay) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  std::cout 
	      <<  " CaloId exception 1 " 
	      << (std::string)except
	      << std::endl;
	}

	Identifier ttId = lvl1_id->tower_id(layId) ;
	Identifier ExtTtId2 ;
	try {
	  ExtTtId2 = lvl1_id->layer_id (ttId, lay) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  std::cout 
	      <<  " CaloId exception 2 " 
	      << (std::string)except
	      << std::endl;
	}
	assert ( ExtTtId2 == ExtTtId);
		
	//
	// ... then return trip to hash in layer context
	//
	IdentifierHash hashId;
	int not_ok = lvl1_id->get_hash ( ExtTtId, hashId, &layerContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_hash"
	      << " ExtTtId " << lvl1_id->show_to_string(ExtTtId) 
	      << " hashId " << hashId
	      << std::endl;
	  error = true;
	}
	
	not_ok =  lvl1_id->get_id ( hashId, ExtTtId2, &layerContext);
	if(not_ok) {
	  std::cout 
	      << "error in get_id"
	      << " id " << lvl1_id->show_to_string(ExtTtId2) 
	      << " hash " <<  hashId 
	      << std::endl;
	  error = true;
	}
	
	assert (ExtTtId == ExtTtId2);
		
    } // end loop on lay

    std::cout << "number of layers= " << nChan  << ", layer_hash_max= " << lvl1_id->layer_hash_max() << std::endl;

    if(!error) {
	std::cout << "calo decoding tests ok" << std::endl;
    }

    std::cout << "finally check the exception throwing (lar and tile)... " << std::endl;

    try {
      /*Identifier wrongRegionId =*/ lvl1_id->region_id (0,-99,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongTowerId =*/ lvl1_id->tower_id (0,-99,0, 0,0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }

    try {
      /*Identifier wrongLayerId =*/ lvl1_id->layer_id (4,99,0, 0,0, 0); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodRegionId = lvl1_id->region_id (-1,0,0); 
      /*Identifier wrongTowerId =*/ lvl1_id->tower_id (goodRegionId,0,-99); 
    }
    catch(CaloID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodTowerId = lvl1_id->tower_id (-1,0,0, 0,0); 
      std::cout << lvl1_id->show_to_string(goodTowerId) << std::endl;
      Identifier wrongLayerId = lvl1_id->layer_id (goodTowerId, -99); 
      std::cout <<  lvl1_id->show_to_string(wrongLayerId) << std::endl;
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

    check_lvl1_id_decoding(idd);
    check_lvl1_helper(idd);
     
    return 0;  
}  
  
 
 
 
 
 
