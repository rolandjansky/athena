/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READMDTRDO_H
#define READMDTRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/NTuple.h"

/////////////////////////////////////////////////////////////////////////////

class ReadMdtRDO: public AthAlgorithm {

 public:
  ReadMdtRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:
 
  NTuple::Tuple* m_ntuplePtr;

 private:

  StatusCode accessNtuple();

  bool m_mdtNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nCsm;    // number of Csm in the container  
  NTuple::Item<long>   m_nAmt;           // number of Amt  
  NTuple::Array<long>  m_subId;          //  SubDetId()
  NTuple::Array<long>  m_csmId;          // Csm Id CsmId()
  NTuple::Array<long>  m_rodId;          //  MrodId()
  NTuple::Array<long>   m_tdcId;          //  tdcId() 
  NTuple::Array<long>  m_channelId;         // channelId()
  NTuple::Array<long>  m_fine;      // fine()
  NTuple::Array<long>  m_coarse;      // coarse() 
  NTuple::Array<long>  m_width;      // width()
};

#endif







