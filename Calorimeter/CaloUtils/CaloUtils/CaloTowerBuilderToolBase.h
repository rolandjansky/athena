/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERBUILDERTOOLBASE_H
#define CALOREC_CALOTOWERBUILDERTOOLBASE_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTowerBuilderToolBase is abstract base class for tower builders
///
/// CaloTowerBuilderToolBase is an abstract base class for a tower builder 
/// tool.
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \date April 30, 2004 - first implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloTowerBuilderToolBase.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloCellContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandle.h"

#include <string>

#include "CxxUtils/checker_macros.h"
class CaloTowerContainer;
class CaloCellContainer;
class IGeoAlignTool;

class CaloTowerBuilderToolBase: public AthAlgTool,
    virtual public ICaloTowerBuilderToolBase
{
  public:

    /// AlgTool constructor
    CaloTowerBuilderToolBase(const std::string& name, const std::string& type,
        const IInterface* parent);
    virtual ~CaloTowerBuilderToolBase();

    /// common initialization
    virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;

    virtual void setTowerSeg(const CaloTowerSeg& theTowerSeg) override;


    /**
     * @brief Run tower building and add results to the tower container.
     *        If a cell container is provided, use that; otherwise, fetch
     *        from SG (the key is given by a job property).
     *        If @c subseg is provided, then we do the building
     *        only within the rectangular region that it describes.
     *        The segmentation of the tower container must match
     *        the region over which we're running the tower building.
     *
     * @param ctx The current event context.
     * @param theContainer The tower container to fill.
     * @param theCell The cell container to read.  If null, we fetch from SG.
     * @param subseg If provided, run tower building only within this window.
     *               The tower container segmentation must match.
     */
    virtual StatusCode execute(const EventContext& ctx,
                               CaloTowerContainer* theContainer,
                               const CaloCellContainer* theCell = nullptr,
                               const CaloTowerSeg::SubSeg* subseg = nullptr) const override = 0;

    /**
     * @brief Run tower building and add results to the tower container.
     * @param ctx The current event context.
     * @param theContainer The tower container to fill.
     *
     * If the segmentation hasn't been set, take it from the tower container.
     * This is for use by converters.
     */
    virtual StatusCode execute (const EventContext& ctx,
                                CaloTowerContainer* theContainer) = 0;


    virtual StatusCode initializeTool() override = 0;

  protected:

    /**
     * @brief Return the tower segmentation.
     */
    const CaloTowerSeg& towerSeg() const;


    /**
     * @brief Retrieve cells from StoreGate.
     */
    const CaloCellContainer* getCells() const;


    ////////////////
    // Properties //
    ////////////////

    SG::ReadHandleKey<CaloCellContainer> m_cellContainerName;
    // ReadHandle to CaloDetDescr CDO in CondStore
    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager","CaloDetDescrManager"};


private:
    ////////////////////////
    // Store and Services //
    ////////////////////////

    CaloTowerSeg m_theTowerSeg;

};
#endif
