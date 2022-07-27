/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2DJTRIG_BEAMSPOT_H
#define TRIGT2DJTRIG_BEAMSPOT_H

#include "BeamSpotConditionsData/BeamSpotData.h"

//keep all code for handling the beamspot info in the displaced jet trigger in one place
//Two tools under different hypo algs use this info so better to only write once
class DisplacedJetBeamspotInfo
{
public:
  DisplacedJetBeamspotInfo(const InDet::BeamSpotData* beamspot = nullptr): m_bs_data(beamspot) {}

  //m_errPar { sigmaX, sigmaY, sigmaZ, tiltX, tiltY, sigmaXY }, so sigma x = 0 sigma y = 1 sigma xy has a helper
  float sigmaX(){
    if(!is_available()){
      return 0.0;
    }

    return m_bs_data->beamSigma(0);
  }

  float sigmaY(){
    if(!is_available()){
      return 0.0;
    }

    return m_bs_data->beamSigma(1);
  }

  float sigmaXY(){
    if(!is_available()){
      return 0.0;
    }

    return m_bs_data->beamSigmaXY();
  }

private:
  const InDet::BeamSpotData* m_bs_data;

  bool is_available(){
    if(m_bs_data == nullptr){
      return false;
    }

    //check if the beamspot info is valid and should be used
    //info from https://twiki.cern.ch/twiki/bin/view/Atlas/CoolBeamSpotParameters
    int beamSpotBitMap = m_bs_data->beamStatus();
    bool isOnlineBeamspot = ((beamSpotBitMap & 0x4) == 0x4);

    //offline/mc -> assume to be valid
    if(!isOnlineBeamspot){
      return true;
    }

    //check if the beamspot has converged
    if((beamSpotBitMap&0x3) == 0x3){
      //beamspot has converged
      return true;
    }

    return false;
  }
};

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H