/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrigEgammaEmulationTool_H
#define TrigEgammaEmulationTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
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
#include "TrigEgammaEmulationTool/TrigEgammaInfo.h"
#include <vector>
#include <map>


namespace Trig{
class TrigEgammaEmulationTool
: public asg::AsgTool,
  virtual public Trig::ITrigEgammaEmulationTool {
  ASG_TOOL_CLASS(TrigEgammaEmulationTool, Trig::ITrigEgammaEmulationTool)

  public:

    //****************************************************************************** 
    TrigEgammaEmulationTool(const std::string& myname);
    virtual ~TrigEgammaEmulationTool() {};

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

    //execute all emulators
    virtual asg::AcceptData executeTool( const std::string &) override;
    virtual asg::AcceptData executeTool( const HLT::TriggerElement *, const std::string &) override;
    virtual const asg::AcceptInfo& getAccept() const override {return m_accept;}
    
    virtual bool EventWiseContainer() override;
    virtual bool isPassed(const std::string&) override;
    virtual bool isPassed(const std::string&, const std::string&) override;

    
    /* Experimental methods */
    virtual void ExperimentalAndExpertMethods() override {m_experimentalAndExpertMethods=true;};
    
    virtual void match( const xAOD::Egamma *, const HLT::TriggerElement *&) override;
    virtual const HLT::TriggerElement* getTEMatched() override {return m_teMatched;};


  private:

    void setTEMatched(const HLT::TriggerElement *te){m_teMatched=te;};

    bool emulationHLT(const xAOD::IParticleContainer *, bool &, const Trig::Info &);

    /* Retrieve features from trigger element */ 
    template<class T> const T* getFeature(const HLT::TriggerElement* te, const std::string key="");
    /* Set trigInfo into the map*/
    void setTrigInfo(const std::string );
    /*! get trigInfo obj */
    Trig::Info getTrigInfo(const std::string &);
    /*! Creates static map to return L1 item from trigger name */
    std::string getL1Item(std::string trigger);
    /*! Map for pid names */
    std::string getPid(const std::string);
    /* decorations for objects */
    void clearDecorations();
    /*! Simple setter to pick up correct probe PID for given trigger */
    void parseTriggerName(const std::string,const std::string, bool&, std::string &,
                          float &, float &, std::string &, std::string &,std::string &, bool&, bool&);
 
    bool                                          m_doL2ElectronFex;
    bool                                          m_doEFCaloPid;       
    bool                                          m_doRinger;
    bool                                          m_experimentalAndExpertMethods;
    std::string                                   m_offElContKey; 
    std::vector<std::string>                      m_trigList;
    std::vector<std::string>                      m_supportingTrigList;
    std::map<std::string, Trig::Info>             m_trigInfo;

    asg::AcceptInfo                               m_accept;
    StoreGateSvc                                 *m_storeGate;
    ToolHandle<Trig::TrigDecisionTool>            m_trigdec;
    ToolHandle<Trig::ITrigEgammaMatchingTool>     m_matchTool;
    //selectors tools
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_l1Selector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_l2Selector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool> m_efCaloSelector;

    // EmulationHLT helper function
    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_efElectronSelector;
    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_efPhotonSelector;

    const HLT::TriggerElement           *m_teMatched;
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
