/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPERFORMANCEALG_H
#define MUONPERFORMANCEALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>
#include <fstream>
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "StoreGate/ReadHandleKey.h"

class MuonPerformanceAlg : public AthAlgorithm {
 public:
  
  // Algorithm Constructor
  MuonPerformanceAlg (const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonPerformanceAlg(){};
  
  // Gaudi algorithm hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  //statistics to be counted
  std::vector<std::string> m_muonLocationList;
  
 private:

  bool passID(const xAOD::TrackParticle*, bool debug) const;

  /** name of external file to write statistics */
  bool m_writeToFile;
  std::string m_fileName;
  
  /** output file*/
  std::ofstream  m_fileOutput;  

  // Containers
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsNameKey{this,"MuonContainerName","Muons","muon container"};
  unsigned int                    m_nevents;
  std::vector<std::string>        m_hitCutString;
  std::vector<int>                m_ntruth;
  std::vector<int>                m_ntruth5;
  std::vector<int>                m_ntruth10;
  std::vector<int>                m_nfound;
  std::vector<int>                m_nfound5;
  std::vector<int>                m_nfound10;
  std::vector<int>                m_nfoundr;
  std::vector<int>                m_nfoundr5;
  std::vector<int>                m_nfoundr10;
  std::vector<int>                m_nreco;
  std::vector<int>                m_nreco5;
  std::vector<int>                m_nreco10;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthMuons{this,"TruthMuons","MuonTruthParticles","truth muons"};

  int m_runNumber;
  int m_eventNumber;

  void print(std::string txt, const xAOD::TruthParticle* muon);
  void print(std::string txt, const xAOD::Muon* muon);

  IntegerArrayProperty m_pdgsToBeConsidered;
  std::set<int> m_selectedPdgs; // set storing particle PDG's considered for matching
  bool selectPdg( int pdg ) const { return m_selectedPdgs.count(pdg); }
 
};

#endif // MUONPERFORMANCEALG


