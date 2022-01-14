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
            int m_id = 0;
            float m_eff = 0.0F, m_errEff = 0.0F, m_res1 = 0.0F, m_res2 = 0.0F, m_resX = 0.0F, m_errRes1 = 0.0F, m_errRes2 = 0.0F, m_errResX = 0.0F, m_time = 0.0F, m_errTime = 0.0F, m_noise = 0.0F, m_errNoise = 0.0F, m_noiseC = 0.0F, m_errNoiseC = 0.0F, m_cs = 0.0F, m_errCs = 0.0F;

    };


    //or use general set function
    template <class T>
        void RpcCalibData::setParameter(RpcCondParType parType,T value) //generic parameter type function.
        {

            switch(parType)
            {
                case RpcCondParType::EFF:
                    m_eff = value;
                    break;
                case RpcCondParType::ERREFF:
                    m_errEff = value;
                    break;
                case RpcCondParType::RES1:
                    m_res1 = value;
                    break;
                case RpcCondParType::ERRRES1:
                    m_errRes1 = value;
                    break;
                case RpcCondParType::RES2:
                    m_res2 = value;
                    break;
                case RpcCondParType::ERRRES2:
                    m_errRes2 = value;
                    break;
                case RpcCondParType::RESX:
                    m_resX = value;
                    break;
                case RpcCondParType::ERRRESX:
                    m_errResX = value;
                    break;
                case RpcCondParType::TIME:
                    m_time = value;
                    break;
                case RpcCondParType::ERRTIME:
                    m_errTime = value;
                    break;
                case RpcCondParType::NOISE:
                    m_noise = value;
                    break;
                case RpcCondParType::NOISEC:
                    m_noiseC = value;
                    break;
                case RpcCondParType::ERRNOISE:
                    m_errNoise = value;
                    break;
                case RpcCondParType::ERRNOISEC:
                    m_errNoiseC = value;
                    break;
                case RpcCondParType::CS:
                    m_cs = value;
                    break;
                case RpcCondParType::ERRCS:
                    m_errCs = value;
                    break;
                default:
                    std::cout  << "RpcCalibData ERROR:  Invalid parameter type specified for setParameter."
                        << std::endl;
            }
        }

}

#endif
