/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnlDbPrep/LArDSPThresholdFillAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGate.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArTools/LArCablingService.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include <fstream>


LArDSPThresholdFillAlg::LArDSPThresholdFillAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_detStore(0),
  m_onlineID(0),
  m_noisetool("CaloNoiseToolDefault"),
  m_cablingSvc("LArCablingService"),
  m_badChannelMasker("LArBadChannelMasker"),
  m_workmode (FIXED)
{

  declareProperty("Key",m_key="DSPThresholds");
  declareProperty("mode",m_mode="fixed",
		  "Select how to set thresholds. Allowed values are 'fixed','group' and 'noise'");

  //For mode 'fixed'
  declareProperty("tQThreshold",m_tqThrsh=250);
  declareProperty("samplesThreshold",m_samplesThrsh=1000);

  //For mode 'group'
  declareProperty("ThresholdsPerCellGroup",m_cellGroupStr);

  //For mode 'Noise'
  declareProperty("sigmaNoiseSamples",m_sigmaNoiseSamples);
  declareProperty("sigmaNoiseQt",m_sigmaNoiseQt);
  declareProperty("usePileupNoiseSamples",m_usePileupNoiseSamples);
  declareProperty("usePileupNoiseQt",m_usePileupNoiseQt);
  declareProperty("NoiseTool",m_noisetool);

  //For channel masking
  declareProperty("MaskBadChannels",m_maskBadChannels=false);
  declareProperty("BadChannelMasker",m_badChannelMasker);
  declareProperty("MaskedtQThreshold",m_maskedtqThrsh=0x7fffffff);
  declareProperty("MaskedsamplesThreshold",m_maskedsamplesThrsh=0x7fffffff);

  declareProperty("GroupingType",m_groupingType="Single");
  declareProperty("Dump",m_dump=false);
  declareProperty("Fill",m_fill=true);
  declareProperty("OutFile",m_outFileName="out.txt");
  //declareProperty("InFile",m_inFileName="");
}

LArDSPThresholdFillAlg::~LArDSPThresholdFillAlg() {}


StatusCode LArDSPThresholdFillAlg::initialize() {

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "start initialize()" << endreq;

  StatusCode sc = service("DetectorStore", m_detStore);
  if (!sc.isSuccess()) {
    log <<MSG::ERROR <<"Could not find Detector Store" <<endreq;
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve(m_onlineID,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve LArOnlineID from DetectorStore" 
	 << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_cablingSvc.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve cabling Svc" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_maskBadChannels){
    sc = m_badChannelMasker.retrieve();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve LArBadChannelMasker" << endreq;
      return StatusCode::FAILURE;
    }
  }

  if (m_mode.compare("fixed")==0) {
    m_workmode=FIXED;
    log << MSG::INFO << "Will used fixed values defined by jobO " 
	<< name() <<".tQThreshold and "
	<< name() << ".samplesThreshold for DSP thresholds" << endreq;
  }
  else if (m_mode.compare("group")==0) {
    m_workmode=GROUP;
    log << MSG::INFO << "Will used cell groups defined in jobO " << name() << ".CellGroup for DSP thresholds" << endreq;
    const CaloCell_ID* caloCellID;
    sc = m_detStore->retrieve(caloCellID,"CaloCell_ID");
    if (sc.isFailure()) {
      log  << MSG::ERROR << "Unable to retrieve CaloCell_ID from DetectorStore" 
	   << endreq;
      return StatusCode::FAILURE;
    }

    if (m_thrPerCell.setDefinition(caloCellID,m_cellGroupStr,log)==false)
      return StatusCode::FAILURE;
    m_thrPerCell.printDef();

    if (m_thrPerCell.getDefaults().size()!=2) {
      log << MSG::ERROR << "Expected 2 values per cell group, got " << m_thrPerCell.getDefaults().size() << endreq;
      return StatusCode::FAILURE;
    }
  }
  else if (m_mode.compare("noise")==0) {
    m_workmode=NOISE;

    if(m_usePileupNoiseSamples)
      log << MSG::INFO << "Will used ICaloCellTool::totalNoiseRMS times " << name() << ".sigmaNoiseSamples and" << endreq;
    else
      log << MSG::INFO << "Will used ICaloCellTool::elecNoiseRMS times " << name() << ".sigmaNoiseSamples and" << endreq;

    if(m_usePileupNoiseQt)
      log << MSG::INFO << "ICaloCellTool::totalNoiseRMS times " << name() << ".sigmaNoiseQt for DSP thresholds" << endreq;
    else
      log << MSG::INFO << "ICaloCellTool::elecNoiseRMS times " << name() << ".sigmaNoiseQt for DSP thresholds" << endreq;

    sc = m_noisetool.retrieve();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve CaloNoiseTool" << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}   

StatusCode LArDSPThresholdFillAlg::stop() {

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "start stop()" << endreq;

  StatusCode sc;
   
  if (m_fill) { //Fill database

    log << MSG::INFO << "Filling database" << endreq;

    LArDSPThresholdsComplete* cont=new LArDSPThresholdsComplete();
    sc=cont->setGroupingType(m_groupingType,log);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Failed to set groupingType for LArDSPThresholdsComplete object" << endreq;
      return sc;
    }
     
    sc=cont->initialize(); 
    if (sc.isFailure()) {
      log << MSG::ERROR << "Failed initialize LArDSPThresholdsComplete object" << endreq;
      return sc;
    }

    sc=m_detStore->record(cont,m_key);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Failed to record LArDSPThresholdsComplete object with key " << m_key << " to DetectorStore!" << endreq;
      return sc;
    }

    const CaloDetDescrManager *theCaloDDM = CaloDetDescrManager::instance();
    if(!theCaloDDM){
      log << MSG::ERROR << "Failed to return CaloDetDescrManager" << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::INFO << "theCaloDDM retrieved" << endreq;

    std::vector<HWIdentifier>::const_iterator chanIt=m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator chanIt_e=m_onlineID->channel_end();
    for (;chanIt!=chanIt_e;++chanIt) {
      const HWIdentifier chid=*chanIt;

      Identifier id=m_cablingSvc->cnvToIdentifier(chid);
      log << MSG::DEBUG << "cell id: " << id << " " << m_cablingSvc->isOnlineConnected(chid) << endreq;

      if(!m_cablingSvc->isOnlineConnected(chid)){
	log << MSG::DEBUG << "cell id: " << id << " not connected channel, skip  " << endreq;
	// Same (very high) thresholds for disconnected channels as masked channels
	cont->set(chid,m_maskedtqThrsh,m_maskedsamplesThrsh,m_maskedtqThrsh);
	continue;
      }

      if(m_maskBadChannels && m_badChannelMasker->cellShouldBeMasked(chid)){ // Default gain is CaloGain::UNKNOWNGAIN
	log << MSG::DEBUG << "cell id: " << id << " is masked; set thresholds to " << m_maskedtqThrsh << ", " << m_maskedsamplesThrsh << endreq;
	cont->set(chid,m_maskedtqThrsh,m_maskedsamplesThrsh,m_maskedtqThrsh);
	continue;
      }

      if (m_workmode==FIXED)
	cont->set(chid,m_tqThrsh,m_samplesThrsh,m_tqThrsh); 
      else if (m_workmode==GROUP) {
	const std::vector<float>& thrs=m_thrPerCell.valuesForCell(id);
	if(thrs.size()!=2) std::cout << "len=" << thrs.size() << std::endl;
	cont->set(chid,thrs[0],thrs[1],thrs[0]); 
      }
      else if (m_workmode==NOISE) {

	const CaloDetDescrElement *caloDDE = theCaloDDM->get_element(id);
	if(!caloDDE){
	  log << MSG::ERROR << "Failed to return CaloDetDescrElement" << endreq;
	  return StatusCode::FAILURE;
	}
	if(caloDDE){
	  if(caloDDE->is_lar_em_barrel())
	    log << MSG::DEBUG << "EM barrel" << endreq;

	  if(caloDDE->is_lar_em_endcap())
	    log << MSG::DEBUG << "EM endcap" << endreq;

	  if(caloDDE->is_lar_hec())
	    log << MSG::DEBUG << "HEC" << endreq;

	  if(caloDDE->is_lar_fcal())
	    log << MSG::DEBUG << "FCAL" << endreq;

	  CaloGain::CaloGain igain = CaloGain::INVALIDGAIN;

	  if(caloDDE->is_lar_fcal())
	    igain = CaloGain::LARMEDIUMGAIN;
	  else
	    igain = CaloGain::LARHIGHGAIN;

	  log << MSG::DEBUG << "hash, eta, phi: " << caloDDE->calo_hash() << ", " << caloDDE->eta() << ", " << caloDDE->phi() << "; noise: " << m_noisetool->totalNoiseRMS(caloDDE,igain) << endreq;

	  float samplesThr = 0.;
	  float QtThr = 0.;

	  if(m_usePileupNoiseSamples)
	    samplesThr = m_noisetool->totalNoiseRMS(caloDDE,igain);
	  else
	    samplesThr = m_noisetool->elecNoiseRMS(caloDDE,igain,-1);

	  if(m_usePileupNoiseQt)
	    QtThr = m_noisetool->totalNoiseRMS(caloDDE,igain);
	  else
	    QtThr = m_noisetool->elecNoiseRMS(caloDDE,igain,-1);

	  cont->set(chid,QtThr*m_sigmaNoiseQt,samplesThr*m_sigmaNoiseSamples,QtThr*m_sigmaNoiseQt);

	  //cont->set(chid,(m_noisetool->totalNoiseRMS(caloDDE,CaloGain::LARHIGHGAIN))*m_sigmaNoiseQt,(m_noisetool->totalNoiseRMS(caloDDE,CaloGain::LARHIGHGAIN))*m_sigmaNoiseSamples,(m_noisetool->totalNoiseRMS(caloDDE,CaloGain::LARHIGHGAIN))*m_sigmaNoiseQt);
	}
      }
    }
  }//end if FILL
  
  if (m_dump) {
    MsgStream log(msgSvc(),name());
    const  LArDSPThresholdsComplete* cont;
    StatusCode sc=m_detStore->retrieve(cont,m_key);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Failed to retrieve LArDSPThresholdsComplete object with key " << m_key << " from DetectorStore!" << endreq;
      return sc;
    }

    std::ostream *out = &(std::cout); 
    std::ofstream outfile;
    if (m_outFileName.size()) {
      outfile.open(m_outFileName.c_str(),std::ios::out);
      if (outfile.is_open()) {
	log << MSG::INFO << "Writing to file " << m_outFileName << endreq;
	out = &outfile;
      }
      else
	log << MSG::ERROR << "Failed to open file " << m_outFileName << endreq;
    }

    std::vector<HWIdentifier>::const_iterator chanIt=m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator chanIt_e=m_onlineID->channel_end ();
    for (;chanIt!=chanIt_e;++chanIt) {
      const HWIdentifier chid=*chanIt;    
      (*out) << std::fixed << chid.get_compact() << " " << cont->tQThr(chid) << " " << cont->samplesThr(chid) << " " << cont->trigSumThr(chid) 
	     << std::endl;

    } //end if loop over channels

    if (outfile.is_open())
      outfile.close();
  }// end if m_dump
  return StatusCode::SUCCESS;
}
