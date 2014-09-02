/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHTAGINPUTS_COPYTRUTHPARTICLES_H
#define TRUTHTAGINPUTS_COPYTRUTHPARTICLES_H

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
  bool classify(const xAOD::TruthParticle* tp);


private:

  /// Name of output collection
  std::string m_outputname;
  /// Particle selection mode
  std::string m_ptype;
  /// Minimum pT for particle selection (in MeV)
  double m_ptmin;

};


#endif
