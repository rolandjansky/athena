/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronChargeCorrection includes
#include "EleChargeAlg.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
// #include "xAODTruth/TruthEventContainer.h"
// #include "xAODTruth/TruthVertex.h"
// #include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "GaudiKernel/ITHistSvc.h"
#include "PATInterfaces/ISystematicsTool.h"

// #include "../ElectronChargeEfficiencyCorrectionTool/IElectronChargeEfficiencyCorrectionTool.h"
#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"



EleChargeAlg::EleChargeAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_syst(),
  m_eccTool("CP::ElectronChargeEfficiencyCorrectionTool/ElectronChargeEfficiencyCorrectionTool", this),
  m_eleContName("Electrons")
{
  declareProperty("ElectronChargeEfficiencyCorrectionTool",  m_eccTool, "The private ElectronChargeEfficiencyCorrectionTool" );
  declareProperty("ElectronContainerName",         m_eleContName, "The name of the input electron container");
}


EleChargeAlg::~EleChargeAlg() {}


StatusCode EleChargeAlg::initialize() {

  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( m_eccTool.retrieve() );
  m_syst = m_eccTool->affectingSystematics();
  for( const auto& variation : m_syst ) {
    ATH_MSG_INFO(" Affecting systematics: " << variation.name());
  }
  return StatusCode::SUCCESS;
}

StatusCode EleChargeAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  ATH_CHECK( m_eccTool.release() );

  return StatusCode::SUCCESS;
}

StatusCode EleChargeAlg::execute() {

  //----------------------------
  // Event information
  //---------------------------

  const xAOD::EventInfo* eventInfo = 0; //NOTE: Everything that comes from the storegate direct from the input files is const!

  // ask the event store to retrieve the xAOD EventInfo container

  CHECK( evtStore()->retrieve( eventInfo) );
  // if there is only one container of that type in the xAOD (as with the EventInfo container), you do not need to pass
  // the key name, the default will be taken as the only key name in the xAOD

  // check if data or MC
  if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
    ATH_MSG_DEBUG( "DATA. Will stop processing this algorithm for the current event.");
    return StatusCode::SUCCESS; // stop this algorithms execute() for this event, here only interested in MC
  }


  //---------
  // electrons
  //---------

  const xAOD::ElectronContainer* electrons = 0;
  CHECK( evtStore()->retrieve( electrons, m_eleContName.value() ) );

  ATH_MSG_VERBOSE ("Executing " << name() << "... in event with: " << electrons->size() << " electrons");

  // Loop over all electrons in this container
  for ( const auto* electron : *electrons ) {

    if ( electron->pt() < 25000 ) continue;
    if ( std::abs(electron->eta()) > 2.5 ) continue;

    ATH_MSG_DEBUG("-------------------------------------------------------------------" );
    ATH_MSG_DEBUG(" ...  electron with pt = " << electron->pt() << " , eta: " << fabs(electron->eta()) );

    double sf=-999;
    //float rate=-999;
    CP::SystematicSet syst_set;

    CP::SystematicCode tmpSysResult = m_eccTool->applySystematicVariation(syst_set);
    ATH_MSG_DEBUG("applySystematicVariation returned CP::SystematicCode = " << tmpSysResult);
    if ( tmpSysResult != CP::SystematicCode::Ok ) {
      ATH_MSG_ERROR("ElectronChargeEfficiencyCorrectionTool did NOT report a CP::SystematicCode::Ok when calling applySystematicVariation");
      return StatusCode::FAILURE;
    }

    CP::CorrectionCode tmpResult = m_eccTool->getEfficiencyScaleFactor(*electron,sf);
    ATH_MSG_DEBUG("Nominal value SF = " << sf << " and CP::CorrectionCode = " << tmpResult);
    if ( tmpResult == CP::CorrectionCode::Error ) {
      ATH_MSG_ERROR("ElectronChargeEfficiencyCorrectionTool reported a CP::CorrectionCode::Error");
      return StatusCode::FAILURE;
    }

    for( const auto& variation : m_syst ) {
      double tmpSF=0;
      ///float tmprate=0;
      CP::SystematicSet syst_set1;
      syst_set1.insert( variation );

      tmpSysResult = m_eccTool->applySystematicVariation(syst_set1);
      ATH_MSG_DEBUG("applySystematicVariation (second time) returned CP::SystematicCode = " << tmpSysResult);
      if ( tmpSysResult != CP::SystematicCode::Ok ) {
        ATH_MSG_ERROR("ElectronChargeEfficiencyCorrectionTool did NOT report a CP::SystematicCode::Ok when calling applySystematicVariation");
        return StatusCode::FAILURE;
      }
      tmpResult = m_eccTool->getEfficiencyScaleFactor(*electron, tmpSF);
      ATH_MSG_DEBUG("getEfficiencyScaleFactor returned CP::CorrectionCode = " << tmpResult);
      if ( tmpResult == CP::CorrectionCode::Error ) {
        ATH_MSG_ERROR("ElectronChargeEfficiencyCorrectionTool reported a CP::CorrectionCode::Error when calling getEfficiencyScaleFactor");
        return StatusCode::FAILURE;
      }
      //   //m_eccTool->getDataEfficiency(*electron, tmprate);
      ATH_MSG_DEBUG("   Systematic "<< variation.name() << ":" << " SF-> " << tmpSF);
      //   //ATH_MSG_DEBUG("Systematic "<< variation.name() << ":" << " data rate-> " << tmprate);
    }


  } // end for loop over electrons


  return StatusCode::SUCCESS;

} //-------end of execute
