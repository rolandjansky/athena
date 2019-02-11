/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArOFCBinAlg.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "CaloIdentifier/CaloIdManager.h"

//#include <ifstream>
#include <fstream>

LArOFCBinAlg::LArOFCBinAlg(const std::string & name, ISvcLocator * pSvcLocator) :
  LArCond2NtupleBase(name,pSvcLocator) {
  
  declareProperty("InputContainer",m_inputContainer="LArOFCBinIn");
  declareProperty("OutputContainer",m_outputContainer="LArOFCBin");
  declareProperty("FileName",m_fileName); 
  declareProperty("PerFebMGCorr",m_perFebMG=true); 
  declareProperty("PerFebCorr",m_perFeb=false); 
  return;
}

LArOFCBinAlg::~LArOFCBinAlg() {}


StatusCode LArOFCBinAlg::initialize() {

  //if(!m_perFebMG) {
  // msg(MSG::ERROR) << "Not implemented yet....." <<endmsg;
  // return StatusCode::FAILURE;
  //}
  m_ntTitle="Bin";
  m_ntpath=std::string("/NTUPLES/FILE1/OFCBINDIFF");
  ATH_CHECK( m_cablingKey.initialize() );
  return LArCond2NtupleBase::initialize();
}

StatusCode LArOFCBinAlg::execute() {


  NTuple::Item<long> ntold, ntnew, ntcorr;
  NTuple::Item<long> ntgain;

  CHECK(m_nt->addItem("gain",ntgain,0,3));
  CHECK(m_nt->addItem("OFCBinOld",ntold,-200,200));
  CHECK(m_nt->addItem("OFCBinNew",ntnew,-200,200));
  CHECK(m_nt->addItem("correction",ntcorr,0,200));


  const LArOnlineID* onlineID=0;
  CHECK(detStore()->retrieve(onlineID));

  std::vector<std::map<HWIdentifier,float> > offsetMap;
  if(m_perFebMG) offsetMap.resize(2); else offsetMap.resize(3);
  std::ifstream rein;
  if(m_perFebMG || m_perFeb) {
     rein.open(m_fileName.c_str(),std::ifstream::in);
     if (!rein.good()) {
       msg(MSG::ERROR) << "Failed to open file " << m_fileName << endmsg;
       return StatusCode::FAILURE;
     }
  }
  
  
  if(m_perFebMG) {
   while (rein.good()) {
    unsigned chidint;
    float offset;
       rein >> std::hex>>chidint >>std::dec>> offset;
       const HWIdentifier fid(chidint);
       offsetMap[1][fid]=offset;
       //std::cout <<std::hex<< fid <<std::dec<< " " << offset << std::endl;
    }
  } else if (m_perFeb) {
   while (rein.good()) {
    unsigned chidint;
    float offset[3];
       rein >> std::hex>>chidint >>std::dec>> offset[0] >>offset[1] >>offset[2];
       if(rein.good()){
          const HWIdentifier fid(chidint);
          for(unsigned i=0; i<3; ++i) offsetMap[i][fid]=offset[i];
       }
       //std::cout <<std::hex<< fid <<std::dec<< " " << offset << std::endl;
    }

  }

  rein.close();

  const LArOFCBinComplete* oldCont=0;
  CHECK(detStore()->retrieve(oldCont,m_inputContainer));
  
  LArOFCBinComplete* newCont=new LArOFCBinComplete();
  newCont->setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
  CHECK(newCont->initialize());
  StatusCode sc=detStore()->record(newCont,m_outputContainer);
  if(sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Failed to register container with key " << m_outputContainer << " to StoreGate" << endmsg;
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  const LArEM_Base_ID* emId;
  const LArHEC_Base_ID* hecId;
  const LArFCAL_Base_ID* fcalId;
  const CaloIdManager *caloIdMgr=CaloIdManager::instance();
  emId=caloIdMgr->getEM_ID();
  fcalId=caloIdMgr->getFCAL_ID();
  hecId=caloIdMgr->getHEC_ID();

  for (int gain=0;gain<3;++gain) {
    msg(MSG::INFO) << "Working on gain " << gain << endmsg;
    LArOFCBinComplete::ConstConditionsMapIterator it=oldCont->begin(gain);
    LArOFCBinComplete::ConstConditionsMapIterator it_e=oldCont->end(gain);
    for (;it!=it_e;++it) {
      const HWIdentifier chid=it.channelId();
      const HWIdentifier fid=onlineID->feb_Id(chid);
      const LArOFCBinP& td=*it;
      if (td.isEmpty()) {
	ATH_MSG_DEBUG("Channel " << onlineID->channel_name(chid) << " has no data");
	continue;
      }
      int bindiff=td.m_bin;
      ntold=bindiff;
      if(m_perFebMG && gain != 1) {// do nothing if we are only correcting MG per FEB
         newCont->set(chid,gain,bindiff);
         continue;
      }
      if(!(m_perFebMG || m_perFeb)) {
         // shift by 24 bins (for L1Calo purposes)
         ntcorr=24;
         bindiff-=ntcorr;
      } else {
         // Do not make corrections for back layer 
         int barrel_ec = onlineID->barrel_ec(chid);
         int layer=-1;
         if (cabling->isOnlineConnected(chid)) {
            Identifier id=cabling->cnvToIdentifier(chid);
            if (emId->is_lar_em(id)) {
               layer     = emId->sampling(id);
            } else if (hecId->is_lar_hec(id)) {
               layer     = m_hecId->sampling(id);
            } else if (fcalId->is_lar_fcal(id)) {
               layer     = m_fcalId->module(id);
            }
         }
         if( m_perFebMG && (layer<0 || (barrel_ec ==0 && layer==3 )) ) { 
            std::cout<<"Skipping channel: "<<chid.get_identifier32()<<std::endl;
            newCont->set(chid,gain,bindiff);
            continue;
         }  

         const float correction=offsetMap[gain][fid];
         int mfac;
         if(correction>0) mfac=1; else mfac=-1;
         const int imult = correction / 1.042; // 25./24. = 1.042
         if(fabs(correction - imult*1.042) < 0.5) ntcorr = imult; else ntcorr=(imult + mfac);
         //std::cout<<bindiff<<" : "<<correction<< " "<<imult<<" : "<<ntcorr<<std::endl;
         std::cout<<std::hex<<fid.get_identifier32()<<" "<<std::dec<<ntcorr<<std::endl;
         bindiff-=ntcorr;
      }

      newCont->set(chid,gain,bindiff);
      ntnew=bindiff;
      fillFromIdentifier(chid);
      ntgain=gain;
      ATH_MSG_DEBUG("Gain " << gain << " Ch:" << onlineID->channel_name(chid) << ":" << td.m_bin << " -> " << bindiff);


      sc=ntupleSvc()->writeRecord(m_nt);
      if (sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) << "writeRecord failed" << endmsg;
	return StatusCode::FAILURE;
      }

    }//end loop over all channels
  }//end loop over gains


  return StatusCode::SUCCESS;
}
