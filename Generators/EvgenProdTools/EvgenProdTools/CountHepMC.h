/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_COUNTHEPMC_H
#define EVGENPRODTOOLS_COUNTHEPMC_H

#include "GeneratorModules/GenBase.h"


/// @brief Count the number of events to pass all algorithms/filters
///
/// CountHepMC counts the number of events to pass algs in the evgen 
/// production chain. It signals to the AthenaEventLoopMgr service
/// when the requested number of events are produced.
///
class CountHepMC : public GenBase {
public:

  CountHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CountHepMC();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ServiceHandle< StoreGateSvc > m_metaDataStore{
    "StoreGateSvc/MetaDataStore", name()};
  int m_nPass;
  int m_nCount;

  int m_firstEv;
  int m_newRunNumber;

  bool m_corHepMC;
  bool m_corEvtID;
  bool m_corRunNumber;
  std::string m_inputKeyName;  
};


#endif

#endif
