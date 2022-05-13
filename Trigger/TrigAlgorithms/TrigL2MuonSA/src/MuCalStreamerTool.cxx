/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuCalStreamerTool.h"
#include "MuCalCircClient.h"

#include <cmath>

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "Identifier/IdentifierHash.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuCalStreamerTool::MuCalStreamerTool(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AthAlgTool(type,name,parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::initialize ()
{
   // locate the region selector
   ATH_CHECK( m_regSel_MDT.retrieve() );
   ATH_CHECK( m_regSel_TGC.retrieve() );


   m_localBuffer.clear();

   m_localBufferSize = 0;

   ATH_CHECK(m_tgcRdoKey.initialize());
   ATH_CHECK(m_readKey.initialize());
   ATH_CHECK(m_eventInfoKey.initialize());
   ATH_CHECK(m_rpcPadKey.initialize());

   return StatusCode::SUCCESS;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


StatusCode TrigL2MuonSA::MuCalStreamerTool::openStream(int calBufferSize)
{

  std::string name = m_calBufferName;
  name += "_"+m_algInstanceName;
  
  if ( m_circ == nullptr ) { 
      try
	{
	  m_circ = new TrigL2MuonSA::MuCalCircClient (0, name, calBufferSize);
	}

      catch (TrigL2MuonSA::Circexception &e)
	{
	  ATH_MSG_WARNING("Could not open muon calibration buffer: name="
			  << name << " buffer size=" << calBufferSize);
	}

      ATH_MSG_INFO("Opening muon calibration stream. Buffer name: " 
		   << name << " buffer size: " << calBufferSize);
  }

  return StatusCode::SUCCESS;
}




// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::closeStream()
{

  std::string name = m_calBufferName+"_"+m_algInstanceName;
  ATH_MSG_DEBUG("I'm going to close the stream "<<name);

  if (m_circ)
    {
      delete m_circ;
      m_circ = nullptr;
    }

  return StatusCode::SUCCESS;

}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::MuCalStreamerTool::isStreamOpen() {return m_circ!=nullptr;}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

 StatusCode TrigL2MuonSA::MuCalStreamerTool::createRoiFragment(const xAOD::MuonRoI* inputroi,
 							      TrigL2MuonSA::TrackPattern& track,
 							      TrigL2MuonSA::MdtHits& mdtHits,
 							      TrigL2MuonSA::RpcHits& rpcHits,
 							      TrigL2MuonSA::TgcHits& tgcHits,
							       // int calBufferSize,
							       bool doDataScouting,
							       bool &updateTriggerElement, const EventContext&                        ctx) const
 {

   // create the fragment
   // ( dummy input for now )

   ATH_MSG_DEBUG("Data scouting is set to"<<doDataScouting);

   // skip the event if it's a noise burst
   unsigned int totalHits = mdtHits.size()+rpcHits.size()+tgcHits.size();
   if (  totalHits > 500 ) {
     ATH_MSG_DEBUG("Too many hits: skip the RoI");
     updateTriggerElement=false;
     return StatusCode::SUCCESS;
   }

   // init roi pointer
   if ( !inputroi ) {
     ATH_MSG_ERROR("Roi not initialized");
     return StatusCode::FAILURE;
   }

   // pointer to the muon roi
   const xAOD::MuonRoI* roi {inputroi};
    

   // retrieve the event and trigger info
   SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
   uint32_t runId  = ctx.eventID().run_number();
   uint32_t lvl1Id = eventInfo->extendedLevel1ID();


   // get track parameters
   float eta = (float) track.etaVtx;
   float phi = (float) track.phiVtx;
   float pt  = (float) track.pt;

   uint32_t mrods[4] = {0, 0, 0, 0};
   // prepare the header information
   std::vector<uint32_t> robIdList_MDT;
   double etaMin = roi->eta()-0.05;
   double etaMax = roi->eta()+0.05;
   double phi_roi = roi->phi();
   double phiMin = roi->phi()-0.05;
   double phiMax = roi->phi()+0.05;
   if( phi_roi < 0 ) phi_roi += 2*M_PI;
   if( phiMin < 0 ) phiMin += 2*M_PI;
   if( phiMax < 0 ) phiMax += 2*M_PI;

   TrigRoiDescriptor roiDescr( roi->eta(), etaMin, etaMax, phi_roi, phiMin, phiMax );
  
   const IRoiDescriptor* iroi = (IRoiDescriptor*) &roiDescr;
   m_regSel_MDT->ROBIDList(*iroi,robIdList_MDT);

   // dump the list of robs for debugging 
   ATH_MSG_DEBUG("Size of the MDT rob list: " << robIdList_MDT.size());
   int isize = robIdList_MDT.size()<5 ? robIdList_MDT.size() : 4;
   for (int ii = 0 ; ii<isize ; ++ii ) {
     ATH_MSG_DEBUG("robId: 0x" << std::hex << robIdList_MDT.at(ii) << std::dec);
     mrods[ii] = robIdList_MDT.at(ii);
   }

   // get the list of TGC robs
   std::vector<uint32_t> robIdList_TGC;
   m_regSel_TGC->ROBIDList(*iroi,robIdList_TGC);
   ATH_MSG_DEBUG("Size of the TGC rob list: " << robIdList_TGC.size());


   LVL2_MUON_CALIBRATION::CalibEvent  event(1,runId,lvl1Id,1,1,mrods,name().c_str(),eta,phi,pt);
   LVL2_MUON_CALIBRATION::MdtCalibFragment mdtFragment;

   // create the MDT fragment
   if ( mdtHits.size()>0 ) {
     ATH_CHECK( createMdtFragment(mdtHits,mdtFragment,phi) );
  
     // add the mdt fragment to the event
     event << mdtFragment;
   }
   else{
     // do not write the calib stream
     return StatusCode::SUCCESS;
   }

   // create the RPC fragment 
   if ( rpcHits.size() > 0 ) {    
     LVL2_MUON_CALIBRATION::RpcCalibFragment rpcFragment;
  
     if ( createRpcFragment(roi, rpcFragment, ctx) != StatusCode::SUCCESS ) {
       ATH_MSG_WARNING("Could not create the Rpc fragment of the calibration stream");
     }
     else {
       ATH_MSG_DEBUG("Adding the RPC fragment to the calibration stream");
       event << rpcFragment;
     }
   }

   if ( tgcHits.size() > 0 ) {    
     LVL2_MUON_CALIBRATION::TgcCalibFragment tgcFragment;

     if ( createTgcFragment(robIdList_TGC,tgcFragment) != StatusCode::SUCCESS ) {
       ATH_MSG_ERROR("Could not create the Tgc fragment of the calibration stream");
     }
     else {
       ATH_MSG_DEBUG("Adding the TGC fragment to the calibration stream");
       event << tgcFragment;
     }
   }



   ATH_MSG_DEBUG("Dumping the event stream");
   ATH_MSG_DEBUG(event);
  if (m_circ)
  {
    m_circ->dumpToCirc (event);
  }

   return StatusCode::SUCCESS;
 }

 //
 // prepare the Mdt fragment of the stream
 //
 StatusCode TrigL2MuonSA::MuCalStreamerTool::createMdtFragment(TrigL2MuonSA::MdtHits& mdtHits,   
 							      LVL2_MUON_CALIBRATION::MdtCalibFragment& mdtFragment, 
 							      float trackPhi) const
 {

   TrigL2MuonSA::MdtHits::const_iterator it = mdtHits.begin();
   for ( ; it != mdtHits.end() ; ++it ) {

     int stationName = (*it).name;
     int stationEta  = (*it).StationEta;
     int stationPhi  = (*it).StationPhi;
     int multilayer  = (*it).Multilayer;
     int layer       = (*it).TubeLayer;
     int tube        = (*it).Tube;

     uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(stationName,
 						       stationEta,stationPhi,multilayer,layer,tube);

     uint16_t  leadingCoarseTime  = (*it).LeadingCoarseTime;
     uint16_t  leadingFineTime    = (*it).LeadingFineTime;
     uint16_t  trailingCoarseTime = (*it).TrailingCoarseTime;
     uint16_t  trailingFineTime   = (*it).TrailingFineTime;
     uint16_t  adc                = (*it).Adc;

     LVL2_MUON_CALIBRATION::MdtCalibData mdt(ID,leadingCoarseTime,leadingFineTime,
 					    trailingCoarseTime,trailingFineTime,adc
 					    ,trackPhi);

     ATH_MSG_DEBUG("Dumping MDT Hit");
     ATH_MSG_DEBUG(mdt);

     mdtFragment << mdt;
   }

   return StatusCode::SUCCESS;
 }

//////////////////////////////////////////////////////////////////////////////////////////
//
// prepare the Rpc fragment of the stream
//
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2MuonSA::MuCalStreamerTool::createRpcFragment(const xAOD::MuonRoI* roi,
							      LVL2_MUON_CALIBRATION::RpcCalibFragment& rpcFragment,const EventContext&                        ctx) const
{

  // exit if it's not in the barrel
  //if ( roi->sysID() != 0 ) return StatusCode::SUCCESS;
  if ( roi->getSource() != 0 ) return StatusCode::SUCCESS;
 
  // retrieve the padId from the RecMuonRoi

  //  decode  roIWord
  unsigned int side = roi->getHemisphere()==xAOD::MuonRoI::Hemisphere::Positive ? 1 : 0;
  unsigned int sector = roi->getSectorID();
  unsigned int roiNumber =  roi->getRoI();

  // retrieve the pad container
  SG::ReadHandle<RpcPadContainer> rh_rpcPad{m_rpcPadKey, ctx};
  const RpcPadContainer* rpcPadContainer=rh_rpcPad.cptr();

  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey,ctx};
  const RpcCablingCondData* readCdo{*readHandle};
  unsigned int padIdHash;
  if (readCdo->give_PAD_address( side, sector, roiNumber, padIdHash)) {

    auto itPad = rpcPadContainer->indexFindPtr(padIdHash);  
    if( itPad==nullptr ) {        
      ATH_MSG_WARNING("Failed to retrieve PAD hash Id " << padIdHash);  
      return StatusCode::FAILURE;                         
    }
    const RpcPad* rpcPad = itPad;

      uint16_t sector = rpcPad->sector();
      uint16_t sysId  = (sector<32)? 0x66 : 0x65;
      uint16_t secId  = sector%32;
      uint16_t padId  = rpcPad->onlineId();
      uint16_t status = rpcPad->status();
      uint16_t error  = rpcPad->errorCode();
      
      LVL2_MUON_CALIBRATION::RpcCalibFragment frag(sysId,secId,padId,status,error);
      rpcFragment = frag;
      RpcPad::const_iterator it3 = rpcPad->begin();
      for (; it3!=rpcPad->end() ; ++it3) {
	const RpcCoinMatrix * cma = (*it3);
	
	uint16_t cmaId  = cma->onlineId();
	uint16_t fel1Id = cma->fel1Id();
	uint16_t febcId = cma->febcId();
	uint16_t crc    = cma->crc();
	
	LVL2_MUON_CALIBRATION::RpcCalibData matrix(cmaId,fel1Id,febcId,crc);
	
	//for each cma for over fired channels
	RpcCoinMatrix::const_iterator it5 = (*it3)->begin();
	for (; it5 != (*it3)->end() ; ++it5) {
	  const RpcFiredChannel * fChannel = (*it5);
	  
	  uint16_t bcid    = fChannel->bcid();
	  uint16_t time    = fChannel->time();
	  uint16_t ijk     = fChannel->ijk();
	  uint16_t channel = fChannel->channel();
	  uint16_t ovl     = fChannel->ovl();
	  uint16_t thr     = fChannel->thr();
	  
	  if(ijk!=7) matrix.getHit(bcid,time,ijk,channel);
	  else       matrix.getHit(bcid,time,ijk,ovl,thr);
	  
	} // loop on the matrix hits
	
	rpcFragment << matrix;
      } // loop on the pad matrices
    
  }
  else {
    ATH_MSG_WARNING("Can't get the pad address from the RpcCablingCondData");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
// prepare the TGC fragment of the stream
//
////////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2MuonSA::MuCalStreamerTool::createTgcFragment(std::vector<uint32_t>& robIdList_TGC,
							      LVL2_MUON_CALIBRATION::TgcCalibFragment& tgcFragment) const
{


  if ( robIdList_TGC.size()<1 ) {
    ATH_MSG_DEBUG("No TGC Rob found");
    return StatusCode::SUCCESS;
  }

  // system Id = 1 should correspond to the endcap
  uint16_t systemId = 1;
  // this is the subdetector Id
  uint16_t subSystemId = ( (robIdList_TGC.at(0) & 0xff0000) >> 16 );
  uint16_t rdoId = ( robIdList_TGC.at(0) & 0xff );

  uint16_t roiNumber = 0;

  tgcFragment = LVL2_MUON_CALIBRATION::TgcCalibFragment(systemId,subSystemId,rdoId,roiNumber);

  // retrieve the tgcrdo container
  SG::ReadHandle<TgcRdoContainer> rdoRH(m_tgcRdoKey);
  if (!rdoRH.isValid()) {
    ATH_MSG_ERROR( "No TGC RDO container found!"  );
    return StatusCode::FAILURE;
  }
  const TgcRdoContainer* tgcRdoContainer = rdoRH.cptr();

  // now get the list of ROB Ids and from the the subdetector ID ( that corresponds to the
  // subsystem ID
  

  LVL2_MUON_CALIBRATION::TgcCalibData tgc_payload;
  TgcRdoContainer::const_iterator it = tgcRdoContainer->begin();
  
  for ( ; it != tgcRdoContainer->end() ; ++it )  {

    ATH_MSG_DEBUG("Adding the hits from a new TGC chamber");

    // loop on the element of the TGC RDO
    ATH_MSG_DEBUG("Number of hits: " << (*it)->size());

    TgcRdo::const_iterator itRaw = (*it)->begin();
    for ( ; itRaw != (*it)->end() ; ++itRaw )  {

      const TgcRawData* raw = (*itRaw);
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTHIT roh;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRIPLETSTRIP tps;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRACKLET rot;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_HIPT hpt;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_SL sl;


      switch (raw->type()) {
      case TgcRawData::TYPE_HIT:
	{
	  roh.channel = raw->channel()-40;
	  roh.sbId = raw->slbId();
	  roh.sbType = raw->slbType();
	  roh.adj = raw->isAdjacent();
	  roh.ldbId = raw->sswId();
	  roh.bcBitmap = bcBitmap(raw->bcTag());
	  roh.tracklet = raw->tracklet();
	  roh.ok = 1;
	  
	}
	break;
      case TgcRawData::TYPE_TRACKLET:
	if (raw->slbType() == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
	  {
	    tps.phi = raw->position();
	    tps.seg = raw->segment();
	    tps.sbId = raw->slbId();
	    tps.ldbId = raw->sswId();
	    tps.bcBitmap = bcBitmap(raw->bcTag());
	    tps.slbType = TgcRawData::SLB_TYPE_TRIPLET_STRIP;
	    tps.ok = 1;
	  }
	else
	  {
	    rot.rphi = raw->position();
	    rot.subm = raw->subMatrix();
	    rot.seg = raw->segment();
	    rot.delta = raw->delta();
	    rot.sbId = raw->slbId();
	    rot.ldbId = raw->sswId();
	    rot.bcBitmap = bcBitmap(raw->bcTag());
	    rot.slbType = raw->slbType();
	    if (raw->slbType() == TgcRawData::SLB_TYPE_INNER_STRIP)
	      rot.slbType = 4;
	    rot.ok = 1;
	  }
	break;
      case TgcRawData::TYPE_HIPT:
	{
	  hpt.delta = raw->delta();
	  hpt.hitId = raw->hitId();
	  hpt.hipt = raw->isHipt();
	  hpt.cand = raw->index();
	  hpt.chip = raw->chip();
	  hpt.sector = raw->sector();
	  hpt.fwd = raw->isForward();
	  hpt.strip = raw->isStrip();
	  hpt.bcBitmap = bcBitmap(raw->bcTag());
	}
	break;
      case TgcRawData::TYPE_SL:
	{
	  sl.roi = raw->roi();
	  sl.overlap = raw->isOverlap();
	  sl.threshold = raw->threshold();
	  sl.sign = raw->isMuplus();
	  sl.cand = raw->index();
	  sl.sector = raw->sector();
	  sl.fwd = raw->isForward();
	  sl.bcBitmap = bcBitmap(raw->bcTag());
	  sl.cand2plus = raw->cand3plus();
	}
	break;
      default:
	{
	  ATH_MSG_ERROR("Invalid TgcRawData type: " << raw->type());
	  return StatusCode::FAILURE;
	}
      }
      
      tgc_payload.getHit(roh,tps,rot,hpt,sl);
      
    }
  }
 
  tgcFragment << tgc_payload;
  
  return StatusCode::SUCCESS;
}
