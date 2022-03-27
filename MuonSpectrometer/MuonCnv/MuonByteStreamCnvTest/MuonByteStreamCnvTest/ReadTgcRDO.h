/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READTGCRDO_H
#define READTGCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/NTuple.h"

/////////////////////////////////////////////////////////////////////////////

class ReadTgcRDO: public AthAlgorithm
{
public:
  ReadTgcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:
  NTuple::Tuple* m_ntuplePtr;
  TgcRdoIdHash* m_hashFunc;

private:
  StatusCode accessNtuple();
  bool m_tgcNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nRdo;      // number of RDOs in the container  
  NTuple::Item<long>   m_nRawData;  // number of RawData
  NTuple::Array<long>  m_l1Id;      // L1ID on ROD
  NTuple::Array<long>  m_bcId;      // BCID on ROD
  NTuple::Array<long>  m_bcTag;     // BC Tag
  NTuple::Array<long>  m_l1IdSLB;   // L1ID on SLB
  NTuple::Array<long>  m_bcIdSLB;   // BCID on SLB
  NTuple::Array<long>  m_subId;     // Sub-detector ID
  NTuple::Array<long>  m_rodId;     // ROD ID
  NTuple::Array<long>  m_sswId;     // SSW ID
  NTuple::Array<long>  m_slbId;     // SLB ID
  NTuple::Array<long>  m_bitPos;    // Bit position
  NTuple::Array<long>  m_isCoincidence;    // Bit position
  NTuple::Array<long>  m_type;    // Bit position
  NTuple::Array<long>  m_index;    // Bit position
  NTuple::Array<long>  m_pos;    // Bit position
  NTuple::Array<long>  m_delta;    // Bit position

};

#endif







