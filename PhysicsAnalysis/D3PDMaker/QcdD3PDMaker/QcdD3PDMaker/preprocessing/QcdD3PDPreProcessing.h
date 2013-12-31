/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QCDD3PDMAKER_QCDD3PDPREPROCESSING_H
#define QCDD3PDMAKER_QCDD3PDPREPROCESSING_H

#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <vector>

class IBeamCondSvc;

namespace QcdD3PD {

class QcdD3PDPreProcessing : public Algorithm {
public:
  QcdD3PDPreProcessing(const std::string& name, ISvcLocator* pSvcLocator);
  ~QcdD3PDPreProcessing();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /* Pointer to storegate **/
  StoreGateSvc* m_storeGate;

  /* the beam condition service **/
  IBeamCondSvc* m_iBeamCondSvc;
  std::string   m_beamCondSvcName;

  StatusCode fillBeamSpot();

}; // class QcdD3PDPreProcessing

} // namespace QcdD3PD

#endif // not QCDD3PDMAKER_QCDD3PDPREPROCESSING_H
