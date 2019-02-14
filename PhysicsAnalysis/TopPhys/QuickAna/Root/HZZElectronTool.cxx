/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/HZZElectronTool.h>

#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/ElectronTool.h>
#include <QuickAna/MessageCheck.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include <xAODTracking/TrackParticlexAODHelpers.h>

//
// method implementations
//

namespace ana
{
  HZZElectronTool ::
  HZZElectronTool (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::ElectronContainer> (name),
      m_wp (WPType::_HZZ4l),
      m_isoStr ("FixedCutLoose"),
      m_selectionTool ("AsgElectronLikelihoodTool", this),
      m_isolationTool ("IsolationSelectionTool", this)
  {
    declareProperty("SelectionString", m_selection);
  }

  StatusCode HZZElectronTool ::
  initialize()
  {
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selectionTool, AsgElectronLikelihoodTool));
    ATH_CHECK (m_selectionTool.setProperty("primaryVertexContainer", "PrimaryVertices"));
    std::string config = "ElectronPhotonSelectorTools/offline/mc16_20170828/";
    if(m_selection == "VLooseLLH") {
      config += "ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf";
    }
    else if(m_selection == "LooseLLH") {
      config += "ElectronLikelihoodLooseOfflineConfig2017_Smooth.conf";
    }
    else if(m_selection == "LooseAndBLayerLLH") {
      config += "ElectronLikelihoodLooseOfflineConfig2017_CutBL_Smooth.conf";
    }
    else if(m_selection == "MediumLLH")
      config += "ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf";
    else if(m_selection == "TightLLH")
      config += "ElectronLikelihoodTightOfflineConfig2017_Smooth.conf";
    else ATH_MSG_ERROR( "Unsupported electron ID: " << m_selection );
    ATH_CHECK (m_selectionTool.setProperty ("ConfigFile", config));
    ATH_CHECK( m_selectionTool.initialize() );
    registerTool(&*m_selectionTool);

    if(m_wp != WPType::_HZZ4l) {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isolationTool, CP::IsolationSelectionTool));

      if(m_wp == WPType::_ZHinv) ATH_CHECK (m_isolationTool.setProperty("ElectronWP", m_isoStr));
      else ATH_CHECK (m_isolationTool.setProperty("ElectronWP", m_isoStr));

      ATH_CHECK (m_isolationTool.initialize());
      registerTool(&*m_isolationTool);
    }

    registerCut (SelectionStep::MET, "Pt", cut_Pt);
    registerCut (SelectionStep::MET, "Eta", cut_Eta);
    registerCut (SelectionStep::MET, "OQ", cut_OQ);
    registerCut (SelectionStep::MET, "selectionTool", cut_selectionTool);
    registerCut (SelectionStep::MET, "Z0", cut_Z0);

    if(m_wp != WPType::_HZZ4l && m_wp != WPType::_SMZZ4l) {
      registerCut (SelectionStep::MET, "Iso", cut_Iso);
      registerCut (SelectionStep::MET, "D0",  cut_D0);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode HZZElectronTool ::
  selectObject (xAOD::Electron& electron)
  {
    const xAOD::Vertex *pv(0);
    const xAOD::VertexContainer* vertices = 0;
    ANA_CHECK (evtStore()->retrieve(vertices, "PrimaryVertices"));
    for ( const auto* const vtx_itr : *vertices )
    {
      if (vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
      else {
        pv = vtx_itr;
        break;
      }
    }
    if(!pv) return StatusCode::SUCCESS;
    double pz0 = pv->z();

    cut_Eta.setPassedIf (std::fabs(electron.caloCluster()->etaBE(2)) < 2.47);

    cut_Pt.setPassedIf (electron.pt() > 7.e3);

    if(m_selection == "VLooseLLH") {
      bool passID = electron.isAvailable<char>("DFCommonElectronsLHVeryLoose") ?
                    static_cast<bool>(electron.auxdata<char>("DFCommonElectronsLHVeryLoose")) :
                    static_cast<bool>(m_selectionTool->accept(&electron));
      cut_selectionTool.setPassedIf ( passID );
    }
    else if(m_selection == "LooseLLH") {
      bool passID = electron.isAvailable<char>("DFCommonElectronsLHLoose") ?
                    static_cast<bool>(electron.auxdata<char>("DFCommonElectronsLHLoose")) :
                    static_cast<bool>(m_selectionTool->accept(&electron));
      cut_selectionTool.setPassedIf ( passID );
    }
    else if(m_selection == "LooseAndBLayerLLH") {
      bool passID = electron.isAvailable<char>("DFCommonElectronsLHLooseBL") ?
                    static_cast<bool>(electron.auxdata<char>("DFCommonElectronsLHLooseBL")) :
                    static_cast<bool>(m_selectionTool->accept(&electron));
      cut_selectionTool.setPassedIf ( passID );
    }
    else if(m_selection == "MediumLLH") {
      bool passID = electron.isAvailable<char>("DFCommonElectronsLHMedium") ?
                    static_cast<bool>(electron.auxdata<char>("DFCommonElectronsLHMedium")) :
                    static_cast<bool>(m_selectionTool->accept(&electron));
      cut_selectionTool.setPassedIf ( passID );
    }
    else if(m_selection == "TightLLH") {
      bool passID = electron.isAvailable<char>("DFCommonElectronsLHTight") ?
                    static_cast<bool>(electron.auxdata<char>("DFCommonElectronsLHTight")) :
                    static_cast<bool>(m_selectionTool->accept(&electron));
      cut_selectionTool.setPassedIf ( passID );
    }

    cut_OQ.setPassedIf( (bool) electron.isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) );

    double z0 = (electron.trackParticle())->z0()+(electron.trackParticle())->vz()-pz0;
    double d0 = (electron.trackParticle())->d0();
    double z0sin = z0*sin((electron.trackParticle())->theta());
    cut_Z0.setPassedIf (fabs(z0sin)<0.5);

    //electron.auxdata<char> ("ana_select_isolationTool") =
    //  (bool) m_isolationTool->accept(electron);
    
    if(m_wp == WPType::_SMZZ4l)
      electron.auxdecor<char>("PassIso") = (bool)m_isolationTool->accept(electron);


    const xAOD::EventInfo* evt = 0;
    ATH_CHECK( evtStore()->retrieve( evt, "EventInfo" ) );
    double d0sig = -999.;
    if(electron.trackParticle() && evt) {
      try
      {
        d0sig = xAOD::TrackingHelpers::d0significance( electron.trackParticle(),
                                                       evt->beamPosSigmaX(),
                                                       evt->beamPosSigmaY(),
                                                       evt->beamPosSigmaXY() );
      } catch (std::exception& e)
      {
        d0sig = -999.0;
        ATH_MSG_INFO ("WARNING of d0sig and assign a default value -999.0 : " << e.what());
      }
    }
    electron.auxdata<double>("d0Sig") = d0sig;
    electron.auxdata<double>("d0value") = d0;
    electron.auxdata<double>("z0value") = z0;

    if(m_wp != WPType::_HZZ4l && m_wp != WPType::_SMZZ4l) {
      cut_D0.setPassedIf (fabs(d0sig)<5.);
      cut_Iso.setPassedIf (m_isolationTool->accept(electron));
    }



    return StatusCode::SUCCESS;
  }


  StatusCode makeHZZElectronTool (ana::DefinitionArgs& args,
                                  const std::string& selection,
                                  int WP, const std::string& input_isoStr)
  {
    using namespace msgObjectDefinition;

    std::string my_idStr = "LooseAndBLayerLLH";
    if (WP == WPType::_ZHinv || WP == WPType::_Hmumu) my_idStr = "MediumLLH";

    std::string my_isoStr = input_isoStr;
//    std::string my_isoStr = "FixedCutLoose";
//    if(WP == WPType::_ZHinv) my_isoStr = "Loose";
//    else if(WP == WPType::_SMZZ4l) my_isoStr = "LooseTrackOnly";

    if (args.firstWP()){
      // Set up the appropriate global config option
      //  Have to swap around the LH string
      args.configuration()->setElectronWP (my_idStr);

      args.configuration()->setElectronIsolationWP (my_isoStr);
    }


    const std::string inputContainer = "Electrons";

    std::unique_ptr<IAnaTool> retrieveTool
      ( new AnaToolRetrieve(args.prefix() + "_retrieve", inputContainer) );
    args.add( std::move(retrieveTool) );

    std::unique_ptr<ElectronToolCorrect> correctTool
      ( new ElectronToolCorrect(args.prefix() + "_correct") );
    ANA_CHECK
      ( correctTool->setProperty("IsAF2", args.configuration()->isAFII()) );
    args.add( std::move(correctTool) );


    std::unique_ptr<HZZElectronTool> selectTool
      ( new HZZElectronTool(args.prefix() + "_select") );
    selectTool->m_wp = WP;
    selectTool->m_isoStr = my_isoStr;
    ANA_CHECK( selectTool->setProperty("SelectionString", selection) );
    args.add( std::move(selectTool) );

    if (args.configuration()->isData() == false)
    {
      std::unique_ptr<ElectronToolWeight> weightTool
      ( new ElectronToolWeight(args.prefix() + "_weight") );

      ANA_CHECK( weightTool->setProperty("IsoString", my_isoStr) );
      ANA_CHECK( weightTool->setProperty("IDString", my_idStr) );
      ANA_CHECK( weightTool->setProperty("IDAna", ""));
      ANA_CHECK( weightTool->setProperty("IsAF2",args.configuration()->isAFII()) );
      args.add( std::move(weightTool) );
    }
    return StatusCode::SUCCESS;

  }

  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("hzz4l", makeHZZElectronTool (args))
  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("smzz4l", makeHZZElectronTool (args, "LooseAndBLayerLLH", WPType::_SMZZ4l))
  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("smzz4l_veryloose", makeHZZElectronTool (args, "VLooseLLH", WPType::_SMZZ4l))
  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("hzhinv_loose", makeHZZElectronTool (args, "LooseLLH", WPType::_ZHinv, "FCLoose"))
  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("hzhinv_medium", makeHZZElectronTool (args, "MediumLLH", WPType::_ZHinv, "FCLoose"))
  QUICK_ANA_ELECTRON_DEFINITION_MAKER ("hmumu", makeHZZElectronTool (args, "MediumLLH", WPType::_Hmumu, "Loose"))
}
