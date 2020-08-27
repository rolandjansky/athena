/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LArHitFilter_H
#define LARDIGITIZATION_LArHitFilter_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"
#include <vector>

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;


class LArHitFilter : public AthAlgorithm
{
//
//
 public:
  LArHitFilter(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHitFilter();
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

 private:
  StatusCode filterContainer(SG::ReadHandle<LArHitContainer>& inputContainer, SG::WriteHandle<LArHitContainer>& outputContainer, int ical);

  std::vector<bool> m_SubDetFlag;
  std::vector <std::string> m_HitContainer; // hit container name list
  std::vector<int> m_CaloType;

  const LArEM_ID*        m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*      m_larfcal_id;

  double m_ecut[4][4];

  //Configurable Properties
  std::string m_SubDetectors;      // subdetectors
  SG::ReadHandle<LArHitContainer>  m_inputEMBHits;
  SG::WriteHandle<LArHitContainer> m_outputEMBHits;
  SG::ReadHandle<LArHitContainer>  m_inputEMECHits;
  SG::WriteHandle<LArHitContainer> m_outputEMECHits;
  SG::ReadHandle<LArHitContainer>  m_inputHECHits;
  SG::WriteHandle<LArHitContainer> m_outputHECHits;
  SG::ReadHandle<LArHitContainer>  m_inputFCALHits;
  SG::WriteHandle<LArHitContainer> m_outputFCALHits;

};

#endif
