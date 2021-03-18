/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthParticlesInConeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PARTICLESINCONETOOLS_ITRUTHPARTICLESINCONETOOL_H
#define PARTICLESINCONETOOLS_ITRUTHPARTICLESINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "xAODTruth/TruthParticle.h"
#include <vector>

namespace xAOD {

static const InterfaceID
  IID_ITruthParticlesInConeTool("xAOD::ITruthParticlesInConeTool", 1, 0);

/** @class ITruthParticlesInConeTool
    @brief interface for collecting truth particles inside a cone
    @author Niels van Eldik
 */
class ITruthParticlesInConeTool : virtual public IAlgTool
{
public:
  static const InterfaceID& interfaceID();

  /**ITruthParticlesInConeTool interface:
     @param[in] eta       eta for matching
     @param[in] phi       phi for matching
     @param[in] dr        cone size
     @param[in] output    output vector to be filled
     @return true if the calculation was successfull
   */
  virtual bool particlesInCone(
    const EventContext& ctx,
    float eta,
    float phi,
    float dr,
    std::vector<const TruthParticle*>& output) const = 0;
};

inline const InterfaceID&
ITruthParticlesInConeTool::interfaceID()
{
  return IID_ITruthParticlesInConeTool;
}

} // end of namespace

#endif
