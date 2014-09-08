/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class ReadLArRaw
 * @brief CTB: code to read raw channels
 * 
 */

#ifndef ReadLArRaw_H
#define ReadLArRaw_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <fstream>

/////////////////////////////////////////////////////////////////////////////

class LArCablingService ;
class LArOnlineID;
class LArRoI_Map; 
class LArEM_ID;

class ReadLArRaw:public AthAlgorithm {

 public:
  ReadLArRaw (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  std::string m_ChannelContainerName ;
  std::string m_dumpFile;
  std::ofstream m_outFile;


  // Provide the LArCablingService
  ToolHandle<LArCablingService> m_cablingService;

  const LArOnlineID * m_onlineID;
  const LArEM_ID*     m_larem_id;


};

#endif
