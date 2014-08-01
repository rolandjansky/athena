/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @author apolici
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewJetContainerTool.h"

static const InterfaceID IID_IMboyViewJetContainerTool("MboyViewJetContainerTool", 1, 0);

const InterfaceID& MboyViewJetContainerTool::interfaceID()
{
  return IID_IMboyViewJetContainerTool;
}

MboyViewJetContainerTool::MboyViewJetContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewJetContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewJetContainerTool::~MboyViewJetContainerTool(){}

// Initialize
StatusCode MboyViewJetContainerTool::initialize(){

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

StatusCode MboyViewJetContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewJetContainerTool::ViewJetContainer(
               std::string JetContainerLocation,
               std::ofstream* pOut,
               float& EtMin,
               float& EtaMin
){

//Access
  if (!(evtStore()->contains<JetCollection>(JetContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << JetContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const JetCollection* jetColl = 0;
  sc = evtStore()->retrieve(jetColl, JetContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "jetColl not found at " << JetContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  //extract jet cone information from container name
  //temporary fix
  int drcone = 0;
  for (size_t i = 0; i < JetContainerLocation.length(); ++i) {
    if (isdigit(JetContainerLocation[i])) {
      drcone = JetContainerLocation[i] - '0';
    }
  }
  double DR = (double(drcone)/10.)*180./3.1415;


  // Loop over jets in JetCollection
  double jet_et  = 0.;
  double jet_eta = 0.;
  double jet_phi = 0.;
  JetCollection::const_iterator collItr  = jetColl->begin();
  JetCollection::const_iterator collItrE = jetColl->end();
  for (; collItr != collItrE; ++collItr) {
    const Jet * jet = (*collItr);
    jet_et  = jet->et();
    jet_eta = jet->eta();
    jet_phi = (jet->phi())*57.2957;
    if(jet_phi > 0.){
      jet_phi = jet_phi;
    }
    if(jet_phi < 0.){
      jet_phi = jet_phi + 360;
    }

    // Write Out
    if (jet_et>=EtMin && fabs(jet_eta)<=EtaMin) {
      *pOut << "JET" << " "
            << JetContainerLocation << " "
            << std::setw(12)<<std::setprecision(4) << jet_eta        << " "
            << std::setw(12)<<std::setprecision(4) << jet_phi        << " "
            << std::setw(12)<<std::setprecision(4) << DR             << " " 
            << std::setw(12)<<std::setprecision(4) << jet_et/1000.   << " " //GeV
            << std::endl;
    } 
  }
 
  return StatusCode::SUCCESS;
  
}

