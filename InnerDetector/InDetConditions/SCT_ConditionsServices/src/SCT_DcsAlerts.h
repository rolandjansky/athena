/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConditionsSvc.h
 * header file containing DCS limits for the SCT_DCSConditionsSvc
 * @author shaun.roe@cern.ch, based on values from A. R-Veronneau
**/

#include <string>
#include <utility>
#include <map>
#include <algorithm>

namespace SCT_ConditionsServices{
  ///Struct to store the upper and lower thresholds for alarms
  struct LimitValues{
    float loAlarm, hiAlarm;
    LimitValues(float a, float b):loAlarm(a), hiAlarm(b) { }
  };
  ///A DCS value may be any of these
  enum DcsStatus{DCS_OK, DCS_ALARM, DCS_UNKNOWN};
  
  typedef std::pair<const std::string, LimitValues> AlertsPair;
  ///Hardwired values for warnings and alarms
  const AlertsPair alertsPairs[]={
    AlertsPair("LVCH_VCC_RECV",LimitValues(0.0, 3.7) ),
    //AlertsPair("LVPS_VCC_RECV",LimitValues(0.0, 3.7) ),
    //AlertsPair("LVRETVCC_RECV",LimitValues(0.0, 5.7) ),
    //AlertsPair("LVCH_VDD_RECV",LimitValues(0.0, 4.2) ),
    //AlertsPair("LVPS_VDD_RECV",LimitValues(0.0, 4.2) ),
    //AlertsPair("LVRETVDD_RECV",LimitValues(0.0, 5.7) ),
    //AlertsPair("LVCHVCSV_RECV",LimitValues(0.0, 4.2) ),
    //AlertsPair("LVCHVCSI_RECV",LimitValues(0.0, 4.0) ),
    //AlertsPair("LVCHPINV_RECV",LimitValues(0.0, 6.2) ),
    //AlertsPair("LVCHPINI_RECV",LimitValues(0.0, 0.6) ),
    //AlertsPair("MOCH_TM0_RECV",LimitValues(10, 33, 35, -273.0) ), //odd value??
    //AlertsPair("MOCH_TM1_RECV",LimitValues(10, 33, 35, -273.0) ), //odd value??
    AlertsPair("HVCHVOLT_RECV",LimitValues(140.0, 157.5) ),
    AlertsPair("HVCHCURR_RECV",LimitValues(0.0, 30000000.0) ), //value in nA
    AlertsPair("LVCH_ICC_RECV",LimitValues(0.0, 1250.0) ),
    //AlertsPair("LVCH_IDD_RECV",LimitValues(0.0, 1000.0) )
  };
  
  ///utility to return the 'end' of a classic array
  template<class T, int Size> T  * endof( T (&parray)[Size]){
   return parray + Size;
  }
  ///A map of the parameter name to the threshold values
  const std::map<std::string, LimitValues> alertsMap(alertsPairs, endof(alertsPairs) );
  
  ///Indicate the status of a given value for a given DCS parameter
  DcsStatus
  DcsValueStatus(const std::string & parameterName, const float parameterValue){
    std::map<std::string, LimitValues>::const_iterator thisPair=alertsMap.find(parameterName);
    if (thisPair == alertsMap.end()) return DCS_UNKNOWN;
    LimitValues theseLimits=thisPair->second;
    //hopefully, OK is most likely value, so check for that first
    if ((parameterValue>theseLimits.loAlarm) and (parameterValue<theseLimits.hiAlarm)) return DCS_OK;
    return DCS_ALARM;
  }
  
  
}//end of namespace scope

