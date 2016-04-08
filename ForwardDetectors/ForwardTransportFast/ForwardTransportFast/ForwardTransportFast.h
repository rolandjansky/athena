/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARD_TRANSPORT_FAST_H
#define FORWARD_TRANSPORT_FAST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleDataTable.hh"

#include "ForwardTracker/ForwardTrack.h"
#include "ForwardTransportSvc/IForwardTransportSvc.h"

#include <string>

class ForwardTransportFast: public::AthAlgorithm {
  
 public:
  
  ForwardTransportFast(const std::string& name, ISvcLocator* pSvcLocator);
  ~ForwardTransportFast() {};
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() { return StatusCode::SUCCESS; };
 
 private:
  
  ServiceHandle<IPartPropSvc>         m_partPropSvc;
  HepPDT::ParticleDataTable*          m_pdt;
  ServiceHandle<IForwardTransportSvc> m_fwdSvc;
  ForwardTrack                        m_fwdTrack;
}; 

#endif 
