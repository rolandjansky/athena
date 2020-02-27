/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAMATCHINGTOOL_H_
#define TRIGEGAMMAMATCHINGTOOL_H_

#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingUtils.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include <iostream>
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig{



class TrigEgammaMatchingTool : public asg::AsgTool, virtual public ITrigEgammaMatchingTool

{
    ASG_TOOL_CLASS( TrigEgammaMatchingTool, ITrigEgammaMatchingTool)

    public:

        TrigEgammaMatchingTool( const std::string& name );


        virtual ~TrigEgammaMatchingTool() override;
        virtual StatusCode initialize() override;

       
        std::string key( std::string ) const;

        bool match(const xAOD::Egamma *,const std::string) const;
        bool match(const xAOD::Egamma *,const std::string, TrigEgammaMatchingUtils::Element &) const;
        

        template<class OBJECT> std::vector< TrigEgammaMatchingUtils::Feature<const OBJECT*> > features( std::string trigger ) const;
        template<class OBJECT> std::vector< TrigEgammaMatchingUtils::Feature<const OBJECT*> > features( std::string trigger, std::string key ) const;



        template<class OBJECT> const OBJECT* getFeature( const TrigEgammaMatchingUtils::Element ) const;
        template<class OBJECT> TrigEgammaMatchingUtils::Container<const OBJECT*> getFeatures( const TrigEgammaMatchingUtils::Element ) const;
        template<class OBJECT> TrigEgammaMatchingUtils::Container<const OBJECT*> getFeatures( const TrigEgammaMatchingUtils::Element, std::string key) const;




        template<class OBJECT> const OBJECT* closestObject(std::pair<const xAOD::Egamma *, const TrigEgammaMatchingUtils::Element>, std::string, float ) const;
        
        template<class CONTAINER> bool ancestorPassed( const TrigEgammaMatchingUtils::Element, const std::string="feature") const;

    private:


        
        bool matchHLTElectron(const xAOD::Electron *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchHLTPhoton(  const xAOD::Photon   *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchHLTCalo(    const xAOD::Egamma   *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchL2Electron( const xAOD::Electron *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchL2Photon(   const xAOD::Photon   *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchL2Calo(     const xAOD::Egamma   *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        bool matchL1(         const xAOD::Egamma   *,const std::string &, TrigEgammaMatchingUtils::Element &) const;
        
        template< class OBJECT> bool closestObject( const xAOD::Egamma *, TrigEgammaMatchingUtils::Element &, std::string trigger, std::string key ) const;
        

        inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) const
        {
          double deta = fabs(eta1 - eta2);
          double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath::Pi() - fabs(phi1 - phi2);
          return sqrt(deta*deta + dphi*dphi);
        };


        std::map<std::string,std::pair<std::string,std::string>> m_keys; 
        
        ToolHandle<Trig::TrigDecisionTool>  m_trigDecTool{this, "TriggerTool", ""};
        const ToolHandle<Trig::TrigDecisionTool>& tdt() const { return m_trigDecTool; };
        
        float m_dR;       
        float m_dRL1;       


};


}


#include "TrigEgammaMatchingTool.icc"

#endif
