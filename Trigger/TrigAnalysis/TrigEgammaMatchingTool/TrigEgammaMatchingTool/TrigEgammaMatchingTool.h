/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAMATCHINGTOOL_H_
#define TRIGEGAMMAMATCHINGTOOL_H_

#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include <iostream>

#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig{
class TrigEgammaMatchingTool : 
    public asg::AsgMetadataTool,
    public virtual ITrigEgammaMatchingTool
{
    ASG_TOOL_INTERFACE(Trig::TrigEgammaMatchingTool)
        ASG_TOOL_CLASS2( TrigEgammaMatchingTool, Trig::ITrigEgammaMatchingTool,Trig::TrigEgammaMatchingTool)

    public:

        TrigEgammaMatchingTool( const std::string& name );

        virtual ~TrigEgammaMatchingTool();

        virtual StatusCode initialize(void);
        virtual StatusCode execute();
        virtual StatusCode finalize();
        /*! Egamma trigger matching which returns TE */
        bool match(const xAOD::Egamma *,const std::string,const HLT::TriggerElement*&);
        /*! Egamma trigger matching to HLT Electron/Photon with DeactivatedTE*/
        bool match(const xAOD::Egamma *,const std::string);
        /*! Egamma trigger matching to HLT Electron/Photon Only */
        bool matchHLT(const xAOD::Egamma *,const std::string);
        /*! Egamma trigger matching to HLT Electron/Photon only -- returns nearest object */
        const xAOD::Egamma* closestHLTObject(const xAOD::Egamma *,const std::string);
        double dR(const double, const double, const double, const double);
        double m_dR;
        double m_dRL1;
    protected:
    private:
        ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
        
        template<class T> const T* getFeature(const HLT::TriggerElement* te);
        template<class T> bool ancestorPassed(const HLT::TriggerElement* te);
        bool matchHLTElectron(const xAOD::Electron *,const std::string,const HLT::TriggerElement*&);
        bool matchHLTPhoton(const xAOD::Photon *,const std::string,const HLT::TriggerElement*&);
        bool matchHLTCalo(const xAOD::Egamma *,const std::string,const HLT::TriggerElement*&);
        bool matchL2Electron(const xAOD::Electron *,const std::string,const HLT::TriggerElement*&);
        bool matchL2Photon(const xAOD::Photon *,const std::string,const HLT::TriggerElement*&);
        bool matchL2Calo(const xAOD::Egamma *,const std::string,const HLT::TriggerElement*&);
        bool matchL1(const xAOD::Egamma *,const std::string,const HLT::TriggerElement*&);

};
}
#endif
