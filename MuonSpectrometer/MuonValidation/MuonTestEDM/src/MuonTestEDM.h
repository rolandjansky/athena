/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTESTEDM_H
#define MUONTESTEDM_H

#include "TrkValAlgs/TrkEDMTestAlg.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>

#include "TTree.h"

class StoreGateSvc;

namespace Trk {
  class TrkDetElementBase;
}

namespace MuonGM {
  class MuonDetectorManager;
}

class IdentifierHash;


class MuonTestEDM : public TrkEDMTestAlg
{
public:
  MuonTestEDM(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonTestEDM();
  StatusCode	initialize();
  StatusCode	finalize();
  StatusCode runTest();

private:

  /**This is the vector of names of all the track collections*/
  std::vector<std::string>  m_trackCollNames;
      
  /**This is the vector of names of all the segment collections*/
  std::vector<std::string>  m_segmentCollNames;
      
  /**This is the vector of names of all the TrackParticle collections*/
  std::vector<std::string>  m_trackParticleCollNames;
  
  /** name of external file */
  std::string m_rdoDumpFileName;
  std::string m_prdDumpFileName;
  std::string m_trackDumpFileName;
  std::string m_trackPartDumpFileName;
  std::string m_segmentDumpFileName;
  std::string m_summaryDumpFileName;

  /** output file*/
  std::ofstream  m_rdoFileOutput;    
  std::ofstream  m_prdFileOutput; 
  std::ofstream  m_trackFileOutput; 
  std::ofstream  m_trackPartFileOutput; 
  std::ofstream  m_segmentFileOutput; 
  std::ofstream  m_summaryFileOutput; 

  /** if true, dump PRDSs*/
  bool m_dumpRDOs;
  bool m_dumpPRDs;
  bool m_dumpTracks;
  bool m_dumpSegments;
  bool m_dumpVertices;
  bool m_dumpTrackParticles;

};

#endif 
