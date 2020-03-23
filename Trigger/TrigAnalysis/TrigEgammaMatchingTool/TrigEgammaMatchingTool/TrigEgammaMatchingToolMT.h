/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaMatchingToolMT_H
#define TrigEgammaMatchingToolMT_H

#include "TrigEgammaMatchingTool/ITrigEgammaMatchingToolMT.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include <iostream>
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig{



class TrigEgammaMatchingToolMT : public asg::AsgTool, virtual public ITrigEgammaMatchingToolMT

{
    ASG_TOOL_CLASS( TrigEgammaMatchingToolMT, ITrigEgammaMatchingToolMT)

    public:

        TrigEgammaMatchingToolMT( const std::string& name );


        virtual ~TrigEgammaMatchingToolMT() override;
        virtual StatusCode initialize() override;

       
        std::string key( std::string ) const;

        bool match(const xAOD::Egamma *,const std::string) const;
        bool match(const xAOD::Egamma *,const std::string, const TrigCompositeUtils::Decision *&) const;
        


        template<class OBJECT> const OBJECT* getFeature( const TrigCompositeUtils::Decision *) const;
        template<class OBJECT> std::vector<const OBJECT*> getFeatures( const TrigCompositeUtils::Decision*, std::string &key ) const;
        template<class CONTAINER> bool ancestorPassed( const TrigCompositeUtils::Decision* ) const;

    private:


        
        bool matchHLTElectron(const xAOD::Electron *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchHLTPhoton(  const xAOD::Photon   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchHLTCalo(    const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Electron( const xAOD::Electron *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Photon(   const xAOD::Photon   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Calo(     const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL1(         const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        
        template< class OBJECT> bool closestObject( const xAOD::Egamma *, const TrigCompositeUtils::Decision *&, std::string trigger, std::string key ) const;
        

        inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) const
        {
          double deta = fabs(eta1 - eta2);
          double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath::Pi() - fabs(phi1 - phi2);
          return sqrt(deta*deta + dphi*dphi);
        };


        std::map<std::string,std::string> m_keys; 
        
        ToolHandle<Trig::TrigDecisionTool>  m_trigDecTool{this, "TriggerTool", ""};
        const ToolHandle<Trig::TrigDecisionTool>& tdt() const { return m_trigDecTool; };
        
        float m_dR;       
        float m_dRL1;       


};


}


#include "TrigEgammaMatchingToolMT.icc"

#endif
