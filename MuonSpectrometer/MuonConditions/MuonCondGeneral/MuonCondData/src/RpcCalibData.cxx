/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCondParType.h"

#include <stdlib.h>
#include <iostream>

namespace MuonCalib{
  //constructor
  RpcCalibData::RpcCalibData()
  {
    setId(-9999);
    setEff(-9999);
    setErrEff(-9999);
    setRes1(-9999);
    setRes2(-9999);
    setResX(-9999);
    setErrRes1(-9999);
    setErrRes2(-9999);
    setErrResX(-9999);
    setTime(-9999);
    setErrTime(-9999);
    setNoise(-9999);
    setNoiseC(-9999);
    setErrNoise(-9999);
    setErrNoiseC(-9999);
    setCs(-9999);
    setErrCs(-9999);
  }
  
  
  //specific set functions
  
  void RpcCalibData::setId(int stripId){m_id=stripId;}		  
  void RpcCalibData::setEff(float theEff){m_eff=theEff;}		  
  void RpcCalibData::setErrEff(float theErrEff){m_errEff=theErrEff;}	  
  void RpcCalibData::setRes1(float theRes1){m_res1=theRes1;}	  
  void RpcCalibData::setRes2(float theRes2){m_res2=theRes2;}	  
  void RpcCalibData::setResX(float theResX){m_resX=theResX;}	  
  void RpcCalibData::setErrRes1(float theErrRes1){m_errRes1=theErrRes1;}	  
  void RpcCalibData::setErrRes2(float theErrRes2){m_errRes2=theErrRes2;}	  
  void RpcCalibData::setErrResX(float theErrResX){m_errResX=theErrResX;}  
  void RpcCalibData::setTime(float theTime){m_time=theTime;}	  
  void RpcCalibData::setErrTime(float theErrTime){m_errTime=theErrTime;}	  
  void RpcCalibData::setNoise(float theNoise){m_noise=theNoise;}	  
  void RpcCalibData::setNoiseC(float theNoiseC){m_noiseC=theNoiseC;}	  
  void RpcCalibData::setErrNoise(float theErrNoise){m_errNoise=theErrNoise;}  
  void RpcCalibData::setErrNoiseC(float theErrNoiseC){m_errNoiseC=theErrNoiseC;}
  void RpcCalibData::setCs(float theCs){m_cs=theCs;}
  void RpcCalibData::setErrCs(float theErrCs){m_errCs=theErrCs;}
  

  //Get a parameter
  //They have defaults, but the other database can be requested
  
  int   RpcCalibData::getId() const {return(m_id); }
  float RpcCalibData::getEff() const {return m_eff;}
  float RpcCalibData::getErrEff() const {return m_errEff;}
  float RpcCalibData::getRes1() const {return m_res1;}
  float RpcCalibData::getRes2() const {return m_res2;}
  float RpcCalibData::getResX() const {return m_resX;}
  float RpcCalibData::getErrRes1() const {return m_errRes1;}
  float RpcCalibData::getErrRes2() const {return m_errRes2;}
  float RpcCalibData::getErrResX() const {return m_errResX;}
  float RpcCalibData::getTime() const {return m_time;}
  float RpcCalibData::getErrTime() const {return m_errTime;}
  float RpcCalibData::getNoise() const {return m_noise;}
  float RpcCalibData::getNoiseC() const {return m_noiseC;}
  float RpcCalibData::getErrNoise() const {return m_errNoise;}
  float RpcCalibData::getErrNoiseC() const {return m_errNoiseC;}
  float RpcCalibData::getCs() const {return m_cs;}
  float RpcCalibData::getErrCs() const {return m_errCs;}
  
  float RpcCalibData::getParameter(RpcCondParType parType) const
  {
    if (parType == RpcCondParType::EFF)
            return(m_eff);
        else if(parType == RpcCondParType::ERREFF)
            return(m_errEff);
        else if(parType == RpcCondParType::RES1)
            return(m_res1);
        else if(parType == RpcCondParType::RES2)
            return(m_res2);
        else if (parType == RpcCondParType::RESX)
            return(m_resX);
        else if(parType == RpcCondParType::ERRRES1)
            return(m_errRes1);
        else if(parType == RpcCondParType::ERRRES2)
            return(m_errRes2);
        else if (parType == RpcCondParType::ERRRESX)
            return(m_errResX);
        else if(parType == RpcCondParType::TIME)
            return(m_time);
        else if(parType == RpcCondParType::ERRTIME)
            return(m_errTime);
        else if (parType == RpcCondParType::NOISE)
            return(m_noise);
        else if (parType == RpcCondParType::NOISEC)
            return(m_noiseC);
        else if (parType == RpcCondParType::ERRNOISE)
            return(m_errNoise);
        else if (parType == RpcCondParType::ERRNOISEC)
            return(m_errNoiseC);
        else if (parType == RpcCondParType::CS)
            return(m_cs);
        else if (parType == RpcCondParType::ERRCS)
            return(m_errCs);

        return(-9999); //Didn't provide a valid parameter type
    }
}// End namespace MuonCalib
