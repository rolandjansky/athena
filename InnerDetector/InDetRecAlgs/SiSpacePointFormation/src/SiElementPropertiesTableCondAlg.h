/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiElementPropertiesTableCondAlg_h
#define SiElementPropertiesTableCondAlg_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"

class SCT_ID;

namespace InDet {

  /**
   * @class SiElementPropertiesTableCondAlg
   * This class is a condition algorithm and inherits AthReentrantAlgorithm.
   * This class reads SCT_DetectorElementCollection from condition store,
   * prepares SCT "neighbours" using SCT_ElementPropertiesTable class,
   * register it in condition store.
   * The created SCT_ElementPropertiesTable object in condition store
   * is used by SiTrackerSpacePointFinder.
   */

  class SiElementPropertiesTableCondAlg : public AthReentrantAlgorithm {
  public:
    /**
     * @name Methods of AthReentrantAlgorithm
     */
    //@{
    SiElementPropertiesTableCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiElementPropertiesTableCondAlg() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };
    //@}

  private:
    /**
     * @name Input data using SG::ReadCondHandleKey
     */
    //@{
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_readKey{this, "ReadKey", "SCT_DetectorElementCollection", "Key of input SiDetectorElementCollection for SCT"};
    //@}

    /**
     * @name Output data using SG::WriteCondHandleKey
     */
    //@{
    SG::WriteCondHandleKey<InDet::SiElementPropertiesTable> m_writeKey{this, "WriteKey", "SCT_ElementPropertiesTable", "Key of output SiElementPropertiesTable for SCT"};
    //@}

    /**
     * @name Parameter for SiElementPropertiesTable and SiElementProperties
     */
    //@{
    FloatProperty m_epsWidth{this, "EpsWidth", 0.02, "Safety margin for half-widths, in cm"};
    //@}

    /**
     * @name Id helper
     */
    //@{
    const SCT_ID* m_idHelper{nullptr};
    //@}
  };

}

#endif // SiElementPropertiesTableCondAlg_h
