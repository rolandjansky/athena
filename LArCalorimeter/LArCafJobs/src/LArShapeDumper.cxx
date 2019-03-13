/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/LArShapeDumper.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawEvent/LArDigit.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "LArRawEvent/LArOFIterResultsContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArShape.h"

#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/RunData.h"
#include "LArCafJobs/EventData.h"
#include "LArRawEvent/LArOFIterResultsContainer.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TFile.h"
#include "TMath.h"

#include "boost/regex.hpp"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// #include "LArCafJobs/DataContainer.h"
// #include "LArCafJobs/HistoryContainer.h"
// #include "LArCafJobs/CellInfo.h"
// #include "LArCafJobs/TreeAccessor.h"


#include <vector>
#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


LArShapeDumper::LArShapeDumper(const std::string & name, ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_count(0),
  m_nWrongBunchGroup(0),
  m_nPrescaledAway(0),
  m_nLArError(0),
  m_nNoDigits(0),
  m_dumperTool("LArShapeDumperTool"),
  m_caloNoiseTool("CaloNoiseToolDefault"),
  m_badChannelMasker("BadChannelMasker", this),
  m_adc2mevTool("LArADC2MeVTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_bcidTool("BunchCrossingTool"),
  m_larPedestal(nullptr),
  m_caloDetDescrMgr(nullptr),
  m_onlineHelper(nullptr),
  m_doEM(false),
  m_doHEC(false),
  m_doFCAL(false),
  m_samples(nullptr),
  m_runData(0)
{
  declareProperty("FileName", m_fileName = "samples.root");
  declareProperty("MaxChannels", m_maxChannels = 200000);
  declareProperty("DigitsKey", m_digitsKey = "FREE");
  declareProperty("ChannelsKey", m_channelsKey = "LArRawChannels");
  declareProperty("Prescale", m_prescale = 1);
  declareProperty("CaloType", m_caloType = "EMHECFCAL");
  declareProperty("EnergyCut", m_energyCut = -1);
  declareProperty("NoiseSignifCut", m_noiseSignifCut = 3);
  declareProperty("MinADCMax", m_minADCMax = -1);
  declareProperty("Gains", m_gainSpec = "HIGH,MEDIUM,LOW");
  declareProperty("DumpDisconnected", m_dumpDisc = false);
  declareProperty("CaloNoiseTool", m_caloNoiseTool);
  declareProperty("BadChannelMasker", m_badChannelMasker);
  declareProperty("ADC2MeVTool", m_adc2mevTool);
  declareProperty("BunchCrossingTool",m_bcidTool);
  declareProperty("DoStream", m_doStream = false);
  declareProperty("DoTrigger", m_doTrigger = true);
  declareProperty("DoOFCIter", m_doOFCIter = true);
  declareProperty("DoAllEvents", m_doAllEvents = true);
  declareProperty("DumpChannelInfos", m_dumpChannelInfos = false);
  declareProperty("DoRoIs", m_doRoIs = true);
  declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
  declareProperty("TriggerNames", m_triggerNames);
  declareProperty("DoAllLvl1", m_doAllLvl1 = true);
  declareProperty("onlyEmptyBC",m_onlyEmptyBC=false);
}


LArShapeDumper::~LArShapeDumper() 
{
}


StatusCode LArShapeDumper::initialize()
{
  ATH_MSG_DEBUG ("in initialize()");

  m_samples = new DataStore();

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_BCKey.initialize() );

  ATH_CHECK( m_badChannelMasker.retrieve() );
  ATH_CHECK( m_adc2mevTool.retrieve() );
  ATH_CHECK( m_caloNoiseTool.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_caloDetDescrMgr) );

  if (m_doTrigger) {
    ATH_CHECK( m_trigDec.retrieve() );
    ATH_CHECK( m_configSvc.retrieve() );
  }

  ATH_CHECK( m_dumperTool.retrieve() );

  if (m_dumperTool->doShape()) {
    ATH_CHECK( detStore()->regHandle(m_autoCorr, "LArAutoCorr") );
  }
  
  std::transform(m_caloType.begin(), m_caloType.end(), m_caloType.begin(), toupper);
  m_doEM   = (m_caloType.find("EM")   != std::string::npos);
  m_doHEC  = (m_caloType.find("HEC")  != std::string::npos);
  m_doFCAL = (m_caloType.find("FCAL") != std::string::npos);

  std::transform(m_gainSpec.begin(), m_gainSpec.end(), m_gainSpec.begin(), toupper);
  m_gains[CaloGain::LARHIGHGAIN]   = (m_gainSpec.find("HIGH")   != std::string::npos);
  m_gains[CaloGain::LARMEDIUMGAIN] = (m_gainSpec.find("MEDIUM") != std::string::npos);
  m_gains[CaloGain::LARLOWGAIN]    = (m_gainSpec.find("LOW")    != std::string::npos);
  
  if (m_onlyEmptyBC)
    ATH_CHECK(m_bcidTool.retrieve());

  return StatusCode::SUCCESS; 
}


StatusCode LArShapeDumper::beginRun()
{
  m_runData = new RunData(0);
  static unsigned int i = 0;

  if (m_doTrigger) {
    std::ofstream xmlfile(Form("frame_%d.xml", i++));
    xmlfile << "      <SEQUENCE_LIST>" << std::endl; 
    for(TrigConf::HLTSequence *seq : m_configSvc->sequences())
       seq->writeXML(xmlfile);
    xmlfile << "      </SEQUENCE_LIST>" << std::endl; 

    //chains
    xmlfile << "      <CHAIN_LIST>" << std::endl; 
    for(TrigConf::HLTChain *ch: m_configSvc->chains() )
       ch->writeXML(xmlfile);

    xmlfile << "      </CHAIN_LIST>" << std::endl; 

    std::vector<boost::regex> regexs;
    for (std::vector<std::string>::const_iterator name = m_triggerNames.begin(); 
         name != m_triggerNames.end(); name++)
      regexs.push_back(boost::regex(*name));

    std::vector<std::string> chains = m_trigDec->getListOfTriggers();
    std::vector<std::string> myChains;
    boost::cmatch match;

    for (std::vector<std::string>::const_iterator chain = chains.begin(); chain != chains.end(); chain++) {
      ATH_MSG_INFO ( "Configured chain : " << *chain );
      for (std::vector<boost::regex>::const_iterator regex = regexs.begin(); regex != regexs.end(); regex++)
        if (boost::regex_match(chain->c_str(), match, *regex)) myChains.push_back(*chain);
    }
    std::vector<std::string> groups = m_trigDec->getListOfGroups();
    for (std::vector<std::string>::const_iterator group = groups.begin(); group != groups.end(); group++)
      ATH_MSG_INFO ( "Configured group : " << *group );
    const Trig::ChainGroup* calibStreamGroup = m_trigDec->getChainGroup("Calibration"); 
    if (calibStreamGroup) {
      std::vector<std::string> chains = calibStreamGroup->getListOfTriggers();
      ATH_MSG_INFO ( "Chains for Calibration group:" );
      for (std::vector<std::string>::const_iterator chain = chains.begin(); chain != chains.end(); chain++)
        ATH_MSG_INFO ( "Calib chain : " << *chain );
    }

    unsigned int idx = 0;
  
    if (m_doAllLvl1) {
      m_trigDec->ExperimentalAndExpertMethods()->enable();
      const Trig::ChainGroup* group = m_trigDec->getChainGroup("L1_.*");
      std::vector<std::string> l1Triggers = group->getListOfTriggers();
      for (std::vector<std::string>::const_iterator l1Item = l1Triggers.begin(); 
           l1Item != l1Triggers.end(); l1Item++) {
        const TrigConf::TriggerItem* confItem = m_trigDec->ExperimentalAndExpertMethods()->getItemConfigurationDetails(*l1Item);
        if (!confItem) {
          ATH_MSG_WARNING ( "LVL1 item " << *l1Item << ", obtained from TrigConfig, cannot be retrieved!" );
          continue;
        }
        int pos = confItem->ctpId();
        if (pos < 0 || pos >= 256) {
          ATH_MSG_WARNING ( "LVL1 item " << *l1Item << "has out-of-range ctpId " << pos );
          continue;
        }
        m_runData->addBit(l1Item->c_str(), pos);
        ATH_MSG_INFO ( "Adding LVL1 trigger bit for " << *l1Item << " at position " << pos );
      }
      idx = 256;
    }
  
    for (std::vector<std::string>::const_iterator name = myChains.begin(); name != myChains.end(); name++) {
      if (m_trigDec->getListOfTriggers(*name).empty()) {
        ATH_MSG_WARNING ( "Requested trigger name " << *name << " is not configured in this run" );
        continue;
      }
      const Trig::ChainGroup* group = m_trigDec->getChainGroup(*name);
      if (!group) {
        ATH_MSG_WARNING ( "Could not retrieve chain group for trigger " << *name );
        continue;
      }
      m_runData->addBit(name->c_str(), idx++);
      m_triggerGroups.push_back(group);
      ATH_MSG_INFO ( "Adding trigger bit for " << *name << " at position " << idx-1 );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::execute()
{    
  m_count++;

  if ((m_prescale > 1 && m_random.Rndm() > 1.0/m_prescale) || m_prescale <= 0) {
    ATH_MSG_VERBOSE ( "======== prescaling event "<< m_count << " ========" );
    m_nPrescaledAway++;
    return StatusCode::SUCCESS;
  }

  ATH_MSG_VERBOSE ( "======== executing event "<< m_count << " ========" );

  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );
  
  int event     = eventInfo->eventNumber();
  int run       = eventInfo->runNumber();
  int lumiBlock = eventInfo->lumiBlock();
  int bunchId   = eventInfo->bcid();

  
  if (m_onlyEmptyBC) {
    const Trig::IBunchCrossingTool::BunchCrossingType bcType=m_bcidTool->bcType(bunchId);
    if (bcType!=Trig::IBunchCrossingTool::BunchCrossingType::Empty) {
      ATH_MSG_DEBUG("Ignoring Event with bunch crossing type " << bcType);
      m_nWrongBunchGroup++;
      return StatusCode::SUCCESS;
    }
  }

  if (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) {
    ATH_MSG_DEBUG("Ignoring Event b/c of LAr ERROR");
    m_nLArError++;
    return StatusCode::SUCCESS;
  }

  EventData* eventData = 0;
  int eventIndex = -1;
  if (m_doAllEvents) {
    eventIndex = makeEvent(eventData, run, event, lumiBlock, bunchId);
    if (eventIndex < 0) return StatusCode::FAILURE;
  }

  const LArDigitContainer* larDigitContainer;
  if (m_digitsKey != "")
    ATH_CHECK( evtStore()->retrieve(larDigitContainer, m_digitsKey) );
  else
    ATH_CHECK( evtStore()->retrieve(larDigitContainer) );

  if (larDigitContainer->size() == 0) {
    ATH_MSG_WARNING ( "LArDigitContainer with key=" << m_digitsKey << " is empty!" );
    m_nNoDigits++;
    return StatusCode::SUCCESS;
  }

  const LArRawChannelContainer* rawChannelContainer = 0;
  ATH_CHECK( evtStore()->retrieve(rawChannelContainer, m_channelsKey) );
  
  ATH_CHECK( detStore()->retrieve(m_larPedestal) );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  const LArOFIterResultsContainer* ofIterResult = 0;
  if (m_doOFCIter) {
    ATH_CHECK( evtStore()->retrieve(ofIterResult, "LArOFIterResult") );
  }

  const LArFebErrorSummary* larFebErrorSummary = 0;
  ATH_CHECK( evtStore()->retrieve(larFebErrorSummary, "LArFebErrorSummary") );
  const std::map<unsigned int,uint16_t>& febErrorMap = larFebErrorSummary->get_all_febs();
  std::map<unsigned int, const LArRawChannel*> channelsToKeep;
  
  for (LArRawChannelContainer::const_iterator channel = rawChannelContainer->begin(); 
       channel != rawChannelContainer->end(); ++channel) 
  {
    if (m_energyCut > 0 && TMath::Abs(channel->energy()) < m_energyCut) continue;
    if (m_badChannelMasker->cellShouldBeMasked(channel->channelID(), channel->gain())) continue;

    IdentifierHash hash = m_onlineHelper->channel_Hash(channel->channelID());
    
    if (!hash.is_valid()) {
      ATH_MSG_FATAL ( "Found a LArRawChannel whose HWIdentifier (" << channel->channelID()
                      << ") does not correspond to a valid hash -- returning StatusCode::FAILURE." );
      return StatusCode::FAILURE;
    }    
    channelsToKeep[hash] = &*channel;
    if (m_dumpChannelInfos) {
      HistoryContainer* histCont = m_samples->hist_cont(hash);
      CellInfo* info = 0;
      if (!histCont) {
        HWIdentifier channelID = channel->hardwareID();
        const Identifier id = cabling->cnvToIdentifier(channelID);
        const CaloDetDescrElement* caloDetElement = m_caloDetDescrMgr->get_element(id);
        info = m_dumperTool->makeCellInfo(channelID, id, caloDetElement);
        if (!info) continue;
        m_samples->makeNewHistory(hash, info);
      }      
    }
  }

  std::map<HWIdentifier, LArOFIterResultsContainer::const_iterator> ofcResultPosition;
  if (m_doOFCIter) 
    for (LArOFIterResultsContainer::const_iterator ofResult = ofIterResult->begin();
         ofResult != ofIterResult->end(); ++ofResult) 
      ofcResultPosition[ofResult->getChannelID()] = ofResult;
  
  ATH_MSG_INFO ( "njpbSizes : " << larDigitContainer->size()
                 << " " << (ofIterResult ? ofIterResult->size() : 0) << " " 
                 << rawChannelContainer->size() << " " << channelsToKeep.size() );

  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(!bcCont) {
     ATH_MSG_ERROR( "Do not have Bad chan container " << m_BCKey.key() );
     return StatusCode::FAILURE;
  }
  
  for (LArDigitContainer::const_iterator digit = larDigitContainer->begin();
       digit != larDigitContainer->end(); ++digit) 
  {    
    //Check Energy selection
    IdentifierHash hash = m_onlineHelper->channel_Hash((*digit)->channelID());
    
    std::map<unsigned int, const LArRawChannel*>::const_iterator findChannel = channelsToKeep.find(hash);
    if (findChannel == channelsToKeep.end()) continue;
    const LArRawChannel* rawChannel = findChannel->second;

    //Check detector part
    HWIdentifier channelID = (*digit)->hardwareID();
    if ((m_onlineHelper->isEMBchannel(channelID) || m_onlineHelper->isEMECchannel(channelID)) && !m_doEM) continue;
    if (m_onlineHelper->isHECchannel(channelID) && !m_doHEC) continue;
    if (m_onlineHelper->isFCALchannel(channelID) && !m_doFCAL) continue;

    //Check gain
    CaloGain::CaloGain gain=(*digit)->gain();
    if (gain >= CaloGain::LARNGAIN || m_gains[gain] == false) continue;

    //Check if connected
    const bool connected = cabling->isOnlineConnected(channelID);
    if (!connected && !m_dumpDisc) continue;
   
    // Check ADCMax selection
    float pedestal = m_larPedestal->pedestal(channelID, gain);
    float pedestalRMS = m_larPedestal->pedestalRMS(channelID, gain);
    if (m_minADCMax > 0 || m_noiseSignifCut > 0) {
      const std::vector<short>& samples = (*digit)->samples();
      double maxValue = -1;
      for (std::vector<short>::const_iterator sample = samples.begin(); 
           sample != samples.end(); sample++)
        if (*sample - pedestal > maxValue) maxValue = *sample - pedestal;
      if (m_minADCMax > 0 && fabs(maxValue) < m_minADCMax) continue;
      if (m_noiseSignifCut > 0 && fabs(maxValue) < pedestalRMS*m_noiseSignifCut) continue;
    }
   
    const Identifier id = cabling->cnvToIdentifier(channelID);
    const CaloDetDescrElement* caloDetElement = 0;
  
    HistoryContainer* histCont = m_samples->hist_cont(hash);
    CellInfo* info = 0;
    if (!histCont) {
      if (!caloDetElement) caloDetElement = m_caloDetDescrMgr->get_element(id);
      info = m_dumperTool->makeCellInfo(channelID, id, caloDetElement);
      if (!info) continue;
      histCont = m_samples->makeNewHistory(hash, info);
    }
    else 
      info = histCont->cell_info();

    float noise = -1;
    unsigned int status = 0xFFFFFFFF;
    if (connected) {
      if (!caloDetElement) caloDetElement = m_caloDetDescrMgr->get_element(id);
      noise = m_caloNoiseTool->getNoise(caloDetElement, ICalorimeterNoiseTool::TOTALNOISE);
      status = bcCont->status(channelID).packedData();
      HWIdentifier febId = m_onlineHelper->feb_Id(m_onlineHelper->feedthrough_Id(channelID), m_onlineHelper->slot(channelID));
      std::map<unsigned int,uint16_t>::const_iterator findError = febErrorMap.find(febId.get_identifier32().get_compact());
      if (findError != febErrorMap.end()) {
	unsigned int febErrWord = findError->second;
	status = status & (febErrWord >> 17);
      }
    }

    //std::vector<float> autoCorr;
    ILArAutoCorr::AutoCorrRef_t autoCorr;
    if (m_dumperTool->doShape()) {
      const LArAutoCorrComplete* autoCorrObj = dynamic_cast<const LArAutoCorrComplete*>(m_autoCorr.cptr());
      if (!autoCorrObj)
        ATH_MSG_WARNING ( "AutoCorr object is not of type LArAutoCorrComplete!" );
      else
        autoCorr = autoCorrObj->autoCorr(channelID, gain);
    }

    if (!info->shape((*digit)->gain())) // this happens if doAllShapes is off
      info->setShape((*digit)->gain(), m_dumperTool->retrieveShape(channelID, gain));
    
    if (!eventData) {
      eventIndex = makeEvent(eventData, run, event, lumiBlock, bunchId); // this happens if doAllEvents is off
      if (eventIndex < 0) return StatusCode::FAILURE;
    }
    
    DataContainer* data = 
        new DataContainer((*digit)->gain(), (*digit)->samples(),
                             rawChannel->energy(),
                             rawChannel->time()/double(1000),
                             rawChannel->quality(),    
                             eventIndex,
                             autoCorr, 
			     noise, pedestal, pedestalRMS, status);
    
   //  std::map<HWIdentifier, LArOFIterResultsContainer::const_iterator>::const_iterator findResult = ofcResultPosition.find(channelID);    
//     if (findResult != ofcResultPosition.end()) {
//       LArOFIterResultsContainer::const_iterator ofResult = findResult->second;
//       if (ofResult->getValid() && ofResult->getConverged())
//         data->setADCMax(ofResult->getAmplitude());
//     }
//     else
//       msg() << MSG::INFO << "OFResult for channel 0x" << MSG::hex << channelID << MSG::dec 
//           << " not found. (size was " << ofcResultPosition.size() << ")" << endmsg;
    
   
    const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(channelID,gain); //dudu
    data->setADCMax(rawChannel->energy()/ramp[1]); //pow(ADCPeak,i); //dudu
	    

    histCont->add(data);
  }
  
  //msg() << MSG::INFO << "Current footprint = " << m_samples->footprint() << ", size = " << m_samples->size() << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::endRun()
{
  m_samples->addRun(m_runData);
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::finalize()
{
  ATH_MSG_DEBUG ("in finalize() ");

  if (m_prescale>1) ATH_MSG_INFO("Prescale dropped " <<  m_nPrescaledAway << "/" << m_count << " events"); 
  if (m_onlyEmptyBC) ATH_MSG_INFO("Dropped " << m_nWrongBunchGroup << "/" << m_count << " events b/c of wrong bunch group"); 
  ATH_MSG_INFO("Dropped " << m_nLArError << "/" <<  m_count << " Events b/c of LAr Veto (Noise burst or corruption)");


  int n = 0;
  for (unsigned int i = 0; i < m_samples->nChannels(); i++)
    if (m_samples->historyContainer(i)) {
      if (m_samples->historyContainer(i)->cellInfo() == 0)
	ATH_MSG_INFO ( "Cell with no cellInfo at index " << i << " !!" );
      //else if (m_samples->historyContainer(i)->cellInfo()->shape() == 0)
	//msg() << MSG::INFO << "Cell with no ShapeInfo at index " << i << " !!" << endmsg;
      //msg() << MSG::INFO << "Non-zero cell at index " << i << " " << m_samples->shape(i)->size() << endmsg;
      n++;
    }

  //for (unsigned int i = 0; i < m_samples->nEvents(); i++) {
  //   msg() << MSG::INFO << "Event " << i << " = " 
  //            << m_samples->eventData(i)->run() << " " << m_samples->eventData(i)->event()
  //            << "trigger = " << m_samples->eventData(i)->triggers() << ", nRoIs = " << m_samples->eventData(i)->nRoIs() << endmsg;
  // }
  ATH_MSG_INFO ( "Non-zero cells = " << n << ", footprint = " << m_samples->footprint() );
  ATH_MSG_INFO ( "Writing..." );

  if (!m_doStream) {
    m_samples->writeTrees(m_fileName.c_str());
/*    TFile* f = TFile::Open(m_fileName.c_str(), "RECREATE");
    msg() << MSG::INFO << "Writing (2)..." << endmsg;
    f->WriteObjectAny(m_samples, "Container", "LArSamples");
    msg() << MSG::INFO << "Closing..." << endmsg;
    f->Close();
    msg() << MSG::INFO << "Deleting..." << endmsg;
    delete m_samples;*/
    msg() << MSG::INFO << "Done!" << endmsg;
  }

  return StatusCode::SUCCESS;
}


int LArShapeDumper::makeEvent(EventData*& eventData,
    		              int run, int event, 
			      int lumiBlock, int bunchXing) const
{
  std::vector<unsigned int> triggerWords;
  if (m_doTrigger) {
    const ROIB::RoIBResult* l1Result = 0;
    if (evtStore()->retrieve(l1Result).isFailure() || !l1Result) {
      ATH_MSG_FATAL ( "Could not retrieve RoIBResult!" );
      return -1;
    }
    const std::vector<ROIB::CTPRoI> tav = l1Result->cTPResult().TAV();
    for (std::vector<ROIB::CTPRoI>::const_iterator word = tav.begin(); word != tav.end(); word++)    
      triggerWords.push_back(word->roIWord());

    for (std::map<TString, unsigned int>::const_iterator bit = m_runData->triggerConfig().begin();
	 bit != m_runData->triggerConfig().end(); bit++) {
      while (triggerWords.size() <= bit->second/32) triggerWords.push_back(0);
      if (m_trigDec->isPassed(bit->first.Data())) {
	triggerWords[bit->second/32] |= (0x1 << (bit->second % 32));
      //msg() << MSG::INFO << "Trigger line " << bit->first.Data() << " passed" << endmsg;
      }
    }
    //msg() << MSG::INFO << "Trigger words : ";
    //for (unsigned int i = 0; i < triggerWords.size(); i++) msg() << MSG::INFO << triggerWords[i] << " ";
    //msg() << MSG::INFO << endmsg;
  }
  
  eventData = new EventData(event, 0, lumiBlock, bunchXing);
  if (m_runData->run() == 0) m_runData->setRun(run);
  eventData->setRunData(m_runData);
  eventData->setTriggerData(triggerWords);
  if (m_doRoIs) {
    //msg() << MSG::INFO << "Filling RoI list" << endmsg;
    for (std::vector<const Trig::ChainGroup*>::const_iterator group = m_triggerGroups.begin();
	 group != m_triggerGroups.end(); group++) {
      std::vector<Trig::Feature<TrigRoiDescriptor> > roIs = (*group)->features().get<TrigRoiDescriptor>();
      for (std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator roI = roIs.begin(); roI != roIs.end(); roI++) {
	//msg() << MSG::INFO << "Found an roi for chain ";
        //for (unsigned int i = 0; i < (*group)->getListOfTriggers().size(); i++) cout << (*group)->getListOfTriggers()[i] << " ";
        //cout << "@ " << roI->cptr()->eta() << ", " << roI->cptr()->phi() << ", TE = " 
	//	 << roI->te()->getId() << " " << Trig::getTEName(*roI->te()) << " with label " << roI->label() << endmsg;
	eventData->addRoI(roI->cptr()->eta(), roI->cptr()->phi(), (*group)->getListOfTriggers()[0].c_str(), roI->label().c_str());
	//msg() << MSG::INFO << "nRoIs so far = " << eventData->nRoIs() << endmsg;
      }
    }
  }
  return m_samples->addEvent(eventData);
}
