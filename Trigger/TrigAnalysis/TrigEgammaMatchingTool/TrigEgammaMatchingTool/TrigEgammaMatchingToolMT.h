/*
 * (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TrigEgammaMatchingToolMT_H
#define TrigEgammaMatchingToolMT_H

#include "GaudiKernel/Property.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"


namespace Trig{



class TrigEgammaMatchingToolMT : public asg::AsgTool

{
    ASG_TOOL_CLASS0( TrigEgammaMatchingToolMT )

    public:

        TrigEgammaMatchingToolMT( const std::string& name );


        ~TrigEgammaMatchingToolMT();
        StatusCode initialize();


        bool match(const xAOD::Egamma *,const std::string&) const;
        bool match(const xAOD::Egamma *,const std::string&, const TrigCompositeUtils::Decision *&) const;
        
        std::string key( std::string ) const;
        template<class CONTAINER> bool ancestorPassed( const TrigCompositeUtils::Decision*, const std::string&trigger , const std::string&key) const;
        template<class OBJECT> const OBJECT* getFeature( const TrigCompositeUtils::Decision * ) const;
        template<class OBJECT> std::vector<const OBJECT*> getFeatures( const TrigCompositeUtils::Decision * ) const;
        template<class OBJECT> const OBJECT* getFeature( const TrigCompositeUtils::Decision *, const std::string &trigger, const std::string &key ) const;
    
    
    private:
        
        bool matchHLTElectron(const xAOD::Electron *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchHLTPhoton(  const xAOD::Photon   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchHLTCalo(    const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Electron( const xAOD::Electron *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Photon(   const xAOD::Photon   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL2Calo(     const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        bool matchL1(         const xAOD::Egamma   *,const std::string &, const TrigCompositeUtils::Decision *&) const;
        
        template< class OBJECT> bool closestObject( const xAOD::Egamma *, const TrigCompositeUtils::Decision *&, std::string trigger, std::string key ) const;
        
        template<class CONTAINER> bool isAvailable(SG::ReadHandle<CONTAINER>& inViewContainer) const;

        inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) const
        {
          double deta = std::abs(eta1 - eta2);
          double dphi = std::abs(phi1 - phi2) < TMath::Pi() ? std::abs(phi1 - phi2) : 2*TMath::Pi() - std::abs(phi1 - phi2);
          return sqrt(deta*deta + dphi*dphi);
        };


        std::map<std::string,std::string> m_keys; 
        
        ToolHandle<Trig::TrigDecisionTool>  m_trigDecTool{this, "TriggerTool", ""};
        const ToolHandle<Trig::TrigDecisionTool>& tdt() const { return m_trigDecTool; };
        
        Gaudi::Property<float> m_dR{this, "DeltaR", 0.07};       
        Gaudi::Property<float> m_dRL1{this, "L1DeltaR", 0.15};       
        SG::ReadHandleKey<xAOD::EmTauRoIContainer>        m_emTauRoIKey{this, "EmTauRoIKey" , "LVL1EmTauRoIs", ""};
        SG::ReadHandleKey<xAOD::TrigEMClusterContainer>   m_emClusterKey{this, "emClusterKey" , "HLT_L2CaloEMClusters", ""};
        SG::ReadHandleKey<xAOD::TrigElectronContainer>    m_trigElectronKey{this, "TrigElectronKey" , "HLT_L2Electrons", ""};
        SG::ReadHandleKey<xAOD::TrigPhotonContainer>      m_trigPhotonKey{this, "TrigPhotonKey" , "HLT_L2Photons", ""};
        SG::ReadHandleKey<xAOD::CaloClusterContainer>     m_clusterKey{this, "ClusterKey" , "HLT_CaloEMClusters", ""};
        SG::ReadHandleKey<xAOD::ElectronContainer>        m_electronKey{this, "ElectronKey" , "HLT_egamma_Electrons", ""};
        SG::ReadHandleKey<xAOD::PhotonContainer>          m_photonKey{this, "PhotonKey" , "HLT_egamma_Photons", ""};
};


}


#include "TrigEgammaMatchingToolMT.icc"

#endif
