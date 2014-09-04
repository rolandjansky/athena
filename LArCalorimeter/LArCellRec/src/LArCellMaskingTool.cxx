/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellMaskingTool
PACKAGE:  offline/Calorimeter/CaloRec

********************************************************************/

#include "LArCellRec/LArCellMaskingTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloEvent/CaloCellContainer.h"

#include "LArTools/LArCablingService.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellMaskingTool::LArCellMaskingTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AlgTool(type, name, parent)
{
  declareInterface<ICaloCellMakerTool>(this); 
  //List of strings to determine detector parts to be masked.
  //Syntax: barrel_endcap pos_neg Feedthrough slot channel (integers separated by white space)
  //Feedthrough, slot, and channel can be left out. In this case all channels belonging to this 
  //Subdetector, Feedthrough or FEB will be masked.
  declareProperty ("RejectLArChannels",m_rejLArChannels);
  m_log=NULL;
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellMaskingTool::initialize()
{
  m_log = new MsgStream(msgSvc(),name());
 
  // Cache pointer to storegate:

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
  {
    (*m_log) << MSG::ERROR
	<< "Unable to retrieve pointer to StoreGate Service"
	<< endreq;
    return sc;
  }

  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     (*m_log) << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
   }

  sc = detStore->retrieve(m_offlineID);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR
	<< "Unable to retrieve CaloCell_ID helper from DetectorStore" << endreq;
    return sc;
  }

  sc = detStore->retrieve(m_onlineID);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR
	<< "Unable to retrieve LArOnline_ID helper from DetectorStore" << endreq;
    return sc;
  }

  IToolSvc*     p_toolSvc;
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL
        << " Tool Service not found "
        << endreq;
    return StatusCode::FAILURE;
  }
  sc = p_toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
  if(sc.isFailure()){
    (*m_log) << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
    return StatusCode::FAILURE;
  }


 // Get hash ranges
  m_offlinehashMax=m_offlineID->calo_cell_hash_max();
  (*m_log) << MSG::DEBUG << "CaloCell Hash Max: " << m_offlinehashMax << endreq;
  
//   m_onlinehashMax=m_onlineID->hash_max();
//   (*m_log) << MSG::DEBUG << "CaloCell Hash Max: " << m_offlinehashMax << endreq;
  
  //Fill the bit map
  m_includedCellsMap.set(); // By default include all cells
  
  sc=fillIncludedCellsMap();

  (*m_log) << MSG::INFO << " Will exclude " << m_includedCellsMap.size() - m_includedCellsMap.count() << " cells from CaloCellContainer" << endreq;
  

  return sc;

}

StatusCode LArCellMaskingTool::fillIncludedCellsMap() {

  std::vector<std::string>::const_iterator it=m_rejLArChannels.begin();
  std::vector<std::string>::const_iterator it_e= m_rejLArChannels.end();
  for (;it!=it_e;it++) {
    std::stringstream is;
    is << (*it);
    bool haveFT=false, haveSlot=false, haveChannel=false;
    int bec=0, pn=0, FT=0, slot=1,channel=0;
    //Want at least subdetector (=pn & bec)
    is >> bec >> pn;
    if (is.bad()) {
      (*m_log) << MSG::ERROR << "jO problem: Malformed string [" << (*it) << "]" << endreq;
      return StatusCode::FAILURE;
    }

    int FTmax;
    if (bec==0) 
      FTmax=32; //Barrel
    else
      FTmax=24; //Endcap



    if (!is.eof()) {//have FT
      is >> FT; 
      haveFT=true;
      //check good?
    }
    if (!is.eof()) {//have slot
      is >> slot; 
      haveSlot=true;
    }
    if (!is.eof()) {//have channel
      is >> channel; 
      haveChannel=true;
    }
    
    (*m_log) << MSG::DEBUG << "Will exclude: bec="<< bec << " pn=" << pn;
    if (haveFT) (*m_log) << " FT=" << FT;
    if (haveSlot) (*m_log) << " slot=" << slot; 
    if (haveChannel) (*m_log) << " channel=" << channel;
    (*m_log) << endreq;

    unsigned nOnlExceptions=0;
    unsigned nOfflExceptions=0;
    unsigned nChannels=0;
    unsigned nDisconnected=0;
    HWIdentifier chanId;
    do { //loop over FTs (only once if FT is set
      //Number of channels for this FT
      int slotMax=15;
      do {
	int channelMax=128;
	do { //loop over channels in slot
	  nChannels++;
	  try {
	    chanId=m_onlineID->channel_Id(bec,pn,FT,slot,channel);
	    if (m_larCablingSvc->isOnlineConnected(chanId)) {
	      const Identifier cellId=m_larCablingSvc->cnvToIdentifier(chanId);
	      const IdentifierHash cellhash=m_offlineID->calo_cell_hash(cellId);
	      m_includedCellsMap.reset(cellhash);
	      //std::cout << "Block channel: bec="<< bec << " pn=" << pn 
	      //		<< " FT=" << FT <<":" << haveFT << " slot=" << slot << ":" << haveSlot << " channel=" << channel <<":" << haveChannel<< std::endl;
	    }
	    else
	      nDisconnected++;
	  }
	  catch (LArOnlID_Exception) {
	    nOnlExceptions++;
	  }
	  catch(LArID_Exception) {
	    nOfflExceptions++;
	  }
	  if (!haveChannel) channel++;
	}while (!haveChannel && channel<channelMax); 
	if (!haveChannel) channel=0;
	if (!haveSlot) slot++;
      }while (!haveSlot && slot<slotMax);
      if (!haveSlot) slot=0;
      if (!haveFT) FT++;
    } while (!haveFT && FT<FTmax); 
    (*m_log) << MSG::DEBUG << "Channels selected for exclusion: "<< nChannels << ". Disconnected: " << nDisconnected << endreq;
  }// end loop over strings
  return StatusCode::SUCCESS;
}



StatusCode LArCellMaskingTool::process(CaloCellContainer * theCont )
{
  //Build bitmap to keep track which cells have been added to reducedCellContainer;
  unsigned cnt=0;
  CaloCellContainer::iterator it=theCont->begin();
  while(it!=theCont->end()) {
    const IdentifierHash cellHash=(*it)->caloDDE()->calo_hash();
    if (!m_includedCellsMap.test(cellHash)) {
      //Possible optimization: check how many consecutive cells to be deleted and erase a range
      it=theCont->erase(it);
      cnt++;
    }
    else
      ++it;
  }
  (*m_log) << MSG::DEBUG << "Removed " << cnt << " Cells from container" << endreq;
  return StatusCode::SUCCESS ;
}


StatusCode LArCellMaskingTool::finalize() {
  if (m_log)
    delete m_log;
  return StatusCode::SUCCESS;
}





