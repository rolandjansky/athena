/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * dual-use tool for converting xAOD truth events back to HepMC
 * Principal Authors (responsible for the core conversion algorithm): Josh McFayden and James Catmore
 * Tool Author: Jim Lacey (Carleton University) 
 * ... updated tool interface to be dual-use
 * ... added missing PDF information and requirements to allow running on full sim (remove Geant4 secondaries)
 * <james.lacey@cern.ch,jlacey@physics.carleton.ca>
 * <dag.gillberg@cern.ch>
 */

#ifndef GENINTERFACES_IXAODTOHEPMCTOOL_H
#define GENINTERFACES_IXAODTOHEPMCTOOL_H 1

#include "AsgTools/IAsgTool.h"


// STL includes
#include <string>
#include <map>

#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthEventContainer.h"

#include "HepMC/GenEvent.h"

namespace HepMC {
  class GenEvent;
  class GenParticle;
  class GenVertex;
}

class IxAODtoHepMCTool : public virtual asg::IAsgTool {
 public:
  ASG_TOOL_INTERFACE( IxAODtoHepMCTool )  
    virtual ~  IxAODtoHepMCTool () { };
 public:
  virtual StatusCode  initialize() = 0;
  virtual StatusCode finalize () = 0;
  virtual std::vector<HepMC::GenEvent> getHepMCEvents(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::EventInfo* eventInfo) = 0;
};

#endif //> !GENINTERFACES_IXAODTOHEPMCTOOL_H
