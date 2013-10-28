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
    mStoreGate("StoreGateSvc",this->name()),
    mlvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this)
{
  declareProperty("AlwaysPass",mAlwaysPass=false);
  declareProperty("InvertSelection",mInvertSelection=false);

  declareProperty("L1ItemNames",mL1ItemNames);
  declareProperty("InvertL1ItemNameSelection",mInvertL1ItemNameSelection=false);
  declareProperty("UseBeforePrescaleBit",     mUseBeforePrescaleBit=false);

  declareProperty("TriggerTypeBitMask"                       ,mTriggerTypeBitMask=0x80);
  declareProperty("L1TriggerBitMask"                         ,mL1TriggerBitMask=0);

  declareProperty("TriggerTypeBit"                       ,mTriggerTypeBit=0x80);
  declareProperty("L1TriggerBit"                         ,mL1TriggerBit=0);
  declareProperty("InvertBitMaskSelection",mInvertBitMaskSelection=false);

  declareProperty("Lvl1ResultAccessToool",mlvl1Tool,"tool to access lvl1 result");
}

L1InfoHypo::~L1InfoHypo() {
}

HLT::ErrorCode L1InfoHypo::hltInitialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "initialize()" << endreq;
  
  log << MSG::INFO << "Parameters for L1InfoHypo:" << name() << endreq;
  log << MSG::INFO << "AlwaysPass                  : " << mAlwaysPass                    << endreq;
  log << MSG::INFO << "InvertSelection             : " << mInvertSelection               << endreq;
  log << MSG::INFO << "L1ItemNames                 : " << mL1ItemNames                   << endreq;
  log << MSG::INFO << "InvertL1ItemNameSelection   : " << mInvertL1ItemNameSelection     << endreq;
  log << MSG::INFO << "TriggerTypeBitMask          : " << mTriggerTypeBitMask            << endreq;
  log << MSG::INFO << "L1TriggerBitMask            : " << mL1TriggerBitMask              << endreq;
  log << MSG::INFO << "TriggerTypeBit              : " << mTriggerTypeBit                << endreq;
  log << MSG::INFO << "L1TriggerBit                : " << mL1TriggerBit                  << endreq;
  log << MSG::INFO << "InvertBitMasksSelection     : " << mInvertBitMaskSelection        << endreq;

  if (mStoreGate.retrieve().isFailure()) 
    {
      msg() << MSG::ERROR << "Cannot retrieve service StoreGateSvc" << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  if ( mlvl1Tool.retrieve().isFailure()) 
    {
      log << MSG::FATAL << "Unable to retrieve lvl1 result access tool: " << mlvl1Tool << endreq;
      return HLT::FATAL;
    }

  StatusCode sc = mlvl1Tool->updateConfig(true,true,true);
  if ( sc.isFailure() ) 
    {
      (log) << MSG::FATAL << "Unable to configure tool!" << endreq;
      return HLT::FATAL;
    }

  log << MSG::DEBUG << "initialize success" << endreq;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltBeginRun() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "beginRun"<<  endreq;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltEndRun() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "endRun"<<  endreq;
  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltFinalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltExecute(const HLT::TriggerElement* /*unused*/,bool &Pass) 
{
  MsgStream log(msgSvc(), name());

  Pass=true;
  bool ItemPass=false;

  int output_level = msgSvc()->outputLevel(name());
  if (output_level <= MSG::VERBOSE) log << MSG::VERBOSE << "execL1InfoHypo"  << endreq;
  const EventInfo* constEventInfo(0);

  if (mAlwaysPass) {Pass=true; return HLT::OK; };

  StatusCode sc =  mStoreGate->retrieve(constEventInfo);

  if(sc.isFailure())
    {
      if(output_level <= MSG::FATAL)
	log << MSG::FATAL << "Can't get EventInfo object"  << endreq;
      return StatusCode::FAILURE;
    }
  else
    {
      if (mL1ItemNames.size())
	{
	  const ROIB::RoIBResult* result;
	  sc = mStoreGate->retrieve(result);
	  
	  if(sc.isFailure())
	    {
	      log << MSG::WARNING
		  << "Unable to retrieve RoIBResult from storeGate!"
		  << endreq;
	      return HLT::NO_LVL1_RESULT;
	    }
	  const std::vector<const LVL1CTP::Lvl1Item*>& items = mlvl1Tool->createL1Items(*result);
	  for (std::vector<const LVL1CTP::Lvl1Item*>::const_iterator item = items.begin(); item != items.end(); ++item) 
	    {
	      if(mUseBeforePrescaleBit) {
		if(!(*item)->isPassedBeforePrescale())
		  continue;
	      }
	      else {
		if(!(*item)->isPassedAfterVeto())
		  continue;
	      }

	      log << MSG::DEBUG << "Found active LVL1 item " << (*item)->name() << " ("
		  << (*item)->hashId() << ")" << endreq;
	      for (int unsigned i=0;i<mL1ItemNames.size();i++)
		{
		  log << MSG::DEBUG << "Comparing " << 
		    (*item)->name() << " to [L1_]" <<
		    mL1ItemNames[i] << " result: " << 
		    ( ((*item)->name()==mL1ItemNames[i]) or ((*item)->name()=="L1_"+mL1ItemNames[i]) ) << endreq;
		  ItemPass=ItemPass or ((*item)->name()==mL1ItemNames[i]) or ((*item)->name()=="L1_"+mL1ItemNames[i]);
		};
	    }

	  if (mInvertL1ItemNameSelection) ItemPass=(ItemPass)?0:1;
	}
      else
	ItemPass=true;

      TriggerInfo* tinfo = constEventInfo->trigger_info();
      TriggerInfo::number_type L1TriggerType=tinfo->level1TriggerType();
      TriggerInfo::number_type L1ID=tinfo->extendedLevel1ID();
      
      //printf("L1 Info: %x %x\n",L1TriggerType,L1ID);
      log << MSG::DEBUG << "L1 Info: " << hex << L1TriggerType << " " << L1ID << endreq;
      bool PassBM=
	((L1TriggerType & mTriggerTypeBitMask)==mTriggerTypeBit||(mTriggerTypeBitMask==0))&&
	((L1ID          & mL1TriggerBitMask  )==mL1TriggerBit    ||(mL1TriggerBitMask  ==0));
      if (mInvertBitMaskSelection) PassBM=(PassBM)?0:1;
      Pass = Pass and PassBM and ItemPass;
    }

  if (mInvertSelection) Pass=(Pass)?0:1;
  
  return HLT::OK;
}

