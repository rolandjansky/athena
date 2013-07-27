/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCALIBDATA_H
#define RPCCALIBDATA_H

#include <string>
#include <iostream>
#include "MuonCondData/RpcCondParType.h"

namespace MuonCalib{

    //This is a satus flag that would never naturally exist, and thus will be the default 
    //value. If you get this, the database value wasn't ever retrieved for the strip you 
    //are requesting.
    
    #define RPC_COND_STATUS_ERROR_CODE 0xFF
    /**
      @class RpcCalibData

      @author andrea.di.simone@cern.ch

      @brief holds all conditions data for rpc strip.

      Holds conditions constants for a single strip. based on similar CSC code
     */

    class RpcCalibData
    {
        public:
            RpcCalibData();
            ~RpcCalibData() {}

            //Set functions
            void setId(int stripId);		  
            void setEff(float theEff);		  
            void setErrEff(float theErrEff);	  
	    void setRes1(float theRes1);	  
	    void setRes2(float theRes2);	  
	    void setResX(float theResX);	  
	    void setErrRes1(float theErrRes1);	  
	    void setErrRes2(float theErrRes2);	  
	    void setErrResX(float theErrResX);	  
	    void setTime(float theTime);	  
	    void setErrTime(float theErrTime);	  
	    void setNoise(float theNoise);	  
	    void setNoiseC(float theNoiseC);	  
	    void setErrNoise(float theErrNoise);  
	    void setErrNoiseC(float theErrNoiseC);
	    void setCs(float theCs);
	    void setErrCs(float theErrCs);
	    
            template <class T>
                void setParameter(RpcCondParType parType, T value); //generic parameter type function.

            //Get a parameter

            int getId() const;

            float getEff() const;
            float getErrEff() const;
	    float getRes1() const;
	    float getRes2() const;
	    float getResX() const;
	    float getErrRes1() const;
	    float getErrRes2() const;
	    float getErrResX() const;
	    float getTime() const;
	    float getErrTime() const;
	    float getNoise() const;
	    float getNoiseC() const;
	    float getErrNoise() const;
	    float getErrNoiseC() const;
	    float getCs() const;
	    float getErrCs() const;

            //For access to parameter's via one function. Accepts enum parType
            float getParameter(RpcCondParType parType) const;

        private:
            int id;
            float eff, errEff, res1, res2, resX, errRes1, errRes2, errResX, time, errTime, noise, errNoise, noiseC, errNoiseC, cs, errCs;

    };


    //or use general set function
    template <class T>
        void RpcCalibData::setParameter(RpcCondParType parType,T value) //generic parameter type function.
        {

            switch(parType)
            {
                case RpcCondParType::EFF:
                    eff = value;
                    break;
                case RpcCondParType::ERREFF:
                    errEff = value;
                    break;
                case RpcCondParType::RES1:
                    res1 = value;
                    break;
                case RpcCondParType::ERRRES1:
                    errRes1 = value;
                    break;
                case RpcCondParType::RES2:
                    res2 = value;
                    break;
                case RpcCondParType::ERRRES2:
                    errRes2 = value;
                    break;
                case RpcCondParType::RESX:
                    resX = value;
                    break;
                case RpcCondParType::ERRRESX:
                    errResX = value;
                    break;
                case RpcCondParType::TIME:
                    time = value;
                    break;
                case RpcCondParType::ERRTIME:
                    errTime = value;
                    break;
                case RpcCondParType::NOISE:
                    noise = value;
                    break;
                case RpcCondParType::NOISEC:
                    noiseC = value;
                    break;
                case RpcCondParType::ERRNOISE:
                    errNoise = value;
                    break;
                case RpcCondParType::ERRNOISEC:
                    errNoiseC = value;
                    break;
                case RpcCondParType::CS:
                    cs = value;
                    break;
                case RpcCondParType::ERRCS:
                    errCs = value;
                    break;
                default:
                    std::cout  << "RpcCalibData ERROR:  Invalid parameter type specified for setParameter."
                        << std::endl;
            }
        }

}

#endif
