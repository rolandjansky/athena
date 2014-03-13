// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_EtmissGoodnessManagerAlg_H
#define MissingETGoodness_EtmissGoodnessManagerAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include "MissingETGoodness/Goodies.h"

/////////////////////////////////////////////////////////////////////////////

class StoreGateSvc;

class EtmissGoodnessManagerAlg : public AthAlgorithm 
{
 public:
  EtmissGoodnessManagerAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~EtmissGoodnessManagerAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  // MB: reference to MET Goodies map for storing derived met goodness quantities
  static MET::Goodies& goodies;

 private:

  int m_version;
  int m_quality;

  StoreGateSvc* m_storeGate;
};

#endif // MissingETGoodness_EtmissGoodnessManagerAlg_H

