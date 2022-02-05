/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCalibDBEntry.h"
#include "MuonCondData/RpcCondParType.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace MuonCalib{

  RpcCalibDBEntry::~RpcCalibDBEntry(){
    // free nmemory used for data
    for(unsigned int k=0;k<m_theEtaData.size();k++) delete m_theEtaData[k];
    for(unsigned int k=0;k<m_thePhiData.size();k++) delete m_thePhiData[k];
  }

  RpcCalibDBEntry::RpcCalibDBEntry(Identifier gapID, std::string_view payLoad):m_nRecEta(0),m_nDetEta(0), m_nRecPhi1(0),m_nRecPhi2(0),m_nDetPhi1(0),m_nDetPhi2(0),m_theGap(gapID) {

    
    std::string::size_type end=payLoad.find("END ");
    std::string_view etaRec=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    std::string_view etaDet=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    std::string_view phiRec1=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    std::string_view phiRec2=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    std::string_view phiDet1=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    std::string_view phiDet2=payLoad.substr(0,end);

    this->initData(std::string(etaRec), std::string(etaDet),std::string(phiRec1),std::string(phiRec2),std::string(phiDet1),std::string(phiDet2));
    
  }

  void  RpcCalibDBEntry::initData(std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2){


    unsigned long int pos = 0;
    std::string::size_type start = etaRec.find_first_not_of(' ',pos);
    if(start == std::string::npos) {
      std::cout << "RpcCalibDBEntry::initData -- problems extracting m_nRecEta -- crashing." << std::endl;
      std::abort();
    }
    std::string::size_type stop = etaRec.find_first_of(' ',start+1);
    if (stop == std::string::npos) stop = etaRec.size();
    m_nRecEta = std::stoi(etaRec.substr(start,stop-start),nullptr);
    etaRec.erase(pos,stop-pos);

    pos = 0;
    start = phiRec1.find_first_not_of(' ',pos);
    if(start == std::string::npos) {
      std::cout << "RpcCalibDBEntry::initData -- problems extracting m_nRecPhi1 -- crashing." << std::endl;
      std::abort();      
    }
    stop = phiRec1.find_first_of(' ',start+1);
    if (stop == std::string::npos) stop = phiRec1.size();
    m_nRecPhi1 = std::stoi(phiRec1.substr(start,stop-start),nullptr);
    phiRec1.erase(pos,stop-pos);

    std::istringstream etaRec_str; 
    std::istringstream etaDet_str; 
    std::istringstream phiRec1_str;
    std::istringstream phiRec2_str;
    std::istringstream phiDet1_str;
    std::istringstream phiDet2_str;
    
    etaRec_str.str(etaRec);     
    etaDet_str.str(etaDet); 
    phiRec1_str.str(phiRec1);
    phiRec2_str.str(phiRec2);
    phiDet1_str.str(phiDet1);
    phiDet2_str.str(phiDet2);
    
    etaDet_str>>m_nDetEta;
    phiRec2_str>>m_nRecPhi2;
    phiDet1_str>>m_nDetPhi1;
    phiDet2_str>>m_nDetPhi2;

    float eff, errEff, res1, res2, resX, errRes1, errRes2, errResX, time, errTime, noise, errNoise, noiseC, errNoiseC, cs, errCs;
    
    // start with eta processing, 41 strips
    
    for(int k=0;k<m_nRecEta;k++){
      
      etaRec_str>>eff>>errEff>>res1>>errRes1>>res2>>errRes2>>resX>>errResX>>time>>errTime;
      etaDet_str>>noise>>errNoise>>noiseC>>errNoiseC>>cs>>errCs;
      
      RpcCalibData * etaData=new RpcCalibData;
      
      etaData->setId(k);
      etaData->setEff(eff);
      etaData->setErrEff(errEff);
      etaData->setRes1(res1);
      etaData->setRes2(res2);
      etaData->setResX(resX);
      etaData->setErrRes1(errRes1);
      etaData->setErrRes2(errRes2);
      etaData->setErrResX(errResX);
      etaData->setTime(time);
      etaData->setErrTime(errTime);
      etaData->setNoise(noise);
      etaData->setNoiseC(noiseC);
      etaData->setErrNoise(errNoise);
      etaData->setErrNoiseC(errNoiseC);
      etaData->setCs(cs);
      etaData->setErrCs(errCs);
      
      m_theEtaData.push_back(etaData);
      
    }
    
    // now phi
    
    for(int k=0;k<m_nRecPhi1;k++){
      
      phiRec1_str>>eff>>res1>>res2>>resX>>time;
      phiRec2_str>>errEff>>errRes1>>errRes2>>errResX>>errTime;
      phiDet1_str>>noise>>errNoise>>noiseC>>errNoiseC>>cs>>errCs;

      RpcCalibData * phiData=new RpcCalibData;

      phiData->setId(k);
      phiData->setEff(eff);
      phiData->setErrEff(errEff);
      phiData->setRes1(res1);
      phiData->setRes2(res2);
      phiData->setResX(resX);
      phiData->setErrRes1(errRes1);
      phiData->setErrRes2(errRes2);
      phiData->setErrResX(errResX);
      phiData->setTime(time);
      phiData->setErrTime(errTime);
      phiData->setNoise(noise);
      phiData->setNoiseC(noiseC);
      phiData->setErrNoise(errNoise);
      phiData->setErrNoiseC(errNoiseC);
      phiData->setCs(cs);
      phiData->setErrCs(errCs);

      m_thePhiData.push_back(phiData);

    }


  }

  // initialize from db columns
  
  RpcCalibDBEntry::RpcCalibDBEntry(Identifier gapID, const std::string& etaRec, const std::string& etaDet, const std::string& phiRec1, const std::string& phiRec2, const std::string& phiDet1, const std::string& phiDet2 ):m_nRecEta(0),m_nDetEta(0), m_nRecPhi1(0),m_nRecPhi2(0),m_nDetPhi1(0),m_nDetPhi2(0),m_theGap(gapID)
  {
    
    this->initData(etaRec, etaDet,phiRec1,phiRec2,phiDet1,phiDet2);


  }


  const Identifier  RpcCalibDBEntry::getGapID() const {
    
    return m_theGap;
    
  }
  
  void RpcCalibDBEntry::getColumns(std::string &recEta, std::string &detEta,std::string &recPhi1, std::string &recPhi2, std::string &detPhi1,std::string &detPhi2) const{
    

    std::ostringstream recEta_str,detEta_str,recPhi1_str,recPhi2_str,detPhi1_str,detPhi2_str;

    float eff, errEff, res1, res2, resX, errRes1, errRes2, errResX, time, errTime, noise, errNoise, noiseC, errNoiseC, cs, errCs;
    
    recEta_str<<m_nRecEta<<' ';
    detEta_str<<m_nDetEta<<' ';

    recPhi1_str<<m_nRecPhi1<<' ';
    detPhi1_str<<m_nDetPhi1<<' ';

    recPhi2_str<<m_nRecPhi2<<' ';
    detPhi2_str<<m_nDetPhi2<<' ';



    for(int k=0;k<m_nRecEta;k++){
      
      const RpcCalibData * theData=m_theEtaData[k];
      
      eff=theData->getEff();
      errEff=theData->getErrEff();
      res1=theData->getRes1();
      res2=theData->getRes2();
      resX=theData->getResX();
      errRes1=theData->getErrRes1();
      errRes2=theData->getErrRes2();
      errResX=theData->getErrResX();
      time=theData->getTime();
      errTime=theData->getErrTime();
      noise=theData->getNoise();
      noiseC=theData->getNoiseC();
      errNoise=theData->getErrNoise();
      errNoiseC=theData->getErrNoiseC();
      cs=theData->getCs();
      errCs=theData->getErrCs();

      recEta_str<< eff<< ' '<<errEff<< ' '<<res1<< ' '<<errRes1<< ' '<<res2<< ' '<<errRes2<< ' '<<resX<< ' '<<errResX<< ' '<<time<< ' '<<errTime<< ' ';
      detEta_str<<noise<< ' '<<errNoise<< ' '<<noiseC<< ' '<<errNoiseC<< ' '<<cs<< ' '<<errCs<< ' ';


    }

    for(int k=0;k<m_nRecPhi1;k++){
      
      const RpcCalibData * theData=m_thePhiData[k];
      
      eff=theData->getEff();
      errEff=theData->getErrEff();
      res1=theData->getRes1();
      res2=theData->getRes2();
      resX=theData->getResX();
      errRes1=theData->getErrRes1();
      errRes2=theData->getErrRes2();
      errResX=theData->getErrResX();
      time=theData->getTime();
      errTime=theData->getErrTime();
      noise=theData->getNoise();
      noiseC=theData->getNoiseC();
      errNoise=theData->getErrNoise();
      errNoiseC=theData->getErrNoiseC();
      cs=theData->getCs();
      errCs=theData->getErrCs();

      recPhi1_str<<eff<<' ' <<res1<<' ' <<res2<<' ' <<resX<<' ' <<time<< ' ';
      recPhi2_str<<errEff<<' ' <<errRes1<<' ' <<errRes2<<' ' <<errResX<<' ' <<errTime<< ' ';
      detPhi1_str<<noise<<' ' <<errNoise<<' ' <<noiseC<<' ' <<errNoiseC<<' ' <<cs<<' ' <<errCs<< ' ';


    }

    // give back strings

    recEta=recEta_str.str();
    detEta=detEta_str.str();
    recPhi1=recPhi1_str.str();
    recPhi2=recPhi2_str.str();
    detPhi1=detPhi1_str.str();
    detPhi2=detPhi2_str.str();

  }

}// End namespace MuonCalib
