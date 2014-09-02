/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscTimeStatus.h
//   Header file for class CscTimeStatus
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement CscTimeStatus for Muon - CSC
// Different from CscClusterStatus dedicated to position measurement flag
///////////////////////////////////////////////////////////////////
// Woochun Park
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_CSCTIMESTATUS_H
#define MUONPREPRAWDATA_CSCTIMESTATUS_H

#include <string>

namespace  Muon {
    /** Enum to represent the cluster time measurement status - see the specific enum values for more details*/ 
  enum CscTimeStatus {

    /** Time measurement is successful to use*/
    CscTimeSuccess =0,

    /** Not successful time measurement but samples indicates early time below -50ns in RAW time. **/
    CscTimeEarly =1,

    /** Not successful time measurement but samples indicates late time above 200ns in RAW time. **/
    CscTimeLate =2,

    /** Time information is not available due to dead/noise/stuck bit channels **/
    CscTimeUnavailable =3,

    /** Time is not assessed indicating potential bug */
    CscTimeStatusUndefined =255
  };

  /** Return a string description of a CSC cluster status flag. */
  inline std::string toString(CscTimeStatus cstat) {
    switch ( cstat ) {
    case CscTimeSuccess:       return "timeSuccess";
    case CscTimeEarly:         return "timeEarly";
    case CscTimeLate:          return "timeLate";
    case CscTimeUnavailable:   return "timeUnavailable";
    case CscTimeStatusUndefined:     return "timeStatusUndefined";
    }
    return "unknown";
  }

}

#endif
