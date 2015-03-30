/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPBaseTool_H
#define TrigEgammaNavZeeTPBaseTool_H

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
//#include "TrigNavigation/Navigation.h"

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"


namespace Trig{
            class FeatureContainer;
}
class IegammaMVATool;

class TrigEgammaNavZeeTPBaseTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPBaseTool( const std::string& myname );
  ~TrigEgammaNavZeeTPBaseTool() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();
  template<class T> const T* getFeature(const HLT::TriggerElement* te);
  template<class T> bool ancestorPassed(const HLT::TriggerElement* te);
protected:
  /*! things like LAr-error, Tile-error, etc, should come here */
  bool EventWiseSelection(); 
  /*! at least one chain should pass. e28_tight_iloose? */
  bool MinimalTriggerRequirement ();
  /*! Tag and Probe method called by derived classes */
  StatusCode executeTandP(const std::string trigItem); 
  /*! Tag Electron selection */
  bool isTagElectron(const xAOD::Electron *el);
  /*! Probe selection */
  bool isGoodProbeElectron(const xAOD::Electron *el,const std::string); 
  /*! Creates pairs of probes and TE */
  bool isProbeElectron(const xAOD::Electron *el,
          const std::string trigItem,
          const HLT::TriggerElement*& finalFC);
  /*! Event-wise trigger selection */
  bool passedTrigger(const HLT::TriggerElement* obj); 
  /*! Clears list of probes after each trigger item per event */
  void clearProbeList(); 
  /*! Dual-use tool for MVA calibration */
  ToolHandle<IegammaMVATool>  m_MVACalibTool; 
  /*! vector of offline electron probe and matched TE */
  std::vector<std::pair<const xAOD::Electron*,const HLT::TriggerElement*> > m_probeElectrons;
  /* vector of offline photon probe and matched TE */
  std::vector<std::pair<const xAOD::Photon*,const HLT::TriggerElement*> > m_probePhotons; 
  /*! List of triggers to study */
  std::vector<std::string> m_probeTrigList; 
  /*! To apply MVA calibration -- TBD */
  bool m_applyMVACalib; 
  /*! dR matching between TE and offline probe */
  float m_dR;
  /*! Directory name for each algorithm */
  std::string m_dir;
  // Additional monitoring function   
  float getEta2(const xAOD::Egamma* eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          return fabs(cluster->etaBE(2)); 
      }
      else return -99.;
  }
  float getEt(const xAOD::Electron* eg){
      if(eg && (eg->caloCluster()) && (eg->trackParticle())){
          float eta   = fabs(eg->trackParticle()->eta()); 
          return eg->caloCluster()->e()/cosh(eta);      
      }
      else return -99.;
  }
  float getEtCluster37(const xAOD::Egamma* eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          float eta2   = fabs(cluster->etaBE(2)); 
          return cluster->e()/cosh(eta2);      
      }
      else return -99.;
  }
  float getDEmaxs1(const xAOD::Egamma *eg){
      if(eg){
          float emax2=0.;
          eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
          float emax=0.;
          eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
          float val = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
          return val;
      }
      else return -99.;
  }
  float rTRT  (const xAOD::Electron* eg) {
      if(eg && eg->trackParticle()){ 
          uint8_t trtHits   = 0;
          eg->trackParticleSummaryValue(trtHits,xAOD::numberOfTRTHits);
          uint8_t trtHTHits = 0; 
          eg->trackParticleSummaryValue(trtHTHits,xAOD::numberOfTRTHighThresholdHits);
          if(trtHits!=0) {
              return ( (double)trtHTHits / (double)trtHits ); 
          }
          else return -99.;
      }
      else return -99.;
  }

  float getSigmaD0(const xAOD::Electron *eg){
      const xAOD::TrackParticle* t = eg->trackParticle();   
      float d0sigma=0.;
      if (t)
      {
          float vard0 = t->definingParametersCovMatrix()(0,0);
          if (vard0 > 0) {
              d0sigma=sqrtf(vard0);
          }
          else return -99.;
          return d0sigma;
      }
      else return -99.;
  }

  float getD0sig(const xAOD::Electron *eg){
      const xAOD::TrackParticle* t = eg->trackParticle();   
      float d0sigma=0.;
      if (t)
      {

          float vard0 = t->definingParametersCovMatrix()(0,0);
          if (vard0 > 0) {
              d0sigma=sqrtf(vard0);
          }
          else return -99.;
          return t->d0()/d0sigma;
      }
      else return -99.;
  }

  float getEnergyBE0(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          return cluster->energyBE(0); 
      }
      else return 0;
  }

  float getEnergyBE1(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          return cluster->energyBE(1); 
      }
      else return 0.;
  }

  float getEnergyBE2(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          return cluster->energyBE(2); 
      }
      else return 0.;
  }

  float getEnergyBE3(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          return cluster->energyBE(3); 
      }
      else return 0.;
  }

  float getEaccordion(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
          float ebe1 = cluster->energyBE(1); 
          float ebe2 = cluster->energyBE(2); 
          float ebe3 = cluster->energyBE(3);
          return (ebe1+ebe2+ebe3);
      }
      else return 0.;
  }

  float getE0Eaccordion(const xAOD::Egamma *eg){
      if(eg && (eg->caloCluster())){
          const xAOD::CaloCluster*   cluster  = eg->caloCluster();
          float ebe0 = cluster->energyBE(0); 
          float ebe1 = cluster->energyBE(1); 
          float ebe2 = cluster->energyBE(2); 
          float ebe3 = cluster->energyBE(3);
          float eacc = ebe1+ebe2+ebe3;
          if(eacc==0.) return 0.;
          return (ebe0/eacc);
      }
      else return 0.;
  }
  /*! Macros for plotting */  
#define GETTER(_name_) float getShowerShape_##_name_(const xAOD::Egamma* eg) \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; } 
GETTER(e011)
GETTER(e132)    
GETTER(e237)
GETTER(e277)
GETTER(ethad)       
GETTER(ethad1)    
GETTER(weta1)
GETTER(weta2)
GETTER(f1)
GETTER(e2tsts1) 
GETTER(emins1)
GETTER(emaxs1)
GETTER(wtots1)
GETTER(fracs1)   
GETTER(Reta)  
GETTER(Rphi)    
GETTER(f3)  
GETTER(f3core)
GETTER(Eratio)
GETTER(Rhad)
GETTER(Rhad1)  
GETTER(DeltaE)    
#undef GETTER

// GETTER for Isolation monitoring
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Electron* eg) \
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; } 
GETTER(etcone20)
GETTER(etcone30)
GETTER(etcone40)    
GETTER(topoetcone20)
GETTER(topoetcone30)
GETTER(topoetcone40)    
GETTER(ptcone20)
GETTER(ptcone30)
GETTER(ptcone40)    
#undef GETTER    

// GETTERs for CaloCluster monitoring   
#define GETTER(_name_) float getCluster_##_name_(const xAOD::Egamma* eg) \
{    if(eg && eg->caloCluster()) \
        return eg->caloCluster()->_name_(); \
    else return -99.;}
GETTER(et)
GETTER(phi)
GETTER(eta)   
#undef GETTER

// GETTERs for Track monitoring   
#define GETTER(_name_) float getTrack_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->trackParticle()) \
        return eg->trackParticle()->_name_(); \
    else return -99.;}
GETTER(pt)
GETTER(phi)
GETTER(eta)   
GETTER(d0)  
GETTER(z0)    
#undef GETTER
// GETTERs for Track details monitoring    
#define GETTER(_name_) float getTrackSummary_##_name_(const xAOD::Electron* eg) \
{ uint8_t val_uint8{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
GETTER(numberOfInnermostPixelLayerHits) 
GETTER(numberOfInnermostPixelLayerOutliers) 
GETTER(numberOfPixelHits)
GETTER(numberOfPixelOutliers)
GETTER(numberOfSCTHits)    
GETTER(numberOfSCTOutliers)    
GETTER(numberOfTRTHits)
GETTER(numberOfTRTHighThresholdHits)
GETTER(numberOfTRTHighThresholdOutliers)
GETTER(numberOfTRTOutliers)
GETTER(expectInnermostPixelLayerHit)
GETTER(numberOfPixelDeadSensors)
GETTER(numberOfSCTDeadSensors)
GETTER(numberOfTRTXenonHits)

#undef GETTER

#define GETTER(_name_) float getTrackSummaryFloat_##_name_(const xAOD::Electron* eg) \
{ float val_float{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_float,xAOD::_name_); \
        return val_float; } \
    else return -99; }
GETTER(eProbabilityComb)
GETTER(eProbabilityHT)
GETTER(eProbabilityToT)
GETTER(eProbabilityBrem)
GETTER(pixeldEdx)    

#undef GETTER
// GETTERs for Calo-Track monitoring
#define GETTER(_name_) float getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
{ float val={-99.}; \
    if(eg && eg->trackParticle()){ \
        eg->trackCaloMatchValue(val,xAOD::EgammaParameters::_name_);} \
    return val; }
GETTER(deltaEta0)
GETTER(deltaPhi0)
GETTER(deltaPhiRescaled0)    
GETTER(deltaEta1)
GETTER(deltaPhi1)
GETTER(deltaPhiRescaled1)    
GETTER(deltaEta2)
GETTER(deltaPhi2)
GETTER(deltaPhiRescaled2)    
GETTER(deltaEta3)
GETTER(deltaPhi3)
GETTER(deltaPhiRescaled3) 
#undef GETTER    
private:
  // ToolHandles
  //
  //Offline ++ selectors
  // In python order will matter. Should always be tight, medium, loose
  // Order no longer important since using a map
  ///*! Offline isEM Selectors */
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronPPCutIDTool;
  /*! Offline LH Selectors */
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool; 

  std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */
  //std::map< std::string, std::string > m_PidMap; /*! Map trigger pid to selector pid */ 
  //
  /*! Event Wise offline ElectronContainer Access and end iterator */
  const xAOD::ElectronContainer* m_offElectrons;
  /*! Jet container for probe selection */
  const xAOD::JetContainer* m_jets;
  /* TP selection properties */
 
  /*! Zee lower mass cut */
  float m_ZeeMassMin;
  /*! Zee upper mass cut */
  float m_ZeeMassMax;
  /*! Not needed */
  unsigned int m_isEMoffTag;
  /*! Define the PID for tag electron */
  std::string m_offTagTightness;
  /*! define the Pid of Probe from the trigger name */ 
  std::string m_offProbeTightness;
  /*! define the Pid of Probe from the user */ 
  std::string m_defaultProbeTightness;
  /*! force user defined probe for pid triggers */
  bool m_forceProbePid;
  /*! Select opposite or same-sign pairs -- for background studies */
  bool m_oppositeCharge;
  /*! Minimum tag Et */
  float m_tagMinEt;
  /*! Minimum probe Et */
  float m_probeMinEt;
  /*! Trigger for tag and event wise selection */
  std::string m_tagTrigItem;
  /*! Map of trigger name and L1 item */
  //std::map<std::string,std::string> m_triggerMap;
  
};

/** Main feature of the code 
 * templates the TE to get a feature
 * or find the passing TE
 * **********************/
template<class T>
const T*
TrigEgammaNavZeeTPBaseTool::getFeature(const HLT::TriggerElement* te){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return NULL;
    return ( (m_trigdec->ancestor<T>(te)).cptr() );
}

template<class T>
bool
TrigEgammaNavZeeTPBaseTool::ancestorPassed(const HLT::TriggerElement* te){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return false;
    return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
}
#endif
