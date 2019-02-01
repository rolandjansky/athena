/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     TrigDataAccessATLFAST.cxx
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

#include "TrigT2CaloCommon/TrigDataAccessATLFAST.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// Event Incident to get EventInfo
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"

// Initialize method for all tools
// Retrieval of all Tools to be used during run
StatusCode TrigDataAccessATLFAST::initialize()
{

        ATH_MSG_DEBUG("in initialize() by TrigDataAccessATLFAST");

	// Some tools are necessary for the Algorithm to run
	// The RegionSelector is being retrieved here
        if( (m_pRegionSelector.retrieve()).isFailure() ) {
	  ATH_MSG_FATAL("Unable to retrieve RegionSelector Service");
	  return StatusCode::FAILURE;
	}

	// Reserve vectors space to avoid online memory allocation
	// Limits to be studied
	m_rIds.reserve(30);
	m_vrodid32.reserve(30);
	m_tile.push_back(0);
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
	if ( detStore()->regFcn(&ITrigDataAccess::beginRunHandle,
		(ITrigDataAccess*)this,
				 febrodmap, "/LAR/Identifier/FebRodMap", true).isFailure() ) { //FIXME hardcoded database folder name
	  ATH_MSG_ERROR(" Can't regFnc with Condition ");
	  return StatusCode::FAILURE;
        }
	// Just to the Missing Et slice
        if ( m_usefullcoll ) {
	IRegSelSvc* regSelSvc = &(*m_pRegionSelector);
	if ( detStore()->regFcn(&IRegSelSvc::handle,
		regSelSvc,
		&ITrigDataAccess::beginRunHandle_RegSelSvc,
		(ITrigDataAccess*)this,true).isFailure() ) {
	  ATH_MSG_ERROR(" Can't regFnc with Condition");
	  return StatusCode::FAILURE;
	}
          m_rIdsem0.reserve(12288);
          m_rIdsem1.reserve(300);
          m_rIdsem2.reserve(300);
          m_rIdsem3.reserve(300);
          m_rIdshec.reserve(4352);
          m_rIdshec0.reserve(300);
          m_rIdshec1.reserve(300);
          m_rIdshec2.reserve(300);
          m_rIdsfcalhad0.reserve(256);
          m_rIdsfcalhad1.reserve(128);
          m_rIdsfcalem0.reserve(300);
          m_rIdstile.reserve(300);
          m_vrodid32lar.reserve(300);
          m_vrodid32em.reserve(724);
          m_vrodid32hec.reserve(72);
          m_vrodid32hec0.reserve(24);
          m_vrodid32hec1.reserve(24);
          m_vrodid32hec2.reserve(24);
          m_vrodid32fcalem.reserve(8);
          m_vrodid32fcalhad.reserve(6);
          m_vrodid32tile.reserve(300);
	} // end of m_usefullcoll

	return StatusCode::SUCCESS;
} // End of initialize

StatusCode TrigDataAccessATLFAST::beginRunHandle(IOVSVC_CALLBACK_ARGS){
	// The ByteStreamCnv (Through LArTT_Selector needs a map of the
	// RoIs, being retrieved here
	if(toolSvc()->retrieveTool("LArRoI_Map",m_roiMap).isFailure()) {
	  ATH_MSG_FATAL("Could not find LArRoI_Map");
	  return StatusCode::FAILURE;
	} // End of if LArRoI_Map

#ifdef DOBYTESTREAMCNV
        //m_datablock.reserve(350);
	m_larcell = new LArCellCont();
	if ( (m_larcell->initialize()).isFailure() ){
	  ATH_MSG_FATAL("Could not init larcell");
        }
        m_sel= new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
        if(m_usefullcoll){
          m_selem = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selhec = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selfcalhad = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
          m_selfcalem = new LArTT_Selector<LArCellCont>(m_roiMap,m_larcell);
        }
	m_tilecell = new TileCellCont();
        if ( (m_tilecell->initialize()).isFailure() ){
	  ATH_MSG_FATAL("Could not init tilecell");
        }
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
          if ( m_rIdsfcalem0.size() != 0 ) m_rIdsfcalem0.clear();
          if ( m_rIdstile.size() != 0 ) m_rIdstile.clear();
	} // End of m_usefullcoll
#endif
	m_iov_called=true;
	return StatusCode::SUCCESS;
}



StatusCode TrigDataAccessATLFAST::beginRunHandle_RegSelSvc(IOVSVC_CALLBACK_ARGS){
	  if(m_usefullcoll){
	    ATH_MSG_DEBUG("Finalizing Preparation of full collections");
	  //	  TrigRoiDescriptor tmproi( 0, -4.8, 4.8, 0, -M_PI, M_PI, 0, 0, 0 );
	  TrigRoiDescriptor tmproi( true );  /// give it true during the constructor, you get a full scan RoI

	  //          const double mineta = -4.8;
	  //          const double maxeta = 4.8;
	  //          const double minphi = -M_PI;
	  //          const double maxphi = M_PI;
	  //           // TTEM 
	  //           m_pRegionSelector->DetROBIDListUint(TTEM,-1,mineta,maxeta,minphi,maxphi,m_vrodid32em);
	  //           // TTHEC
	  //           m_pRegionSelector->DetROBIDListUint(TTHEC,0,mineta,maxeta,minphi,maxphi,m_vrodid32hec0);
	  //           m_pRegionSelector->DetROBIDListUint(TTHEC,1,mineta,maxeta,minphi,maxphi,m_vrodid32hec1);
	  //           m_pRegionSelector->DetROBIDListUint(TTHEC,2,mineta,maxeta,minphi,maxphi,m_vrodid32hec2);
	  //           // FCALHAD
	  //           m_pRegionSelector->DetROBIDListUint(FCALHAD,-1,mineta,maxeta,minphi,maxphi,m_vrodid32fcalhad);
	  //           m_pRegionSelector->DetROBIDListUint(FCALEM,-1,mineta,maxeta,minphi,maxphi,m_vrodid32fcalem);
	  //           // TILE
	  //           m_pRegionSelector->DetROBIDListUint(TILE,mineta,maxeta,minphi,maxphi,m_vrodid32tile);
	  //           m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32em.begin(),m_vrodid32em.end());
	  //           m_vrodid32hec.insert(m_vrodid32hec.end(),m_vrodid32hec0.begin(),m_vrodid32hec0.end());
	  //           m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalhad.begin(),m_vrodid32fcalhad.end());
	  //           m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalem.begin(),m_vrodid32fcalem.end());
	  //           m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32hec.begin(),m_vrodid32hec.end());
	  //           m_pRegionSelector->DetHashIDList(TTEM,0,mineta,maxeta,minphi,maxphi,&m_rIdsem0);
	  //           m_pRegionSelector->DetHashIDList(TTEM,1,mineta,maxeta,minphi,maxphi,&m_rIdsem1);
	  //           m_pRegionSelector->DetHashIDList(TTEM,2,mineta,maxeta,minphi,maxphi,&m_rIdsem2);
	  //           m_pRegionSelector->DetHashIDList(TTEM,3,mineta,maxeta,minphi,maxphi,&m_rIdsem3);
	  //           m_pRegionSelector->DetHashIDList(TTHEC,0,mineta,maxeta,minphi,maxphi,&m_rIdshec0);
	  //           m_pRegionSelector->DetHashIDList(TTHEC,1,mineta,maxeta,minphi,maxphi,&m_rIdshec1);
	  //           m_pRegionSelector->DetHashIDList(TTHEC,2,mineta,maxeta,minphi,maxphi,&m_rIdshec2);
	  //           m_pRegionSelector->DetHashIDList(FCALHAD,0,mineta,maxeta,minphi,maxphi,&m_rIdsfcalhad0);
	  //           m_pRegionSelector->DetHashIDList(FCALHAD,1,mineta,maxeta,minphi,maxphi,&m_rIdsfcalhad1);
	  //           m_pRegionSelector->DetHashIDList(FCALEM,0,mineta,maxeta,minphi,maxphi,&m_rIdsfcalem0);
	  //           m_pRegionSelector->DetHashIDList(TILE,mineta,maxeta,minphi,maxphi,&m_rIdstile);
	  

	  m_pRegionSelector->DetROBIDListUint(TTEM,-1, tmproi, m_vrodid32em);
          // TTHEC
          m_pRegionSelector->DetROBIDListUint(TTHEC,0, tmproi, m_vrodid32hec0);
          m_pRegionSelector->DetROBIDListUint(TTHEC,1, tmproi, m_vrodid32hec1);
          m_pRegionSelector->DetROBIDListUint(TTHEC,2, tmproi, m_vrodid32hec2);
          // FCALHAD
          m_pRegionSelector->DetROBIDListUint(FCALHAD,-1, tmproi, m_vrodid32fcalhad);
          m_pRegionSelector->DetROBIDListUint(FCALEM,-1,  tmproi, m_vrodid32fcalem);
          // TILE
          m_pRegionSelector->DetROBIDListUint(TILE, tmproi, m_vrodid32tile);
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32em.begin(),m_vrodid32em.end());
          m_vrodid32hec.insert(m_vrodid32hec.end(),m_vrodid32hec0.begin(),m_vrodid32hec0.end());
	  m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalhad.begin(),m_vrodid32fcalhad.end());
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32fcalem.begin(),m_vrodid32fcalem.end());
          m_vrodid32lar.insert(m_vrodid32lar.end(),m_vrodid32hec.begin(),m_vrodid32hec.end());
          m_pRegionSelector->DetHashIDList(TTEM,0, tmproi, m_rIdsem0);
          m_pRegionSelector->DetHashIDList(TTEM,1, tmproi, m_rIdsem1);
          m_pRegionSelector->DetHashIDList(TTEM,2, tmproi, m_rIdsem2);
          m_pRegionSelector->DetHashIDList(TTEM,3, tmproi, m_rIdsem3);
          m_pRegionSelector->DetHashIDList(TTHEC,0, tmproi, m_rIdshec0);
          m_pRegionSelector->DetHashIDList(TTHEC,1, tmproi, m_rIdshec1);
          m_pRegionSelector->DetHashIDList(TTHEC,2, tmproi, m_rIdshec2);
          m_pRegionSelector->DetHashIDList(FCALHAD,0, tmproi, m_rIdsfcalhad0);
          m_pRegionSelector->DetHashIDList(FCALHAD,1, tmproi, m_rIdsfcalhad1);
	  m_pRegionSelector->DetHashIDList(FCALEM,0, tmproi, m_rIdsfcalem0);
          m_pRegionSelector->DetHashIDList(TILE, tmproi,  m_rIdstile);
      

          // TTEM 
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem1.begin(),m_rIdsem1.end());
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem2.begin(),m_rIdsem2.end());
          m_rIdsem0.insert(m_rIdsem0.end(),m_rIdsem3.begin(),m_rIdsem3.end());
          // TTHEC
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec0.begin(),m_rIdshec0.end());
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec1.begin(),m_rIdshec1.end());
          m_rIdshec.insert(m_rIdshec.end(),m_rIdshec2.begin(),m_rIdshec2.end());
          // FCALHAD
          m_rIdsfcalhad0.insert(m_rIdsfcalhad0.end(),m_rIdsfcalhad1.begin(),m_rIdsfcalhad1.end());

          m_selem->setRoIs(m_rIdsem0);
          m_selhec->setRoIs(m_rIdshec);
          m_selfcalhad->setRoIs(m_rIdsfcalhad0);
          m_selfcalem->setRoIs(m_rIdsfcalem0);
	} // End of m_usefullcoll
	return StatusCode::SUCCESS;
}

// Finalize method for all tools
StatusCode TrigDataAccessATLFAST::finalize(){
        if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_INFO( "in finalize() by TrigDataAccess" );
	}
#ifdef DOBYTESTREAMCNV
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
#endif
	return StatusCode::SUCCESS;

} // End of finalize

// This should be run to provide ByteStreamCnv with rodid's to
// look up for. addROBData only works in athenaMT
void TrigDataAccessATLFAST::RegionSelectorRobID (const int sampling,
						 const IRoiDescriptor& roi,
						 const DETID detid, bool /*fetchROBs*/) {
        // Clears up m_vrodid32
        if ( m_vrodid32.size() != 0 ) m_vrodid32.clear();
        if ( detid != TILE ){
	  m_pRegionSelector->DetROBIDListUint(detid,sampling, roi, m_vrodid32);
        }
        else { // TILE does not need sample
	  m_pRegionSelector->DetROBIDListUint(detid, roi, m_vrodid32);
        }
	if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG( "m_vrodid32.size() = " << m_vrodid32.size() );
	  for(unsigned int i = 0 ; i < m_vrodid32.size() ; i++)
            ATH_MSG_DEBUG( "m_vrodid32[" << i << "]=" << m_vrodid32[i] );
	}
}  // End of RegionSelectorRobID


// This code should load m_listID with Detectors ID
// Should this return a flag in case of failure?
void TrigDataAccessATLFAST::RegionSelectorListID (const int sampling,
						  const IRoiDescriptor& roi, const DETID detid) {
        // Clears up m_rIds
        if ( m_rIds.size() != 0 ) m_rIds.clear();
  
        if ( detid != TILE ){
	  m_pRegionSelector->DetHashIDList(detid,sampling, roi, m_rIds);
	}
        else{ // TILE does not need sample
	  m_pRegionSelector->DetHashIDList(detid, roi, m_rIds);					   
	}
	
	if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG( "m_rIds.size() = " << m_rIds.size() );
	  for(unsigned int i = 0; i < m_rIds.size() ; i++)
            ATH_MSG_DEBUG( "m_rIds[" << i << "]=" << m_rIds[i] );
	}	
} // End of RegionSelectorListID





// This method should load all the infrastructure for the offline
// Collection of cells. The iterators m_iBegin, m_iEnd and m_it
// will have the cells Collection to be used by the Algorithm
StatusCode TrigDataAccessATLFAST::LoadCollections (
		LArTT_Selector<LArCellCont>::const_iterator& Begin,
		LArTT_Selector<LArCellCont>::const_iterator& End,
		const unsigned int /*sample*/, bool /*prepare*/) {
        if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG( "m_rIds.size() in LoadColl = " << m_rIds.size() );
	  for(unsigned int i = 0 ; i < m_rIds.size() ; i++)
            ATH_MSG_DEBUG( "m_rIds[" << i << "]=" << m_rIds[i] );
	}

	Begin=End;

	const DataHandle<CaloCellContainer> cells;
	if (evtStore()->retrieve(cells,"AllCalo").isFailure()) {
	  ATH_MSG_ERROR("Could not find AllCalo container");
	  return StatusCode::FAILURE;
        } // End of if StoreGateSvc

        int rodidx = 0;
        for (size_t i = 0; i < m_vrodid32.size(); ++i){
		// Find the collection to fill
		const std::vector<LArCellCollection*>::const_iterator it =
			(m_larcell->find(m_vrodid32[rodidx]));
/*
        	LArCellCollection* col =
			*(m_larcell->find(m_vrodid32[rodidx]));
*/
		if ( it != m_larcell->end() ) { // Alread decoded collection
		LArCellCollection *col = *it;
		// Fills from the container
		fillColl(cells,*col);
        	// if (col->size() == 0) return StatusCode::FAILURE;
		}
		rodidx++;

  	} // End of for through RobFrags

        // These iterators can be used by every Tool
        m_sel->setRoIs(m_rIds);
        Begin = m_sel->begin();
        End   = m_sel->end();
	if (msgLvl(MSG::DEBUG)) {
	  LArTT_Selector<LArCellCont>::const_iterator it;
	  for ( it=Begin; it != End; ++it ){
            ATH_MSG_DEBUG( "Eta: " << (*it)->eta()
                           << "; Phi: " << (*it)->phi() <<
                           "; Energy: " << (*it)->energy() );
	  } // End of for printout cells
	}
	return StatusCode::SUCCESS;
} // End of method
StatusCode TrigDataAccessATLFAST::LoadCollections (
                TileCellCollection::const_iterator& Begin,
                TileCellCollection::const_iterator& End,
                const unsigned int sample, bool /*prepare*/){

	const DataHandle<CaloCellContainer> cells;
	if (evtStore()->retrieve(cells,"AllCalo").isFailure()) {
	  ATH_MSG_ERROR("Could not find AllCalo container");
	  return StatusCode::FAILURE;
        } // End of if StoreGateSvc

	int i = sample;
	Begin=End;
	//for (size_t i = 0; i < m_listID.size(); ++i){
		m_tile[0] = m_tilecell->find_rod(m_rIds[i]);
		// Find the collection to fill
                TileCellCollection* col = NULL;
		col = *(m_tilecell->find(m_rIds[i]));
		if ( col != NULL ) {
		fillColl(cells,*col);
                Begin = col->begin();
                End = col->end();
		}

        //} // End of for through RobFrags

	if (msgLvl(MSG::DEBUG)) {
	  TileCellCollection::const_iterator itt = Begin;
	  for (itt=Begin;itt!=End;++itt){
            ATH_MSG_DEBUG( "Eta: " << (*itt)->eta()
                           << "; Phi: " << (*itt)->phi() <<
                           "; Energy: " << (*itt)->energy() << 
                           "; Hash Id: " << (*itt)->caloDDE()->calo_hash() );
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;

} // End of LoadCollections

// This method does not make sense for ATLFAST
StatusCode TrigDataAccessATLFAST::LoadCollections (
                LArFebEnergyCollection::const_iterator& /*Begin*/,
                LArFebEnergyCollection::const_iterator& /*End*/,
                const unsigned int /*sample*/, bool /*prepare*/){

/*
	m_robFrags.clear();
        m_robDataProvider->getROBData(m_vrodid32,m_robFrags);

        //int rodidx = 0;
	LArFebEnergyCollection* m_febcoll;
        m_febcoll = new LArFebEnergyCollection();
        for (size_t i = 0; i < m_robFrags.size(); ++i){
                m_lardecoder->check_valid(m_robFrags[i],msg());
                //m_datablock.clear();
                // Get Rod Data and size of fragment
                const uint32_t* roddata1 = 0;
                m_robFrags[i]->rod_data(roddata1);
                size_t roddatasize = m_robFrags[i]->rod_ndata();
                if (roddatasize < 3) {
                        ATH_MSG_FATAL( "Error reading bytestream"<<
                           "event: Empty ROD block (less than 3 words)" );
                        return StatusCode::FAILURE;
                } // End of if small size
                m_lardecoder->setRobFrag(m_robFrags[i]);
                m_lardecoder->fillCollectionHLTFeb(roddata1,roddatasize,*m_febcoll);
                // if (col->size() == 0) return StatusCode::FAILURE;

        } // End of for through RobFrags
        // These iterators can be used by every Tool
        Begin = m_febcoll->begin();
        End   = m_febcoll->end();

#ifndef NDEBUG
        for(LArFebEnergyCollection::const_iterator it = Begin; 
              it!=End; ++it){
              ATH_MSG_DEBUG( " Feb ID = " << (*it)->getFebId() 
                       << " Feb Ex = " << (*it)->getFebEx()
                       << " Feb Ey = " << (*it)->getFebEy() 
                       << " Feb Ez = " << (*it)->getFebEz() );
        }
#endif
*/
        return StatusCode::SUCCESS;

} // End of LoadCollections

StatusCode TrigDataAccessATLFAST::LoadFullCollections (
                LArTT_Selector<LArCellCont>::const_iterator& Begin,
                LArTT_Selector<LArCellCont>::const_iterator& End,
                const DETID /*detid*/, bool /*prepare*/) {

        if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG( "m_rIds.size() in LoadColl = " << m_rIds.size() );
	  for(unsigned int i = 0 ; i < m_rIds.size() ; i++)
            ATH_MSG_DEBUG( "m_rIds[" << i << "]=" << m_rIds[i] );
	}

        Begin=End;

        const DataHandle<CaloCellContainer> cells;
        if (evtStore()->retrieve(cells,"AllCalo").isFailure()) {
	  ATH_MSG_ERROR("Could not find AllCalo container");
	  return StatusCode::FAILURE;
        } // End of if StoreGateSvc

        int rodidx = 0;
        for (size_t i = 0; i < m_vrodid32lar.size(); ++i){
           const std::vector<LArCellCollection*>::const_iterator it =
                   (m_larcell->find(m_vrodid32lar[rodidx]));
          if (it != m_larcell->end() ) {
              LArCellCollection *col = *it;
              fillColl(cells,*col);
          }
                rodidx++;
        } // End of for through RobFrags


	if (msgLvl(MSG::DEBUG)) {
	  int i=0;
	  LArTT_Selector<LArCellCont>::const_iterator it;
	  for ( it=Begin; it != End; ++it ){
            ATH_MSG_DEBUG( "Eta: " << (*it)->eta()
                           << "; Phi: " << (*it)->phi() <<
                           "; Energy: " << (*it)->energy() );
	    i++;
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;
} // End of method
StatusCode TrigDataAccessATLFAST::LoadFullCollections (
                TileCellCollection::const_iterator& Begin,
                TileCellCollection::const_iterator& End,
                const unsigned int sample, bool /*prepare*/){

        const DataHandle<CaloCellContainer> cells;
        if (evtStore()->retrieve(cells,"AllCalo").isFailure()) {
	  ATH_MSG_ERROR("Could not find AllCalo container");
	    return StatusCode::FAILURE;
        } // End of if StoreGateSvc


        int i = sample;
        Begin=End;
                m_tile[0] = m_tilecell->find_rod(m_rIdstile[i]);
                // Find the collection to fill
                TileCellCollection* col = NULL;
                col = *(m_tilecell->find(m_rIdstile[i]));
                if( col!=NULL){
                  fillColl(cells,*col);
                  Begin = col->begin();
                  End = col->end();
                }

	if (msgLvl(MSG::DEBUG)) {
	  TileCellCollection::const_iterator itt = Begin;
	  for (itt=Begin;itt!=End;++itt){
            ATH_MSG_DEBUG( "Eta: " << (*itt)->eta()
                           << "; Phi: " << (*itt)->phi() <<
                           "; Energy: " << (*itt)->energy() <<
                           "; Hash Id: " << (*itt)->caloDDE()->calo_hash() );
	  } // End of for printout cells
	}
        return StatusCode::SUCCESS;

} // End of LoadCollections

// The following method does not make sense to ATLFAST
StatusCode TrigDataAccessATLFAST::LoadFullCollections (
                LArFebEnergyCollection::const_iterator& /*Begin*/,
                LArFebEnergyCollection::const_iterator& /*End*/,
                const DETID /*detid*/, bool /*prepare*/){


/*
	m_robFrags.clear();
        switch (detid) {
           case TTEM:
             m_robDataProvider->getROBData(m_vrodid32em,m_robFrags);
             break;
           case TTHEC:
             m_robDataProvider->getROBData(m_vrodid32hec0,m_robFrags);
             break;
           case FCALEM:
             m_robDataProvider->getROBData(m_vrodid32fcalem,m_robFrags);
             break;
           case FCALHAD:
             m_robDataProvider->getROBData(m_vrodid32fcalhad,m_robFrags);
             break;
           default:
             break;
        }

        LArFebEnergyCollection* m_febcoll;
        m_febcoll = new LArFebEnergyCollection();

        for( size_t i=0; i< m_robFrags.size(); ++i){
                m_lardecoder->check_valid(m_robFrags[i],msg());
                const uint32_t* roddata1 = 0;
                m_robFrags[i]->rod_data(roddata1);
                size_t roddatasize = m_robFrags[i]->rod_ndata();
                if(roddatasize < 3) {
                       ATH_MSG_FATAL( "Error reading bytestream " <<
                                  "event: Empty ROD block (less than 3 words)" );
                       return StatusCode::FAILURE;
                }
                m_lardecoder->setRobFrag(m_robFrags[i]);
                m_lardecoder->fillCollectionHLTFeb(roddata1,roddatasize,*m_febcoll);
        }

        Begin = m_febcoll->begin();
        End = m_febcoll->end();
        std::cout << "This is the detectorID = " << detid << std::endl;
        std::cout << "This is the febcoll size = " << m_febcoll->size() << std::endl;

#ifndef NDEBUG
        for(LArFebEnergyCollection::const_iterator it = Begin;
              it!=End; ++it){
              ATH_MSG_DEBUG( " Feb ID = " << (*it)->getFebId()
                       << " Feb Ex = " << (*it)->getFebEx()
                       << " Feb Ey = " << (*it)->getFebEy()
                       << " Feb Ez = " << (*it)->getFebEz() );
        }
#endif
*/
        return StatusCode::SUCCESS;

} // End of LoadFullCollections

// The following method does not make sense to ATLFAST
StatusCode TrigDataAccessATLFAST::LoadFullCollections (
                TileL2Container::const_iterator& /*Begin*/,
                TileL2Container::const_iterator& /*End*/){
        return StatusCode::SUCCESS;

} // End of LoadFullCollections

// handle events (beginEvent only) to prepare containers
void TrigDataAccessATLFAST::handle(const Incident & inc ) {
  const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
  if(!eventInc) {
    ATH_MSG_ERROR(" Unable to get EventInfo from either EventStore or BeginRun incident");
    return;
  }
  else {
    m_larcell->eventNumber(inc.context().eventID().event_number());
  }
}

template<class T>
void TrigDataAccessATLFAST::fillColl(const DataHandle<CaloCellContainer>& input, T& output){
	typedef typename T::iterator IT;
	IT it,end;
	it=  output.begin();
	end= output.end();
	for( ; it!= end; ++it ) {
		if ( (*it)->caloDDE() != NULL ) {
		  IdentifierHash id = (*it)->caloDDE()->calo_hash();
		  const CaloCell* cell = NULL;
		  cell = input->findCell(id);
		  if ( cell != NULL ) (*it)->setEnergy(cell->energy()+1.0);
		}
	}
}



