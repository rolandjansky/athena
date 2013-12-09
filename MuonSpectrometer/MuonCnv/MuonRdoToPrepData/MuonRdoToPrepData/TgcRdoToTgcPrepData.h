/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRDOTOTGCPREPDATA_H
#define TGCRDOTOTGCPREPDATA_H 

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"


#include <string>
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

/** Algorithm to decode RDO into TgcPrepData, using the Muon::TgcRdoToTgcPrepDataTool 
 @auther Takashi Kubota (Takashi.Kubota@cern.ch)
*/

class TgcRdoToTgcPrepData : public Algorithm {

 public:

  TgcRdoToTgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  void printTgcPrepRawData(); //!< Prints information about the resultant PRDs.

  ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.

  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.

  MsgStream                               m_log;

  bool                                    m_debug; //!< Set in initialize(). If true, output DEBUG info
  bool                                    m_verbose; //!< Set in initialize(). If true, output VERBOSE info

  int                                     m_setting; //!< Setting for debugging
  std::vector<int>                        m_settingVector; //!< Setting for debugging
};

#endif /// TGCRDOTOTGCPREPDATA_H
