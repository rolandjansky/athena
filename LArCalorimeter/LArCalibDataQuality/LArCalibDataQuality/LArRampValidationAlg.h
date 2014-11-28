//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file  LArRampValidatonAlg.h
 * @author Jessica Leveque <jessica.leveque @cern.ch>
 * @date March 2008
 * @brief Uses the LArCalibValidationAlg to validate ramp runs.
 */




#ifndef LARRAMPVALIDATIONALG_H
#define LARRAMPVALIDATIONALG_H
 
#include <vector>
#include <string>
 
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArRawRampContainer.h"
#include "LArCalibDataQuality/LArCalibValidationAlg.h"
#include "CaloIdentifier/CaloCellGroup.h"

typedef LArCalibValidationAlg<LArRampComplete> LArRampValidationBase;


/** 
  * @class LArCalibValidationAlg
  * @brief Algorithm to validate LAr Ramp
  * Algorithm to validate LAr Ramp runs by comparing them channel-by-channel with a reference run.
  * This is the first implementation, more inteded as test-case for the base-class. Needs certainly some
  * refinement.
  */
                                                   
class LArRampValidationAlg: public LArRampValidationBase {
 
 public:
  /** 
   * @brief Regular algorithm constructor
   */
  LArRampValidationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  
 private:
  /** @brief Method to validate the ramps single readout channels
   */
  bool validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain);

  /** @brief Summary method executed after the loop over all channels */
  StatusCode summary();
  
  /** @brief Executed before the loop over all channels to reset global sums */
  StatusCode preLoop();

  /** To store Gain Keys for Raw Ramps (job-Property)*/
  std::vector<std::string> m_contKey;
  /** Raw Ramp time tolerance (in ns) (job-Property)*/
  float m_rawrampTimeTolerance;
  /** Raw Ramp ADC (job-Property)*/
  float m_rawrampTimeADC;

  /** Ramp tolerance as init string for CellGroupObject (job-Property)*/
  std::vector<std::string> m_toleranceInit;

  /** Ramp tolerance as CaloCellGroup object */
  CaloCellGroupList m_tolerance;

  /** Ramp tolerance (FEB average) as CaloCellGroup object (job-Property)*/
  std::vector<std::string> m_toleranceInitFEB;

  /** Ramp tolerance (FEB average) as initializer string CaloCellGroup object */
  CaloCellGroupList m_toleranceFEB;

  /** To check if Raw Ramps are found */
  bool m_hasRawRampContainer;

  //The following is for keeping track of entire FEBs
  /** @brief Method to compare FEB averages */
  bool febSummary();

  //The following is used to look for channels deviating from average
  bool deviateFromAvg(const LArCondObj& val, const HWIdentifier chid, const int gain);

  class DataPerFEB {
  public:
    DataPerFEB () : chid(0), febid(0), gain(0),
		   rampVal(0.),rmsVal(0.),rampRef(0.),rmsRef(0.), nEntries(0) {}
    DataPerFEB (const HWIdentifier cid, const HWIdentifier fid, int g) : chid(cid), febid(fid), gain(g),
		   rampVal(0.),rmsVal(0.),rampRef(0.),rmsRef(0.),nEntries(0) {}
    HWIdentifier chid;  //We need to remember the first connected channel for the offline id conversion
    HWIdentifier febid;
    int          gain;
    double       rampVal;
    double       rmsVal;
    double       rampRef;
    double       rmsRef;
    unsigned     nEntries;
  };
  
  class DataPerSector {
  public:
    DataPerSector () : pos_neg(0), region(0), layer(0), eta(0), gain(0),
		   rampVal(0.),rmsVal(0.),rampRef(0.),rmsRef(0.), nEntries(0) {}
    DataPerSector (int pn, int reg, int lay, int ieta, int g) :  pos_neg(pn), region(reg), layer(lay), eta(ieta), gain(g),
		   rampVal(0.),rmsVal(0.),rampRef(0.),rmsRef(0.),nEntries(0) {}
    int          pos_neg;
    int          region;
    int          layer;
    int          eta;
    int          gain;
    double       rampVal;
    double       rmsVal;
    double       rampRef;
    double       rmsRef;
    unsigned     nEntries;
  };

  std::vector<DataPerFEB> m_vDataPerFEB;
  std::vector<DataPerSector> m_vDataPerSector;

  LArRawRampContainer* m_rawRampContainer;

  //The following is for keeping track of the global average
  double m_rampGlobalVal,m_rmsGlobalVal;
  double m_rampGlobalRef,m_rmsGlobalRef;
  unsigned m_nEntriesGlobal;
};


#endif

