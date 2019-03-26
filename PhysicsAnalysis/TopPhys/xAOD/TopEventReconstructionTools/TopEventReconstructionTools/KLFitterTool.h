/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: KLFitterTool.h 788027 2016-12-04 18:30:45Z aknue $
#ifndef ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERTOOL_H
#define ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "TopConfiguration/Tokenize.h"


// KLFitter include(s):
#include "KLFitter/Fitter.h"
#include "KLFitter/LikelihoodTopLeptonJets.h"
#include "KLFitter/LikelihoodTTHLeptonJets.h"
#include "KLFitter/LikelihoodTopLeptonJets_JetAngles.h"
#include "KLFitter/LikelihoodTTZTrilepton.h"
#include "KLFitter/LikelihoodTopAllHadronic.h"
#include "KLFitter/BoostedLikelihoodTopLeptonJets.h"
#include "KLFitter/PhysicsConstants.h"
#include "KLFitter/LikelihoodBase.h"
#include "KLFitter/DetectorAtlas_8TeV.h"
#include "KLFitter/Permutations.h"

// system include(s):
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <memory>


  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 788027 $
  /// $Date: 2016-12-04 18:30:45 +0000 (Sun, 04 Dec 2016) $
  ///
  /// @brief Interface to KLFitter
  ///  
  
// Forware declare
namespace KLFitter{
  class Particles;
}

namespace top{
  class Event;
  class TopConfig;
  
  /// Some additional enums
  namespace KLFitterJetSelection{
    enum JetSelectionMode{ kLeadingThree ,  kLeadingFour , kLeadingFive , kLeadingSix , kLeadingSeven , kBtagPriorityThreeJets , kBtagPriorityFourJets , kBtagPriorityFiveJets , kBtagPrioritySixJets , kBtagPrioritySevenJets};
  }
        

  class KLFitterTool final : public asg::AsgTool {

    public:      
      /// Constructor
      explicit KLFitterTool( const std::string& name );
      /// Destructor
      virtual ~KLFitterTool(){}
      
      /// Function initialising the tool
      virtual StatusCode initialize();
      /// Function executing the tool
      virtual StatusCode execute(const top::Event&);
      /// Function finalizing the tool
      virtual StatusCode finalize();
      
    private:
      
      // set jets depending on selection mode 
      bool  setJets(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskLeadingThree(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskLeadingFour(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskLeadingFive(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskLeadingSix(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskLeadingSeven(const top::Event&,KLFitter::Particles* inputParticles);

      bool  setJetskLeadingX(const top::Event& event,KLFitter::Particles* inputParticles, const unsigned int);


      bool  setJetskBtagPriorityThreeJets(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskBtagPriorityFourJets(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskBtagPriorityFiveJets(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskBtagPrioritySixJets(const top::Event&,KLFitter::Particles* inputParticles);
      bool  setJetskBtagPrioritySevenJets(const top::Event&,KLFitter::Particles* inputParticles);

      bool  setJetskBtagPriority(const top::Event&,KLFitter::Particles* inputParticles,const unsigned int maxJets);
      
      // configuration
      std::shared_ptr<top::TopConfig> m_config;
      float m_massTop;   
      float m_bTagCutValue;
      bool m_isWorkingPoint;
      bool findOption(std::vector<std::string> full_options, std::string option, std::string& op_value);

      // Tools and functions for btagging                                                                                                                                               
      ToolHandle<IBTaggingEfficiencyTool> m_btagging_eff_tool;
      bool HasTag(const xAOD::Jet& jet, double& weight) const;
      void retrieveEfficiencies(const xAOD::Jet& jet, float* efficiency, float* inefficiency);
      
      /// KLFitter parameters, to be set by input file
      std::string m_transferFunctionsPathPrefix;
      std::string m_transferFunctionsPath;
      
      std::string m_selectionName;
      std::string m_leptonType;
      std::string m_customParameters;
      std::string m_LHType;
      KLFitter::LikelihoodTopLeptonJets::LeptonType m_leptonTypeKLFitterEnum;      
      KLFitter::LikelihoodTTHLeptonJets::LeptonType m_leptonTypeKLFitterEnum_TTH;
      KLFitter::LikelihoodTopLeptonJets_JetAngles::LeptonType m_leptonTypeKLFitterEnum_JetAngles;
      KLFitter::LikelihoodTTZTrilepton::LeptonType m_leptonTypeKLFitterEnum_TTZ;
      KLFitter::BoostedLikelihoodTopLeptonJets::LeptonType m_leptonTypeKLFitterEnum_BoostedLJets;

      std::unique_ptr<KLFitter::LikelihoodTopLeptonJets> m_myLikelihood;
      std::unique_ptr<KLFitter::LikelihoodTTHLeptonJets> m_myLikelihood_TTH;
      std::unique_ptr<KLFitter::LikelihoodTopLeptonJets_JetAngles> m_myLikelihood_JetAngles;
      std::unique_ptr<KLFitter::LikelihoodTTZTrilepton> m_myLikelihood_TTZ;
      std::unique_ptr<KLFitter::LikelihoodTopAllHadronic> m_myLikelihood_AllHadronic;
      std::unique_ptr<KLFitter::BoostedLikelihoodTopLeptonJets> m_myLikelihood_BoostedLJets;

      std::unique_ptr<KLFitter::DetectorAtlas_8TeV> m_myDetector;

      top::KLFitterJetSelection::JetSelectionMode m_jetSelectionModeKLFitterEnum;     
      KLFitter::LikelihoodBase::BtaggingMethod m_bTaggingMethodKLFitterEnum;
      
      /// The KLFitter
      std::unique_ptr<KLFitter::Fitter> m_myFitter;
    
  };
}
#endif
