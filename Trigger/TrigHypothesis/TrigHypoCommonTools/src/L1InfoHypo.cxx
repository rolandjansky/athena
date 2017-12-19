/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1InfoHypo.cxx
*/
#include <cmath>
#include "TrigHypoCommonTools/L1InfoHypo.hxx"

#include <EventInfo/EventID.h>
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"

#include "RegionSelector/IRegSelSvc.h"
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteering/Lvl1ItemsAndRoIs.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

using namespace std;

L1InfoHypo::L1InfoHypo(const std::string& name, ISvcLocator* svc) 
  : HLT::HypoAlgo(name, svc),
    m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this)
{
  declareProperty("AlwaysPass",m_alwaysPass=false);
  declareProperty("InvertSelection",m_invertSelection=false);

  declareProperty("L1ItemNames",m_L1ItemNames);
  declareProperty("InvertL1ItemNameSelection",m_invertL1ItemNameSelection=false);
  declareProperty("UseBeforePrescaleBit",     m_useBeforePrescaleBit=false);

  declareProperty("TriggerTypeBitMask"                       ,m_triggerTypeBitMask=0x80);
  declareProperty("L1TriggerBitMask"                         ,m_L1TriggerBitMask=0);

  declareProperty("TriggerTypeBit"                       ,m_triggerTypeBit=0x80);
  declareProperty("L1TriggerBit"                         ,m_L1TriggerBit=0);
  declareProperty("InvertBitMaskSelection",m_invertBitMaskSelection=false);

  declareProperty("Lvl1ResultAccessToool",m_lvl1Tool,"tool to access lvl1 result");
}

L1InfoHypo::~L1InfoHypo() {
}

HLT::ErrorCode L1InfoHypo::hltInitialize() 
{
  msg() << MSG::DEBUG << "initialize()" << endmsg;
  
  msg() << MSG::INFO << "Parameters for L1InfoHypo:" << name() << endmsg;
  msg() << MSG::INFO << "AlwaysPass                  : " << m_alwaysPass                    << endmsg;
  msg() << MSG::INFO << "InvertSelection             : " << m_invertSelection               << endmsg;
  msg() << MSG::INFO << "L1ItemNames                 : " << m_L1ItemNames                   << endmsg;
  msg() << MSG::INFO << "InvertL1ItemNameSelection   : " << m_invertL1ItemNameSelection     << endmsg;
  msg() << MSG::INFO << "TriggerTypeBitMask          : " << m_triggerTypeBitMask            << endmsg;
  msg() << MSG::INFO << "L1TriggerBitMask            : " << m_L1TriggerBitMask              << endmsg;
  msg() << MSG::INFO << "TriggerTypeBit              : " << m_triggerTypeBit                << endmsg;
  msg() << MSG::INFO << "L1TriggerBit                : " << m_L1TriggerBit                  << endmsg;
  msg() << MSG::INFO << "InvertBitMasksSelection     : " << m_invertBitMaskSelection        << endmsg;


  if ( m_lvl1Tool.retrieve().isFailure()) 
    {
      msg() << MSG::FATAL << "Unable to retrieve lvl1 result access tool: " << m_lvl1Tool << endmsg;
      return HLT::FATAL;
    }

  StatusCode sc = m_lvl1Tool->updateConfig(true,true,true);
  if ( sc.isFailure() ) 
    {
      (msg()) << MSG::FATAL << "Unable to configure tool!" << endmsg;
      return HLT::FATAL;
    }

  msg() << MSG::DEBUG << "initialize success" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltBeginRun() 
{
  msg() << MSG::DEBUG << "beginRun"<<  endmsg;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltEndRun() 
{
  msg() << MSG::DEBUG << "endRun"<<  endmsg;
  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltFinalize() 
{
  msg() << MSG::DEBUG << "finalize()" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltExecute(const HLT::TriggerElement* /*unused*/,bool &Pass) 
{

  Pass=true;
  bool ItemPass=false;

  int output_level = msgSvc()->outputLevel(name());
  if (output_level <= MSG::VERBOSE) msg() << MSG::VERBOSE << "execL1InfoHypo"  << endmsg;
  const EventInfo* constEventInfo(0);

  if (m_alwaysPass) {Pass=true; return HLT::OK; };

  StatusCode sc =  evtStore()->retrieve(constEventInfo);

  if(sc.isFailure())
    {
      if(output_level <= MSG::FATAL)
	msg() << MSG::FATAL << "Can't get EventInfo object"  << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  else
    {
      if (m_L1ItemNames.size())
	{
	  const ROIB::RoIBResult* result;
	  sc = evtStore()->retrieve(result);
	  
	  if(sc.isFailure())
	    {
	      msg() << MSG::WARNING
		  << "Unable to retrieve RoIBResult from storeGate!"
		  << endmsg;
	      return HLT::NO_LVL1_RESULT;
	    }
	  const std::vector<const LVL1CTP::Lvl1Item*>& items = m_lvl1Tool->createL1Items(*result);
	  for (std::vector<const LVL1CTP::Lvl1Item*>::const_iterator item = items.begin(); item != items.end(); ++item) 
	    {
	      if(m_useBeforePrescaleBit) {
		if(!(*item)->isPassedBeforePrescale())
		  continue;
	      }
	      else {
		if(!(*item)->isPassedAfterVeto())
		  continue;
	      }

	      msg() << MSG::DEBUG << "Found active LVL1 item " << (*item)->name() << " ("
		  << (*item)->hashId() << ")" << endmsg;
	      for (int unsigned i=0;i<m_L1ItemNames.size();i++)
		{
		  msg() << MSG::DEBUG << "Comparing " << 
		    (*item)->name() << " to [L1_]" <<
		    m_L1ItemNames[i] << " result: " << 
		    ( ((*item)->name()==m_L1ItemNames[i]) or ((*item)->name()=="L1_"+m_L1ItemNames[i]) ) << endmsg;
		  ItemPass=ItemPass or ((*item)->name()==m_L1ItemNames[i]) or ((*item)->name()=="L1_"+m_L1ItemNames[i]);
		};
	    }

	  if (m_invertL1ItemNameSelection) ItemPass=(ItemPass)?0:1;
	}
      else
	ItemPass=true;

      const TriggerInfo* tinfo = constEventInfo->trigger_info();
      TriggerInfo::number_type L1TriggerType=tinfo->level1TriggerType();
      TriggerInfo::number_type L1ID=tinfo->extendedLevel1ID();
      
      //printf("L1 Info: %x %x\n",L1TriggerType,L1ID);
      msg() << MSG::DEBUG << "L1 Info: " << hex << L1TriggerType << " " << L1ID << endmsg;
      bool PassBM=
	((L1TriggerType & m_triggerTypeBitMask)==m_triggerTypeBit||(m_triggerTypeBitMask==0))&&
	((L1ID          & m_L1TriggerBitMask  )==m_L1TriggerBit    ||(m_L1TriggerBitMask  ==0));
      if (m_invertBitMaskSelection) PassBM=(PassBM)?0:1;
      Pass = Pass and PassBM and ItemPass;
    }

  if (m_invertSelection) Pass=(Pass)?0:1;
  
  return HLT::OK;
}

