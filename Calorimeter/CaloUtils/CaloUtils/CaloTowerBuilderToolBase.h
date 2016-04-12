/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloTowerBuilderToolBase.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "StoreGate/StoreGate.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

class CaloTowerContainer;
class CaloCellContainer;
class IGeoAlignTool;

class CaloTowerBuilderToolBase: public AthAlgTool,
    virtual public ICaloTowerBuilderToolBase,
    public IIncidentListener
{
  public:

    /// AlgTool constructor
    CaloTowerBuilderToolBase(const std::string& name, const std::string& type,
        const IInterface* parent);
    virtual ~CaloTowerBuilderToolBase();

    /// common initialization
    virtual StatusCode initialize();

    virtual void setTowerSeg(const CaloTowerSeg& theTowerSeg);

    virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);


    /**
     * @brief Run tower building and add results to the tower container.
     *        If a cell container is provided, use that; otherwise, fetch
     *        from SG (the key is given by a job property).
     *        If @c subseg is provided, then we do the building
     *        only within the rectangular region that it describes.
     *        The segmentation of the tower container must match
     *        the region over which we're running the tower building.
     *
     * @param theContainer The tower container to fill.
     * @param theCell The cell container to read.  If null, we fetch from SG.
     * @param subseg If provided, run tower building only within this window.
     *               The tower container segmentation must match.
     */
    virtual StatusCode execute(CaloTowerContainer* theContainer,
                               const CaloCellContainer* theCell = 0,
                               const CaloTowerSeg::SubSeg* subseg = 0) = 0;

    virtual StatusCode initializeTool() = 0;

    // abstract to be implemented by derived class (Tile, LarFCal, Calo)
    virtual void handle(const Incident&) = 0;

  protected:

    ////////////////
    // Properties //
    ////////////////

    // works only with big CaloCellContainer!
    std::string m_cellContainerName;

    ////////////////////////
    // Store and Services //
    ////////////////////////

    // Warning: m_storeGate is deprecated !!!
    // Method evtStore() from AthAlgTool should be used instead!
    ServiceHandle<StoreGateSvc> m_storeGate;
    CaloTowerSeg m_theTowerSeg;
    bool m_cacheValid;

    //StringProperty m_caloAlignTool;
    ToolHandle<IGeoAlignTool> m_caloAlignTool;

    /////////////////////////////
    // Specific Initialization //
    /////////////////////////////

};
#endif
