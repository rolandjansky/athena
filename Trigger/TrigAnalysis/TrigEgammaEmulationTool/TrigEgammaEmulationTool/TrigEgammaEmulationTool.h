/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEmulationTool_H
#define TrigEgammaEmulationTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/TAccept.h"
#include "AthContainers/AuxElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
//#include "xAODCaloRings/RingSetContainer.h"
//#include "xAODCaloRings/CaloRingsContainer.h"
//#include "xAODCaloRings/tools/getCaloRingsDecorator.h"
#include <string>

namespace Trig{
class TrigEgammaEmulationTool
: public asg::AsgTool,
  virtual public Trig::ITrigEgammaEmulationTool {
  ASG_TOOL_CLASS(TrigEgammaEmulationTool, Trig::ITrigEgammaEmulationTool)

  public:

    //****************************************************************************** 
    TrigEgammaEmulationTool(const std::string& myname);
    ~TrigEgammaEmulationTool() {};

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
   
    //execute all emulators
    bool EventWiseContainer();
    const Root::TAccept& executeTool( const std::string &);
    const Root::TAccept& executeTool( const HLT::TriggerElement *, const std::string &);
    const Root::TAccept& executeTool( const xAOD::Egamma*, const std::string &, const std::string & );
    
    bool isPassed(const std::string&);
    bool isPassed(const std::string&, const std::string&);

  private:
    
    //****************************************************************************** 
    template<class T> const T* getFeature(const HLT::TriggerElement* te, const std::string key="");

    void clearDecorations();
    //****************************************************************************** 

    Root::TAccept                                 m_accept;
    StoreGateSvc                                 *m_storeGate;
    ToolHandle<Trig::TrigDecisionTool>            m_trigdec;
    ToolHandle<Trig::ITrigEgammaMatchingTool>     m_matchTool;

    //selectors tools
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_l1Selector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_l2Selector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_efCaloSelector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_efElectronSelector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_efPhotonSelector;

    //containers
    std::string m_offElContKey;
    const xAOD::ElectronContainer       *m_offElectrons;
    const xAOD::ElectronContainer       *m_onlElectrons;
    const xAOD::PhotonContainer         *m_onlPhotons;
    const xAOD::TrigElectronContainer   *m_trigElectrons;
    const xAOD::CaloClusterContainer    *m_caloClusters;
    const xAOD::TrigEMClusterContainer  *m_trigEMClusters;
    const xAOD::EmTauRoIContainer       *m_emTauRois;


    xAOD::EmTauRoIContainer *m_l1Cont;
};

//************************************************************************************************
  template<class T>
  const T*
  TrigEgammaEmulationTool::getFeature(const HLT::TriggerElement* te,const std::string key){
      if ( te == NULL ) return NULL;
      if ( (m_trigdec->ancestor<T>(te,key)).te() == NULL )
          return NULL;
      return ( (m_trigdec->ancestor<T>(te)).cptr() );
  }
//************************************************************************************************

}//namespace

#endif
