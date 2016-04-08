/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READCONSTITUENT_H
#define ANALYSISTEST_READCONSTITUENT_H

/*
  templated Read constituent

  @author Tadashi Maeno
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

template <class CONT>
class ReadConstituent : public AthAlgorithm
{
public:
  ReadConstituent (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ReadConstituent () {}
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

protected:

  StatusCode accessNtuple();

  NTuple::Tuple* m_ntuplePtr;

  // Container key
  std::string m_ContainerName;

  // prefix for Ntuple
  std::string m_prefix;
  
  // Ntuple ID
  std::string m_NtupleLocID;

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nCon;

  // user defined action
  virtual StatusCode userInit() { return StatusCode::SUCCESS; } 
  virtual StatusCode userExec(const typename CONT::base_value_type *) { return StatusCode::SUCCESS; } 
};

#include "AnalysisTest/ReadConstituent.icc"

#endif







