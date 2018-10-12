// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TrackExtractor.h

#ifndef TrackExtractor_H
#define TrackExtractor_H

#include "JetRec/IConstituentExtractor.h"
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include <string>

class TrackExtractor : public IConstituentExtractor {

public:
  TrackExtractor(const xAOD::TrackParticleContainer* tps,
                         const std::string & label,
                         bool isGhost);

  virtual ~TrackExtractor();
  virtual TrackExtractor* clone() const override;
  virtual TrackExtractor* ghostClone() const override;
  virtual void 
  addToJet(xAOD::Jet&, const std::vector<int>& indices) const override;
  virtual std::string toString(int level) const override;
  virtual bool checkIntegrity() const override;

private:
  const xAOD::TrackParticleContainer* m_originalParticles;
  std::string m_label;
  bool m_isGhost; 

};

#endif
