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
  declareProperty("PerFebMGCorr",m_perFebMG=false); 
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

  CHECK(m_nt->addItem("gain",ntgain,0,3));
  CHECK(m_nt->addItem("PCDiffOld",ntold,-200,200));
  CHECK(m_nt->addItem("PCDiffNew",ntnew,-200,200));
  if(!m_perFebMG) CHECK(m_nt->addItem("offset",ntoffset,0,200));
  CHECK(m_nt->addItem("correction",ntcorr,0,200));


  const LArOnlineID* onlineID=0;
  CHECK(detStore()->retrieve(onlineID));

  std::vector<std::map<HWIdentifier,float> > offsetMap;
  if(m_perFebMG) offsetMap.resize(2); else offsetMap.resize(3);
  std::ifstream rein(m_fileName.c_str(),std::ifstream::in);
  if (!rein.good()) {
    ATH_MSG_ERROR( "Failed to open file " << m_fileName );
    return StatusCode::SUCCESS;
  }
  
  
  while (rein.good()) {
    unsigned gain,chidint;
    float offset;
    if(m_perFebMG) {
       rein >> std::hex>>chidint >>std::dec>> offset;
       const HWIdentifier fid(chidint);
       offsetMap[1][fid]=offset;
       //std::cout <<std::hex<< fid <<std::dec<< " " << offset << std::endl;
    } else {   
       rein >> chidint >> gain >> offset;
       const HWIdentifier fid(chidint);
       offsetMap[gain][fid]=offset;
    }
  }

  rein.close();

  const LArPhysCaliTdiffComplete* oldCont=0;
  CHECK(detStore()->retrieve(oldCont,m_inputContainer));
  
  LArPhysCaliTdiffComplete* newCont=new LArPhysCaliTdiffComplete();
  newCont->setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
  CHECK(newCont->initialize());
  StatusCode sc=detStore()->record(newCont,m_outputContainer);
  if(sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Failed to register container with key " << m_outputContainer << " to StoreGate" );
  }

  for (int gain=0;gain<3;++gain) {
    ATH_MSG_INFO( "Working on gain " << gain );
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
      if(m_perFebMG && gain != 1) {// do nothing if we are only correcting MG per FEB
         newCont->set(chid,gain,timediff);
         continue;
      }
      if(!m_perFebMG) {
        ntoffset=offsetMap[gain][fid]; 
        int refGain=0;
        if (onlineID->isHECchannel(chid)) refGain=1; 

        if (gain>refGain) {
	   ntcorr=offsetMap[gain][fid]-offsetMap[refGain][fid];
	   const float correction=offsetMap[gain][fid]-offsetMap[refGain][fid];
	   timediff+=correction;
        } else ntcorr=-999;
      } else {
         ntcorr=offsetMap[gain][fid];
         const float correction=offsetMap[gain][fid];
         timediff+=correction;
      }

      newCont->set(chid,gain,timediff);
      ntnew=timediff;
      fillFromIdentifier(chid);
      ntgain=gain;
      ATH_MSG_DEBUG("Gain " << gain << " Ch:" << onlineID->channel_name(chid) << ":" << td.m_Tdiff << " -> " << timediff);


      sc=ntupleSvc()->writeRecord(m_nt);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "writeRecord failed" );
	return StatusCode::FAILURE;
      }

    }//end loop over all channels
    std::cout<<"2"<<std::endl;
  }//end loop over gains


  return StatusCode::SUCCESS;
}
