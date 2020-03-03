/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArNoisyROMonAlg.h"

#include "LArRecEvent/LArEventBitInfo.h"
#include "LArIdentifier/LArOnlineID.h"
#include "xAODEventInfo/EventInfo.h"

#include <sstream>
#include <stdexcept>
#include <cmath>

LArNoisyROMonAlg::LArNoisyROMonAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthMonitorAlgorithm(name, pSvcLocator), 
  m_LArOnlineIDHelper(nullptr), m_knownFilled(false) {
  
}


StatusCode LArNoisyROMonAlg::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID" ));
  ATH_CHECK(m_inputKey.initialize());
  ATH_CHECK(m_badFebKey.initialize());
  ATH_CHECK(m_MNBFebKey.initialize());
  
  m_histoGroups.reserve(m_SubDetNames.size());
  for (unsigned i=0; i<m_SubDetNames.size(); ++i) {
    std::vector<std::string> part;
    part.push_back(m_partitions[2*i]);
    part.push_back(m_partitions[2*i+1]);
    m_histoGroups.push_back(Monitored::buildToolMap<int>(m_tools,m_SubDetNames[i],part));
  }

  return AthMonitorAlgorithm::initialize();
}

StatusCode LArNoisyROMonAlg::fillHistograms(const EventContext& ctx) const
{
  { // extra namespace for mutex
     std::lock_guard<std::mutex> lock(m_lock);
     if(!m_knownFilled) { // first time fill known Bad and MNB FEBs
        SG::ReadCondHandle<LArBadFebCont> badHdl{m_badFebKey, ctx};
        const LArBadFebCont *badCont{*badHdl};
        if(badCont) {
           auto sl=Monitored::Scalar<unsigned>("slotBad",0);
           auto FT=Monitored::Scalar<unsigned>("FTBad",0);
           if(badCont->begin() == badCont->end()) {
                ATH_MSG_WARNING("List of known Bad FEBs empty !? ");
           } else {
              for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); i++) {
                HWIdentifier chid(i->first);
                sl = m_LArOnlineIDHelper->slot(chid);
                FT = m_LArOnlineIDHelper->feedthrough(chid);
                unsigned sd = partitionNumber(chid)/2;
                unsigned part = partitionNumber(chid);
                ATH_MSG_INFO("Filled known Bad FEBs for " << sd << " and " << part);
                fill(m_tools[m_histoGroups.at(sd).at(m_partitions[part])],sl,FT);
              }
              ATH_MSG_DEBUG("Filled known Bad FEBs .... ");
           }
        } else {
           ATH_MSG_WARNING("Known Bad FEBs container not existing !? ");
        }
        
        SG::ReadCondHandle<LArBadFebCont> mnbHdl(m_MNBFebKey, ctx);
        const LArBadFebCont* mnbCont{*mnbHdl};
        if(mnbCont) {
           auto sl=Monitored::Scalar<unsigned>("slotMNB",0);
           auto FT=Monitored::Scalar<unsigned>("FTMNB",0);
           if(mnbCont->begin() == mnbCont->end()) {
                ATH_MSG_WARNING("List of known MNB FEBs empty !? ");
           } else {
              for(LArBadFebCont::BadChanVec::const_iterator i = mnbCont->begin(); i!=mnbCont->end(); i++) {
                HWIdentifier chid(i->first);
                sl = m_LArOnlineIDHelper->slot(chid);
                FT = m_LArOnlineIDHelper->feedthrough(chid);
                unsigned sd = partitionNumber(chid)/2;
                unsigned part = partitionNumber(chid);
                ATH_MSG_INFO("Filled known MNB FEBs for " << sd << " and " << part);
                fill(m_tools[m_histoGroups.at(sd).at(m_partitions[part])],sl,FT);
              }
              ATH_MSG_DEBUG("Filled known MNB FEBs .... ");
           }
        } else {
           ATH_MSG_WARNING("Known MNB FEBs container not existing !? ");
        }
        m_knownFilled=true;
     }
  } // namespace for lock

  // retrieve 
  SG::ReadHandle<LArNoisyROSummary> noisyRO{m_inputKey, ctx};
  if (!noisyRO.isValid()) 
  {
    ATH_MSG_WARNING( "Can't retrieve LArNoisyROSummary " );
    return StatusCode::SUCCESS;
  }
  
  
  SG::ReadHandle<xAOD::EventInfo> eventInfo = GetEventInfo(ctx);
  
  unsigned int LBN = eventInfo->lumiBlock();
  bool burstveto = eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO);

  const std::vector<HWIdentifier>& mnbtightFEB = noisyRO->get_MNBTight_febs();  
  const std::vector<HWIdentifier>& mnbtight_PsVetoFEB = noisyRO->get_MNBTight_PsVeto_febs();
  const std::vector<HWIdentifier>& mnblooseFEB = noisyRO->get_MNBLoose_febs();  

  // Filling Candidate MNB Tree
  std::vector<int> v_candidate_MNBTightFEB;
  std::vector<int> v_candidate_MNBTight_PsVetoFEB;
  std::vector<int> v_candidate_MNBLooseFEB;

  for(unsigned int iFeb=0; iFeb<mnbtightFEB.size(); iFeb++) 
    v_candidate_MNBTightFEB.push_back(mnbtightFEB.at(iFeb).get_identifier32().get_compact());

  for(unsigned int iFeb=0; iFeb<mnbtight_PsVetoFEB.size(); iFeb++) 
    v_candidate_MNBTight_PsVetoFEB.push_back(mnbtight_PsVetoFEB.at(iFeb).get_identifier32().get_compact());

  if (m_storeLooseMNBFEBs){ // joboption - By default the FEB flagged as MNB-Loose are not stored in the TTree
    for(unsigned int iFeb=0; iFeb<mnblooseFEB.size(); iFeb++)
      v_candidate_MNBLooseFEB.push_back(mnblooseFEB.at(iFeb).get_identifier32().get_compact());
  }

  if(v_candidate_MNBLooseFEB.size() > 0 || v_candidate_MNBTightFEB.size() > 0) {
    auto candidate_MNB_time = Monitored::Scalar<unsigned int>("candidate_MNB_time", eventInfo->timeStamp());
    auto candidate_MNB_time_ns = Monitored::Scalar<unsigned int>("candidate_MNB_time_ns", eventInfo->timeStampNSOffset());
    auto n_candidate_MNBTight_FEB = Monitored::Scalar<unsigned int>("n_candidate_MNBTight_FEB", mnbtightFEB.size());
    auto n_candidate_MNBTight_PsVeto_FEB = Monitored::Scalar<unsigned int>("n_candidate_MNBTight_PsVeto_FEB", mnbtightFEB.size());
    auto n_candidate_MNBLoose_FEB = Monitored::Scalar<unsigned int>("n_candidate_MNBLoose_FEB", mnblooseFEB.size());  
 
    auto mon_candidate_MNBTightFEB = Monitored::Collection("v_candidate_MNBTightFEB", v_candidate_MNBTightFEB);
    auto mon_candidate_MNBTight_PsVetoFEB = Monitored::Collection("v_candidate_MNBTight_PsVetoFEB", v_candidate_MNBTight_PsVetoFEB);
    auto mon_candidate_MNBLooseFEB = Monitored::Collection("v_candidate_MNBLooseFEB", v_candidate_MNBLooseFEB);  

    ATH_MSG_DEBUG("Filling CandidateMNB tree");

    fill(m_MonGroupName,candidate_MNB_time,candidate_MNB_time_ns,n_candidate_MNBTight_FEB,n_candidate_MNBTight_PsVeto_FEB,n_candidate_MNBLoose_FEB,mon_candidate_MNBTightFEB,mon_candidate_MNBTight_PsVetoFEB,mon_candidate_MNBLooseFEB);  
  }

  // Fill the noise bursts timestamp ttree
  auto time =  Monitored::Scalar<int>("time",eventInfo->timeStamp());
  auto time_ns = Monitored::Scalar<int>("time_ns",eventInfo->timeStampNSOffset()); 
  auto algo = Monitored::Scalar<unsigned char>("algo",0);
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS) ) 
  {
    algo |= 0x1;
  }
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) 
  {
    algo |= 0x2;
  }
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS_W) ) 
  {
    algo |= 0x8;
  }
  if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTLOOSE))
  {
    algo |= 0x10;
  }
  if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT))
  {
    algo |= 0x20;
  }
  if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT_PSVETO))
  {
    algo |= 0x40;
  }

  if ( algo != 0 ) {
    if ( burstveto ) algo |= 0x4;
    ATH_MSG_DEBUG("Filling LArNoise tree with algo: " << algo);
    fill(m_MonGroupName,time,time_ns,algo);
  } else { 
    ATH_MSG_DEBUG("Not a noisy event");
  }

  // Triggers
  uint8_t trigbits = 0;
  uint8_t L1trigbits = 0;
  if ( m_doTrigger )
  {
    for ( size_t i = 0; i < m_EF_NoiseBurst_Triggers.size(); i++)
    {
      if ( m_trigDecTool->isPassed(m_EF_NoiseBurst_Triggers[i]))  trigbits |= (0x1 << i);
    }
    for ( size_t i = 0; i < m_L1_NoiseBurst_Triggers.size(); i++)
    {
      if ( m_trigDecTool->isPassed(m_L1_NoiseBurst_Triggers[i]))  L1trigbits |= (0x1 << i);
    }
  }
  if(!m_doHisto) return StatusCode::SUCCESS;
  
  // Fill the 2D map of noisy and mini-noisy FEBs
  const std::vector<HWIdentifier>& noisyFEB = noisyRO->get_noisy_febs();

  // Loop on all FEBs noisy in Std definition (i.e >30 channels with q factor > 4000)
  // And fill the 2D maps of fraction of fraction of noisy events
  unsigned int NbNoisyFEB = 0;
  std::array<unsigned,4> partMask({{LArNoisyROSummary::EMECAMask,LArNoisyROSummary::EMBAMask,LArNoisyROSummary::EMBCMask,LArNoisyROSummary::EMECCMask}});

  for (size_t i = 0; i<noisyFEB.size(); i++)
  {
    NbNoisyFEB++;
    const HWIdentifier& febid = noisyFEB[i];
    // Will be used in nest versions:
    //HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    //int FT = m_LArOnlineIDHelper->feedthrough(id);
    //int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      // FIXME:
      //if (m_IsOnline)
      //	  m_partHistos[partition].h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      //	else
      //	  m_partHistos[partition].h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
    }

  } // End of loop on all RNB - noisy FEB
  auto n_noisyFEB = Monitored::Scalar<int>("n_noisyFEBs",NbNoisyFEB);
  auto lb = Monitored::Scalar<int>("LBN",LBN);
  fill(m_MonGroupName,n_noisyFEB,lb);

  // Loop on all FEBs noisy in MNB-tight definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Fill two histograms with veto cut and all events
  for (size_t i = 0; i<mnbtightFEB.size(); i++)
  {
    const HWIdentifier& febid = mnbtightFEB[i];
    // Will be used in next iteration
    //HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    //int FT = m_LArOnlineIDHelper->feedthrough(id);
    //int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
       //FIXME
      if (m_IsOnline)
        {
	  //m_partHistos[partition].h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0){ 
            //m_partHistos[partition].h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
          }
        }
        else
        {
	  //m_partHistos[partition].h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0){ 
            //m_partHistos[partition].h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
          }
        }
    }

  }// End of loop on all MNB-Tight - noisy FEB

  // Loop on all FEBs noisy in MNB-tight-PsVeto definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Fill two histograms with veto cut and all events
  //  std::cout << "Nb of tight_PsVeto FEB" << mnbtightFEB.size() << std::endl;
  for (size_t i = 0; i<mnbtight_PsVetoFEB.size(); i++)
  {
    const HWIdentifier& febid = mnbtight_PsVetoFEB[i];
    // Will be used in next iteration
    //HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    //int FT = m_LArOnlineIDHelper->feedthrough(id);
    //int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      //FIXME
      if (m_IsOnline)
        {
        //m_partHistos[partition].h_CandidateMNBTight_PsVetoFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTight_PsVetoFlaggedPartitions() & partMask[partition]) != 0){ 
            //m_partHistos[partition].h_MNBTight_PsVetoFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
             ;
          }
        }
        else
        {
        //m_partHistos[partition].h_CandidateMNBTight_PsVetoFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTight_PsVetoFlaggedPartitions() & partMask[partition]) != 0){ 
            //m_partHistos[partition].h_MNBTight_PsVetoFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
          }
        }
    }

  }// End of loop on all MNB-Tight-PsVeto - noisy FEB

  // Loop on all FEBs noisy in MNB-loose definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Fill two histograms with veto cut and all events
  for (size_t i = 0; i<mnblooseFEB.size(); i++)
  {
    //std::cout << "MNBLoose FEB " <<  noisyFEB[i].get_compact() << std::endl;
    const HWIdentifier& febid = mnblooseFEB[i];
    // Will be used in next iteration:
    //HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    //int FT = m_LArOnlineIDHelper->feedthrough(id);
    //int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      // FIXME
      if (m_IsOnline)
        {
	  //m_partHistos[partition].h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0) {
            //m_partHistos[partition].h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
          }
        }
        else
        {
	  //m_partHistos[partition].h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0){ 
            //m_partHistos[partition].h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
          }
        }
    }

  }// End of loop on all MNB-Loose - noisy FEB


  // End of 2D map of FEB found as noisy (in any definition : Std, MNB-Tight, MNB-Tight-PsVeto or MNB-Loose)
  // Now fill 1D histograms of fraction of events found as noisy vetoed or not
  // Event found noisy by Std method
  uint8_t BadFEBPartitions = noisyRO->BadFEBFlaggedPartitions();
  if ( BadFEBPartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (BadFEBPartitions & partMask[i]) != 0 ) {
         //FIXME
	//m_partHistos[i].h_NoisyEvent->Fill(LBN);
	//if ( m_doTrigger ) fillTriggerHisto(m_partHistos[i],trigbits,L1trigbits);
	//if ( ! burstveto ) m_partHistos[i].h_NoisyEventTimeVeto->Fill(LBN);
      }
    }
  } // End of test on RNB

  // event flagged by # of saturated quality cells
  uint8_t SatTightPartitions = noisyRO->SatTightFlaggedPartitions();
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) {
    for (int i= 0;i<4;i++){
      if ( (SatTightPartitions & partMask[i]) != 0 ) {
         //FIXME
	//m_partHistos[i].h_SaturatedNoisyEvent->Fill(LBN);
	//if ( ! burstveto ) m_partHistos[i].h_SaturatedNoisyEventTimeVeto->Fill(LBN);
      }
    }
  } // end of test on RNB-Saturated


  // event flagged by tight-MNB
  uint8_t MNBTightPartitions = noisyRO->MNBTightFlaggedPartitions();
  if ( MNBTightPartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (MNBTightPartitions & partMask[i]) != 0 ) {
         //FIXME
	//m_partHistos[i].h_MNBTightEvent->Fill(LBN);
	//if ( ! burstveto ) m_partHistos[i].h_MNBTightEventTimeVeto->Fill(LBN);
      }
    }
  } // End of test on MNB-Tight

  // event flagged by tight-MNB-PsVeto
  uint8_t MNBTight_PsVetoPartitions = noisyRO->MNBTight_PsVetoFlaggedPartitions();
  if ( MNBTight_PsVetoPartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (MNBTight_PsVetoPartitions & partMask[i]) != 0 ) {
      //FIXME
      //m_partHistos[i].h_MNBTight_PsVetoEvent->Fill(LBN);
      //if ( ! burstveto ) m_partHistos[i].h_MNBTight_PsVetoEventTimeVeto->Fill(LBN);
      }
    }
  } // End of test on MNB-Tight-PsVeto

  // event flagged by loose-MNB
  uint8_t MNBLoosePartitions = noisyRO->MNBLooseFlaggedPartitions();
  if ( MNBLoosePartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (MNBLoosePartitions & partMask[i]) != 0 ) {
        //FIXME
	//m_partHistos[i].h_MNBLooseEvent->Fill(LBN);
	//if ( ! burstveto ) m_partHistos[i].h_MNBLooseEventTimeVeto->Fill(LBN);
      }
    }
  } // end fo test on MNB-Loose
    
  return StatusCode::SUCCESS;
}

