/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_READMDTPRD_H
#define MUONBYTESTREAMCNVTEST_READMDTPRD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/NTuple.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/////////////////////////////////////////////////////////////////////////////

class ReadMdtPRD: public AthAlgorithm {

 public:
  ReadMdtPRD (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();

 protected:
  NTuple::Tuple* m_ntuplePtr;

 private:
  StatusCode accessNtuple();
  bool m_mdtNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nColl;    // number of collection in the container
  NTuple::Item<long>   m_nPrd;     // number of digit in the collection 
  NTuple::Array<float> m_tdc;
  NTuple::Array<float> m_adc;
  NTuple::Array<float> m_multi;    // Return the multilayer number
  NTuple::Array<float> m_layer;    // Return the layer number
  NTuple::Array<float> m_wire;     // Return the tube number
};

#endif







