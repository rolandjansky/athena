/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LArBadChannel2Ascii_H
#define LArBadChannel2Ascii_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class LArBadChanTool;
class StoreGateSvc;
class LArCablingService;

class LArBadChannel2Ascii : public AthAlgorithm 
{
public:
  LArBadChannel2Ascii(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadChannel2Ascii();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode stop();

private:

  ToolHandle< LArBadChanTool > m_BadChanTool;
  ToolHandle<LArCablingService> m_larCablingSvc;

  std::string                  m_dbFolder;
  std::string                  m_fileName;
  std::string                  m_executiveSummaryFile;
  bool                         m_wMissing;
  bool                         m_skipDisconnected;

  enum DetPart {
    EMB=0,
    EMEC,
    HEC,
    FCAL,
    nParts
  };

  enum CoarseProblemType {
    DeadReadout=0,
    DeadPhys,
    DeadCalib,
    DeadFEB,
    Noisy,
    Sporadic,
    GrandTotalDead,
    nProblemTypes 
  };

  void writeSum(std::ofstream& exeFile, std::vector<unsigned> probs) const;


};

#endif
