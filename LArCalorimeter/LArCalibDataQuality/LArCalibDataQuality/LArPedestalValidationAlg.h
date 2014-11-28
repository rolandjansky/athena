//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file  LArPedestalValidatonAlg.h
 * @author Walter Lampl <walter.lampl @cern.ch>
 * @date Feb 2008
 * @brief Uses the LArCalibValidationAlg to validate pedestal runs.
 */




#ifndef LARPEDESTALVALIDATIONALG_H
#define LARPEDESTALVALIDATIONALG_H
 
#include <vector>
#include <string>
 
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArCalibDataQuality/LArCalibValidationAlg.h"
#include "CaloIdentifier/CaloCellGroup.h"


typedef LArCalibValidationAlg<LArPedestalComplete> LArPedestalValidationBase;


/** 
  * @class LArCalibValidationAlg
  * @brief Algorithm to validate LAr Pedestal
  * Algorithm to validate LAr Pedestal runs by comparing them channel-by-channel with a reference run.
  * This is the first implementation, more inteded as test-case for the base-class. Needs certainly some
  * refinement.
  */
                                                   
class LArPedestalValidationAlg: public LArPedestalValidationBase {
 
 public:
  /** 
   * @brief Regular algorithm constructor
   */
  LArPedestalValidationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  
 private:
  /** @brief Method to validate the pedestal single readout channels
   */
  bool validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain);

  /** @brief Summary method executed after the loop over all channels */
  StatusCode summary();
  
  /** @brief Executed before the loop over all channels to reset global sums */
  StatusCode preLoop();


  /** Pedestal tolerance (in ADC counts) (job-Property)*/
  //float m_pedTolerance;
  /** Pedestal RMS tolerance (in ADC counts) (job-Property)*/
  //float m_rmsTolerance;

  /** Pedestal tolerances as CaloCellGroupList (job-Properties)  */
  //Expectes three numbers (one per gain)
  std::vector<std::string> m_pedToleranceInit;
  CaloCellGroupList m_pedTolerance;

  
  /** Pedestal RMS tolerances as CaloCellGroupList (job-Properties) */
  //Expectes three numbers (one per gain)
  std::vector<std::string> m_rmsToleranceInit;
  CaloCellGroupList m_rmsTolerance;


  /** Tolerance fro the average pedestal of one FEB (in ADC counts) (job-Property) */
  //float m_pedToleranceFEB;
  std::vector<std::string> m_pedToleranceFEBInit;
  CaloCellGroupList m_pedToleranceFEB;

  /** Tolerance fro the average pedestal RMS (noise) of one FEB (in ADC counts) (job-Property) */
  //float m_rmsToleranceFEB;
  std::vector<std::string> m_rmsToleranceFEBInit;
  CaloCellGroupList m_rmsToleranceFEB;

  //The following is for keeping track of entire FEBs
  /** @brief Method to compare FEB averages */
  bool febSummary();

  class DataPerFEB {
  public:
    DataPerFEB () : chid(0), febid(0), gain(0),
		   pedVal(0.),rmsVal(0.),pedRef(0.),rmsRef(0.), nEntries(0) {}
    DataPerFEB (const HWIdentifier cid, const HWIdentifier fid, int g) :  chid(cid), febid(fid), gain(g),
		   pedVal(0.),rmsVal(0.),pedRef(0.),rmsRef(0.),nEntries(0) {}
    HWIdentifier chid;  //We need to remember the first connected channel for the offline id conversion
    HWIdentifier febid;
    int          gain;
    double       pedVal;
    double       rmsVal;
    double       pedRef;
    double       rmsRef;
    unsigned     nEntries;
  };
  
  std::vector<DataPerFEB> m_vDataPerFEB;

  //The following is for keeping track of the global average
  double m_pedGlobalVal,m_rmsGlobalVal;
  double m_pedGlobalRef,m_rmsGlobalRef;
  unsigned m_nEntriesGlobal;
};


#endif

