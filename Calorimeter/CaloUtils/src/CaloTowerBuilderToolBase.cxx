/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  , m_storeGate("StoreGateSvc", name)
  , m_cacheValid(false)
  , m_caloAlignTool("CaloAlignTool")
{

  // common properties
  declareProperty("CellContainerName", m_cellContainerName);
  declareInterface<ICaloTowerBuilderToolBase>(this);
}

CaloTowerBuilderToolBase::~CaloTowerBuilderToolBase()
{ }

StatusCode CaloTowerBuilderToolBase::initialize() {
  // StoreGate
//  CHECK(service("StoreGateSvc",m_storeGate));
  CHECK(m_storeGate.retrieve());

  bool LArDD = true;

  IProperty* propertyServer(0);
  StatusCode sc = serviceLocator()->service("GeoModelSvc", propertyServer);
  if (sc.isSuccess()) {

    std::vector<std::string> values;
    StringArrayProperty sap("DetectorTools", values);

    sc = propertyServer->getProperty(&sap);
    if (sc.isSuccess()) {
      values = sap.value();
      LArDD = false;
      for (unsigned int i = 0; i < values.size(); ++i) {
        LArDD |= (values[i].compare(0, 3, "LAr") == 0);
      }
    }
  }

  if (LArDD) {
    CHECK(m_caloAlignTool.retrieve());

    // Try to register a callback only if LAr is used in the job
    CHECK(detStore()->regFcn(&IGeoAlignTool::align,
        dynamic_cast<IGeoAlignTool*>(&(*m_caloAlignTool)),
        &ICaloTowerBuilderToolBase::LoadCalibration,
        dynamic_cast<ICaloTowerBuilderToolBase*>(this)));

    ATH_MSG_DEBUG(" registered callback to caloAlignTool ");
  }

  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",this->name());
  CHECK(incSvc.retrieve());

  // start listening to "BeginRun". The incident should be fired
  // AFTER the IOV callbacks and only once.
  const long priority = std::numeric_limits<long>::min(); //Very low priority
  incSvc->addListener(this, "BeginRun", priority, true, true);

  m_cacheValid = false;

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

StatusCode CaloTowerBuilderToolBase::LoadCalibration(IOVSVC_CALLBACK_ARGS){

  ATH_MSG_DEBUG(" in CaloTowerBuilderToolBase::LoadCalibration ");
  m_cacheValid = false;
  return StatusCode::SUCCESS;
}
