/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTPERFORMANCEALG_H
#define MUONSEGMENTPERFORMANCEALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>
#include <fstream>
#include "xAODMuon/MuonSegmentContainer.h"

class MuonSegmentPerformanceAlg : public AthAlgorithm {
 public:
  
  // Algorithm Constructor
  MuonSegmentPerformanceAlg (const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonSegmentPerformanceAlg()=default;
  
  // Gaudi algorithm hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  bool retrieve(const SG::ReadHandleKey<xAOD::MuonSegmentContainer>&, const xAOD::MuonSegmentContainer*& ptr) const;
  std::string printRatio( std::string prefix, unsigned int begin, unsigned int end, const std::vector<int> reco, const std::vector<int> truth ) const;
  std::string printRatio( std::string prefix, unsigned int begin, unsigned int end, const std::vector<int> reco ) const;


  /** name of external file to write statistics */
  bool m_writeToFile;
  std::string m_fileName;
  
  /** output file*/
  std::ofstream  m_fileOutput;  

  //statistics to be counted
  SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_segmentKey;
  SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_truthSegmentKey;
 
  unsigned int                    m_nevents;
  std::vector<int>                m_nhitCuts;
  std::vector<std::string>        m_hitCutString;
  std::vector< std::vector<int> > m_ntruth;
  std::vector< std::vector<int> > m_nfound;
  std::vector< std::vector<int> > m_nfake;

};

#endif // MUONPERFORMANCEALG


