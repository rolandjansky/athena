// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameters.h
 * @brief header file containing the number of elements and enumerated type
 * of parameters which may be retrieved by the ISCT_ConditionsParameterTool.
 * @author shaun.roe@cern.ch
 **/

#ifndef SCT_CONDITIONSDATA_SCT_CONDITIONSPARAMETERS_H
#define SCT_CONDITIONSDATA_SCT_CONDITIONSPARAMETERS_H

namespace SCT_ConditionsData {
  enum NumberOfElements{SIDES_PER_MODULE=2, 
                        NUMBER_OF_MODULES=4088, NUMBER_OF_WAFERS=NUMBER_OF_MODULES*SIDES_PER_MODULE,
                        CHIPS_PER_SIDE=6, CHIPS_PER_MODULE=SIDES_PER_MODULE*CHIPS_PER_SIDE,
                        STRIPS_PER_WAFER=768, STRIPS_PER_CHIP=STRIPS_PER_WAFER/CHIPS_PER_SIDE};
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
}

#endif // SCT_CONDITIONSDATA_SCT_CONDITIONSPARAMETERS_H
