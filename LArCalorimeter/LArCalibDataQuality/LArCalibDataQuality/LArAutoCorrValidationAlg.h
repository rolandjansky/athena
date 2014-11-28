//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @LArAutoCorrValidatonAlg.h
 * @date Feb 2008
 * @brief Uses the LArCalibValidationAlg to validate the noise autocorrelation measurement
 */

#ifndef LARAUTOCORRVALIDATIONALG_H
#define LARAUTOCORRVALIDATIONALG_H
 
#include <vector>
#include <string>
 
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArCalibDataQuality/LArCalibValidationAlg.h"
#include "CaloIdentifier/CaloCellGroup.h"

typedef LArCalibValidationAlg<LArAutoCorrComplete> LArAutoCorrValidationBase;


/** 
  * @class LArCalibValidationAlg
  * @brief Algorithm to validate LAr Autocorr; 
  */
                                                   
class LArAutoCorrValidationAlg: public LArAutoCorrValidationBase 
{
  

 
 public:
  /** 
   * @brief Regular algorithm constructor
   */
  LArAutoCorrValidationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  
 private:
  /** @brief Method to validate readout channels
   */
  bool validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain);

  /** @brief Summary method executed after the loop over all channels */
  StatusCode summary();
  
  /** @brief Executed before the loop over all channels to reset global sums */
  StatusCode preLoop();


  /** Covariace tolerance of one FEB  (job-Property)*/
  //float m_covTolerance;
  std::vector<std::string> m_covToleranceFEBInit;
  CaloCellGroupList m_covToleranceFEB;

  /** Covariance tolerance (job-Property) */
  //float m_covToleranceFEB;
  std::vector<std::string> m_covToleranceInit;
  CaloCellGroupList m_covTolerance;

  /** Number of elements of the Covariance vector that should be checked */
  unsigned m_nSamplesToCheck;

  bool m_checkFifthSample;

  //The following is for keeping track of entire FEBs
  /** @brief Method to compare FEB averages */
  bool febSummary();

  class DataPerFEB {
  public:
    DataPerFEB () : chid(0), febid(0), gain(0),
		   covVal(0.),covRef(0.) {}
    DataPerFEB (const HWIdentifier cid, const HWIdentifier fid, int g) : chid(cid), febid(fid), gain(g),
		   covVal(0.),covRef(0.),nEntries(0) {}
    HWIdentifier chid;  //We need to remember the first connected channel for the offline id conversion
    HWIdentifier febid;
    int          gain;
    double       covVal;
    double       covRef;
    unsigned     nEntries;
  };
  
  std::vector<DataPerFEB> m_vDataPerFEB;

  //The following is for keeping track of the global average
  double m_covGlobalVal,m_covGlobalRef;
  unsigned m_nEntriesGlobal;
};


#endif

