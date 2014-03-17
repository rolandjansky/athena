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

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include <string>

class HitRdoOverlayExample : public Algorithm {

 public:

   HitRdoOverlayExample(const std::string& name, ISvcLocator* pSvcLocator);
   ~HitRdoOverlayExample();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();
 
 private:

   /// a handle on Store Gate 
   StoreGateSvc* m_sgSvc;

   /// a handle of PileUpMergwSvc
   PileUpMergeSvc* p_mergeSvc;

   /// get a handle on the Hist/TTree registration service
   ITHistSvc * m_thistSvc;

   int m_nevt;
};

#endif // OVERLAY_EXAMPLE_HITRDOOVERLAY_H

