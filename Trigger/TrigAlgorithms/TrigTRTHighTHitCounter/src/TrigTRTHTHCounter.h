/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTHTHCOUNTER_H
#define TRIGTRTHTHCOUNTER_H

#include "TrigInterfaces/FexAlgo.h"
#include <string>
#include <vector>


/** @class TrigTRTHTHCounter
    @author Jiri Masik <Jiri.Masik@hep.manchester.ac.uk>
    @brief TrigTRTHTHCounter gives back the number of TRT high-threshold hits 
           based on the code of HIPsFilterAlg by Sergey Burdin 

*/

class TRT_ID;
class ITrigTRT_DriftCircleProviderTool;
class IdentifierHash;

class TrigTRTHTHCounter: public HLT::FexAlgo {
 public:
  
  TrigTRTHTHCounter(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrigTRTHTHCounter(void);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();  
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  
 private:

  
  const TRT_ID *m_trtHelper;                     //!<  TRT ID helper

  std::vector<IdentifierHash> m_listOfTrtIds;    //!<  IDs of DE from regionSelector



  std::vector<int> m_trtDataErrors;
 
  /** A pointer to the DriftCircles container */
  std::string m_trtDCContainerName;

  float m_phiHalfWidth;
  float m_etaHalfWidth;
  bool  m_doFullScan;

  float m_maxCaloEta;
  float m_roadWidth;
  int m_nBinCoarse;
  int m_nBinFine;
  float m_wedgeMinEta;
  float m_roadMaxEta;
  int m_wedgeNBin;

  std::vector<float> m_trththits;
};

#endif
