/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/Calorimeter/CaloIdentifier/test/test_cell_id.cxx,v 1.19 2006-03-20 20:32:50 ssnyder Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/IdentifierHash.h" 
#include "CaloIdentifier/CaloCell_ID.h"
#include <iostream> 
 

static void
check_cell_id_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_cell_id_decoding" << std::endl;
    std::cout << "=========>  check_cell_id_decoding" << std::endl;

    bool error = false;

    LArEM_ID em_id;
    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init em from dict" << std::endl;
	error = true;
    }
    const LArEM_ID* em_p = &em_id;

    LArEM_SuperCell_ID sem_id;
    if(sem_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init sem from dict" << std::endl;
	error = true;
    }
    const LArEM_SuperCell_ID* sem_p = &sem_id;

    LArHEC_ID hec_id;
    if(hec_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init hec from dict" << std::endl;
	error = true;
    }
    const LArHEC_ID* hec_p = &hec_id;

    LArFCAL_ID fcal_id;
    if(fcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init fcal from dict" << std::endl;
	error = true;
    }
    const LArFCAL_ID* fcal_p = &fcal_id;

    LArMiniFCAL_ID minifcal_id;
    if(minifcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init minifcal from dict" << std::endl;
	error = true;
    }
    const LArMiniFCAL_ID* minifcal_p = &minifcal_id;

    TileID tile_id;
    if(tile_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init tile from dict" << std::endl;
	error = true;
    }
    const TileID* tile_p = &tile_id;

    CaloCell_ID* calo_id = new CaloCell_ID(em_p, hec_p, fcal_p, minifcal_p, tile_p);
    if(calo_id->initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init calocell from dict" << std::endl;
	error = true;
    }

    bool verif = calo_id->do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    calo_id->set_do_checks(true);
    verif = calo_id->do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of hash calculation

    // channels
    std::map < Identifier, int > counter_scell;
    std::vector<Identifier>::const_iterator itId = calo_id->cell_begin();
    std::vector<Identifier>::const_iterator itIdEnd = calo_id->cell_end();
  
    int nChan=0;
    int nEm=0;
    int nEmBarrel=0;
    int nEmEndCap=0;
    int nEmECInner=0;
    int nEmECOuter=0;
    int nHec=0;
    int nFcal=0;
    int nTile=0;

    int last_sub_calo = -999;
    

    
    IdentifierHash min=0;
    IdentifierHash max=0;
    std::cout << "Will Start the Loop" << std::endl;
  
    for(; itId!=itIdEnd;++itId){
	Identifier chId = *itId;
	//	std::cout << " ================= NCHAN = " << nChan << std::endl;
	nChan++;

	if(nChan == 1) std::cout << "test show_to_string with CaloCellID; chId = " << calo_id->show_to_string(chId) << std::endl;

	if(calo_id->is_em(chId)) {
	  nEm++;
	  calo_id->calo_cell_hash_range(chId, min, max);
	  //	  std::cout << "em: min, max " << min << " " << max << std::endl;
	  if(calo_id->is_em_barrel(chId)) {
	    nEmBarrel++;
            if ( (em_p->sampling(chId) == 1) || (em_p->sampling(chId)==2) ){
                std::cout << "Found : " << em_p->sampling(chId) << " ";
                std::cout << chId << " ";
                chId.show();
		std::cout << " ";
		std::cout << em_p->show_to_string(chId);
                std::cout << std::endl;
		int eta_div = 999;
		int phi_div = 999;
		if ( em_p->sampling(chId) == 1) {
			eta_div = 8;
			phi_div = 1;
		} else {
			eta_div = 1;
			phi_div = 4;
		}
		if ( eta_div != 999 ){
                  //int region = em_p->region(chId);
                  //int sampling = em_p->sampling(chId);
			int eta = em_p->eta(chId);
			int phi = em_p->phi(chId);
			eta = floorf( ((float)eta)/eta_div );
			phi = floorf( ((float)phi)/phi_div );

			Identifier sCellID = sem_p->channel_id( em_p->region_id(chId), eta,phi);
			std::cout << "\t\tFound SC : " << sem_p->sampling(sCellID) << " ";
	                std::cout << sCellID << " ";
        	        sCellID.show();
               		std::cout << " ";
            		std::cout << sem_p->show_to_string(sCellID);
	                std::cout << std::endl;
			if ( counter_scell.find(sCellID) == counter_scell.end() ){
				counter_scell[sCellID] = 1;
			} else
				counter_scell[sCellID] ++;
		}
		else std::cout << "\tWeird!!" << std::endl;
            }

	  }
	  else if(calo_id->is_em_endcap(chId)) {
	    nEmEndCap++;
	    if(calo_id->is_em_endcap_inner(chId)) {
	      nEmECInner++;
	    }
	    else if(calo_id->is_em_endcap_outer(chId)) {
	      nEmECOuter++;
	    }
	  }
	}
	else if(calo_id->is_hec(chId)) {
	  nHec++;
	  calo_id->calo_cell_hash_range(chId, min, max);
	  //	  std::cout << "hec: min, max " << min << " " << max << std::endl;
	}
	else if(calo_id->is_fcal(chId)) {
	  nFcal++;
	  calo_id->calo_cell_hash_range(chId, min, max);
	  //	  std::cout << "fcal: min, max " << min << " " << max << std::endl;
	}
	else if(calo_id->is_tile(chId)) {
	  nTile++;
	  calo_id->calo_cell_hash_range(chId, min, max);
	  //	  std::cout << "tile: min, max " << min << " " << max << std::endl;
	}
	
	IdentifierHash hashId = calo_id->calo_cell_hash ( chId );
	//	std::cout << "hashId = " << hashId << std::endl;


        if (calo_id->sub_calo(chId) != last_sub_calo) {
            std::cout << "New calo: chId = " << calo_id->show_to_string(chId) << " hash " << hashId << std::endl;
            last_sub_calo = calo_id->sub_calo(chId);
        }

        
	int subcalo1 = calo_id->sub_calo(hashId);
	IdentifierHash min2=0;
	IdentifierHash max2=0;
	calo_id->calo_cell_hash_range(subcalo1, min2, max2);
	if ( min != min2 || max != max2 ) {
	  std::cout << "problem with min/max, subcalo, min, min2, max, max2 = " 
		    << subcalo1 << " " << min << " " << min2 << " " << max << " " << max2 << std::endl;
	  error = true;
	}

	int subcalo2=-1;
	if(calo_id->is_em(chId)) {
	  IdentifierHash emHash = calo_id->subcalo_cell_hash(hashId, subcalo2) ;
	  if ( subcalo2 != subcalo1 ) {
	    std::cout << "(em)subcalo2 incorrect: " << subcalo2
		      << "         should be " <<  subcalo1
		      << " hashId = " << hashId
		      << std::endl;
	    error = true;
	  }
	  IdentifierHash caloCellHash = calo_id->calo_cell_hash(CaloCell_ID::LAREM, emHash) ;
	  if(hashId != caloCellHash) {
	    std::cout << " em return trip check failed = " << caloCellHash << " " << emHash << std::endl;
	    error = true;
	  }
	}
	else if(calo_id->is_hec(chId)) {
	  IdentifierHash hecHash = calo_id->subcalo_cell_hash(hashId, subcalo2) ;
	  if ( subcalo2 != subcalo1 ) {
	    std::cout << "(hec)subcalo2 incorrect: " << subcalo2
		      << "         should be " <<  subcalo1
		      << " hashId = " << hashId
		      << std::endl;
	    error = true;
	  }
	  IdentifierHash caloCellHash = calo_id->calo_cell_hash(CaloCell_ID::LARHEC, hecHash) ;
	  if(hashId != caloCellHash) {
	    std::cout << " hec return trip check failed = " << caloCellHash << " " << hecHash << std::endl;
	    error = true;
	  }
	}
	else if(calo_id->is_fcal(chId)) {
	  IdentifierHash fcalHash = calo_id->subcalo_cell_hash(hashId, subcalo2) ;
	  if ( subcalo2 != subcalo1 ) {
	    std::cout << "(fcal)subcalo2 incorrect: " << subcalo2
		      << "         should be " <<  subcalo1
		      << " hashId = " << hashId
		      << std::endl;
	    error = true;
	  }
	  IdentifierHash caloCellHash = calo_id->calo_cell_hash(CaloCell_ID::LARFCAL, fcalHash) ;
	  if(hashId != caloCellHash) {
	    std::cout << " fcal return trip check failed = " << caloCellHash << " " << fcalHash << std::endl;
	    error = true;
	  }
	}
	else if(calo_id->is_tile(chId)) {
	  IdentifierHash tileHash = calo_id->subcalo_cell_hash(hashId, subcalo2) ;
	  if ( subcalo2 != subcalo1 ) {
	    std::cout << "(tile)subcalo2 incorrect: " << subcalo2
		      << "         should be " <<  subcalo1
		      << " hashId = " << hashId
		      << std::endl;
	    error = true;
	  }
	  IdentifierHash caloCellHash = calo_id->calo_cell_hash(CaloCell_ID::TILE, tileHash) ;
	  if(hashId != caloCellHash) {
	    std::cout << " tile return trip check failed = " << caloCellHash << " " << tileHash << std::endl;
	    error = true;
	  }
	}

	Identifier chanId1 = calo_id->cell_id ( hashId );
	
	if ( chanId1 != chId ) {
	  std::cout
	    << " chanId1 incorrect: " <<  calo_id->show_to_string(chanId1) 
	    << "         should be " <<  calo_id->show_to_string(chId) 
	    << "           hashId = " << hashId
	    << std::endl;
	  error = true;
	}
	
	int subcalo3 = calo_id->sub_calo(chId);
	if ( subcalo3 != subcalo1 ) {
	  std::cout << " subcalo incorrect, subcalo3, subcalo1 = " << subcalo3 << " " << subcalo1
		    << " chId = " <<  calo_id->show_to_string(chId) 
		    << std::endl;
	  error = true;
	}

	int bec    = 888888;
	int samp   = 888888;
	int reg    = 888888;
	int eta    = 888888;
	int phi    = 888888;
	if(calo_id->is_lar(chId)) {  
	  bec    = calo_id->pos_neg(chId);
	  samp   = calo_id->sampling(chId);
	  reg    = calo_id->region(chId);
	  eta    = calo_id->eta(chId);
	  phi    = calo_id->phi(chId);
	}
	else  if(calo_id->is_tile(chId)) {
	  bec    = calo_id->section(chId);
	  samp   = calo_id->side(chId);
	  reg    = calo_id->module(chId);
	  eta    = calo_id->tower(chId);
	  phi    = calo_id->sample(chId);
	}
	
	Identifier chanId3 = calo_id->cell_id (subcalo3, bec, samp, reg, eta, phi);
	if ( chanId3 != chId ) {
	  std::cout << "chanId3 incorrect: " <<  calo_id->show_to_string(chanId3) 
		    << "         should be " <<  calo_id->show_to_string(chId) 
		    << " input val of subcalo3, bec, samp, reg= " << subcalo3 << " " << bec << " " << samp << " " << reg
		    << " input val of eta, phi= " << eta << " " << phi
		    << std::endl;
	  error = true;
	}

	Identifier regId = calo_id->region_id (chId);
	if(!calo_id->is_tile(chId)) {           
	  Identifier chanId2 = calo_id->cell_id (regId, eta, phi);
	  if ( chanId2 != chId ) {
	    std::cout << "chanId2 incorrect: " <<  calo_id->show_to_string(chanId2) 
		      << "         should be " <<  calo_id->show_to_string(chId) 
		      << "         regId     " <<  calo_id->show_to_string(regId) 
		      << " input val of eta, phi= " << eta << " " << phi
		      << std::endl;
	    error = true;
	  }
	}

	int subcalo4=-1;
	IdentifierHash subHashId = calo_id->subcalo_cell_hash (chId, subcalo4) ;
	if ( subcalo4 != subcalo1 ) {
	  std::cout << " subcalo incorrect, subcalo4, subcalo1 = " << subcalo4 << " " << subcalo1
		    << " chId = " <<  calo_id->show_to_string(chId) 
		    << std::endl;
	  error = true;
	}
	Identifier chanId4 = calo_id->cell_id ( subcalo3, subHashId );
	if ( chanId4 != chId ) {
	  std::cout
	    << " chanId4 incorrect: " <<  calo_id->show_to_string(chanId4) 
	    << "         should be " <<  calo_id->show_to_string(chId) 
	    << "      subcalo3, subHashId = " << subcalo3 << " " << subHashId
	    << std::endl;
	  error = true;
      }


    }
    std::map<Identifier,int>::const_iterator it,itE;
    it = counter_scell.begin();
    itE = counter_scell.end();
    for( ; it!=itE;++it){
	std::cout << "SC ID : " << sem_p->sampling((*it).first)
	<< " " << (*it).first << " ";
	std::cout << "; Number : " << (*it).second;
	std::cout << std::endl;
    }

    //sub-calos
    for (int subCalo = 0; subCalo < CaloCell_ID::NSUBCALO; subCalo++) { 
      itId = calo_id->cell_begin(subCalo);
      itIdEnd = calo_id->cell_end(subCalo);
      int nSubCalo = 0;
      for(; itId!=itIdEnd;++itId){
	nSubCalo++;
      }
      calo_id->calo_cell_hash_range(subCalo,min,max);
      std::cout << "subCalo, min, max " << subCalo << " " << min << " " << max << std::endl;
      if ( nSubCalo != (int)(max-min) ) {
	std::cout << "nSubCalo incorrect: " <<  nSubCalo
		  << "         should be max - min " <<  max << " - " << min
		  << std::endl;
	error = true;
      }

    }

    std::cout << "total number of cells= " << nChan << ", calo_cell_hash_max= " << calo_id->calo_cell_hash_max() << std::endl;
    std::cout << "number of Em, Hec, Fcal, Tile cells= " << nEm << ", " << nHec << ", " << nFcal << ", " << nTile << std::endl;
    std::cout << "number of Em Barrel, EndCap cells= " << nEmBarrel << ", " << nEmEndCap << std::endl;
    std::cout << "number of Em EndCap Inner, Outer cells= " << nEmECInner << ", " << nEmECOuter << std::endl;
    std::cout << " ====================================== " << std::endl;

    
    // AL--> TESTS 
    
    //int LARTEST= 8321;
    //const int subCalo = CaloCell_ID::GetSubCaloName( LARTEST );
    //std::cout << "====> TEST of GetSubCaloName ..." << CaloCell_ID::NSUBCALO << " Returned int= " << subCalo << std::endl;



    // regions
    itId = calo_id->reg_begin();
    itIdEnd = calo_id->reg_end();
  
    int nReg=0;
  
    for(; itId!=itIdEnd;++itId){

      nReg++;
      Identifier regId = *itId;
      

      IdentifierHash hashId = calo_id->calo_region_hash ( regId );

      std::cout << " region " << calo_id->sub_calo(regId) << " " << hashId << " etaMin, etaMax, phiMin, phiMax= " 
		<< calo_id ->eta_min(regId) << ", " 
		<< calo_id ->eta_max(regId) << ", " 
		<< calo_id ->phi_min(regId) << ", " 
		<< calo_id ->phi_max(regId) 
		<< std::endl;

      //      std::cout << " region " << hashId << " eta, phi granularities= " << calo_id->etaGranularity(regId)
      //		<< ", " << calo_id->phiGranularity(regId) << std::endl;
      
      Identifier regId1 = calo_id->region_id ( hashId );
      if ( regId1 != regId ) {
	std::cout
	  << " regId1 incorrect: " <<  calo_id->show_to_string(regId1) 
	  << "         should be " <<  calo_id->show_to_string(regId) 
	  << "           hasId = " << hashId
	  << std::endl;
	error = true;
      }
      
      
      int subcalo = calo_id->sub_calo(regId);

      calo_id->calo_reg_hash_range(regId, min, max);
      IdentifierHash min2=0;
      IdentifierHash max2=0;
      calo_id->calo_reg_hash_range(subcalo, min2, max2);
      if ( min != min2 || max != max2 ) {
	std::cout << "problem with region min/max, subcalo, min, min2, max, max2 = " 
		  << subcalo << " " << min << " " << min2 << " " << max << " " << max2 << std::endl;
	error = true;
      }

      int subcalo2=-1;
      IdentifierHash subHashId =  calo_id->subcalo_region_hash (hashId, subcalo2) ;
      if ( subcalo2 != subcalo ) {
	std::cout << "(region loop) subcalo2 incorrect: " << subcalo2
		  << "         should be " <<  subcalo
		  << " hashId = " << hashId
		  << std::endl;
	error = true;
      }
      IdentifierHash hashId2 =  calo_id->calo_region_hash (subcalo, subHashId) ;
      if ( hashId2 != hashId ) {
	std::cout << "hashId2 incorrect: " << hashId2
		  << "         should be " <<  hashId
		  << std::endl;
	error = true;
      }

      int bec  = calo_id->pos_neg(regId);
      int samp = calo_id->sampling(regId);
      int reg  = calo_id->region(regId);
      if(!calo_id->is_tile(regId)) {
	Identifier regId2 = calo_id->region_id (subcalo, bec, samp, reg);
	if ( regId2 != regId ) {
	  std::cout << "regId2 incorrect: " <<  calo_id->show_to_string(regId2) 
		    << "         should be " <<  calo_id->show_to_string(regId) 
		    << " input val of subcalo, bec, samp, reg= " << subcalo << " " << bec << " " << samp << " " << reg
		    << std::endl;
	  error = true;
	}
      }

      int subcalo3 = -1;
      IdentifierHash subHashId2 = calo_id->subcalo_region_hash (regId, subcalo3) ;
      if ( subcalo3 != subcalo ) {
	std::cout << "(region loop) subcalo3 incorrect: " << subcalo3
		  << "         should be " <<  subcalo
		  << " hashId = " << hashId
		  << std::endl;
	error = true;
      }
      Identifier regId3 = calo_id->region_id ( subcalo, subHashId2 );
      if ( regId3 != regId ) {
	std::cout
	<< " regId3 incorrect: " <<  calo_id->show_to_string(regId3) 
	<< "         should be " <<  calo_id->show_to_string(regId) 
	<< "           subHashId2 = " << subHashId2
	<< std::endl;
	error = true;
      }

    }
    std::cout << "number of regions= " << nReg  << ", calo_region_hash_max= " << calo_id->calo_region_hash_max() << std::endl;

    if(!error) {
	std::cout << "calo cell decoding tests ok" << std::endl;
    }

    std::cout << "finally check the exception throwing... " << std::endl;
    try {
      /*Identifier wrongChanId =*/ calo_id->cell_id (0, 99, 0, 0, 0, 0);
    }
    catch ( LArID_Exception &excpt ) {
	std::cout << "*** LArID_Exception caught: " << excpt.message() 
		  << std::endl
		  << "***   error code: " << excpt.code()   
		  << std::endl;
    }       
    std::cout << "... which is ok " << std::endl;



}

static void
check_cell_neighbours(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_cell_neighbours" << std::endl;
    std::cout << "=========>  check_cell_neighbours" << std::endl;

    bool error = false;

    LArEM_ID em_id;
    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_neighbours - cannot init em from dict" << std::endl;
    }
    const LArEM_ID* em_p = &em_id;

    LArHEC_ID hec_id;
    if(hec_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_neighbours - cannot init hec from dict" << std::endl;
    }
    const LArHEC_ID* hec_p = &hec_id;

    LArFCAL_ID fcal_id;
    if(fcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_neighbours - cannot init fcal from dict" << std::endl;
    }
    const LArFCAL_ID* fcal_p = &fcal_id;

    LArMiniFCAL_ID minifcal_id;
    if(minifcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init minifcal from dict" << std::endl;
	error = true;
    }
    const LArMiniFCAL_ID* minifcal_p = &minifcal_id;

    TileID tile_id;
    if(tile_id.initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init tile from dict" << std::endl;
	error = true;
    }
    const TileID* tile_p = &tile_id;

    CaloCell_ID* calo_id = new CaloCell_ID(em_p, hec_p, fcal_p, minifcal_p, tile_p);
    if(calo_id->initialize_from_dictionary (idd)) {
	std::cout << "check_cell_id_decoding - cannot init calocell from dict" << std::endl;
	error = true;
    }


    // loop on cell hashes

    std::vector<IdentifierHash> neighbourList;
    IdentifierHash hash_min = 888888 ;
    IdentifierHash hash_max = 0 ;
    for (unsigned int iCell = 0 ; iCell < calo_id->calo_cell_hash_max(); ++iCell){
      Identifier cellId = calo_id->cell_id(iCell);
      calo_id->calo_cell_hash_range(cellId, hash_min, hash_max);

      int res_neighb = calo_id->get_neighbours(iCell, LArNeighbours::all3D, neighbourList);
      if(res_neighb) {
	std::cout << "return code of get_neighbours not ok for iCell = " << iCell << "cellId = " << calo_id->show_to_string(cellId) << std::endl;
      }
      else {
        for (IdentifierHash neighbourHash : neighbourList) {
	  if(neighbourHash < hash_min || neighbourHash > hash_max) {
	    std::cout << " neighbour index " << neighbourHash << " outside of det boundaries for iCell " << iCell << std::endl;
	    error = true;
	  }
	}
      }
    }

    if(!error) {
	std::cout << "calo cell neighbours micro tests ok" << std::endl;
    }
}


int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  
    IdDictParser parser;  
    std::string lArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml";
    // std::string lArIDFileName = "IdDictLArCalorimeter_sLHC-LArMiniFcal-00.xml";
    // std::string lArIDFileName = "IdDictLArCalorimeter_HGTD_01.xml";
    //    std::string lArIDFileName = "IdDictLArCalorimeter.xml";
    //    std::string lArIDFileName = "IdDictLArCalorimeter_H8_2004.xml";
    //    std::string lArIDFileName = "IdDictLArCalorimeter_H6_2004.xml";
    parser.register_external_entity("LArCalorimeter", lArIDFileName); 
    IdDictMgr& idd = parser.parse (argv[1]);  
    std::cout << "got dict mgr for file " << lArIDFileName << std::endl;

    // Set some default file names for neighbours (RDS 12/2009):
    idd.add_metadata("FULLATLASNEIGHBORS",  "SuperCaloNeighbours-DC3-05-Comm-01.dat");  
    idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
    idd.add_metadata("TILENEIGHBORS",       "TileNeighbour_reduced.txt");  


    std::cout << "HELLO" << std::endl;
    check_cell_neighbours(idd);
    check_cell_id_decoding(idd);
     
    return 0;  
}  
  
 
 
 
 
 
