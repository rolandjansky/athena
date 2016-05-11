/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaSelectorBaseTool_H_
#define TrigEgammaSelectorBaseTool_H_


#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AthContainers/AuxElement.h"
#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"


namespace Trig{
  class TrigEgammaSelectorBaseTool:
    public asg::AsgTool,
    virtual public Trig::ITrigEgammaSelectorBaseTool
  { 
    ASG_TOOL_CLASS(TrigEgammaSelectorBaseTool, Trig::ITrigEgammaSelectorBaseTool)
    public:

      //using ITrigEgammaSelectorBaseTool::emulation;

      TrigEgammaSelectorBaseTool(const std::string& myname);
      ~TrigEgammaSelectorBaseTool(){;}

      StatusCode initialize();
      StatusCode finalize();

      //FIXME: static_cast for IParticleContainer to EmTau and emCluster
      //doent work. Because this I add these extra methods. Need to check
      //how generate this for these levels.
      bool emulation(const xAOD::EmTauRoI               *, bool&, const std::string&){return true;};
      bool emulation(const xAOD::TrigEMCluster          *, bool&, const std::string&){return true;};
      //generic method
      bool emulation(const xAOD::IParticleContainer     *, bool&, const std::string&){return true;};

      //parser TDT tool as a pointer
      void setParents(ToolHandle<Trig::TrigDecisionTool> &t, StoreGateSvc *s){ m_trigdec=&(*t); m_storeGate=s; };
      void setParents(Trig::TrigDecisionTool *t, StoreGateSvc *s){ m_trigdec=t; m_storeGate=s; };


    protected:
      /*! Simple setter to pick up correct probe PID for given trigger */
      void parseTriggerName(const std::string,const std::string, bool&, std::string &,
                            float &, float &, std::string &,std::string &, bool&, bool&);
      /*! Split double object trigger in two simple object trigger */
      bool splitTriggerName(const std::string, std::string &, std::string &);
      /*! Creates static map to return L1 item from trigger name */
      std::string getL1Item(std::string trigger);
      /*! Map for pid names */
      std::string getPid(const std::string);
      /* dress decision */
      void dressDecision(const SG::AuxElement*, std::string, bool);

      template<class T> const T* getFeature(const HLT::TriggerElement* te,const std::string key="");
      template<class T> bool ancestorPassed(const HLT::TriggerElement* te,const std::string key="");
      /* trig rings and offline rings helper method for feature extraction from xaod */
      bool getCaloRings( const xAOD::Electron *, std::vector<float> & );
      const xAOD::TrigRingerRings* getTrigCaloRings( const xAOD::TrigEMCluster * );
      float dR(const float, const float, const float, const float );
      
      StoreGateSvc           *m_storeGate;
      Trig::TrigDecisionTool *m_trigdec;
  };


  /** Main feature of the code
   * templates the TE to get a feature
   * or find the passing TE
   * **********************/
  template<class T>
  const T*
  TrigEgammaSelectorBaseTool::getFeature(const HLT::TriggerElement* te,const std::string key){
      if ( te == NULL ) return NULL;
      if ( (m_trigdec->ancestor<T>(te,key)).te() == NULL )
          return NULL;
      return ( (m_trigdec->ancestor<T>(te)).cptr() );
  }
  
  template<class T>
  bool
  TrigEgammaSelectorBaseTool::ancestorPassed(const HLT::TriggerElement* te,const std::string key){
      if ( te == NULL ) return false;
      if ( (m_trigdec->ancestor<T>(te,key)).te() == NULL )
          return false;
      return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
  }
}//namespace
#endif
