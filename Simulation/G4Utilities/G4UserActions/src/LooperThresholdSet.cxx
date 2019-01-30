/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "G4UserActions/LooperThresholdSet.h"
#include <iostream>

#include "G4Version.hh"

#include "G4Electron.hh"
#include "G4ProcessManager.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

//---------------------------------------------------------------------------
LooperThresholdSet::LooperThresholdSet(const Config& config) :
        AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"), "LooperThresholdSet"),
        m_config(config)
{
}

//---------------------------------------------------------------------------
void LooperThresholdSet::BeginOfRunAction(const G4Run*)
{
        ChangeLooperParameters( G4Electron::Electron() );
}

void LooperThresholdSet::ChangeLooperParameters(const G4ParticleDefinition* particleDef)
{
        if( particleDef == nullptr )
                particleDef = G4Electron::Electron();
        auto transportPair = findTransportation( particleDef );
        auto transport = transportPair.first;
        auto coupledTransport = transportPair.second;

        if( transport != nullptr )
        {
                // Change the values of the looping particle parameters of Transportation
                ATH_MSG_INFO("Setting looper values of G4Transportation process...");
                if( m_config.WarningEnergy   >= 0.0 ) {
                        transport->SetThresholdWarningEnergy(  m_config.WarningEnergy );
                        ATH_MSG_INFO("Set ThresholdWarningEnergy to " << m_config.WarningEnergy);
                }
                else
                        ATH_MSG_WARNING("Invalid ThresholdEnergy: " << m_config.WarningEnergy);
                if( m_config.ImportantEnergy >= 0.0 ) {
                        transport->SetThresholdImportantEnergy(  m_config.ImportantEnergy );
                        ATH_MSG_INFO("Set ImportantEnergy to " << m_config.ImportantEnergy);

                }
                else
                        ATH_MSG_WARNING("Invalid ImportantEnergy: " << m_config.ImportantEnergy);
                if( m_config.NumberOfTrials  > 0 ) {
                        transport->SetThresholdTrials( m_config.NumberOfTrials );
                        ATH_MSG_INFO("Set NumberOfTrials to " << m_config.NumberOfTrials);

                }
                else
                        ATH_MSG_WARNING("Invalid NumberOfTrials: " << m_config.NumberOfTrials);
                // Geant4 printout
#if G4VERSION_NUMBER > 1049
                transport->ReportLooperThresholds();
#endif
        }
        else if( coupledTransport != nullptr )
        {
                // Change the values for Coupled Transport
                ATH_MSG_INFO("Setting looper values of G4CoupledTransportation process...");
                if( m_config.WarningEnergy   >= 0.0 ) {
                        coupledTransport->SetThresholdWarningEnergy(  m_config.WarningEnergy );
                        ATH_MSG_INFO("Set ThresholdWarningEnergy to " << m_config.WarningEnergy);
                }
                else
                        ATH_MSG_WARNING("Invalid ThresholdEnergy: " << m_config.WarningEnergy);
                if( m_config.ImportantEnergy >= 0.0 ) {
                        coupledTransport->SetThresholdImportantEnergy(  m_config.ImportantEnergy );
                        ATH_MSG_INFO("Set ImportantEnergy to " << m_config.ImportantEnergy);

                }
                else
                        ATH_MSG_WARNING("Invalid ImportantEnergy: " << m_config.ImportantEnergy);
                if( m_config.NumberOfTrials  > 0 ) {
                        coupledTransport->SetThresholdTrials( m_config.NumberOfTrials );
                        ATH_MSG_INFO("Set NumberOfTrials to " << m_config.NumberOfTrials);

                }
                else
                        ATH_MSG_WARNING("Invalid NumberOfTrials: " << m_config.NumberOfTrials);
                // Geant4 printout
#if G4VERSION_NUMBER > 1049
                transport->ReportLooperThresholds();
#endif
        }
}

std::pair<G4Transportation*, G4CoupledTransportation*>
LooperThresholdSet::findTransportation( const G4ParticleDefinition* particleDef )
{
        const auto *partPM =  particleDef->GetProcessManager();

        G4VProcess* partTransport = partPM->GetProcess("Transportation");
        auto transport = dynamic_cast<G4Transportation*>(partTransport);

        partTransport = partPM->GetProcess("CoupledTransportation");
        auto coupledTransport=
                dynamic_cast<G4CoupledTransportation*>(partTransport);

        return std::make_pair( transport, coupledTransport );
}

} // namespace G4UA
