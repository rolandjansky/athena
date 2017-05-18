/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectCollectionMaker.h 794215 2017-01-27 15:12:13Z tpelzer $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_JETOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_JETOBJECTCOLLECTIONMAKER_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief JetObjectCollectionMaker
  *   Makes all systematic variations of jets
  * 
  *   This is actually quite a complex tool
  *   The steps are as follows
  *   (1) Calibrate data/MC jets and save this collection to TStore
  *   (2) Loop over the various NP JES scenarios, using the output of (1) as an input
  *   (3) Loop over the various NP JER scenarios, using the output of (1) as an input
  *   (2.a , 3.a) Re-calculate the JVT for each jet
  * 
  * $Revision: 794215 $
  * $Date: 2017-01-27 16:12:13 +0100 (Fri, 27 Jan 2017) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <set>
#include <list>
#include <unordered_map>


// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODJet/JetConstituentVector.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetResolution/IJERSmearingTool.h"
#include "JetInterface/IJetUpdateJvt.h"

#include "TopJetSubstructure/TopJetSubstructure.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class JetObjectCollectionMaker final : public asg::AsgTool {

    using systMap = std::unordered_map<CP::SystematicSet,CP::SystematicSet>;
    
    public:
      explicit JetObjectCollectionMaker( const std::string& name );
      virtual ~JetObjectCollectionMaker(){}
      
      // Delete Standard constructors
      JetObjectCollectionMaker(const JetObjectCollectionMaker& rhs) = delete;
      JetObjectCollectionMaker(JetObjectCollectionMaker&& rhs) = delete;
      JetObjectCollectionMaker& operator=(const JetObjectCollectionMaker& rhs) = delete;
      
      StatusCode initialize(); 
      
      StatusCode executeJets();
      StatusCode executeLargeRJets();
      StatusCode executeTrackJets();
      
      StatusCode printoutJets();
      StatusCode printoutLargeRJets();
             
      // return specific Systematic
      inline virtual const std::list<CP::SystematicSet>& specifiedSystematics()   const {return m_specifiedSystematics;}
      inline virtual const std::list<CP::SystematicSet>& specifiedSystematicsLargeR()   const {return m_specifiedSystematicsLargeR;}
  
      // return all recommendedSystematics
      inline const std::list<CP::SystematicSet>& recommendedSystematics()   const {return m_recommendedSystematics;}
      
    protected:
      // specify Systematic
      virtual void specifiedSystematics( const std::set<std::string>& specifiedSystematics , const ToolHandle<ICPJetUncertaintiesTool>& tool , std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map , const std::string& modName , bool isLargeR = false); 
      virtual void specifiedSystematics( const std::set<std::string>& specifiedSystematics , const ToolHandle<IJERSmearingTool>& tool , std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map );      
      
      StatusCode execute( const bool isLargeR );
      
      StatusCode calibrate( const bool isLargeR );
      virtual StatusCode applySystematic( ToolHandle<ICPJetUncertaintiesTool>& tool, const std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map , bool isLargeR = false);
      virtual StatusCode applySystematic( ToolHandle<IJERSmearingTool>& tool, const std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map );
      
      StatusCode printout( const bool isLargeR );

      void addCorrelation(const std::string& name,
                          systMap& map_one,
                          const std::string& syst_one_name,
                          systMap& map_two,
                          const std::string& syst_two_name);

      void addCorrelation(const std::string& name,
                          systMap& map_one,
                          const std::string& syst_one_name,
                          std::list<CP::SystematicSet>& jet_specified);
                          
      
    private: 
      std::shared_ptr<top::TopConfig> m_config;
      bool m_doJER;
      bool m_doFull_JER;

      bool m_do_fjvt = false;
           
      std::list<CP::SystematicSet> m_specifiedSystematics;  
      std::list<CP::SystematicSet> m_specifiedSystematicsLargeR;
      std::list<CP::SystematicSet> m_specifiedSystematicsTrackJets;
      std::list<CP::SystematicSet> m_recommendedSystematics;
      CP::SystematicSet m_nominalSystematicSet;
          
      ToolHandle<IJetCalibrationTool> m_jetCalibrationTool;
      ToolHandle<IJetCalibrationTool> m_jetCalibrationToolLargeR;  
      
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesTool;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario1;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario2;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario3;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario4;

      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_strong;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_medium;
      ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_weak;
     
      ToolHandle<IJERSmearingTool> m_jetJERSmearingTool;
      ToolHandle<IJetUpdateJvt> m_jetUpdateJvtTool;
      ToolHandle<IJetModifier> m_fjvtTool;
      
      std::string m_truthJetCollForHS;

      std::unique_ptr<top::TopJetSubstructure> m_jetSubstructure;
      
      systMap m_systMap_AllNP;
      systMap m_systMap_ReducedNPScenario1;
      systMap m_systMap_ReducedNPScenario2;
      systMap m_systMap_ReducedNPScenario3;
      systMap m_systMap_ReducedNPScenario4;
      systMap m_systMap_LargeR_strong;
      systMap m_systMap_LargeR_medium;
      systMap m_systMap_LargeR_weak;
      systMap m_systMap_JER;
      typedef std::unordered_map<CP::SystematicSet,CP::SystematicSet>::const_iterator Itr;

      StatusCode decorateBJets(xAOD::Jet& jet);
      StatusCode decorateHSJets();
  };
} // namespace
#endif
