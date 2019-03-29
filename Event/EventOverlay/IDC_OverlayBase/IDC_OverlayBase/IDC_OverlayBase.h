/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*- 

/**
 * @file
 * 
 * Common base class and generic overlaying code for boolean-like hits.
 * Factored out from InDetOverlay.
 *
 * @author Tadej Novak
 * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2006-2009
 */

#ifndef IDC_OVERLAYBASE_H
#define IDC_OVERLAYBASE_H

#include <AthenaBaseComps/AthAlgorithm.h>

class IDC_OverlayBase : public AthAlgorithm
{
public:
  IDC_OverlayBase(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator) {}

protected:
  template <class IDC_Container>
  StatusCode overlayContainer(const IDC_Container *bkgContainer,
                              const IDC_Container *signalContainer,
                              IDC_Container *outputContainer);

};

#include "IDC_OverlayBase/IDC_OverlayBase.icc"

#endif
