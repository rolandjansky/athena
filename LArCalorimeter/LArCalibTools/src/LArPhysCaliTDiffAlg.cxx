/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArPhysCaliTDiffAlg.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"

//#include <ifstream>
#include <fstream>

LArPhysCaliTDiffAlg::LArPhysCaliTDiffAlg(const std::string & name, ISvcLocator * pSvcLocator) :
  LArCond2NtupleBase(name,pSvcLocator) {
  
  declareProperty("InputContainer",m_inputContainer="LArPhysCaliTdiffIn");
  declareProperty("OutputContainer",m_outputContainer="LArPhysCaliTdiff");
  declareProperty("FileName",m_fileName); 
  return;
}

LArPhysCaliTDiffAlg::~LArPhysCaliTDiffAlg() {}


StatusCode LArPhysCaliTDiffAlg::initialize() {

  m_ntTitle="TimeDiffs";
  m_ntpath=std::string("/NTUPLES/FILE1/TIMEDIFF");
  return LArCond2NtupleBase::initialize();
}

StatusCode LArPhysCaliTDiffAlg::execute() {


  NTuple::Item<float> ntold, ntnew, ntoffset, ntcorr;
  NTuple::Item<long> ntgain;

  m_nt->addItem("gain",ntgain,0,3).ignore();
  m_nt->addItem("PCDiffOld",ntold,0,200).ignore();
  m_nt->addItem("PCDiffNew",ntnew,0,200).ignore();
  //m_nt->addItem("T0",ntT0,0,2000).ignore();
  m_nt->addItem("offset",ntoffset,0,200).ignore();
  m_nt->addItem("correction",ntcorr,0,200).ignore();


  const LArOnlineID* onlineID=0;
  if (detStore()->retrieve(onlineID).isFailure()) {
    msg(MSG::ERROR) << "Failed to retreive online helper" << endreq;
    return StatusCode::FAILURE;
  }


  std::vector<std::map<HWIdentifier,float> > offsetMap;
  offsetMap.resize(3);
  std::ifstream rein(m_fileName.c_str(),std::ifstream::in);
  if (!rein.good()) {
    msg(MSG::ERROR) << "Failed to open file " << m_fileName << endreq;
    return StatusCode::SUCCESS;
  }
  
  
  while (rein.good()) {
    unsigned gain,chidint;
    float offset;
    rein >> chidint >> gain >> offset;
    //std::cout << gain << " " << chidint << " " << offset << std::endl;
    const HWIdentifier fid(chidint);
    offsetMap[gain][fid]=offset;
  }

  rein.close();


  const LArPhysCaliTdiffComplete* oldCont=0;
  if (detStore()->retrieve(oldCont,m_inputContainer).isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve LArPhysCaliTdiffComplete with key " << m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }
  
  LArPhysCaliTdiffComplete* newCont=new LArPhysCaliTdiffComplete();
  newCont->setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
  newCont->initialize();
  if(detStore()->record(newCont,m_outputContainer).isFailure()) {
    msg(MSG::ERROR) << "Failed to register container with key " << m_outputContainer << " to StoreGate" << endreq;
  }

  for (int gain=0;gain<3;++gain) {
    msg(MSG::INFO) << "Working on gain " << gain << endreq;
    LArPhysCaliTdiffComplete::ConstConditionsMapIterator it=oldCont->begin(gain);
    LArPhysCaliTdiffComplete::ConstConditionsMapIterator it_e=oldCont->end(gain);
    for (;it!=it_e;++it) {
      const HWIdentifier chid=it.channelId();
      const HWIdentifier fid=onlineID->feb_Id(chid);
      const LArPhysCaliTdiffP& td=*it;
      if (td.isEmpty()) {
	ATH_MSG_DEBUG("Channel " << onlineID->channel_name(chid) << " has no data");
	continue;
      }
      float timediff=td.m_Tdiff;
      ntold=timediff;
      ntoffset=offsetMap[gain][fid];
      int refGain=0;
      if (onlineID->isHECchannel(chid)) 
	refGain=1; 

      if (gain>refGain) {
	ntcorr=offsetMap[gain][fid]-offsetMap[refGain][fid];
	const float correction=offsetMap[gain][fid]-offsetMap[refGain][fid];
	timediff+=correction;
      }
      else
	ntcorr=-999;
      newCont->set(chid,gain,timediff);
      ntnew=timediff;
      fillFromIdentifier(chid);
      ntgain=gain;
      ATH_MSG_DEBUG("Gain " << gain << " Ch:" << onlineID->channel_name(chid) << ":" << td.m_Tdiff << " -> " << timediff);


      StatusCode sc=ntupleSvc()->writeRecord(m_nt);
      if (sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) << "writeRecord failed" << endreq;
	return StatusCode::FAILURE;
      }

    }//end loop over all channels
  }//end loop over gains


  return StatusCode::SUCCESS;
}
