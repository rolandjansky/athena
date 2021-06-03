/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
*/

// ********************************************************************
// NAME:     LArCoherentNoisefractionMon.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Pavol Strizenec, based on codes from M. Spalla and Y. Enari
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
#include "LArCoherentNoisefractionMonAlg.h"


/*---------------------------------------------------------*/
LArCoherentNoisefractionMonAlg::LArCoherentNoisefractionMonAlg( const std::string& name, ISvcLocator* pSvcLocator) 
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(nullptr)
{ 
}

/*---------------------------------------------------------*/
LArCoherentNoisefractionMonAlg::~LArCoherentNoisefractionMonAlg()
{ }


/*---------------------------------------------------------*/
StatusCode 
LArCoherentNoisefractionMonAlg::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArCoherentNoisefractionMonAlg" );  
  
  ATH_CHECK(detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ));

  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_keyPedestal.initialize());
  ATH_CHECK(m_LArDigitContainerKey.initialize());
  
  /** Configure event info */
  ATH_CHECK(m_eventInfoKey.initialize());

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  if(m_processGroup.size() == 0 || m_groupNames.size() == 0 || m_groupNChan.size() ==0 ||
     m_processGroup.size() != m_groupNames.size() || m_processGroup.size() != m_groupNChan.size() ){
     ATH_MSG_ERROR("Wrong configuration of LArCoherentNoisefractionMonAlg, bombing !");
     return StatusCode::FAILURE;
  }
  unsigned int nGroups=0;
  ATH_MSG_DEBUG("Running for selected groups ...");
  for(auto g : m_processGroup) {
       if(g) ++nGroups;
  }
  if(nGroups==0) { // something wrong
    ATH_MSG_ERROR(" List of groups to monitor is empty !!! ");
    return StatusCode::FAILURE;
  }

  m_febMap.reserve(m_FEBlist.size());
  for(auto feb =  m_LArOnlineIDHelper->feb_begin(); feb != m_LArOnlineIDHelper->feb_end(); ++feb) {
     bool plotThisFEB=false;
     for(uint ifm=0;ifm<m_FEBlist.size();ifm++) {
     if(febString(*feb)==m_FEBlist[ifm]) {
          plotThisFEB=true;
          break;
        }
      }
     if(plotThisFEB) m_febMap.emplace_back(*feb);
  } 

  /*now the groups*/
  for(unsigned group=0; group<m_processGroup.size(); ++group) {
     if(m_processGroup[group]) {
        m_histoGroups.push_back(Monitored::buildToolMap<int>(m_tools,m_groupNames[group],m_FEBlist));
     }
  }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArCoherentNoisefractionMonAlg::fillHistograms(const EventContext& ctx) const
{

  ATH_MSG_DEBUG("in fillHists()" ); 
  /** nothing to fill, if FEB list was empty **/
  if(m_FEBlist.size() == 0) return StatusCode::SUCCESS;

  /** check trigger */
  bool passTrig = m_isCalibrationRun;
  if(!m_isCalibrationRun) { 
    ATH_MSG_DEBUG( "Parsing trigger chain list" ); 
     /**EventID is a part of EventInfo, search event informations:*/
    SG::ReadHandle<xAOD::EventInfo> thisEvent{m_eventInfoKey,ctx};
     if (!thisEvent.isValid()) {
       ATH_MSG_ERROR("xAOD::EventInfo retrieval failed");
       return StatusCode::FAILURE;
     }
  
    const ToolHandle<Trig::TrigDecisionTool> trigTool=getTrigDecisionTool();

     bool passBCID;
     if(!trigTool.empty()) {
       // BCIDs of the abort gap
       const int ABORT_GAP_START = 3446;
       const int ABORT_GAP_END   = 3563;
       for(auto trig_chain : m_vTrigChainNames) {
	passBCID = ((trig_chain == "HLT_noalg_cosmiccalo_L1RD1_EMPTY")?(thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END):true);
	passTrig=(passTrig || (passBCID && trigTool->isPassed(trig_chain)));
       }
     }
  } else {
    ATH_MSG_INFO( "Running as 'calibration run'. No trigger selection will be applied...");
  }

  if (!passTrig) { 
      ATH_MSG_DEBUG ( " Failed trigger selection " );
      return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ( " Pass trigger selection " );
  }

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
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 

  /** create tmp arrays **/
  //for (auto & arr_entry : m_div) arr_entry.second.clear();
  std::map<std::pair<HWIdentifier,unsigned>, std::vector<float> > div_array;
  for (auto arr_entry : m_febMap) {
     for(unsigned group=0; group<m_processGroup.size(); ++group) {
        if(m_processGroup[group])  div_array[std::make_pair(arr_entry,group)]=std::vector<float>(); 
     }
  }
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
      if(div_array.count(std::make_pair(febID,group)) > 0) {
        try {
         std::vector<float> &tmparr=div_array.at(std::make_pair(febID,group));
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
      } // if kay exists
    } //over groups

  }/** End of loop on LArDigit*/
 
  //now fill the plots
  for(unsigned group=0; group < m_processGroup.size(); ++group) {
     if(!m_processGroup[group]) {
        ATH_MSG_DEBUG("Group "<<m_groupNames[group]<<" not filled");
        continue;
     }
     ATH_MSG_DEBUG("febMap size: "<<m_febMap.size());
     for(auto const& feb_entry : m_febMap) {
        std::string febstr = febString(feb_entry);
        auto chanSumDev = Monitored::Scalar<double>("SumDev",calc_sum_dev(&div_array[std::make_pair(feb_entry,group)]));   
        auto chanDev = Monitored::Scalar<double>("Dev",calc_dev(&div_array[std::make_pair(feb_entry,group)]));   
        fill(m_tools[m_histoGroups.at(group).at(febstr)],chanSumDev,chanDev);
     }
  }
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
/** build the FEB string, following instructions from python config */
std::string LArCoherentNoisefractionMonAlg::febString(const HWIdentifier afeb) const {
  std::string eb=m_LArOnlineIDHelper->isEMBchannel(afeb) ? "Barrel" : "Endcap";
  std::string ac=(m_LArOnlineIDHelper->pos_neg(afeb)==1) ? "A" : "C";
  int FT = m_LArOnlineIDHelper->feedthrough(afeb);
  int SL = m_LArOnlineIDHelper->slot(afeb);
  return eb+ac+Form("ft%02d",FT)+Form("slot%02d",SL);
}

double LArCoherentNoisefractionMonAlg::calc_dev(std::vector<float> *input_vector) const {
  double tmean=0.; 
  double tdiv = 0.;
  for(auto it : *input_vector) tmean += it;
  tmean = tmean / input_vector->size();
  for(auto it : *input_vector) tdiv += (tmean - it)*(tmean - it);
  tdiv = tdiv / input_vector->size();
  return tdiv;
}
double LArCoherentNoisefractionMonAlg::calc_sum_dev(std::vector<float> *input_vector) const {
  double tsum=0.; 
  for(auto it : *input_vector) tsum += it;
  return tsum;
}


