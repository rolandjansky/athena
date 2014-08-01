/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @author apolici
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MissingETEvent/MissingET.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewMETContainerTool.h"

static const InterfaceID IID_IMboyViewMETContainerTool("MboyViewMETContainerTool", 1, 0);

const InterfaceID& MboyViewMETContainerTool::interfaceID()
{
  return IID_IMboyViewMETContainerTool;
}

MboyViewMETContainerTool::MboyViewMETContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewMETContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewMETContainerTool::~MboyViewMETContainerTool(){}

// Initialize
StatusCode MboyViewMETContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewMETContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewMETContainerTool::ViewMETContainer(
               std::string METContainerLocation,
               std::ofstream* pOut,
               float& EtMin
){

//Access
  if (!(evtStore()->contains<MissingET>(METContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << METContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const MissingET* et_miss = 0;
  sc = evtStore()->retrieve(et_miss, METContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "et_miss not found at " << METContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  double met_et  = 0.;
  double met_phi = 0.;

  met_et  = et_miss->et();
  met_phi = (et_miss->phi())*57.2957;
  if(met_phi > 0.){
    met_phi = met_phi;
  }
  if(met_phi < 0.){
    met_phi = met_phi + 360;
  }

  // Write Out
  if (met_et>=EtMin) {
    *pOut << "MET" << " "
          << METContainerLocation << " "
          << std::setw(12)<<std::setprecision(4) <<met_phi  << " "
          << std::setw(12)<<std::setprecision(4) <<met_et   << " "
          << std::endl;
  } 
 
  return StatusCode::SUCCESS;
  
}

