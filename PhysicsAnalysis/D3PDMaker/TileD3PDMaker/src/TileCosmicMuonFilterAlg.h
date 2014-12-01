/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCosmicMuonFilterAlg.h
 * Author: Carlos.Solans@cern.ch
 * Author: Archil.Durglishvili@cern.ch
 *
 * July 2014
 */

#ifndef TILECOSMICMUONFILTERALG_H
#define TILECOSMICMUONFILTERALG_H

//Local track tools
#include "TrackTools.h"

//xAOD Muons
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"


//Type definitions
typedef xAOD::Muon MUON;
typedef xAOD::MuonContainer     MUONCONTAINER;
typedef xAOD::MuonAuxContainer     MUONAUXCONTAINER;
typedef std::vector<std::vector<int> > ASSOCCONTAINER;

#define ASSOCCONTAINER_CONSTRUCTOR(size) ASSOCCONTAINER( size ,std::vector<int>(0))

//================================================
class TileCosmicMuonFilterAlg : public AthAlgorithm{
//================================================

public:
  
  TileCosmicMuonFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  ~TileCosmicMuonFilterAlg(){};

  virtual StatusCode initialize();
  virtual StatusCode execute(); 

private:

  std::string m_inputTracks;
  std::string m_outputTracks;
  std::string m_inputMuons;
  std::string m_outputMuons;
  std::string m_inputCells;
  std::string m_outputCells;
  std::string m_muonCells;
  float       m_muonPt;
  float       m_muonEta;
  float       m_muonCellR;
  bool        m_dumpLarCells;
  bool        m_useCuts;
  short       m_trackType;

  StoreGateSvc* m_storeGate;

  // TOOLHANDLES
  ToolHandle<ITrackTools> m_trackInCalo;
  
}; 

#endif
