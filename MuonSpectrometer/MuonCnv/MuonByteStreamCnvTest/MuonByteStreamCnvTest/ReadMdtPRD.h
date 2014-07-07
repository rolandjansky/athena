/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_READMDTPRD_H
#define MUONBYTESTREAMCNVTEST_READMDTPRD_H

#include "GaudiKernel/Algorithm.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "GaudiKernel/NTuple.h"
#include "StoreGate/StoreGate.h"

/////////////////////////////////////////////////////////////////////////////

class MsgStream;

class ReadMdtPRD: public Algorithm {

 public:
  ReadMdtPRD (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:
 
  NTuple::Tuple* m_ntuplePtr;
  StoreGateSvc* m_eventStore;

 private:

  StatusCode accessNtuple();

  bool m_mdtNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  const MdtIdHelper* m_mdtIdHelper;
  
  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nColl;    // number of collection in the container
  NTuple::Item<long>   m_nPrd;     // number of digit in the collection 
  NTuple::Array<float> m_tdc;
  NTuple::Array<float> m_adc;
  NTuple::Array<float> m_multi;    // Return the multilayer number
  NTuple::Array<float> m_layer;    // Return the layer number
  NTuple::Array<float> m_wire;     // Return the tube number

  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;
  
};

#endif







