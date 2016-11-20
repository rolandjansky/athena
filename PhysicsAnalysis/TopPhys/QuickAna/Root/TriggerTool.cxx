/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <QuickAna/TriggerTool.h>

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/InternalConfiguration.h>

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/multi_index/random_access_index.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop
// Suppressing unused parameter warning (ugh..)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <TrigDecisionTool/FeatureContainer.h>
#pragma GCC diagnostic pop

#include <TrigConfxAOD/xAODConfigTool.h>
#include <TriggerMatchingTool/MatchingTool.h>
// Trigger scale factors
#include <MuonEfficiencyCorrections/MuonTriggerScaleFactors.h>
#include <ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h>
//
// method implementations
//

namespace ana
{

  TriggerTool ::
  TriggerTool (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_isEleToolSetup (false),
      m_trigDecTool ("TrigDecisionTool", nullptr),
      m_trigconfigTool ("xAODConfigTool", nullptr),
      m_triggerMatching ("ana_TriggerMatching", nullptr),
      m_mu_trig_sf2015 ("ana_MuonTriggerScaleFactors2015", nullptr),
      m_mu_trig_sf2016 ("ana_MuonTriggerScaleFactors2016", nullptr),
      m_el_trig_sf ("ana_AsgElectronEfficiencyCorrectionTool", this),
      m_el_trig_eff ("ana_AsgElectronEfficiencyTool", this)
  {
    declareProperty("TriggerList", m_trigList="");
    declareProperty("GroupName", m_groupName="");
    declareProperty("MatchEl", m_matchEl=true);
    declareProperty("MatchMu", m_matchMu=true);
    declareProperty("MuonSFstring", m_muon_trig_str="");
    declareProperty("IsData", m_isData=false);
    declareProperty("IsAF2", m_isAF2=false);
    declareProperty("TriggerPtThreshold", m_TriggerPtThreshold=0);
    declareProperty("TriggerYear", m_year=Only2015_2016);
  }


  StatusCode TriggerTool ::
  useInitialConfiguration (const InternalConfiguration& conf)
  {
    ATH_CHECK (AnaTool::useInitialConfiguration (conf));

    // Set to Loose, Medium, or Tight
    m_muon_wp = conf.muonWP();
    m_muon_iso_wp = conf.muonIsolationWP();
    // Set to LooseLH, MediumLH, or TightLH
    m_ele_wp = conf.electronWP();
    m_ele_iso_wp = conf.electronIsolationWP();
    // From SUSYTools -- GradientLoose not fully supported, but Gradient is close
    if ("GradientLoose"==m_ele_iso_wp) m_ele_iso_wp="Gradient";
    return StatusCode::SUCCESS;
  }



  StatusCode TriggerTool ::
  initialize()
  {
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_trigconfigTool, TrigConf::xAODConfigTool));
    ATH_CHECK (m_trigconfigTool.initialize());

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_trigDecTool, Trig::TrigDecisionTool));
//    ATH_CHECK (m_trigDecTool.make ("Trig::TrigDecisionTool"));
    ATH_CHECK (m_trigDecTool.setProperty ("ConfigTool", m_trigconfigTool));
    ATH_CHECK (m_trigDecTool.setProperty ("TrigDecisionKey", "xTrigDecision"));
    ATH_CHECK (m_trigDecTool.initialize ());

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_triggerMatching, Trig::MatchingTool));
//    ATH_CHECK( m_triggerMatching.make("Trig::MatchingTool"));
    ATH_CHECK( m_triggerMatching.setProperty ("TrigDecisionTool", m_trigDecTool) );
    ATH_CHECK( m_triggerMatching.initialize() );

    // Set up the muon trigger scale factor tools - separate tools for each year
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_mu_trig_sf2015, CP::MuonTriggerScaleFactors));
    ATH_CHECK (m_mu_trig_sf2015.setProperty("MuonQuality", m_muon_wp));
    ATH_CHECK (m_mu_trig_sf2015.setProperty("Isolation", m_muon_iso_wp));
    ATH_CHECK (m_mu_trig_sf2015.setProperty("Year", "2015"));
    ATH_CHECK (m_mu_trig_sf2015.initialize());
    registerTool(&*m_mu_trig_sf2015);

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_mu_trig_sf2016, CP::MuonTriggerScaleFactors));
    ATH_CHECK (m_mu_trig_sf2016.setProperty("MuonQuality", m_muon_wp));
    ATH_CHECK (m_mu_trig_sf2016.setProperty("Isolation", m_muon_iso_wp));
    ATH_CHECK (m_mu_trig_sf2016.initialize());
    registerTool(&*m_mu_trig_sf2016);

    std::vector<std::string> elFiles,elEffFiles;
    std::string effNonIsoSuffix = "OOPS_STRING_NOT_SET";
    std::string effIsoSuffix = "OOPS_STRING_NOT_SET";

    // Thanks to Moritz (SUSY trigger contact): using same SFs for AF2 and full sim for the time being
    // Trying to clean up the code below by defining some filename stubs here
    const std::string effPrefix = "ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v3/trigger/efficiencySF.";
    //                             01234567890123456789012345678901234567890123456789012345678901234567890123456789012345
    //                             0         1         2         3         4         5         6         7         8  
    // We use an updated TightLLH (v05) for improved high-ET electron efficiency
    effNonIsoSuffix = m_ele_wp + "_d0z0_v11.root";
    effIsoSuffix = m_ele_wp + "_d0z0_v11_isol" + m_ele_iso_wp + ".root";

    // Decide whether we use the isolation-based SFs or the non-iso SFs.
    std::string effSuffix = effNonIsoSuffix;
    if ( (m_ele_wp == "TightLLH" &&
          (m_ele_iso_wp == "Tight" ||
           m_ele_iso_wp == "Gradient" ||
           m_ele_iso_wp == "Loose" ||
           m_ele_iso_wp == "LooseTrackOnly")) ||
         (m_ele_wp == "MediumLLH" &&
          (m_ele_iso_wp == "Gradient" ||
           m_ele_iso_wp == "Loose" ||
           m_ele_iso_wp == "LooseTrackOnly")) ||
         (m_ele_wp == "LooseAndBLayerLLH" &&
          (m_ele_iso_wp == "FixedCutLoose" ||
           m_ele_iso_wp == "LooseTrackOnly")) )
    {
      effSuffix = effIsoSuffix;
    }

    // Finally, fill the SF file list
    if (m_trigList.find("e24_lhmedium_L1EM20VH") != std::string::npos ||
        m_trigList.find("e60_lhmedium") != std::string::npos || // already matches e60_lhmedium_nod0; for post-ICHEP this should only be nod0
        m_trigList.find("e120_lhloose") != std::string::npos ||
        m_trigList.find("e24_lhtight_nod0_ivarloose") != std::string::npos ||
        m_trigList.find("e140_lhloose_nod0") != std::string::npos )
    {
      elFiles.push_back( effPrefix + 
                         "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0." + 
                         effSuffix );
    }
    else if (m_trigList.find("e12_lhloose_L1EM10VH") != std::string::npos ||
             m_trigList.find("e17_lhvloose_nod0") != std::string::npos)
      elFiles.push_back( effPrefix + "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0." + effSuffix );
    else if (m_trigList.find("e15_lhvloose_nod0_L1EM13VH") != std::string::npos)
      elFiles.push_back( effPrefix + "DI_E_2015_e12_lhloose_L1EM10VH_2016_e15_lhvloose_nod0_L1EM13VH." + effSuffix );
    else if (m_trigList.find("e17_lhloose"))
      elFiles.push_back( effPrefix + "DI_E_2015_e17_lhloose_2016_e17_lhloose." + effSuffix );
    // QA only has single- and di-electron trigger tools, so only supporting those for the time being
    /* For the very brave, these are also available:
      * TRI_E_2015_e17_lhloose_2016_e17_lhloose_nod0
      * TRI_E_2015_e17_lhloose_2016_e17_lhmedium_nod0
      * TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod
      * TRI_E_2015_e9_lhloose_2016_e9_lhmedium_nod0
      * MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0
      * MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0
    */
    elEffFiles.push_back( elFiles[0].replace(83,2,"") ); // efficiencySF -> efficiency


    if (elFiles.size())
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_el_trig_sf, AsgElectronEfficiencyCorrectionTool));
      ATH_CHECK (m_el_trig_sf.setProperty("CorrectionFileNameList", elFiles) );
      ATH_CHECK (m_el_trig_sf.setProperty("ForceDataType",1+(m_isAF2?2:0)) );
      ATH_CHECK (m_el_trig_sf.setProperty ("CorrelationModel", "TOTAL") );
      ATH_CHECK (m_el_trig_sf.initialize() );
      registerTool(&*m_el_trig_sf);

      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_el_trig_eff, AsgElectronEfficiencyCorrectionTool));
      ATH_CHECK (m_el_trig_eff.setProperty("CorrectionFileNameList", elEffFiles) );
      ATH_CHECK (m_el_trig_eff.setProperty("ForceDataType",1+(m_isAF2?2:0)) );
      ATH_CHECK (m_el_trig_eff.initialize() );

      m_isEleToolSetup=true;
    }

    return StatusCode::SUCCESS;
  }


  // Why is this method defined at all?
  StatusCode TriggerTool ::
  setObjectType (ObjectType /*type*/, const std::string& /*workingPoint*/)
  {
    //    m_type=OBJECT_ELECTRON;
    //    if (type != OBJECT_TRIGGER)
    return StatusCode::SUCCESS;
  }


  AnalysisStep TriggerTool ::
  step () const
  {
    return STEP_TRIGGER;
  }

  unsigned TriggerTool ::
  inputTypes () const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON) | (1 << OBJECT_EVENTINFO);
  }



  unsigned TriggerTool ::
  outputTypes () const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON) | (1 << OBJECT_EVENTINFO);
  }




  StatusCode TriggerTool ::
  execute (IEventObjects& objects)
  {
    std::vector<const xAOD::IParticle*> trig_part;
    std::vector<std::string> trig_names;
    boost::algorithm::split(trig_names, m_trigList, boost::is_any_of("\t "));
    std::map<std::string, std::string> names;
    const std::string group_pass = m_groupName+"_passTrig";
    objects.eventinfo()->auxdata< bool >(group_pass)=false;
    bool any_passed = false;

    for (auto trig : trig_names)
    {
      if (!trig.empty()){
        bool trig_dec = m_trigDecTool->isPassed(trig);
        any_passed = any_passed || trig_dec;
        // OR of all triggers in group
        objects.eventinfo()->auxdata<bool>(group_pass)
            = objects.eventinfo()->auxdata<bool>(group_pass) || trig_dec;
        // OR of all triggers
        objects.eventinfo()->auxdata<bool>("passAllTrig")
            = objects.eventinfo()->auxdata<bool>("passAllTrig") || trig_dec;
      }
      else continue;

      if (m_matchMu) {
        if (objects.muons()) {
          for (auto m : *objects.muons()){
            // TODO: comment
            trig_part.push_back(m);
            m->auxdata<bool>(m_groupName+"_trigMatch")
                = m->auxdata<bool>(m_groupName+"_trigMatch") || isTrigMatched(trig_part, trig);
            trig_part.clear();
          }
        }
      }
      if(m_matchEl) {
        if (objects.electrons()) {
          for (auto e : *objects.electrons()){
            trig_part.push_back(e);

            e->auxdata<bool>(m_groupName+"_trigMatch")
                = e->auxdata<bool>(m_groupName+"_trigMatch") || isTrigMatched(trig_part,trig);
            trig_part.clear();
          }
        }
      }
    }

    // Remainder all applies to scale factors - not applicable if this is data
    if (m_isData || !any_passed) return StatusCode::SUCCESS;

    //again brute forcing something that will get from configuration
    double sf=1;
    if (!m_muon_trig_str.empty()){

      // Configure the tools.
      // @TODO Need this to be the run number determined by the mu reweighting tool!
      //  Returns a CP correction code
      int my_runNumber = objects.eventinfo()->auxdata<unsigned int>( "RandomRunNumber" );

      // In the case of small amounts of data, the pileup reweighting tool may
      // return a zero run number. This is not really an error, so we don't need
      // to throw an exception here, but we do need to make sure that we get
      // reasonable muon trigger scale factors. So we patch the run number here.
      if (my_runNumber<=0) my_runNumber = 284285;
      // Note that this run number used to be 276073 (first good run in 2015), but
      // that run has rather atypical scale factors and performance.  It has
      // therefre been changed to the run in 2015 with the most luminosity.

      // If we are in the wrong year, then stop here
      if ( (my_runNumber<290000 && m_year==Only2016) ||
           (my_runNumber>290000 && m_year==Only2015) ) return StatusCode::SUCCESS;

      if ( muonSFToolForThisYear(my_runNumber)->setRunNumber(my_runNumber)!=CP::CorrectionCode::Ok ){
        ATH_MSG_ERROR ("setRunNumber failed for muon trigger SF tool");
        return StatusCode::FAILURE;
      }

      // This is unreadably ugly...
      bool isDiMuTrig = (m_muon_trig_str.find("2mu") != std::string::npos || m_muon_trig_str.find("mu", m_muon_trig_str.find("mu")+1 ) != std::string::npos);
      // For the time being: if the trigger string has "OR" then it is still a single-lepton trigger, it's just several OR'd together
      if(m_muon_trig_str.find("OR") != std::string::npos) isDiMuTrig=false;

      // Should only pass selected muons!  See
      //  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15#Muon_trigger_efficiency_scale_fa
      xAOD::MuonContainer my_muons(SG::VIEW_ELEMENTS);
      if (objects.muons()){
        for (const auto m : *objects.muons()){
          if ( m->auxdata<char>("ana_select_or") &&
               !m->auxdata<char>("overlaps") &&
               m->pt() > m_TriggerPtThreshold*1000. ){
            my_muons.push_back( m ); // 1.05*online pt requirement
            //// As long as we are here, get the efficiency
            double trig_eff = 1.;
            QA_CHECK_WEIGHT
              ( double , trig_eff, muonSFToolForThisYear(my_runNumber)->getTriggerEfficiency(*m,trig_eff,m_muon_trig_str,m_isData) );
            m->auxdata<double>(m_groupName + "_Mu_TrigEff") = trig_eff;
          }
        }
      }

      auto evtInfo = objects.eventinfo();
      if (!isDiMuTrig && my_muons.size()>=1){
        // Single muon trigger case - simple, scale factor for all the muons
        QA_CHECK_WEIGHT
          ( double, sf, muonSFToolForThisYear(my_runNumber)->getTriggerScaleFactor(my_muons, sf, m_muon_trig_str) );
        evtInfo->auxdata<double>(m_groupName + "_Mu_TrigSF") = sf;
      } else if (isDiMuTrig && my_muons.size() == 2 ) {
        // Di-muon case - slightly more complicated
        if( TString(m_muon_trig_str).Contains("2mu14") ){ // set SF=1 for 2mu14 since the SFs are bogus
          sf = 1;
        }
        else{
          QA_CHECK_WEIGHT
            ( double, sf, muonSFToolForThisYear(my_runNumber)->getTriggerScaleFactor(my_muons, sf, m_muon_trig_str) );
        }
        evtInfo->auxdata<double>(m_groupName + "_Mu_TrigSF") = sf;
      } else {
        // Otherwise - dimuon trigger, but we didn't have two muons
        evtInfo->auxdata<double>(m_groupName + "_Mu_TrigSF") = sf;
      }

    } // Had a muon trigger match string

    // Electron trigger scale factors as well!
    if (objects.electrons() && m_isEleToolSetup) {
      for (auto el : *objects.electrons()){
        // Decorate with the three scale factors
        double trig_weight = 1.;
        // \todo We're not catching errors!!!
        if (m_el_trig_sf->getEfficiencyScaleFactor(*el, trig_weight)
            == CP::CorrectionCode::Ok) {
          el->auxdata<double>(m_groupName+"_TrigSF") = trig_weight;
        }
        else el->auxdata<double>(m_groupName+"_TrigSF") = 1.;
        // Same thing for the efficiency
        double trig_eff = 1.;
        if (m_el_trig_eff->getEfficiencyScaleFactor(*el, trig_eff)
            == CP::CorrectionCode::Ok) {
          el->auxdata<double>(m_groupName+"_TrigEff") = trig_eff;
        }
        else el->auxdata<double>(m_groupName+"_TrigEff") = 1.;
      } // Loop over electrons
    } // Had electrons in the event

    return StatusCode::SUCCESS;
  }


 
  bool TriggerTool::isTrigMatched (std::vector<const xAOD::IParticle*> myParticles, std::string trigger)
  {
    return m_triggerMatching->match(myParticles,trigger);
  }

  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>& TriggerTool::muonSFToolForThisYear(const int runNumber)
  {
    // Was it 2016?
    if (runNumber>290000) return m_mu_trig_sf2016;
    // It was 2015 then
    return m_mu_trig_sf2015;
  }


  StatusCode makeTriggerTool (DefinitionArgs& args,
                              std::string trigList,
                              std::string groupName,
                              std::string MuonSFstring ,
                              bool matchEl,
                              bool matchMu,
                              double trigPtThresh=0,
                              TriggerTool::TrigYear year = TriggerTool::Only2015_2016)
  {
    using namespace msgObjectDefinition;

    std::unique_ptr<TriggerTool> TrigTool
      ( new TriggerTool(args.prefix()) );

    ANA_CHECK( TrigTool->setProperty("TriggerList", trigList) );
    ANA_CHECK( TrigTool->setProperty("GroupName", groupName) );
    ANA_CHECK( TrigTool->setProperty("MatchMu", matchMu) );
    ANA_CHECK( TrigTool->setProperty("MatchEl", matchEl) );
    ANA_CHECK( TrigTool->setProperty("MuonSFstring", MuonSFstring) );
    ANA_CHECK( TrigTool->setProperty("IsData",args.configuration()->isData()) );
    ANA_CHECK( TrigTool->setProperty("IsAF2",args.configuration()->isAFII()) );
    trigPtThresh *= 1.05;//Muon trigger tool requires offline pt = 1.05 * online
    ANA_CHECK( TrigTool->setProperty("TriggerPtThreshold", trigPtThresh) );
    ANA_CHECK( TrigTool->setProperty("TriggerYear", static_cast<int>(year)) );
    args.add( std::move(TrigTool) );
    return StatusCode::SUCCESS;
  }

  // Format
  //Definition Name, makeTriggerTool(args, Triggers separated by spaces, group name, match electrons, match muon)

  // Some Combined Trigger groups
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("SingleMu",
    makeTriggerTool (args, "HLT_mu20_iloose_L1MU15", "SingleMu", "HLT_mu20_iloose_L1MU15", false, true,20,TriggerTool::Only2015))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("SingleE",
    makeTriggerTool (args, "HLT_e24_lhmedium_iloose_L1EM20VH HLT_e24_tight_iloose", "SingleE", "", true, false))

  // Some individual Trigger groups
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_iloose_L1MU15",
    makeTriggerTool (args, "HLT_mu20_iloose_L1MU15", "HLT_mu20_iloose_L1MU15", "HLT_mu20_iloose_L1MU15", false, true,20,TriggerTool::Only2015))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_iloose_L1EM20VH",
    makeTriggerTool (args, "HLT_e24_lhmedium_iloose_L1EM20VH", "HLT_e24_lhmedium_iloose_L1EM20VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_tight_iloose",
    makeTriggerTool (args, "HLT_e24_tight_iloose", "HLT_e24_tight_iloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e26_lhtight_nod0_ivarloose",
    makeTriggerTool (args, "HLT_e26_lhtight_nod0_ivarloose", "HLT_e26_lhtight_nod0_ivarloose", "", true, false))

  // single electron 2016 --> disable the sf for the moment
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhtight_nod0_ivarloose",
    makeTriggerTool (args, "HLT_e24_lhtight_nod0_ivarloose", "HLT_e24_lhtight_nod0_ivarloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e60_medium",
    makeTriggerTool (args, "HLT_e60_medium", "HLT_e60_medium", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e140_lhloose_nod0",
    makeTriggerTool (args, "HLT_e140_lhloose_nod0", "HLT_e140_lhloose_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e300_etcut",
    makeTriggerTool (args, "HLT_e300_etcut", "HLT_e300_etcut", "", true, false))
  //for special run -->probably to be removed in future
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_nod0_ivarloose",
    makeTriggerTool (args, "HLT_e24_lhmedium_nod0_ivarloose", "HLT_e24_lhmedium_nod0_ivarloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_nod0_L1EM20VH",
    makeTriggerTool (args, "HLT_e24_lhmedium_nod0_L1EM20VH", "HLT_e24_lhmedium_nod0_L1EM20VH", "", true, false))

  //Combined Single Mu --> due to combined scale factors
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu26_imedium",
    makeTriggerTool (args, "HLT_mu26_imedium", "HLT_mu26_imedium", "HLT_mu26_imedium_OR_HLT_mu50", false, true, 26, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu26_ivarmedium",
    makeTriggerTool (args, "HLT_mu26_ivarmedium", "HLT_mu26_ivarmedium", "HLT_mu26_imedium_OR_HLT_mu50", false, true, 26, TriggerTool::Only2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu50",
    makeTriggerTool (args, "HLT_mu50", "HLT_mu50", "HLT_mu50", false, true, 50, TriggerTool::Only2015_2016))

  // single muon 2016 --> disable the sf for the moment
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_ivarloose",
    makeTriggerTool (args, "HLT_mu24_ivarloose", "HLT_mu24_ivarloose", "", false, true, 24, TriggerTool::Only2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_iloose",
    makeTriggerTool (args, "HLT_mu24_iloose", "HLT_mu24_iloose", "", false, true, 24, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_ivarloose_L1MU15",
    makeTriggerTool (args, "HLT_mu24_ivarloose_L1MU15", "HLT_mu24_ivarloose_L1MU15", "", false, true, 24, TriggerTool::Only2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_iloose_L1MU15",
    makeTriggerTool (args, "HLT_mu24_iloose_L1MU15", "HLT_mu24_iloose_L1MU15", "", false, true, 24, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_ivarmedium",
    makeTriggerTool (args, "HLT_mu24_ivarmedium", "HLT_mu24_ivarmedium", "", false, true, 24, TriggerTool::Only2016))


  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_iloose_L1MU15_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu20_iloose_L1MU15 HLT_mu50", "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", false, true, 20, TriggerTool::Only2015))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_iloose_L1MU15_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu24_iloose_L1MU15 HLT_mu50", "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", false, true, 24, TriggerTool::Only2015))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_imedium_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu24_imedium HLT_mu50", "HLT_mu24_imedium_OR_HLT_mu50", "HLT_mu24_imedium_OR_HLT_mu50", false, true, 24, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu26_imedium_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu26_imedium HLT_mu50", "HLT_mu26_imedium_OR_HLT_mu50", "HLT_mu26_imedium_OR_HLT_mu50", false, true, 26, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_ivarmedium_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu24_ivarmedium HLT_mu50", "HLT_mu24_ivarmedium_OR_HLT_mu50", "HLT_mu24_ivarmedium_OR_HLT_mu50", false, true, 24, TriggerTool::Only2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu26_ivarmedium_OR_HLT_mu50",
    makeTriggerTool (args, "HLT_mu26_ivarmedium HLT_mu50", "HLT_mu26_ivarmedium_OR_HLT_mu50", "HLT_mu26_ivarmedium_OR_HLT_mu50", false, true, 26, TriggerTool::Only2016))

  // Stand alone muon triggers
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2mu14",
    makeTriggerTool (args, "HLT_2mu14", "HLT_2mu14", "HLT_2mu14", false, true,14, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu14",
    makeTriggerTool (args, "HLT_mu14", "HLT_mu14", "HLT_mu14", false, true,14, TriggerTool::Only2015_2016))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2mu10",
    makeTriggerTool (args, "HLT_2mu10", "HLT_2mu10", "", false, true,10, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu10",
    makeTriggerTool (args, "HLT_mu10", "HLT_mu10", "", false, true,10, TriggerTool::Only2015_2016))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24_mu8noL1",
    makeTriggerTool (args, "HLT_mu24_mu8noL1", "HLT_mu24_mu8noL1", "", false, true,8, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu24",
    makeTriggerTool (args, "HLT_mu24", "HLT_mu24", "", false, true,24))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu22_mu8noL1",
    makeTriggerTool (args, "HLT_mu22_mu8noL1", "HLT_mu22_mu8noL1", "", false, true,8, TriggerTool::Only2015_2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu22",
    makeTriggerTool (args, "HLT_mu22", "HLT_mu22", "", false, true,22))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_mu8noL1",
    makeTriggerTool (args, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1", "", false, true,8, TriggerTool::Only2016))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20",
    makeTriggerTool (args, "HLT_mu20", "HLT_mu20", "", false, true,20))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu18_mu8noL1",
    makeTriggerTool (args, "HLT_mu18_mu8noL1", "HLT_mu18_mu8noL1", "", false, true,8, TriggerTool::Only2015))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu18",
    makeTriggerTool (args, "HLT_mu18", "HLT_mu18", "", false, true,18))

  // 2016 additions
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2mu14_nomucomb",
    makeTriggerTool (args, "HLT_2mu14_nomucomb", "HLT_2mu14_nomucomb", "", false, true,14))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu40",
    makeTriggerTool (args, "HLT_mu40", "HLT_mu40", "", false, true,40))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2mu10_nomucomb",
    makeTriggerTool (args, "HLT_2mu10_nomucomb", "HLT_2mu10_nomucomb", "", false, true, 10))

  // electrons
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_iloose_L1EM18VH",
    makeTriggerTool (args, "HLT_e24_lhmedium_iloose_L1EM18VH", "HLT_e24_lhmedium_iloose_L1EM18VH", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e60_lhmedium",
    makeTriggerTool (args, "HLT_e60_lhmedium", "HLT_e60_lhmedium", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e12_lhloose",
    makeTriggerTool (args, "HLT_2e12_lhloose", "HLT_2e12_lhloose", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e17_lhloose",
    makeTriggerTool (args, "HLT_2e17_lhloose", "HLT_2e17_lhloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose",
    makeTriggerTool (args, "HLT_e17_lhloose", "HLT_e17_lhloose", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e12_lhloose_L12EM10VH",
    makeTriggerTool (args, "HLT_2e12_lhloose_L12EM10VH", "HLT_2e12_lhloose_L12EM10VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e12_lhloose_L1EM10VH",
    makeTriggerTool (args, "HLT_e12_lhloose_L1EM10VH", "HLT_e12_lhloose_L1EM10VH", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e15_lhloose_L12EM13VH",
    makeTriggerTool (args, "HLT_2e15_lhloose_L12EM13VH", "HLT_2e15_lhloose_L12EM13VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e15_lhloose_L1EM13VH",
    makeTriggerTool (args, "HLT_e15_lhloose_L1EM13VH", "HLT_e15_lhloose_L1EM13VH", "", true, false))

  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_L1EM20VH",
    makeTriggerTool (args, "HLT_e24_lhmedium_L1EM20VH", "HLT_e24_lhmedium_L1EM20VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_L1EM20VHI",
    makeTriggerTool (args, "HLT_e24_lhmedium_L1EM20VHI", "HLT_e24_lhmedium_L1EM20VHI", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_L1EM18VH",
    makeTriggerTool (args, "HLT_e24_lhmedium_L1EM18VH", "HLT_e24_lhmedium_L1EM18VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e120_lhloose",
    makeTriggerTool (args, "HLT_e120_lhloose", "HLT_e120_lhloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e7_lhmedium",
    makeTriggerTool (args, "HLT_e7_lhmedium", "HLT_e7_lhmedium", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e26_lhmedium_L1EM22VHI",
    makeTriggerTool (args, "HLT_e26_lhmedium_L1EM22VHI", "HLT_e26_lhmedium_L1EM22VHI", "", true, false))

  // New for 2016
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e60_lhmedium_nod0",
    makeTriggerTool (args, "HLT_e60_lhmedium_nod0", "HLT_e60_lhmedium_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e17_lhvloose_nod0",
    makeTriggerTool (args, "HLT_2e17_lhvloose_nod0", "HLT_2e17_lhvloose_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e12_lhvloose_nod0_L12EM10VH",
    makeTriggerTool (args, "HLT_2e12_lhvloose_nod0_L12EM10VH", "HLT_2e12_lhvloose_nod0_L12EM10VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e15_lhvloose_nod0_L12EM13VH",
    makeTriggerTool (args, "HLT_2e15_lhvloose_nod0_L12EM13VH", "HLT_2e15_lhvloose_nod0_L12EM13VH", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose_nod0",
    makeTriggerTool (args, "HLT_e17_lhloose_nod0", "HLT_e17_lhloose_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e26_lhtight_nod0_iloose",
    makeTriggerTool (args, "HLT_e26_lhtight_nod0_iloose", "HLT_e26_lhtight_nod0_iloose", "", true, false))

  // electron-muon
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose_mu14",
    makeTriggerTool (args, "HLT_e17_lhloose_mu14", "HLT_e17_lhloose_mu14", "", true, true,14))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e7_lhmedium_mu24",
    makeTriggerTool (args, "HLT_e7_lhmedium_mu24", "HLT_e7_lhmedium_mu24", "HLT_mu24", true, true,24))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e26_lhmedium_L1EM22VHI_mu8noL1",
    makeTriggerTool (args, "HLT_e26_lhmedium_L1EM22VHI_mu8noL1", "HLT_e26_lhmedium_L1EM22VHI_mu8noL1", "", true, true,8))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_L1EM20VHI_mu8noL1",
    makeTriggerTool (args, "HLT_e24_lhmedium_L1EM20VHI_mu8noL1", "HLT_e24_lhmedium_L1EM20VHI_mu8noL1", "", true, true,8))

  // New for 2016
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1",
    makeTriggerTool (args, "HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1", "HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1", "", true, true,8))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e7_lhmedium_nod0_mu24",
    makeTriggerTool (args, "HLT_e7_lhmedium_nod0_mu24", "HLT_e7_lhmedium_nod0_mu24", "HLT_mu24", true, true,24))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose_nod0_mu14",
    makeTriggerTool (args, "HLT_e17_lhloose_nod0_mu14", "HLT_e17_lhloose_nod0_mu14", "", true, true,14))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1",
    makeTriggerTool (args, "HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1", "HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1", "", true, true, 8))

  // MET Triggers
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("L1_XE50",
    makeTriggerTool (args, "L1_XE50", "L1_XE50", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe70",
    makeTriggerTool (args, "HLT_xe70", "HLT_xe70", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe80",
    makeTriggerTool (args, "HLT_xe80", "HLT_xe80", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe80_wEFMu",
    makeTriggerTool (args, "HLT_xe80_wEFMu", "HLT_xe80_wEFMu", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe100",
    makeTriggerTool (args, "HLT_xe100", "HLT_xe100", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe100_mht_L1XE50",
    makeTriggerTool (args, "HLT_xe100_mht_L1XE50", "HLT_xe100_mht_L1XE50", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe110_mht_L1XE50",
    makeTriggerTool (args, "HLT_xe110_mht_L1XE50", "HLT_xe110_mht_L1XE50", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe110_mht_L1XE60",
    makeTriggerTool (args, "HLT_xe110_mht_L1XE60", "HLT_xe110_mht_L1XE60", "", false, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_xe110_mht_L1XE50_AND_xe70_L1XE50",
    makeTriggerTool (args, "HLT_xe110_mht_L1XE50_AND_xe70_L1XE50", "HLT_xe110_mht_L1XE50_AND_xe70_L1XE50", "", false, false))



  //specially for H4l analysis --> they use lots of combinations
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose_2e9_lhloose",
    makeTriggerTool (args, "HLT_e17_lhloose_2e9_lhloose", "HLT_e17_lhloose_2e9_lhloose", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhloose_nod0_2e9_lhloose_nod0",
    makeTriggerTool (args, "HLT_e17_lhloose_nod0_2e9_lhloose_nod0", "HLT_e17_lhloose_nod0_2e9_lhloose_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0",
    makeTriggerTool (args, "HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0", "HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0", "", true, false))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu60_0eta105_msonly",
    makeTriggerTool (args, "HLT_mu60_0eta105_msonly", "HLT_mu60_0eta105_msonly", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_ivarloose",
    makeTriggerTool (args, "HLT_mu20_ivarloose", "HLT_mu20_ivarloose", "",false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_iloose",
    makeTriggerTool (args, "HLT_mu20_iloose", "HLT_mu20_iloose", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_3mu6",
    makeTriggerTool (args, "HLT_3mu6", "HLT_3mu6", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_3mu6_msonly",
    makeTriggerTool (args, "HLT_3mu6_msonly", "HLT_3mu6_msonly", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu18_2mu4noL1",
    makeTriggerTool (args, "HLT_mu18_2mu4noL1", "HLT_mu18_2mu4noL1", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_2mu4noL1",
    makeTriggerTool (args, "HLT_mu20_2mu4noL1", "HLT_mu20_2mu4noL1", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_3mu4",
    makeTriggerTool (args, "HLT_3mu4", "HLT_3mu4", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu6_2mu4",
    makeTriggerTool (args, "HLT_mu6_2mu4", "HLT_mu6_2mu4", "", false, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e12_lhloose_mu10",
    makeTriggerTool (args, "HLT_2e12_lhloose_mu10", "HLT_2e12_lhloose_mu10", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e12_lhloose_2mu10",
    makeTriggerTool (args, "HLT_e12_lhloose_2mu10", "HLT_e12_lhloose_2mu10", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e24_medium_L1EM20VHI_mu8noL1",
    makeTriggerTool (args, "HLT_e24_medium_L1EM20VHI_mu8noL1", "HLT_e24_medium_L1EM20VHI_mu8noL1", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e7_medium_mu24",
    makeTriggerTool (args, "HLT_e7_medium_mu24", "HLT_e7_medium_mu24", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_e12_lhloose_nod0_2mu10",
    makeTriggerTool (args, "HLT_e12_lhloose_nod0_2mu10", "HLT_e12_lhloose_nod0_2mu10", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_2e12_lhloose_nod0_mu10",
    makeTriggerTool (args, "HLT_2e12_lhloose_nod0_mu10", "HLT_2e12_lhloose_nod0_mu10", "", true, true))
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("HLT_mu20_ivarloose_L1MU15",
    makeTriggerTool (args, "HLT_mu20_ivarloose_L1MU15", "HLT_mu20_ivarloose_L1MU15", "", false, true))



  // Default do nothing
  QUICK_ANA_TRIGGER_DEFINITION_MAKER ("skip", makeTriggerTool (args, "", "", "", false, false))
} // namespace ana
