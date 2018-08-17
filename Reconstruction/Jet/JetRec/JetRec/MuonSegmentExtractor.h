// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// IParticleExtractor.h

#ifndef MUONSEGMENTEXTRACTOR_H
#define MUONSEGMENTEXTRACTOR_H

#include "JetRec/IConstituentExtractor.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODJet/Jet.h"
#include <string>

class MuonSegmentExtractor : public IConstituentExtractor{ 

public:
  MuonSegmentExtractor(const xAOD::MuonSegmentContainer*,
                       const std::string & label="");


  virtual ~MuonSegmentExtractor();
  virtual MuonSegmentExtractor* clone() const override;
  virtual MuonSegmentExtractor* ghostClone() const override;
  virtual void 
  addToJet(xAOD::Jet&, const std::vector<int>& indices) const override;
  virtual std::string toString(int level) const override;
  virtual bool checkIntegrity() const override;

private:

  const xAOD::MuonSegmentContainer* m_originalParticles;
  std::string m_label;

};


#endif
