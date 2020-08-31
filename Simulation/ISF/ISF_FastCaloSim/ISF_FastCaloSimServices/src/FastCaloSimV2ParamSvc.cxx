/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "FastCaloSimV2ParamSvc.h"


// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

//!
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/TileID.h"
//!

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include <fstream>

/** Constructor **/
ISF::FastCaloSimV2ParamSvc::FastCaloSimV2ParamSvc(const std::string& name, ISvcLocator* svc)
  : base_class(name, svc)
  , m_detStore( "StoreGateSvc/DetectorStore", name )

{
  declareProperty( "DetStore",
                   m_detStore = StoreGateSvc_t ("StoreGateSvc/DetectorStore", name),
                   "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
                   "retrieve data during the course of the job" );
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename);
  declareProperty("ParamsInputObject"              ,       m_paramsObject);
  declareProperty("PrintParametrization"           ,       m_printParametrization);
  declareProperty("CompressMemory"                 ,       m_CompressMemory);
}

/** framework methods */
StatusCode ISF::FastCaloSimV2ParamSvc::initialize()
{
  const CaloDetDescrManager *calo_dd_man = CaloDetDescrManager::instance();
  const FCALDetectorManager *fcalManager{};
  ATH_CHECK(detStore()->retrieve(fcalManager));

  m_caloGeo = std::make_unique<CaloGeometryFromCaloDDM>();
  m_caloGeo->LoadGeometryFromCaloDDM(calo_dd_man);
  if (!m_caloGeo->LoadFCalChannelMapFromFCalDDM(fcalManager)) {
    ATH_MSG_FATAL("Found inconsistency between FCal_Channel map and GEO file. Please, check if they are configured properly.");
    return StatusCode::FAILURE;
  }

  const std::string fileName = m_paramsFilename; //?
  std::string inputFile=PathResolverFindCalibFile(fileName);
  std::unique_ptr<TFile> paramsFile(TFile::Open( inputFile.c_str(), "READ" ));
  if (paramsFile == nullptr) {
    ATH_MSG_ERROR("file = "<<m_paramsFilename<< " not found");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Opened parametrization file = "<<m_paramsFilename);
  paramsFile->ls();
  m_param=static_cast<TFCSParametrizationBase*>(paramsFile->Get(m_paramsObject.c_str()));
  if (!m_param) {
    ATH_MSG_WARNING("file = "<<m_paramsFilename<< ", object "<< m_paramsObject<<" not found");
    return StatusCode::FAILURE;
  }

  paramsFile->Close();

  if(m_CompressMemory) m_param->RemoveDuplicates();
  m_param->set_geometry(m_caloGeo.get()); /// does not take ownership
  m_param->setLevel(msg().level());
  if (m_printParametrization) {
    m_param->Print("short");
  }
  if(m_CompressMemory) m_param->RemoveNameTitle();
  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode ISF::FastCaloSimV2ParamSvc::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  FCSReturnCode status = m_param->simulate(simulstate, truth, extrapol);
  if (status != FCSSuccess) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
