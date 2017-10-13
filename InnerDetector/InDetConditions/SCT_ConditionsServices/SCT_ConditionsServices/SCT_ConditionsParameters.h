/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameters.h
 * header file containing enumerated type of parameters which may be retrieved
 * by the ISCT_ConditionsParameterSvc, and their string interpretation
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_ConditionsParameters_h
#define SCT_ConditionsParameters_h

#include <string>

namespace SCT_ConditionsServices{
  enum NumberOfElements{SIDES_PER_MODULE=2, CHIPS_PER_SIDE=6, CHIPS_PER_MODULE=12, NUMBER_OF_MODULES=4088};
  enum NumberOfCalibrationData{N_NPTGAIN=6, N_NOISEOCC=4};
  enum MajorityParameters{HighAndLowVoltageOK=17, /* 17 = 0x11 -> majority state for both LV and HV. */
                          OVERALL=110,
                          BARREL=111,
                          ECA=114,
                          ECC=115,
                          N_REGIONS=4,
                          INDEX_HVfraction=1,
                          INDEX_MajorityState=3};
  enum Bec{bec_ECC=-2, bec_BARREL=0, bec_ECA=+2};
  enum ParameterIndex{AVG_THRESHOLD, N_PARAMETERS, INVALID_PARAMETER};
  static const std::string parameterNames[]{"average threshold per module side","number of parameters"};
}

#endif // SCT_ConditionsParameters_h
