/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiElementPropertiesTableCondAlg_h
#define SiElementPropertiesTableCondAlg_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"

#include "GaudiKernel/ICondSvc.h"

class SCT_ID;

namespace InDet {

  class SiElementPropertiesTableCondAlg : public AthReentrantAlgorithm {
  public:
    SiElementPropertiesTableCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiElementPropertiesTableCondAlg() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_readKey{this, "ReadKey", "SCT_DetectorElementCollection", "Key of input SiDetectorElementCollection for SCT"};
    SG::WriteCondHandleKey<InDet::SiElementPropertiesTable> m_writeKey{this, "WriteKey", "SCT_ElementPropertiesTable", "Key of output SiElementPropertiesTable for SCT"};

    FloatProperty m_epsWidth{this, "EpsWidth", 0.02, "Safety margin for half-widths, in cm"};

    ServiceHandle<ICondSvc> m_condSvc;
    const SCT_ID* m_idHelper;
  };

}

#endif // SiElementPropertiesTableCondAlg_h
