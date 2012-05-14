/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  Authors
  Nikola Makovec <makovec@cern.ch>
  Nils Ruthmann <nils.ruthmann@cern.ch>
  Justin Keung <jkeung@cern.ch>
   
  Purpose: define the jet identification criteria

  - Version 2.0
     implements the rel 16 cleaning at three working points.

  Usage:
  bool isbad_medium = JetID::isBad(JetID::MediumBad,emf,HECf,LArQ,HECq,time,SumPtTrk,eta,pt,fmax,negE);
  bool isbad_loose  = JetID::isBad(JetID:: LooseBad,emf,HECf,LArQ,HECq,time,SumPtTrk,eta,pt,fmax,negE);
**/


#include "SUSYTools/JetID.hpp"
#include <cmath>
namespace JetID
{
  bool isBadJet(BadJetCategory criteria,
		double emf,
		double hecf,
		double larq,
		double hecq,
		double time,     //in ns
		double sumpttrk, //in GeV, same as sumpttrk
		double eta,      //emscale Eta  
		double pt,       //in GeV, same as sumpttrk
		double fmax,
		double negE      //in MeV
		)
  {  
    double chf=sumpttrk/pt;

    //Non-collision background & cosmics
    if(fabs(time)>25)                                  return true;
    if(criteria==MediumBad || criteria==TightBad){
      if(fabs(time)>10)                                return true;
    }
    if(emf<0.05 && chf<0.05 && fabs(eta)<2)            return true;
    if(emf<0.05 && fabs(eta)>=2)                       return true;
    if(fmax>0.99 && fabs(eta)<2)                       return true;
    if(criteria==MediumBad || criteria==TightBad){
      if(emf<0.05 && chf<0.1  && fabs(eta)<2)          return true;  
      if(emf>0.95 && chf<0.05 && fabs(eta)<2)          return true;
    }
    if(criteria==TightBad){
      if(emf<0.1 && chf<0.2  && fabs(eta)<2)           return true; 
      if(emf<0.1 && fabs(eta)>=2 )                     return true;    
      if(emf>0.9 && chf<0.02 && fabs(eta)<2)           return true;
    }

    //HEC spikes
    if(fabs(negE/1000.)>60)                            return true;
    if(hecf>0.5 && fabs(hecq)>0.5)                     return true;  
    if(criteria==MediumBad || criteria==TightBad){
      if(hecf>1-fabs(hecq))                            return true;
    }

    //EM coherent noise
    if(emf>0.95 && fabs(larq)>0.8 && fabs(eta)<2.8)    return true;
    if( criteria==MediumBad || criteria==TightBad){
      if(emf>0.9 && fabs(larq)>0.8 && fabs(eta)<2.8)   return true;
    }
    if(criteria==TightBad){
      if(fabs(larq)>0.95)                              return true;
      if(emf>0.98 && fabs(larq)>0.05)                  return true;
    } 

    return false;
  }
}

