/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// implementation of LArCellBuilderFromLArRawChannelTool 
//                             H.Ma              Mar 2001

// Updated Jun 10, 2001.    H. Ma
// made a tool June 2, 2004 D. Rousseau
// Major overhaul, Feb 2008, W.Lampl


#include "LArRecEvent/LArCell.h"
#include "LArCellRec/LArCellBuilderFromLArRawChannelTool.h"
#include "LArCabling/LArCablingService.h"
#include "LArRecEvent/LArCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/ReadHandle.h"


#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadFeb.h"

#include <bitset>

LArCellBuilderFromLArRawChannelTool::LArCellBuilderFromLArRawChannelTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_rawChannelsKey("LArRawChannels"),
   m_addDeadOTX(true),
   m_initialDataPoolSize(-1),
   m_nTotalCells(0),
   m_caloDDM(nullptr),
   m_cablingSvc("LArCablingService"),
   m_onlineID(nullptr),
   m_caloCID(nullptr),
   m_badChannelTool("LArBadChanTool"),
   m_nDeadFEBs(0)
{ 
  declareInterface<ICaloCellMakerTool>(this); 
  //key of input raw channel
  declareProperty("RawChannelsName",m_rawChannelsKey,"Name of input container");
  // bad channel tool
  declareProperty("badChannelTool",m_badChannelTool,"Bad Channel Tool to provide list of missing FEBs");
  // activate creation of cells from missing Febs
  declareProperty("addDeadOTX",m_addDeadOTX,"Add dummy cells for missing FEBs");
  declareProperty("InitialCellPoolSize",m_initialDataPoolSize,"Initial size of the DataPool<LArCells>");
}



LArCellBuilderFromLArRawChannelTool::~LArCellBuilderFromLArRawChannelTool() { 
}


StatusCode LArCellBuilderFromLArRawChannelTool::initialize() {

  const IGeoModelSvc *geoModel=0;
  ATH_CHECK( service("GeoModelSvc", geoModel) );

  ATH_CHECK(m_rawChannelsKey.initialize());

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized()){
    return geoInit(dummyInt,dummyList);
  }
  else{
    ATH_CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel,
                                  &LArCellBuilderFromLArRawChannelTool::geoInit,this) );
  }
  return StatusCode::SUCCESS;
}

StatusCode
LArCellBuilderFromLArRawChannelTool::geoInit(IOVSVC_CALLBACK_ARGS) {  
  ATH_MSG_DEBUG("Accesssing CaloDetDescrManager");
  m_caloDDM = CaloDetDescrManager::instance() ;

  ATH_MSG_DEBUG("Accesssing CaloCellID");
  m_caloCID = m_caloDDM->getCaloCell_ID();

  ATH_CHECK( m_cablingSvc.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );

  if (!m_badChannelTool.empty()) {
    ATH_CHECK( m_badChannelTool.retrieve() );
  }
  else {
    if (m_addDeadOTX) {
      ATH_MSG_ERROR( "Configuration problem: 'addDeadOTX' set, but no bad-channel tool given."  );
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( detStore()->regFcn(&LArCablingService::iovCallBack,&(*m_cablingSvc),
                                &LArCellBuilderFromLArRawChannelTool::cablingSvc_CB,
                                this,true) );

  if (m_addDeadOTX) {
    ATH_CHECK( detStore()->regFcn(&ILArBadChanTool::updateBadFebsFromDB,&(*m_badChannelTool),
                                  &LArCellBuilderFromLArRawChannelTool::missingFEB_CB,
                                  this,true) );
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
StatusCode LArCellBuilderFromLArRawChannelTool::process(CaloCellContainer * theCellContainer) {
 
  if (theCellContainer->ownPolicy() == SG::OWN_ELEMENTS) {
    ATH_MSG_ERROR( "Called with a CaloCellContainer with wrong ownership policy! Need a VIEW container!"  );
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<LArRawChannelContainer> rawColl(m_rawChannelsKey);
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
   
   return fillCompleteCellCont(rawColl.ptr(),theCellContainer);
}

// ========================================================================================== //
StatusCode LArCellBuilderFromLArRawChannelTool::fillCompleteCellCont(const LArRawChannelContainer * rawColl, 
								     CaloCellContainer * theCellContainer) {
  unsigned nCellsAdded=0;
  std::bitset<CaloCell_ID::NSUBCALO> includedSubcalos;
  // resize calo cell container to correct size
  if (theCellContainer->size()) {
    ATH_MSG_ERROR( "filleCellCont: container should be empty! Clear now."   );
    theCellContainer->clear();
  }

  DataPool<LArCell> pool;
  
  theCellContainer->resize(m_nTotalCells);
  LArRawChannelContainer::const_iterator itrRawChannel=rawColl->begin();
  LArRawChannelContainer::const_iterator lastRawChannel=rawColl->end();
  for ( ; itrRawChannel!=lastRawChannel; ++itrRawChannel) {
    IdentifierHash hashid;
    LArCell* larcell=getCell(*itrRawChannel,hashid,pool);
    if (larcell) {
      if ((*theCellContainer)[hashid]) {
	ATH_MSG_WARNING( "Channel added twice! Data corruption? hash=" << hashid  
                         << " online ID=0x" << std::hex << itrRawChannel->channelID().get_compact() 
                         << "  " << m_onlineID->channel_name(itrRawChannel->channelID())  );
      }
      else {
	(*theCellContainer)[hashid]=larcell;
	++nCellsAdded;
	includedSubcalos.set(m_caloCID->sub_calo(hashid));
      }
    }//end if larcell
  }//end loop over LArRawChannelContainer
  //Now add in dummy cells
  unsigned nMissingButPresent=0;
  itrRawChannel=m_deadFEBChannels.begin();
  lastRawChannel=m_deadFEBChannels.end();
  for ( ; itrRawChannel!=lastRawChannel; ++itrRawChannel) {
    IdentifierHash hashid;
    LArCell* larcell=getCell(*itrRawChannel,hashid,pool);
    if (larcell) {
      if ((*theCellContainer)[hashid]) {
	++nMissingButPresent;
	ATH_MSG_DEBUG("The supposedly missing channel with online ID=0x" << std::hex << itrRawChannel->channelID().get_compact() 
		      << "  " << m_onlineID->channel_name(itrRawChannel->channelID()) 
		      << " is actually present in the LArRawChannel container");
      }
      else {
	(*theCellContainer)[hashid]=larcell;
	++nCellsAdded;
	includedSubcalos.set(m_caloCID->sub_calo(hashid));
      }	
    }//end if larcell
  }//End loop over dummy cells
  
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


// ========================================================================================== //
LArCell*
LArCellBuilderFromLArRawChannelTool::getCell(const LArRawChannel& theRawChannel,
                                             IdentifierHash& newHash,
                                             DataPool<LArCell>& pool)
{
  const HWIdentifier hWId=theRawChannel.channelID();
  const CaloDetDescrElement * theDDE=this->caloDDE(hWId);		  
  if (theDDE) {
    newHash=theDDE->calo_hash() ;
    const double energy = theRawChannel.energy();
    const double time = theRawChannel.time()*1e-3;
    const uint16_t quality = theRawChannel.quality();
    const uint16_t provenance = (theRawChannel.provenance() & 0x3FFF);   // to be sure not to set by error "dead" bit
    const CaloGain::CaloGain gain = theRawChannel.gain();

    LArCell *pCell   = pool.nextElementPtr();
    *pCell = LArCell (theDDE,
                      theDDE->identify(),
                      energy,
                      time,
                      quality,
                      provenance,
                      gain);
    return pCell;
  } //end if got DDE
  return NULL;
}


// ========================================================================================== //
StatusCode LArCellBuilderFromLArRawChannelTool::caloDDEsInitialize() 
{
  m_hwHashToCaloDDEmap.assign(m_onlineID->channelHashMax(),NULL);
  CaloDetDescrManager::calo_element_const_iterator itrCaloDDE=m_caloDDM->element_begin();
  CaloDetDescrManager::calo_element_const_iterator endCaloDDE=m_caloDDM->element_end();
  
  for (;itrCaloDDE!=endCaloDDE;++itrCaloDDE){
    const CaloDetDescrElement * caloDDE = *itrCaloDDE ;
    if (caloDDE==0) {
      continue ;
    }
	
    const CaloDetDescriptor * caloDDD = caloDDE->descriptor();
    if (caloDDD==0) {
      ATH_MSG_ERROR( " caloDDE does not have a descriptor "  );
      continue ;

    }
    if (caloDDD->is_tile()) continue;

    const HWIdentifier hwID=m_cablingSvc->createSignalChannelID((*itrCaloDDE)->identify());
    const IdentifierHash hwIDhash=m_onlineID->channel_Hash(hwID);
    m_hwHashToCaloDDEmap[hwIDhash]=*itrCaloDDE ;    
   }
  return StatusCode::SUCCESS;  
}


StatusCode LArCellBuilderFromLArRawChannelTool::cablingSvc_CB(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_INFO( "Callback on LArCablingSvc. Now initialize CaloDDE"  );
  return caloDDEsInitialize() ;

}


StatusCode LArCellBuilderFromLArRawChannelTool::missingFEB_CB(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_INFO( "Callback from ILArBadChanTool. "  );

  m_deadFEBChannels.clear(); 
  const uint16_t provenance = 0x0A00;   // 0x0800 (dead) + 0x0200  (to indicate missing readout)

  typedef  std::vector< std::pair<HWIdentifier,LArBadFeb> > BADFEBSTATUS;
  const BADFEBSTATUS& badFebList= m_badChannelTool->fullBadFebsState();
  //m_deadFEBChannels.reserve(128*badFebList.size());
  //Loop over all FEBs known to be problematic
  BADFEBSTATUS::const_iterator it=badFebList.begin();
  BADFEBSTATUS::const_iterator it_e=badFebList.end();
  m_nDeadFEBs=0;
  for(;it!=it_e;++it) {
    const LArBadFeb& febstatus=it->second;
    if (febstatus.deadReadout()  ||  febstatus.deadAll() || febstatus.deactivatedInOKS() ) {
      ++m_nDeadFEBs;
      const HWIdentifier& febId=it->first;
      //Loop over channels belonging to this FEB
      const int chansPerFeb=m_onlineID->channelInSlotMax(febId);
      for (int ch=0; ch<chansPerFeb; ++ch) {
	const HWIdentifier hwid = m_onlineID->channel_Id(febId, ch);
	if ( m_cablingSvc->isOnlineConnected(hwid))
	  m_deadFEBChannels.push_back(LArRawChannel(hwid,0,0,0,provenance,CaloGain::LARHIGHGAIN));
      }//end loop over feb-channels
    }//end if febStatus
  }//end loop over bad febs
  ATH_MSG_INFO( "Set up " << m_deadFEBChannels.size() << " dummy LArRawChannels for " << m_nDeadFEBs << " dead FEBs"  );
  return StatusCode::SUCCESS;
}

