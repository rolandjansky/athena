/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>

#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "./L1CaloDecoder.h"

L1CaloDecoder::L1CaloDecoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_trigRoIs("OutputRoIs"),
    m_recEMTauRoIs("OutputRecEMTauRoIs"),
    m_decisions("OutputDecisions"),
    m_decisionsAux("OutputDecisionsAux."),
    //    m_view("View"),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_RoIBResult("RoIBResult")
{
  // outputs
  declareProperty("OutputRoIs", m_trigRoIs, "Name of the RoIs object produced by the unpacker");
  declareProperty("OutputRecEMTauRoIs", m_recEMTauRoIs, "Name of the RoIs object produced by the unpacker");
  declareProperty("OutputDecisions", m_decisions, "Name of the decisions object (wiht links to specific RoIs)");
  declareProperty("OutputDecisionsAux", m_decisionsAux, "Name of the decisions object (wiht links to specific RoIs) - aux");

  // input
  declareProperty("RoIBResult", m_RoIBResult, "Name of the RoIB result");

  // views
  //  declareProperty("View", m_view, "Name of the generated view" );

  // services
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

StatusCode L1CaloDecoder::initialize() {
  CHECK(m_configSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode L1CaloDecoder::start() {
  using namespace TrigConf;
  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  for( auto caloType : std::vector<L1DataDef::TriggerType>{ L1DataDef::EM/*, L1DataDef::TAU*/} ) {    
    for (TriggerThreshold * th : thresholdConfig->getThresholdVector( caloType ) ) {
      if ( th != nullptr ) {
        ATH_MSG_DEBUG("Found threshold in the configuration: " << th->name() << " of ID: " << HLTUtils::string2hash(th->name(), "TE"));
        m_emtauThresholds.push_back(th);
      }
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode L1CaloDecoder::execute() {
  
  if ( not m_RoIBResult.isValid() ) {
    ATH_MSG_WARNING("No L1 result");
    return StatusCode::RECOVERABLE;
  }
  

  IProxyDict * view = 0;

  // redirect handles to that view
  CHECK( m_trigRoIs.setProxyDict(view) );
  CHECK( m_recEMTauRoIs.setProxyDict(view) );
  CHECK( m_decisions.setProxyDict(view) );
  CHECK( m_decisionsAux.setProxyDict(view) );

  // define output
  m_trigRoIs = CxxUtils::make_unique< TrigRoiDescriptorCollection >();
  m_recEMTauRoIs = CxxUtils::make_unique< DataVector<LVL1::RecEmTauRoI> >();

  m_decisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_decisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_decisions->setStore(m_decisionsAux.ptr());
  
  for ( const ROIB::EMTauResult& fragment : m_RoIBResult->eMTauResult() ) {
    for ( const ROIB::EMTauRoI& emtau : fragment.roIVec()  ) {
      uint32_t roIWord = emtau.roIWord();

      auto recRoI = new LVL1::RecEmTauRoI( roIWord, &m_emtauThresholds );
      m_recEMTauRoIs->push_back(recRoI);
      
      auto trigRoI = new TrigRoiDescriptor(recRoI->roiWord(), 0u ,0u,
					   recRoI->eta(),recRoI->eta()-0.1,recRoI->eta()+0.1,
					   recRoI->phi(),recRoI->phi()-0.1,recRoI->phi()+0.1);
      m_trigRoIs->push_back(trigRoI);
			  
      ATH_MSG_DEBUG("RoI word: 0x" << MSG::hex << std::setw(8) << roIWord << ", threshold pattern " << MSG::dec);
      
      std::vector<TriggerElementID> passedThresholdIDs;
      for ( auto th: m_emtauThresholds ) {
	if ( recRoI->passedThreshold(th->thresholdNumber()) ) {
	  passedThresholdIDs.push_back(TrigConf::HLTUtils::string2hash(th->name(), "TE"));
	}
      }
      
      xAOD::TrigComposite * decision  = new xAOD::TrigComposite();
      m_decisions->push_back(decision);
      decision->setDetail("Passed", passedThresholdIDs);      
      decision->setObjectLink("initialRoI", ElementLink<TrigRoiDescriptorCollection>(m_trigRoIs.name(), m_trigRoIs->size()-1 ));
      decision->setObjectLink("initialRecRoI", ElementLink<DataVector<LVL1::RecEmTauRoI>>(m_recEMTauRoIs.name(), m_recEMTauRoIs->size()-1));      
    }
  }
  for ( auto roi: *m_trigRoIs ) {
    ATH_MSG_DEBUG("RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord());
  }

  return StatusCode::SUCCESS; // what else
}

StatusCode L1CaloDecoder::finalize() {
  return StatusCode::SUCCESS;
}
