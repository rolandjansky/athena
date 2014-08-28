/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleTruthMaker.h
//   Header file for class TrackParticleTruthMaker
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_TRACKPARTICLETRUTHMAKER_H
#define TRUTHPARTICLEALGS_TRACKPARTICLETRUTHMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

namespace Muon {

class TrackParticleTruthMaker : public AthAlgorithm  {

public:

  // Constructor with parameters:
  TrackParticleTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:
  TrackParticleTruthMaker();
  TrackParticleTruthMaker(const TrackParticleTruthMaker&);
  TrackParticleTruthMaker &operator=(const TrackParticleTruthMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  std::string m_trackParticlesName;
  std::string m_trackParticleTruthCollection;
  std::string m_tracksName;
  std::string m_tracksTruthName;

};

} // namespace Muon


#endif //TRUTHPARTICLEALGS_TRACKPARTICLETRUTHMAKER_H
