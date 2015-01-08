/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadFeb2Ascii_H
#define LArBadFeb2Ascii_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class LArBadChanTool;

class LArBadFeb2Ascii : public AthAlgorithm 
{
public:
  LArBadFeb2Ascii(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadFeb2Ascii();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ToolHandle< LArBadChanTool > m_BadChanTool;
  std::string                  m_dbFolder;
  std::string                  m_fileName;
};

#endif
