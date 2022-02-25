/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READRPCRDO_H
#define READRPCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/NTuple.h"

/////////////////////////////////////////////////////////////////////////////

class ReadRpcRDO: public AthAlgorithm {

 public:
  ReadRpcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:
  NTuple::Tuple* m_ntuplePtr;

 private:
  StatusCode accessNtuple();
  bool m_rpcNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nPads;          // number of pads in the container  
  NTuple::Item<long>   m_nCMA;           // number of coincidence matrices  
  NTuple::Item<long>   m_nFiredChannels; // number of fired channel  
  NTuple::Array<long>  m_sector;         // pad sector Id
  NTuple::Array<long>  m_padId;          // pad Id
  NTuple::Array<long>  m_status;         // status
  NTuple::Array<long>  m_errorCode;      // error code
  NTuple::Array<long>  m_cmaId;          // CMA Id
  NTuple::Array<long>  m_fel1Id;         // fel1Id
  NTuple::Array<long>  m_febcId;         // febcId
  NTuple::Array<long>  m_crc;            // crc
  NTuple::Array<long>  m_bcId;           // bc Id
  NTuple::Array<long>  m_time;           // time
  NTuple::Array<long>  m_ijk;            // ijk code
  NTuple::Array<long>  m_channel;        // fired channel
};

#endif







