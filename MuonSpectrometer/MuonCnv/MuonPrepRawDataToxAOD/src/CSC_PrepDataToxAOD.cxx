/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_PrepDataToxAOD.cxx
//   Implementation file for class CSC_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "CSC_PrepDataToxAOD.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// Constructor with parameters:
CSC_PrepDataToxAOD::CSC_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{  
    
}

// Initialize method:
StatusCode CSC_PrepDataToxAOD::initialize()
{
  m_cscPrds="CSC_Clusters";
  m_trackMeasVal="CSC_Clusters_TrackMeasVal";
  ATH_CHECK(m_cscPrds.initialize());
  ATH_CHECK(m_trackMeasVal.initialize());
  ATH_CHECK(m_idHelper.retrieve());
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode CSC_PrepDataToxAOD::execute() 
{
  SG::ReadHandle<Muon::CscPrepDataContainer> cscPrds(m_cscPrds);
  if(!cscPrds.isValid()){
    ATH_MSG_WARNING(m_cscPrds.key()<<" not valid");
    return StatusCode::FAILURE;
  }
  if(!cscPrds.isPresent()){
    ATH_MSG_DEBUG("no "<<m_cscPrds.key()<<" collection present");
    return StatusCode::SUCCESS;
  }

  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> xaod(m_trackMeasVal);
  ATH_CHECK(xaod.record(std::make_unique<xAOD::TrackMeasurementValidationContainer>(),std::make_unique<xAOD::TrackMeasurementValidationAuxContainer>()));
  
  Muon::CscPrepDataContainer::const_iterator it = cscPrds->begin();
  Muon::CscPrepDataContainer::const_iterator it_end = cscPrds->end();
  for( ; it!=it_end; ++it ) {
    // skip empty collections
    if( (*it)->empty() ) continue;
    
    // loop over collection and convert to xAOD
    for( const auto& prd : **it ){
      
      // create and add xAOD object
      xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
      xaod->push_back(xprd);
      
      xprd->setIdentifier(prd->identify().get_compact());
      xprd->setLocalPosition(prd->localPosition().x(),prd->localPosition().y());
      xprd->setLocalPositionError(Amg::error(prd->localCovariance(),Trk::locR), 0.,0.);
      Amg::Vector3D gpos = prd->globalPosition();
      xprd->setGlobalPosition(gpos.x(),gpos.y(),gpos.z());

      xprd->auxdata<float>("time") = static_cast<float>(prd->time());
      xprd->auxdata<float>("charge") = prd->charge();
      xprd->auxdata<int>("status") = prd->status();
      xprd->auxdata<int>("timeStatus") = prd->timeStatus();
    }
  }
  ATH_MSG_DEBUG( " recorded CSC_PrepData obejcts: size " << xaod->size() );

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode CSC_PrepDataToxAOD::finalize()
{
  return StatusCode::SUCCESS;
}
