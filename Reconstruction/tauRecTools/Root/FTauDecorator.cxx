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
  std::vector<float> track500;
  std::vector<float> track1000;
  if (xTau.jetLink().isValid()) {
    width =  xTau.jet()->auxdataConst<float>( "Width" );
    jvt   =  xTau.jet()->auxdataConst<float>( "Jvt" );
    track500 = xTau.jet()->auxdataConst<std::vector<float>>( "TrackWidthPt500" );
    track1000 = xTau.jet()->auxdataConst<std::vector<float>>( "TrackWidthPt1000" );
  }  
  xTau.auxdata< float >( "seedJetWidth" ) = width;
  xTau.auxdata< float >( "seedJetJvt" )   = jvt;  
  xTau.auxdata< std::vector<float> >( "seedTrackWidthPt500" )   = track500;
  xTau.auxdata< std::vector<float> >( "seedTrackWidthPt1000" )   = track1000;
  
  return StatusCode::SUCCESS;

}
