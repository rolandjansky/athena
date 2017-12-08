/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "TestLArHardwareID/testTTMap_Algo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloID.h"
#include "LArIdentifier/LArIdManager.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCell_P.h"
#include "CaloTriggerTool/LArTTCellMap.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"

/********************************************************/
testTTMap_Algo::testTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  AthAlgorithm( name , pSvcLocator) ,
  m_lvl1Helper(0),
  m_emHelper(0),
  m_hecHelper(0),
  m_fcalHelper(0),
  m_ttSvc(0),
  m_dumpMap(false)
{
  declareProperty("dumpMap", m_dumpMap );
}

testTTMap_Algo::~testTTMap_Algo()
{
}

// ==============================================================
StatusCode testTTMap_Algo::initialize(){
// ==============================================================
  ATH_MSG_INFO ( " initializing " );


  const CaloIdManager*	caloIdMgr = nullptr;
  ATH_CHECK( detStore()->retrieve(caloIdMgr) );
  ATH_MSG_INFO ( "Successfully retrieved CaloIdManager from DetectorStore" );
  
  const CaloLVL1_ID* caloId = caloIdMgr->getLVL1_ID();
  if (!caloId) {
    ATH_MSG_ERROR ( "Could not access calolvl1 ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed calolvl1 ID helper" );
    m_lvl1Helper = caloId ;
  }
  
  const LArEM_ID* emId = caloIdMgr->getEM_ID();
  if (!emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar EM ID helper" );
    m_emHelper=emId;
  }
    
  const LArHEC_ID* hecId = caloIdMgr->getHEC_ID();
  if (!hecId) {
    ATH_MSG_ERROR ( "Could not access lar HEC ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar HEC ID helper" );
    m_hecHelper=hecId;
  }
  
  const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
  if (!fcalId) {
    ATH_MSG_ERROR ( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar FCAL ID helper" );
    m_fcalHelper=fcalId;
  }

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service( "ToolSvc",toolSvc  ) );
  ATH_CHECK( toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc) );
  
  return StatusCode::SUCCESS;
}


// ====================================================================================
StatusCode testTTMap_Algo::execute(){
// ====================================================================================

  ATH_MSG_INFO ( "=> testTTMap_Algo::Executing " );

  const LArTTCellMap* ttCellMap_c = nullptr;
  ATH_CHECK( detStore()->retrieve( ttCellMap_c ) );
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  
  //
  // first test the structure
  //
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::const_iterator it   = ttCell_P->m_v.begin(); 
  VTTCELL::const_iterator it_e = ttCell_P->m_v.end(); 
  ATH_MSG_DEBUG("  Dump of LArTTCellMap" );
  ATH_MSG_DEBUG(" Persistent LArTTCell_P version = "<<ttCell_P->m_version);
  unsigned int nLine=0;
  for (; it!=it_e;++it)       {
    const LArTTCell_P::LArTTCell_P_t& t = *it;    
    ATH_MSG_VERBOSE
      (" det="<<t.det
       <<" pn="<<t.pn 
       <<" sample="<<t.sample
       <<" region="<<t.region
       <<" eta="<<t.eta
       <<" phi="<<t.phi
       <<" trig_pn="<<t.tpn
       <<" trig_samp="<<t.tsample
       <<" trig_reg="<<t.tregion
       <<" trig_eta="<<t.teta
       <<" trig_phi="<<t.tphi
       <<" layer="<<t.layer
       );
    nLine++;
  } 
  ATH_MSG_DEBUG(" nb of lines in struct= " << nLine );
  
  //
  // then test the map itself
  //
  ATH_MSG_DEBUG ( " --1-- Loop on layers to test the map itself (directly, not via CaloTriggerTowerService)"  );
  // loop on generalised trigger towers (1 id per layer)
  unsigned int nTT=0;
  unsigned int nTTincTile=0;
  unsigned int nCell=0;
  unsigned int nWrong=0;
  unsigned int nEmpty=0;
  unsigned int notInLvl1=0;
  // useful in debug phase; can be removed afterwards
  std::set<Identifier> cellIdSet;

  std::vector<Identifier>::const_iterator itTt  = m_lvl1Helper->layer_begin() ;
  std::vector<Identifier>::const_iterator itEnd = m_lvl1Helper->layer_end() ;
  for(; itTt!=itEnd;++itTt){
    Identifier layerId = (*itTt) ;
    nTTincTile++;
    // ........ skip Tile cal
    if(!m_lvl1Helper->is_tile(layerId)){ 
      nTT++;
      std::vector<Identifier> cellVec = ttCellMap->createCellIDvec(layerId);
      if(cellVec.size()==0) {
	ATH_MSG_VERBOSE ( " --1-- empty TT " << m_lvl1Helper->show_to_string(layerId) );
	nEmpty++;
      } else {
	for (unsigned int iCell=0;iCell<cellVec.size();iCell++){	
	  nCell++;
	  if(!(cellIdSet.insert(cellVec[iCell])).second) {
	    ATH_MSG_ERROR(" --1-- Duplicate cell id " 
                          << m_lvl1Helper->show_to_string(cellVec[iCell]) 
                          << " in TT= "
                          << m_lvl1Helper->show_to_string(layerId) );
	  }

	  // test return trip
	  Identifier layerId2 = ttCellMap->whichTTID(cellVec[iCell]);
	  if(layerId2 != layerId) {
	    nWrong++;
	    ATH_MSG_DEBUG ( " test1: return trip wrong for TT1 " 
                            << m_lvl1Helper->show_to_string(layerId) 
                            << " TT2 "
                            << m_lvl1Helper->show_to_string(layerId2) 
                            << " and cell "
                            << m_lvl1Helper->show_to_string(cellVec[iCell]) 
                            );
	  }
	  // test is_in_lvl1 in passing
	  bool inLvl1=m_ttSvc->is_in_lvl1(cellVec[iCell]);
	  if(!inLvl1) {
	    notInLvl1++;
	    ATH_MSG_VERBOSE ( " cell not in LVL1; TTid= " << m_lvl1Helper->show_to_string(layerId) );
	  }
	}
      }
    }
  }
  ATH_MSG_DEBUG ( " --1-- nTT, nTT incl Tile, nEmpty(LAr only) =" 
                  << nTT 
                  << " " << nTTincTile  
                  << " " << nEmpty  );
  ATH_MSG_DEBUG ( " --1-- nCell, nWrong, nCell not in LVL1=" 
                  << " " << nCell 
                  << " " << nWrong 
                  << " " << notInLvl1 );


  //
  // then test the map via TriggerTowerService
  //
  std::ofstream* dumpTTMap=0;
  std::string file="testDump.txt";
  if(m_dumpMap) {
    dumpTTMap=new std::ofstream(file.c_str());   
    if (dumpTTMap==0) {
      std::cout << "Problem opening TTmapTest dump file" << std::endl;
      return 1;
    }
  }

  ATH_MSG_DEBUG ( " --2-- Loop on layers to test CaloTriggerTowerService" );
  // loop on generalised trigger towers (1 id per layer)
  nTT=0;
  nTTincTile=0;
  nCell=0;
  nWrong=0;
  nEmpty=0;
  cellIdSet.clear();
  itTt  = m_lvl1Helper->layer_begin() ;
  itEnd = m_lvl1Helper->layer_end() ;
  for(; itTt!=itEnd;++itTt){
    Identifier layerId = (*itTt) ;
    nTTincTile++;
    // ........ skip Tile cal
    if(!m_lvl1Helper->is_tile(layerId)){ 
      nTT++;
      std::vector<Identifier> cellVec = m_ttSvc->createCellIDvecLayer(layerId);
      if(cellVec.size()==0) {
	ATH_MSG_VERBOSE ( " --2-- empty TT " << m_lvl1Helper->show_to_string(layerId) );
	nEmpty++;
      } else {
	for (unsigned int iCell=0;iCell<cellVec.size();iCell++){	
	  nCell++;
	  // dump map for checks
	  if(m_dumpMap){
	    *dumpTTMap
	      <<" cellID="<<m_lvl1Helper->show_to_string(cellVec[iCell])
	      <<" layerID="<<m_lvl1Helper->show_to_string(layerId)
	      <<std::endl;
	  }

	  if(!(cellIdSet.insert(cellVec[iCell])).second) {
	    ATH_MSG_ERROR(" --2-- Duplicate cell id " 
                          << m_lvl1Helper->show_to_string(cellVec[iCell]) 
                          << " in TT= "
                          << m_lvl1Helper->show_to_string(layerId) );
	  }

	  // test return trip
	  Identifier layerId2 = m_ttSvc->whichTTID(cellVec[iCell]);
	  if(layerId2 != layerId) {
	    nWrong++;
	    ATH_MSG_DEBUG ( " test2: return trip wrong for TT1 " 
                            << m_lvl1Helper->show_to_string(layerId) 
                            << " TT2 "
                            << m_lvl1Helper->show_to_string(layerId2) 
                            << " and cell "
                            << m_lvl1Helper->show_to_string(cellVec[iCell]) );
	  }
	}
      }
    }
  }
  if(m_dumpMap) {
    dumpTTMap->close();
    delete dumpTTMap;
  }
  ATH_MSG_DEBUG ( " --2-- nTT (incl Tile), nEmpty= " 
                  << nTT 
                  << " (" << nTTincTile << ") " 
                  << " " << nEmpty );
  ATH_MSG_DEBUG ( " --2-- nCell, nWrong= " 
                  << " " << nCell 
                  << " " << nWrong );

  ATH_MSG_DEBUG ( " --3-- Loop on towers to test CaloTriggerTowerService" );
  // loop on trigger towers (1 per 'sampling' = EM or had)
  nTT=0;
  nTTincTile=0;
  nCell=0;
  nWrong=0;
  nEmpty=0;
  int nTTlay=0;
  itTt  = m_lvl1Helper->tower_begin() ;
  itEnd = m_lvl1Helper->tower_end() ;
  for(; itTt!=itEnd;++itTt){
    Identifier towerId = (*itTt) ;
    nTTincTile++;
    // ........ skip Tile cal
    if(!m_lvl1Helper->is_tile(towerId)){ 
      nTT++;

      Identifier regId=m_lvl1Helper->region_id(towerId);
      int maxLay=m_lvl1Helper->layer_max(regId);
      //      std::cout << "ttid, maxLay= " << m_lvl1Helper->show_to_string(towerId) << " " << maxLay << std::endl;

      for(int iLay=0;iLay<=maxLay;++iLay) {
	nTTlay++;
      }

      std::vector<Identifier> cellVec = m_ttSvc->createCellIDvecTT(towerId);
      if(cellVec.size()==0) {
	ATH_MSG_VERBOSE ( " --3-- empty TT " << m_lvl1Helper->show_to_string(towerId) );
	nEmpty++;
      } else {
	for (unsigned int iCell=0;iCell<cellVec.size();iCell++){	
	  nCell++;
	  //	  std::cout << "cell id2= " << m_lvl1Helper->show_to_string(cellVec[iCell])<<std::endl;
	  Identifier layerId = m_ttSvc->whichTTID(cellVec[iCell]);
	  Identifier towerId2=m_lvl1Helper->tower_id(layerId);
	  if(towerId2 != towerId) {
	    nWrong++;
	    ATH_MSG_DEBUG ( " test3: return trip wrong for TT1 " 
                            << m_lvl1Helper->show_to_string(towerId) 
                            << " TT2 "
                            << m_lvl1Helper->show_to_string(towerId2) 
                            << " and cell "
                            << m_lvl1Helper->show_to_string(cellVec[iCell]) );
	  }
	}
      }
    }
  }
  ATH_MSG_DEBUG ( " --3-- nTT (incl Tile), nEmpty, nTT if looping on layers= " 
                  << nTT 
                  << " (" << nTTincTile << ") " 
                  << " " << nEmpty 
                  << " " << nTTlay );
  ATH_MSG_DEBUG ( " --3-- nCell, nWrong= "
                  << " " << nCell 
                  << " " << nWrong );

  return StatusCode::SUCCESS ;
}

/********************************************************/
StatusCode testTTMap_Algo::finalize(){
	
  ATH_MSG_INFO ( " finalizing " );
  return StatusCode::SUCCESS ; 
}
