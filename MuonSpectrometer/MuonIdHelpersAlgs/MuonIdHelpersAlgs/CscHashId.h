/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCHASHID_H
#define CSCHASHID_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"

class StoreGateSvc;
class CscIdHelper;

/////////////////////////////////////////////////////////////////////////////

class CscHashId: public AthAlgorithm {

public:

  CscHashId (const std::string& name, ISvcLocator* pSvcLocator);
  ~CscHashId()=default;

  StatusCode initialize();
  StatusCode execute();

private:

  StatusCode cscHash();

private:
  const CscIdHelper          * m_cscId;

};

#endif



