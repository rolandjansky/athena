// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_EtmissGoodnessSelectorAlg_H
#define MissingETGoodness_EtmissGoodnessSelectorAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MissingETGoodness/EtmissGoodness.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////

class EtmissGoodnessSelectorAlg : public AthAlgorithm 
{
 public:
  EtmissGoodnessSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~EtmissGoodnessSelectorAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  int m_quality;
  std::string m_qualifier;
  MET::EtmissGoodness m_etmissgoodness;
};

#endif // MissingETGoodness_EtmissGoodnessSelectorAlg_H

