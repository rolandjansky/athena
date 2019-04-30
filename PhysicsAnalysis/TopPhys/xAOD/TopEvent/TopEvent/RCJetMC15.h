/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RCJETMC15_H_
#define RCJETMC15_H_

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/SystematicEvent.h"
#include "TopConfiguration/ConfigurationSettings.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "JetReclustering/JetReclusteringTool.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>
#include <list>
#include <vector>

// Forward declaration(s):
namespace xAOD{
    class SystematicEvent;
}

namespace top{
    class TopConfig;
    class Event;
}

namespace fastjet{
  class JetDefinition;
  namespace contrib{
    class Nsubjettiness;
    class EnergyCorrelator;
  }
}

namespace JetSubStructureUtils{
  class KtSplittingScale;
  class Qw;
  class EnergyCorrelatorGeneralized;
}

class RCJetMC15 final : public asg::AsgTool{
public:
    //Default - so root can load based on a name
    explicit RCJetMC15(const std::string& name);

    //Default - so we can clean up
    ~RCJetMC15();

    // Delete Standard constructors
    RCJetMC15(const RCJetMC15& rhs) = delete;
    RCJetMC15(RCJetMC15&& rhs) = delete;
    RCJetMC15& operator=(const RCJetMC15& rhs) = delete;

    //Run once at the start of the job
    StatusCode initialize();

    //Run for every event (in every systematic) that needs saving
    StatusCode execute(const top::Event& event);

    //Clean-up remaining things
    StatusCode finalize();

    //Check if systematic needs a unique container
    bool isUniqueSyst( std::string syst_name );

    //Access all of the container names from the Event Saver
    std::string inputContainerName( std::size_t hash_value, bool isLooseEvent );
    std::string rcjetContainerName( std::size_t hash_value, bool isLooseEvent );

    bool passSelection(const xAOD::Jet& jet) const;
    
    void getEMTopoClusters(std::vector<fastjet::PseudoJet>& clusters,const xAOD::Jet* rcjet);
    void getLCTopoClusters(std::vector<fastjet::PseudoJet>& clusters,const xAOD::Jet* rcjet);
    
    void getPflowConstituent(std::vector<fastjet::PseudoJet>& clusters, const xAOD::Jet* rcjet, const top::Event& event);


private:

    std::string m_name;

    std::shared_ptr<top::TopConfig> m_config;
    bool m_VarRCjets;
    std::string m_VarRCjets_rho;
    std::string m_VarRCjets_mass_scale;
    float m_ptcut;       // in GeV
    float m_etamax;
    float m_trim;
    float m_radius;
    float m_minradius;
    float m_massscale;
    bool  m_useJSS;
    bool  m_useAdditionalJSS;
    
    std::string m_egamma;
    std::string m_jetsyst;
    std::string m_muonsyst;
    std::string m_tracksyst;

    std::string m_InJetContainerBase;
    std::string m_OutJetContainerBase;
    std::string m_InputJetContainer;
    std::string m_OutputJetContainer;
    int m_loose_hashValue;

    //Substructure tool definitions
    std::shared_ptr<fastjet::JetDefinition> m_jet_def_rebuild; 	  
    std::shared_ptr<fastjet::contrib::Nsubjettiness> m_nSub1_beta1;
    std::shared_ptr<fastjet::contrib::Nsubjettiness> m_nSub2_beta1;
    std::shared_ptr<fastjet::contrib::Nsubjettiness> m_nSub3_beta1;
    std::shared_ptr<fastjet::contrib::EnergyCorrelator> m_ECF1;
    std::shared_ptr<fastjet::contrib::EnergyCorrelator> m_ECF2;
    std::shared_ptr<fastjet::contrib::EnergyCorrelator> m_ECF3;
    std::shared_ptr<JetSubStructureUtils::KtSplittingScale> m_split12;
    std::shared_ptr<JetSubStructureUtils::KtSplittingScale> m_split23;
    std::shared_ptr<JetSubStructureUtils::Qw> m_qw;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF332;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF461;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF322;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF331;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF422;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF441;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF212;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF321;
    std::shared_ptr<JetSubStructureUtils::EnergyCorrelatorGeneralized> m_gECF311;
    
    bool m_unique_syst;

    typedef std::map<std::size_t,std::string>::iterator m_iterator;
    std::map<std::size_t,std::string> m_inputContainerNames;
    std::map<std::size_t,std::string> m_outputContainerNames;

    std::map<std::string,float> mass_scales = {
        {"m_t",172500.},
        {"m_w",80385.},
        {"m_z",91188.},
        {"m_h",125090.}};

    //re-clustered jets
    //  -> need unordered map for systematics
    std::unordered_map<std::size_t, std::shared_ptr<JetReclusteringTool> > m_jetReclusteringTool;
    typedef std::unordered_map<std::size_t, std::shared_ptr<JetReclusteringTool> >::iterator m_tool_iterator;

    ClassDef(RCJetMC15, 0);

};

#endif
