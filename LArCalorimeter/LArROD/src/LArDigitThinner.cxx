/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArDigitThinner.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthContainers/ConstDataVector.h"

#include <bitset>
#include <limits>
#include  <cstdlib>

LArDigitThinner::LArDigitThinner(const std::string& name,
				 ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
    , m_onlineID(NULL)
    , m_caloCellId(NULL)
    , m_inputContainerName("LArDigitContainer_MC")
    , m_outputContainerName("LArDigitContainer_MC_Thinned")
    , m_rawChannelContainerName("LArRawChannels")
    , m_digitsPerRegion(1+DISCONNECTED,0)
{
  declareProperty("EnergyCuts_Barrel",  m_energyCuts_barrel);
  declareProperty("EnergyCuts_Endcap",  m_energyCuts_endcap);
  declareProperty("EnergyCut_HEC",  m_energyCut_hec = 5000);
  declareProperty("EnergyCut_FCAL",  m_energyCut_fcal = 20000);
  declareProperty("InputContainerName", m_inputContainerName);
  declareProperty("OutputContainerName",m_outputContainerName);
  declareProperty("RawChannelContainerName",m_rawChannelContainerName);
}

LArDigitThinner::~LArDigitThinner() {
}


StatusCode LArDigitThinner::initialize() {
  
  StatusCode sc = m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve LArCablingService Tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  } 
  
  sc = detStore()->retrieve(m_caloCellId,"CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get CaloCell_ID helper !" << endreq;
    return sc;
  } 
   
  sc = detStore()->regFcn(&LArCablingService::iovCallBack,&(*m_larCablingSvc),
                           &LArDigitThinner::iovCallBack,
                           this,true) ;
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not register callback to cabling service." << endreq;
    return sc;
  }
    
  //Fix up jobOptions if needed:
  int defaultCuts_barrel[] = { 1000, 1000, 1000, 1000 };
  int defaultCuts_endcap[] = { 2000, 2000, 2000, 2000 };

  if (m_energyCuts_barrel.size() != 4) {
    msg(MSG::WARNING) << "Only " << m_energyCuts_barrel.size() 
		      << " energy cut values provided for the endcap : reverting to default" << endreq;
    for (size_t i=0;i<4;++i) m_energyCuts_barrel.push_back(defaultCuts_barrel[i]);
  }

  if (m_energyCuts_endcap.size() != 4) {
    msg(MSG::WARNING) << "Only " << m_energyCuts_endcap.size() 
		      << " energy cut values provided for the endcap : reverting to default" << endreq;
    for (size_t i=0;i<4;++i) m_energyCuts_endcap.push_back(defaultCuts_endcap[i]);
  }
  
  msg(MSG::INFO) << "Energy cuts (Barrel) : ";
  for (unsigned int i = 0; i < 4; i++) msg() <<  m_energyCuts_barrel[i] << " ";
  msg(MSG::INFO) << "GeV" << endreq;
  
  msg() << MSG::INFO << "Energy cuts (Endcap) : ";
  for (unsigned int i = 0; i < 4; i++) msg() <<  m_energyCuts_endcap[i] << " ";
  msg() << MSG::INFO << "GeV" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinner::iovCallBack(IOVSVC_CALLBACK_ARGS) 
{
  msg() << MSG::INFO << " iovCallBack " << endreq;
  initCutValues();
  return StatusCode::SUCCESS;
}

void LArDigitThinner::initCutValues() {

  m_energyCuts.assign(m_onlineID->channelHashMax(),std::numeric_limits<int>::max());
  
  CaloCell_ID::id_iterator it=m_caloCellId->cell_begin(CaloCell_ID::LAREM);
  CaloCell_ID::id_iterator it_e=m_caloCellId->cell_end(CaloCell_ID::LAREM);
  for(;it!=it_e;++it) {
    const Identifier id=(*it);
    HWIdentifier chid=m_larCablingSvc->createSignalChannelID(id);
    IdentifierHash onlHash=m_onlineID->channel_Hash(chid);
    int sampling = m_caloCellId->sampling(id);
    if (sampling < 0) continue;
    if (m_caloCellId->is_em_barrel(id)) {
      m_energyCuts[onlHash]=m_energyCuts_barrel[sampling];
    }
    else { //endcap
      m_energyCuts[onlHash]=m_energyCuts_endcap[sampling];
      continue;
    }
  }//end loop over EM cells;

  it=m_caloCellId->cell_begin(CaloCell_ID::LARHEC);
  it_e=m_caloCellId->cell_end(CaloCell_ID::LARHEC);
  for(;it!=it_e;++it) {
    const Identifier id=(*it);
    HWIdentifier chid=m_larCablingSvc->createSignalChannelID(id);
    IdentifierHash onlHash=m_onlineID->channel_Hash(chid);
    m_energyCuts[onlHash]= m_energyCut_hec;
  }//End loop over HEC cells
  
  it=m_caloCellId->cell_begin(CaloCell_ID::LARFCAL);
  it_e=m_caloCellId->cell_end(CaloCell_ID::LARFCAL);
  for(;it!=it_e;++it) {
    const Identifier id=(*it);
    HWIdentifier chid=m_larCablingSvc->createSignalChannelID(id);
    IdentifierHash onlHash=m_onlineID->channel_Hash(chid);
    m_energyCuts[onlHash]= m_energyCut_fcal;
  }//end loop over FCAL cells

  return;
}


StatusCode LArDigitThinner::finalize()
{
  msg(MSG::INFO) << "LArDigitThinner Finalize" << endreq;  
  msg(MSG::INFO) << "Sampling    0        1          2         3         Total" << endreq;
  msg(MSG::INFO) << "Barrel   ";
  
  int nPassTot = 0;//, nSeenTot = 0;
  
  for (int i = EMB0; i <= EMB3 ; i++) {
    msg() << m_digitsPerRegion[i] << " ";
    nPassTot +=  m_digitsPerRegion[i];
  }
  msg() << MSG::INFO << nPassTot  << endreq;
  
  msg() << MSG::INFO << "Endcap   ";
  nPassTot = 0;
  
  for (int i = EMEC0; i<=EMEC3;++i) {
    msg(MSG::INFO) <<  m_digitsPerRegion[i] << " ";
    nPassTot +=  m_digitsPerRegion[i];
  }
  msg() << nPassTot << endreq;
  
  msg(MSG::INFO) << "HEC      " <<  m_digitsPerRegion[HEC] << endreq;
  msg(MSG::INFO) << "FCAL     " <<  m_digitsPerRegion[FCAL] << endreq;

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinner::execute()
{
  // Create the new digit container
  ConstDataVector<LArDigitContainer>* outputContainer = new ConstDataVector<LArDigitContainer>(SG::VIEW_ELEMENTS);
  if (!outputContainer){
    msg(MSG::ERROR) << "Could not allocate a new LArDigitContainer" << endreq;
    return StatusCode::FAILURE;	  
  }
  
  StatusCode sc = evtStore()->record(outputContainer , m_outputContainerName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not record output LArDigitContainer with key " 
		    << m_outputContainerName << endreq;
    return sc;
  }

  const LArDigitContainer* inputContainer = 0;
  sc = evtStore()->retrieve(inputContainer, m_inputContainerName);
  
  if (sc.isFailure()) { 
    msg(MSG::WARNING) << "Input LArDigitContainer not found with key"
		      << m_inputContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  const LArRawChannelContainer* rawChannelContainer = 0;
  sc = evtStore()->retrieve(rawChannelContainer, m_rawChannelContainerName);
    
  if (sc.isFailure()) {
    msg(MSG::WARNING) << "LArRawChannelContainer not found with key"
		      << m_rawChannelContainerName << endreq;
    return StatusCode::SUCCESS;
  }


  std::bitset<200000> keepSet;

  //start loop over LArRawChannels
  LArRawChannelContainer::const_iterator chanIt = rawChannelContainer->begin();
  LArRawChannelContainer::const_iterator chanIt_e = rawChannelContainer->end(); 
  for (;chanIt!=chanIt_e;++chanIt) {
    const LArRawChannel& chan=(*chanIt);    
    const IdentifierHash onlHash=m_onlineID->channel_Hash(chan.hardwareID());
    if (abs(chan.energy())>=m_energyCuts[onlHash]) {
      keepSet.set(onlHash);
    }
  }//end loop over raw channels


  //start loop over digits
  LArDigitContainer::const_iterator digIt=inputContainer->begin();
  LArDigitContainer::const_iterator digIt_e=inputContainer->end();
  for (;digIt!=digIt_e;++digIt) {
    const IdentifierHash onlHash=m_onlineID->channel_Hash((*digIt)->hardwareID());
    if (keepSet.test(onlHash)) {
      outputContainer->push_back(*digIt);
      if (msgLvl(MSG::INFO) ) {
	const int reg=getRegion((*digIt)->hardwareID());
	++(m_digitsPerRegion[reg]);
      }
    }
  }

  ATH_MSG_DEBUG("Copied " << outputContainer->size() << " of " 
		<< inputContainer->size() << " digits.");

  return StatusCode::SUCCESS;
}

    
int LArDigitThinner::getRegion(const HWIdentifier chid) const {
   if (m_onlineID->isEMBchannel(chid)) {
     const Identifier id= m_larCablingSvc->cnvToIdentifier(chid);
     if (!id.is_valid()) return DISCONNECTED;
     return (EMB0+m_caloCellId->sampling(id));
   }
   
   if (m_onlineID->isEMECchannel(chid)) {
     const Identifier id= m_larCablingSvc->cnvToIdentifier(chid);
     if (!id.is_valid()) return DISCONNECTED;
     return (EMEC0+(Position)m_caloCellId->sampling(id));
   }

   if (m_onlineID->isHECchannel(chid)) {
     return HEC;
   }

   if (m_onlineID->isFCALchannel(chid)) {
     return FCAL;
   }

   return DISCONNECTED;
 }
