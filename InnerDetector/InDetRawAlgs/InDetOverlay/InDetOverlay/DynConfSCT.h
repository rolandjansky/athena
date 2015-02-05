/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file
 * Dynamically configure the concrete type of SCT RDOs, on event per event basis.
 * The "real data" input stream defines the type that should be used.  We need
 * to make sure that SCT digitization that runs before InDetOverlay produces
 * the output in the same format.
 *
 * @author Andrei Gaponenko, 2008-2009
 */

#ifndef INDETOVERLAY_DYNCONFSCT_H
#define INDETOVERLAY_DYNCONFSCT_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// forward declaring template typedefs is unwieldy
#include "InDetRawData/SCT_RDO_Container.h"

class StoreGateSvc;
class AlgTool;

class DynConfSCT : public AthAlgorithm
{
public:

  DynConfSCT(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  bool m_inputFormatDetermined;

  //----------------
  //! These two members are NOT properties. They are automatically set
  //! to the same values that are used by InDetOverlay
  ServiceHandle<StoreGateSvc> m_storeGateData;
  std::string m_mainInputSCT_Name;

  //! Use DataStore of this InDetOverlay alg to get "real data" SCT inputs
  std::string m_InDetOverlayName;

  //! this is the alg we control: set its output type to what we see in real data.
  std::string m_SCT_DigitizationName;
  AlgTool *m_digiAlgTool;

  //----------------
  //! gets a non-null RDO, if any
  const SCT_RDORawData *getRDO(const SCT_RDO_Container *cont) const;
};

#endif/*INDETOVERLAY_DYNCONFSCT_H*/
