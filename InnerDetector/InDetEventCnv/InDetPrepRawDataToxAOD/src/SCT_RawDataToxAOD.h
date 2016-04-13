// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATATOXAOD_H
#define SCT_RAWDATATOXAOD_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

class ISvcLocator;
class SCT_ID;

/** Algorithm to read RDO information from SCT ntuple and write augmented xAOD.
 **/
class SCT_RawDataToxAOD : public AthAlgorithm {
public:
  SCT_RawDataToxAOD(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  const SCT_ID* m_SCTHelper;

  std::string m_clustercontainer;
//  std::string m_SDOcontainer;
//  std::string m_sihitContainer;
//  std::string m_multiTruth;
};

#endif
