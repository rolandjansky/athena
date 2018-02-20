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
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteering/Lvl1ItemsAndRoIs.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

using namespace std;

L1InfoHypo::L1InfoHypo(const std::string& name, ISvcLocator* svc) 
  : HLT::HypoAlgo(name, svc),
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
  ATH_MSG_DEBUG( "initialize()" );
  
  ATH_MSG_INFO( "Parameters for L1InfoHypo:" << name() );
  ATH_MSG_INFO( "AlwaysPass                  : " << mAlwaysPass                    );
  ATH_MSG_INFO( "InvertSelection             : " << mInvertSelection               );
  ATH_MSG_INFO( "L1ItemNames                 : " << mL1ItemNames                   );
  ATH_MSG_INFO( "InvertL1ItemNameSelection   : " << mInvertL1ItemNameSelection     );
  ATH_MSG_INFO( "TriggerTypeBitMask          : " << mTriggerTypeBitMask            );
  ATH_MSG_INFO( "L1TriggerBitMask            : " << mL1TriggerBitMask              );
  ATH_MSG_INFO( "TriggerTypeBit              : " << mTriggerTypeBit                );
  ATH_MSG_INFO( "L1TriggerBit                : " << mL1TriggerBit                  );
  ATH_MSG_INFO( "InvertBitMasksSelection     : " << mInvertBitMaskSelection        );


  if ( mlvl1Tool.retrieve().isFailure()) 
    {
      ATH_MSG_FATAL( "Unable to retrieve lvl1 result access tool: " << mlvl1Tool );
      return HLT::FATAL;
    }

  StatusCode sc = mlvl1Tool->updateConfig(true,true,true);
  if ( sc.isFailure() ) 
    {
      ATH_MSG_FATAL( "Unable to configure tool!" );
      return HLT::FATAL;
    }

  ATH_MSG_DEBUG( "initialize success" );

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltBeginRun() 
{
  ATH_MSG_DEBUG( "beginRun");

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltEndRun() 
{
  ATH_MSG_DEBUG( "endRun");
  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltFinalize() 
{
  ATH_MSG_DEBUG( "finalize()" );

  return HLT::OK;
}

HLT::ErrorCode L1InfoHypo::hltExecute(const HLT::TriggerElement* /*unused*/,bool &Pass) 
{

  Pass=true;
  bool ItemPass=false;

  ATH_MSG_VERBOSE( "execL1InfoHypo"  );
  const EventInfo* constEventInfo(0);

  if (mAlwaysPass) {Pass=true; return HLT::OK; };

  StatusCode sc =  evtStore()->retrieve(constEventInfo);

  if(sc.isFailure())
    {
	ATH_MSG_FATAL( "Can't get EventInfo object"  );
      return StatusCode::FAILURE;
    }
  else
    {
      if (mL1ItemNames.size())
	{
	  const ROIB::RoIBResult* result;
	  sc = evtStore()->retrieve(result);
	  
	  if(sc.isFailure())
	    {
	      ATH_MSG_WARNING("Unable to retrieve RoIBResult from storeGate!");
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

	      ATH_MSG_DEBUG( "Found active LVL1 item " << (*item)->name() << " ("
		  << (*item)->hashId() << ")" );
	      for (int unsigned i=0;i<mL1ItemNames.size();i++)
		{
		  ATH_MSG_DEBUG( "Comparing " << 
		    (*item)->name() << " to [L1_]" <<
		    mL1ItemNames[i] << " result: " << 
		    ( ((*item)->name()==mL1ItemNames[i]) or ((*item)->name()=="L1_"+mL1ItemNames[i]) ) );
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
      ATH_MSG_DEBUG("L1 Info: " << hex << L1TriggerType << " " << L1ID);
      bool PassBM=
	((L1TriggerType & mTriggerTypeBitMask)==mTriggerTypeBit||(mTriggerTypeBitMask==0))&&
	((L1ID          & mL1TriggerBitMask  )==mL1TriggerBit    ||(mL1TriggerBitMask  ==0));
      if (mInvertBitMaskSelection) PassBM=(PassBM)?0:1;
      Pass = Pass and PassBM and ItemPass;
    }

  if (mInvertSelection) Pass=(Pass)?0:1;
  
  return HLT::OK;
}

