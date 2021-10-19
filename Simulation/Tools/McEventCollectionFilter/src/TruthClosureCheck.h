/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHCLOSURECHECK_H
#define TRUTHCLOSURECHECK_H

// Base class include
#include "AthenaBaseComps/AthAlgorithm.h"

// Athena includes
#include "StoreGate/ReadHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"

class TruthClosureCheck : public AthAlgorithm {

public:
   TruthClosureCheck(const std::string& name, ISvcLocator* pSvcLocator);
  ~TruthClosureCheck() = default;
   virtual StatusCode initialize() override final;
   virtual StatusCode execute() override final;

  private:
   StatusCode sanityCheck(const HepMC::GenEvent& event) const;
#ifdef HEPMC3
   StatusCode compareGenVertex(HepMC::ConstGenVertexPtr origVertex,
                               HepMC::ConstGenVertexPtr resetVertex) const;
   StatusCode compareGenParticle(HepMC::ConstGenParticlePtr origParticle,
                                 HepMC::ConstGenParticlePtr resetParticle) const;
   void printGenVertex(HepMC::ConstGenVertexPtr origVertex,
                       HepMC::ConstGenVertexPtr resetVertex) const;
#else
   StatusCode compareGenVertex(const HepMC::GenVertex& origVertex,
                               const HepMC::GenVertex& resetVertex) const;
   StatusCode compareGenParticle(const HepMC::GenParticle& origParticle,
                                 const HepMC::GenParticle& resetParticle) const;
   void printGenVertex(const HepMC::GenVertex& origVertex,
                       const HepMC::GenVertex& resetVertex) const;
#endif
   StatusCode compareMomenta(const HepMC::FourVector& origMomenta,
                             const HepMC::FourVector& resetMomenta) const;

   SG::ReadHandleKey<McEventCollection> m_originalMcEventCollection;
   SG::ReadHandleKey<McEventCollection> m_resetMcEventCollection;
   bool m_compareMomenta{true};
   double m_momentaLimit{0.000000000001};
   bool m_postSimulation{false};
};
#endif
