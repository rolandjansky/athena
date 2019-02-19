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
 * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2006-2009
 *
 */

#ifndef IDC_OVERLAYBASE_H
#define IDC_OVERLAYBASE_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"
#include "IDC_OverlayBase/IDC_OverlayCommon.h"

class IDC_OverlayBase;

namespace Overlay {

  /**
   *  Merging of hits on the same channel.  Modifies the first argument by adding data from the second..
   *  Implementations can assume that the arguments have the same Id.  (That is, correspond to the same readout channel.)
   *  
   *  A generic implementation of this template is provided in this package, 
   *  but subdetectors can provide specializations suitable for their RDO type.
   *  A simple example of providing a specialization is in InDetOverlay.cxx (for TRT_RDORawData).
   *  A more elaborate implementation can be found e.g. in MdtOverlay.cxx
   *  
   *  Note that a declaration of a specialized template must occur before it is used
   *  by overlayContainer()/mergeCollections() [typically from the execute() method of your algorithm].
   */
  template<class Datum> void mergeChannelData(Datum& r1, const Datum& r2, IDC_OverlayBase* parent);

  /**
   *  Adds data and mc from the second collection to the output one merging where necessary.
   *  After this data and mc collections stay unchanged and output collection contains all information
   * 
   *  A generic implementation in the .icc file can be overriden by a
   *  specialization, see above.
   */

  template<class Collection> void mergeCollectionsNew(Collection *mc_coll, Collection *data_coll, Collection *out_coll, IDC_OverlayBase* parent);
}


class IDC_OverlayBase : public OverlayAlgBase  {
public:
  
  IDC_OverlayBase(const std::string &name, ISvcLocator *pSvcLocator)
    : OverlayAlgBase(name, pSvcLocator)
  {}

  /**
   *  Transfers all collections from the first and second arguments to the output the first merging where necessary.
   */
  template<class IDC_Container> void overlayContainer(const IDC_Container* data, const IDC_Container* mc, IDC_Container* out ) {
    Overlay::overlayContainer(data, mc, out, this);
  }

  template<class IDC_Container> void overlayContainerNew(const IDC_Container* data, const IDC_Container* mc, IDC_Container* out ) {
    Overlay::overlayContainerNew(data, mc, out, this);
  }

  template<class IDC_Container> std::string shortPrint(const IDC_Container *container, unsigned numprint = 25) {
    return Overlay::shortPrint(container, numprint);
  }

  /**
   *  Adds data and mc from the second collection to the output one merging where necessary.
   *  After this data and mc collections stay unchanged and output collection contains all information
   */
  template<class Collection> void mergeCollections(Collection *mc_coll, Collection *data_coll, Collection *out_coll);

};

#include "IDC_OverlayBase/IDC_OverlayBase.icc"

#endif/*IDC_OVERLAYBASE_H*/
