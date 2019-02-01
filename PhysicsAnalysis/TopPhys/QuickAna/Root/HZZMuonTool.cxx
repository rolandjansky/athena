/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/HZZMuonTool.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include <MuonSelectorTools/MuonSelectionTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MuonTool.h>
#include <QuickAna/MessageCheck.h>

#include <xAODTracking/TrackParticle.h>
#include <xAODTracking/TrackParticlexAODHelpers.h>

//
// method implementations
//

namespace ana
{
  HZZMuonTool ::
  HZZMuonTool (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::MuonContainer> (name),
      m_quality (xAOD::Muon::Loose),
      m_wp (WPType::_HZZ4l),
      m_isoStr ("FixedCutLoose"),
      m_selection ("selection", this),
      m_isolationTool ("IsolationSelectionTool", this)
  {}

  StatusCode HZZMuonTool ::
  initialize()
  {
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selection, CP::MuonSelectionTool));
    ATH_CHECK (m_selection.setProperty ("MuQuality", int (m_quality)));
    if(m_wp == WPType::_ZHinv) ATH_CHECK (m_selection.setProperty ("MaxEta", 2.5));
    else if(m_wp == WPType::_Hmumu) ATH_CHECK (m_selection.setProperty ("MaxEta", 2.7));
    ATH_CHECK (m_selection.initialize());

    if(m_wp != WPType::_HZZ4l) {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isolationTool, CP::IsolationSelectionTool));
      ATH_CHECK (m_isolationTool.setProperty("MuonWP", m_isoStr));
      ATH_CHECK (m_isolationTool.initialize());
      registerTool(&*m_isolationTool);
    }

    registerCut (SelectionStep::MET, "ID", cut_ID);
    registerCut (SelectionStep::MET, "Pt", cut_Pt);
    registerCut (SelectionStep::MET, "D0", cut_D0);
    registerCut (SelectionStep::MET, "Z0", cut_Z0);

//    if(m_wp == WPType::_SMZZ4l) {
//      registerCut (SelectionStep::MET, "Iso", cut_Iso); 
//    }

    if(m_wp == WPType::_ZHinv || m_wp == WPType::_Hmumu) {
      registerCut (SelectionStep::MET, "CB", cut_CB);
      registerCut (SelectionStep::MET, "Eta", cut_Eta);
      registerCut (SelectionStep::MET, "Iso", cut_Iso);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode HZZMuonTool ::
  selectObject (xAOD::Muon& muon)
  {
    const xAOD::Vertex *pv(0);
    const xAOD::VertexContainer* vertices = 0;
    ANA_CHECK (evtStore()->retrieve(vertices, "PrimaryVertices"));
    for ( const auto* const vtx_itr : *vertices )
    {
      if (vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
      else { pv = vtx_itr; break;}
    }
    if(!pv) return StatusCode::SUCCESS;

    double pz0 = pv->z();

    int type  = muon.muonType();
    double pt = muon.pt();
    double eta= muon.eta();


    cut_ID.setPassedIf (m_selection->accept(muon));

    double d0=-999.0, d0sig=-999.0, z0=-999.0, z0sin=-999.0;
    if(muon.primaryTrackParticle()) {
      d0 = (muon.primaryTrackParticle())->d0();
      z0 = (muon.primaryTrackParticle())->z0()+(muon.primaryTrackParticle())->vz()-pz0;
      z0sin = z0*sin((muon.primaryTrackParticle())->theta());
    }

    if (muon.muonType() != xAOD::Muon::MuonStandAlone) {
      const xAOD::EventInfo* evt = 0;
      ATH_CHECK( evtStore()->retrieve( evt, "EventInfo" ) );
      try
      {
        d0sig = (muon.primaryTrackParticle() && evt) ? xAOD::TrackingHelpers::d0significance( muon.primaryTrackParticle() , evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY() ) : -999.0;
      } catch (std::exception& e)
      {
        d0sig = -999.0;
        ATH_MSG_INFO ("WARNING of d0sig and assign a default value -999.0 : " << e.what());
      }
    }
    muon.auxdata<double>("d0Sig") = d0sig;
    muon.auxdata<double>("z0sintheta") = z0sin;

    if(m_wp != WPType::_HZZ4l) {

      if (muon.muonType() != xAOD::Muon::MuonStandAlone) {
        cut_Z0.setPassedIf (fabs(z0sin)<0.5);

        if(m_wp == WPType::_SMZZ4l) {
          cut_D0.setPassedIf (fabs(d0)<1.);
        }else {
          cut_D0.setPassedIf (fabs(d0sig)<3.);
        }

      }else {
        cut_D0.setPassedIf ( true );
        cut_Z0.setPassedIf ( true );
      }

      if(m_wp == WPType::_SMZZ4l) {
        muon.auxdecor<char>("PassIso") = (bool)m_isolationTool->accept(muon);

        if(type==0 || type==2) {
          cut_Pt.setPassedIf (pt>6.e3);
        }

        if(type==1) {
          cut_Pt.setPassedIf (pt>6.e3);
        }

        if(type==3 && muon.author()==xAOD::Muon::CaloTag) {
           cut_Pt.setPassedIf (pt>15.e3);
        }
      }
      
      if(m_wp == WPType::_ZHinv) {
        cut_Iso.setPassedIf (m_isolationTool->accept(muon));
        cut_CB.setPassedIf (type==0);
        cut_Eta.setPassedIf (fabs(eta)<2.5);
        cut_Pt.setPassedIf (pt>7.e3);
      }

      if(m_wp == WPType::_Hmumu) {
        cut_Iso.setPassedIf (m_isolationTool->accept(muon));
        cut_CB.setPassedIf (true);
        cut_Eta.setPassedIf (fabs(eta)<2.7);
        cut_Pt.setPassedIf (pt>15.e3);
      }


      return StatusCode::SUCCESS;
    }
    
    if(type==0 || type==2) {
      cut_D0.setPassedIf (fabs(d0)<1.);
      cut_Z0.setPassedIf (fabs(z0sin)<0.5);
      cut_Pt.setPassedIf (pt>5.e3);
    }

    if(type==1) {
      cut_Pt.setPassedIf (pt>5.e3);
      cut_D0.setPassedIf (true);
      cut_Z0.setPassedIf (true);
    }

    if(type==3 && muon.author()==xAOD::Muon::CaloTag) {
      cut_Pt.setPassedIf (pt>15.e3);
      cut_D0.setPassedIf (fabs(d0)<1.);
      cut_Z0.setPassedIf (fabs(z0sin)<0.5);
    }



    return StatusCode::SUCCESS;
  }


  StatusCode makeHZZMuonTool (ana::DefinitionArgs& args,
                              const xAOD::Muon::Quality& quality,
                              int WP, const std::string& my_isoStr)
  {
    using namespace msgObjectDefinition;

    std::string my_idStr = "Loose";
    if      (quality==xAOD::Muon::Loose)  my_idStr = "Loose";
    else if (quality==xAOD::Muon::Medium) my_idStr = "Medium";
    else if (quality==xAOD::Muon::Tight)  my_idStr = "Tight";


    if (args.firstWP())
    {
      args.configuration()->setMuonIsolationWP (my_isoStr);
      if(WP == WPType::_ZHinv) {
        args.configuration()->setMuonWP ("Medium");
      }else {
        args.configuration()->setMuonWP (my_idStr);
      }
    }

    std::unique_ptr<IAnaTool> retrieveTool
      (new AnaToolRetrieve (args.prefix() + "_retrieve", "Muons"));
    args.add (std::move (retrieveTool));

    std::unique_ptr<IAnaTool> correctTool
      (new MuonToolCorrect (args.prefix() + "_correct"));
    args.add (std::move (correctTool));

    std::unique_ptr<HZZMuonTool> selectTool
      (new HZZMuonTool (args.prefix() + "_select"));
    selectTool->m_quality = quality;
    selectTool->m_wp = WP;
    selectTool->m_isoStr = my_isoStr;
    args.add (std::move (selectTool));

    if (args.configuration()->isData() == false)
    {
      std::unique_ptr<MuonToolWeight> weightTool // Need concrete base class because of public member set
        (new MuonToolWeight (args.prefix() + "_weight"));
      weightTool->m_quality = quality;

      //const auto& lumiCalcFiles = args.configuration()->muDataFiles();
      //if(lumiCalcFiles.empty()) {
      //  ANA_MSG_ERROR("Muon SFs now require ilumicalc files. Please set " <<
      //                "the muDataFiles in your QuickAna configuration");
      //  return StatusCode::FAILURE;
      //}
      //ANA_CHECK( weightTool->setProperty("LumiCalcFiles", lumiCalcFiles) );

      ANA_CHECK( weightTool->setProperty("IsolationWP", my_isoStr) );
      args.add (std::move (weightTool));
    }

    return StatusCode::SUCCESS;
  }

  QUICK_ANA_MUON_DEFINITION_MAKER ("hzz4l", makeHZZMuonTool (args))
  QUICK_ANA_MUON_DEFINITION_MAKER ("smzz4l", makeHZZMuonTool (args, xAOD::Muon::Loose, WPType::_SMZZ4l))
  QUICK_ANA_MUON_DEFINITION_MAKER ("hzhinv_loose", makeHZZMuonTool (args, xAOD::Muon::Loose, WPType::_ZHinv, "FCLoose_FixedRad"))
  QUICK_ANA_MUON_DEFINITION_MAKER ("hzhinv_medium", makeHZZMuonTool (args, xAOD::Muon::Medium, WPType::_ZHinv, "FCLoose_FixedRad"))
  QUICK_ANA_MUON_DEFINITION_MAKER ("hzhinv_loose_isoPF", makeHZZMuonTool (args, xAOD::Muon::Loose, WPType::_ZHinv, "FixedCutPflowLoose"))
  QUICK_ANA_MUON_DEFINITION_MAKER ("hzhinv_medium_isoPF", makeHZZMuonTool (args, xAOD::Muon::Loose, WPType::_ZHinv, "FixedCutPflowLoose"))
  QUICK_ANA_MUON_DEFINITION_MAKER ("hmumu", makeHZZMuonTool (args, xAOD::Muon::Loose, WPType::_Hmumu, "LooseTrackOnly"))

}
