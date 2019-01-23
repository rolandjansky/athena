/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellMaskingTool
PACKAGE:  offline/Calorimeter/CaloRec

********************************************************************/

#include "LArCellMaskingTool.h"
#include "CaloEvent/CaloCellContainer.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellMaskingTool::LArCellMaskingTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_onlineID(nullptr),
    m_offlineID(nullptr)
{
  m_mapInitialized = false;

  declareInterface<ICaloCellMakerTool>(this); 
  //List of strings to determine detector parts to be masked.
  //Syntax: barrel_endcap pos_neg Feedthrough slot channel (integers separated by white space)
  //Feedthrough, slot, and channel can be left out. In this case all channels belonging to this 
  //Subdetector, Feedthrough or FEB will be masked.
  declareProperty ("RejectLArChannels",m_rejLArChannels);
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellMaskingTool::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_offlineID) );
  ATH_CHECK( detStore()->retrieve(m_onlineID) );
  ATH_CHECK( m_cablingKey.initialize());
  
 // Get hash ranges
  m_offlinehashMax=m_offlineID->calo_cell_hash_max();
  ATH_MSG_DEBUG ("CaloCell Hash Max: " << m_offlinehashMax);
  
//   m_onlinehashMax=m_onlineID->hash_max();
//   (*m_log) << MSG::DEBUG << "CaloCell Hash Max: " << m_offlinehashMax << endmsg;
  
  //Fill the bit map
  m_includedCellsMap.set(); // By default include all cells
  
  ATH_MSG_INFO (" Will exclude " << m_includedCellsMap.size() - m_includedCellsMap.count() << " cells from CaloCellContainer");
  

  return StatusCode::SUCCESS;

}

StatusCode LArCellMaskingTool::fillIncludedCellsMap(const LArOnOffIdMapping* cabling) const
{

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
      ATH_MSG_ERROR ("jO problem: Malformed string [" << (*it) << "]");
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
    
    msg() << MSG::DEBUG << "Will exclude: bec="<< bec << " pn=" << pn;
    if (haveFT) msg() << " FT=" << FT;
    if (haveSlot) msg() << " slot=" << slot; 
    if (haveChannel) msg() << " channel=" << channel;
    msg() << endmsg;

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
	    if (cabling->isOnlineConnected(chanId)) {
	      const Identifier cellId=cabling->cnvToIdentifier(chanId);
	      const IdentifierHash cellhash=m_offlineID->calo_cell_hash(cellId);
	      m_includedCellsMap.reset(cellhash);
	      //std::cout << "Block channel: bec="<< bec << " pn=" << pn 
	      //		<< " FT=" << FT <<":" << haveFT << " slot=" << slot << ":" << haveSlot << " channel=" << channel <<":" << haveChannel<< std::endl;
	    }
	    else
	      nDisconnected++;
	  }
	  catch (const LArOnlID_Exception&) {
	    nOnlExceptions++;
	  }
	  catch(const LArID_Exception&) {
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
    ATH_MSG_DEBUG ("Channels selected for exclusion: "<< nChannels << ". Disconnected: " << nDisconnected);
  }// end loop over strings
  return StatusCode::SUCCESS;
}



StatusCode LArCellMaskingTool::process(CaloCellContainer * theCont )
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  if (! m_mapInitialized) {
    // FIXME: Can we do this in start()?
    std::lock_guard<std::mutex> lock (m_mutex);
    if (!m_mapInitialized) {
      SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl (m_cablingKey, ctx);
      const LArOnOffIdMapping* cabling=*cablingHdl;
      ATH_CHECK(fillIncludedCellsMap(cabling));
      m_mapInitialized=true;
    }
  }

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
  ATH_MSG_DEBUG ("Removed " << cnt << " Cells from container");
  return StatusCode::SUCCESS ;
}


StatusCode LArCellMaskingTool::finalize() {
  return StatusCode::SUCCESS;
}





