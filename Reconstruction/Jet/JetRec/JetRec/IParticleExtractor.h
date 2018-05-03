// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IParticleExtractor.h

#ifndef IParticleExtractor_H
#define IParticleExtractor_H

#include "JetRec/IConstituentExtractor.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/Jet.h"
#include <string>
#include <vector>

class IParticleExtractor : public IConstituentExtractor{ 

public:
  //IParticleExtractor();
  IParticleExtractor(const xAOD::IParticleContainer*,
                     const std::string & label="", 
                     bool isGhost=false,
                     bool isTrigger=false);


  virtual ~IParticleExtractor();
  virtual IConstituentExtractor* ghostClone() const override;
  virtual void 
  addToJet(xAOD::Jet&, const std::vector<int>& indices) const override;
  virtual std::string toString(int level) const override;
  virtual bool checkIntegrity() const override;
private:
  xAOD::IParticleContainer m_iParticles;
  std::string m_label;
  bool m_isGhost; 
  bool m_debug{true};
  bool m_isTrigger{false};
};


#endif
