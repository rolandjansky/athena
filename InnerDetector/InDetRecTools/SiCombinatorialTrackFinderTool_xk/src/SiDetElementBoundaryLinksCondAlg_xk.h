/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiDetElementBoundaryLinksCondAlg_xk_h
#define SiDetElementBoundaryLinksCondAlg_xk_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLinks_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

namespace InDet {

  class SiDetElementBoundaryLinksCondAlg_xk : public AthReentrantAlgorithm {
  public:
    SiDetElementBoundaryLinksCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiDetElementBoundaryLinksCondAlg_xk() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_readKey{this, "ReadKey", "SCT_DetectorElementCollection", "Key of input SiDetectorElementCollection for SCT"};
    SG::WriteCondHandleKey<InDet::SiDetElementBoundaryLinks_xk> m_writeKey{this, "WriteKey", "SCT_DetElementBoundaryLinks_xk", "Key of output SiDetElementBoundaryLinks_xk for SCT"};

    ServiceHandle<ICondSvc> m_condSvc;
  };

}

#endif // SiDetElementBoundaryLinksCondAlg_xk_h
