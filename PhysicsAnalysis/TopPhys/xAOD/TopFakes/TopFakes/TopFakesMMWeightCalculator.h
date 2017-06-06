/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopFakesMMWeightCalculator.h 762053 2016-07-15 04:39:49Z tpelzer $
#ifndef TOPFAKESMMWEIGHTCALCULATOR_H
#define TOPFAKESMMWEIGHTCALCULATOR_H

/**
  * @author Timothée Theveneaux-Pelzer <tpelzer@cern.ch>
  * 
  * @brief TopFakesMMWeightCalculator
  *   Calculate Matrix-Method Weights for fake prompt lepton background estimates
  **/

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// For Object containers
#include "TopEvent/Event.h"

#include <map>

// Classes from fakesWeight framework
class FakesWeights;
class MMLepton;
class MMEvent;

// Forward declaration(s):
namespace top{
  class TopConfig;
  class Event;
  namespace fakes {
    enum class LeptonDefinitions;
  }
}

namespace top{

  class TopFakesMMWeightCalculator final : public asg::AsgTool {
    public:
      explicit TopFakesMMWeightCalculator( const std::string& name = "MMWeightCalculator" );
      virtual ~TopFakesMMWeightCalculator();

      // Delete Standard constructors
      TopFakesMMWeightCalculator(const TopFakesMMWeightCalculator& rhs) = delete;
      TopFakesMMWeightCalculator(TopFakesMMWeightCalculator&& rhs) = delete;
      TopFakesMMWeightCalculator& operator=(const TopFakesMMWeightCalculator& rhs) = delete;

      StatusCode initialize();
      StatusCode setSelectionConfigs(std::string selection, std::vector<std::string> configs);
      StatusCode execute(top::Event* topEvent);
      
      std::vector<std::string> GetFakesMMConfigNames(std::string selection) const;

    private:
      std::shared_ptr<top::TopConfig> m_config;
      
      std::string m_dir;
      
      bool m_debug;
      
      // unordered_map of FakesWeight objects
      // m_fakesWeightTools[selection][configuration]
      std::unordered_map<std::string,std::unordered_map<std::string,std::shared_ptr<FakesWeights>>> m_fakesWeightTools;
      
      float getMMweightE(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info);
      float getMMweightM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info);
      float getMMweightEE(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info);
      float getMMweightMM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info);
      float getMMweightEM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info);
      
      MMLepton* getLeptonMMInfo(const xAOD::Electron& el, const xAOD::JetContainer& jets, const xAOD::MissingET& met, const xAOD::EventInfo* event_info);
      MMLepton* getLeptonMMInfo(const xAOD::Muon& mu, const xAOD::JetContainer& jets, const xAOD::MissingET& met, const xAOD::EventInfo* event_info);
      MMEvent*  getEventMMInfo( const xAOD::JetContainer& jets, const xAOD::MissingET& met);
      
      int getPeriod ( const std::string& period_string );
      
//       fakes::LeptonDefinitions m_leptonDefs;
//       void CheckConfiguration();
      
      ///-- Tell RootCore to build a dictionary (we need this) --///
      ClassDef(top::TopFakesMMWeightCalculator, 0);

  };
} // namespace
#endif
