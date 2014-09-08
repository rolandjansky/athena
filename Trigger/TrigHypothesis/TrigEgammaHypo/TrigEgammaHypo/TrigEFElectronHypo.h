/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFElectronHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * MAINTAINED BY: Ryan M. White, Alessandro Tricoli
 *
 *********************************************************************/
#ifndef TRIGEFEGAMMAHYPO_H
#define TRIGEFEGAMMAHYPO_H


#include <string>
#include <cmath> 

// athena stuff stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AIDA/IHistogram1D.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

//xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"

// Asg SelectorTools
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "VxVertex/RecVertex.h"

/**
 * \class TrigEFElectronHypo
 * \brief TrigEFElectronHypo is a Trigger Hypothesis  Algorithm that retrieves the EF EgammaContainer
 * container created by the EF algorithm TrigEgammaRec
 * It can apply the isem offline cut for electrons: loose, tight or medium
 * or it can apply ET, shower shape selection, TRT hits and E/p cuts to select a good electron at the EF
 * an event will be accepted if all electron selection variables are satisfied
 *
 */


class StoreGateSvc;
class TriggerElement;

namespace Trk
{
  class Vertex;
}

namespace Reco  { class ITrackToVertex; }//AT Jan 2010:added

class TrigEFElectronHypo : public HLT::HypoAlgo {

 public:

  TrigEFElectronHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFElectronHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltBeginRun();

  /** Find the impact parameter by extrapolating to either the beam spot or (0,0) **/
  double findImpact(const xAOD::TrackParticle* track) const;//AT: copied from Offline

 private:

  // define the properties:
  //----------------------------

  // m_acceptAll = true, all events are taken, not selection cuts applied
  // m_acceptAll = false, selection cuts applied

  bool m_acceptAll; //!<  true all events are taken

  bool m_caloCutsOnly;

  // Cuts to be applied:




  //AT Jan2010:
  std::string m_egammaElectronCutIDToolName;
  std::string m_athElectronLHIDSelectorToolName;
  ToolHandle<IAsgElectronIsEMSelector> m_egammaElectronCutIDTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_athElectronLHIDSelectorTool;
  bool m_useAthElectronLHIDSelector;
  //Offline isEM bit cut
  unsigned int        m_IsEMrequiredBits;  //!< isem flag bits required
  bool       m_applyIsEM;  //!< true if isem flag required 
  double     m_emEt;  //!< ET cut

  //Isolation
  bool m_applyIsolation; 
  unsigned int m_EtConeSizes;
  unsigned int m_PtConeSizes;
  bool m_useClusETforCaloIso;
  bool m_useClusETforTrackIso;
  std::vector<float> m_RelEtConeCut;
  std::vector<float> m_EtConeCut;
  std::vector<float> m_RelPtConeCut;
  std::vector<float> m_PtConeCut;
  std::map<int, string> m_mapEtCone,m_mapPtCone;
  std::map<int, string> m_mapRelEtCone,m_mapRelPtCone;

  int m_NofPassedCuts;
  int m_NofPassedCutsIsEM;
  int m_NofPassedCutsIsEMTrig;

  //AT: variables for isEM monitoring
  //std::vector<unsigned int> m_isEMTrig; //AT Aug2011: variable not needed: deactivate histogram egIsEM - outdated
  std::vector<unsigned int> m_NcandIsEM;   
  std::vector<unsigned int> m_NcandIsEMAfterCut; 
  std::vector<unsigned int> m_IsEMRequiredBits;
  std::vector<unsigned int> m_IsEMRequiredBitsAfterCut;
  ///
  std::string m_path;

  //Impact parameter monitoring
  std::vector<double> m_a0;

  // egamma container
  const xAOD::ElectronContainer* m_EgammaContainer; //!<  pointer to EgammaContainer

 // Timing:

  ITrigTimerSvc*            m_timersvc;
  TrigTimer* m_totalTimer;
  TrigTimer* m_timerPIDTool;

  /** The extrapolator tool */
  //AT Jan 2010: ToolHandle<Trk::IExtrapolator> m_extrapolator; //Extrapolator tool 
  /** The Beam Service **/
  //AT Jan 2010: IBeamCondSvc* m_iBeamCondSvc; //pointer to the beam condition service


  Amg::Vector3D m_primaryVertex; //AT Jan 2010  
  /** @brief Tool handle for track extrapolation to vertex */   	   	 
  ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool; //AT Jan 2010

  void prepareMonitoringVars();
  
  /**
   * \brief implements histograms for monitoring
   */
  ////////////

  // Calo and ShowerShape Monitoring accesible via xAOD::Egamma
  static inline float getF1 (const xAOD::Egamma* eg)	{
      float val_float=-99;
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::f1); 
      return val_float;
  }
  static inline float getF3 (const xAOD::Egamma* eg)	{
      float val_float=-99;
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::f3); 
      return val_float;
  }
  static inline double getE237 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237); 
     return val_float;
  }
  static inline double getE277 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::e277); 
     return val_float;
  }
  static inline double getEthad1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::ethad1); 
     return val_float;
  }
  static inline double getWeta1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::weta1); 
     return val_float;
  }
  static inline double getWeta2 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::weta2); 
     return val_float;
  }
  static inline double getWtots1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::wtots1); 
     return val_float;
  }
  static inline double getE2tsts1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1); 
     return val_float;
  }
  static inline double getEmins1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::emins1); 
     return val_float;
  }
  static inline double getfracs1 (const xAOD::Egamma* eg) {
     float val_float=-99;
     eg->showerShapeValue(val_float,xAOD::EgammaParameters::fracs1); 
     return val_float;
  }
  static inline double EtCluster(const xAOD::Egamma* eg){
    if(eg && (eg->caloCluster())){
        return eg->caloCluster()->et();
    }
    else return -99.;
  }
  static inline double EtCluster37(const xAOD::Egamma* eg){
    if(eg && (eg->caloCluster())){
      const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
      double eta2   = fabs(cluster->etaBE(2)); 
      return cluster->e()/cosh(eta2);      
    }
    else return -99.;
  }
  static inline float getEratio(const xAOD::Egamma* eg) {
      if(eg) {
          // E of 2nd max between max and min in strips
          float emax2  = -99;
          eg->showerShapeValue(emax2,xAOD::EgammaParameters::e2tsts1);   
          // E of 1st max in strips
          float emax   = -99;
          eg->showerShapeValue(emax2,xAOD::EgammaParameters::emaxs1);   
          // E(min) in strips
          float _demaxs1 = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
          return _demaxs1;
      }
      else {return -99.;}
  }

  // Track releated Monitoring accesible via xAOD::Electron
  static inline double getNbLayer  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfBLayerHits); 
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNPixel  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfPixelHits);
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNSCT  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfSCTHits);
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNTRT  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfTRTHits);
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNTRThighTh  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfTRTHighThresholdHits);
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNTRThighThOutliers  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfTRTHighThresholdOutliers);
          return val_uint8;
      }
      else return -99.;
  }

  static inline double getNTRTOutliers  (const xAOD::Electron* eg) {
      uint8_t val_uint8=0;
      if(eg && eg->trackParticle()) {
          eg->trackParticleSummaryValue(val_uint8,xAOD::numberOfTRTOutliers);
          return val_uint8;
      }
      else return -99.;
  }
  static inline double PtTrack (const xAOD::Electron* eg) {
      if(eg && (eg->trackParticle())) 
          return eg->trackParticle()->pt(); 
      else return -99.;
  }
  static inline double dEta (const xAOD::Electron* eg) {
      float val_float = -99;
      if(eg) 
          eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta1);	
      return val_float;
  }
  static inline double dPhi (const xAOD::Electron* eg) {
      float val_float = -99;
      if(eg) 
          eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2);	
      return val_float;
  }
  static inline double Eoverp(const xAOD::Electron* eg)	{
    if(eg && (eg->trackParticle()) && (eg->caloCluster())){
      return eg->caloCluster()->et()/ eg->trackParticle()->pt();   
    }
    else return -99.;
  }
  static inline double rTRT  (const xAOD::Electron* eg) {
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
  

  static inline double getEtconeIso(const xAOD::Egamma* eg){
    if(eg && eg->caloCluster() ){
      const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
      float eta2   = fabs(cluster->etaBE(2)); 
      float et_37  = cluster->e()/cosh(eta2);  
      float val_float = -99;
      eg->isolationValue(val_float,xAOD::EgammaParameters::etcone20); 
      return et_37!=0. ? val_float/et_37 : -99.;
    }
    else return -99.;
  }
  static inline double getPtconeIso(const xAOD::Electron* eg){
    if(eg && eg->trackParticle() ){
      float val_float = -99;
      eg->isolationValue(val_float,xAOD::EgammaParameters::ptcone20); 
      return val_float;
    }
    else return -99.;
  }

  

};
#endif

