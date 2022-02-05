/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     LArCoherentNoisefractionMon.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   pavol Strizenec, based on codes from M. Spalla and Y. Enari
// 
// Computes and plots the coherent noise fraction (CNF) for some group of the channels on the FEB
//
// CNF are computed per FEB. The FEBs to be monitored are set in the JO.
//
// Available parameters in the jo file:
//   1) List of FEBs to be monitored: should be passed as a list of strings of the form 'BarrelCFT00Slot02', case insensitive (corresponding to type  LARONLINEID defined in the package atlas/LArCalorimeter/LArMonTools/LArMonTools/LArOnlineIDStrHelper.h). If the list is empty, all FEBs are monitored.
//   2) list of triggers to be used ('TriggerChain'): to be passed as a single string "trigger_chain_1, trigger_chain_2". The default is "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY", for the latter, only events in the abort gap are used, selection done by hand.
//   3) control IsCalibrationRun: to be set to true when running on calibration, it switches off the trigger selection.
//   4) list of groups to be computed, vector of strings, possible values: ["tot","top","bot","left","right","q1","q2","q3","q4","qs1","qs2","qs3","qs4"] - default is empty list, which means all groups are computed
//
// ********************************************************************

//Histograms
#include "GaudiKernel/ITHistSvc.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1F_LW.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

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
#include "LArCoherentNoisefractionMon.h"


// BCIDs of the abort gap
const int ABORT_GAP_START = 3446;
const int ABORT_GAP_END   = 3563;

static const std::vector<std::string>  groupNames={"total","top","bottom","left","right","quad1","quad2","quad3","quad4","interval1","interval2","interval3","interval4"};
static const std::vector<std::string>  groupIn={"tot","top","bot","left","right","q1","q2","q3","q4","qs11","qs22","qs3","qs4"};
static const std::vector<unsigned> groupNChan={128,64,64,64,64,32,32,32,32,32,32,32,32};

/*---------------------------------------------------------*/
LArCoherentNoisefractionMon::LArCoherentNoisefractionMon(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_strHelper(nullptr),
    m_LArOnlineIDHelper(nullptr),
    m_nGroups(0)
{
  /** FEBs to be monitored. If empty, all FEBs will be monitored*/
  std::vector<std::string> empty_vector(0);
  declareProperty("FEBsToMonitor",m_FEBsToMonitor=empty_vector);
	
  /** To be set to true when running on calibration run. It swithces off trigger selection*/
  declareProperty("IsCalibrationRun",m_isCalibrationRun=false);

  /** list of the groups to compute **/
  declareProperty("ListOfGroups", m_groupNamesToMonitor={});

   /**bool use to mask the bad channels*/
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "FREE");
  declareProperty("IsOnline",            m_IsOnline=false);
}

/*---------------------------------------------------------*/
LArCoherentNoisefractionMon::~LArCoherentNoisefractionMon()
{
}

StatusCode 
LArCoherentNoisefractionMon::finalize()
{
  ATH_MSG_INFO( "finalHists LArCoherentNoisefractionMon" );

  /** delete tmp histograms. */
  if(m_IsOnline)
    {
      for (auto const& feb_entry : m_FEBhistograms)
	{
	  auto histos=feb_entry.second;
	  if(histos.first)
	    {
	      LWHist::safeDelete(&*(histos.first));
	      histos.first=nullptr;
	    }
	  if(histos.second)
	    {
	      LWHist::safeDelete(histos.second);
	      histos.second=nullptr;
	    }
	}
    }

  delete m_strHelper;
  return StatusCode::SUCCESS;
 
}

/*---------------------------------------------------------*/
StatusCode 
LArCoherentNoisefractionMon::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArCoherentNoisefractionMon" );

  m_evtCounter=0;
  
  /** Get LAr Online Id Helper*/
  ATH_CHECK(detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ));

    
  /** Init cabling kay */
  ATH_CHECK(m_cablingKey.initialize());
  
  /** Retrieve pedestals container*/
  ATH_CHECK(m_keyPedestal.initialize());

  /** get the trigger list from the 'm_triggerChainProp'*/
  m_triggers.clear();
  if(m_isCalibrationRun) {
    ATH_MSG_INFO( "Running as 'calibration run'. No trigger selection will be applied...");
  } else {
    ATH_MSG_DEBUG( "Parsing trigger chain list" );
      StatusCode sc=ManagedMonitorToolBase::parseList(m_triggerChainProp, m_triggers);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Error parsing the trigger chain list, exiting." );
      return StatusCode::FAILURE;
    }
  }

  /** parse the list of group names **/
  if(m_groupNamesToMonitor.size()==0) { //all groups
    ATH_MSG_DEBUG("Running for all groups ...");
    m_nGroups = groupNames.size();
    for (unsigned i=0; i<m_nGroups; ++i) {
      m_processGroup.push_back(true);
    }
  } else { //only selected groups
      ATH_MSG_DEBUG("Running for selected groups ...");
      m_processGroup.resize(m_nGroups, false);
      for(unsigned group=0; group<m_groupNamesToMonitor.size(); ++group) {
        for(unsigned i=0; i< m_nGroups; ++i) { 
         if(groupIn[i] ==  m_groupNamesToMonitor[group] ) { // should monitor this group
           m_processGroup[group] = true;
           ++m_nGroups;
           break;
         }
        }
      }
      if(m_nGroups==0) { // something wrong
        ATH_MSG_ERROR(" List of groups to monitor is not valid !!! ");
        return StatusCode::FAILURE;
      }
  }   

  return ManagedMonitorToolBase::initialize();
}

/*---------------------------------------------------------*/
StatusCode 
LArCoherentNoisefractionMon::bookHistograms()
{
    /** lar noise correlation */
    m_Nchan=128;
    m_chan_low=-0.5;
    m_chan_up=127.5;

    MonGroup GroupEMBA( this, "/LAr/CoherentNoiseFraction/EMBA", run, ATTRIB_MANAGED );
    MonGroup GroupEMBC( this, "/LAr/CoherentNoiseFraction/EMBC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECA( this, "/LAr/CoherentNoiseFraction/EMECA", run, ATTRIB_MANAGED );
    MonGroup GroupEMECC( this, "/LAr/CoherentNoiseFraction/EMECC", run, ATTRIB_MANAGED );
    MonGroup GroupHECA( this, "/LAr/CoherentNoiseFraction/HECA", run, ATTRIB_MANAGED );
    MonGroup GroupHECC( this, "/LAr/CoherentNoiseFraction/HECC", run, ATTRIB_MANAGED );
    MonGroup GroupFCALA( this, "/LAr/CoherentNoiseFraction/FCALA", run, ATTRIB_MANAGED );
    MonGroup GroupFCALC( this, "/LAr/CoherentNoiseFraction/FCALC", run, ATTRIB_MANAGED );

    /**declare strings for histograms title*/
    m_hist_name = "CoherentNoiseFraction_"; 
    m_hist_title = "Coherent Noise Fraction ";
    m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
    m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);

    /** Check which (if any) FEBs we want to monitor*/ 
    if(m_FEBsToMonitor.size()==0) {
      bookAllFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
    }
    else bookSelectedFEBs(GroupEMBA,GroupEMBC,GroupEMECA,GroupEMECC,GroupHECA,GroupHECC,GroupFCALA,GroupFCALC);
  
  ATH_MSG_DEBUG("m_FEBhistograms size: " << m_FEBhistograms.size());

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArCoherentNoisefractionMon::fillHistograms()
{
  ATH_MSG_DEBUG("in fillHists()" );
    
  /**increase internal event counter, for calibration run only */
  m_evtCounter++;

  /** check trigger */
  bool passTrig = m_isCalibrationRun;
  if(!m_isCalibrationRun) { 
     /**EventID is a part of EventInfo, search event informations:*/
     const xAOD::EventInfo* thisEvent=nullptr;
     ATH_CHECK(evtStore()->retrieve(thisEvent));
  
     if(thisEvent) m_evtId = thisEvent->eventNumber(); else m_evtId = 0;
     ATH_MSG_DEBUG("Event nb " << m_evtId );  

    bool passBCID=false;
    if(!m_trigDecTool.empty()) {
      for(auto trig_chain : m_triggers) {
	passBCID = ((trig_chain == "HLT_noalg_cosmiccalo_L1RD1_EMPTY")?(thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END):true);
	passTrig=(passTrig || (passBCID && m_trigDecTool->isPassed(trig_chain)));
      }
    }
  }

  if (!passTrig) {
      ATH_MSG_DEBUG ( " Failed trigger selection " );
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
  
  /** reset tmp arrays **/
  for (auto & arr_entry : m_div) arr_entry.second.clear();
  
  ATH_MSG_DEBUG ( " LArDigitContainer size "<<pLArDigitContainer->size()<<" for key "<<m_LArDigitContainerKey); 
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  
  HWIdentifier febID;
  unsigned chan;

  /** Loop over digits*/
  for ( ; itDig!=itDig_e;++itDig) {
    const LArDigit* pLArDigit = *itDig;
    
    /** Retrieve pedestals */
    HWIdentifier id = pLArDigit->hardwareID();
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = pedestals->pedestal(id,gain);    

    /** Retrieve samples*/
    const std::vector<short>* digito = &pLArDigit->samples();
    
    /** Retrieve once, all the cell info:*/
    febID = m_LArOnlineIDHelper->feb_Id(id);
    chan = m_LArOnlineIDHelper->channel(id);

    for(unsigned group=0; group < m_processGroup.size(); ++group) {
 
       if(!m_processGroup[group]) continue;
       try {
         std::vector<float> &tmparr=m_div.at(std::make_pair(febID,group));
         short sample=digito->at(2);
         float sdiff = sample-pedestal;
         if(group==0) {
           tmparr.push_back(sdiff); 
         }else if (group==1 && chan%2==0) {
           tmparr.push_back(sdiff); 
         }else if (group==2 && chan%2==1) {
           tmparr.push_back(sdiff); 
         }else if (group==3 && chan<64) {
           tmparr.push_back(sdiff); 
         }else if (group==4 && chan>63) {
           tmparr.push_back(sdiff); 
         }else if (group==5 && chan%2==0 && chan<64) {
           tmparr.push_back(sdiff); 
         }else if (group==6 && chan%2==1 && chan<64) {
           tmparr.push_back(sdiff); 
         }else if (group==7 && chan%2==0 && chan>63) {
           tmparr.push_back(sdiff); 
         }else if (group==8 && chan%2==1 && chan>63) {
           tmparr.push_back(sdiff); 
         }else if (group==9 && chan<32) {
           tmparr.push_back(sdiff); 
         }else if (group==10 && chan>31 && chan<64) {
           tmparr.push_back(sdiff); 
         }else if (group==11 && chan>63 && chan<96) {
           tmparr.push_back(sdiff); 
         }else if (group==12 && chan>95) {
           tmparr.push_back(sdiff); 
         }
       }
       catch (const std::out_of_range& oor) {
	continue;
       }
    } //over groups

  }/** End of loop on LArDigit*/

  for (auto const& feb_entry : m_FEBhistograms) {
      if(m_div[feb_entry.first].size() == 0) {
         ATH_MSG_WARNING("Zero accumulted array for FEB " << feb_entry.first );
      }
      feb_entry.second.first->Fill(calc_sum_dev(&m_div[feb_entry.first]));       
      feb_entry.second.second->Fill(calc_dev(&m_div[feb_entry.first]));       
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCoherentNoisefractionMon::procHistograms()
{
  if(endOfRunFlag()) {
      fillInCNF();
  }
  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
/**Compute correlations and fill in the plots*/
void LArCoherentNoisefractionMon::fillInCNF()
{
  ATH_MSG_DEBUG("fillInCNF()" );
  double tot_noi, noncoh_noi;
  double coh_fraction;
  int slot, ft;
  for (auto const& feb_entry : m_FEBhistograms) {
          tot_noi     = feb_entry.second.first->getROOTHist()->GetRMS();
          noncoh_noi = feb_entry.second.second->getROOTHist()->GetMean()*groupNChan[feb_entry.first.second];
          double dtmp=tot_noi*tot_noi - noncoh_noi;
          slot          = m_LArOnlineIDHelper->slot(feb_entry.first.first);
          ft  = m_LArOnlineIDHelper->feedthrough(feb_entry.first.first);
          if(dtmp < 0. || tot_noi == 0) {
             ATH_MSG_WARNING("FT "<< ft << " slot " << slot << " group " <<feb_entry.first.second <<" has tot noise " << tot_noi << " but ncoh part :" << noncoh_noi << " with " << groupNChan[feb_entry.first.second] << " channnels");
             coh_fraction=0.;
          } else {
             coh_fraction=100.*sqrt(tot_noi*tot_noi - noncoh_noi) / (tot_noi/sqrt(groupNChan[feb_entry.first.second]) * groupNChan[feb_entry.first.second]);
          } 
          ATH_MSG_DEBUG("feb_entry.first.second " << feb_entry.first.second );
	  // EMB
	  if(m_LArOnlineIDHelper->isEMBchannel( feb_entry.first.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first.first) ){
	      m_h_summary_plot_EMBA[feb_entry.first.second]->SetBinContent( slot, ft+1,  coh_fraction);
	    }
	    else{
	      m_h_summary_plot_EMBC[feb_entry.first.second]->SetBinContent( slot, ft+1,  coh_fraction);
	    }
	  }
	  // EMEC
	  else if(m_LArOnlineIDHelper->isEMECchannel( feb_entry.first.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first.first)){
	      m_h_summary_plot_EMECA[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	    else{
	      m_h_summary_plot_EMECC[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	  }
	  // HEC
	  else if(m_LArOnlineIDHelper->isHECchannel( feb_entry.first.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first.first)){
	      m_h_summary_plot_HECA[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	    else{
	      m_h_summary_plot_HECC[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	  }
	  // FCAL
	  else if(m_LArOnlineIDHelper->isFCALchannel( feb_entry.first.first )){
	    if(m_LArOnlineIDHelper->pos_neg(feb_entry.first.first)){
	      m_h_summary_plot_FCALA[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	    else{
	      m_h_summary_plot_FCALC[feb_entry.first.second]->SetBinContent( slot, ft,  coh_fraction);
	    }
	  }
    }
}


/*---------------------------------------------------------*/
/**Loops on selected FEBS to book and define histograms.*/
void LArCoherentNoisefractionMon::bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
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
    for(unsigned i=0; i<m_processGroup.size(); ++i) {
            if(m_processGroup[i]) bookThisFEB(febid, i, grEMBA,grEMBC,grEMECA,grEMECC,grHECA,grHECC,grFCALA,grFCALC);
    }
  }
  // now book summaries for all groups
  for(unsigned i=0; i<m_processGroup.size(); ++i) {
     if(m_processGroup[i]) {

        TH2F_LW *h_EMBA     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMBC     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMECA    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMECC    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_HECA     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_HECC     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_FCALA    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_FCALC    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        grEMBA.regHist(h_EMBA).ignore();  
        m_h_summary_plot_EMBA.push_back(h_EMBA);
        grEMBC.regHist(h_EMBC).ignore();  
        m_h_summary_plot_EMBC.push_back(h_EMBC);
        grEMECA.regHist(h_EMECA).ignore();  
        m_h_summary_plot_EMECA.push_back(h_EMECA);
        grEMECC.regHist(h_EMECC).ignore();  
        m_h_summary_plot_EMECC.push_back(h_EMECC);
        grHECA.regHist(h_HECA).ignore();  
        m_h_summary_plot_HECA.push_back(h_HECA);
        grHECC.regHist(h_HECC).ignore();  
        m_h_summary_plot_HECC.push_back(h_HECC);
        grFCALA.regHist(h_FCALA).ignore();  
        m_h_summary_plot_FCALA.push_back(h_FCALA);
        grFCALC.regHist(h_FCALC).ignore();  
        m_h_summary_plot_FCALC.push_back(h_FCALC);
     }
  }// over groups
}


/*---------------------------------------------------------*/
/**Loops on all FEBS to book and define histograms.*/
void LArCoherentNoisefractionMon::bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  ATH_MSG_INFO( "No selected FEBs provided: booking all FEBs.");
  
  /** loop on FEBs to init histograms */
  std::vector<HWIdentifier>::const_iterator feb_it = m_LArOnlineIDHelper->feb_begin();
  std::vector<HWIdentifier>::const_iterator feb_it_e = m_LArOnlineIDHelper->feb_end();
  for ( ; feb_it!=feb_it_e;++feb_it) {
    for(unsigned i=0; i<m_groupNamesToMonitor.size(); ++i) {
            if(m_processGroup[i]) bookThisFEB((*feb_it), i, grEMBA,grEMBC,grEMECA,grEMECC,grHECA,grHECC,grFCALA,grFCALC);
    }
  }
  // now book summaries for all groups
  for(unsigned i=0; i<m_groupNamesToMonitor.size(); ++i) {
     if(m_processGroup[i]) {

        TH2F_LW *h_EMBA     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMBC     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMECA    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_EMECC    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_HECA     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_HECC     = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_FCALA    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        TH2F_LW *h_FCALC    = TH2F_LW::create((m_hist_name+groupNames[i]).c_str(), (m_hist_title+groupNames[i]).c_str() ,14, 1, 15, 33, 0, 33);
        grEMBA.regHist(h_EMBA).ignore();  
        m_h_summary_plot_EMBA.push_back(h_EMBA);
        grEMBC.regHist(h_EMBC).ignore();  
        m_h_summary_plot_EMBC.push_back(h_EMBC);
        grEMECA.regHist(h_EMECA).ignore();  
        m_h_summary_plot_EMECA.push_back(h_EMECA);
        grEMECC.regHist(h_EMECC).ignore();  
        m_h_summary_plot_EMECC.push_back(h_EMECC);
        grHECA.regHist(h_HECA).ignore();  
        m_h_summary_plot_HECA.push_back(h_HECA);
        grHECC.regHist(h_HECC).ignore();  
        m_h_summary_plot_HECC.push_back(h_HECC);
        grFCALA.regHist(h_FCALA).ignore();  
        m_h_summary_plot_FCALA.push_back(h_FCALA);
        grFCALC.regHist(h_FCALC).ignore();  
        m_h_summary_plot_FCALC.push_back(h_FCALC);
     }
  }// over groups
}

/*---------------------------------------------------------*/
/**Book and defines histogramms for a given FEB.*/
void LArCoherentNoisefractionMon::bookThisFEB(HWIdentifier id, unsigned group, MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC)
{
  ATH_MSG_DEBUG("Booking " << id.getString() );
  int num_slot          = m_LArOnlineIDHelper->slot(id);
  int num_feedthrough   = m_LArOnlineIDHelper->feedthrough(id);

  std::string this_name=m_strHelper->feb_str(id)+"_"+std::to_string(group)+"_tot";
  std::string slot_feed_title = std::to_string(num_slot) + "_" + std::to_string(num_feedthrough)+"_tot";
  TH1F_LW* h_tot = TH1F_LW::create((this_name).c_str(), slot_feed_title.c_str(), 80, -400., 400.);

  this_name=m_strHelper->feb_str(id)+"_"+std::to_string(group)+"_noncoh";
  slot_feed_title = std::to_string(num_slot) + "_" + std::to_string(num_feedthrough)+"_noncoh";
  TH1F_LW* h_noncoh = TH1F_LW::create((this_name).c_str(), slot_feed_title.c_str(), 120, -40., 200.);

  m_FEBhistograms[std::make_pair(id, group)]=std::make_pair(h_tot,h_noncoh);
  m_div[std::make_pair(id, group)]=std::vector<float>();

  ATH_MSG_DEBUG("size " << m_FEBhistograms.size() );
  if(m_LArOnlineIDHelper->isEMBchannel(id)) {
    if(m_LArOnlineIDHelper->pos_neg(id)==1) {
      grEMBA.regHist(h_noncoh).ignore();
      grEMBA.regHist(h_tot).ignore();
    } else {
      grEMBC.regHist(h_noncoh).ignore();
      grEMBC.regHist(h_tot).ignore();
    }
  } else {
    if(m_LArOnlineIDHelper->isEMECchannel(id)) {
      if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	grEMECA.regHist(h_noncoh).ignore();
	grEMECA.regHist(h_tot).ignore();
      } else {
	grEMECC.regHist(h_noncoh).ignore();
	grEMECC.regHist(h_tot).ignore();
      }
    } else {
      if(m_LArOnlineIDHelper->isHECchannel(id)) {
	if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	  grHECA.regHist(h_noncoh).ignore();
	  grHECA.regHist(h_tot).ignore();
	} else {
	  grHECC.regHist(h_noncoh).ignore();
	  grHECC.regHist(h_tot).ignore();
	}
      } else {
	if(m_LArOnlineIDHelper->isFCALchannel(id)) {
	  if(m_LArOnlineIDHelper->pos_neg(id)==1) {
	    grFCALA.regHist(h_noncoh).ignore();
	    grFCALA.regHist(h_tot).ignore();
	  } else {
	    grFCALC.regHist(h_noncoh).ignore();
	    grFCALC.regHist(h_tot).ignore();
	  }
	}
      }
     }
   }
}

double LArCoherentNoisefractionMon::calc_dev(std::vector<float> *input_vector) const {
  double tmean=0.; 
  double tdiv = 0.;
  for(unsigned it=0;it<input_vector->size();it++) tmean += input_vector->at(it);
  tmean = tmean / input_vector->size();
  for(unsigned it=0;it<input_vector->size();it++) tdiv += (tmean - input_vector->at(it))*(tmean - input_vector->at(it));
  tdiv = tdiv / input_vector->size();
  return tdiv;
}

double LArCoherentNoisefractionMon::calc_sum_dev(std::vector<float> *input_vector) const {
  double tsum=0.; 
  for(unsigned it=0;it<input_vector->size();it++) tsum += input_vector->at(it);
  return tsum;
}
