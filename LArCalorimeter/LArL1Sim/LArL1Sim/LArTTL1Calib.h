/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARL1SIM_LARTTL1CALIB_H
#define LARL1SIM_LARTTL1CALIB_H
// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Institut ......: ISN Grenoble                                        +
// + Creation date .: 20/04/2004                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"


class StoreGateSvc;
class CaloLVL1_ID;

/**
   @brief This algorithm is meant to be run in standalone only for TTL1 calibration purposes
 */
class LArTTL1Calib : public AthAlgorithm
{
//
// >>>>>>>> public methods
//
 public:
  /** constructor */
  LArTTL1Calib(const std::string& name, ISvcLocator* pSvcLocator);

/** destructor */
  ~LArTTL1Calib();
//
// ..... Gaudi algorithm hooks
//
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

//
// >>>>>>>> private algorithm parts
//

  void decodeInverseTTChannel(int region, int layer, int eta, int & bec, int & emhf, int & Ieta) const;

//
// >>>>>>>> private data parts
//

  float   m_thresholdGeVTTs ;
  int     m_maxNtt ; 

// pointer to ntuple
  NTuple::Tuple* m_pnt;

// variables to be in ntuple.  
  
  NTuple::Item<long>   m_ntotem; 
  NTuple::Item<float>  m_etotem;  
  NTuple::Item<long>   m_n3x3em; 
  NTuple::Item<float>  m_e3x3em;  
  NTuple::Item<float>  m_emostem;  
  NTuple::Item<long>   m_bec_mostEem; 
  NTuple::Item<long>   m_emfcal_mostEem; 
  NTuple::Item<long>   m_Ieta_mostEem; 
  NTuple::Item<long>   m_Lphi_mostEem; 
  NTuple::Item<long>   m_nttem; 
  NTuple::Array<float>  m_ettem;
  NTuple::Array<long>  m_becttem; 
  NTuple::Array<long>  m_emfcalttem; 
  NTuple::Array<long>  m_Ietattem;
  NTuple::Array<long>  m_Lphittem;

  NTuple::Item<long>   m_ntothad; 
  NTuple::Item<float>  m_etothad;  
  NTuple::Item<long>   m_n3x3had; 
  NTuple::Item<float>  m_e3x3had;  
  NTuple::Item<float>  m_emosthad;  
  NTuple::Item<long>   m_hecfcal_mostEhad; 
  NTuple::Item<long>   m_Ieta_mostEhad; 
  NTuple::Item<long>   m_Lphi_mostEhad; 
  NTuple::Item<long>   m_ntthad; 
  NTuple::Array<float>  m_etthad;
  NTuple::Array<long>  m_hecfcaltthad; 
  NTuple::Array<long>  m_Ietatthad;
  NTuple::Array<long>  m_Lphitthad;
  
  const CaloLVL1_ID*                 m_lvl1Helper;

};

#endif












