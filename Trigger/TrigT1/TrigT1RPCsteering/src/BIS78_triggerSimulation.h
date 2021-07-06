/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BIS78_triggerSimulation_h
#define BIS78_triggerSimulation_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRDO/RpcBis78_TrigRawDataContainer.h"

#include <vector>


/*
  This class is for BIS78 trigger simulation.
  BIS78 trigger goes to TGC sector logic.
  This is different from other BIS chambers of phase 2, in this case the
  trigger is in the barrel logic. A different class may be needed.
*/

/////////////////////////////////////////////////////////////////////////////

class BIS78_triggerSimulation {
  
  public:
  BIS78_triggerSimulation ();
  ~BIS78_triggerSimulation() = default;
    
  StatusCode  clear();
  void  build_trigRawData(Muon::RpcBis78_TrigRawDataContainer* trgContainer, uint8_t dstrip_phi, uint8_t dstrip_eta, uint16_t bcid);

  unsigned int  AddStrip(int sectoreta, int sectorphi, int igap, int mphi, int strip);

  private:

  // These are to collect all the strip data
  // The first index is the side (0 or 1) because it is just for BIS78 (otherwise, for full BIS, it should be the eta sector)
  std::vector<int> m_strip_eta[2][8][3];  // side, sector, gasgap
  std::vector<int> m_strip_phi[2][8][3];  // side, sector, gasgap

  std::unique_ptr<Muon::RpcBis78_TrigRawDataContainer> m_bisTrgContainer;
};


#endif
