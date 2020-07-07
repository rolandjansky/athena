/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTPERFORMANCEALG_H
#define MUONSEGMENTPERFORMANCEALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>
#include <fstream>
#include "xAODMuon/MuonSegmentContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

class MuonSegmentPerformanceAlg : public AthAlgorithm {
 public:
  
  // Algorithm Constructor
  MuonSegmentPerformanceAlg (const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonSegmentPerformanceAlg(){};
  
  // Gaudi algorithm hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  
 private:
  bool retrieve( std::string location, xAOD::MuonSegmentContainer*& ) const;
  std::string printRatio( std::string prefix, unsigned int begin, unsigned int end, const std::vector<int> reco, const std::vector<int> truth ) const;
  std::string printRatio( std::string prefix, unsigned int begin, unsigned int end, const std::vector<int> reco ) const;


  /** name of external file to write statistics */
  bool m_writeToFile;
  std::string m_fileName;
  
  /** output file*/
  std::ofstream  m_fileOutput;  

  //statistics to be counted
  std::string m_segmentLocation;
  std::string m_truthSegmentLocation;
 
  unsigned int                    m_nevents;
  std::vector<int>                m_nhitCuts;
  std::vector<std::string>        m_hitCutString;
  std::vector< std::vector<int> > m_ntruth;
  std::vector< std::vector<int> > m_nfound;
  std::vector< std::vector<int> > m_nfake;

  ToolHandle<Muon::MuonIdHelperTool>  m_idHelper;

};

#endif // MUONPERFORMANCEALG


