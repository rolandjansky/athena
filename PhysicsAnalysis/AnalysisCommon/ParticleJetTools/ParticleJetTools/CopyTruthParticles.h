/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHPARTICLES_H
#define COPYTRUTHPARTICLES_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTruth/TruthParticle.h"


class CopyTruthParticles : public AthAlgorithm {
public:

  /// Constructor
  CopyTruthParticles(const std::string& name, ISvcLocator* pSvcLocator);

  /// @name Event loop algorithm methods
  //@{
  StatusCode initialize() { return StatusCode::SUCCESS; }
  StatusCode execute();
  StatusCode finalize() { return StatusCode::SUCCESS; }
  //@}

  /// Classifier function(s)
  virtual bool classify(const xAOD::TruthParticle* tp) = 0;


protected:

  /// Name of output collection
  std::string m_outputname;

  /// Minimum pT for particle selection (in MeV)
  double m_ptmin;

};


#endif
