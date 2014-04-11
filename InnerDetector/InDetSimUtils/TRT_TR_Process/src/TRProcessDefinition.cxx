/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "globals.hh"
#include "TRT_TR_Process/TRProcessDefinition.h"
#include "FadsPhysics/UserProcessDefinition.h"

#include "G4VProcess.hh"
#include "TRT_TR_Process/TRTTransitionRadiation.h"

#include <iostream>
#include "G4ProcessManager.hh"

static UserPhysicsDefinitionProxy<TRProcessDefinition> plugin;

TRProcessDefinition::TRProcessDefinition(): UserPhysicsDefinition(),
  m_msg("TRProcessDefinition")
{
  m_pXTR = new TRTTransitionRadiation( "XTR" ) ;
}

void TRProcessDefinition::ConstructProcess()
{

  if (msgLevel(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " This is TRProcessDefinition::ConstructProcess()"
		    << endreq;
    msg(MSG::DEBUG) << " The TR process is being initialized" << endreq;
  }

  theParticleIterator->reset();
  while( (*theParticleIterator)() ) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if ( particle->GetPDGCharge() != 0.0 && particle->GetPDGMass() != 0.0 ) {
      m_pXTR->SetVerboseLevel(1);
      pmanager->AddDiscreteProcess( m_pXTR );
    }
  }
}
