/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/**
// TrigTrackParticleTruthMaker.h
//   Header file for class TrigTrackParticleTruthMaker
*/
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_TRIGTRACKPARTICLETRUTHMAKER_H
#define TRUTHPARTICLEALGS_TRIGTRACKPARTICLETRUTHMAKER_H

#include "GaudiKernel/Algorithm.h"
#include <string>
//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

namespace InDet {

class TrigTrackParticleTruthMaker : public  HLT::FexAlgo  {

public:

  //!< Constructor with parameters:
  TrigTrackParticleTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  ///////////////////////////////////////////////////////////////////
  //!< Non-const methods:
  ///////////////////////////////////////////////////////////////////

  //!< Basic algorithm methods:
  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
  virtual HLT::ErrorCode hltFinalize();

  ///////////////////////////////////////////////////////////////////
  //!< Private methods:
  ///////////////////////////////////////////////////////////////////
private:
  TrigTrackParticleTruthMaker();
  TrigTrackParticleTruthMaker(const TrigTrackParticleTruthMaker&);
  TrigTrackParticleTruthMaker &operator=(const TrigTrackParticleTruthMaker&);
  
  ///////////////////////////////////////////////////////////////////
  //!< Private data members:
  ///////////////////////////////////////////////////////////////////
private:
  bool m_doTruth; //!< flag which governs whether the truth association is done or not


  int m_numTracks;
  int m_numTracksTruth;
  int m_numParticleTracks;
  int m_numParticleTracksTruth;

};

} //end of namespace
#endif //TRUTHPARTICLEALGS_TRIGTRACKPARTICLETRUTHMAKER_H
