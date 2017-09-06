/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadFeb2Ascii_H
#define LArBadFeb2Ascii_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"

#include <string>


class LArBadFeb2Ascii : public AthAlgorithm 
{
public:
  LArBadFeb2Ascii(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadFeb2Ascii();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey;
  std::string m_fileName;

};

#endif
