/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__X_AOD_TRUTH_H
#define QUICK_ANA__X_AOD_TRUTH_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

// #include <QuickAna/xAODInclude.h>
// // #include <xAODBase/IParticleContainer.h>
// #include <string>

#include <xAODTruth/TruthParticleContainer.h>

namespace asg
{
  class AsgTool;
}

namespace xAOD
{
  class TEvent;
}

#define NAME "TruthParticles"

namespace ana
{
  const xAOD::TruthParticleContainer *
  getCTruth (StoreType& store, const std::string& name = NAME, bool allowNull = false);
#ifdef ROOTCORE
  const xAOD::TruthParticleContainer *
  getCTruth (xAOD::TEvent& store, const std::string& name = NAME, bool allowNull = false);
#endif
  const xAOD::TruthParticleContainer *
  getCTruth (asg::AsgTool& tool, const std::string& name = NAME, bool allowNull = false);
#ifdef ROOTCORE
  const xAOD::TruthParticleContainer *
  getCTruth (const std::string& name = NAME, bool allowNull = false);
#endif
}

#undef NAME

#endif
