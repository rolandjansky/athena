/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READCSCDIGIT_H
#define READCSCDIGIT_H

/*
  Conversion byte stream to CSC Digit ...

  Author: Ketevi A. Assamagan
  BNL, December 31 2003
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/////////////////////////////////////////////////////////////////////////////

class ReadCscDigit: public AthAlgorithm
{
public:
  ReadCscDigit (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:
  NTuple::Tuple* m_ntuplePtr;
  ServiceHandle<ActiveStoreSvc> m_activeStore;

private:
  StatusCode accessNtuple();
  bool m_cscNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nColl;    // number of collection in the container
  NTuple::Item<long>   m_nDig;     // number of digit in the collection 

  NTuple::Array<long>  m_stationName; // ID information
  NTuple::Array<long>  m_stationEta;
  NTuple::Array<long>  m_stationPhi;
  NTuple::Array<long>  m_chamberLayer;
  NTuple::Array<long>  m_wireLayer;
  NTuple::Array<long>  m_measuresPhi;
  NTuple::Array<long>  m_strip;
  NTuple::Array<long>  m_charge;  // the charge on the strip
};

#endif
