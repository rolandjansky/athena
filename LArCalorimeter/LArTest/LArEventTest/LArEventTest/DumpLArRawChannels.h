/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPLARRAWCHANNEL_H
#define DUMPLARRAWCHANNEL_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"


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
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey {this,"keyCabling", "LArOnOffIdMap", "Input key for Id mapping"};
  int m_count, m_chan;
  //LArRawChannelContainer* m_larChannelCont;
  const LArOnlineID*       m_onlineHelper; 
  const LArEM_ID* m_emId;
  std::ofstream m_outfile;
  std::string m_key, m_FileName;
 private:

  std::string m_ntup;
  TTree* m_tree=nullptr;
  int m_evt=0;
  float m_e=0,m_t=0,m_Q=0;
  unsigned m_gain=0;
  unsigned m_id=0;

  ServiceHandle<ITHistSvc> m_thistSvc;

  class mySort {
  public:
    bool operator()(const LArRawChannel* a, const LArRawChannel* b);
  };
};

#endif
