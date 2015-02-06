/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEHIJINGEVENTPARS_H
#define MCTRUTHSIMALGS_MERGEHIJINGEVENTPARS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class PileUpMergeSvc;

/** @class MergeHijingPars
 *  @brief an algorithm to copy in the overlayed store the HijingEventParams from the signal event
 *
 *  $Id:
 *  @author John Chapman (from Paolo's MergeMcEventCollection)
 *
 */
class MergeHijingPars : public AthAlgorithm {
public:
  MergeHijingPars(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
