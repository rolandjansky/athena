/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOTAU_T2CALOTAUERRORMON_H 
#define TRIGT2CALOTAU_T2CALOTAUERRORMON_H
#include "TrigT2CaloTau/T2CaloTauErrorHandler.h"

namespace TAUCLUSTMON {

    /** enumerate tau-specific errors for monitoring */
    enum TAUCLUSTMON{
      LARPROB=0,
      TILEPROB=1,
      ROBPROB=2,
      RODPROB=3,
      FAILSEED=4,
      FAILPRESEED=5,
      EMS0E0=6,
      EMS1E0=7,
      EMS2E0=8,
      EMS3E0=9,
      HADS1E0=10,
      HADS2E0=11,
      HADS3E0=12,
      OTHERERRORS=13,
      GOODCLUST=14,
      ALLCLUST=15
    };

    bool GetClusterError(unsigned int bit,uint32_t error ) { return ((error >> bit)&0x1)!=0 ;}

    void FillErrorMonitoring(uint32_t error, std::vector<unsigned char> * quality){
      bool isError=false;
      if ( 0x000000FF & error ) {isError=true; quality->push_back(LARPROB); }
      if ( 0x0FFF0000 & error ) {isError=true; quality->push_back(TILEPROB); }
      if ( 0x10000000 & error ) {isError=true; quality->push_back(ROBPROB); }
      if ( 0x20000000 & error ) {isError=true; quality->push_back(RODPROB); }
      if ( GetClusterError( TAUCLUSTERROR::FAILSEED    , error) ) {isError=true; quality->push_back(FAILSEED); }
      if ( GetClusterError( TAUCLUSTERROR::FAILPRESEED , error) ) {isError=true; quality->push_back(FAILPRESEED); }
      if ( GetClusterError( TAUCLUSTERROR::EMS0E0      , error) ) {isError=true; quality->push_back(EMS0E0); }
      if ( GetClusterError( TAUCLUSTERROR::EMS1E0      , error) ) {isError=true; quality->push_back(EMS1E0); }
      if ( GetClusterError( TAUCLUSTERROR::EMS2E0      , error) ) {isError=true; quality->push_back(EMS2E0); }
      if ( GetClusterError( TAUCLUSTERROR::EMS3E0      , error) ) {isError=true; quality->push_back(EMS3E0); }
      if ( GetClusterError( TAUCLUSTERROR::HADS1E0     , error) ) {isError=true; quality->push_back(HADS1E0); }
      if ( GetClusterError( TAUCLUSTERROR::HADS2E0     , error) ) {isError=true; quality->push_back(HADS2E0); }
      if ( GetClusterError( TAUCLUSTERROR::HADS3E0     , error) ) {isError=true; quality->push_back(HADS3E0); }
      if (isError==false && error>0 ) {quality->push_back(OTHERERRORS); }
    }

}
#endif

