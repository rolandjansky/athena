/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPLARRAWCHANNEL_H
#define DUMPLARRAWCHANNEL_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"

//#include "LArDetDescr/LArDetDescrManager.h"
#include <fstream>

class DumpLArRawChannels : public AthAlgorithm
{
 public:
  DumpLArRawChannels(const std::string & name, ISvcLocator * pSvcLocator);

  ~DumpLArRawChannels();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  int m_count, m_chan;
  //LArRawChannelContainer* m_larChannelCont;
  const LArOnlineID*       m_onlineHelper; 
  LArCablingService *m_larCablingSvc;
  const LArEM_ID* m_emId;
  std::ofstream m_outfile;
  std::string m_key, m_FileName;
 private:
  class mySort {
  public:
    bool operator()(const LArRawChannel* a, const LArRawChannel* b);
  };
};

#endif
