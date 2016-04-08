/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAY_EXAMPLE_HITRDOOVERLAY_H
#define OVERLAY_EXAMPLE_HITRDOOVERLAY_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : HitRdoOverlayExample.h
/// Package : offline/Event/EventOverlay/OverlayExamples
/// Author  : Guillaume Unal
/// Created : December 2007
///
/// DESCRIPTION:
///   An example to demonstrate how to overlay G4 simulated Hits onto real data RDo events
///
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include <string>

class HitRdoOverlayExample : public AthAlgorithm {

public:

  HitRdoOverlayExample(const std::string& name, ISvcLocator* pSvcLocator);
  ~HitRdoOverlayExample();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

private:

  /// a handle of PileUpMergwSvc
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;

  int m_nevt;
};

#endif // OVERLAY_EXAMPLE_HITRDOOVERLAY_H
