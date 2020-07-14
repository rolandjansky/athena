/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFCaloHypoNoise.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR: Denis Oliveira Damazio
//
// ********************************************************************
//
#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "LArRecEvent/LArNoisyROSummary.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigEFCaloHypoNoise.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "hltinterface/IInfoRegister.h"
#include "hltinterface/ContainerFactory.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypoNoise::TrigEFCaloHypoNoise(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator), m_isInterface(false), m_noisyROTool("",this),m_hasFebs(false) {

  declareProperty("Etcut",   m_EtCut = 40*Gaudi::Units::GeV); // Default: 40 GeV
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("NoiseTool",   m_noisyROTool);

  declareProperty( "BadFEBCut", m_MinBadFEB=5 );
  declareProperty( "TimeToClear", m_timeTagPosToClear=300);
  declareProperty( "ISPublishTime", m_publishTime=180);
  declareProperty( "BadFEBFlaggedPartitions", m_badFEBFlaggedPartitions=true);
  declareProperty( "SatTightFlaggedPartitions", m_satTightFlaggedPartitions=true);
  declareProperty( "MNBLooseFlaggedPartitions", m_mNBLooseFlaggedPartitions=true);
  declareProperty( "MNBTightFlaggedPartitions", m_mNBTightFlaggedPartitions=true);
  declareProperty( "MNBTight_PsVetoFlaggedPartitions", m_mNBTight_PsVetoFlaggedPartitions=true);

}

TrigEFCaloHypoNoise::~TrigEFCaloHypoNoise()
{  }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypoNoise::hltInitialize()
  // ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigEFCaloHypoNoise.TrigEFCaloHypoTot");
    m_timers.push_back(tmp);
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  m_isInterface = false;

  m_timeTagPosRec = 0;

  if ( m_noisyROTool.retrieve().isFailure() ){
	msg() << MSG::WARNING << "Could not retrieve tool, no noise burst hunting" << endmsg;
	return HLT::OK;
  }

  StatusCode sc = m_knownBadFEBsVecKey.initialize();
  if(sc != StatusCode::SUCCESS) {
     ATH_MSG_WARNING( "Could not find Known Bad FEBs list, assuming empty !!!" );
  }
  sc = m_knownMNBFEBsVecKey.initialize() ;
  if(sc != StatusCode::SUCCESS) {
     ATH_MSG_WARNING( "Could not find Known MNB FEBs list, assuming empty !!!" );
  }
  m_hasFebs=false;

  auto cfact = hltinterface::ContainerFactory::getInstance();
  if ( cfact ) {
      msg() << MSG::DEBUG << "Got the factory for TDAQ interface, will try to register vectors" << endmsg;
      try {
          m_IsObject = cfact->constructContainer("LArISInfo_"+name(),"LArNoiseBurstCandidates");
          m_evntPos = cfact->addIntVector(m_IsObject,"Flag",hltinterface::GenericHLTContainer::LASTVALUE);
          m_timeTagPos = cfact->addIntVector(m_IsObject,"TimeStamp",hltinterface::GenericHLTContainer::LASTVALUE);
          m_timeTagPosns = cfact->addIntVector(m_IsObject,"TimeStamp_ns",hltinterface::GenericHLTContainer::LASTVALUE);
          hltinterface::IInfoRegister::instance()->registerObject("/HLTObjects/",m_IsObject);
          m_isInterface = true;
      }
      catch (std::exception& ex ) {
	  msg() << MSG::WARNING << "Cannot really use ISInfo publication. got exception " << ex.what() << endmsg;
          m_isInterface = false;
      }
  } // if cfact

  // create mask
  m_mask = 0x0;
  if ( m_badFEBFlaggedPartitions ) m_mask|=0x1;
  if ( m_satTightFlaggedPartitions ) m_mask|=0x2;
  if ( m_mNBLooseFlaggedPartitions ) m_mask|=0x10;
  if ( m_mNBTightFlaggedPartitions ) m_mask|=0x20;
  if ( m_mNBTight_PsVetoFlaggedPartitions ) m_mask|=0x40;
  msg() << MSG::DEBUG << "using a mask to selec events : " << std::hex << m_mask << std::dec << endmsg;
  
  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypoNoise::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypoNoise::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------
  //  if (m_timersvc) m_timers[0]->start();

  pass=false;
  bool msgDebug = msgLvl(MSG::DEBUG);

  if ( !m_noisyROTool ) return HLT::OK;
  // no tool running anyway

  const CaloCellContainer* outCells(0);
  HLT::ErrorCode ec = getFeature(outputTE, outCells);
  if(ec!=HLT::OK) {
    ATH_MSG_WARNING(" Failed to get CellCollections ");
    return ec;
  }

  if(!m_hasFebs){
     bool hasBad=false; bool hasMNB=false;
     if (!evtStore()->contains<LArBadFebCont>(m_knownBadFEBsVecKey.key())) { 
         ATH_MSG_WARNING("No Bad FEBs object existing, assume empty list");
         hasBad=true;
     } else {   
         SG::ReadCondHandle<LArBadFebCont> badHdl(m_knownBadFEBsVecKey);
         const LArBadFebCont* badCont=*badHdl;
         if(badCont) {
           for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); i++) {
              m_knownBadFEBs.insert(i->first);
           }
           if(m_knownBadFEBs.size() == 0) {
                 ATH_MSG_WARNING("List of known Bad FEBs empty !? ");
           }
           hasBad=true;
         }
     }
     ATH_MSG_DEBUG("Number of known Bad FEBs: "<<m_knownBadFEBs.size());

     if (!evtStore()->contains<LArBadFebCont>(m_knownMNBFEBsVecKey.key())) { 
         ATH_MSG_WARNING("No MNB FEBs object existing, assume empty list");
         hasMNB=true;
     } else {   
         SG::ReadCondHandle<LArBadFebCont> MNBHdl(m_knownMNBFEBsVecKey);
         const LArBadFebCont* MNBCont=*MNBHdl;
         if(MNBCont) {
           for(LArBadFebCont::BadChanVec::const_iterator i = MNBCont->begin(); i!=MNBCont->end(); i++) {
              m_knownMNBFEBs.push_back(HWIdentifier(i->first));
           }
           if(m_knownMNBFEBs.size() == 0) {
                 ATH_MSG_WARNING("List of known MNB FEBs empty !? ");
           }
           hasMNB=true;
         }
     }
     ATH_MSG_DEBUG("Number of known MNB FEBs: "<<m_knownMNBFEBs.size());
     if(hasBad && hasMNB) {
        m_hasFebs=true;
     } else {
        ATH_MSG_WARNING("No known Bad or MNB Febs list available, will be empty !!");
     }      
  }   
  const std::set<unsigned int> knownBadFEBs(m_knownBadFEBs);
  const std::vector<HWIdentifier> knownMNBFEBs(m_knownMNBFEBs);
  unsigned int flag = 0;
  if ( outCells ) {
	if ( msgDebug ) msg() << MSG::DEBUG << "Got cell container, will process it" << endmsg;
	std::unique_ptr<LArNoisyROSummary> noisyRO = m_noisyROTool->process(outCells, &knownBadFEBs, &knownMNBFEBs);
	if ( msgDebug ) msg() << MSG::DEBUG << "processed it" << endmsg;
        if ( noisyRO->BadFEBFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : BadFEBFlaggedPartitions" << endmsg;
	      flag |= 0x1;
        }
        if ( noisyRO->BadFEB_WFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : BadFEB_WFlaggedPartitions" << endmsg;
	      flag |= 0x8;
        }
        if ( noisyRO->SatTightFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : SatTightFlaggedPartitions" << endmsg;
	      flag |= 0x2;
        }
        if ( noisyRO->MNBLooseFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : MNBLooseFlaggedPartions" << endmsg;
	      flag |= 0x10;
        }
        if ( noisyRO->MNBTightFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : MNBTightFlaggedPartions" << endmsg;
	      flag |= 0x20;
        }
  } // end of if outCells

  if ( msgDebug ) msg() << MSG::DEBUG << "got the flag : " << (unsigned int)flag << endmsg;
  


  if ( flag != 0x0 ) {
	if ( msgDebug ) msg() << MSG::DEBUG << "LAr Noise detected : ";
	pass = true;
  }
        else if ( msgDebug ) msg() << MSG::DEBUG << "LAr Noise not detected!" << endmsg;

  const xAOD::EventInfo* evt;
  if ( (store()->retrieve(evt)).isFailure() ) {
  	msg(MSG::DEBUG) << endmsg;
  	msg(MSG::ERROR) << "Cannot access eventinfo" << endmsg;
  } 
  long int thisTimeStamp = 0;
  if ( evt ) thisTimeStamp = evt->timeStamp();
  if ( m_isInterface ) { // only clear up if the vector is being filled
       if ( ( ( thisTimeStamp - m_timeTagPosRec) > m_timeTagPosToClear ) ){

		    std::vector<long>& ee = m_IsObject->getIntVecField(m_evntPos);
		    std::vector<long>& tt = m_IsObject->getIntVecField(m_timeTagPos);
		    std::vector<long>& ttn = m_IsObject->getIntVecField(m_timeTagPosns);
		    int toBeCleared = 0;
		    for(unsigned int dd=0;dd<tt.size();dd++){
			if ( ( thisTimeStamp - tt[dd] )  < m_publishTime ) {
				toBeCleared = (int)dd-1;
				break;
			}
		    }
		    if ( toBeCleared > 0 ) {
			ee.erase(ee.begin(),ee.begin()+(toBeCleared) );
			tt.erase(tt.begin(),tt.begin()+(toBeCleared) );
			ttn.erase(ttn.begin(),ttn.begin()+(toBeCleared) );
		    }


        	m_timeTagPosRec = thisTimeStamp ; // records to remember when things were cleared
       }
  }
  if ( pass ) {
	if ( evt ) {
		if ( msgDebug ) msg() << MSG::DEBUG << "at event number : "
			<< evt->eventNumber() << "; LB : "
			<< evt->lumiBlock() << "; timeStamp : "
			<< evt->timeStamp() << "; timeStamp ns : "
			<< evt->timeStampNSOffset() << endmsg;
		if ( m_isInterface ) {
		    m_IsObject->appendField(m_evntPos,std::vector<long>{flag});
		    m_IsObject->appendField(m_timeTagPos,std::vector<long>{(long int)evt->timeStamp()});
		    m_IsObject->appendField(m_timeTagPosns,std::vector<long>{(long int)evt->timeStampNSOffset()});
		}
	}
  }

  return HLT::OK;

}
