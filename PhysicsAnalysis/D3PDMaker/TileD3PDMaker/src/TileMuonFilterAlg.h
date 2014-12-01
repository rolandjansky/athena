/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMuonFilterAlg.h
 * Author: Carlos.Solans@cern.ch
 * Author: Archil.Durglishvili@cern.ch
 *
 * Nov 2014
 */

#ifndef TILEMUONFILTERALG_H
#define TILEMUONFILTERALG_H

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
class TileMuonFilterAlg : public AthAlgorithm{
//================================================

public:
  
  TileMuonFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  ~TileMuonFilterAlg(){};

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
  float       m_isoCone;
  float       m_ErelCut;
  bool        m_dumpLarCells;
  bool        m_useCuts;
  short       m_trackType;

  StoreGateSvc* m_storeGate;

  // TOOLHANDLES
  ToolHandle<ITrackTools> m_trackInCalo;

}; 

#endif
