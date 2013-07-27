/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCondParType.h"

#include <stdlib.h>
#include <iostream>


using namespace std;

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
  
  void RpcCalibData::setId(int stripId){id=stripId;}		  
  void RpcCalibData::setEff(float theEff){eff=theEff;}		  
  void RpcCalibData::setErrEff(float theErrEff){errEff=theErrEff;}	  
  void RpcCalibData::setRes1(float theRes1){res1=theRes1;}	  
  void RpcCalibData::setRes2(float theRes2){res2=theRes2;}	  
  void RpcCalibData::setResX(float theResX){resX=theResX;}	  
  void RpcCalibData::setErrRes1(float theErrRes1){errRes1=theErrRes1;}	  
  void RpcCalibData::setErrRes2(float theErrRes2){errRes2=theErrRes2;}	  
  void RpcCalibData::setErrResX(float theErrResX){errResX=theErrResX;}  
  void RpcCalibData::setTime(float theTime){time=theTime;}	  
  void RpcCalibData::setErrTime(float theErrTime){errTime=theErrTime;}	  
  void RpcCalibData::setNoise(float theNoise){noise=theNoise;}	  
  void RpcCalibData::setNoiseC(float theNoiseC){noiseC=theNoiseC;}	  
  void RpcCalibData::setErrNoise(float theErrNoise){errNoise=theErrNoise;}  
  void RpcCalibData::setErrNoiseC(float theErrNoiseC){errNoiseC=theErrNoiseC;}
  void RpcCalibData::setCs(float theCs){cs=theCs;}
  void RpcCalibData::setErrCs(float theErrCs){errCs=theErrCs;}
  

  //Get a parameter
  //They have defaults, but the other database can be requested
  
  int   RpcCalibData::getId() const {return(id); }
  float RpcCalibData::getEff() const {return eff;}
  float RpcCalibData::getErrEff() const {return errEff;}
  float RpcCalibData::getRes1() const {return res1;}
  float RpcCalibData::getRes2() const {return res2;}
  float RpcCalibData::getResX() const {return resX;}
  float RpcCalibData::getErrRes1() const {return errRes1;}
  float RpcCalibData::getErrRes2() const {return errRes2;}
  float RpcCalibData::getErrResX() const {return errResX;}
  float RpcCalibData::getTime() const {return time;}
  float RpcCalibData::getErrTime() const {return errTime;}
  float RpcCalibData::getNoise() const {return noise;}
  float RpcCalibData::getNoiseC() const {return noiseC;}
  float RpcCalibData::getErrNoise() const {return errNoise;}
  float RpcCalibData::getErrNoiseC() const {return errNoiseC;}
  float RpcCalibData::getCs() const {return cs;}
  float RpcCalibData::getErrCs() const {return errCs;}
  
  float RpcCalibData::getParameter(RpcCondParType parType) const
  {
    if (parType == RpcCondParType::EFF)
            return(eff);
        else if(parType == RpcCondParType::ERREFF)
            return(errEff);
        else if(parType == RpcCondParType::RES1)
            return(res1);
        else if(parType == RpcCondParType::RES2)
            return(res2);
        else if (parType == RpcCondParType::RESX)
            return(resX);
        else if(parType == RpcCondParType::ERRRES1)
            return(errRes1);
        else if(parType == RpcCondParType::ERRRES2)
            return(errRes2);
        else if (parType == RpcCondParType::ERRRESX)
            return(errResX);
        else if(parType == RpcCondParType::TIME)
            return(time);
        else if(parType == RpcCondParType::ERRTIME)
            return(errTime);
        else if (parType == RpcCondParType::NOISE)
            return(noise);
        else if (parType == RpcCondParType::NOISEC)
            return(noiseC);
        else if (parType == RpcCondParType::ERRNOISE)
            return(errNoise);
        else if (parType == RpcCondParType::ERRNOISEC)
            return(errNoiseC);
        else if (parType == RpcCondParType::CS)
            return(cs);
        else if (parType == RpcCondParType::ERRCS)
            return(errCs);

        return(-9999); //Didn't provide a valid parameter type
    }
}// End namespace MuonCalib
