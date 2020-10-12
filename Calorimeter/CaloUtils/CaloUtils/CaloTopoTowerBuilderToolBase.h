/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOPOTOWERBUILDERTOOLBASE_H
#define CALOREC_CALOTOPOTOWERBUILDERTOOLBASE_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTopoTowerBuilderToolBase is abstract base class for tower builders
///
/// CaloTopoTowerBuilderToolBase is an abstract base class for a tower builder 
/// tool.
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \date April 30, 2004 - first implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloTopoTowerBuilderToolBase.h"
#include "CaloEvent/CaloTowerSeg.h"
#include <string>

class CaloTowerContainer;
class CaloCellContainer;
class IGeoAlignTool;

class CaloTopoTowerBuilderToolBase : public AthAlgTool,
				 virtual public ICaloTopoTowerBuilderToolBase,
                                 public IIncidentListener
{
 public:
  
  /// AlgTool constructor
  CaloTopoTowerBuilderToolBase(const std::string& name, const std::string& type,
			   const IInterface* parent);
  virtual ~CaloTopoTowerBuilderToolBase();

  /// common initialization
  virtual StatusCode initialize() override;

  virtual void setTowerSeg(const CaloTowerSeg& theTowerSeg) override;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) override;

  /// execute is abstract
  virtual StatusCode execute(CaloTopoTowerContainer* theContainer,const CaloCellContainer* theCell=0) const override = 0;

  virtual StatusCode initializeTool() override = 0;

  // abstract to be implemented by derived class (Tile, LarFCal, Calo)
  virtual void handle(const Incident&) override = 0;

 protected:

  ////////////////
  // Properties //
  ////////////////

  ////////////////////////
  // Store and Services //
  ////////////////////////

  CaloTowerSeg m_theTowerSeg;

  //bool m_cacheValid;

  ToolHandle<IGeoAlignTool> m_caloAlignTool;
  

  /////////////////////////////
  // Specific Initialization //
  /////////////////////////////

};
#endif
