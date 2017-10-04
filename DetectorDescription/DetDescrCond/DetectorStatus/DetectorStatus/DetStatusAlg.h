/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETSTATUS_DETSTATALG_H
#define DETSTATUS_DETSTATALG_H
// DetStatusAlg.h - a simple algorithm illustrating access to detector status
// information via the DetStatusSvc
// Richard Hawkings, started 18/12/06

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"

class IDetStatusSvc;

class DetStatusAlg: public AthAlgorithm {
 public:
  DetStatusAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~DetStatusAlg();

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);  

 private:
  void doVeto();

  const IDetStatusSvc* m_detstatussvc;
  bool m_par_print;
  bool m_par_veto;
  unsigned int m_count;
  unsigned int m_nveto;
};

#endif // DETSTATUS_DETSTATALG_H
