/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODCore/ShallowAuxInfo.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "QuickAna/OutputToolXAOD.h"
#include <xAODRootAccess/Init.h>

int main(int, char **argv)
{
  // declare the return type of this function for ANA_CHECK
  ANA_CHECK_SET_TYPE (int);

  // needed for messaging macros
  using namespace ana::msgUserCode;

  // make unchecked status codes abort the job
  ana::enableStatusCodeFailure ();

  // initialize xAOD classes
  ANA_CHECK (xAOD::Init (argv[0]));

  // Try instantiating the tools
  ana::OutputToolXAOD<xAOD::EventInfo, xAOD::ShallowAuxInfo>
    t1("EventInfoOutputTool");
  ana::OutputToolXAOD<xAOD::ElectronContainer, xAOD::ShallowAuxContainer>
    t2("ElectronOutputTool");
  ana::OutputToolXAOD<xAOD::MissingETContainer, xAOD::MissingETAuxContainer>
    t3("MetOutputTool");

  ANA_CHECK( t1.initialize() );
  ANA_CHECK( t2.initialize() );
  ANA_CHECK( t3.initialize() );

  return 0;
}
