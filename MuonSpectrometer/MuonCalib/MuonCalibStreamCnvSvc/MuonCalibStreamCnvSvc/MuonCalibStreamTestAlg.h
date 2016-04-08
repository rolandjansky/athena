/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMTESTALG_H
#define MUONCALIBSTREAMTESTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGate.h"
#include "MuonIdHelpers/MdtIdHelper.h"

class MuonCalibStreamTestAlg: public AthAlgorithm 
{

public:

  MuonCalibStreamTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonCalibStreamTestAlg();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();


protected:

  ActiveStoreSvc* m_activeStore;
  const MdtIdHelper* m_mdtIdHelper;

private:

  int m_byCont;
  int m_byColl;

};

#endif
