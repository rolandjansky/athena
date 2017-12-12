/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "StoreGate/StoreGateSvc.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigT1Result/RoIBResult.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigmuRoI/TrigmuRoI.h"

#include "TrigSteeringEvent/PhiHelper.h"

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
  m_log.setLevel(outputLevel());

  m_log << MSG::INFO << "initialize()" << endreq;

#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    m_log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
  if(tmp_msgSvc != 0) {
    m_log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name() << endreq;
  }
#endif

   m_log << MSG::INFO << " ROB ID: DAQ CTP                            = " << m_daqCTPROBid
	 << std::setw(6) << " (=0x" << MSG::hex << m_daqCTPROBid.value() << MSG::dec << ")" << endreq;
   m_log << MSG::INFO << " ROB ID: DAQ muCTPi                         = " << m_daqMuCTPiROBid
	 << std::setw(6) << " (=0x" << MSG::hex << m_daqMuCTPiROBid.value() << MSG::dec << ")" << endreq;

   m_log << MSG::INFO << m_minValueForOutOfTimeBC << endreq;
   m_log << MSG::INFO << m_maxValueForOutOfTimeBC << endreq;


   // Retrieve the MuonRoITool
   StatusCode sc = m_trigMuonRoITool.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Could not retrieve " << m_trigMuonRoITool << endreq;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved tool " << m_trigMuonRoITool << endreq;
   }

   sc = m_recRPCRoiSvc.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Couldn't connect to " << m_recRPCRoiSvc << endreq;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved Service " << m_recRPCRoiSvc << endreq;
   }
  
   sc = m_recTGCRoiSvc.retrieve();
   if ( sc.isFailure() ) {
     m_log << MSG::ERROR << "Couldn't connect to " << m_recTGCRoiSvc << endreq;
     return HLT::ERROR;
   } else {
     m_log << MSG::INFO << "Retrieved Service " << m_recTGCRoiSvc << endreq;
   }

   m_log << MSG::DEBUG << "initialization - done" << endreq;

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
	    << ")" << endreq;
   }

   int roi_id = 0;

   //--------------------------------------------------------------------------
   // Gather the Muon RoIs out of time by the
   //--------------------------------------------------------------------------
   int BCID_diff = distance(m_trigMuonRoITool->begin_OutOfTimeRoIs(),m_trigMuonRoITool->end_OutOfTimeRoIs());
    
   if(m_log.level() <= MSG::DEBUG) {
       m_log << MSG::DEBUG << "=====================================================" << endreq;
       m_log << MSG::DEBUG << " RoIs out of time with event BCID:  Number of RoIs = "
             << BCID_diff << endreq;
       m_log << MSG::DEBUG << "=====================================================" << endreq;
   }
   
   std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it_begin_ot, it_end_ot;
   
   if(m_log.level() <= MSG::DEBUG)
       m_log << MSG::DEBUG << "===> execute() TrigMuonRoITool Test Algorithm: get begin in time iterator" << endreq;
   
   it_begin_ot = m_trigMuonRoITool->begin_OutOfTimeRoIs();
   
   if(m_log.level() <= MSG::DEBUG)
       m_log << MSG::DEBUG << "===> execute() TrigMuonRoITool Test Algorithm: get end   in time iterator" << endreq;
   
   it_end_ot   = m_trigMuonRoITool->end_OutOfTimeRoIs();
   
   if(m_log.level() <= MSG::DEBUG)
       m_log << MSG::DEBUG << "===> execute() TrigMuonRoITool Test Algorithm: print out of time RoIs" << endreq;
   
   for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it = m_trigMuonRoITool->begin_OutOfTimeRoIs();
       it != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++it) {
       
       if(m_log.level() <= MSG::DEBUG) {
           m_log << MSG::DEBUG << " Difference(RoI(BCID) - Event(BCID)) = " << (*it).second << endreq;
           m_log << MSG::DEBUG << " ------------------------------------- " << endreq;
           m_log << MSG::DEBUG << "RoIB word               : 0x" << MSG::hex << ((*it).first).roIWord() << MSG::dec << endreq;
           m_log << MSG::DEBUG << "Threshold               :  pt" << ((*it).first).pt() << endreq;
           m_log << MSG::DEBUG << "Sector ID               :  " << ((*it).first).getSectorID() << endreq;
           m_log << MSG::DEBUG << "Sector addr             :  0x" << MSG::hex << ((*it).first).getSectorAddress() << MSG::dec << endreq;
           m_log << MSG::DEBUG << "Sector overflow         :  " << ((*it).first).getSectorOverflow() << endreq;
           m_log << MSG::DEBUG << "RoI overflow            :  " << ((*it).first).getRoiOverflow() << endreq;
           m_log << MSG::DEBUG << "RoI number              :  " << ((*it).first).getRoiNumber() << endreq;
           m_log << MSG::DEBUG << "IsHighestPt             :  " << ((*it).first).getCandidateIsHighestPt() << endreq;
           m_log << MSG::DEBUG << "=================================================" << endreq;
       }
       
       unsigned int temp_sysID =
           getBitMaskValue(((*it).first).getSectorAddress(), LVL1::SysIDMask );
       unsigned int sysID = 0;                // Barrel
       if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
       else if( temp_sysID & 0x1 ) sysID = 2; // Forward


       if ( sysID == 0 ) m_RpcOutOfTime.push_back((*it).second);
       else              m_TgcOutOfTime.push_back((*it).second);
      
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
       m_etaOutOfTime.push_back(eta);
       m_phiOutOfTime.push_back(phi);
     
       double etamin = eta - 0.2;
       double etamax = eta + 0.2;
       double phimin = HLT::wrapPhi(phi - 0.2);
       double phimax = HLT::wrapPhi(phi + 0.2); 

       if ((*it).second >= m_minValueForOutOfTimeBC &&
           (*it).second <= m_maxValueForOutOfTimeBC    ) {

           // generic TrigRoiDescriptor
           HLT::TriggerElement* te;
	   TrigRoiDescriptor* roiDescriptor = 
	     new TrigRoiDescriptor( ((*it).first).roIWord(), 0, roi_id, eta, etamin, etamax, phi, phimin, phimax,0,-255,255);

           te = addRoI(type_out, roiDescriptor);
           te->setActiveState(true);

           // MuFaststeering also requires a RecMuonRoI
           std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;
	   LVL1::RecMuonRoI* muonroi =
	     new LVL1::RecMuonRoI( ((*it).first).roIWord(), &( *m_recRPCRoiSvc ), &( *m_recTGCRoiSvc ), &dummy_thresholds );

	   if( attachFeature( te, muonroi, "L1MuRoI" ) != HLT::OK ) return HLT::ERROR;

	   if(m_log.level() <= MSG::DEBUG) {
	       m_log << MSG::DEBUG << "New RoI descriptor for "
	             << region << " created from word 0x"
	             << MSG::hex  << ((*it).first).roIWord() << MSG::dec 
		     << endreq;
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
