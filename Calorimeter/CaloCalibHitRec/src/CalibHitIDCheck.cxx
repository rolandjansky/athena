/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************************************
//  Filename : CalibHitIDCheck.cxx
//
//  Author   : Gia   gia@mail.cern.ch
//  Created  : April, 2005
//
//  DESCRIPTION:
//
//    Helper tool for CalibHits Identifiers checking
//
//*************************************************************************************

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"

// Calo include
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloCalibHitRec/CalibHitIDCheck.h"

#include "AtlasDetDescr/AtlasDetectorID.h" 

#include <iostream>


// Constructor & destructor
CalibHitIDCheck::CalibHitIDCheck(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator), 
   m_Merge(false),
   m_LArDMHitCnt(nullptr),
   m_ActiveHitCnt(nullptr),
   m_InactiveHitCnt(nullptr),
   m_TileActiveHitCnt(nullptr),
   m_TileInactiveHitCnt(nullptr),
   m_TileDMHitCnt(nullptr),
   m_Check(true), 
   m_ShowAll(false), 
   m_CheckAll(false),
   m_id_helper(nullptr)
{
  m_larInactiveHitContainer  = "LArCalibrationHitInactive";
  m_larActiveHitContainer    = "LArCalibrationHitActive";
  m_larDMHitContainer        = "LArCalibrationHitDeadMaterial";
  m_tileActiveHitContainer   = "TileCalibHitActiveCell";
  m_tileInactiveHitContainer = "TileCalibHitInactiveCell";
  m_tiledmHitContainer       = "TileCalibHitDeadMaterial";
  
  declareProperty("InactiveCalibHitCnt", m_larInactiveHitContainer);    
  declareProperty("ActiveCalibHitCnt",   m_larActiveHitContainer); 
  declareProperty("LArDMCalibHitCnt",    m_larDMHitContainer); 
  declareProperty("TileActiveHitCnt",    m_tileActiveHitContainer);
  declareProperty("TileInactiveHitCnt",  m_tileInactiveHitContainer);
  declareProperty("TileDMCalibHitCnt",   m_tiledmHitContainer);

  declareProperty("Check",    m_Check    );
  declareProperty("ShowAll",  m_ShowAll  );  
  declareProperty("CheckAll", m_CheckAll );

}

CalibHitIDCheck::~CalibHitIDCheck()
{}



/////////////////   INITIALIZE   /////////////////////
StatusCode CalibHitIDCheck::initialize() 
{ 
  ATH_CHECK( detStore()->retrieve(m_id_helper) );
  ATH_MSG_INFO("initialisation completed" );
  return StatusCode::SUCCESS;
}




////////////////   EXECUTE   //////////////////////
StatusCode CalibHitIDCheck::execute()
{
  ATH_CHECK( evtStore()->retrieve(m_LArDMHitCnt,        m_larDMHitContainer) );
  ATH_CHECK( evtStore()->retrieve(m_ActiveHitCnt,       m_larActiveHitContainer) );
  ATH_CHECK( evtStore()->retrieve(m_InactiveHitCnt,     m_larInactiveHitContainer) );
  ATH_CHECK( evtStore()->retrieve(m_TileActiveHitCnt,   m_tileActiveHitContainer) );
  ATH_CHECK( evtStore()->retrieve(m_TileInactiveHitCnt, m_tileInactiveHitContainer) );
  ATH_CHECK( evtStore()->retrieve(m_TileDMHitCnt,       m_tiledmHitContainer) );

  //We need merge only Inactive and Active
  //LAr CalibHit Containers only once
  m_Merge=false;


  //check containers separately
  if(m_Check) {
    if(m_InactiveHitCnt)    { check(0); }
    if(m_ActiveHitCnt)      { check(1); }
    if(m_TileInactiveHitCnt){ check(2); }
    if(m_TileActiveHitCnt)  { check(3); }
    if(m_LArDMHitCnt)       { check(4); }
    if(m_TileDMHitCnt)      { check(5); }
  }


  //show all IDs from all containers 
  if(m_ShowAll)   show_all();


  //check all containers 
  if(m_CheckAll)  check_all_cnts();


  //reset member 'm_id_vec'
  reset();


  ATH_MSG_DEBUG("execute() completed successfully" );
  return StatusCode::SUCCESS;
}




///////////////   FINALIZE   ////////////////////
StatusCode CalibHitIDCheck::finalize()
{
  ATH_MSG_INFO("finalize() successfully" );
  return StatusCode::SUCCESS;
}





//**********************************************************
// ------------------------ METHODS ------------------------
//**********************************************************

//This method checks separately all the presented CalibHit
//containers for not valid, container not respected 
//('alien') and repeated Identifiers
void CalibHitIDCheck::check(int l_cnt) {

  std::vector<Identifier>   id_vec;
  std::map<Identifier, int> id_map;
  std::map<Identifier, int>::iterator map_it;
  CaloCalibrationHitContainer::const_iterator it,end;

  bool no_invalid = true;
  bool no_alien   = true;

  //define appropriate iterator over the container
  switch(l_cnt) {
  case 0:

    if((m_InactiveHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("*************************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from LAr Inactive CalibHit Container" );
        ATH_MSG_INFO("*************************************************************" );

        it = m_InactiveHitCnt->begin();
        end = m_InactiveHitCnt->end();
      }
    else
      {
        return;
      }
    break;


  case 1:

    if((m_ActiveHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("***********************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from LAr Active CalibHit Container" );
        ATH_MSG_INFO("***********************************************************" );

        it = m_ActiveHitCnt->begin();
        end = m_ActiveHitCnt->end();

        m_Merge=true;
      }
    else
      {
        return;
      }
    break;


  case 2:
    
    if((m_TileInactiveHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("**************************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from Tile Inactive CalibHit container" );
        ATH_MSG_INFO("**************************************************************" );
        
        it  = m_TileInactiveHitCnt->begin();
        end = m_TileInactiveHitCnt->end();
      }
    else
      {
        return;
      }
    break;


  case 3:
    
    if((m_TileActiveHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("************************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from Tile Active CalibHit container" );
        ATH_MSG_INFO("************************************************************" );
        
        it  = m_TileActiveHitCnt->begin();
        end = m_TileActiveHitCnt->end();

        m_Merge=true;
      }
    else
      {
        return;
      }
    break;


  case 4:

    if((m_LArDMHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("*******************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from LAr DM CalibHit Container" );
        ATH_MSG_INFO("*******************************************************" );

        it = m_LArDMHitCnt->begin();
        end = m_LArDMHitCnt->end();
      }
    else
      {
        return;
      }
    break;


  case 5:

    if((m_TileDMHitCnt->Size()) != 0 )
      {
        ATH_MSG_INFO(" " );
        ATH_MSG_INFO("********************************************************" );
        ATH_MSG_INFO("Investigate CalibHit IDs from Tile DM CalibHit Container" );
        ATH_MSG_INFO("********************************************************" );
        
        it  = m_TileDMHitCnt->begin();
        end = m_TileDMHitCnt->end();
      }
    else
      {
        return;
      }
    break;

  default:
    return;
  }

 
  //Now, iterate over the container to find
  //not valid, 'alien' and repeated IDs
  for(; it != end; it++) {
    
    Identifier id=(*it)->cellID();

    
    //Check ID whether is it valid or not
    if(!id.is_valid()) {

      ATH_MSG_ERROR("Invalid Identifier of CalibHit is found! "
                    <<m_id_helper->show_to_string(id) );

      no_invalid=false;
    }



    //Check for 'alien' IDs in the container
    switch(l_cnt) {
    case 0:
    case 1:
      if( !(m_id_helper->is_lar(id)) ) {
        ATH_MSG_INFO("Found CalibHit with NO LAr ID "
                     <<m_id_helper->show_to_string(id) );
        no_alien=false;
      } 
      break;

    case 2:
    case 3:
      if( !(m_id_helper->is_tile(id)) ) {
        ATH_MSG_INFO("Found CalibHit with NO Tile ID "
                     <<m_id_helper->show_to_string(id) );
        no_alien=false;
      }
      break;

    case 4:
      if( !(m_id_helper->is_lar_dm(id))  & 
          //It's normal to find Tile DM ID 
          //recorded by Default Calculator
          !(m_id_helper->is_tile_dm(id))  ) {
        ATH_MSG_INFO("Found CalibHit with NO LAr DM ID "
                     <<m_id_helper->show_to_string(id) );
        no_alien=false;
      }     
      break;
       
    case 5:
      if( !(m_id_helper->is_tile_dm(id)) ) { 
        ATH_MSG_INFO("Found CalibHit with NO Tile DM ID "
                     <<m_id_helper->show_to_string(id) );
        no_alien=false;
      }
      break;

    }


    bool no_rep  = true;

    //Fill id_map with IDs, which are repeted more then once in the container
    for (int iter=0; iter<static_cast<int>(id_vec.size()); iter++) {
      if(id == id_vec[iter]) {

	if(id_map.find(id)==id_map.end()) {
	  id_map.insert(std::make_pair(id,2));
	}
	else {
	  map_it=id_map.find(id);
	  (map_it->second)++ ;
	}
	
	no_rep  = false;	
        break;
      }
    }

    if(no_rep) {
      id_vec.push_back(id);

      //store id in the vector for all containers and 
      //don't forget merge Inactive & Active CalibHit IDs
      if(m_Merge) merge(id);
      else        m_id_vec.push_back(id);
    }
    else {
      //repeated ID should not be stored because 
      //its first pattern was already stored &
      //should not be merged still the same reason 
      //as above. 
    }

  }//End of looping over the CalibHits inside the container      
  

  //reset merge flag anyway. 
  //It is needed 'true' only twice
  m_Merge=false;


  //tell if no invalid IDs were eppeared
  if(no_invalid) {

    ATH_MSG_INFO("------------------------------------- " );
    ATH_MSG_INFO("No invalid Identifiers has been found " );
    ATH_MSG_INFO("------------------------------------- " );

  }


  //tell if no 'alien' IDs were eppeared
  if(no_alien) {

    ATH_MSG_INFO("------------------------------------------------------" );
    ATH_MSG_INFO("No container not-respective Identifiers has been found" );
    ATH_MSG_INFO("------------------------------------------------------" );

  }


  //Print the result of looking for repeated IDs
  if(!id_map.empty()) {

    ATH_MSG_INFO("----------------------------------------------- " );
    ATH_MSG_INFO("Found repeated Identifiers   |   how many times " );
    ATH_MSG_INFO("----------------------------------------------- " );
    
    for(map_it=id_map.begin(); map_it!=id_map.end(); map_it++) {
         
      ATH_MSG_INFO(m_id_helper->show_to_string(map_it->first)
                   <<"                  "<<map_it->second );
    }
   
  }
  else {
    ATH_MSG_INFO("----------------------------------------------- " );
    ATH_MSG_INFO("No repeated CalibHit Identifiers has been found " );
    ATH_MSG_INFO("----------------------------------------------- " );
  }

}



//This method merges CalibHits from Inactive & Active 
//containers in case they have the same Identifiers, 
//what is the normal situation if that happens
//only once for the given Identifier
void CalibHitIDCheck::merge(Identifier id) {

  bool new_id = false;

  if(!m_id_vec.empty()) {

     for (int iter=0; iter != static_cast<int>(m_id_vec.size()); iter++) {
      if(id == m_id_vec[iter]) {
	new_id = false;
	break;
      }
      else {
	new_id = true;
      }
     }
	
     if(new_id) m_id_vec.push_back(id);   
  }
  else {
    //nothing to merge. 
    //push all IDs in the vector
    m_id_vec.push_back(id) ;
  }

}




//This method shows all IDs into the string format
void CalibHitIDCheck::show_all() {

  ATH_MSG_INFO("=================================================" );
  ATH_MSG_INFO("Show all CalibHits Identifiers for current events" );
  ATH_MSG_INFO("=================================================" );

  for (int iter=0; iter != static_cast<int>(m_id_vec.size()); iter++) {

    ATH_MSG_INFO(m_id_helper->show_to_string(m_id_vec[iter]) );

  }  

}




//This method is useful to find those IDs, which are 
//probably not repeated inside the same CalibHits 
//container but in the different ones.
//
//If there aren't repeated IDs in the different
//CalibHit containers then this method must tell
//that it did not find any of them.
//
//This is claimed because 'm_id_vec' keeps only 
//one pattern of those IDs, which were repeated
//inside the one particular container.
//
//Otherwise, if this method found a repeated ID
//it means that this ID belongs to the CalibHits
//contained inside the different containers! 
void CalibHitIDCheck::check_all_cnts() {

  ATH_MSG_INFO("======================================================" );
  ATH_MSG_INFO("Check for the CalibHits Identifiers has been repeated " );
  ATH_MSG_INFO("      inside the different CalibHit containers        " );
  ATH_MSG_INFO("======================================================" );


  bool no_rep=true;

  for (size_t i=0; i < m_id_vec.size(); i++) {

    ID::iterator it_end = std::remove (m_id_vec.begin()+i+1,
                                       m_id_vec.end(),
                                       m_id_vec[i]);
    int same = m_id_vec.end() - it_end;
    m_id_vec.erase (it_end, m_id_vec.end());

    if(same!=0) {
      ATH_MSG_INFO("Identifier  "
                   <<m_id_helper->show_to_string(m_id_vec[i])
                   <<"     is repeated  "<<(same+1)<<"  times"
                   <<" inside different containers " );

      no_rep=false;
    }
  } //end looping


  if(no_rep) {
    ATH_MSG_INFO("*** No repeated CalibHit Identifier " 
                 <<"in the different containers was found ***"  );
  }

}


