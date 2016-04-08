/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READPARTICLE_H
#define ANALYSISTEST_READPARTICLE_H

/*
  templated Read Particle

  @author Tadashi Maeno
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

template <class PARTICLE>
class ReadParticle : public AthAlgorithm
{
public:
  ReadParticle (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ReadParticle () {}
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:

  StatusCode accessNtuple();

  NTuple::Tuple* m_ntuplePtr;

  // Container key
  std::string m_ContainerName;

  // prefix for Ntuple
  std::string m_prefix;
  
  // Ntuple ID
  std::string     m_NtupleLocID;

  // Define variables in the Ntuple:

  NTuple::Item<long>   m_nParticle;
  NTuple::Array<double>  m_pt;
};

#include "AnalysisTest/ReadParticle.icc"

#endif







