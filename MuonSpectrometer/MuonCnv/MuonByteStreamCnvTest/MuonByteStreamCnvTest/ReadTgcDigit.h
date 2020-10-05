/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READTGCDIGIT_H
#define READTGCDIGIT_H

/*
  Test byte stream converters, write to and read from byte stream ...

  @author Tadashi Maeno
    based on ReadMdtDigit by Michela Biglietti
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/NTuple.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/////////////////////////////////////////////////////////////////////////////

class ReadTgcDigit: public AthAlgorithm
{
public:
  ReadTgcDigit (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();

protected:
  NTuple::Tuple* m_ntuplePtr;
  ServiceHandle<ActiveStoreSvc> m_activeStore;

private:
  StatusCode accessNtuple();
  bool m_tgcNtuple;

  //Ntuple ID
  std::string     m_NtupleLocID;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nColl;    // number of collection in the container
  NTuple::Item<long>   m_nDig;     // number of digit in the collection 

  NTuple::Array<long>  m_stationName; // ID information
  NTuple::Array<long>  m_stationEta;
  NTuple::Array<long>  m_stationPhi;
  NTuple::Array<long>  m_gasGap;
  NTuple::Array<long>  m_isStrip;
  NTuple::Array<long>  m_channel;
  NTuple::Array<long>  m_bcTag;
};

#endif







