
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
*/


// ********************************************************************
// NAME:     LArNoiseCorrelationMonAlg.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Margherita Spalla (margherita.spalla@cern.ch)
//           Based on structure of LArDigitMon by Helary Louis (helary@lapp.in2p3.fr)
// 
// Computes and plots the correlation between single channels to monitor coherent noise. 
// The correlation is computed as: 
//    corr(i,j) = Cov(i,j)/sqrt(Var(i)*Var(j)) where 'Cov(i,j)' is the sample covariance of channels  i and j and 'Var(i)' is the sample variance of channel i.
//    Variance and covariance are computed summing over all samples and all events for each channel: Cov(i,j)=[sum(x_i*x_j)-N*mean_i*mean_j]/(N-1) , where x_i is the single sample minus the pedestal.
//
// Correlation histograms are computed per FEB. The FEBs to be monitored are set in the JO.
//
// Available parameters in the jo file:
//   1) List of FEBs to be monitored: should be passed as a list of strings of the form 'BarrelCFT00Slot02'. If the list is empty, all FEBs are monitored.
//   2) control PublishAllFebsOnline: if it is set to true, switched off the monitoring in case the FEB list (1) is empty and the algorithm is running online.
//   3) list of triggers to be used ('TriggerChain'): to be passed as a single string "trigger_chain_1, trigger_chain_2". The default is "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY", for the latter, only events in the abort gap are used, selection done by hand.
//   4) control IsCalibrationRun: to be set to true when running on calibration, it switches off the trigger selection.
//   5) control PublishPartialSums: tells the algorithm to also publish the partial sum histograms. default is false.

//
// ********************************************************************


//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

//ROOT
#include "TPRegexp.h"


//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"

//for looping on FEBs
#include "LArRawEvent/LArFebHeaderContainer.h"

//Helper:
#include "LArMonitoring/LArStrHelper.h"

//Header:
#include "LArNoiseCorrelationMonAlg.h"






/*---------------------------------------------------------*/
LArNoiseCorrelationMonAlg::LArNoiseCorrelationMonAlg( const std::string& name, ISvcLocator* pSvcLocator) 
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(nullptr),
    m_badChannelMask("BadLArRawChannelMask",this)
{ 
  declareProperty("LArBadChannelMask",m_badChannelMask);
}

/*---------------------------------------------------------*/
LArNoiseCorrelationMonAlg::~LArNoiseCorrelationMonAlg()
{ }


/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMonAlg::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArNoiseCorrelationMonAlg" );  
  
  ATH_CHECK(detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ));

  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_keyPedestal.initialize());
  ATH_CHECK(m_LArDigitContainerKey.initialize());
  
  /** Get bad-channel mask (only if jO IgnoreBadChannels is true)*/
  if (m_ignoreKnownBadChannels) {
    ATH_CHECK(m_badChannelMask.retrieve());
  } else m_badChannelMask.disable();
  
  /** Configure event info */
  ATH_CHECK(m_eventInfoKey.initialize());

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  /*now the group*/
  m_noiseCorrGroups=Monitored::buildToolMap<int>(m_tools,"NoiseCorrRAW",m_FEBlist);

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMonAlg::fillHistograms(const EventContext& ctx) const
{
  if(m_plotsOFF) {
    ATH_MSG_DEBUG("Plotting switched off, either we are online and custom FEB list is empty, OR something went wrong with the custom list of FEBs passed");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("in fillHists()" ); 
    

  /** check trigger */
  bool passTrig = m_isCalibrationRun;
  if(!m_isCalibrationRun) { 
     /**EventID is a part of EventInfo, search event informations:*/
    SG::ReadHandle<xAOD::EventInfo> thisEvent{m_eventInfoKey,ctx};
     if (!thisEvent.isValid()) {
       ATH_MSG_ERROR("xAOD::EventInfo retrieval failed");
       return StatusCode::FAILURE;
     }
  
    const ToolHandle<Trig::TrigDecisionTool> trigTool=getTrigDecisionTool();

     bool passBCID;
     if(trigTool) {
       // BCIDs of the abort gap
       const int ABORT_GAP_START = 3446;
       const int ABORT_GAP_END   = 3563;
       for(auto trig_chain : m_vTrigChainNames) {
	passBCID = ((trig_chain == "HLT_noalg_cosmiccalo_L1RD1_EMPTY")?(thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END):true);
	passTrig=(passTrig || (passBCID && trigTool->isPassed(trig_chain)));
       }
     }
  }

  if (!passTrig) { 
      ATH_MSG_DEBUG ( " Failed trigger selection " );
      return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ( " Pass trigger selection " );
  }

  /*setup vectors for filling: want to use collactions and array or I'll call fill() tousands of times */
  std::map<std::string,std::vector<std::pair<int,double> > > meanMap;
  std::map<std::string,std::vector<std::pair<std::pair<int,int>,double> > > partSumMap;

  /*retrieve cabling*/
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR("Do not have cabling map with key: "<<m_cablingKey.key());
     return StatusCode::FAILURE;
  }

  /*retrieve pedestal*/
  SG::ReadCondHandle<ILArPedestal> pedestalHdl{m_keyPedestal,ctx};
  const ILArPedestal* pedestals=*pedestalHdl;

  /** retrieve LArDigitContainer*/
  SG::ReadHandle<LArDigitContainer> pLArDigitContainer{m_LArDigitContainerKey,ctx};
  
  ATH_MSG_DEBUG ( " LArDigitContainer size "<<pLArDigitContainer->size()<<" for key "<<m_LArDigitContainerKey); 
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_2;
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 

  /** helper for feb names*/
  LArStrHelper larStrHelp;

  /** Loop over digits*/
  for ( ; itDig!=itDig_e;++itDig) {
    const LArDigit* pLArDigit = *itDig;
    
    /** Retrieve pedestals */
    HWIdentifier id = pLArDigit->hardwareID();
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = pedestals->pedestal(id,gain);    

    if(!isGoodChannel(id,pedestal,cabling))
	continue;
    
    /** Retrieve samples*/
    const std::vector<short>* digito = &pLArDigit->samples();
    
    /** Retrieve once, all the cell info:*/
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    int ch1 = m_LArOnlineIDHelper->channel(id);

    std::string febSTR = larStrHelp.febNameString(m_LArOnlineIDHelper->isEMBchannel(id),m_LArOnlineIDHelper->pos_neg(id),m_LArOnlineIDHelper->feedthrough(id),m_LArOnlineIDHelper->slot(id));
      //febString(febID); 
    /** If we are only plotting a sub-set of FEBs, check if it belongs to that set */
    if(m_plotCustomFEBSset) {
      bool plotThisFEB=false;
      for(uint ifm=0;ifm<m_FEBlist.size();ifm++) {
	if(febSTR==m_FEBlist[ifm]) {
	  plotThisFEB=true;
	  break;
	}
      }
      if(!plotThisFEB) continue;
    }

    /** Second loop over digit */
    bool av_set=false;
    for(itDig_2 = itDig; itDig_2!=itDig_e;++itDig_2)
      {
	const LArDigit* pLArDigit2 = *itDig_2;
	HWIdentifier id2 = pLArDigit2->hardwareID();
	if(m_LArOnlineIDHelper->feb_Id(id2)!=febID) continue;

	/** get the pedestal */
	CaloGain::CaloGain gain2 = pLArDigit2->gain();
	float pedestal2 = pedestals->pedestal(id2,gain2);

	if(!isGoodChannel(id2,pedestal2,cabling))  continue;

	/** get the channel number */
	int ch2 = m_LArOnlineIDHelper->channel(id2);

	/** get the samples */
	const std::vector<short>* digito2 = &pLArDigit2->samples();
	std::vector<short>::const_iterator iterSam2= digito2->begin();
	std::vector<short>::const_iterator iterSam= digito->begin();

	double part_sum=0;
	int Nsam=pLArDigit->nsamples();
	if(pLArDigit2->nsamples()!=Nsam)
	  {
	    ATH_MSG_WARNING( Form("Different number of samples between channels %d vs %d: skipping these two",Nsam,pLArDigit2->nsamples()) );
	    continue;
	  }

	/** Loop over the samples and compute average and sum of squares*/
	for(int i=0;i<Nsam;i++,iterSam++,iterSam2++)
	  {
	    if(!av_set) { /** fill the mean only once per ch1. This code is here to avoid one additional loop over samples before the second loop. */
	      meanMap[febSTR].push_back(std::make_pair(ch1,(*iterSam-pedestal)));
	    }
	    /** now compute sum of squares */
	    part_sum+=((*iterSam-pedestal)*(*iterSam2-pedestal2));
	  }
	av_set=true; /** now the average is set and I won't do this again in next ch2 loop*/
	partSumMap[febSTR].push_back(std::make_pair(std::make_pair(ch1,ch2),part_sum));
      }/** End of second loop on LArDigit*/    
  }/** End of loop on LArDigit*/
 
  //now fill the plots
  for(std::map<std::string,std::vector<std::pair<int,double> > >::iterator feb_iter = meanMap.begin(); feb_iter != meanMap.end(); ++feb_iter) {
    std::string febid = feb_iter->first;
    auto chanMean = Monitored::Collection("chanMean",feb_iter->second,[](const std::pair<int,double> ch){return ch.second;});
    auto chanMeanX = Monitored::Collection("chanMeanX",feb_iter->second,[](const std::pair<int,double> ch){return ch.first;});
    auto chanPartSum = Monitored::Collection("chanPartSum",partSumMap[febid],[](const std::pair<std::pair<int,int>,double> ch){return ch.second;});
    auto chanPartSumX = Monitored::Collection("chanPartSumX",partSumMap[febid],[](const std::pair<std::pair<int,int>,double> ch){return std::min(ch.first.first,ch.first.second);});
    auto chanPartSumY = Monitored::Collection("chanPartSumY",partSumMap[febid],[](const std::pair<std::pair<int,int>,double> ch){return std::max(ch.first.first,ch.first.second);}); //needs max and min to fill the correlation plot always on the same side of the diagonal, otherwise it would be mixed up
  
    //fill the correct FEB
    fill(m_tools[m_noiseCorrGroups.at(febid)],chanMean,chanMeanX,chanPartSum,chanPartSumX,chanPartSumY);

    /* actual correlations will be computed at post-processing stage */

  }
  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
/** check if channel is ok for monitoring */
 bool LArNoiseCorrelationMonAlg::isGoodChannel(const HWIdentifier ID,const float ped, const LArOnOffIdMapping *cabling) const
 {
    /** Remove problematic channels*/
   if (m_ignoreKnownBadChannels && m_badChannelMask->cellShouldBeMasked(ID))
     return false;

    /**skip cells with no pedestals reference in db.*/
    if(ped <= (1.0+LArElecCalib::ERRORCODE))
      return false;
    
    /**skip disconnected channels:*/
    if(!cabling->isOnlineConnected(ID))
      return false;

    return true;
 }


/*---------------------------------------------------------*/
/** build the FEB string, following instructions from python config */
std::string LArNoiseCorrelationMonAlg::febString(const HWIdentifier afeb) const {
  std::string eb=m_LArOnlineIDHelper->isEMBchannel(afeb) ? "Barrel" : "Endcap";
  std::string ac=(m_LArOnlineIDHelper->pos_neg(afeb)==1) ? "A" : "C";
  int FT = m_LArOnlineIDHelper->feedthrough(afeb);
  int SL = m_LArOnlineIDHelper->slot(afeb);
  return eb+ac+Form("ft%02d",FT)+Form("slot%02d",SL);
}




