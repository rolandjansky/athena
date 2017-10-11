/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     TrigDataAccess.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: This will provide infrastructure for the Tools
//		to perform their algorithms
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "LArRecEvent/LArCell.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
// #include "Identifier/IdentifierHash.h"
// #include "LArRawUtils/LArTT_Selector.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "LArRawUtils/LArRoI_Map.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/ROBData.h"

#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"

// Initialize method for all tools
// Retrieval of all Tools to be used during run
StatusCode TrigDataAccess::initialize()
{
	// Some tools are necessary for the Algorithm to run
	// The RegionSelector is being retrieved here
        if( (m_pRegionSelector.retrieve()).isFailure() ) {
          ATH_MSG_FATAL("Unable to retrieve RegionSelector Service");
	  return StatusCode::FAILURE;
	}

	// Get ROBDataProvider to help in data prefetching (only
	// realy used in AthenaMT)
	if( (m_robDataProvider.retrieve()).isFailure()){
	        ATH_MSG_ERROR(" Can't get ROBDataProviderSvc ");
		return StatusCode::FAILURE;
	} // End of if ROBDataProvider

	// Reserve vectors space to avoid online memory allocation
	// Limits to be studied
	m_rIds.reserve(30);
	m_vrodid32.reserve(30);
	m_vrodid32_found.reserve(30);
	m_vrodid32_missed.reserve(30);
        m_partial_vrodid32.reserve(35);
        m_full_vrodid32.reserve(250);
	m_robFrags.reserve(30);
	m_tile.push_back(0);
        m_drawcoll = NULL;
// Event handling
	IIncidentSvc* p_incSvc;
	if ( service("IncidentSvc",p_incSvc, true).isFailure() ) {
	  ATH_MSG_ERROR("Unable to get the IncidentSvc");
	}else{
	  p_incSvc->addListener(this, "BeginEvent",100);
	}
	// Method not called yet
	m_iov_called=false;
	// Register function for the detector store
        const DataHandle<AthenaAttributeList> febrodmap;
	if ( detStore()->regFcn(&ITrigDataAccess::beginRunHandle,(ITrigDataAccess*)this,
				febrodmap, "/LAR/Identifier/FebRodMap", true).isFailure() ) { //FIXME, hardcoded database folder name
	        ATH_MSG_ERROR(" Can't regFnc with Condition ");
		return StatusCode::FAILURE;
        }
	// Just to the Missing Et slice
	IRegSelSvc* regSelSvc = &(*m_pRegionSelector);
	if ( detStore()->regFcn(&IRegSelSvc::handle,
		regSelSvc,
		&ITrigDataAccess::beginRunHandle_RegSelSvc,
		(ITrigDataAccess*)this,true).isFailure() ) {
	        ATH_MSG_ERROR("Can't regFnc with Condition");
		return StatusCode::FAILURE;
	}
        if ( m_usefullcoll ) {
          m_rIdsem0.reserve(12288);
          m_rIdsem1.reserve(300);
          m_rIdsem2.reserve(300);
          m_rIdsem3.reserve(300);
          m_rIdshec.reserve(4352);
          m_rIdshec0.reserve(300);
          m_rIdshec1.reserve(300);
          m_rIdshec2.reserve(300);
          m_rIdshec3.reserve(300);
          m_rIdsfcalhad0.reserve(256);
          m_rIdsfcalhad1.reserve(128);
          m_rIdsfcalhad2.reserve(128);
          m_rIdsfcalhad3.reserve(128);
          m_rIdsfcalem0.reserve(300);
          m_rIdstile.reserve(300);
          m_vrodid32lar.reserve(900);
          m_vrodid32em.reserve(724);
          m_vrodid32hec.reserve(72);
          m_vrodid32hec0.reserve(24);
          m_vrodid32hec1.reserve(24);
          m_vrodid32hec2.reserve(24);
          m_vrodid32fcalem.reserve(8);
          m_vrodid32fcalhad.reserve(6);
          m_vrodid32tile.reserve(300);
	  m_vrodid32fullDet.reserve(900);
	  m_fullCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
	  m_fullCellContainer->reserve(190000);
	} // end of m_usefullcoll
	//m_vrodid32ros.push_back(0x007dFFFF);
	//m_vrodid32tros.push_back(0x007eFFFF);

        m_febcoll = new LArFebEnergyCollection();
	m_mbts_done = false; // Just to reset
	m_zdc_rods.push_back(0x00830000);
	m_zdc_rods.push_back(0x00830001);
	m_zdc_rods.push_back(0x00830002);
	m_zdc_rods.push_back(0x00830003);

	// luminosiry tool
	if ( m_applyOffsetCorrection ) {
	  ATH_MSG_INFO("Apply BCID/<mu> dependent offset correction");
	  if ( m_caloLumiBCIDTool.retrieve().isFailure() ) {
	    ATH_MSG_FATAL("Could not find m_caloLumiBCID");
	    return StatusCode::FAILURE;
	  }
	  //
	  //if ( m_lumiTool.retrieve().isFailure() ) {
	  //  (*m_log) << MSG::FATAL << "Could not find m_lumiTool" <<endreq;
	  //  return StatusCode::FAILURE;
	  //} else {
	  //  std::cout << "Retrieve lumiTool successfully" << std::endl;
	  //}
	} else {
	  ATH_MSG_INFO("No BCID/<mu> dependent offset correction");
	}

	return StatusCode::SUCCESS;
} // End of initialize

StatusCode TrigDataAccess::beginRunHandle(IOVSVC_CALLBACK_ARGS){
	// The ByteStreamCnv (Through LArTT_Selector needs a map of the
	// RoIs, being retrieved here
	if(toolSvc()->retrieveTool("LArRoI_Map",m_roiMap).isFailure()) {
	        ATH_MSG_FATAL("Could not find LArRoI_Map");
		return StatusCode::FAILURE;
	} // End of if LArRoI_Map

	if((m_lardecoder.retrieve()).isFailure()) {
	        ATH_MSG_FATAL("Could not find LArRodDecoder");
	}
	if((m_tiledecoder.retrieve()).isFailure()){
	        ATH_MSG_FATAL("Could not find TileRodDecoder");
	}
        //m_datablock.reserve(350);
	if((m_zdcdecoder.retrieve()).isFailure()){
	        ATH_MSG_FATAL("Could not find ZdcByteStreamTool");
	}
	if((m_zdcrectool.retrieve()).isFailure()){
	        ATH_MSG_FATAL("Could not find ZdcRecChannelTool");
	}

	// Delete if things already exist
        if ( m_iov_called ){
        if ( (m_larcell->finalize()).isFailure() ){
	      ATH_MSG_FATAL("Could not finish larcell");
        }
        delete m_larcell;
        if ( (m_tilecell->finalize()).isFailure() ){
	      ATH_MSG_FATAL("Could not finish tilecell");
        }
        delete m_tilecell;
        if ( m_sel ) delete m_sel;
        if(m_usefullcoll){
       //delete m_fullCellContainer;
         delete m_selem;
         delete m_selhec;
         delete m_selfcalhad;
         delete m_selfcalem;
        }
	if ( m_drawcoll != NULL) {
		delete m_drawcoll;
		m_drawcoll = NULL;
	}
        }
	
	m_larcell = new LArCellCont();
	if ( (m_larcell->initialize(m_applyOffsetCorrection)).isFailure() ){
	      ATH_MSG_FATAL("Could not init larcell");
        }
	// set frequency
        m_sel= new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
        if(m_usefullcoll){
          m_selem = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selhec = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selfcalhad = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selfcalem = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
        }
	m_tilecell = new TileCellCont();
	m_tilecell->setHashIdToROD( m_tiledecoder->getHid2reHLT() );
        if ( (m_tilecell->initialize()).isFailure() ){
	      ATH_MSG_FATAL("Could not init tilecell");
        }
        for(unsigned int i=0;i<4;i++){
	m_tiledecoder->loadRw2Cell (i,m_tilecell->Rw2CellMap (i));
	m_tiledecoder->loadRw2Pmt  (i,m_tilecell->Rw2PmtMap  (i));
	}
	m_tiledecoder->loadMBTS_Ptr(m_tilecell->MBTS_collection(),
		m_tilecell->MBTS_map(), m_tilecell->MBTS_channel() );
	m_tiledecoder->load_list_of_masked_drawers( m_tilecell->masked_drawers() );
	// ROD Address for MBTS
        m_mbts_rods = m_tilecell->MBTS_RODs();
	// unique set
	m_mbts_add_rods.insert(m_mbts_add_rods.end(),(*m_mbts_rods).begin(),(*m_mbts_rods).end());
	sort(m_mbts_add_rods.begin(),m_mbts_add_rods.end());
        m_mbts_add_rods.erase(std::unique(m_mbts_add_rods.begin(),m_mbts_add_rods.end()),m_mbts_add_rods.end());
	
        const CaloDetDescrManager* theCaloDDM = CaloDetDescrManager::instance();
        const CaloCell_ID* theCaloCCIDM = theCaloDDM->getCaloCell_ID();
        unsigned int hashMax = theCaloCCIDM->calo_cell_hash_max();
        std::vector<CaloCell*> local_cell_copy;
        local_cell_copy.reserve(200000);
        if(m_usefullcoll){
        for(unsigned int lcidx=0; lcidx < m_larcell->size(); lcidx++){
                LArCellCollection* lcc = m_larcell->at(lcidx);
                unsigned int lccsize = lcc->size();
                for(unsigned int lccidx=0; lccidx<lccsize; lccidx++){
                        CaloCell* cell = ((*lcc).at(lccidx));
                        if ( cell && cell->caloDDE() ) local_cell_copy.push_back( cell );
                } // end of loop over cells
          } // end of loop over collection
          for(unsigned int lcidx=0; lcidx < m_tilecell->size(); lcidx++){
                TileCellCollection* lcc = m_tilecell->at(lcidx);
                unsigned int lccsize = lcc->size();
                for(unsigned int lccidx=0; lccidx<lccsize; lccidx++){
                        CaloCell* cell = ((*lcc).at(lccidx));
                        if ( cell ) local_cell_copy.push_back( cell );
                } // end of loop over cells
          } // end of loop over collection

          for(unsigned int i=0;i<hashMax;i++)
                m_fullCellContainer->push_back_fast(NULL);

          unsigned int localcellMax = local_cell_copy.size();
          for(unsigned int i=0;i<localcellMax;i++){
                unsigned int j = local_cell_copy.at(i)->caloDDE()->calo_hash();
                if ( j < hashMax ) {
                        m_fullCellContainer->at(j) = local_cell_copy.at(i);
                }
          }
          for(unsigned int i=0;i<hashMax;i++)
                if ( m_fullCellContainer->at(i) == NULL ){
                        Identifier id = theCaloCCIDM->cell_id(i);
                        if ( id!=0 ){
                          const CaloDetDescrElement* el = theCaloDDM->get_element(id);
                          m_fullCellContainer->at(i) = (new CaloCell(el,0,0,0,(CaloGain::CaloGain)0) );
                        }
                }

          m_fullCellContainer->setHasCalo(CaloCell_ID::LAREM);
          m_fullCellContainer->setHasCalo(CaloCell_ID::LARHEC);
          m_fullCellContainer->setHasCalo(CaloCell_ID::LARFCAL);
          m_fullCellContainer->setHasCalo(CaloCell_ID::TILE);

          m_fullCellContainer->order();

          if ( hashMax != m_fullCellContainer->size() )
	        ATH_MSG_ERROR("Problem in the size of the full container");
          m_fullCellContainer->setIsOrdered(true);
          m_fullCellContainer->setIsOrderedAndComplete(true);

          local_cell_copy.clear();

	} // end of if full cell

        //m_febcoll = new LArFebEnergyCollection();
	if(m_usefullcoll){
	  ATH_MSG_DEBUG("Preparing the full collections");
          if ( m_rIdsem0.size() != 0 ) m_rIdsem0.clear();
          if ( m_rIdsem1.size() != 0 ) m_rIdsem1.clear();
          if ( m_rIdsem2.size() != 0 ) m_rIdsem2.clear();
          if ( m_rIdsem3.size() != 0 ) m_rIdsem3.clear();
          if ( m_rIdshec.size() != 0 ) m_rIdshec.clear();
          if ( m_rIdshec0.size() != 0 ) m_rIdshec0.clear();
          if ( m_rIdshec1.size() != 0 ) m_rIdshec1.clear();
          if ( m_rIdsfcalhad0.size() != 0 ) m_rIdsfcalhad0.clear();
          if ( m_rIdsfcalhad1.size() != 0 ) m_rIdsfcalhad1.clear();
          if ( m_rIdsfcalhad2.size() != 0 ) m_rIdsfcalhad2.clear();
          if ( m_rIdsfcalhad3.size() != 0 ) m_rIdsfcalhad3.clear();
          if ( m_rIdsfcalem0.size() != 0 ) m_rIdsfcalem0.clear();
          if ( m_rIdstile.size() != 0 ) m_rIdstile.clear();
	} // End of m_usefullcoll
        m_drawcoll = new TileL2Container();
	m_drawcoll->reserve(256);
        for(int i=0;i<256;i++) {
                int collId = m_tiledecoder->hashFunc()->identifier(i);
                TileL2* sumE = new TileL2(collId);
                m_drawcoll->push_back(sumE);
        }

	m_iov_called=true;
	return StatusCode::SUCCESS;
}


StatusCode TrigDataAccess::beginRunHandle_RegSelSvc(IOVSVC_CALLBACK_ARGS){
	  //IRegSelSvc* iRegionSelector(m_pRegionSelector);
	  //RegSelSvc* regionSelector = dynamic_cast<RegSelSvc*>( &(*m_pRegionSelector) );

	  m_pRegionSelector->DisableMissingROBs( m_larcell->MissingROBs() );
	  // regionSelector->DisableMissingROBs( m_larcell->MissingROBs() );
	  if(m_usefullcoll){
             ATH_MSG_DEBUG("Finalizing Preparation of full collections");

	  TrigRoiDescriptor tmproi(true);
	  //TrigRoiDescriptor tmproi( true ); /// use new fullscan RoI

	  //          const double mineta = -4.8;
	  //          const double maxeta = 4.8;
	  //          const double minphi = -M_PI;
	  //          const double maxphi = M_PI;
          // TTEM 
          m_pRegionSelector->DetROBIDListUint(TTEM,-1,tmproi,m_vrodid32em);
          // TTHEC
          m_pRegionSelector->DetROBIDListUint(TTHEC,0,tmproi,m_vrodid32hec0);
          m_pRegionSelector->DetROBIDListUint(TTHEC,1,tmproi,m_vrodid32hec1);
          m_pRegionSelector->DetROBIDListUint(TTHEC,2,tmproi,m_vrodid32hec2);
          m_pRegionSelector->DetROBIDListUint(TTHEC,3,tmproi,m_vrodid32hec3);
          // FCALHAD
          m_pRegionSelector->DetROBIDListUint(FCALHAD,-1,tmproi,m_vrodid32fcalhad);
          m_pRegionSelector->DetROBIDListUint(FCALEM,-1,tmproi,m_vrodid32fcalem);
          // TILE
	  // NB!!! If a different pi convention is used in the Tile, the computation should best be 
	  //       done inside the Tile Hash map - in fact, why not correct this when building the 
	  //       table in the first place.  
	  m_pRegionSelector->DetROBIDListUint(TILE,tmproi,m_vrodid32tile);
          // m_pRegionSelector->DetROBIDListUint(TILE,tmptroi,m_vrodid32tile);
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32em.begin(),m_vrodid32em.end());
          m_vrodid32hec.insert(m_vrodid32hec.end(),m_vrodid32hec0.begin(),m_vrodid32hec0.end());
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32hec.begin(),m_vrodid32hec.end());
	  m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalhad.begin(),m_vrodid32fcalhad.end());
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalem.begin(),m_vrodid32fcalem.end());
	  m_vrodid32fullDet.insert(m_vrodid32fullDet.end(), m_vrodid32lar.begin(), m_vrodid32lar.end() );
          m_pRegionSelector->DetHashIDList(TTEM,0,tmproi,m_rIdsem0);
          m_pRegionSelector->DetHashIDList(TTEM,1,tmproi,m_rIdsem1);
          m_pRegionSelector->DetHashIDList(TTEM,2,tmproi,m_rIdsem2);
          m_pRegionSelector->DetHashIDList(TTEM,3,tmproi,m_rIdsem3);
          m_pRegionSelector->DetHashIDList(TTHEC,0,tmproi,m_rIdshec0);
          m_pRegionSelector->DetHashIDList(TTHEC,1,tmproi,m_rIdshec1);
          m_pRegionSelector->DetHashIDList(TTHEC,2,tmproi,m_rIdshec2);
          m_pRegionSelector->DetHashIDList(TTHEC,3,tmproi,m_rIdshec3);
          m_pRegionSelector->DetHashIDList(FCALHAD,0,tmproi,m_rIdsfcalhad0);
          m_pRegionSelector->DetHashIDList(FCALHAD,1,tmproi,m_rIdsfcalhad1);
          m_pRegionSelector->DetHashIDList(FCALHAD,2,tmproi,m_rIdsfcalhad2);
          m_pRegionSelector->DetHashIDList(FCALHAD,3,tmproi,m_rIdsfcalhad3);
	  m_pRegionSelector->DetHashIDList(FCALEM,0,tmproi,m_rIdsfcalem0);
          m_pRegionSelector->DetHashIDList(TILE,tmproi,m_rIdstile);
          m_alltile.clear();
          std::vector<uint32_t> tilelists; 
          for(unsigned int i=0;i<m_rIdstile.size();i++){ 
           bool include=true; 
           uint32_t tilerodid = m_tilecell->find_rod(m_rIdstile[i]); 
           m_alltile.push_back(tilerodid); 
           for(unsigned int j=0;j<tilelists.size();j++) 
                if ( tilelists.at(j) == tilerodid ) include=false; 
           if ( include ) tilelists.push_back ( tilerodid ); 
          } 
          m_vrodid32fullDet.insert(m_vrodid32fullDet.end(), tilelists.begin(), tilelists.end() ); 
          tilelists.clear(); 
          // TTEM 
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem1.begin(),m_rIdsem1.end());
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem2.begin(),m_rIdsem2.end());
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem3.begin(),m_rIdsem3.end());
          // TTHEC
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec0.begin(),m_rIdshec0.end());
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec1.begin(),m_rIdshec1.end());
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec2.begin(),m_rIdshec2.end());
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec3.begin(),m_rIdshec3.end());
          // FCALHAD
          m_rIdsfcalhad0.insert(m_rIdsfcalhad0.end(),m_rIdsfcalhad1.begin(),m_rIdsfcalhad1.end());
          m_rIdsfcalhad0.insert(m_rIdsfcalhad0.end(),m_rIdsfcalhad2.begin(),m_rIdsfcalhad2.end());
          m_rIdsfcalhad0.insert(m_rIdsfcalhad0.end(),m_rIdsfcalhad3.begin(),m_rIdsfcalhad3.end());

          m_selem->setRoIs(m_rIdsem0);
          m_selhec->setRoIs(m_rIdshec);
          m_selfcalhad->setRoIs(m_rIdsfcalhad0);
          m_selfcalem->setRoIs(m_rIdsfcalem0);
	} // End of m_usefullcoll
	return StatusCode::SUCCESS;
}

// Finalize method for all tools
StatusCode TrigDataAccess::finalize(){
        ATH_MSG_DEBUG("in finalize() by TrigDataAccess");
	if ( m_iov_called ){
        if ( (m_larcell->finalize()).isFailure() ){
	        ATH_MSG_FATAL("Could not finish larcell");
        }
        delete m_larcell;
        if ( (m_tilecell->finalize()).isFailure() ){
	        ATH_MSG_FATAL("Could not finish tilecell");
        }
        delete m_tilecell;
        delete m_sel;
	if(m_usefullcoll){
         delete m_selem;
         delete m_selhec;
         delete m_selfcalhad;
         delete m_selfcalem;
	}
	}
        delete m_febcoll;
	if ( m_drawcoll!=NULL){
	        delete m_drawcoll;
		m_drawcoll = NULL;
	}
	m_zdcDigitCollection.clear();
	m_zdcCol.clear();
	return StatusCode::SUCCESS;

} // End of finalize

// This should be run to provide ByteStreamCnv with rodid's to
// look up for. addROBData only works in athenaMT
void TrigDataAccess::RegionSelectorRobID (const int sampling,
					  const IRoiDescriptor& roi,
					  const DETID detid, bool fetchROBs) {
        // Clears up m_vrodid32
        if ( m_vrodid32.size() != 0 ) m_vrodid32.clear();
        if ( detid != TILE ){
	  m_pRegionSelector->DetROBIDListUint(detid,sampling, roi, m_vrodid32);
        }
        else { // TILE does not need sample
	  m_pRegionSelector->DetROBIDListUint(detid, roi, m_vrodid32);
        }

	if ( m_loadAllSamplings && m_present_roi!=&roi ) { 

	  m_full_vrodid32.clear();

	  m_present_roi = &roi;
	  
	  for(int sa=0;sa<4;sa++){
	    m_partial_vrodid32.clear();
	    m_pRegionSelector->DetROBIDListUint(TTEM, sa, roi, m_partial_vrodid32); 
	    m_full_vrodid32.insert(m_full_vrodid32.end(),m_partial_vrodid32.begin(),m_partial_vrodid32.end());
	  }
	  
	  for(int sa=0;sa<4;sa++){
	    m_partial_vrodid32.clear();
	    m_pRegionSelector->DetROBIDListUint(TTHEC,sa, roi, m_partial_vrodid32); 
	    m_full_vrodid32.insert(m_full_vrodid32.end(),m_partial_vrodid32.begin(),m_partial_vrodid32.end());
	  }
	  
	  m_partial_vrodid32.clear();
	  m_pRegionSelector->DetROBIDListUint(FCALEM, 0, roi, m_partial_vrodid32); 
	  
	  m_full_vrodid32.insert(m_full_vrodid32.end(),m_partial_vrodid32.begin(),m_partial_vrodid32.end());
	  
	  for(int sa=0;sa<2;sa++){
	    m_partial_vrodid32.clear();
	    m_pRegionSelector->DetROBIDListUint(FCALHAD, sa, roi, m_partial_vrodid32);
	    
	    m_full_vrodid32.insert(m_full_vrodid32.end(),m_partial_vrodid32.begin(),m_partial_vrodid32.end());
	  }
	  
	  m_partial_vrodid32.clear();
	  m_pRegionSelector->DetROBIDListUint(TILE, roi, m_partial_vrodid32); 
	  
	  
	  m_full_vrodid32.insert(m_full_vrodid32.end(),m_partial_vrodid32.begin(),m_partial_vrodid32.end());
	  sort(m_full_vrodid32.begin(),m_full_vrodid32.end());

	  m_full_vrodid32.erase(std::unique(m_full_vrodid32.begin(),m_full_vrodid32.end()),m_full_vrodid32.end());
	  if ( fetchROBs ) {m_robDataProvider->addROBData(m_full_vrodid32); m_robDataProvider->getROBData(m_full_vrodid32,m_robFrags); m_robFrags.clear();} 
        } 
	else if ( fetchROBs ) {m_robDataProvider->addROBData(m_vrodid32); m_robDataProvider->getROBData(m_vrodid32,m_robFrags); m_robFrags.clear();} 

	if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "m_vrodid32.size() = " << m_vrodid32.size() );
          for(unsigned int i = 0 ; i < m_vrodid32.size() ; i++)
            ATH_MSG_VERBOSE( "m_vrodid32[" << i << "]=" << m_vrodid32[i]);
        }
}  // End of RegionSelectorRobID


// This code should load m_listID with Detectors ID
// Should this return a flag in case of failure?
void TrigDataAccess::RegionSelectorListID (const int sampling, const IRoiDescriptor& roi, const DETID detid) {
        // Clears up m_rIds
        if ( m_rIds.size() != 0 ) m_rIds.clear();

        if ( detid != TILE ){
	  m_pRegionSelector->DetHashIDList(detid,sampling,
					   roi,
					   m_rIds);
        }
        else{ // TILE does not need sample
	  m_pRegionSelector->DetHashIDList(detid,
					   roi, 
					   m_rIds);
        }

	if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "m_rIds.size() = " << m_rIds.size() );
          for(unsigned int i = 0; i < m_rIds.size() ; i++)
            ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << m_rIds[i] );
        }

} // End of RegionSelectorListID

// This method should load all the infrastructure for the offline
// Collection of cells. The iterators m_iBegin, m_iEnd and m_it
// will have the cells Collection to be used by the Algorithm
StatusCode TrigDataAccess::LoadCollections (
		LArTT_Selector<LArCellCont>::const_iterator& Begin,
		LArTT_Selector<LArCellCont>::const_iterator& End,
		const unsigned int /*sample*/, bool /*prepare*/) {
	if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "m_rIds.size() in LoadColl = " << m_rIds.size() );
          for(unsigned int i = 0 ; i < m_rIds.size() ; i++)
            ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << m_rIds[i] );
        }
        // Resets error flag
        m_error=0;
	Begin=End=m_sel->end();
	m_robFrags.clear();
	bool rob_miss=false;
        m_robDataProvider->getROBData(m_vrodid32,m_robFrags);
	if ( m_vrodid32.size() != m_robFrags.size() ){
		// Number of ROBs smaller than requested is below
		m_error|=0x10000000;
		rob_miss=true;
		m_vrodid32_found.clear();
		m_vrodid32_missed.clear();
	}

        //int rodidx = 0;
        for (size_t i = 0; i < m_robFrags.size(); ++i){
		// Source ID has correct number
		uint32_t source_id = m_robFrags[i]->source_id();
		// Find the collection to fill
		const std::vector<LArCellCollection*>::const_iterator it =
			(m_larcell->find(source_id));
		if ( rob_miss ) m_vrodid32_found.push_back(source_id);
		if ( it != m_larcell->end() && (*it)->size()!=0 ) { // Already decoded collection
		LArCellCollection *m_col = *it;
        	m_lardecoder->setsecfeb(m_larcell->findsec(source_id));
        	if ( !m_lardecoder->check_valid(m_robFrags[i],msg()) ){
                  ATH_MSG_WARNING( "Error reading bytestream"<<
                                   "event: Bad ROB block (eformat checks) : 0x"
                                   << std::hex << source_id << std::dec );
		  // Data seems corrupted
		  m_error|=0x20000000;
		  reset_LArCol ( m_col );
		} else {
		  // Get Rod Data and size of fragment
        	  const uint32_t* roddata1 = 0;
        	  m_robFrags[i]->rod_data(roddata1);
        	  size_t roddatasize = m_robFrags[i]->rod_ndata();
        	  if (roddatasize < 3) {
                    ATH_MSG_WARNING( "Error reading bytestream"<<
                                     "event: Empty ROD block (less than 3 words) : 0x"
                                     << std::hex << source_id << std::dec );
		    // Data seems corrupted
		    m_error|=0x20000000;
		    reset_LArCol ( m_col );
		    //return StatusCode::SUCCESS;
        	  } else { // End of if small size
        	        m_lardecoder->setRobFrag(m_robFrags[i]);
        	        m_lardecoder->fillCollectionHLT(roddata1,roddatasize,*m_col);
		        if (m_applyOffsetCorrection) m_larcell->applyBCIDCorrection(source_id);
		        // Accumulates inferior byte from ROD Decoder
		        m_error|=m_lardecoder->report_error();
		  } //roddatasize < 3
		} // check rob frag valid
		//rodidx++;
		} // Collection not cached yet
  	} // End of for through RobFrags

	if ( rob_miss ) { 
	        // have to reset missing collections
	        // find rod addresses missing
		std::vector<uint32_t>::const_iterator i = m_vrodid32.begin();
		std::vector<uint32_t>::const_iterator j = m_vrodid32_found.begin();
		for(; i!=m_vrodid32.end() && j!=m_vrodid32_found.end(); ++i,++j){
        		while ( ((*i)!=(*j)) && i!=m_vrodid32.end() ) {m_vrodid32_missed.push_back(*i); ++i;}
		}
		for(; i!=m_vrodid32.end(); ++i) m_vrodid32_missed.push_back(*i);
		for(std::vector<uint32_t>::const_iterator i=m_vrodid32_missed.begin(); i!=m_vrodid32_missed.end();i++){
		  const std::vector<LArCellCollection*>::const_iterator it =
			(m_larcell->find((*i)));
		  if ( it != m_larcell->end() && (*it)->size()!=0 ) { // Already decoded collection
		    LArCellCollection *m_col = *it;
		    reset_LArCol ( m_col );
		  } // end of if
		}
	} // End of missing ROBs treatment

        // These iterators can be used by every Tool
        m_sel->setRoIs(m_rIds);
        Begin = m_sel->begin();
        End   = m_sel->end();
	if (msgLvl(MSG::VERBOSE)) {
	  LArTT_Selector<LArCellCont>::const_iterator m_it;
	  for ( m_it=Begin; m_it != End; ++m_it ){
            ATH_MSG_VERBOSE( "Eta: " << (*m_it)->eta()
                           << "; Phi: " << (*m_it)->phi() 
                           << "; Energy: " << (*m_it)->energy() );
	  } // End of for printout cells
	}
	return StatusCode::SUCCESS;
} // End of method


StatusCode TrigDataAccess::LoadCollections (
                TileCellCollection::const_iterator& Begin,
                TileCellCollection::const_iterator& End,
                const unsigned int sample, bool /*prepare*/){

        // Resets error flag
        m_error=0;
	m_robFrags.clear();

	int i = sample;
	Begin=End;
	//for (size_t i = 0; i < m_listID.size(); ++i){
		m_tile[0] = m_tilecell->find_rod(m_rIds[i]);
                m_robDataProvider->getROBData(m_tile,m_robFrags);
		// Number of ROBs smaller than requested is below
		if ( m_robFrags.empty() ) m_error|=0x10000000;
		// Find the collection to fill
		const std::vector<TileCellCollection*>::const_iterator it =
			(m_tilecell->find(m_rIds[i]));
                TileCellCollection* m_col = *it;
		if ( m_robFrags.size()!=0 && m_col != NULL ) {
		size_t roddatasize = m_robFrags[0]->rod_ndata();
		// insert data into vector (to be removed soon)
		if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream"<<
                                   "event: Empty ROD block (less than 3 words) : 0x"
                                   << std::hex << m_tile[0] << std::dec );
		  msg(MSG::WARNING) << "Error reading bytestream "
				    << "event: Empty ROD block (less than 3 words) : 0x"
				    << std::hex << m_tile[0] << std::dec << endreq;
		  // Data seems corrupted
		  m_error|=0x20000000;
                  if ( !m_tilecell->cached(m_rIds[i])){
                        // resets collection
                        reset_TileCol(m_col);
                  }
                  m_robFrags.clear();
                  Begin = m_col->begin();
                  End = m_col->end();
		} else  {// End of if small size
		if ( !m_tilecell->cached(m_rIds[i]))
		m_tiledecoder->fillCollectionHLT(m_robFrags[0],*m_col);
		m_tiledecoder->mergeD0cellsHLT(*m_col);
		// Accumulates superior byte from ROD Decoder
		m_error|=m_tiledecoder->report_error();
                Begin = m_col->begin();
                End = m_col->end();
                m_robFrags.clear();
		}
		}
	//} // End of for through RobFrags

	if (msgLvl(MSG::VERBOSE)) {
	  TileCellCollection::const_iterator m_itt = Begin;
	  for (m_itt=Begin;m_itt!=End;++m_itt){
            ATH_MSG_VERBOSE( "Eta: " << (*m_itt)->eta()
                           << "; Phi: " << (*m_itt)->phi() 
                           << "; Energy: " << (*m_itt)->energy() 
                           << "; Hash Id: " << (*m_itt)->caloDDE()->calo_hash() );
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;

} // End of LoadCollections

StatusCode TrigDataAccess::LoadMBTS (
                TileCellCollection::const_iterator& Begin,
                TileCellCollection::const_iterator& End) {
        m_robFrags.clear();
        // ROD Address for 
        const std::vector<unsigned int>* ids = m_tilecell->MBTS_IDs();
        m_robDataProvider->addROBData((m_mbts_add_rods));
	if ( m_mbts_add_rods.size() != m_robFrags.size() ){
		// Number of ROBs smaller than requested is below
		m_error|=0x10000000;
	}
        TileCellCollection* mbts = m_tilecell->MBTS_collection();
        if ( !m_mbts_done ){
        /* 
        for (size_t i = 0; i < (*mbts).size(); ++i)
                (*mbts)[i]->setEnergy(0.0);
        */
        for (size_t i = 0; i < (*m_mbts_rods).size(); ++i){
                m_tile[0] = (*m_mbts_rods)[i];
                m_robDataProvider->getROBData(m_tile,m_robFrags);
                // Find the collection to fill
		const std::vector<TileCellCollection*>::const_iterator it =
			(m_tilecell->find((*ids)[i]));
                TileCellCollection* m_col = NULL;
                m_col = (*it);
                if ( m_robFrags.size()!=0 && m_col != NULL ) {
                size_t roddatasize = m_robFrags[0]->rod_ndata();
                if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream MBTS"<<
                                   "event: Empty ROD block (less than 3 words)" );
                  if ( !m_tilecell->cached((*ids)[i])){
                        // resets collection
                        reset_TileCol(m_col);
                  }
                  m_robFrags.clear();
                  continue;
		  //return StatusCode::SUCCESS;
                } // End of if small size
		if ( !m_tilecell->cached((*ids)[i]))
                m_tiledecoder->fillCollectionHLT(m_robFrags[0],*m_col);
                }
                m_robFrags.clear();

        } // End of for through RobFrags
	m_mbts_done=true;
	} // End of check m_mbts_done
        Begin = (*mbts).begin();
        End = (*mbts).end();
        return StatusCode::SUCCESS;

}

StatusCode TrigDataAccess::LoadZdcCollection(ZdcRawChannelCollection::const_iterator& Begin,
                ZdcRawChannelCollection::const_iterator& End){
        m_error=0;
	Begin=End;
        m_robFrags.clear();
        // ROD Address for
        m_robDataProvider->addROBData(m_zdc_rods);
        m_robDataProvider->getROBData(m_zdc_rods,m_robFrags);
        if ( m_zdc_rods.size() != m_robFrags.size() )
	      { m_error|=0x10000000; }
	if ( !m_robFrags.empty() ){
	m_zdcDigitCollection.clear();
	m_zdcCol.clear();
	ZdcRawChannelCollection* zdcColPointer = &m_zdcCol;
	if ( !(m_zdcdecoder->convert(m_robFrags,&m_zdcDigitCollection)).isFailure() ) {
	m_zdcrectool->makeRawFromDigits(m_zdcDigitCollection, zdcColPointer);
        Begin = m_zdcCol.begin();
        End = m_zdcCol.end();
        } // zdc if conversion
	} // got some data
        return StatusCode::SUCCESS;
}

StatusCode TrigDataAccess::LoadCollections (
                LArFebEnergyCollection::const_iterator& Begin,
                LArFebEnergyCollection::const_iterator& End,
                const unsigned int /*sample*/, bool /*prepare*/){

        m_error = 0;
	Begin=End;

	m_robFrags.clear();
        m_robDataProvider->getROBData(m_vrodid32,m_robFrags);

        if( m_vrodid32.size() != m_robFrags.size() ){
             m_error|=0x10000000;
        }
        m_febcoll->clear();
        //int rodidx = 0;
        for (size_t i = 0; i < m_robFrags.size(); ++i){
	      if ( m_lardecoder->check_valid(m_robFrags[i],msg())){
                  uint32_t source_id = m_robFrags[i]->source_id();
                  // Get Rod Data and size of fragment
                  const uint32_t* roddata1 = 0;
                  m_robFrags[i]->rod_data(roddata1);
                  size_t roddatasize = m_robFrags[i]->rod_ndata();
                  if (roddatasize < 3) {
                    ATH_MSG_WARNING( "Error reading bytestream FEB"<<
                                     "event: Empty ROD block (less than 3 words) : 0x"
                                     << std::hex << source_id << std::dec );
		    // Data seems corrupted
		    m_error|=0x20000000;
		    //return StatusCode::SUCCESS;
                  } // End of if small size
		  else {
                        m_lardecoder->setRobFrag(m_robFrags[i]);
                        m_lardecoder->fillCollectionHLTFeb(roddata1,roddatasize,*m_febcoll);
                        // if (m_col->size() == 0) return StatusCode::FAILURE;
                        m_error|=m_lardecoder->report_error();
		  }
		}
        } // End of for through RobFrags
        // These iterators can be used by every Tool
        Begin = m_febcoll->begin();
        End   = m_febcoll->end();

	if (msgLvl(MSG::VERBOSE)) {
	  for(LArFebEnergyCollection::const_iterator it = Begin; it!=End; ++it){
            ATH_MSG_VERBOSE( " Feb ID = " << (*it)->getFebId() 
                           << " Feb Ex = " << (*it)->getFebEx()
                           << " Feb Ey = " << (*it)->getFebEy() 
                           << " Feb Ez = " << (*it)->getFebEz() );
	  }
	}
        return StatusCode::SUCCESS;

} // End of LoadCollections

StatusCode TrigDataAccess::LoadFullCollections (
                LArTT_Selector<LArCellCont>::const_iterator& Begin,
                LArTT_Selector<LArCellCont>::const_iterator& End,
                const DETID detid, bool /*prepare*/) {
        if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "m_rIds.size() in LoadColl = " << m_rIds.size() );
	  for(unsigned int i = 0 ; i < m_rIds.size() ; i++)
            ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << m_rIds[i] );
	}
	// Resets error flag
	m_error=0;
        Begin=End=m_sel->end();
        if ( detid == 0 ) { 
            m_robDataProvider->addROBData( m_vrodid32lar ); 
            m_robDataProvider->getROBData( m_vrodid32lar, m_robFrags ); 
            m_robFrags.clear(); 
        } 
	m_robFrags.clear();
	bool rob_miss=false;
	size_t  checkSize = 0;
        // Keep a local reference
        std::vector<uint32_t>* vrodids = &m_vrodid32em;

        switch (detid) {
           case TTEM:
             m_robDataProvider->addROBData(m_vrodid32em);
             m_robDataProvider->getROBData(m_vrodid32em,m_robFrags);
	     checkSize = m_vrodid32em.size();
             Begin = m_selem->begin();
             End = m_selem->end();
             break;
           case TTHEC:
             m_robDataProvider->addROBData(m_vrodid32hec);
             m_robDataProvider->getROBData(m_vrodid32hec,m_robFrags);
	     checkSize = m_vrodid32hec.size();
             vrodids = &m_vrodid32hec;
             Begin = m_selhec->begin();
             End = m_selhec->end();
             break;
           case FCALEM:
             m_robDataProvider->addROBData(m_vrodid32fcalem);
             m_robDataProvider->getROBData(m_vrodid32fcalem,m_robFrags);
	     checkSize = m_vrodid32fcalem.size();
             vrodids = &m_vrodid32fcalem;
             Begin = m_selfcalem->begin();
             End = m_selfcalem->end();
             break;
           case FCALHAD:
             m_robDataProvider->addROBData(m_vrodid32fcalhad);
             m_robDataProvider->getROBData(m_vrodid32fcalhad,m_robFrags);
	     checkSize = m_vrodid32fcalhad.size();
             vrodids = &m_vrodid32fcalhad;
             Begin = m_selfcalhad->begin();
             End = m_selfcalhad->end();
             break;
           default:
             break;
        }
	if ( checkSize != m_robFrags.size() ){
                // Number of ROBs smaller than requested is below
                m_error|=0x10000000;
		rob_miss=true; 
		m_vrodid32_found.clear(); 
		m_vrodid32_missed.clear(); 
        }
        //
        int rodidx = 0;
        for (size_t i = 0; i < m_robFrags.size(); ++i){
	// Source ID has correct number
        uint32_t source_id = m_robFrags[i]->source_id();
	if ( rob_miss ) m_vrodid32_found.push_back(source_id);
        std::vector<LArCellCollection*>::const_iterator it;
        it = m_larcell->find(source_id);
        if (it != m_larcell->end() && (*it)->size()!=0 ) {
           LArCellCollection *m_col = *it;
           m_lardecoder->setsecfeb(m_larcell->findsec(source_id));
           if ( !m_lardecoder->check_valid(m_robFrags[i],msg()) ) {
             ATH_MSG_WARNING( "Error reading bytestream"
                              << "event: Bad ROB block (eformat checks) : 0x"
                              << std::hex << source_id << std::dec );
	     m_error|=0x20000000;
	     reset_LArCol ( m_col );
           } else {
                  const uint32_t* roddata1 = 0;
                  m_robFrags[i]->rod_data(roddata1);
                  size_t roddatasize = m_robFrags[i]->rod_ndata();
                // insert data into vector (to be removed soon)
                  if (roddatasize < 3) {
                    ATH_MSG_WARNING( "Error reading bytestream"
                                     <<"event: Empty ROD block (less than 3 words) : 0x"
                                     << std::hex << source_id << std::dec );
		    // Data seems corrupted
		    m_error|=0x20000000;
		    reset_LArCol ( m_col );
		    //return StatusCode::SUCCESS;
                  } // End of if small size
		  else {
                        m_lardecoder->setRobFrag(m_robFrags[i]);
                        m_lardecoder->fillCollectionHLT(roddata1,roddatasize,*m_col);
		        if (m_applyOffsetCorrection) m_larcell->applyBCIDCorrection(source_id);
		        // Accumulates inferior byte from ROD Decoder
                        m_error|=m_lardecoder->report_error();
		  }
                }
	} // end of if it
                rodidx++;
        } // End of for through RobFrags
        if ( rob_miss ) { // have to reset missing collections
                // find rod addresses missing
                std::vector<uint32_t>::const_iterator i = (*vrodids).begin();
                std::vector<uint32_t>::const_iterator j = m_vrodid32_found.begin();
                for(; i!=(*vrodids).end() && j!=m_vrodid32_found.end(); ++i,++j){
                        while ( ((*i)!=(*j)) && i!=(*vrodids).end() ) {m_vrodid32_missed.push_back(*i); ++i;}
                }
                for(; i!=(*vrodids).end(); ++i) m_vrodid32_missed.push_back(*i);
                for(std::vector<uint32_t>::const_iterator i=m_vrodid32_missed.begin(); i!=m_vrodid32_missed.end();i++){
                  const std::vector<LArCellCollection*>::const_iterator it =
                        (m_larcell->find((*i)));
                  if ( it != m_larcell->end() && (*it)->size()!=0 ) { // Already decoded collection
                    LArCellCollection *m_col = *it;
                    reset_LArCol ( m_col );
                  } // end of if
                }
        } // End of missing ROBs treatment


	if (msgLvl(MSG::VERBOSE)) {
	  int i=0;
	  LArTT_Selector<LArCellCont>::const_iterator m_it;
	  for ( m_it=Begin; m_it != End; ++m_it ){
            ATH_MSG_VERBOSE( "Eta: " << (*m_it)->eta()
                           << "; Phi: " << (*m_it)->phi() 
                           <<"; Energy: " << (*m_it)->energy() );
	    i++;
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;
} // End of method
StatusCode TrigDataAccess::LoadFullCollections (
                TileCellCollection::const_iterator& Begin,
                TileCellCollection::const_iterator& End,
                const unsigned int sample, bool /*prepare*/){

	// Resets error flag
        m_error=0;
        m_robFrags.clear();

        if ( sample == 0 ) { 
            m_robDataProvider->addROBData( m_alltile ); 
            m_robDataProvider->getROBData( m_alltile, m_robFrags ); 
            m_robFrags.clear(); 
        } 
        int i = sample;
        Begin=End;
//                m_tile[0] = m_tilecell->find_rod(m_rIdstile[i]);
//              m_tile[0] = m_tilecell->find_rod(m_rIdstile[0]);
		m_tile[0] = m_alltile[i]; 
                m_robDataProvider->addROBData(m_tile);
                m_robDataProvider->getROBData(m_tile,m_robFrags);
		// Number of ROBs smaller than requested is below
                if ( m_robFrags.empty() ) m_error|=0x10000000;
                // Find the collection to fill
                TileCellCollection* m_col = NULL;
                m_col = *(m_tilecell->find(m_rIdstile[i]));
                if ( m_robFrags.size()!=0 && m_col != NULL ) {
                size_t roddatasize = m_robFrags[0]->rod_ndata();
                // insert data into vector (to be removed soon)
                if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream"
                                   << "event: Empty ROD block (less than 3 words)" 
                                   << std::hex << m_tile[0] << std::dec );
		  // Data seems corrupted
		  m_error|=0x20000000;
                  if ( !m_tilecell->cached(m_rIdstile[i])){
                        // resets collection
                        reset_TileCol(m_col);
                  }
                  m_robFrags.clear();
                  Begin = m_col->begin();
                  End = m_col->end();
		  //return StatusCode::SUCCESS;
                } else {// End of if small size
		if ( !m_tilecell->cached(m_rIdstile[i]))
                m_tiledecoder->fillCollectionHLT(m_robFrags[0],*m_col);
                m_tiledecoder->mergeD0cellsHLT(*m_col);
                // Accumulates superior byte from ROD Decoder
                m_error|=m_tiledecoder->report_error();
                Begin = m_col->begin();
                End = m_col->end();
                m_robFrags.clear();
		}
                }
        //} // End of for through RobFrags

	if (msgLvl(MSG::VERBOSE)) {
	  TileCellCollection::const_iterator m_itt = Begin;
	  for (m_itt=Begin;m_itt!=End;++m_itt){
            ATH_MSG_VERBOSE( "Eta: " << (*m_itt)->eta()
                           << "; Phi: " << (*m_itt)->phi()
                           << "; Energy: " << (*m_itt)->energy()
                           << "; Hash Id: " << (*m_itt)->caloDDE()->calo_hash() );
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;

} // End of LoadCollections

StatusCode TrigDataAccess::LoadFullCollections (
                LArFebEnergyCollection::const_iterator& Begin,
                LArFebEnergyCollection::const_iterator& End,
                const DETID detid, bool /*prepare*/){


        m_error = 0;
        Begin = End ;
	if ( detid != TTEM ) return StatusCode::SUCCESS;
	m_robFrags.clear();
        m_febcoll->clear();

        m_robDataProvider->addROBData(m_vrodid32ros);
        m_robDataProvider->getROBData(m_vrodid32ros,m_robFrags);

	if (  m_usefullcoll && m_robFrags.size()==0 ) {
             m_robDataProvider->addROBData(m_vrodid32lar);
             m_robDataProvider->getROBData(m_vrodid32lar,m_robFrags);

        if(m_vrodid32lar.size()!=m_robFrags.size()) {
                // Number of ROBs smaller than requested is below
                m_error|=0x10000000;
        }
            
        for( size_t i=0; i< m_robFrags.size(); ++i){
	    if ( m_lardecoder->check_valid(m_robFrags[i],msg()) ) {
                uint32_t source_id = m_robFrags[i]->source_id();
                const uint32_t* roddata1 = 0;
                m_robFrags[i]->rod_data(roddata1);
                size_t roddatasize = m_robFrags[i]->rod_ndata();
                if(roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream " 
                                   << "event: Empty ROD block (less than 3 words) : 0x"
                                   << std::hex << source_id << std::dec );
		  m_error|=0x20000000; 
		  //return StatusCode::SUCCESS;
                } else {
                       m_lardecoder->setRobFrag(m_robFrags[i]);
                       m_lardecoder->fillCollectionHLTFeb(roddata1,roddatasize,*m_febcoll);
                       m_error|=m_lardecoder->report_error();
		}
	    } else {
              ATH_MSG_WARNING( "Error reading bytestream"
                               << "event: Bad ROB block (eformat checks) " );
	      // Data seems corrupted
	      m_error|=0x20000000;
	    }
        }

        } else {// Acquire data from ROSes
           // m_robDataProvider->addROBData(m_vrodid32ros); // Done previously
           // m_robDataProvider->getROBData(m_vrodid32ros,m_robFrags);
	   if ( m_robFrags.size() <= 0 ) // that is all we can do now
                m_error|=0x10000000;
           for( size_t i=0; i< m_robFrags.size(); ++i){
	      if ( m_lardecoder->check_valid(m_robFrags[i],msg()) ) {
                const uint32_t* roddata1 = 0;
                m_robFrags[i]->rod_data(roddata1);
                size_t roddatasize = m_robFrags[i]->rod_ndata();
                if(roddatasize < 3) {
                       uint32_t source_id = m_robFrags[i]->source_id();
                       ATH_MSG_WARNING( "Error reading bytestream " 
                                        << "event: Empty ROD block (less than 3 words) : 0x"
                                        << std::hex << source_id << std::dec );
                       m_error|=0x20000000;
                       //return StatusCode::SUCCESS;
                } else {
                      m_lardecoder->setRobFrag(m_robFrags[i]);
                      m_lardecoder->fillCollectionHLTROSFeb(roddata1,roddatasize,*m_febcoll);
                      m_error|=m_lardecoder->report_error();
                }
	      } else {
		ATH_MSG_WARNING( "Error reading bytestream"
                                 << "event: Bad ROB block (eformat checks) " );
		// Data seems corrupted
		m_error|=0x20000000;
	      }
           } // loop over robs
	} // if ROS feb and TTEM

        Begin = m_febcoll->begin();
        End = m_febcoll->end();

	if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "This is the detectorID = " << detid );
          ATH_MSG_VERBOSE( "This is the febcoll size = " << m_febcoll->size() );
	  for(LArFebEnergyCollection::const_iterator it = Begin; it!=End; ++it){
            ATH_MSG_VERBOSE( " Feb ID = " << (*it)->getFebId()
                           << " Feb Ex = " << (*it)->getFebEx()
                           << " Feb Ey = " << (*it)->getFebEy()
                           << " Feb Ez = " << (*it)->getFebEz() );
	  }
	}
        return StatusCode::SUCCESS;

} // End of LoadFullCollections

StatusCode TrigDataAccess::LoadFullCollections (
                TileL2Container::const_iterator& Begin,
                TileL2Container::const_iterator& End){
        // Resets error flag
        m_error=0;
        std::vector<float> sumE(3,0.0);
        for(size_t i=0; i<m_drawcoll->size();i++)
		((*m_drawcoll)[i])->setEt(sumE);

        Begin=End;
        m_robFrags.clear();
        m_robDataProvider->addROBData(m_vrodid32tros);
        m_robDataProvider->getROBData(m_vrodid32tros,m_robFrags);
	if (  m_usefullcoll && m_robFrags.size()==0 ) {
        for(size_t sample=0;sample<TileFullContSize();sample+=4){ // 4 drawers per ROB
                m_tile[0] = m_tilecell->find_rod(m_rIdstile[sample]);
        	m_robFrags.clear();
                m_robDataProvider->addROBData(m_tile);
                m_robDataProvider->getROBData(m_tile,m_robFrags);
                if ( m_robFrags.size()!=0 ) {
                size_t roddatasize = m_robFrags[0]->rod_ndata();
                // insert data into vector (to be removed soon)

                if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream"
                                   << "event: Empty ROD block (less than 3 words)"
                                   << std::hex << m_tile[0] << std::dec );
		  // Data seems corrupted
		  m_error|=0x20000000;
		  return StatusCode::SUCCESS;
                } // End of if small size
                m_tiledecoder->fillCollectionL2(m_robFrags[0],*m_drawcoll);
        	Begin = m_drawcoll->begin();
	        End = m_drawcoll->end();
		} // end of robFrags.size=0 check
	}
	} else { // ROS access
	// Data Already request
        for( size_t i=0; i< m_robFrags.size(); ++i){
             size_t roddatasize = m_robFrags[i]->rod_ndata();
             //const uint32_t* roddata1 = 0; // commented out since not used (J Stelzer)
             if(roddatasize < 3) {
             	    uint32_t source_id = m_robFrags[i]->source_id();
                    ATH_MSG_WARNING( "Error reading bytestream " 
                                     << "event: Empty ROD block (less than 3 words) : 0x"
                                     << std::hex << source_id << std::dec );
                    m_error|=0x20000000;
                    //return StatusCode::SUCCESS;
             } else {
                   m_tiledecoder->fillCollectionL2ROS(m_robFrags[i],*m_drawcoll);
        	   Begin = m_drawcoll->begin();
	           End = m_drawcoll->end();
             }
        }
	} // end of ROS check
	return StatusCode::SUCCESS;
}

StatusCode TrigDataAccess::LoadFullCollections (
		CaloCellContainer::const_iterator& Begin,
                CaloCellContainer::const_iterator& End){
        if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_VERBOSE( "m_rIds.size() in LoadColl = " << m_rIds.size() );
	  for(unsigned int i = 0 ; i < m_rIds.size() ; i++)
            ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << m_rIds[i] );
	}
        // Resets error flag
        m_error=0;
        Begin=End=m_fullCellContainer->end();
        m_robFrags.clear();
        bool rob_miss=false;
        size_t  checkSize = 0;
        // Keep a local reference
        std::vector<uint32_t>* vrodids = &m_vrodid32em;

        m_robDataProvider->addROBData( m_vrodid32fullDet );
        m_robDataProvider->getROBData( m_vrodid32fullDet, m_robFrags );
	m_robFrags.clear();
        m_robDataProvider->getROBData( m_vrodid32lar, m_robFrags );
        checkSize = m_vrodid32lar.size();

        if ( checkSize != m_robFrags.size() ){
		// Number of ROBs smaller than requested is below
		m_error|=0x10000000;
                rob_miss=true;
                m_vrodid32_found.clear();
                m_vrodid32_missed.clear();
        }
        int rodidx = 0;
        for (size_t i = 0; i < m_robFrags.size(); ++i){
        // Source ID has correct number
        uint32_t source_id = m_robFrags[i]->source_id();
        if ( rob_miss ) m_vrodid32_found.push_back(source_id);
        std::vector<LArCellCollection*>::const_iterator it;
        it = m_larcell->find(source_id);
        if (it != m_larcell->end() && (*it)->size()!=0 ) {
           LArCellCollection *m_col = *it;
           m_lardecoder->setsecfeb(m_larcell->findsec(source_id));
           if ( !m_lardecoder->check_valid(m_robFrags[i],msg()) ) {
             ATH_MSG_WARNING( "Error reading bytestream"
                              << "event: Bad ROB block (eformat checks) : 0x"
                              << std::hex << source_id << std::dec );
	     m_error|=0x20000000;
	     reset_LArCol ( m_col );
           } else {
                const uint32_t* roddata1 = 0;
                m_robFrags[i]->rod_data(roddata1);
                size_t roddatasize = m_robFrags[i]->rod_ndata();
                // insert data into vector (to be removed soon)
                if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream"
                                   << "event: Empty ROD block (less than 3 words) : 0x"
                                   << std::hex << source_id << std::dec );
		  // Data seems corrupted
		  m_error|=0x20000000;
		  reset_LArCol ( m_col );
		  //return StatusCode::SUCCESS;
                } // End of if small size
                else {
                      m_lardecoder->setRobFrag(m_robFrags[i]);
                      m_lardecoder->fillCollectionHLT(roddata1,roddatasize,*m_col);
		      if (m_applyOffsetCorrection) m_larcell->applyBCIDCorrection(source_id);
                      // Accumulates inferior byte from ROD Decoder
                      m_error|=m_lardecoder->report_error();
                }
	   }
        } // end of if it
                rodidx++;
        } // End of for through RobFrags
        if ( rob_miss ) { // have to reset missing collections
                // find rod addresses missing
                std::vector<uint32_t>::const_iterator i = (*vrodids).begin();
                std::vector<uint32_t>::const_iterator j = m_vrodid32_found.begin();
                for(; i!=(*vrodids).end() && j!=m_vrodid32_found.end(); ++i,++j){
                        while ( ((*i)!=(*j)) && i!=(*vrodids).end() ) {m_vrodid32_missed.push_back(*i); ++i;}
                }
                for(; i!=(*vrodids).end(); ++i) m_vrodid32_missed.push_back(*i);
                for(std::vector<uint32_t>::const_iterator i=m_vrodid32_missed.begin(); i!=m_vrodid32_missed.end();i++){
                  const std::vector<LArCellCollection*>::const_iterator it =
                        (m_larcell->find((*i)));
                  if ( it != m_larcell->end() && (*it)->size()!=0 ) { // Already decoded collection
                    LArCellCollection *m_col = *it;
                    reset_LArCol ( m_col );
                  } // end of if 
                }
        } // End of missing ROBs treatment

        m_error=0;
        m_robFrags.clear();

        int tilefullcont = TileFullContSize();
        for(int i=0;i<tilefullcont;i++){
                m_tile[0] = m_alltile[i];
                m_robDataProvider->getROBData(m_tile,m_robFrags);
                // Number of ROBs smaller than requested is below
                if ( m_robFrags.empty() ) m_error|=0x10000000;
                // Find the collection to fill 
                TileCellCollection* m_col = NULL;
                m_col = *(m_tilecell->find(m_rIdstile[i]));
                if ( m_robFrags.size()!=0 && m_col != NULL ) {
                size_t roddatasize = m_robFrags[0]->rod_ndata();
                // insert data into vector (to be removed soon) 
                if (roddatasize < 3) {
                  ATH_MSG_WARNING( "Error reading bytestream"
                                   << "event: Empty ROD block (less than 3 words)"
                                   << std::hex << m_tile[0] << std::dec );
		  // Data seems corrupted 
		  m_error|=0x20000000;
		  if ( !m_tilecell->cached(m_rIdstile[i])){
			// resets collection 
			reset_TileCol(m_col);
		  }
                  m_robFrags.clear();
		  continue;
                } // End of if small size
                if ( !m_tilecell->cached(m_rIdstile[i]))
                m_tiledecoder->fillCollectionHLT(m_robFrags[0],*m_col);
                m_tiledecoder->mergeD0cellsHLT(*m_col);
                // Accumulates superior byte from ROD Decoder
                m_error|=m_tiledecoder->report_error();
                m_robFrags.clear();
                } 
        } 


        Begin = m_fullCellContainer->begin();
        End   = m_fullCellContainer->end();

	if (msgLvl(MSG::VERBOSE)) {
	  int i=0;
          ATH_MSG_VERBOSE( "m_fullCellContainer->size() within LoadFullCol : " << m_fullCellContainer->size() );
	  CaloCellContainer::const_iterator m_it;
	  for ( m_it=Begin; m_it != End; ++m_it ){
            ATH_MSG_VERBOSE( "Eta: " << (*m_it)->eta()
                           << "; Phi: " << (*m_it)->phi()
                           << "; Energy: " << (*m_it)->energy() );
	    i++;
	  } // End of for printout cells
          ATH_MSG_VERBOSE( "number of counted cells : " << i );
	}
        return StatusCode::SUCCESS;
} // End of method
 

void TrigDataAccess::handle(const Incident & inc ) {
        const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
         if(!eventInc) {
             ATH_MSG_WARNING(" Unable to get EventInfo from either EventStore or BeginRun incident");
             return;
         }
         else {
	     const EventInfo* evt;
	     evt = &eventInc->eventInfo();
             m_larcell->eventNumber(evt->event_ID()->event_number());
	     if (m_applyOffsetCorrection) m_larcell->lumiBlock_BCID(evt->event_ID()->lumi_block(), evt->event_ID()->bunch_crossing_id());
             m_tilecell->eventNumber(evt->event_ID()->event_number());
             m_tiledecoder->initD0cellsHLT();
             //m_full_vrodid32.clear();
             m_present_roi = 0;
             TileCellCollection* mbts = m_tilecell->MBTS_collection();
             for (size_t i = 0; i < (*mbts).size(); ++i)
                (*mbts)[i]->setEnergy(0.0);
             m_mbts_done=false;
         }
}

void TrigDataAccess::ROBList( const IRoiDescriptor& roi, std::vector<uint32_t>& vec){
        vec.clear();
        if ( (m_vrodid32fullDet.size() > 0 ) && ( roi.isFullscan() ) ) {
		vec.insert(vec.end(), m_vrodid32fullDet.begin(), m_vrodid32fullDet.end() );
		return;
	}
        this->RegionSelectorRobID( 2, roi, TTEM, true );
        vec.insert(vec.end(),m_full_vrodid32.begin(),m_full_vrodid32.end()); 
	return;
}
