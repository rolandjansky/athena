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

/*
StatusCode LArNoisyROMonAlg::bookHistograms()
{
    
    m_eventCounter=0;
    std::string hTitle;
    std::stringstream tit;

    MonGroup overall(this, "/LAr/NoisyRO", run, ATTRIB_MANAGED );
    if(m_doHisto) {
    // Book histograms per partitions    
    MonGroup GroupBarrel(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED );
    MonGroup GroupBarrelFrac(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "weightedEff");
    MonGroup GroupBarrelFracBin(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "perBinEffPerCent");
    m_partHistos.resize(4);
    bookPartitionHistos(m_partHistos[1],"EMBA",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    bookPartitionHistos(m_partHistos[2],"EMBC",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    
    MonGroup GroupEMEC(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECFrac(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "weightedEff" );
    MonGroup GroupEMECFracBin(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "perBinEffPerCent" );
    bookPartitionHistos(m_partHistos[0],"EMECA",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);
    bookPartitionHistos(m_partHistos[3],"EMECC",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);

    
    // Fill Suspicious MNB FEBs
    for (uint i=0;i<m_knownMNBFEBs.size();i++){
      const HWIdentifier& febid = HWIdentifier(m_knownMNBFEBs[i]);
      HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
      int FT = m_LArOnlineIDHelper->feedthrough(id);
      int slot = m_LArOnlineIDHelper->slot(id);
      int partition = partitionNumber(febid);
      m_partHistos[partition].h_MNBKnownFEB->Fill(slot,FT);
    }


    }

    
  return StatusCode::SUCCESS;
}

*/


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
    //unsigned uf=algo;
    //std::cout<<"LArNoisyROMonAlg flag: "<<uf<<std::endl;
    //m_NoiseTimeTree->Fill();
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
    //std::cout << "Noisy FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbNoisyFEB++;
    const HWIdentifier& febid = noisyFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
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
    //std::cout << "MNBTight FEB " <<  noisyFEB[i].get_compact() << std::endl;
    const HWIdentifier& febid = mnbtightFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      if (m_IsOnline)
        {
	  //m_partHistos[partition].h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
        }
        else
        {
	  //m_partHistos[partition].h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
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
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);
//    std::cout << "MNBTight_PsVeto FEB " <<  mnbtight_PsVetoFEB[i].get_compact() << std::endl;
//    std::cout << "Partitions : " << (noisyRO->MNBTight_PsVetoFlaggedPartitions() & partMask[partition]) << std::endl;

    if (partition<4){
      if (m_IsOnline)
        {
        //m_partHistos[partition].h_CandidateMNBTight_PsVetoFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTight_PsVetoFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBTight_PsVetoFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
             ;
        }
        else
        {
        //m_partHistos[partition].h_CandidateMNBTight_PsVetoFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTight_PsVetoFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBTight_PsVetoFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
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
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      if (m_IsOnline)
        {
	  //m_partHistos[partition].h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
        }
        else
        {
	  //m_partHistos[partition].h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0) 
            //m_partHistos[partition].h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
            ;
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
	//m_partHistos[i].h_MNBLooseEvent->Fill(LBN);
	//if ( ! burstveto ) m_partHistos[i].h_MNBLooseEventTimeVeto->Fill(LBN);
      }
    }
  } // end fo test on MNB-Loose
    
  return StatusCode::SUCCESS;
}

/*
StatusCode LArNoisyROMonAlg::procHistograms()
{  
  if(!m_doHisto) return StatusCode::SUCCESS;

  if ( endOfRunFlag() || endOfEventsBlockFlag()){
    if ( m_IsOnline ){
      // copy the "running" histo to the final ones, to be normalised
      for (int i=0;i<4;i++){
	copyHisto(m_partHistos[i].h_NoisyFEBPerEvt,m_partHistos[i].h_NoisyFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_MNBTightFEBPerEvt,m_partHistos[i].h_MNBTightFEBFracPerEvt);
        copyHisto(m_partHistos[i].h_MNBTight_PsVetoFEBPerEvt,m_partHistos[i].h_MNBTight_PsVetoFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_MNBLooseFEBPerEvt,m_partHistos[i].h_MNBLooseFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_CandidateMNBTightFEBPerEvt,m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt);
        copyHisto(m_partHistos[i].h_CandidateMNBTight_PsVetoFEBPerEvt,m_partHistos[i].h_CandidateMNBTight_PsVetoFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_CandidateMNBLooseFEBPerEvt,m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt);
      }
    }

    if (m_eventCounter>0) {
      const double scale =  100./static_cast<double>(m_eventCounter);

      for (int i=0;i<4;i++){
	m_partHistos[i].h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
        m_partHistos[i].h_MNBTight_PsVetoFEBFracPerEvt->scaleContentsAndErrors(scale);
        m_partHistos[i].h_MNBTight_PsVetoFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
        m_partHistos[i].h_CandidateMNBTight_PsVetoFEBFracPerEvt->scaleContentsAndErrors(scale);
        m_partHistos[i].h_CandidateMNBTight_PsVetoFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_NoisyEventFrac,m_partHistos[i].h_NoisyEvent,m_h_LBN);
	m_partHistos[i].h_NoisyEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_SaturatedNoisyEventFrac,m_partHistos[i].h_SaturatedNoisyEvent,m_h_LBN);
	m_partHistos[i].h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBTightEventFrac,m_partHistos[i].h_MNBTightEvent,m_h_LBN);
	m_partHistos[i].h_MNBTightEventFrac->SetEntries(m_eventCounter);
        divideHisto(m_partHistos[i].h_MNBTight_PsVetoEventFrac,m_partHistos[i].h_MNBTight_PsVetoEvent,m_h_LBN);
        m_partHistos[i].h_MNBTight_PsVetoEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBLooseEventFrac,m_partHistos[i].h_MNBLooseEvent,m_h_LBN);
	m_partHistos[i].h_MNBLooseEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_NoisyEventTimeVetoFrac,m_partHistos[i].h_NoisyEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_SaturatedNoisyEventTimeVetoFrac,m_partHistos[i].h_SaturatedNoisyEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBTightEventTimeVetoFrac,m_partHistos[i].h_MNBTightEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
        divideHisto(m_partHistos[i].h_MNBTight_PsVetoEventTimeVetoFrac,m_partHistos[i].h_MNBTight_PsVetoEventTimeVeto,m_h_LBN);
        m_partHistos[i].h_MNBTight_PsVetoEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBLooseEventTimeVetoFrac,m_partHistos[i].h_MNBLooseEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      }    
    }//end if m_eventCounter>0
  }
  

  return StatusCode::SUCCESS;
}
*/

/*
void LArNoisyROMonAlg::bookPartitionHistos(partitionHistos& partition, const std::string& name, MonGroup& group, MonGroup& groupfrac, MonGroup& groupfracbin )
{
  std::string hName, hTitle;
  std::stringstream tit;
  
  // declare histograms properties
  // Endcaps
  int FEB=25,slot=15;
  double FEB_low=-0.5, FEB_up=24.5, slot_low=0.5, slot_up=15.5;
  // for Barrel
  if( name == "EMBA" || name == "EMBC")
  {
    FEB=32, slot=14;
    FEB_low=-0.5, FEB_up=31.5, slot_low=0.5, slot_up=14.5;
  }

  // 2D FEB maps (x : slot / y : FT) with z: fraction of events for which the FEB is noisy or mini-noisy.
  hName  = "MNBKnownFEB_"+name;
  hTitle = "Suspicious MNB FEBs - "+name;
  partition.h_MNBKnownFEB = TH2I_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBKnownFEB->GetXaxis()->SetTitle("Slot");
  partition.h_MNBKnownFEB->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBKnownFEB).ignore();
  

  hName  = "NoisyFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB noisy "+m_NoisyFEBDefStr+" - "+name;
  partition.h_NoisyFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_NoisyFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_NoisyFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_NoisyFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_NoisyFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_NoisyFEBPerEvt).ignore();
  }
   
  hName  = "MNBTightFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Tight " + m_MNBTightFEBDefStr+ " - "+name+" (only vetoed events)";
  partition.h_MNBTightFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBTightFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_MNBTightFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBTightFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_MNBTightFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_MNBTightFEBPerEvt).ignore();
  }

  hName  = "MNBTight_PsVetoFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Tight_PsVeto " + m_MNBTight_PsVetoFEBDefStr+ " - "+name+" (only vetoed events)";
  partition.h_MNBTight_PsVetoFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBTight_PsVetoFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_MNBTight_PsVetoFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBTight_PsVetoFEBFracPerEvt).ignore();  

  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_MNBTight_PsVetoFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_MNBTight_PsVetoFEBPerEvt).ignore();
  }
      
  hName  = "MNBLooseFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Loose " + m_MNBLooseFEBDefStr + " - "+name+" (only vetoed events)";
  partition.h_MNBLooseFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBLooseFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_MNBLooseFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBLooseFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_MNBLooseFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_MNBLooseFEBPerEvt).ignore();
  }

  hName  = "CandidateMNBTightFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Tight "+ m_MNBTightFEBDefStr+ " - "+name;
  partition.h_CandidateMNBTightFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_CandidateMNBTightFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_CandidateMNBTightFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_CandidateMNBTightFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_CandidateMNBTightFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_CandidateMNBTightFEBPerEvt).ignore();
  }

   hName  = "CandidateMNBTight_PsVetoFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Tight_PsVeto "+ m_MNBTight_PsVetoFEBDefStr+ " - "+name;
  partition.h_CandidateMNBTight_PsVetoFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_CandidateMNBTight_PsVetoFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_CandidateMNBTight_PsVetoFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_CandidateMNBTight_PsVetoFEBFracPerEvt).ignore();  

  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_CandidateMNBTight_PsVetoFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_CandidateMNBTight_PsVetoFEBPerEvt).ignore();
  }
      
  hName  = "CandidateMNBLooseFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB-Loose " + m_MNBLooseFEBDefStr + " - "+name;
  partition.h_CandidateMNBLooseFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_CandidateMNBLooseFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_CandidateMNBLooseFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_CandidateMNBLooseFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_CandidateMNBLooseFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_CandidateMNBLooseFEBPerEvt).ignore();
  }

  // Fraction of events found noisy per LB - Regular Noise Burst(RNB) Standard flag
  hName = "NoisyEvent_"+name;
  hTitle = "Yield of events flagged as RNB-Standard - " + name;
  partition.h_NoisyEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_NoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB - RNB-Saturated flag
  hName = "SaturatedNoisyEvent_"+name;
  hTitle = "Yield of events flagged as RNB-Saturated - "+name;
  partition.h_SaturatedNoisyEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_SaturatedNoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_"+name;
  hTitle = "Yield of events flagged as MNB-Tight - "+name;
  partition.h_MNBTightEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBTightEvent_"+name;
  partition.h_MNBTightEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTight_PsVetoEvent_"+name;
  hTitle = "Yield of events flagged as MNB-Tight_PsVeto - "+name;
  partition.h_MNBTight_PsVetoEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTight_PsVetoEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTight_PsVetoEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBTight_PsVetoEvent_"+name;
  partition.h_MNBTight_PsVetoEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTight_PsVetoEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_"+name;
  hTitle = "Yield of events flagged as MNB-Loose - "+name;
  partition.h_MNBLooseEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBLooseEvent_"+name;
  partition.h_MNBLooseEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEvent->GetXaxis()->SetTitle("Luminosity Block");

//  // Fraction of events found noisy per LB - Weighted flag
//  hName = "NoisyWEvent_"+name;
//  partition.h_NoisyWEventFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO_StdOpt", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventFrac->GetXaxis()->SetTitle("Luminosity Block");
//  groupfracbin.regHist(partition.h_NoisyWEventFrac).ignore();  
//  hName = "temp_NoisyWEvent_"+name;
//  partition.h_NoisyWEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEvent->GetXaxis()->SetTitle("Luminosity Block");
  
  // Fraction of events found noisy per LB after time veto - Std flag
  hName = "NoisyEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as RNB-Standard not vetoed by time window - "+name;
  partition.h_NoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventTimeVetoFrac).ignore();

  hName = "temp_NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB after time veto - Sat flag
  hName = "SaturatedNoisyEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as RNB-Saturated not vetoed by time window - "+ name;
  partition.h_SaturatedNoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventTimeVetoFrac).ignore();

  hName = "temp_SaturatedNoisyEvent_TimeVeto_"+name;
  partition.h_SaturatedNoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_TimeVeto_"+name;
  hTitle =  "Yield of events flagged as MNB-Tight not vetoed by time window - " + name;
  partition.h_MNBTightEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBTightEvent_TimeVeto_"+name;
  partition.h_MNBTightEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTight_PsVetoEvent_TimeVeto_"+name;
  hTitle =  "Yield of events flagged as MNB-Tight_PsVeto not vetoed by time window - " + name;
  partition.h_MNBTight_PsVetoEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTight_PsVetoEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTight_PsVetoEventTimeVetoFrac).ignore();    

  hName = "temp_MNBTight_PsVetoEvent_TimeVeto_"+name;
  partition.h_MNBTight_PsVetoEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTight_PsVetoEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as MNB-Loose not vetoed by time window - " + name;
  partition.h_MNBLooseEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBLooseEvent_TimeVeto_"+name;
  partition.h_MNBLooseEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

//  // Fraction of events found noisy per LB after time veto - Weighted flag
//  hName = "NoisyWEvent_TimeVeto_"+name;
//  partition.h_NoisyWEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as RNB-StandardOpt not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
//  groupfracbin.regHist(partition.h_NoisyWEventTimeVetoFrac).ignore();
//  
//  hName = "temp_NoisyWEvent_TimeVeto_"+name;
//  partition.h_NoisyWEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
  
  if ( m_doTrigger ) 
  {
    unsigned int siz = m_EF_NoiseBurst_Triggers.size();
    hName = "NoisyEventTrigger_"+name;
    hTitle = "Trigger fired for RNB flagged events - "+name;
    partition.h_NoisyEventTrigger = TH1I_LW::create(hName.c_str(), hTitle.c_str(), siz+1, 0.5, siz+1.5);
    LWHist::LWHistAxis* axis = partition.h_NoisyEventTrigger->GetXaxis();
    axis->SetTitle("Special trigger fired");
    for ( size_t i = 0; i < siz; i++)
    {
      axis->SetBinLabel(i+1,m_EF_NoiseBurst_Triggers[i].c_str());
    }
    axis->SetBinLabel(siz+1,"NONE");
    group.regHist(partition.h_NoisyEventTrigger).ignore();


    siz = m_L1_NoiseBurst_Triggers.size();
    hName = "NoisyEventL1Term_"+name;
    hTitle = "L1 term fired for RNB flagged events - "+name;
    partition.h_NoisyEventTriggerL1 = TH1I_LW::create(hName.c_str(), hTitle.c_str(), siz+1, 0.5, siz+1.5);
    axis = partition.h_NoisyEventTriggerL1->GetXaxis();
    axis->SetTitle("L1 term fired");
    for ( size_t i = 0; i < siz; i++)
    {
      axis->SetBinLabel(i+1,m_L1_NoiseBurst_Triggers[i].c_str());
    }
    axis->SetBinLabel(siz+1,"NONE");
    group.regHist(partition.h_NoisyEventTriggerL1).ignore();
  }
}
*/

/*
void LArNoisyROMonAlg::fillTriggerHisto(partitionHistos& partition, uint8_t triggerbits, uint8_t L1triggerbits)
{
  if ( triggerbits ==0 )
  {
    partition.h_NoisyEventTrigger->Fill(double(m_EF_NoiseBurst_Triggers.size()+1));
  }
  else
  {
    for ( size_t i = 0; i < m_EF_NoiseBurst_Triggers.size(); i++)
    {
      if ( triggerbits & (0x1 << i) ) partition.h_NoisyEventTrigger->Fill(double(i+1));
    }
  }

  if ( L1triggerbits ==0 )
  {
    partition.h_NoisyEventTriggerL1->Fill(double(m_L1_NoiseBurst_Triggers.size()+1));
  }
  else
  {
    for ( size_t i = 0; i < m_L1_NoiseBurst_Triggers.size(); i++)
    {
      if ( L1triggerbits & (0x1 << i) ) partition.h_NoisyEventTriggerL1->Fill(double(i+1));
    }
  }

}
*/

