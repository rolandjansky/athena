/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileTBOldNtupleWrite_H
#define TileTBOldNtupleWrite_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// STL includes
#include <string>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TTree.h"

#include "TileConditions/TileCablingService.h"

class TileTBOldNtupleRead;
class TileHWID;
class TileInfo;
class TileCondToolEmscale;

class TileTBOldNtupleWrite : public AthAlgorithm {
public:
  TileTBOldNtupleWrite(std::string name, ISvcLocator* pSvcLocator);
  virtual ~TileTBOldNtupleWrite();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  std::string m_ntupleID;
  std::string m_ntupleDir;
  std::string m_digitsContainer;
  std::string m_rawChannelContainerFit;
  std::string m_rawChannelContainerFlat;

  bool m_bigain;
  bool m_pmtOrder;
  bool m_calibrateEnergy;
  bool m_calibrateEnergyThisEvent;
  
  const TileHWID * m_tileHWID;
  const TileCablingService * m_cabling;

  ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool

  int m_runNumber;
  int m_eventNumber;
  int m_trigType;

  TFile * m_file;
  TTree * m_ntupleWrite;
  TTree * m_ntupleRead;

  TileTBOldNtupleRead * m_readNtupleAlg;

  inline int digiChannel2PMT(int ros, int chan) {
    return (abs(m_cabling->channel2hole(ros,chan))-1); 
  }

  Float_t * BranchWriteNtuple(const std::string name);

  Float_t    m_ene[2][6][48];
  Float_t   m_time[2][6][48];
  Float_t   m_efit[2][6][48];
  Float_t   m_tfit[2][6][48];
  Float_t m_pedfit[2][6][48];
  Float_t   m_chi2[2][6][48];
};

#endif
