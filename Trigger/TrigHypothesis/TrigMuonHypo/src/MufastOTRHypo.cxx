/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "TrigMuonHypo/MufastOTRHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigT1Result/RoIBResult.h"

class ISvcLocator;


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


MufastOTRHypo::MufastOTRHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_trigMuonRoITool( "TrigMuonRoITool" ) 
{
	
	declareProperty("AcceptAll", m_acceptAll=false);
        declareProperty("SegmentsTh", m_segments=1);
	
	declareMonitoredStdContainer("RpcOutOfTimeIn",  m_RpcOutOfTimeIn, AutoClear);
	declareMonitoredStdContainer("RpcOutOfTimeOut", m_RpcOutOfTimeOut, AutoClear);
	declareMonitoredStdContainer("TgcOutOfTimeIn",  m_TgcOutOfTimeIn, AutoClear);
	declareMonitoredStdContainer("TgcOutOfTimeOut", m_TgcOutOfTimeOut, AutoClear);
	
	declareMonitoredStdContainer("Pt",  m_fex_pt, AutoClear);
	declareMonitoredStdContainer("Eta", m_fex_eta, AutoClear);
	declareMonitoredStdContainer("Phi", m_fex_phi, AutoClear);

        m_storeGate = 0;
}

MufastOTRHypo::~MufastOTRHypo(){
}

HLT::ErrorCode MufastOTRHypo::hltInitialize(){

  msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	<< PACKAGE_VERSION << endmsg;

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endmsg;
  }
   
  // Retrieve the MuonRoITool
  StatusCode sc = m_trigMuonRoITool.retrieve();
  if ( sc.isFailure() ) {
     msg() << MSG::ERROR << "Could not retrieve " << m_trigMuonRoITool 
           << endmsg;
     return HLT::ERROR;
  } else {
     msg() << MSG::INFO << "Retrieved tool " << m_trigMuonRoITool << endmsg;
  }

  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endmsg;
  
  return HLT::OK;
}


HLT::ErrorCode MufastOTRHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}



std::pair<unsigned int, int> 
MufastOTRHypo::getRoIFeature(uint32_t roiWord) {
 
    int BCID_diff = 0;
    unsigned int sysID = 0;
  
    for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it = m_trigMuonRoITool->begin_OutOfTimeRoIs();
           it != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++it) {
        if( ((*it).first).roIWord() == roiWord ) {
            BCID_diff = (*it).second;
	    unsigned int temp_sysID =
            getBitMaskValue(((*it).first).getSectorAddress(), LVL1::SysIDMask );
            if( temp_sysID & 0x2 ) sysID = 1;
            else if( temp_sysID & 0x1 ) sysID = 2;
	    break;
        }
    }
    return std::pair<unsigned int,int>(sysID,BCID_diff);
}


HLT::ErrorCode MufastOTRHypo::hltExecute(const HLT::TriggerElement* outputTE, 
                                      bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;

  std::vector<const TrigRoiDescriptor*> vectorOfRoIs; 
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfRoIs,"secondaryRoI_L2");
  if(status!=HLT::OK) {
      msg() << MSG::DEBUG << "no TrigRoI descriptor found" << endmsg;
      return status;
  }
  
  // Check that there is at least 1 RoI descriptor 
  if (vectorOfRoIs.size() == 0){
      msg() << MSG::ERROR << "Couldn't find any Trig RoI descriptor"
            << endmsg;
      return HLT::NAV_ERROR;
  }


  msg() << MSG::DEBUG << "Found n. " << vectorOfRoIs.size()
                      << " TrigRoI descriptors" << endmsg;
		      
  unsigned int sysID=0;
  int          BCID_diff=0;

  
  for ( std::vector<const TrigRoiDescriptor*>::const_iterator it =
                vectorOfRoIs.begin(); it!=vectorOfRoIs.end(); ++it) {

      uint32_t roi_word = (vectorOfRoIs.front())->roiWord();
      std::pair<unsigned int, int> roi_feature = getRoIFeature(roi_word);
      if ( roi_feature.first == 0 ) m_RpcOutOfTimeIn.push_back(roi_feature.second);
      else                          m_TgcOutOfTimeIn.push_back(roi_feature.second);
  }


   // Get vector of pointers to L2StandAloneMuon linked to the outputTE 
   const xAOD::L2StandAloneMuonContainer* vectorOfMuons(0);
   status = getFeature(outputTE, vectorOfMuons);
   if (status!=HLT::OK) {
      msg() << MSG::DEBUG << "no L2StandAloneMuon found" << endmsg;
      return status;
   }

  // Check that there at least one L2StandAloneMuon
  if (vectorOfMuons->size() == 0){
      msg() << MSG::ERROR << "Couldn't find any L2StandAloneMuon" << endmsg;
      return HLT::NAV_ERROR;
  }

  msg() << MSG::DEBUG << "Found n. " << vectorOfMuons->size()
                      << " L2StandAloneMuon" << endmsg;

  
  bool result = false;

  for ( xAOD::L2StandAloneMuonContainer::const_iterator ft =
        vectorOfMuons->begin(); ft !=vectorOfMuons->end();  ++ft) {
  
    const xAOD::L2StandAloneMuon* pMuon = *ft;
      for ( std::vector<const TrigRoiDescriptor*>::const_iterator it =
                    vectorOfRoIs.begin(); it!=vectorOfRoIs.end(); ++it) {
          if ((unsigned int)pMuon->roiId() == (*it)->roiId()) {
	      uint32_t roi_word = (*it)->roiWord();
              std::pair<unsigned int, int> roi_feature = getRoIFeature(roi_word);
              sysID     = roi_feature.first;
              BCID_diff = roi_feature.second;
          } else {
	      msg() << MSG::ERROR << 
	      "Couldn't find connection among RoI Descriptor and Muon Feature"
	      << endmsg;
	      return HLT::NAV_ERROR; 
	  }
      }

      if(m_acceptAll) {
          pass = true;
          // fill Monitoring histos
          m_fex_pt.push_back( (pMuon->pt())? pMuon->pt()  : -9999 );
          m_fex_eta.push_back( (pMuon->etaMS())? pMuon->etaMS() : -9999);
          m_fex_phi.push_back( (pMuon->phiMS())? pMuon->phiMS() : -9999);
      
          if ( sysID == 0 ) m_RpcOutOfTimeOut.push_back(BCID_diff);
          else              m_TgcOutOfTimeOut.push_back(BCID_diff);
          if(msgLvl() <= MSG::DEBUG) {
              msg() << MSG::DEBUG 
	            << "Accept property is set: taking all the events"
	    	    << endmsg;
          }
          return HLT::OK;
      }
  

      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "Accept property not set: applying selection!"
                << endmsg;
      }
    
      int num_of_segments = 0;

      int inner  = (pMuon->sAddress()==-1)?
        xAOD::L2MuonParameters::Chamber::EndcapInner: xAOD::L2MuonParameters::Chamber::BarrelInner;
      int middle = (pMuon->sAddress()==-1)?
        xAOD::L2MuonParameters::Chamber::EndcapMiddle: xAOD::L2MuonParameters::Chamber::BarrelMiddle;
      int outer  = (pMuon->sAddress()==-1)?
        xAOD::L2MuonParameters::Chamber::EndcapOuter: xAOD::L2MuonParameters::Chamber::BarrelOuter;

      if (pMuon->superPointZ(inner) != 0)  ++num_of_segments;
      if (pMuon->superPointZ(middle) != 0)  ++num_of_segments;
      if (pMuon->superPointZ(outer) != 0)  ++num_of_segments;
  
      // Some debug output:
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() 
	        << endmsg;
      }
  

      if(num_of_segments >= m_segments) {
          result = true;
          // fill Monitoring histos
          m_fex_pt.push_back( (pMuon->pt())? pMuon->pt()  : -9999);
          m_fex_eta.push_back( (pMuon->etaMS())? pMuon->etaMS() : -9999);
          m_fex_phi.push_back( (pMuon->etaMS())? pMuon->phiMS() : -9999);
          if ( sysID == 0 ) m_RpcOutOfTimeOut.push_back(BCID_diff);
          else              m_TgcOutOfTimeOut.push_back(BCID_diff);
      } else {
          // fill Monitoring histos
          m_fex_pt.push_back( 9999 );
          m_fex_eta.push_back( 9999 );
          m_fex_phi.push_back( 9999 );
          if ( sysID == 0 ) m_RpcOutOfTimeOut.push_back( 9999 );
          else              m_TgcOutOfTimeOut.push_back( 9999 );
      }

      if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << " Super Point I, z = " << pMuon->superPointZ(inner) <<endmsg; 
          msg() << MSG::DEBUG << " Super Point M, z = " << pMuon->superPointZ(middle) <<endmsg; 
          msg() << MSG::DEBUG << " Super Point O, z = " << pMuon->superPointZ(outer) <<endmsg; 
          msg() << MSG::DEBUG << " Result of Out of Time RoI Hypothesis is " 
                << (result?"true":"false") << endmsg;
      }
  }
  
  //store the result
  pass = result;
  
  return HLT::OK;
}
