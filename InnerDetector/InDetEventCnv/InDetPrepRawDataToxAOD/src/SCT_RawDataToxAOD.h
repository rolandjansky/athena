// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATATOXAOD_H
#define SCT_RAWDATATOXAOD_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

// StoreGate Data Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
// SCT ntuple container type
#include "InDetRawData/SCT_RDO_Container.h"
// xAOD container type
#include "xAODTracking/SCTRawHitValidationContainer.h"

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
  SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerName;
  SG::WriteHandleKey<xAOD::SCTRawHitValidationContainer> m_xAodRawHitContainerName;
};

#endif
