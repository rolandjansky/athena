/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TriggerElement.h"
#include "TrigT1Result/RoIBResult.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigmuRoI/TrigmuRoI.h"

#include "CxxUtils/phihelper.h"

unsigned int getBitMaskValue( const unsigned int uintValue,
	                       const unsigned int mask ) {
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

TrigmuRoI::TrigmuRoI(const std::string& name, ISvcLocator* pSvcLocator)
   : HLT::AllTEAlgo(name, pSvcLocator),
     m_log(msgSvc(), name),
     m_executed(false),
     m_trigMuonRoITool( "TrigMuonRoITool" ),
     m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
     m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc","")
{
   declareProperty("DaqCTPROBid",           m_daqCTPROBid=0x770000);
   declareProperty("DaqMuCTPiROBid",        m_daqMuCTPiROBid=0x760000);
   
   // mode choice
   declareProperty("MinValueForOutOfTimeBC",  m_minValueForOutOfTimeBC=-9999);
   declareProperty("MaxValueForOutOfTimeBC",  m_maxValueForOutOfTimeBC= 9999);
   
   // monitoring
   declareMonitoredStdContainer("RpcOutOfTime", m_RpcOutOfTime, AutoClear);
   declareMonitoredStdContainer("TgcOutOfTime", m_TgcOutOfTime, AutoClear);
   declareMonitoredStdContainer("EtaOutOfTime", m_etaOutOfTime, AutoClear);
   declareMonitoredStdContainer("PhiOutOfTime", m_phiOutOfTime, AutoClear);
}


// ================================================================================
// ================================================================================

HLT::ErrorCode TrigmuRoI::hltInitialize()
{
  m_log.setLevel(msgLevel());

  m_log << MSG::INFO << "initialize()" << endmsg;

  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    m_log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name() << endmsg;
  }

   m_log << MSG::INFO << " ROB ID: DAQ CTP                            = " << m_daqCTPROBid
	 << std::setw(6) << " (=0x" << MSG::hex << m_daqCTPROBid.value() << MSG::dec << ")" << endmsg;
   m_log << MSG::INFO << " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
	 << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" << endmsg;

   m_log << MSG::INFO << m_minValueForOutOfTimeBC << endmsg;
   m_log << MSG::INFO << m_maxValueForOutOfTimeBC << endmsg;


   // Retrieve the MuonRoITool
   StatusCode sc = m_trigMuonRoITool.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Could not retrieve " << m_trigMuonRoITool << endmsg;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved tool " << m_trigMuonRoITool << endmsg;
   }

   sc = m_recRPCRoiSvc.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Couldn't connect to " << m_recRPCRoiSvc << endmsg;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved Service " << m_recRPCRoiSvc << endmsg;
   }
  
   sc = m_recTGCRoiSvc.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Couldn't connect to " << m_recTGCRoiSvc << endmsg;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved Service " << m_recTGCRoiSvc << endmsg;
   }

   m_log << MSG::DEBUG << "initialization - done" << endmsg;

   return HLT::OK;
}

// ================================================================================
// ================================================================================

HLT::ErrorCode TrigmuRoI::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,
					      unsigned int type_out)
{

   // clear montioring variables
   m_RpcOutOfTime.clear();
   m_TgcOutOfTime.clear();


   // execute only once
   if ( m_executed ) return HLT::OK;
   m_executed = true;   
    
   beforeExecMonitors().ignore();
   
   if ( m_log.level() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing TrigmuRoI (" << name()
	    << ")" << endmsg;
   }

   int roi_id = 0;

   //--------------------------------------------------------------------------
   // Gather the Muon RoIs out of time by the
   //--------------------------------------------------------------------------
   auto ctx = getContext();
   auto roiVectors = m_trigMuonRoITool->decodeMuCTPi(ctx);
   if(!roiVectors){
     ATH_MSG_VERBOSE("No RoIs found");
     return HLT::OK;
   }

   //loop over out of time rois   
   for(auto it : *(roiVectors->outOfTimeRois)){   
       
       if(m_log.level() <= MSG::DEBUG) {
           ATH_MSG_DEBUG(" Difference(RoI(BCID) - Event(BCID)) = " << (it).second );
           ATH_MSG_DEBUG(" ------------------------------------- " );
           ATH_MSG_DEBUG("RoIB word               : 0x" << MSG::hex << ((it).first).roIWord() << MSG::dec );
           ATH_MSG_DEBUG("Threshold               :  pt" << ((it).first).pt() );
           ATH_MSG_DEBUG("Sector ID               :  " << ((it).first).getSectorID() );
           ATH_MSG_DEBUG("Sector addr             :  0x" << MSG::hex << ((it).first).getSectorAddress() << MSG::dec );
           ATH_MSG_DEBUG("Sector overflow         :  " << ((it).first).getSectorOverflow() );
           ATH_MSG_DEBUG("RoI overflow            :  " << ((it).first).getRoiOverflow() );
           ATH_MSG_DEBUG("RoI number              :  " << ((it).first).getRoiNumber() );
           ATH_MSG_DEBUG("IsHighestPt             :  " << ((it).first).getCandidateIsHighestPt() );
           ATH_MSG_DEBUG("=================================================" );
       }
       
       unsigned int temp_sysID =
           getBitMaskValue(((it).first).getSectorAddress(), LVL1::SysIDMask );
       unsigned int sysID = 0;                // Barrel
       if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
       else if( temp_sysID & 0x1 ) sysID = 2; // Forward


       if ( sysID == 0 ) m_RpcOutOfTime.push_back((it).second);
       else              m_TgcOutOfTime.push_back((it).second);
      
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
	   
       recMuonRoiSvc->reconstruct( ((it).first).roIWord() );
       // create new trigger element for this out of time RoI
       double eta = recMuonRoiSvc->eta();
       double phi = recMuonRoiSvc->phi();
       m_etaOutOfTime.push_back(eta);
       m_phiOutOfTime.push_back(phi);
     
       double etamin = eta - 0.2;
       double etamax = eta + 0.2;
       double phimin = CxxUtils::wrapToPi(phi - 0.2);
       double phimax = CxxUtils::wrapToPi(phi + 0.2);

       if ((it).second >= m_minValueForOutOfTimeBC &&
           (it).second <= m_maxValueForOutOfTimeBC    ) {

           // generic TrigRoiDescriptor
           HLT::TriggerElement* te;
	   TrigRoiDescriptor* roiDescriptor = 
	     new TrigRoiDescriptor( ((it).first).roIWord(), 0, roi_id, eta, etamin, etamax, phi, phimin, phimax,0,-255,255);

           te = addRoI(type_out, roiDescriptor);
           te->setActiveState(true);

           // MuFaststeering also requires a RecMuonRoI
           std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;
	   LVL1::RecMuonRoI* muonroi =
	     new LVL1::RecMuonRoI( ((it).first).roIWord(), &( *m_recRPCRoiSvc ), &( *m_recTGCRoiSvc ), &dummy_thresholds );

	   if( attachFeature( te, muonroi, "L1MuRoI" ) != HLT::OK ) return HLT::ERROR;

	   if(m_log.level() <= MSG::DEBUG) {
	       ATH_MSG_DEBUG("New RoI descriptor for "
	             << region << " created from word 0x"
			     << MSG::hex  << ((it).first).roIWord());
           }
       }
       roi_id += 1;
   }
  

   afterExecMonitors().ignore();

   // everything's ok
   return HLT::OK;
}

// ================================================================================
// ================================================================================

HLT::ErrorCode TrigmuRoI::hltEndEvent() {
  m_executed = false;
  return HLT::OK;
}
