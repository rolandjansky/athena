/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// implementation of LArCellBuilderFromLArRawChannelTool 
//                             H.Ma              Mar 2001

// Updated Jun 10, 2001.    H. Ma
// made a tool June 2, 2004 D. Rousseau
// Major overhaul, Feb 2008, W.Lampl
// Migrate to athenaMT, March 2018, W.Lampl


#include "LArCellBuilderFromLArRawChannelTool.h"
#include "LArRecEvent/LArCell.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include <bitset>

LArCellBuilderFromLArRawChannelTool::LArCellBuilderFromLArRawChannelTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent),
   m_rawChannelsKey("LArRawChannels"),
   m_addDeadOTX(true),
   m_initialDataPoolSize(-1),
   m_nTotalCells(0),
   m_caloDDM(nullptr),
   m_cablingKey("LArOnOffIdMap"),
   m_onlineID(nullptr),
   m_caloCID(nullptr),
   m_missingFebKey("LArBadFeb")
{ 
  //key of input raw channel
  declareProperty("RawChannelsName",m_rawChannelsKey,"Name of input container");
  // bad channel tool
  declareProperty("MissingFebKey",m_missingFebKey,"Key of conditions data object holding bad-feb info");
  // activate creation of cells from missing Febs
  declareProperty("addDeadOTX",m_addDeadOTX,"Add dummy cells for missing FEBs");
  declareProperty("InitialCellPoolSize",m_initialDataPoolSize,"Initial size of the DataPool<LArCells>");
  declareProperty("LArCablingKey",m_cablingKey,"Key of  conditions data object holding cabling");
}



LArCellBuilderFromLArRawChannelTool::~LArCellBuilderFromLArRawChannelTool() { 
}


StatusCode LArCellBuilderFromLArRawChannelTool::initialize() {

  ATH_CHECK(m_rawChannelsKey.initialize());

  ATH_MSG_DEBUG("Accesssing CaloDetDescrManager");
  m_caloDDM = CaloDetDescrManager::instance() ;

  ATH_MSG_DEBUG("Accesssing CaloCellID");
  m_caloCID = m_caloDDM->getCaloCell_ID();

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );

  if (!m_missingFebKey.key().empty()) {
    ATH_CHECK( m_missingFebKey.initialize() );
  }
  else {
    if (m_addDeadOTX) {
      ATH_MSG_ERROR( "Configuration problem: 'addDeadOTX' set, but no bad-channel tool given."  );
      return StatusCode::FAILURE;
    }
  }

  //Compute total number of cells


  m_nTotalCells=0;

  IdentifierHash caloCellMin, caloCellMax ;
  m_caloCID->calo_cell_hash_range(CaloCell_ID::LAREM,caloCellMin,caloCellMax);
  m_nTotalCells+=caloCellMax-caloCellMin;
  m_caloCID->calo_cell_hash_range(CaloCell_ID::LARHEC,caloCellMin,caloCellMax);
  m_nTotalCells+=caloCellMax-caloCellMin;
  m_caloCID->calo_cell_hash_range(CaloCell_ID::LARFCAL,caloCellMin,caloCellMax);
  m_nTotalCells+=caloCellMax-caloCellMin;
  
  if (m_initialDataPoolSize<0)  m_initialDataPoolSize=m_nTotalCells;
  ATH_MSG_DEBUG("Initial size of DataPool<LArCell>: " << m_initialDataPoolSize);
  
  ATH_MSG_DEBUG("Initialisating finished");
  return StatusCode::SUCCESS;
}



// ========================================================================================== //
StatusCode
LArCellBuilderFromLArRawChannelTool::process (CaloCellContainer* theCellContainer,
                                              const EventContext& ctx) const
{
  if (theCellContainer->ownPolicy() == SG::OWN_ELEMENTS) {
    ATH_MSG_ERROR( "Called with a CaloCellContainer with wrong ownership policy! Need a VIEW container!"  );
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<LArRawChannelContainer> rawColl(m_rawChannelsKey, ctx);
  if(!rawColl.isValid()) { 
    ATH_MSG_ERROR( " Can not retrieve LArRawChannelContainer: "
                   << m_rawChannelsKey.key()  );
    return StatusCode::FAILURE;      
   }  

   const size_t nRawChannels=rawColl->size();
   if (nRawChannels==0) {
     ATH_MSG_WARNING( "Got empty LArRawChannel container. Do nothing"  );
     return StatusCode::SUCCESS;
   }
   else
     ATH_MSG_DEBUG("Got " << nRawChannels << " LArRawChannels");
   

  unsigned nCellsAdded=0;
  std::bitset<CaloCell_ID::NSUBCALO> includedSubcalos;
  // resize calo cell container to correct size
  if (theCellContainer->size()) {
    ATH_MSG_ERROR( "fillCompleteCellCont: container should be empty! Clear now."   );
    theCellContainer->clear();
  }

  DataPool<LArCell> pool (ctx);

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl(m_cablingKey, ctx);
  const LArOnOffIdMapping* cabling=*cablingHdl;

  
  theCellContainer->resize(m_nTotalCells);
  //LArRawChannelContainer::const_iterator itrRawChannel=rawColl->begin();
  //LArRawChannelContainer::const_iterator lastRawChannel=rawColl->end();
  //for ( ; itrRawChannel!=lastRawChannel; ++itrRawChannel) {

  for (const LArRawChannel& rawChan : *rawColl) {
    const HWIdentifier hwid=rawChan.channelID();
    if ( cabling->isOnlineConnected(hwid)) {
      const Identifier id=cabling->cnvToIdentifier(hwid);
      const IdentifierHash hashid= m_caloCID->calo_cell_hash(id);
      const CaloDetDescrElement * theDDE=m_caloDDM->get_element(hashid);
        
      LArCell *pCell   = pool.nextElementPtr();

      *pCell = LArCell (theDDE,
			id,
			rawChan.energy(),
			rawChan.time(),
			rawChan.quality(),
			rawChan.provenance(),
			rawChan.gain());

      if ((*theCellContainer)[hashid]) {
	ATH_MSG_WARNING( "Channel added twice! Data corruption? hash=" << hashid  
			 << " online ID=0x" << std::hex << hwid.get_identifier32().get_compact()  
			 << std::dec << "  " << m_onlineID->channel_name(hwid));
      }
      else {
	(*theCellContainer)[hashid]=pCell;
	++nCellsAdded;
	includedSubcalos.set(m_caloCID->sub_calo(hashid));
      }
    }//end if connected
  }//end loop over LArRawChannelContainer

  //Now add in dummy cells (if requested)
    unsigned nMissingButPresent=0;

  if (m_addDeadOTX) {
    SG::ReadCondHandle<LArBadFebCont> missingFebHdl(m_missingFebKey, ctx);
    const LArBadFebCont::BadChanVec& allMissingFebs=(*missingFebHdl)->fullCont();

    for (const LArBadFebCont::BadChanEntry& it : allMissingFebs) {
      const LArBadFeb& febstatus=it.second;
      if (febstatus.deadReadout()  ||  febstatus.deadAll() || febstatus.deactivatedInOKS() ) {
	const HWIdentifier febId(it.first);
	//Loop over channels belonging to this FEB
	const int chansPerFeb=m_onlineID->channelInSlotMax(febId);
	for (int ch=0; ch<chansPerFeb; ++ch) {
	  const HWIdentifier hwid = m_onlineID->channel_Id(febId, ch);
	  if ( cabling->isOnlineConnected(hwid)) {
	    const Identifier id=cabling->cnvToIdentifier(hwid);
	    const IdentifierHash hashid= m_caloCID->calo_cell_hash(id);
	    const CaloDetDescrElement * theDDE=m_caloDDM->get_element(hashid);
	    LArCell *pCell   = pool.nextElementPtr();
	    *pCell = LArCell (theDDE,
			      id,
			      0.0, //energy
			      0.0, //time
			      0.0, //quality;
			      0x0A00,   // 0x0800 (dead) + 0x0200  (to indicate missing readout)
			      CaloGain::LARHIGHGAIN);
	  
	
	    if ((*theCellContainer)[hashid]) {
	      ++nMissingButPresent;
	      ATH_MSG_DEBUG("The supposedly missing channel with online ID=0x" << std::hex 
			    << hwid.get_identifier32().get_compact() << std::dec 
			    << "  " << m_onlineID->channel_name(hwid) 
			    << " is actually present in the LArRawChannel container");
	    }
	    else {
	      (*theCellContainer)[hashid]=pCell;
	      ++nCellsAdded;
	      includedSubcalos.set(m_caloCID->sub_calo(hashid));
	    }
	  }//end if connected
	}//end loop over channels of one missing FEB
      }//end if is dead/missing FEB
    }//end loop over bad/missing FEBs
  }//end if  m_addDeadOTX

  //Set the 'hasCalo' variable of CaloCellContainer
  for (int iCalo=0;iCalo<CaloCell_ID::NSUBCALO;++iCalo) {
    if (includedSubcalos.test(iCalo))
      theCellContainer->setHasCalo(static_cast<CaloCell_ID::SUBCALO>(iCalo));
  }

  if (nMissingButPresent) 
    ATH_MSG_WARNING( "A total of " << nMissingButPresent 
                     << " supposedly missing channels where present in the LArRawChannelContainer"  );

  if (nCellsAdded!=m_nTotalCells) {
    ATH_MSG_DEBUG("Filled only  " << nCellsAdded << " out of " << m_nTotalCells << " cells. Now search for holes..");
    //Clear out holes by pointer-reshuffling
    //Note this works only because the cells are actually owned by the DataPool<LArCell>
    size_t i=0,j=1;
    for (i=0;i<m_nTotalCells;++i) {
      if (theCellContainer->at(i)==0) {
	ATH_MSG_VERBOSE("Cell with hash " << i << " missing");
	if (j<=i) j=i+1;
	while (j<m_nTotalCells && theCellContainer->at(j)==0) ++j;
	if (j>=m_nTotalCells) break;
	//Now j points to next filled place
	theCellContainer->at(i)=theCellContainer->at(j);
	theCellContainer->at(j)=NULL;
	ATH_MSG_VERBOSE("Replacing cell with hash " << i << " by cell from position " << j);
      }//end if at(i)==0
    }//end loop over cells
    theCellContainer->resize(nCellsAdded);
    ATH_MSG_DEBUG("Resized the cell container to " << nCellsAdded << "(" << m_nTotalCells-nCellsAdded << " cells missing)");
  }//end if nCellsAdded!=m_nTotalCells
  else
    ATH_MSG_DEBUG("All " << nCellsAdded << "cells filled (no holes)");

  return StatusCode::SUCCESS;
}
