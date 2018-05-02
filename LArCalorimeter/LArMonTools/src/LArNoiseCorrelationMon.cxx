/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//     TBD: UPDATE THIS
// NAME:     LArNoiseCorrelationMon.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Helary Louis (helary@lapp.in2p3.fr)
//           Originaly From LArOddCellsMonTool.cxx  by Benjamin Trocme
// 
// Monitor a few things in the LArDigit...
//
//	1) Check that the highest value of the LArDigit is contained in an interval. 
//         If it is not the case increment 3 histograms for each subdetector:
//	      a) Out of range histograms
//	      b) The average histograms: give the average value of the highest digit sample 
//	      c) Channel VS FEB histograms: gives wich slot on wich FEB has his highest digit sample ou of the range
//	2) Check if a digits samples are in saturation. If it's the case increment the saturation histograms. 
//
// Available cuts in the jo file:
//
//   a) SampleRangeLow-SampleRangeUp: range to check the digit sample.
//   b) ADCcut : To select Digits Samples with signal.
//   c) ADCsature: lowest value to check if a Digit sample is in saturation.
// ********************************************************************

//Histograms
#include "GaudiKernel/ITHistSvc.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"

#include "LArTrigStreamMatching.h"

//for looping on FEBs
#include "LArRawEvent/LArFebHeaderContainer.h"

//Header:
#include "LArMonTools/LArNoiseCorrelationMon.h"

//

// BCIDs of the abort gap
const int ABORT_GAP_START = 3446;
const int ABORT_GAP_END   = 3563;




/*---------------------------------------------------------*/
LArNoiseCorrelationMon::LArNoiseCorrelationMon(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_strHelper(0),
    m_LArOnlineIDHelper(0),
    m_badChannelMask("BadLArRawChannelMask"),
    m_thisTrigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{	
  declareProperty("silenceMonitoringOnline",m_silenceMonitoringOnline=false);
  /** FEBs to be monitored*/
  std::vector<std::string> empty_vector(0);
  declareProperty("FEBsToMonitor",m_FEBsToMonitor=empty_vector);
   /**bool use to mask the bad channels*/
  declareProperty("IgnoreBadChannels", m_ignoreKnownBadChannels=false);
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("LArPedestalKey", m_larPedestalKey="Pedestal");
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "FREE");
  declareProperty("IsOnline",            m_IsOnline=false);
}

/*---------------------------------------------------------*/
LArNoiseCorrelationMon::~LArNoiseCorrelationMon()
{
}

StatusCode 
LArNoiseCorrelationMon::finalize()
{
  ATH_MSG_INFO( "finalHists LArNoiseCorrelationMon" );

  /** delete local histograms. */
  if(m_IsOnline)
    {
      for (auto const& feb_entry : m_FEBhistograms)
	{
	  m_histos=feb_entry.second;
	  if(m_histos.second.first)
	    {
	      LWHist::safeDelete(m_histos.second.first);
	      m_histos.second.first=0;
	    }
	  if(m_histos.second.second)
	    {
	      LWHist::safeDelete(m_histos.second.second);
	      m_histos.second.second=0;
	    }
	}
    }
  return StatusCode::SUCCESS;
 
}

/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArNoiseCorrelationMon" );
  
  StatusCode sc;
  
  /** Get LAr Online Id Helper*/
  if ( detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess() ) {
    
    ATH_MSG_DEBUG("connected non-tool: LArOnlineID" );
    
  } else {
    
    ATH_MSG_FATAL( "unable to connect non-tool: LArOnlineID" );
    return StatusCode::FAILURE;
    
  }
    
  /** Get LAr Calbling Service*/
  sc=m_LArCablingService.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArCablingService" );
    return StatusCode::FAILURE;
  }
  
  /** Get bad-channel mask (only if jO IgnoreBadChannels is true)*/
  if (m_ignoreKnownBadChannels) { 
    sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
      return StatusCode::FAILURE;
    }
  }
  
  /** Retrieve pedestals container*/
  sc =  detStore()->regHandle(m_larPedestal,m_larPedestalKey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "could not register handle for pedestal " );
    return StatusCode::FAILURE;
  }
    
  /** get trigger decision tool*/ 
    if( !m_thisTrigDecTool.empty() ) {
      ATH_CHECK( m_thisTrigDecTool.retrieve() );
     ATH_MSG_INFO ( "  --> Found AlgTool TrigDecisionTool" );
     }


  return ManagedMonitorToolBase::initialize();
}

/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::bookHistograms()
{
    /** lar noise correlation */
    Nchan=128;
    chan_low=-0.5;
    chan_up=127.5;

    MonGroup GroupEMBA( this, "/LAr/NoiseCorrel/EMBA", run, ATTRIB_MANAGED );
    MonGroup GroupEMBC( this, "/LAr/NoiseCorrel/EMBC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECA( this, "/LAr/NoiseCorrel/EMECA", run, ATTRIB_MANAGED );
    MonGroup GroupEMECC( this, "/LAr/NoiseCorrel/EMECC", run, ATTRIB_MANAGED );
    MonGroup GroupHECA( this, "/LAr/NoiseCorrel/HECA", run, ATTRIB_MANAGED );
    MonGroup GroupHECC( this, "/LAr/NoiseCorrel/HECC", run, ATTRIB_MANAGED );
    MonGroup GroupFCALA( this, "/LAr/NoiseCorrel/FCALA", run, ATTRIB_MANAGED );
    MonGroup GroupFCALC( this, "/LAr/NoiseCorrel/FCALC", run, ATTRIB_MANAGED );

    /**declare strings for histograms title*/
    std::string  hist_name = ""; //SEEMS NOT TO BE WORKING TO CHECK
    std::string hist_title = "LAr Noise Correlation";
    m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
    m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);

    /** Check which (if any) FEBs we want to monitor*/ 
    if(m_silenceMonitoringOnline && m_IsOnline) {
      ATH_MSG_INFO("Noise correlation monitoring is stopped for online run. Do nothing.");
      return StatusCode::SUCCESS;
    }
    if(m_FEBsToMonitor.size()==0) bookAllFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
    else bookSelectedFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::fillHistograms()
{
  /** if this monitoring has been silenced, then no need to loop over histograms */
  if(m_silenceMonitoringOnline && m_IsOnline) {
      ATH_MSG_DEBUG("Noise correlation monitoring is stopped for online run. Do nothing.");
      return StatusCode::SUCCESS;
    }

  ATH_MSG_DEBUG("in fillHists()" );
    

  /**EventID is a part of EventInfo, search event informations:*/
  const xAOD::EventInfo* thisEvent;
  if (evtStore()->retrieve(thisEvent).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }
  
  m_evtId = thisEvent->eventNumber();
  ATH_MSG_DEBUG("Event nb " << m_evtId );  

  /** check trigger */
  bool passTrig = false; 
  if ( (m_triggerChainProp != "")  && (!m_thisTrigDecTool.empty()) ) {
    passTrig = m_thisTrigDecTool->isPassed(m_triggerChainProp);
    /* For "HLT_noalg_cosmiccalo_L1RD1_EMPTY", we need to manually select abort gap*/
    if (m_triggerChainProp == "HLT_noalg_cosmiccalo_L1RD1_EMPTY") passTrig=(passTrig && thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END); 
  }
  if (!passTrig) {
    ATH_MSG_DEBUG ( " Failed trigger selection " );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG ( " Pass trigger selection " );
  }


  /** retrieve LArDigitContainer*/
  const LArDigitContainer* pLArDigitContainer;
  StatusCode sc = evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArDigitContainer with key " 
		      << m_LArDigitContainerKey );
    return StatusCode::SUCCESS;
  }
  
  
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_2;
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  const LArDigit* pLArDigit;
  const LArDigit* pLArDigit2;  
  
  /** Loop over digits*/
  for ( ; itDig!=itDig_e;++itDig) {
    pLArDigit = *itDig;
    
    /** Retrieve pedestals */
    HWIdentifier id = pLArDigit->hardwareID();
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = m_larPedestal->pedestal(id,gain);
    
    if(!isGoodChannel(id,pedestal))
	continue;
    
    /** Retrieve samples*/
    const std::vector<short>* digito = &pLArDigit->samples();
    
    /** Retrieve once, all the cell info:*/
    m_febID = m_LArOnlineIDHelper->feb_Id(id);
    m_ch1 = m_LArOnlineIDHelper->channel(id);

    try {
      m_histos=m_FEBhistograms.at(m_febID);
    }
    catch (const std::out_of_range& oor) {
	continue;
    }

    /** Second loop over digit */
    bool av_set=false;
    for(itDig_2 = itDig; itDig_2!=itDig_e;++itDig_2)
      {
	pLArDigit2 = *itDig_2;
	HWIdentifier id2 = pLArDigit2->hardwareID();
	if(m_LArOnlineIDHelper->feb_Id(id2)!=m_febID) continue;

	/** get the pedestal */
	CaloGain::CaloGain gain2 = pLArDigit2->gain();
	float pedestal2 = m_larPedestal->pedestal(id2,gain2);

	if(!isGoodChannel(id2,pedestal2))  continue;

	/** get the channel number */
	m_ch2 = m_LArOnlineIDHelper->channel(id2);

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
	      m_histos.second.second->Fill(m_ch1,(*iterSam-pedestal));
	    }
	    /** now compute sum of squares */
	    part_sum+=((*iterSam-pedestal)*(*iterSam2-pedestal2));
	  }
	av_set=true; /** now the average is set and I won't do this again in next ch2 loop*/
	m_histos.second.first->Fill(m_ch1,m_ch2,part_sum);
	m_histos.second.first->Fill(m_ch2,m_ch1,part_sum);
      }/** End of second loop on LArDigit*/    
  }/** End of loop on LArDigit*/
 

  /** compute correlation and update the published plot once per LB */
  if(endOfLumiBlockFlag() || endOfEventsBlockFlag())
    {
      double mean1,mean2;
      double sumVar1,sumVar2;
      double N;
      double cor;
      for (auto const& feb_entry : m_FEBhistograms)
	{
	  m_histos=feb_entry.second;
	  for(int i=1;i<=Nchan;i++)
	    {
	      mean1=m_histos.second.second->GetBinContent(i);
	      sumVar1=m_histos.second.first->GetBinContent(i,i);
	      N=m_histos.second.second->GetBinEntries(i);
	      if(N==0) 
		{
		  ATH_MSG_DEBUG( "Bin " << i << " has 0 entries" );
		  continue;
		}
	      for(int j=i+1;j<=Nchan;j++)
		{
		  mean2=m_histos.second.second->GetBinContent(j);
		  sumVar2=m_histos.second.first->GetBinContent(j,j);
		  if((sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2)==0) {
		    ATH_MSG_DEBUG( "sigma_i*sigma_j is zero for bin (" << i << "," << j << "). Correlation cannot be computed." );
		    continue;
		  }
		  cor=(m_histos.second.first->GetBinContent(i,j)-N*mean1*mean2)/TMath::Sqrt((sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2));
		  m_histos.first->SetBinContent(i,j,cor);
		  m_histos.first->SetBinContent(j,i,cor);
		}
	    }
	}
    }
 
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArNoiseCorrelationMon::procHistograms()
{
  if(endOfRunFlag()) 
    {
      if(m_IsOnline)
	{
	  for (auto const& feb_entry : m_FEBhistograms)
	    {
	      m_histos=feb_entry.second;
	      if(m_histos.second.first) m_histos.second.first->Reset(); 
	      if(m_histos.second.second) m_histos.second.second->Reset(); 
	    }
	}
    }
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
/** check if channel is ok for monitoring */
 bool LArNoiseCorrelationMon::isGoodChannel(HWIdentifier ID,float ped)
 {
    /** Remove problematic channels*/
   if (m_ignoreKnownBadChannels && m_badChannelMask->cellShouldBeMasked(ID))
     return false;

    /**skip cells with no pedestals reference in db.*/
    if(ped <= (1.0+LArElecCalib::ERRORCODE))
      return false;
    
    /**skip disconnected channels:*/
    if(!m_LArCablingService->isOnlineConnected(ID))
      return false;

    return true;
 }


/*---------------------------------------------------------*/
/**Loops on selected FEBS to book and define histograms.*/
void LArNoiseCorrelationMon::bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  ATH_MSG_INFO( "Booking selected FEBs: " << m_FEBsToMonitor.size() << " provided." );
  std::string aFEB; /**aFEB is the feb to monitor*/
  HWIdentifier febid;
  for(uint feb_i=0;feb_i<m_FEBsToMonitor.size();feb_i++) {
    aFEB=m_FEBsToMonitor[feb_i];
    febid=m_strHelper->feb_id(aFEB);
    if(!febid.is_valid()) {
      ATH_MSG_WARNING( "FEB id " << aFEB << " not valid. It will not be monitored." );
      continue;
    }
    bookThisFEB(febid,grEMBA,grEMBC,grEMECA,grEMECC,grHECA,grHECC,grFCALA,grFCALC);
  }
}


/*---------------------------------------------------------*/
/**Loops on all FEBS to book and define histograms.*/
void LArNoiseCorrelationMon::bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  ATH_MSG_INFO( "No selected FEBs provided: booking all FEBs.");
  
  /** loop on FEBs to init histograms */
  std::vector<HWIdentifier>::const_iterator feb_it = m_LArOnlineIDHelper->feb_begin();
  std::vector<HWIdentifier>::const_iterator feb_it_e = m_LArOnlineIDHelper->feb_end();
  for ( ; feb_it!=feb_it_e;++feb_it) {
    bookThisFEB((*feb_it),grEMBA,grEMBC,grEMECA,grEMECC,grHECA,grHECC,grFCALA,grFCALC);
  }
}

/*---------------------------------------------------------*/
/**Book and defines histogramms for a given FEB.*/
void LArNoiseCorrelationMon::bookThisFEB(HWIdentifier id,MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  std::string this_name=m_strHelper->feb_str(id);
  std::cout << ":) " << (hist_name+this_name).c_str() << std::endl; //this we need to understand why it's not getting the histogram name
  TH2F_LW* h_corr = TH2F_LW::create((hist_name+this_name).c_str(), hist_title.c_str(),Nchan,chan_low,chan_up,Nchan,chan_low,chan_up);
  TH2F_LW* h_TMP_sums = TH2F_LW::create((hist_name+this_name+"_TMP_sum").c_str(),(hist_title+" TMP sum").c_str(),Nchan,chan_low,chan_up,Nchan,chan_low,chan_up);
  TProfile_LW* h_av = TProfile_LW::create((hist_name+this_name+"_TMP_av").c_str(),(hist_title+" TMP av").c_str(),Nchan,chan_low,chan_up,"s");
  m_FEBhistograms[id]=std::make_pair(h_corr,std::make_pair(h_TMP_sums,h_av));

  if(m_LArOnlineIDHelper->isEMBchannel(id)) {
    if(m_LArOnlineIDHelper->pos_neg(id)==1) grEMBA.regHist(h_corr).ignore();
    else grEMBC.regHist(h_corr).ignore();
  }
  else
    if(m_LArOnlineIDHelper->isEMECchannel(id)) {
      if(m_LArOnlineIDHelper->pos_neg(id)==1) grEMECA.regHist(h_corr).ignore();
      else grEMECC.regHist(h_corr).ignore();
    }
    else
      if(m_LArOnlineIDHelper->isHECchannel(id)) {
	if(m_LArOnlineIDHelper->pos_neg(id)==1) grHECA.regHist(h_corr).ignore();
	else grHECC.regHist(h_corr).ignore();
      }
      else
	if(m_LArOnlineIDHelper->isFCALchannel(id)) {
	  if(m_LArOnlineIDHelper->pos_neg(id)==1) grFCALA.regHist(h_corr).ignore();
	  else grFCALC.regHist(h_corr).ignore();
	}
}




