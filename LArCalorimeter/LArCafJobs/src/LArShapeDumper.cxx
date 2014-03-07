/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/LArShapeDumper.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "LArRawEvent/LArDigit.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
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
#include "LArRecConditions/ILArBadChanTool.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
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
  Algorithm(name, pSvcLocator),
  m_log(0),
  m_count(0),
  m_dumperTool("LArShapeDumperTool"),
  m_larCablingSvc("LArCablingService"),
  m_caloNoiseTool("CaloNoiseToolDefault"),
  m_badChannelTool("LArBadChanTool"),
  m_badChannelMasker("BadChannelMasker", this),
  m_adc2mevTool("LArADC2MeVTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
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
  declareProperty("BadChannelTool", m_badChannelTool),
  declareProperty("BadChannelMasker", m_badChannelMasker);
  declareProperty("ADC2MeVTool", m_adc2mevTool);
  declareProperty("DoStream", m_doStream = false);
  declareProperty("DoTrigger", m_doTrigger = true);
  declareProperty("DoOFCIter", m_doOFCIter = true);
  declareProperty("DoAllEvents", m_doAllEvents = true);
  declareProperty("DumpChannelInfos", m_dumpChannelInfos = false);
  declareProperty("DoRoIs", m_doRoIs = true);
  declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
  declareProperty("TriggerNames", m_triggerNames);
  declareProperty("DoAllLvl1", m_doAllLvl1 = true);
}


LArShapeDumper::~LArShapeDumper() 
{
}


StatusCode LArShapeDumper::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  (*m_log) << MSG::DEBUG << "in initialize()" << endreq;

  m_samples = new DataStore();

  StatusCode sc = service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Unable to retrieve pointer to StoreGate Service" << endreq;
    return sc;
  }	
  
  sc = service("DetectorStore", m_detectorStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to locate Service DetectorStore" << endreq;
    return sc;
  }

  sc = m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to retrieve LArCablingService" << endreq;
    return sc;
  }
  
  sc = m_badChannelMasker.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Could not retrieve BadChannelMask " << m_badChannelMasker << endreq;
    return sc;
  }
  
  sc = m_adc2mevTool.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to find tool for LArADC2MeV" << endreq;
    return sc;
  }

  sc = m_caloNoiseTool.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Could not retrieve CaloNoiseTool " << m_caloNoiseTool << endreq;
    return sc;
  }

  sc = m_badChannelTool.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Could not retrieve Bad channel tool " << m_badChannelTool << endreq;
    return sc;
  }

  sc = m_detectorStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure() || !m_onlineHelper) {
    (*m_log) << MSG::FATAL << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  }

  sc = m_detectorStore->retrieve(m_caloDetDescrMgr);
  if (sc.isFailure() || !m_caloDetDescrMgr) {
    (*m_log) << MSG::FATAL << "Could not get CaloDetDescrMgr " << endl << m_detectorStore->dump() << endreq;
    return sc;
  }

  if (m_doTrigger) {
    sc = m_trigDec.retrieve();
    if (sc.isFailure()) {
      (*m_log) << MSG::FATAL << "Could not retrieve TrigDecisionTool " << endreq;
      return sc;
    }

    sc = m_configSvc.retrieve();
    if ( sc.isFailure() ) {
       (*m_log) << MSG::FATAL << "Unable to get pointer to TrigConfigSvc" << endreq;
       return sc;
    }
  }

  sc = m_dumperTool.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Could not retrieve dumper tool " << m_dumperTool << endreq;
    return sc;
  }  

  if (m_dumperTool->doShape()) {
    if (m_count == 1) (*m_log) << MSG::INFO << "Reading LArAutoCorr handle" << endreq;
    sc = m_detectorStore->regHandle(m_autoCorr, "LArAutoCorr");
    if (sc.isFailure()) {
      (*m_log) << MSG::WARNING << "Cannot get register handle for LArAutoCorr" << endl << m_detectorStore->dump() << endreq;
      return sc;
    }
  }
  
  std::transform(m_caloType.begin(), m_caloType.end(), m_caloType.begin(), toupper);
  m_doEM   = (m_caloType.find("EM")   != std::string::npos);
  m_doHEC  = (m_caloType.find("HEC")  != std::string::npos);
  m_doFCAL = (m_caloType.find("FCAL") != std::string::npos);

  std::transform(m_gainSpec.begin(), m_gainSpec.end(), m_gainSpec.begin(), toupper);
  m_gains[CaloGain::LARHIGHGAIN]   = (m_gainSpec.find("HIGH")   != std::string::npos);
  m_gains[CaloGain::LARMEDIUMGAIN] = (m_gainSpec.find("MEDIUM") != std::string::npos);
  m_gains[CaloGain::LARLOWGAIN]    = (m_gainSpec.find("LOW")    != std::string::npos);
  
  return StatusCode::SUCCESS; 
}


StatusCode LArShapeDumper::beginRun()
{
  m_runData = new RunData(0);
  static unsigned int i = 0;

  if (m_doTrigger) {
    ofstream xmlfile(Form("frame_%d.xml", i++));
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
      (*m_log) << MSG::INFO << "Configured chain : " << *chain << endreq;
      for (std::vector<boost::regex>::const_iterator regex = regexs.begin(); regex != regexs.end(); regex++)
        if (boost::regex_match(chain->c_str(), match, *regex)) myChains.push_back(*chain);
    }
    std::vector<std::string> groups = m_trigDec->getListOfGroups();
    for (std::vector<std::string>::const_iterator group = groups.begin(); group != groups.end(); group++)
      (*m_log) << MSG::INFO << "Configured group : " << *group << endreq;
    const Trig::ChainGroup* calibStreamGroup = m_trigDec->getChainGroup("Calibration"); 
    if (calibStreamGroup) {
      std::vector<std::string> chains = calibStreamGroup->getListOfTriggers();
        (*m_log) << MSG::INFO << "Chains for Calibration group:" << endreq;
      for (std::vector<std::string>::const_iterator chain = chains.begin(); chain != chains.end(); chain++)
        (*m_log) << MSG::INFO << "Calib chain : " << *chain << endreq;
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
          (*m_log) << MSG::WARNING << "LVL1 item " << *l1Item << ", obtained from TrigConfig, cannot be retrieved!" << endreq;
          continue;
        }
        int pos = confItem->ctpId();
        if (pos < 0 || pos >= 256) {
          (*m_log) << MSG::WARNING << "LVL1 item " << *l1Item << "has out-of-range ctpId " << pos << endreq;
          continue;
        }
        m_runData->addBit(l1Item->c_str(), pos);
        (*m_log) << MSG::INFO << "Adding LVL1 trigger bit for " << *l1Item << " at position " << pos << endreq;
      }
      idx = 256;
    }
  
    for (std::vector<std::string>::const_iterator name = myChains.begin(); name != myChains.end(); name++) {
      if (m_trigDec->getListOfTriggers(*name).empty()) {
        (*m_log) << MSG::WARNING << "Requested trigger name " << *name << " is not configured in this run" << endreq;
        continue;
      }
      const Trig::ChainGroup* group = m_trigDec->getChainGroup(*name);
      if (!group) {
        (*m_log) << MSG::WARNING << "Could not retrieve chain group for trigger " << *name << endreq;
        continue;
      }
      m_runData->addBit(name->c_str(), idx++);
      m_triggerGroups.push_back(group);
      (*m_log) << MSG::INFO << "Adding trigger bit for " << *name << " at position " << idx-1 << endreq;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::execute()
{    
  if ((m_prescale > 1 && m_random.Rndm() > 1.0/m_prescale) || m_prescale <= 0) {
    (*m_log) << MSG::VERBOSE << "======== prescaling event "<< m_count << " ========" << endreq;
    return StatusCode::SUCCESS;
  }
  m_count++;
  StatusCode sc;
  (*m_log) << MSG::VERBOSE << "======== executing event "<< m_count << " ========" << endreq;

  const EventInfo* eventInfo = 0;
  sc = m_eventStore->retrieve(eventInfo);
  if (!sc.isSuccess() || !eventInfo || !eventInfo->event_ID()) {
    (*m_log) << MSG::FATAL << "Unable to retrieve Event info object" << endreq;
    return sc;
  }
  
  int event     = eventInfo->event_ID()->event_number();
  int run       = eventInfo->event_ID()->run_number();
  int lumiBlock = eventInfo->event_ID()->lumi_block();
  int bunchId   = eventInfo->event_ID()->bunch_crossing_id();

  EventData* eventData = 0;
  int eventIndex = -1;
  if (m_doAllEvents) {
    eventIndex = makeEvent(eventData, run, event, lumiBlock, bunchId);
    if (eventIndex < 0) return StatusCode::FAILURE;
  }

  const LArDigitContainer* larDigitContainer;
  if (m_digitsKey != "")
    sc = m_eventStore->retrieve(larDigitContainer, m_digitsKey);
  else
    sc = m_eventStore->retrieve(larDigitContainer);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << " Cannot read LArDigitContainer from StoreGate! key=" << m_digitsKey << endl << m_eventStore->dump() << endreq;
    return sc;
  }

  if (larDigitContainer->size() == 0) {
    (*m_log) << MSG::WARNING << "LArDigitContainer with key=" << m_digitsKey << " is empty!" << endreq;
    return StatusCode::SUCCESS;
  }

  const LArRawChannelContainer* rawChannelContainer = 0;
  sc = m_eventStore->retrieve(rawChannelContainer, m_channelsKey);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "LArRawChannelContainer not found with key"
        << m_channelsKey << endl << m_eventStore->dump() << endreq;
    return sc;
  }
  
  if (m_count == 1) (*m_log) << MSG::INFO << "Reading pedestal" << endreq;
  sc = m_detectorStore->retrieve(m_larPedestal);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Cannot retrieve pedestal(s) from Conditions Store!" << endl << m_detectorStore->dump()  << endreq;
    return sc;
  }   

  const LArOFIterResultsContainer* ofIterResult = 0;
  if (m_doOFCIter) {
    if (m_count == 1) (*m_log) << MSG::INFO << "Reading LArOFIterResult" << endreq;
    sc = m_eventStore->retrieve(ofIterResult, "LArOFIterResult");
    if (sc.isFailure() || !ofIterResult) {
      (*m_log) << MSG::FATAL << "Cannot retrieve OF Iter results from Event Store!" << endl << m_eventStore->dump() << endreq;
      return sc;
    }  
  }

  if (m_count == 1) (*m_log) << MSG::INFO << "Reading LArFebErrorSummary" << endreq;
  const LArFebErrorSummary* larFebErrorSummary = 0;
  sc = m_eventStore->retrieve(larFebErrorSummary, "LArFebErrorSummary");
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Cannot retrieve Feb error summary." << endreq;
    return sc;
  }
  const std::map<unsigned int,uint16_t>& febErrorMap = larFebErrorSummary->get_all_febs();
  std::map<unsigned int, const LArRawChannel*> channelsToKeep;
  
  for (LArRawChannelContainer::const_iterator channel = rawChannelContainer->begin(); 
       channel != rawChannelContainer->end(); ++channel) 
  {
    if (m_energyCut > 0 && TMath::Abs(channel->energy()) < m_energyCut) continue;
    if (m_badChannelMasker->cellShouldBeMasked(channel->channelID(), channel->gain())) continue;

    IdentifierHash hash = m_onlineHelper->channel_Hash(channel->channelID());
    
    if (!hash.is_valid()) {
      (*m_log) << MSG::FATAL << "Found a LArRawChannel whose HWIdentifier (" << channel->channelID()
          << ") does not correspond to a valid hash -- returning StatusCode::FAILURE."
          << endreq;
      return StatusCode::FAILURE;
    }    
    channelsToKeep[hash] = &*channel;
    if (m_dumpChannelInfos) {
      HistoryContainer* histCont = m_samples->hist_cont(hash);
      CellInfo* info = 0;
      if (!histCont) {
        HWIdentifier channelID = channel->hardwareID();
        const Identifier id = m_larCablingSvc->cnvToIdentifier(channelID);
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
  
  (*m_log) << MSG::INFO << "njpbSizes : " << larDigitContainer->size()
           << " " << (ofIterResult ? ofIterResult->size() : 0) << " " 
           << rawChannelContainer->size() << " " << channelsToKeep.size() << endreq;
  
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
    const bool connected = m_larCablingSvc->isOnlineConnected(channelID);
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
   
    const Identifier id = m_larCablingSvc->cnvToIdentifier(channelID);
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
      status = m_badChannelTool->status(channelID).packedData();
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
        (*m_log) << MSG::WARNING << "AutoCorr object is not of type LArAutoCorrComplete!" << endreq;
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
//       (*m_log) << MSG::INFO << "OFResult for channel 0x" << MSG::hex << channelID << MSG::dec 
//           << " not found. (size was " << ofcResultPosition.size() << ")" << endreq;
    
   
    const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(channelID,gain); //dudu
    data->setADCMax(rawChannel->energy()/ramp[1]); //pow(ADCPeak,i); //dudu
	    

    histCont->add(data);
  }
  
  //(*m_log) << MSG::INFO << "Current footprint = " << m_samples->footprint() << ", size = " << m_samples->size() << endreq;
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::endRun()
{
  m_samples->addRun(m_runData);
  return StatusCode::SUCCESS;
}


StatusCode LArShapeDumper::finalize()
{
  (*m_log) << MSG::DEBUG << "in finalize() " << endreq;

  int n = 0;
  for (unsigned int i = 0; i < m_samples->nChannels(); i++)
    if (m_samples->historyContainer(i)) {
      if (m_samples->historyContainer(i)->cellInfo() == 0)
	(*m_log) << MSG::INFO << "Cell with no cellInfo at index " << i << " !!" << endreq;
      //else if (m_samples->historyContainer(i)->cellInfo()->shape() == 0)
	//(*m_log) << MSG::INFO << "Cell with no ShapeInfo at index " << i << " !!" << endreq;
      //(*m_log) << MSG::INFO << "Non-zero cell at index " << i << " " << m_samples->shape(i)->size() << endreq;
      n++;
    }

  //for (unsigned int i = 0; i < m_samples->nEvents(); i++) {
  //   (*m_log) << MSG::INFO << "Event " << i << " = " 
  //            << m_samples->eventData(i)->run() << " " << m_samples->eventData(i)->event()
  //            << "trigger = " << m_samples->eventData(i)->triggers() << ", nRoIs = " << m_samples->eventData(i)->nRoIs() << endreq;
  // }
  (*m_log) << MSG::INFO << "Non-zero cells = " << n << ", footprint = " << m_samples->footprint() <<  endreq;
  (*m_log) << MSG::INFO << "Writing..." << endreq;

  if (!m_doStream) {
    m_samples->writeTrees(m_fileName.c_str());
/*    TFile* f = TFile::Open(m_fileName.c_str(), "RECREATE");
    (*m_log) << MSG::INFO << "Writing (2)..." << endreq;
    f->WriteObjectAny(m_samples, "Container", "LArSamples");
    (*m_log) << MSG::INFO << "Closing..." << endreq;
    f->Close();
    (*m_log) << MSG::INFO << "Deleting..." << endreq;
    delete m_samples;*/
    (*m_log) << MSG::INFO << "Done!" << endreq;
  }

  delete m_log;
  return StatusCode::SUCCESS;
}


int LArShapeDumper::makeEvent(EventData*& eventData,
    		              int run, int event, 
			      int lumiBlock, int bunchXing) const
{
  std::vector<unsigned int> triggerWords;
  if (m_doTrigger) {
    const ROIB::RoIBResult* l1Result = 0;
    if (m_eventStore->retrieve(l1Result).isFailure() || !l1Result) {
      (*m_log) << MSG::FATAL << "Could not retrieve RoIBResult!" << endreq;
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
      //(*m_log) << MSG::INFO << "Trigger line " << bit->first.Data() << " passed" << endreq;
      }
    }
    //(*m_log) << MSG::INFO << "Trigger words : ";
    //for (unsigned int i = 0; i < triggerWords.size(); i++) (*m_log) << MSG::INFO << triggerWords[i] << " ";
    //(*m_log) << MSG::INFO << endreq;
  }
  
  eventData = new EventData(event, 0, lumiBlock, bunchXing);
  if (m_runData->run() == 0) m_runData->setRun(run);
  eventData->setRunData(m_runData);
  eventData->setTriggerData(triggerWords);
  if (m_doRoIs) {
    //(*m_log) << MSG::INFO << "Filling RoI list" << endreq;
    for (std::vector<const Trig::ChainGroup*>::const_iterator group = m_triggerGroups.begin();
	 group != m_triggerGroups.end(); group++) {
      std::vector<Trig::Feature<TrigRoiDescriptor> > roIs = (*group)->features().get<TrigRoiDescriptor>();
      for (std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator roI = roIs.begin(); roI != roIs.end(); roI++) {
	//(*m_log) << MSG::INFO << "Found an roi for chain ";
        //for (unsigned int i = 0; i < (*group)->getListOfTriggers().size(); i++) cout << (*group)->getListOfTriggers()[i] << " ";
        //cout << "@ " << roI->cptr()->eta() << ", " << roI->cptr()->phi() << ", TE = " 
	//	 << roI->te()->getId() << " " << Trig::getTEName(*roI->te()) << " with label " << roI->label() << endreq;
	eventData->addRoI(roI->cptr()->eta(), roI->cptr()->phi(), (*group)->getListOfTriggers()[0].c_str(), roI->label().c_str());
	//(*m_log) << MSG::INFO << "nRoIs so far = " << eventData->nRoIs() << endreq;
      }
    }
  }
  return m_samples->addEvent(eventData);
}
