/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaSelectorBaseTool_H_
#define TrigEgammaSelectorBaseTool_H_


#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "AthContainers/AuxElement.h"
#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include <bitset>
#include <map>
#include <boost/any.hpp>

//#include "xAODCaloRings/RingSetContainer.h"                     //
//#include "xAODCaloRings/CaloRingsContainer.h"                   //
//#include "xAODCaloRings/tools/getCaloRingsDecorator.h"          //


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
      bool emulation(const xAOD::EmTauRoI               *, bool&, const Trig::Info &){return true;};
      bool emulation(const xAOD::TrigEMCluster          *, bool&, const Trig::Info &){return true;};
      bool emulation(const xAOD::IParticle              *, bool&, const Trig::Info &){return true;};
      bool emulation(const xAOD::IParticleContainer     *, bool&, const Trig::Info &){return true;};
      
      //parser TDT tool as a pointer
      void setParents(ToolHandle<Trig::TrigDecisionTool> &t, StoreGateSvc *s)
      { m_trigdec=&(*t); m_storeGate=s;};
      void setParents(Trig::TrigDecisionTool *t, StoreGateSvc *s)
      { m_trigdec=t; m_storeGate=s;};
      void setTe(const HLT::TriggerElement *te){m_te=te;}; 

    protected:
      /* dress decision */
      void dressDecision(const SG::AuxElement*, std::string, bool);
      /* Offline rings helper method for feature extraction from xaod */
      bool getCaloRings( const xAOD::Electron *, std::vector<float> & );
      /* Trigger rings helper method for feature extraction from xaod */
      const xAOD::TrigRingerRings* getTrigCaloRings( const xAOD::TrigEMCluster * );
      /* DettaR  */
      float dR(const float, const float, const float, const float );

      float getOnlAverageMu();
      float getAverageMu();
 
      //******************************************************************
      template<class T> const T* getFeature(const HLT::TriggerElement* te,const std::string key="");
      template<class T> bool ancestorPassed(const HLT::TriggerElement* te,const std::string key="");
      //******************************************************************
      
     
      ToolHandle<ILumiBlockMuTool>     m_lumiBlockMuTool; // This would retrieve the online <mu>
      ToolHandle<ILuminosityTool>      m_lumiTool; // This would retrieve the offline <mu>
      StoreGateSvc                    *m_storeGate;
      Trig::TrigDecisionTool          *m_trigdec;
      const HLT::TriggerElement       *m_te;
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
