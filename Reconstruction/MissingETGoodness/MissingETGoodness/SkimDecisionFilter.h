// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_SkimDecisionFilter_H
#define MissingETGoodness_SkimDecisionFilter_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MissingETGoodness/Goodies.h"

#include <string>

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class SkimDecisionFilter : public AthAlgorithm 
{
 public:
  SkimDecisionFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~SkimDecisionFilter(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  // MB: reference to MET Goodies map for storing derived met goodness quantities
  static MET::Goodies& s_goodies;
  
  std::string m_containerName;

  StoreGateSvc* m_metaStoreInInit;

};

#endif // MissingETGoodness_SkimDecisionFilter_H

