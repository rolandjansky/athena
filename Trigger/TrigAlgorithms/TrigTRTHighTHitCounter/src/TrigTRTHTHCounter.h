/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
class IRegSelSvc;
class ITrigTRT_DriftCircleProviderTool;
class IdentifierHash;

class TrigTRTHTHCounter: public HLT::FexAlgo {
 public:
  
  TrigTRTHTHCounter(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrigTRTHTHCounter(void);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();  
  HLT::ErrorCode hltFinalize();  
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  
 private:

  
  /** A data member to retain a connection to the Detector Store. */
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_storeGate;


  const TRT_ID *m_trtHelper;                     //!<  TRT ID helper
  //  ServiceHandle<IRegSelSvc>    m_regionSelector; //!<  region selector service

  std::vector<IdentifierHash> m_listOfTrtIds;    //!<  IDs of DE from regionSelector



  ToolHandle<ITrigTRT_DriftCircleProviderTool> m_rawDataTool; 
  std::vector<int> m_trtDataErrors;
 
  /** A pointer to the DriftCircles container */
  std::string m_trtDCContainerName;

  float m_phiHalfWidth;
  float m_etaHalfWidth;
  bool  m_doFullScan;

  float m_maxCaloEta;
  
  std::vector<float> m_trthits;
  std::vector<float> m_trththits;
  std::vector<float> m_phiWindows;
  static const unsigned int c_phibins;
};

#endif
