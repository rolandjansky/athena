/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Algorithm.h"

#include "DumpEventDataToJsonAlg.h"

#include <string>
#include <fstream>

DumpEventDataToJsonAlg::DumpEventDataToJsonAlg(const std::string& name, 
				       ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator)
{
  declareProperty("OutputLocation", m_outputJSON_Name  );
}

DumpEventDataToJsonAlg::~DumpEventDataToJsonAlg()
{ 
}

StatusCode DumpEventDataToJsonAlg::initialize()
{
  ATH_CHECK( m_eventInfoKey.initialize());
  ATH_CHECK( m_trackParticleKeys.initialize() );
  ATH_CHECK( m_jetKeys.initialize() );
  if (m_extrapolateTracks) ATH_CHECK( m_extrapolator.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode DumpEventDataToJsonAlg::execute ()
{
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (!eventInfo.isValid() ){
    ATH_MSG_WARNING("Did not find xAOD::EventInfo at "<<m_eventInfoKey);
    return StatusCode::SUCCESS;
  }
    
  ATH_MSG_VERBOSE("Run num :"<<eventInfo->runNumber()<<" Event num: "<<eventInfo->eventNumber());

  nlohmann::json j;
  j["Event num"] = eventInfo->eventNumber();
  j["Run num"]   = eventInfo->runNumber();

  ATH_CHECK(getAndFillArrayOfContainers(j, m_jetKeys, "Jets"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_trackParticleKeys, "Tracks"));

  m_eventData.push_back(j);

  return StatusCode::SUCCESS;
}

template< class TYPE >
StatusCode DumpEventDataToJsonAlg::getAndFillArrayOfContainers(nlohmann::json& event, 
                                                              const SG::ReadHandleKeyArray< TYPE >& keys, 
                                                              const std::string jsonType ){
  for (SG::ReadHandle<TYPE> handle : keys.makeHandles() ){
    ATH_MSG_VERBOSE("Trying to load "<<handle.key());
    ATH_CHECK( handle.isValid() );  
    for (auto object : *handle ){
      event[jsonType][handle.key()].push_back( getData(*object) );
    }
  }
  return StatusCode::SUCCESS;
}

// Specialisation for Jets
template <> 
nlohmann::json DumpEventDataToJsonAlg::getData( const xAOD::Jet& jet){
  nlohmann::json data;
  data["phi"]    = jet.phi();
  data["eta"]    = jet.eta();
  data["energy"] = jet.e();
  return data;
}

// Specialisation for Tracks
template <> 
nlohmann::json DumpEventDataToJsonAlg::getData( const xAOD::TrackParticle& tp){
  nlohmann::json data;
  data["chi2"]    = tp.chiSquared();
  data["dof"]    = tp.numberDoF();
  data["dparams"] = { tp.d0(), tp.z0(), tp.phi0(), tp.theta(), tp.qOverP() };
  
  if (!m_extrapolateTracks){
    data["pos"] = { { tp.perigeeParameters ().position().x(), tp.perigeeParameters ().position().y(), tp.perigeeParameters ().position().z() } };
    for (unsigned int i=0; i<tp.numberOfParameters() ; ++i ) {
      data["pos"].push_back( {tp.parameterX (i), tp.parameterY (i), tp.parameterZ (i)} );
    }
  } else {
    
    std::vector<Amg::Vector3D> positions; 
    const Trk::Perigee& peri = tp.perigeeParameters ();
    positions.push_back(Amg::Vector3D(peri.position().x(), peri.position().y(), peri.position().z() ) );

    
    Trk::CurvilinearParameters startParameters(peri.position(),peri.momentum(),peri.charge());
    Trk::ExtrapolationCell<Trk::TrackParameters> ecc(startParameters);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectPassive);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectSensitive);
    Trk::ExtrapolationCode eCode = m_extrapolator->extrapolate(ecc);
    if (eCode.isSuccess()){
      // loop over the collected information
      for (auto& es : ecc.extrapolationSteps){
      
        // continue if we have parameters
        const Trk::TrackParameters* parameters = es.parameters;
        if (parameters){
          Amg::Vector3D pos = parameters->position();
          positions.push_back(pos);        
          delete parameters;
        }
      }
      positions.push_back( ecc.endParameters->position() );
     
      // Now add the positions to the output
      for (auto pos : positions) {
        data["pos"].push_back( {pos.x(), pos.y(), pos.z()} );
      } 
    } else {
      ATH_MSG_WARNING("Failure in extrapolation for Track with start parameters "<<startParameters);
    }
  }
  return data;
}

StatusCode DumpEventDataToJsonAlg::finalize()
{
  std::ofstream outputFile(m_outputJSON_Name);
  if (!outputFile.is_open()) {
    ATH_MSG_WARNING("Unable to open "<<m_outputJSON_Name<<" for writing.");
    return StatusCode::FAILURE;
  }
  outputFile<<m_eventData;  
  return StatusCode::SUCCESS;
}



