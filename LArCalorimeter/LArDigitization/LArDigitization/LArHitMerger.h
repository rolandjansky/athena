/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LArHitMerger_H
#define LARDIGITIZATION_LArHitMerger_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include <vector>

class StoreGateSvc;
class PileUpMergeSvc;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class LArHitMerger : public AthAlgorithm
{
//
//
 public:
  LArHitMerger(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHitMerger();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  PileUpMergeSvc *p_mergeSvc;
  
  std::vector<bool> m_SubDetFlag;
  std::vector <std::string> m_HitContainer; // hit container name list
  std::vector<int> m_CaloType;

  std::string m_SubDetectors;      // subdetectors  
  std::string m_EmBarrelHitContainerName; 
  std::string m_EmEndCapHitContainerName;
  std::string m_HecHitContainerName;
  std::string m_ForWardHitContainerName;
 
  const LArEM_ID*        m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*      m_larfcal_id;
};

#endif
