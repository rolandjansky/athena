/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ListItem.h"

#include "CaloUtils/CaloTopoTowerBuilderToolBase.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"

CaloTopoTowerBuilderToolBase::CaloTopoTowerBuilderToolBase(const std::string& name,
						   const std::string& type,
						   const IInterface* parent)
  : AthAlgTool(name,type,parent),
    m_caloAlignTool("CaloAlignTool")
{
  // common properties
  declareInterface<ICaloTopoTowerBuilderToolBase>(this);
}

CaloTopoTowerBuilderToolBase::~CaloTopoTowerBuilderToolBase()
{ }

StatusCode CaloTopoTowerBuilderToolBase::initialize()
{
  return this->initializeTool();
}

void CaloTopoTowerBuilderToolBase::setTowerSeg(const CaloTowerSeg& theTowerSeg)
{
  ATH_MSG_DEBUG(" in CaloTopoTowerBuilderToolBase::setTowerSeg ");
  m_theTowerSeg = theTowerSeg;
  ATH_MSG_DEBUG("   neta,nphi,etamin,etamax " << theTowerSeg.neta() << " " << theTowerSeg.nphi() 
		<< " " << theTowerSeg.etamin() << " " << theTowerSeg.etamax());
}

StatusCode CaloTopoTowerBuilderToolBase::LoadCalibration(IOVSVC_CALLBACK_ARGS)
{
  /* MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << " in CaloTopoTowerBuilderToolBase::LoadCalibration " << endreq;
  m_cacheValid=false;*/
  return StatusCode::SUCCESS;
}
