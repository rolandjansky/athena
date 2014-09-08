/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFPhotonHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEFPhotonHypo
 *
 * AUTHOR:   Valeria Perez Reale
 * MAINTAINED BY: R.M. White 
 * CREATED:  Nov 12, 2006
 * MODIFIED: V. Perez Reale  added doxygen comments 25/6/07
 *           A.Tricoli: added extra variables to monitor isEM flag (Jan 2010)
 * MODIFIED: R.M. White: xAOD migration (April 2014)
 *********************************************************************/
#ifndef TRIGEFPHOTONHYPO_H
#define TRIGEFPHOTONHYPO_H

#include <string>
#include <cmath> 
//#include <map.h>

// athena stuff stuff
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "CLHEP/Units/SystemOfUnits.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"

//xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"

//RMW Sept 2013
#include "PATCore/IAthSelectorTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"

//#include <string
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

class StoreGateSvc;
class TriggerElement;
class TH1F;


/**
 * \class TrigEFPhotonHypo
 * \brief TrigEFPhotonHypo is a Trigger Hypothesis  Algorithm that retrieves the EF EgammaContainer
 * container created by the EF algorithm TrigEgammaRec
 * Applies a selection on ET, shower shape selection to select a good photon at the EF
 * an event will be accepted if all photon selection variables are satisfied
 *
 */


class TrigEFPhotonHypo : public HLT::HypoAlgo {

 public:
  TrigEFPhotonHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFPhotonHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltBeginRun();

 private:

  // define the properties:
  //----------------------------

  // m_acceptAll = true, all events are taken, not selection cuts applied
  // m_acceptAll = false, selection cuts applied

  bool m_acceptAll; //!<  true all events are taken
  bool m_acceptLOOSE; //!<  true only events that satisfy ET > ETcut are taken
  
  //Calocuts offline

  /** @brief  Luminosity level */
 // std::string m_LumiLevel; replaced with usePhotonCuts
  bool m_usePhotonCuts;

  std::string m_egammaElectronCutIDToolName;
  ToolHandle<IAsgElectronIsEMSelector> m_egammaElectronCutIDTool;

  std::string m_egammaPhotonCutIDToolName;
  ToolHandle<IAsgPhotonIsEMSelector> m_egammaPhotonCutIDTool;

  //Offline isEM bit cut
  unsigned int        m_IsEMrequiredBits;  //!< isem flag bits required
  bool       m_applyIsEM;  //!< true if isem flag required
  double       m_emEt;              //!< ET cut
  int        m_NofPassedCuts;
  int        m_NofPassedCutsIsEM; 
  int        m_NofPassedCutsIsEMTrig; 

  //AT: variables for isEM monitoring
  //std::vector<unsigned int> m_isEMTrig; //AT Aug2011: variable not needed - deactivate histogram egIsEM - outdated
  std::vector<unsigned int> m_NcandIsEM;  
  std::vector<unsigned int> m_NcandIsEMAfterCut; 
  std::vector<unsigned int> m_IsEMRequiredBits; 
  std::vector<unsigned int> m_IsEMRequiredBitsAfterCut;
  
  const xAOD::PhotonContainer* m_EgammaContainer; //!< pointer to EgammaContainer
  
  
  std::vector<IHistogram1D*> nCandHistVector;

  // path for the histograms:

  std::string m_path;

  // Histogram Service:
  IHistogramSvc* m_histsvc;


 // Timing:

  TrigTimer* m_totalTimer, *m_timerPIDTool_Ele, *m_timerPIDTool_Pho;
  ITrigTimerSvc*            m_timersvc; //!< pointer to TimerSvc

  void prepareMonitoringVars();

  /**
   * \brief implements histograms for monitoring
   */

  static inline double Et    (const xAOD::Egamma* eg)	{if(eg && (eg->caloCluster())) return eg->caloCluster()->et(); else return -99.;}
  static inline double Eta    (const xAOD::Egamma* eg)	{if(eg && (eg->caloCluster())) return eg->eta(); else return -99.;}
  static inline double Phi    (const xAOD::Egamma* eg)	{if(eg && (eg->caloCluster())) return eg->phi(); else return -99.;}
  
  static inline double EtCluster37(const xAOD::Egamma* eg){
    if(eg && (eg->caloCluster())){
      const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
      double eta2   = fabs(cluster->etaBE(2)); 
      return cluster->e()/cosh(eta2);      
    }
    else return -99.;
  }
  static inline float rE37E77 (const xAOD::Egamma* eg) {
     float e277=-99;
     float e237=-99;
     eg->showerShapeValue(e237,xAOD::EgammaParameters::e237); 
     eg->showerShapeValue(e277,xAOD::EgammaParameters::e277); 
     float _rE37E77 = (e277 != 0.) ? ( e237 / e277 ) : -99. ;
     return _rE37E77;
  }
  static inline float rE33E77 (const xAOD::Egamma* eg) {
     float e277=-99;
     float e233=-99;
     eg->showerShapeValue(e233,xAOD::EgammaParameters::e233); 
     eg->showerShapeValue(e277,xAOD::EgammaParameters::e277); 
     float _rE33E77 = (e277 != 0.) ? ( e233 / e277 ) : -99. ;
     return _rE33E77;
  }

  static inline float Eratio(const xAOD::Egamma* eg) {
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


  //xAOD, what is in EDM
  //TBD
 /*static inline int monIsEM(const egamma* eg){
    if(eg && eg->isem() ) {
      int pass = 0;
      if ((eg->isem() & 0xFD)      ==0)   pass = 1;  //AT 16 March 0xFF -> 0xFD (remove ConversionMatch) //loose
      if ((eg->isem() & 0x1E8EFD)  ==0)   pass = 2;  //AT 16 March 0x1E8EFF -> 0x1E8EFD (remove ConversionMatch) //medium //AT jan 2010: 0x1E8FFF ->0x1E8EFF (no difference)
      if ((eg->isem() & 0x3FFFFFF) ==0)   pass = 3;  //tight
      return pass;
    } else {
      return 0;
    }
  }   

  static inline unsigned int monIsEMTrig(unsigned int isem){
    int pass = 0;
    if ((isem & 0xFF)      ==0)   pass = 1;  //loose
    if ((isem & 0x1E8EFF)  ==0)   pass = 2;  //medium //AT jan 2010: 0x1E8FFF ->0x1E8EFF (no difference)
    if ((isem & 0x3FFFFFF) ==0)   pass = 3;  //tight
    return pass;
  }  */ 


};
#endif

