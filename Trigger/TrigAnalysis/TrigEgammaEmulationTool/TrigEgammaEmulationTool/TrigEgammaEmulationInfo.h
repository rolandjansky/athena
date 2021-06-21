

#ifndef TrigEgammaEmulationInfo_h
#define TrigEgammaEmulationInfo_h

#include <string>
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "RingerSelectorTools/AsgRingerSelectorTool.h"


namespace Trig{


// Trigger Information struct
typedef struct _trigdata
{
    const TrigRoiDescriptor *roi;
    const xAOD::EmTauRoI *l1;
    const xAOD::TrigEMCluster *emCluster;
    const xAOD::TrigRingerRings *rings;
    const xAOD::TrigPhoton* trigPhoton;
    
    std::vector<const xAOD::CaloCluster*> clusters;
    std::vector<const xAOD::TrigElectron*> trigElectrons;
    std::vector<const xAOD::Electron*> electrons;
    std::vector<const xAOD::Photon*> photons;

    float avgmu;


    // selector tools
    ToolHandleArray<IAsgElectronLikelihoodTool> electronLHTools;
    ToolHandleArray<IAsgPhotonIsEMSelector> photonIsEMTools;
    ToolHandleArray<Ringer::IAsgRingerSelectorTool> ringerTools;

    bool isValid;

     
} TrigData;

}

#endif