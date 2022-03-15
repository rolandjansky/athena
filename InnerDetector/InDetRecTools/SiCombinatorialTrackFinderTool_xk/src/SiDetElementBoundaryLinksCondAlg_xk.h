/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiDetElementBoundaryLinksCondAlg_xk_h
#define SiDetElementBoundaryLinksCondAlg_xk_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLinks_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

namespace InDet {

  /**
   * @class SiDetElementBoundaryLinksCondAlg_xk
   *
   * Prepare a condition object Pixel/SCT_DetElementBoundaryLinks_xk
   * from a condition object Pixel/SCT_DetectorElementCollection
   * for SiCombinatorialTrackFinder_xk and stores it in condition
   * store using ReadCondHandle and WriteCondHandle.
   * In AthenaMT, detector elements are condition objects and
   * this condition algorithm is necessary.
   */

  class SiDetElementBoundaryLinksCondAlg_xk : public AthReentrantAlgorithm {
  public:
    SiDetElementBoundaryLinksCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiDetElementBoundaryLinksCondAlg_xk() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;
    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };

  private:
    /** Input condition object, detector elements of Pixel or SCT in condition store. */
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_readKey{this, "ReadKey", "SCT_DetectorElementCollection", "Key of input SiDetectorElementCollection"};
    /** Output condition object for SiCombinatorialTrackFinder_xk */
    SG::WriteCondHandleKey<InDet::SiDetElementBoundaryLinks_xk> m_writeKey{this, "WriteKey", "SCT_DetElementBoundaryLinks_xk", "Key of output SiDetElementBoundaryLinks_xk"};
    BooleanProperty m_ITkGeometry{this, "ITkGeometry", false};
  };

}

#endif // SiDetElementBoundaryLinksCondAlg_xk_h
