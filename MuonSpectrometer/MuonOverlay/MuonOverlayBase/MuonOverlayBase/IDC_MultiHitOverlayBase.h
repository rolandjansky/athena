/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file
 * 
 * Common base class and generic overlaying code for boolean-like hits.
 * Multi hit overlay n1 hits + n hits -> m hits
 *
 * @author Ketevi A. Assamagan, 2009
 *
 */

#ifndef IDC_MULTIHITOVERLAYBASE_H
#define IDC_MULTIHITOVERLAYBASE_H

#include <string>
#include <vector>

#include "OverlayAlgBase/OverlayAlgBase.h"
#include "IDC_OverlayBase/IDC_OverlayCommon.h"

class StoreGateSvc;

class IDC_MultiHitOverlayBase;

namespace Overlay {

  /**
   *  Merging of hits on the same channel.  Modifies the first argument by adding data from the second..
   *  Implementations can assume that the arguments have the same Id.  (That is, correspond to the same readout channel.)
   *  
   *  A generic implementation of this template is provided in this package, 
   *  but subdetectors can provide specializations suitable for their RDO type.
   *  A simple example of providing a specialization is in InDetOverlay.cxx (for TRT_RDORawData).
   *  A more elaborate implementation can be found e.g. in MdtOverlay.cxx
   *  Here we general same channel hit  merging to the case where we have
      n1 hits + n2 Hits -> n3 Hits. This may corresponds to the cases of the RPC and TGC
   *  
   */
  template<class Datum> 
  void mergeMultiHits(std::vector<Datum*>& dataHits, std::vector<Datum*>& mcHits, 
                      std::vector<Datum*>& outHits, IDC_MultiHitOverlayBase* parent);
}


class IDC_MultiHitOverlayBase : public OverlayAlgBase  {
public:
  
  IDC_MultiHitOverlayBase(const std::string &name, ISvcLocator *pSvcLocator)
    : OverlayAlgBase(name, pSvcLocator)
  {}

  /**
   *  Transfers all collections from the first and second arguments to the output the first merging where necessary.
   */
  template<class IDC_Container> void overlayContainer(const IDC_Container* data, const IDC_Container* mc, IDC_Container* output) {
    Overlay::overlayContainer(data, mc, output, this);
  }

  template<class IDC_Container> void overlayContainer(const std::auto_ptr<IDC_Container>& data, const std::auto_ptr<IDC_Container>& mc, const std::auto_ptr<IDC_Container>& output) {
    this->overlayContainer(data.get(), mc.get(), output.get());
  }

  template<class IDC_Container> std::string shortPrint(const IDC_Container *container, unsigned numprint = 25) {
    return Overlay::shortPrint(container, numprint);
  }
  
  template<class IDC_Container> std::string shortPrint(const std::auto_ptr<IDC_Container>& ac, unsigned numprint = 25) {
    return Overlay::shortPrint(ac, numprint);
  }

  /**
   *  Adds data from the second collection to the first merging where necessary.
   *  After this call the "data" collection contains all information, and the "mc"
   *  collection is empty.
   */
  template<class Collection> void mergeCollections(Collection *mc_coll, Collection *data_coll, Collection *out_coll);
 
};

#include "MuonOverlayBase/IDC_MultiHitOverlayBase.icc"

#endif/*IDC_MULTIHITOVERLAYBASE_H*/
