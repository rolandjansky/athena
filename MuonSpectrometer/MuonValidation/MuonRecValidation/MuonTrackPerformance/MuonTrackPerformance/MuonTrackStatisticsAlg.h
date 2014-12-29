/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSTATISTICSALG_MUONTRACKSTATISTICSALG_H
#define MUONTRACKSTATISTICSALG_MUONTRACKSTATISTICSALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <fstream>

class MsgStream;
class StoreGateSvc;
class MuonTrackStatisticsTool;

class MuonTrackStatisticsAlg : public AthAlgorithm
{
 public:
  
  // Algorithm Constructor
  MuonTrackStatisticsAlg (const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonTrackStatisticsAlg(){};
  
  // Gaudi algorithm hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  //statistics to be counted
  std::vector<std::string> m_trackLocationList;
  
 private:

    // Cache the StoreGateSvc ptr for efficiency
    StoreGateSvc*	p_SGevent;

    //ToolHandle<Muon::MuonEDMHelperTool>   m_helperTool;
    ToolHandle <MuonTrackStatisticsTool>    m_statisticsTool;
    
    /** name of external file to write statistics */
    std::string m_fileName;

    /** output file*/
    std::ofstream  m_fileOutput;  

    int			m_print_level;
    bool                m_writeToFile;
    bool                m_doTruth;
    MsgStream*          m_log;



    void storeTruthTracks(void);
 
};

#endif // MUONTRACKSTATISTICSALG_MUONTRACKSTATISTICSALG_H


