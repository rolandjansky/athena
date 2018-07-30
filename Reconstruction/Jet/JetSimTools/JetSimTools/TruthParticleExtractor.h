// this file is -*- C++ -*-
// TruthParticleExtractor.h


/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthParticleExtractor_H
#define TruthParticleExtractor_H

#include "JetRec/IConstituentExtractor.h"
#include "xAODJet/Jet.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <string>

class TruthParticleExtractor : public IConstituentExtractor {

public:
  TruthParticleExtractor(const xAOD::TruthParticleContainer* tps,
                         const std::string & label,
                         bool isGhost);

  virtual ~TruthParticleExtractor();
  virtual TruthParticleExtractor* clone() const override;
  virtual TruthParticleExtractor* ghostClone() const override;
  virtual void 
  addToJet(xAOD::Jet&, const std::vector<int>& indices) const override;
  virtual std::string toString(int) const override;
  virtual bool checkIntegrity() const override;

private:
  const xAOD::TruthParticleContainer* m_originalParticles;
  std::string m_label;
  bool m_isGhost; 

};

#endif
