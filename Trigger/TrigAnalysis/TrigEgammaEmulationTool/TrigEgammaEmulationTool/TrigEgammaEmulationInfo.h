
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
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "RingerSelectorTools/AsgRingerSelectorTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronSelectorTool.h"




namespace Trig{


struct TrigData{

  TrigData( std::string type): signature(type) {}
  std::string signature;
  const TrigRoiDescriptor *roi;
  const xAOD::EmTauRoI *l1;
  const xAOD::TrigEMCluster *emCluster;
  const xAOD::TrigRingerRings *rings;
  const xAOD::TrigPhoton* trig_photon;
  std::vector<const xAOD::CaloCluster*> clusters;
  std::vector<const xAOD::TrigElectron*> trig_electrons;
  std::vector<const xAOD::Electron*> electrons;
  std::vector<const xAOD::Photon*> photons;

  
  /* precision electron selectors */    
  ToolHandleArray<IAsgElectronLikelihoodTool>   egammaElectronDNNTools;
  ToolHandleArray<IAsgElectronIsEMSelector>     egammaElectronCBTools;
  ToolHandleArray<IAsgElectronLikelihoodTool>   egammaElectronLHTools;
  
  /* precision photon selectors */
  ToolHandleArray< IAsgPhotonIsEMSelector >     egammaPhotonCBTools;

  /* ringer selectors */
  ToolHandleArray<Ringer::IAsgRingerSelectorTool> ringerTools;


  bool isValid(){
    return (roi && l1 && emCluster && rings);
    if (signature == "photon"){
      return (trig_photon && !photons.empty());
    }else if (signature == "electron"){
      return (!trig_electrons.empty() && !electrons.empty());
    }else{
      return false;
    }
  }
  void clear(){
    electrons.clear(); trig_electrons.clear(); clusters.clear(); photons.clear();
    roi = nullptr;
    l1 = nullptr;
    emCluster = nullptr;
    rings = nullptr;
    trig_photon = nullptr;
  }
  
};

}


#endif