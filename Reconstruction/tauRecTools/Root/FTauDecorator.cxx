/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/FTauDecorator.h"

// tools include(s) 
//#include "TauAnalysisTools/HelperFunctions.h"

//_____________________________________________________________________________
FTauDecorator::FTauDecorator(const std::string& name) 
  : TauRecToolBase(name)
  //, m_xEventInfo(0)
  //, m_xVertexContainer(0) 
  //, m_mu(0)
  //, m_nVtxPU(0)
{
}

//_____________________________________________________________________________
FTauDecorator::~FTauDecorator()
{
}

//_____________________________________________________________________________
StatusCode FTauDecorator::eventInitialize()
{

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode FTauDecorator::execute(xAOD::TauJet& xTau) {
  

  // Decorate Tau width
  //LF Decoration in 3 different ways for development purpose only
  
  static SG::AuxElement::Accessor<float> acc_jetwidth("seedJetWidth");
  acc_jetwidth(xTau) =  xTau.jetLink().isValid() ? xTau.jet()->auxdataConst<float>( "Width" ) : -1;
  
  // Alternativ Decoration of jet seed variables
  /*float width = -1.;
  if (xTau.jetLink().isValid()) {
    width =  xTau.jet()->auxdataConst<float>( "Width" );
  }  
  xTau.auxdata< float >( "seedJetWidth2" ) = width;
  */
  
  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode FTauDecorator::eventFinalize()
{
  return StatusCode::SUCCESS;
}
