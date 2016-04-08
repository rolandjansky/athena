/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file
 *
 * Code to set up access to two StoreGate-s for overlaying algorithms.
 * Factored out from InDetOverlay.
 *
 * @author Andrei Gaponenko, 2006-2009
 */

#ifndef OVERLAYALGBASE_H
#define OVERLAYALGBASE_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;

/**
 * This class sets up acces to the two event stores and makes sure
 * they are syncronized.  It also handles McEventCollection.
 *
 * Algorithms inheriting from OverlayAlgBase should not redefine
 * initialize(), execute() and finalize() but provide
 * overlayInitialize() etc. instead.
 */
class OverlayAlgBase : public AthAlgorithm
{
public:

  OverlayAlgBase(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //----------------
  virtual StatusCode overlayInitialize() = 0;
  virtual StatusCode overlayExecute() = 0;
  virtual StatusCode overlayFinalize() = 0;

protected:
  // ----------------------------------------------------------------
  ServiceHandle<StoreGateSvc> m_storeGateData;
  ServiceHandle<StoreGateSvc> m_storeGateMC;
  ServiceHandle<StoreGateSvc> m_storeGateOutput; //FIXME this should be dropped in favour of the evtStore method of AthAlgorithm

  template<class TypeToBeRemoved> void removeAllObjectsOfType(StoreGateSvc *sg);
  template<class TypeToBeCopied> void copyAllObjectsOfType(StoreGateSvc *to, StoreGateSvc *from);
};

#include "OverlayAlgBase/OverlayAlgBase.icc"

#endif/*OVERLAYALGBASE_H*/
