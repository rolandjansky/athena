/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ListItem.h"

#include "AthenaKernel/errorcheck.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"

#include "CaloUtils/CaloTowerBuilderToolBase.h"


CaloTowerBuilderToolBase::CaloTowerBuilderToolBase(const std::string& name,
    const std::string& type, const IInterface* parent)
  : AthAlgTool(name, type, parent)
  , m_cellContainerName("AllCalo")
{

  // common properties
  declareProperty("CellContainerName", m_cellContainerName);
  declareInterface<ICaloTowerBuilderToolBase>(this);
}

CaloTowerBuilderToolBase::~CaloTowerBuilderToolBase()
{ }

StatusCode CaloTowerBuilderToolBase::initialize ATLAS_NOT_THREAD_SAFE () {
  if (!m_cellContainerName.key().empty()) {
    ATH_CHECK( m_cellContainerName.initialize() );
  }
  ATH_CHECK(m_caloMgrKey.initialize());
 // invoke internal initialization
  return this->initializeTool();
}

void CaloTowerBuilderToolBase::setTowerSeg(const CaloTowerSeg& theTowerSeg) {

  ATH_MSG_DEBUG(" in CaloTowerBuilderToolBase::setTowerSeg ");
  m_theTowerSeg = theTowerSeg;
  ATH_MSG_DEBUG("   neta,nphi,etamin,etamax " << theTowerSeg.neta()
      << " " << theTowerSeg.nphi() << " " << theTowerSeg.etamin()
      << " " << theTowerSeg.etamax());
}


/**
 * @brief Return the tower segmentation.
 */
const CaloTowerSeg& CaloTowerBuilderToolBase::towerSeg() const
{
  return m_theTowerSeg;
}


/**
 * @brief Retrieve cells from StoreGate.
 */
const CaloCellContainer* CaloTowerBuilderToolBase::getCells() const
{
  const CaloCellContainer* cells = nullptr;
  if (!m_cellContainerName.key().empty()) {
    cells = SG::get (m_cellContainerName);
    if (!cells) {
      ATH_MSG_WARNING("no CaloCellContainer with key <"
                      << m_cellContainerName.key() << "> found, skip tool!");
      
    }
  }
  return cells;
}

