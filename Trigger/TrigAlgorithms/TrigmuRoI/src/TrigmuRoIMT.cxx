/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuRoIMT.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "CxxUtils/phihelper.h"

// ================================================================================
// ================================================================================

TrigmuRoIMT::TrigmuRoIMT(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_trigMuonRoITool( "TrigMuonRoITool" ),
    m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc","")
{   
}


// ================================================================================
// ================================================================================

StatusCode TrigmuRoIMT::initialize()
{

  ATH_MSG_DEBUG("MinValueForOutOfTimeBC: "<<m_minValueForOutOfTimeBC);
  ATH_MSG_DEBUG("MaxValueForOutOfTimeBC: "<<m_maxValueForOutOfTimeBC);

  // Retrieve the tools/services
  ATH_CHECK(m_trigMuonRoITool.retrieve());
  ATH_CHECK(m_recRPCRoiSvc.retrieve());
  ATH_CHECK(m_recTGCRoiSvc.retrieve());
  ATH_CHECK(m_roisWriteHandleKey.initialize());

  if (!m_monTool.empty()) {
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  } else {
    ATH_MSG_INFO("No monTool configured => NO MONITORING");
  }

  return StatusCode::SUCCESS;
}

// ================================================================================
// ================================================================================

unsigned int TrigmuRoIMT::getBitMaskValue( const unsigned int uintValue, const unsigned int mask ) {
  unsigned int result;
  unsigned int maskcopy;
  maskcopy = mask;
  result = uintValue & mask;
  if ( mask != 0 ) {
    while ( ( maskcopy & 0x00000001 ) == 0 ) {
      maskcopy = maskcopy >> 1;
      result = result >> 1;
    }
  }
  return result;
}

// ================================================================================
// ================================================================================

StatusCode TrigmuRoIMT::execute()
{

  std::vector<int> outOfTimeRpc, outOfTimeTgc;
  std::vector<float> etaRoI, phiRoI;
  auto rpcOutOfTime = Monitored::Collection("RpcOutOfTime", outOfTimeRpc);
  auto tgcOutOfTime = Monitored::Collection("TgcOutOfTime", outOfTimeTgc);
  auto etaOutOfTimeRoI = Monitored::Collection("EtaOutOfTime", etaRoI);
  auto phiOutOfTimeRoI = Monitored::Collection("PhiOutOfTime", phiRoI);

  auto mon = Monitored::Group(m_monTool, rpcOutOfTime, tgcOutOfTime, etaOutOfTimeRoI, phiOutOfTimeRoI);

  int roi_id = 0;

  //--------------------------------------------------------------------------
  // Gather the Muon RoIs out of time by the
  //--------------------------------------------------------------------------
      
  SG::WriteHandle<TrigRoiDescriptorCollection> wh_roiCollection(m_roisWriteHandleKey);
  ATH_CHECK(wh_roiCollection.record(std::make_unique<TrigRoiDescriptorCollection>()));
  auto roiColl = wh_roiCollection.ptr();

  for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it = m_trigMuonRoITool->begin_OutOfTimeRoIs();
	it != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++it) {
    
    ATH_MSG_DEBUG(" Difference(RoI(BCID) - Event(BCID)) = " << (*it).second);
    ATH_MSG_DEBUG(" ------------------------------------- ");
    ATH_MSG_DEBUG("RoIB word               : 0x" << MSG::hex << ((*it).first).roIWord() << MSG::dec);
    ATH_MSG_DEBUG("Threshold               :  pt" << ((*it).first).pt());
    ATH_MSG_DEBUG("Sector ID               :  " << ((*it).first).getSectorID());
    ATH_MSG_DEBUG("Sector addr             :  0x" << MSG::hex << ((*it).first).getSectorAddress() << MSG::dec);
    ATH_MSG_DEBUG("Sector overflow         :  " << ((*it).first).getSectorOverflow());
    ATH_MSG_DEBUG("RoI overflow            :  " << ((*it).first).getRoiOverflow());
    ATH_MSG_DEBUG("RoI number              :  " << ((*it).first).getRoiNumber());
    ATH_MSG_DEBUG("IsHighestPt             :  " << ((*it).first).getCandidateIsHighestPt());
    ATH_MSG_DEBUG("=================================================");
            
    unsigned int temp_sysID = getBitMaskValue(((*it).first).getSectorAddress(), LVL1::SysIDMask );
    unsigned int sysID = 0;                // Barrel
    if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
    else if( temp_sysID & 0x1 ) sysID = 2; // Forward


    if ( sysID == 0 ) outOfTimeRpc.push_back((*it).second);
    else              outOfTimeTgc.push_back((*it).second);
      
    const LVL1::RecMuonRoiSvc* recMuonRoiSvc = 0;
    std::string region = "";
    if( sysID == 0 ) {
      recMuonRoiSvc = &(*m_recRPCRoiSvc);
      region = "Barrel region";
    } else if ( sysID == 1 ){
      recMuonRoiSvc = &(*m_recTGCRoiSvc);
      region = "Endcap region";
    } else {
      recMuonRoiSvc = &(*m_recTGCRoiSvc);
      region = "Forward region";
    }
	   
    recMuonRoiSvc->reconstruct( ((*it).first).roIWord() );
    // create new trigger element for this out of time RoI
    double eta = recMuonRoiSvc->eta();
    double phi = recMuonRoiSvc->phi();
    etaRoI.push_back(eta);
    phiRoI.push_back(phi);
     
    double etamin = eta - 0.2;
    double etamax = eta + 0.2;
    double phimin = CxxUtils::wrapToPi(phi - 0.2);
    double phimax = CxxUtils::wrapToPi(phi + 0.2);

    if ((*it).second >= m_minValueForOutOfTimeBC &&
	(*it).second <= m_maxValueForOutOfTimeBC    ) {

      // generic TrigRoiDescriptor
      auto roiDescriptor = new TrigRoiDescriptor( ((*it).first).roIWord(), 0, roi_id, eta, etamin, etamax, phi, phimin, phimax,0,-255,255);
      roiColl->push_back(roiDescriptor);

      ATH_MSG_DEBUG("Created RoI descriptor with id, eta, phi: "<<roi_id<<" "<<eta<<" "<<phi);
    }
    roi_id += 1;
  }
  
  return StatusCode::SUCCESS;
}

