/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/IBLParameterSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/PathResolver.h"

//Includes related to determining presence of IBL
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

 
/**
 ** Constructor(s)
 **/
IBLParameterSvc::IBLParameterSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_disablePixMapCondDB(false),
    m_disableSpecialPixels(false),
    m_disableCalibCondDB(false),
    m_disableAlignable(false),
    m_disableAllClusterSplitting(false),
    m_disableClusterMakerOfflineCalib(false),
    m_disableDCS(true)
{
	declareProperty("GeoDbTagSvc",m_geoDbTagSvc);
	declareProperty("RDBAccessSvc",m_rdbAccessSvc);
 	declareProperty("DisablePixMapCondDB",m_disablePixMapCondDB);
 	declareProperty("DisableSpecialPixels",m_disableSpecialPixels);
 	declareProperty("DisableCalibCondDB",m_disableCalibCondDB);
 	declareProperty("DisableAlignable",m_disableAlignable);
 	declareProperty("DisableAllClusterSplitting",m_disableAllClusterSplitting);
 	declareProperty("DisableClusterMakerOfflineCalib",m_disableClusterMakerOfflineCalib);
 	declareProperty("DisableDCS",m_disableDCS);
}
 
IBLParameterSvc::~IBLParameterSvc()
{
}

inline StatusCode IBLParameterSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if (riid == IBLParameterSvc::interfaceID()){
    *ppvIf = dynamic_cast<IBLParameterSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return base_class::queryInterface( riid, ppvIf );

}
 
/**
 ** Initialize Service
 **/
StatusCode IBLParameterSvc::initialize()
{
  //MsgStream log(msgSvc(), name());
 
  StatusCode result = AthService::initialize();
  if (result.isFailure())
  {
   ATH_MSG_FATAL ( "Unable to initialize the service!" );
   return result;
  }
  // read Charge Collection Probability Maps
  //
  // get the PixelDigitization random stream
  //
  
  if (!setIblParameters().isSuccess()) return StatusCode::FAILURE; 

  ATH_MSG_DEBUG ( "initialized service!" );
  return result;
 
}  

//Determine if IBL is present and set appropriate parameters
StatusCode IBLParameterSvc::setIblParameters() {
  if (m_geoDbTagSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  } 
  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "Pixel");

  if (m_rdbAccessSvc.retrieve().isFailure()) {
     msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
     return (StatusCode::FAILURE); 
  } 
  IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("PixelSwitches", versionKey.tag(), versionKey.node());
  const IRDBRecord    *switchTable   = (*switchSet)[0];
  std::string versionName("");
  if (!switchTable->isFieldNull("VERSIONNAME")) versionName=switchTable->getString("VERSIONNAME");
  m_DBMpresent=false;
  if (!switchTable->isFieldNull("BUILDDBM")) m_DBMpresent=switchTable->getInt("BUILDDBM");
  if (versionName=="IBL") {
	m_IBLpresent = true;
	ATH_MSG_INFO("IBL geometry");
  }
  else {
	m_IBLpresent = false;
	ATH_MSG_VERBOSE("Default geometry");
  }
  m_LayerFEsPerHalfModule_3d = 0;
  m_LayerFEsPerHalfModule.clear();
  if (m_IBLpresent) {
	IRDBRecordset_ptr PixelReadout = m_rdbAccessSvc->getRecordsetPtr("PixelReadout", versionKey.tag(), versionKey.node());
	IRDBRecordset_ptr PixelStave = m_rdbAccessSvc->getRecordsetPtr("PixelStave", versionKey.tag(), versionKey.node());
	const IRDBRecord *IBLreadout   = (*PixelReadout)[1];
	if (!IBLreadout->isFieldNull("COLSPERCHIP")) m_LayerColumnsPerFE=IBLreadout->getInt("COLSPERCHIP");
	if (!IBLreadout->isFieldNull("ROWSPERCHIP")) m_LayerRowsPerFE=IBLreadout->getInt("ROWSPERCHIP");
	if (!IBLreadout->isFieldNull("NCHIPSETA")) m_LayerFEsPerHalfModule_planar=IBLreadout->getInt("NCHIPSETA");
	if ((*PixelReadout).size()>2) {
		const IRDBRecord *IBL3Dreadout   = (*PixelReadout)[2];
		if (!IBL3Dreadout->isFieldNull("NCHIPSETA")) m_LayerFEsPerHalfModule_3d=IBL3Dreadout->getInt("NCHIPSETA");
	}
	const IRDBRecord *IBLstave   = (*PixelStave)[1];
	if (!IBLstave->isFieldNull("LAYOUT")) m_layout=IBLstave->getInt("LAYOUT");
	if (m_layout==4) for (int i = 0; i < 16; i++) m_LayerFEsPerHalfModule.push_back(m_LayerFEsPerHalfModule_planar);
	if (m_layout==5) {
		for (int i =0; i < 4; i++) m_LayerFEsPerHalfModule.push_back(m_LayerFEsPerHalfModule_3d);
		for (int i =0; i < 12; i++) m_LayerFEsPerHalfModule.push_back(m_LayerFEsPerHalfModule_planar);
		for (int i =0; i < 4; i++) m_LayerFEsPerHalfModule.push_back(m_LayerFEsPerHalfModule_3d);
	}
  }
  return StatusCode::SUCCESS;
} 

StatusCode IBLParameterSvc::finalize()
{
        return StatusCode::SUCCESS;
}

