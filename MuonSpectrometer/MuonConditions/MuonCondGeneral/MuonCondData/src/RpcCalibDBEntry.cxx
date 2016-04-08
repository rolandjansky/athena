/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCalibDBEntry.h"
#include "MuonCondData/RpcCondParType.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>

//     class RpcCalibDBEntry
//     {
//         public:
//             RpcCalibDBEntry(Identifier gapID, std::string payload);
//             ~RpcCalibDBEntry() {}

// 	    const RpcCalibData * getData(int measPhi, int strip);

//         private:
// 	    std::vector<RpcCalibData*> thePhiData;
// 	    std::vector<RpcCalibData*> theEtaData;



using namespace std;

namespace MuonCalib{

  // initialize from  whole string read from file


  RpcCalibDBEntry::~RpcCalibDBEntry(){

    // free nmemory used for data

    for(unsigned int k=0;k<m_theEtaData.size();k++) delete m_theEtaData[k];

    for(unsigned int k=0;k<m_thePhiData.size();k++) delete m_thePhiData[k];


  }

// 	    int nRecEta, nDetEta,nRecPhi1,nRecPhi2,nDetPhi1,nDetPhi2;
// 	    Identifier theGap;
// 	    std::vector<RpcCalibData*> m_thePhiData;
// 	    std::vector<RpcCalibData*> m_theEtaData;


  RpcCalibDBEntry::RpcCalibDBEntry(Identifier gapID, std::string payLoad):nRecEta(0),nDetEta(0), nRecPhi1(0),nRecPhi2(0),nDetPhi1(0),nDetPhi2(0),theGap(gapID) {

    
    string::size_type end=payLoad.find("END ");
    string etaRec=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    string etaDet=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    string phiRec1=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    string phiRec2=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    string phiDet1=payLoad.substr(0,end);
    payLoad=payLoad.substr(end+4,payLoad.size()-end-4);

    end=payLoad.find("END ");
    string phiDet2=payLoad.substr(0,end);

    this->initData(etaRec, etaDet,phiRec1,phiRec2,phiDet1,phiDet2);
    
  }

  void  RpcCalibDBEntry::initData(std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2){


    unsigned long int pos = 0;
    std::string::size_type start = etaRec.find_first_not_of(" ",pos);
    if(start == string::npos) {
      std::cout << "RpcCalibDBEntry::initData -- problems extracting nRecEta -- crashing." << std::endl;
      throw;      
    }
    std::string::size_type stop = etaRec.find_first_of(" ",start+1);
    if (stop == std::string::npos) stop = etaRec.size();
    nRecEta = std::stoi(etaRec.substr(start,stop-start),nullptr);
    etaRec.erase(pos,stop-pos);

    pos = 0;
    start = phiRec1.find_first_not_of(" ",pos);
    if(start == string::npos) {
      std::cout << "RpcCalibDBEntry::initData -- problems extracting nRecPhi1 -- crashing." << std::endl;
      throw;      
    }
    stop = phiRec1.find_first_of(" ",start+1);
    if (stop == std::string::npos) stop = phiRec1.size();
    nRecPhi1 = std::stoi(phiRec1.substr(start,stop-start),nullptr);
    phiRec1.erase(pos,stop-pos);

    istringstream etaRec_str; 
    istringstream etaDet_str; 
    istringstream phiRec1_str;
    istringstream phiRec2_str;
    istringstream phiDet1_str;
    istringstream phiDet2_str;
    
    etaRec_str.str(etaRec);     
    etaDet_str.str(etaDet); 
    phiRec1_str.str(phiRec1);
    phiRec2_str.str(phiRec2);
    phiDet1_str.str(phiDet1);
    phiDet2_str.str(phiDet2);
    
    etaDet_str>>nDetEta;
    phiRec2_str>>nRecPhi2;
    phiDet1_str>>nDetPhi1;
    phiDet2_str>>nDetPhi2;

    float eff, errEff, res1, res2, resX, errRes1, errRes2, errResX, time, errTime, noise, errNoise, noiseC, errNoiseC, cs, errCs;
    
    // start with eta processing, 41 strips
    
    for(int k=0;k<nRecEta;k++){
      
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
    
    for(int k=0;k<nRecPhi1;k++){
      
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
  
  RpcCalibDBEntry::RpcCalibDBEntry(Identifier gapID, std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2 ):nRecEta(0),nDetEta(0), nRecPhi1(0),nRecPhi2(0),nDetPhi1(0),nDetPhi2(0),theGap(gapID)
  {
    
    this->initData(etaRec, etaDet,phiRec1,phiRec2,phiDet1,phiDet2);


  }


  const Identifier  RpcCalibDBEntry::getGapID() const {
    
    return theGap;
    
  }
  
  void RpcCalibDBEntry::getColumns(string &recEta, string &detEta,string &recPhi1, string &recPhi2, string &detPhi1,string &detPhi2) const{
    

    std::ostringstream recEta_str,detEta_str,recPhi1_str,recPhi2_str,detPhi1_str,detPhi2_str;

    float eff, errEff, res1, res2, resX, errRes1, errRes2, errResX, time, errTime, noise, errNoise, noiseC, errNoiseC, cs, errCs;
    
    recEta_str<<nRecEta<<" ";
    detEta_str<<nDetEta<<" ";

    recPhi1_str<<nRecPhi1<<" ";
    detPhi1_str<<nDetPhi1<<" ";

    recPhi2_str<<nRecPhi2<<" ";
    detPhi2_str<<nDetPhi2<<" ";



    for(int k=0;k<nRecEta;k++){
      
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

      //      std::cout<<" provo a metterci "<<eff<< " "<<errEff<< " "<<res1<< " "<<errRes1<< " "<<res2<< " "<<errRes2<< " "<<resX<< " "<<errResX<< " "<<time<< " "<<errTime<<std::endl;

      recEta_str<< eff<< " "<<errEff<< " "<<res1<< " "<<errRes1<< " "<<res2<< " "<<errRes2<< " "<<resX<< " "<<errResX<< " "<<time<< " "<<errTime<< " ";
      detEta_str<<noise<< " "<<errNoise<< " "<<noiseC<< " "<<errNoiseC<< " "<<cs<< " "<<errCs<< " ";
      
      //      std::cout<<" vediamo se ha funzionato,  ora receta e' "<<recEta.str()<<std::endl;

    }

    for(int k=0;k<nRecPhi1;k++){
      
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

      recPhi1_str<<eff<<" " <<res1<<" " <<res2<<" " <<resX<<" " <<time<< " ";
      recPhi2_str<<errEff<<" " <<errRes1<<" " <<errRes2<<" " <<errResX<<" " <<errTime<< " ";
      detPhi1_str<<noise<<" " <<errNoise<<" " <<noiseC<<" " <<errNoiseC<<" " <<cs<<" " <<errCs<< " ";


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
