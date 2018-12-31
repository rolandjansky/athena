/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/FTauDecorator.h"

// tools include(s) 

//_____________________________________________________________________________
FTauDecorator::FTauDecorator(const std::string& name) 
  : TauRecToolBase(name)
{
}

//_____________________________________________________________________________
FTauDecorator::~FTauDecorator()
{
}

//_____________________________________________________________________________
StatusCode FTauDecorator::execute(xAOD::TauJet& xTau) {
  

  // Alternative Decorate Tau width
  
  //static SG::AuxElement::Accessor<float> acc_jetwidth("seedJetWidth");
  //acc_jetwidth(xTau) =  xTau.jetLink().isValid() ? xTau.jet()->auxdataConst<float>( "Width" ) : -1;
  
  //Decoration of jet seed variables
  float width = -1.;
  float jvt = -1.;
  if (xTau.jetLink().isValid()) {
    width =  xTau.jet()->auxdataConst<float>( "Width" );
    jvt   =  xTau.jet()->auxdataConst<float>( "Jvt" );
  }  
  xTau.auxdata< float >( "seedJetWidth" ) = width;
  xTau.auxdata< float >( "seedJetJvt" )   = jvt;  
  
  return StatusCode::SUCCESS;

}
