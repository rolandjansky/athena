/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
*/


// ********************************************************************
// NAME:     LArNoiseCorrelationMon.cxx
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
//   1) List of FEBs to be monitored: should be passed as a list of strings of the form 'BarrelCFT00Slot02', case insensitive (corresponding to type  LARONLINEID defined in the package atlas/LArCalorimeter/LArMonTools/LArMonTools/LArOnlineIDStrHelper.h). If the list is empty, all FEBs are monitored.
//   2) control PublishAllFebsOnline: if it is set to true, switched off the monitoring in case the FEB list (1) is empty and the algorithm is running online.
//   3) list of triggers to be used ('TriggerChain'): to be passed as a single string "trigger_chain_1, trigger_chain_2". The default is "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY", for the latter, only events in the abort gap are used, selection done by hand.
//   4) control IsCalibrationRun: to be set to true when running on calibration, it switches off the trigger selection.
//   5) control PublishPartialSums: tells the algorithm to also publish the partial sum histograms. default is false.

//
// ********************************************************************

//Histograms
#include "GaudiKernel/ITHistSvc.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TH2F_LW.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

//ROOT
#include "TPRegexp.h"
#include "TF1.h"

//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArOnlineIDStrHelper.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"

//for looping on FEBs
#include "LArRawEvent/LArFebHeaderContainer.h"

//Header:
#include "LArNoiseCorrelationMon.h"


// BCIDs of the abort gap
const int ABORT_GAP_START = 3446;
const int ABORT_GAP_END   = 3563;




/*---------------------------------------------------------*/
LArNoiseCorrelationMon::LArNoiseCorrelationMon(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_strHelper(nullptr),
    m_LArOnlineIDHelper(nullptr),
    m_badChannelMask("BadLArRawChannelMask")
{
  /** FEBs to be monitored. If empty, all FEBs will be monitored*/
  std::vector<std::string> empty_vector(0);
  declareProperty("FEBsToMonitor",m_FEBsToMonitor=empty_vector);
	
  /** If false, blocks the histogram booking in case it's running online and the 'm_FEBsToMonitor' list is empty. i.e. prevents the algorithm from publishing one histogram per FEB for all FEBs in online environnement */
  declareProperty("PublishAllFebsOnline",m_publishAllFebsOnline=true);
  declareProperty("PublishPartialSums",m_publishPartialSums=false);

  /** To be set to true when running on calibration run. It swithces off trigger selection*/
  declareProperty("IsCalibrationRun",m_isCalibrationRun=false);

   /**bool use to mask the bad channels*/
  declareProperty("IgnoreBadChannels", m_ignoreKnownBadChannels=false);
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "FREE");
  declareProperty("IsOnline",            m_IsOnline=false);

  /**bool enable summary plot */
  declareProperty("DoSummary",           m_DoSummary=true);
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
	      m_histos.second.first=nullptr;
	    }
	  if(m_histos.second.second)
	    {
	      LWHist::safeDelete(m_histos.second.second);
	      m_histos.second.second=nullptr;
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

  m_evtCounter=0;
  
  /** Get LAr Online Id Helper*/
  ATH_CHECK(detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ));

    
  /** Init cabling kay */
  ATH_CHECK(m_cablingKey.initialize());

  
  /** Get bad-channel mask (only if jO IgnoreBadChannels is true)*/
  if (m_ignoreKnownBadChannels) {
    ATH_CHECK(m_badChannelMask.retrieve());
  }
  
  /** Retrieve pedestals container*/
  ATH_CHECK(m_keyPedestal.initialize());

  /** get the trigger list from the 'm_triggerChainProp'*/
  m_triggers.clear();
  if(m_isCalibrationRun) {
    ATH_MSG_INFO( "Running as 'calibration run'. No trigger selection will be applied.");
  }
  else {
    ATH_MSG_DEBUG( "Parsing trigger chain list" );
      StatusCode sc=ManagedMonitorToolBase::parseList(m_triggerChainProp, m_triggers);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Error parsing the trigger chain list, exiting." );
      return StatusCode::FAILURE;
    }
  }

  return ManagedMonitorToolBase::initialize();
}

/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::bookHistograms()
{
    /** lar noise correlation */
    m_Nchan=128;
    m_chan_low=-0.5;
    m_chan_up=127.5;

    MonGroup GroupEMBA( this, "/LAr/NoiseCorrelation/EMBA", run, ATTRIB_MANAGED );
    MonGroup GroupEMBC( this, "/LAr/NoiseCorrelation/EMBC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECA( this, "/LAr/NoiseCorrelation/EMECA", run, ATTRIB_MANAGED );
    MonGroup GroupEMECC( this, "/LAr/NoiseCorrelation/EMECC", run, ATTRIB_MANAGED );
    MonGroup GroupHECA( this, "/LAr/NoiseCorrelation/HECA", run, ATTRIB_MANAGED );
    MonGroup GroupHECC( this, "/LAr/NoiseCorrelation/HECC", run, ATTRIB_MANAGED );
    MonGroup GroupFCALA( this, "/LAr/NoiseCorrelation/FCALA", run, ATTRIB_MANAGED );
    MonGroup GroupFCALC( this, "/LAr/NoiseCorrelation/FCALC", run, ATTRIB_MANAGED );

    /**declare strings for histograms title*/
    m_hist_name = "NoiseCorr_"; 
    m_hist_summary_name1 = "SummaryPlot_maximumCorr"; 
    m_hist_summary_name2 = "SummaryPlot_maximumCorr_sigma"; 
    m_hist_title = "LAr Noise Correlation";
    m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
    m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);

    /** Check which (if any) FEBs we want to monitor*/ 
    if(m_FEBsToMonitor.size()==0) {
      if(!m_publishAllFebsOnline && m_IsOnline) {
	ATH_MSG_INFO("'m_publishAllFebsOnline' set to false: cannot publish all FEBs in online run. Do nothing.");
	return StatusCode::SUCCESS;
      }
      bookAllFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
    }
    else bookSelectedFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::fillHistograms()
{
  /** if publishing all FEBs has been forbidden and m_FEBsToMonitor is empty, then no histogram will be published anyway: no need to loop over channels */
  if(!m_publishAllFebsOnline && m_IsOnline && m_FEBsToMonitor.size()==0) {
      ATH_MSG_DEBUG("'m_publishAllFebsOnline' set to false and empty FEB list in online run. No FEB will be monitored.");
      return StatusCode::SUCCESS;
    }

  ATH_MSG_DEBUG("in fillHists()" );
    
  /**increase internal event counter, for calibration run only */
  m_evtCounter++;

  /** check trigger */
  bool passTrig = m_isCalibrationRun;
  if(!m_isCalibrationRun) { 
     /**EventID is a part of EventInfo, search event informations:*/
     const xAOD::EventInfo* thisEvent;
     ATH_CHECK(evtStore()->retrieve(thisEvent));
  
     m_evtId = thisEvent->eventNumber();
     ATH_MSG_DEBUG("Event nb " << m_evtId );  

    bool passBCID;
    if(!m_trigDecTool.empty()) {
      for(auto trig_chain : m_triggers) {
	passBCID = ((trig_chain == "HLT_noalg_cosmiccalo_L1RD1_EMPTY")?(thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END):true);
	passTrig=(passTrig || (passBCID && m_trigDecTool->isPassed(trig_chain)));
      }
    }
  }

  if (!passTrig) {
      ATH_MSG_DEBUG ( " Failed trigger selection " );
      if(m_IsOnline && (endOfLumiBlockFlag() || endOfEventsBlockFlag())) fillInCorrelations(); //seems not to work offline, so done for online only
      return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ( " Pass trigger selection " );
  }

  /*retrieve cabling*/
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR("Do not have cabling map with key: "<<m_cablingKey.key());
     return StatusCode::FAILURE;
  }
  

  /*retrieve pedestal*/
  SG::ReadCondHandle<ILArPedestal> pedestalHdl{m_keyPedestal};
  const ILArPedestal* pedestals=*pedestalHdl;

  /** retrieve LArDigitContainer*/
  const LArDigitContainer* pLArDigitContainer=nullptr;
  ATH_CHECK(evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey));
  
  
  ATH_MSG_DEBUG ( " LArDigitContainer size "<<pLArDigitContainer->size()<<" for key "<<m_LArDigitContainerKey); 
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_2;
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  
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
	const LArDigit* pLArDigit2 = *itDig_2;
	HWIdentifier id2 = pLArDigit2->hardwareID();
	if(m_LArOnlineIDHelper->feb_Id(id2)!=m_febID) continue;

	/** get the pedestal */
	CaloGain::CaloGain gain2 = pLArDigit2->gain();
	float pedestal2 = pedestals->pedestal(id2,gain2);

	if(!isGoodChannel(id2,pedestal2,cabling))  continue;

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
 

  /** compute correlation and update the published plot once per LB (seems not to work offline, so done for online only)*/
  if(m_IsOnline && (endOfLumiBlockFlag() || endOfEventsBlockFlag())) fillInCorrelations();
 
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArNoiseCorrelationMon::procHistograms()
{
  if(endOfRunFlag()) 
    {
      fillInCorrelations();
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
 bool LArNoiseCorrelationMon::isGoodChannel(const HWIdentifier ID,const float ped, const LArOnOffIdMapping *cabling) const
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
/**Compute correlations and fill in the plots*/
void LArNoiseCorrelationMon::fillInCorrelations()
{
  double mean1,mean2;
  double sumVar1,sumVar2;
  double N;
  double cor;
  for (auto const& feb_entry : m_FEBhistograms)
    {
      double tmp_maxcorr = 0.0;
      TH1F *hist_tmp_correlation;
      hist_tmp_correlation         = new TH1F("Tmp_correlation_value", "TMP correlation value", 40, -1.0, 1.0 );
      double tmp_RMS  = 0.0;
      m_histos=feb_entry.second;
      for(int i=1;i<=m_Nchan;i++)
	{
	  mean1=m_histos.second.second->GetBinContent(i);
	  sumVar1=m_histos.second.first->GetBinContent(i,i);
	  N=m_histos.second.second->GetBinEntries(i);
	  if(N==0) 
	    {
	      ATH_MSG_DEBUG( "Bin " << i << " has 0 entries" );
	      continue;
	    }
	  for(int j=i+1;j<=m_Nchan;j++)
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
	      /** calculate maximum correlation */
	      /** First variable: maximum correlation in the FEB */
	      if ( tmp_maxcorr < std::abs(cor) )
		tmp_maxcorr = std::abs(cor);
	      /** Second valiable: RMS value in correlation distribution */
	      hist_tmp_correlation->Fill(cor);
	    }
	}

      tmp_RMS = hist_tmp_correlation->GetRMS();

      // Fill in summary histogram
      TPMERegexp tmp('_');
      int nsplit = tmp.Split( m_histos.first->GetTitle() );
      if(m_DoSummary){
	if(nsplit > 1){
	  // EMB
	  if(m_LArOnlineIDHelper->isEMBchannel( feb_entry.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first) ){
	      m_h_summary_plot1_EMBA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_maxcorr);
	      m_h_summary_plot2_EMBA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_RMS);
	    }
	    else{
	      m_h_summary_plot1_EMBC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_maxcorr);
	      m_h_summary_plot2_EMBC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_RMS);
	    }
	  }
	  // EMEC
	  else if(m_LArOnlineIDHelper->isEMECchannel( feb_entry.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first)){
	      m_h_summary_plot1_EMECA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_maxcorr);
	      m_h_summary_plot2_EMECA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_RMS);
	    }
	    else{
	      m_h_summary_plot1_EMECC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_maxcorr);
	      m_h_summary_plot2_EMECC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_RMS);
	    }
	  }
	  // HEC
	  else if(m_LArOnlineIDHelper->isHECchannel( feb_entry.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first)){
	      m_h_summary_plot1_HECA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_maxcorr );
	      m_h_summary_plot2_HECA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_RMS );
	    }
	    else{
	      m_h_summary_plot1_HECC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_maxcorr );
	      m_h_summary_plot2_HECC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1,  tmp_RMS );
	    }
	  }
	  // FCAL
	  else if(m_LArOnlineIDHelper->isFCALchannel( feb_entry.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first)){
	      m_h_summary_plot1_FCALA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_maxcorr );
	      m_h_summary_plot2_FCALA->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_RMS );
	    }
	    else{
	      m_h_summary_plot1_FCALC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_maxcorr );
	      m_h_summary_plot2_FCALC->SetBinContent( atoi(tmp[0])+1, atoi(tmp[1])+1, tmp_RMS );
	    }
	  }
	}
      }
      // Delete them in order to save the memory
      delete hist_tmp_correlation;

    }
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
  // summary plot
  if(m_DoSummary){
    m_h_summary_plot1_EMBA     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMBA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMBC     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMBC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMECA    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMECA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMECC    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMECC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_HECA     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot HECA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_HECC     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot HECC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_FCALA    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot FCALA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_FCALC    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot FCALC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMBA     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMBA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMBC     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMBC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMECA    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMECA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMECC    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMECC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_HECA     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot HECA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_HECC     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot HECC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_FCALA    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot FCALA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_FCALC    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot FCALC" ,16, 0, 16, 34, 0, 34);
    grEMBA.regHist(m_h_summary_plot1_EMBA).ignore();  
    grEMBC.regHist(m_h_summary_plot1_EMBC).ignore();  
    grEMECA.regHist(m_h_summary_plot1_EMECA).ignore();  
    grEMECC.regHist(m_h_summary_plot1_EMECC).ignore();  
    grHECA.regHist(m_h_summary_plot1_HECA).ignore();  
    grHECC.regHist(m_h_summary_plot1_HECC).ignore();  
    grFCALA.regHist(m_h_summary_plot1_FCALA).ignore();  
    grFCALC.regHist(m_h_summary_plot1_FCALC).ignore();  
    grEMBA.regHist(m_h_summary_plot2_EMBA).ignore();  
    grEMBC.regHist(m_h_summary_plot2_EMBC).ignore();  
    grEMECA.regHist(m_h_summary_plot2_EMECA).ignore();  
    grEMECC.regHist(m_h_summary_plot2_EMECC).ignore();  
    grHECA.regHist(m_h_summary_plot2_HECA).ignore();  
    grHECC.regHist(m_h_summary_plot2_HECC).ignore();  
    grFCALA.regHist(m_h_summary_plot2_FCALA).ignore();  
    grFCALC.regHist(m_h_summary_plot2_FCALC).ignore();  
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
  // summary plot
  if(m_DoSummary){
    m_h_summary_plot1_EMBA     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMBA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMBC     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMBC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMECA    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMECA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_EMECC    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot EMECC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_HECA     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot HECA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_HECC     = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot HECC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_FCALA    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot FCALA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot1_FCALC    = TH2F_LW::create(m_hist_summary_name1.c_str(), "Summary1 plot FCALC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMBA     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMBA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMBC     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMBC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMECA    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMECA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_EMECC    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot EMECC" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_HECA     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot HECA"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_HECC     = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot HECC"  ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_FCALA    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot FCALA" ,16, 0, 16, 34, 0, 34);
    m_h_summary_plot2_FCALC    = TH2F_LW::create(m_hist_summary_name2.c_str(), "Summary2 plot FCALC" ,16, 0, 16, 34, 0, 34);

    grEMBA.regHist(m_h_summary_plot1_EMBA).ignore();  
    grEMBC.regHist(m_h_summary_plot1_EMBC).ignore();  
    grEMECA.regHist(m_h_summary_plot1_EMECA).ignore();  
    grEMECC.regHist(m_h_summary_plot1_EMECC).ignore();  
    grHECA.regHist(m_h_summary_plot1_HECA).ignore();  
    grHECC.regHist(m_h_summary_plot1_HECC).ignore();  
    grFCALA.regHist(m_h_summary_plot1_FCALA).ignore();  
    grFCALC.regHist(m_h_summary_plot1_FCALC).ignore();  

    grEMBA.regHist(m_h_summary_plot2_EMBA).ignore();  
    grEMBC.regHist(m_h_summary_plot2_EMBC).ignore();  
    grEMECA.regHist(m_h_summary_plot2_EMECA).ignore();  
    grEMECC.regHist(m_h_summary_plot2_EMECC).ignore();  
    grHECA.regHist(m_h_summary_plot2_HECA).ignore();  
    grHECC.regHist(m_h_summary_plot2_HECC).ignore();  
    grFCALA.regHist(m_h_summary_plot2_FCALA).ignore();  
    grFCALC.regHist(m_h_summary_plot2_FCALC).ignore();  

  }
}

/*---------------------------------------------------------*/
/**Book and defines histogramms for a given FEB.*/
void LArNoiseCorrelationMon::bookThisFEB(HWIdentifier id,MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  std::string this_name=m_strHelper->feb_str(id);
  int num_slot          = m_LArOnlineIDHelper->slot(id);
  int num_feedthrough   = m_LArOnlineIDHelper->feedthrough(id);
  std::string slot_feed_title = std::to_string(num_slot) + "_" + std::to_string(num_feedthrough);

  TH2F_LW* h_corr = TH2F_LW::create((m_hist_name+this_name).c_str(), slot_feed_title.c_str(), m_Nchan,m_chan_low,m_chan_up,m_Nchan,m_chan_low,m_chan_up);
  TH2F_LW* h_TMP_sums = TH2F_LW::create((m_hist_name+this_name+"_TMP_sum").c_str(),(m_hist_title+" TMP sum").c_str(),m_Nchan,m_chan_low,m_chan_up,m_Nchan,m_chan_low,m_chan_up);
  TProfile_LW* h_av = TProfile_LW::create((m_hist_name+this_name+"_TMP_av").c_str(),(m_hist_title+" TMP av").c_str(),m_Nchan,m_chan_low,m_chan_up,"s");
  m_FEBhistograms[id]=std::make_pair(h_corr,std::make_pair(h_TMP_sums,h_av));

  if(m_LArOnlineIDHelper->isEMBchannel(id)) {
    if(m_LArOnlineIDHelper->pos_neg(id)==1) {
      grEMBA.regHist(h_corr).ignore();
      if(m_publishPartialSums && !m_IsOnline) {
	grEMBA.regHist(h_av).ignore();
	grEMBA.regHist(h_TMP_sums).ignore();
      }
    }
    else {
      grEMBC.regHist(h_corr).ignore();
      if(m_publishPartialSums && !m_IsOnline) {
	grEMBC.regHist(h_av).ignore();
	grEMBC.regHist(h_TMP_sums).ignore();
      }
    }
  }
  else
    if(m_LArOnlineIDHelper->isEMECchannel(id)) {
      if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	grEMECA.regHist(h_corr).ignore();
	if(m_publishPartialSums && !m_IsOnline) {
	  grEMECA.regHist(h_av).ignore();
	  grEMECA.regHist(h_TMP_sums).ignore();
	}
      }
      else {
	grEMECC.regHist(h_corr).ignore();
      if(m_publishPartialSums && !m_IsOnline) {
	grEMECC.regHist(h_av).ignore();
	grEMECC.regHist(h_TMP_sums).ignore();
      }
      }
    }
    else
      if(m_LArOnlineIDHelper->isHECchannel(id)) {
	if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	  grHECA.regHist(h_corr).ignore();
	  if(m_publishPartialSums && !m_IsOnline) {
	    grHECA.regHist(h_av).ignore();
	    grHECA.regHist(h_TMP_sums).ignore();
	  }
	}
	else {
	  grHECC.regHist(h_corr).ignore();
	  if(m_publishPartialSums && !m_IsOnline) {
	    grHECC.regHist(h_av).ignore();
	    grHECC.regHist(h_TMP_sums).ignore();
	  }
	}
      }
      else
	if(m_LArOnlineIDHelper->isFCALchannel(id)) {
	  if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	    grFCALA.regHist(h_corr).ignore();
	    if(m_publishPartialSums && !m_IsOnline) {
	      grFCALA.regHist(h_av).ignore();
	      grFCALA.regHist(h_TMP_sums).ignore();
	    }
	  }
	  else {
	    grFCALC.regHist(h_corr).ignore();
	    if(m_publishPartialSums && !m_IsOnline) {
	      grFCALC.regHist(h_av).ignore();
	      grFCALC.regHist(h_TMP_sums).ignore();
	    }
	  }
	}
}




