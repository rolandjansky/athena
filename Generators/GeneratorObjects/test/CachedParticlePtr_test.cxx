/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeneratorObjects/CachedParticlePtr_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Unit tests for CachedParticlePtr
 */


#undef NDEBUG
#include "GeneratorObjects/CachedParticlePtr.h"
#include "AtlasHepMC/GenParticle.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

#ifdef HEPMC3
  HepMC::ConstGenParticlePtr gp1 = std::make_shared<HepMC::GenParticle>();
#else
  HepMC::GenParticle gp0;
  HepMC::ConstGenParticlePtr gp1 = &gp0;
#endif

  GeneratorObjects::CachedParticlePtr p1 (gp1);
  assert (p1.get() == gp1);

  GeneratorObjects::CachedParticlePtr p2;
  assert (p2.get() == nullptr);
  assert (!p2.isValid());
  p2.set (gp1);
  assert (p2.get() == gp1);
  assert (p2.isValid());
  p2.reset();
  assert (p2.get() == nullptr);

  GeneratorObjects::CachedParticlePtr p3 (p1);
  assert (p1.get() == gp1);
  p2 = p1;
  assert (p2.get() == gp1);
}


int main()
{
  std::cout << "CachedParticlePtr_test\n";

  test1();

  return 0;
}
