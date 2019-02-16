/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*- 

/**
 * @file
 * 
 * Overlaying of Identifiable Containers.  Functions common to the
 * single-hit-per-readout-channel-in-an-event case and the
 * possible-multiple-hits-on-a-channel case are declared in this file.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2009
 *
 */

#ifndef IDC_OVERLAYCOMMON_H
#define IDC_OVERLAYCOMMON_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class IDC_OverlayBase;

namespace Overlay {

  template<class Collection> void copyCollection(const Collection *input_coll, Collection *copy_coll);

  //Forward decl for compiling ...
  template<class Collection> void mergeCollectionsNew(Collection*, Collection*, Collection*, IDC_OverlayBase*);

  /**
   *  Transfers all collection from the second argument the first merging where necessary.
   *  After this call the "data" container contains all information, and the "mc" 
   *  container is empty.
   */
  template<class IDC_Container, class OvlAlg> void overlayContainer(const IDC_Container* data, const IDC_Container* mc,  IDC_Container* out, OvlAlg *parent);

  template<class IDC_Container, class OvlAlg> void overlayContainerNew(const IDC_Container* dataContainer, const IDC_Container* mcContainer, IDC_Container* outputContainer, OvlAlg *parent);

  /** Diagnostic output */
  template<class IDC_Container> std::string shortPrint(const IDC_Container *container, unsigned numprint = 25);

}

#include "IDC_OverlayBase/IDC_OverlayCommon.icc"

#endif/*IDC_OVERLAYCOMMON_H*/
