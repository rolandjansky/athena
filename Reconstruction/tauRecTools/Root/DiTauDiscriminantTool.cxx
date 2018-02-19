/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/DiTauDiscriminantTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"

#include "tauRecTools/HelperFunctions.h"
#include "PathResolver/PathResolver.h"


using namespace tauRecTools;


//=================================PUBLIC-PART==================================
//______________________________________________________________________________
DiTauDiscriminantTool::DiTauDiscriminantTool( const std::string& name )
  : AsgTool(name)
  , m_bdt(0)
  , m_eDecayMode(DecayMode::Default)
{
  declareProperty( "WeightsFile", m_sWeightsFile = "tauRecTools/00-02-00/DiTau_JetBDT_spring2017.weights.root");
  declareProperty( "BDTScoreName", m_sBDTScoreName = "JetBDT");
  declareProperty( "DiTauDecayChannel", m_sDecayMode = "HadHad");
}

//______________________________________________________________________________
DiTauDiscriminantTool::~DiTauDiscriminantTool( )
{

}

//______________________________________________________________________________
StatusCode DiTauDiscriminantTool::initialize()
{  
   ATH_MSG_INFO( "Initializing DiTauDiscriminantTool" );
   ATH_MSG_DEBUG( "path to weights file: " << m_sWeightsFile );

   if(m_sDecayMode == "HadHad")
     m_eDecayMode = DecayMode::HadHad;
   else if(m_sDecayMode == "HadEl")
     m_eDecayMode = DecayMode::HadEl;
   else if(m_sDecayMode == "HadMu")
     m_eDecayMode = DecayMode::HadMu;
   if(m_eDecayMode == DecayMode::Default)
     ATH_MSG_ERROR( "No valid DecayMode initialized for DiTauDiscriminantTool. Possible Options: HadHad, HadEl, HadMu");

   switch(m_eDecayMode){
   case(DecayMode::HadHad):
     m_mIDSpectators = {
       {"ditau_pt", nullptr},
       {"mu", nullptr},
       {"pt_weight", nullptr},
       {"isSignal", nullptr}
     };

     m_mIDVariables = {
       {"f_core_lead", nullptr},
       {"f_core_subl", nullptr},
       {"f_subjet_lead", nullptr},
       {"f_subjet_subl", nullptr},
       {"f_subjets", nullptr},
       {"f_track_lead", nullptr},
       {"f_track_subl", nullptr},
       {"R_max_lead", nullptr},
       {"R_max_subl", nullptr},
       {"n_Subjets", nullptr},
       {"n_track", nullptr},
       {"n_tracks_lead", nullptr},
       {"n_tracks_subl", nullptr},
       {"n_isotrack", nullptr},
       {"n_othertrack", nullptr},
       {"R_track", nullptr},
       {"R_track_core", nullptr},
       {"R_track_all", nullptr},
       {"R_isotrack", nullptr},
       {"R_core_lead", nullptr},
       {"R_core_subl", nullptr},
       {"R_tracks_lead", nullptr},
       {"R_tracks_subl", nullptr},
       {"m_track", nullptr},
       {"m_track_core", nullptr},
       {"m_core_lead", nullptr},
       {"m_core_subl", nullptr},
       {"m_track_all", nullptr},
       {"m_tracks_lead", nullptr},
       {"log(m_tracks_lead)", nullptr},
       {"m_tracks_subl", nullptr},
       {"log(m_tracks_subl)", nullptr},
       {"E_frac_subl", nullptr},
       {"E_frac_subsubl", nullptr},
       {"R_subjets_subl", nullptr},
       {"R_subjets_subsubl", nullptr},
       {"d0_leadtrack_lead", nullptr},
       {"log(abs(d0_leadtrack_lead))", nullptr},
       {"d0_leadtrack_subl", nullptr},
       {"log(abs(d0_leadtrack_subl))", nullptr},
       {"f_isotracks", nullptr},
       {"log(f_isotracks)", nullptr},
       {"n_iso_ellipse", nullptr},
       {"n_antikt_subjets", nullptr},
       {"n_ca_subjets", nullptr},
       {"mu_massdrop", nullptr},
       {"y_massdrop", nullptr}
     };
     
     break;
   case(DecayMode::HadEl):
     m_mIDVariables = {
       {"DiTauJetsHadEl.el_IDSelection"           , nullptr},
       {"DiTauJetsHadEl.n_track"                  , nullptr},
       {"DiTauJetsHadEl.E_frac_HadEl"             , nullptr},
       {"DiTauJetsHadEl.tau_f_subjet"             , nullptr},
       {"DiTauJetsHadEl.tau_n_tracks"             , nullptr},
       {"DiTauJetsHadEl.m_track"                  , nullptr},
       {"DiTauJetsHadEl.tau_f_core"               , nullptr},
       {"DiTauJetsHadEl.R_track_all"              , nullptr},
       {"DiTauJetsHadEl.tau_m_tracks"             , nullptr},
       {"DiTauJetsHadEl.tau_d0_leadtrack"         , nullptr},
       {"DiTauJetsHadEl.tau_R_max"                , nullptr},
       {"DiTauJetsHadEl.tau_leadingElIDSelection" , nullptr},
       {"DiTauJetsHadEl.tau_R_tracks"             , nullptr},
       {"DiTauJetsHadEl.tau_m_core"               , nullptr},
       {"DiTauJetsHadEl.tau_R_core"               , nullptr},
       {"DiTauJetsHadEl.E_frac_subsubl"           , nullptr},
       {"DiTauJetsHadEl.tau_leadingElDeltaR"      , nullptr},
       {"DiTauJetsHadEl.el_isoGL"                 , nullptr},
       {"DiTauJetsHadEl.tau_f_track"              , nullptr},
     };
     break;
   case(DecayMode::HadMu):
     m_mIDVariables = {
       {"DiTauJetsHadMu.MuonQuality"         , nullptr},
       {"DiTauJetsHadMu.dRmax"               , nullptr},
       {"DiTauJetsHadMu.mEflowApprox"        , nullptr},
       {"DiTauJetsHadMu.centFrac"            , nullptr},
       {"DiTauJetsHadMu.tau_ntrack"          , nullptr},
       {"DiTauJetsHadMu.innerTrkAvgDist"     , nullptr},
       {"DiTauJetsHadMu.SumPtTrkFrac"        , nullptr},
       {"DiTauJetsHadMu.ptRatioEflowApprox"  , nullptr},
       {"DiTauJetsHadMu.massTrkSys"	     , nullptr},
       {"DiTauJetsHadMu.etOverPtLeadTrk"     , nullptr},
       {"DiTauJetsHadMu.mu_isoGL"            , nullptr},
       {"DiTauJetsHadMu.EMPOverTrkSysP"      , nullptr},
       {"DiTauJetsHadMu.trFlightPathSig"     , nullptr},
       {"DiTauJetsHadMu.absipSigLeadTrk"     , nullptr},
       {"DiTauJetsHadMu.ChPiEMEOverCaloEME"  , nullptr},
       {"(DiTauJetsHadMu.mu_MeasEnergyLoss-DiTauJetsHadMu.mu_ParamEnergyLoss)/(DiTauJetsHadMu.mu_MeasEnergyLoss+DiTauJetsHadMu.mu_ParamEnergyLoss)"  , nullptr},
     };
     break;
   default:
     ATH_MSG_ERROR( "No valid DecayMode" );
     break;
   }
   
   

   ATH_CHECK(parseWeightsFile());

   // m_bIsInitialized = true;
   return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//                              Wrapper functions                             //
////////////////////////////////////////////////////////////////////////////////
double DiTauDiscriminantTool::getJetBDTScore(const xAOD::DiTauJet& xDiTau)
{
  execute(xDiTau);
  return xDiTau.auxdata<double>(m_sBDTScoreName);
}

StatusCode DiTauDiscriminantTool::execute(const xAOD::DiTauJet& xDiTau){
  setIDVariables(xDiTau);

  double bdtScore = m_bdt->GetClassification();

  xDiTau.auxdecor<double>(m_sBDTScoreName) = bdtScore;

  ATH_MSG_DEBUG("Jet BDT score: " << bdtScore);
  return StatusCode::SUCCESS;
}

std::string DiTauDiscriminantTool::getDecayMode(){
  return m_sDecayMode;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________

StatusCode DiTauDiscriminantTool::parseWeightsFile()
{
  std::string weight_file = PathResolverFindCalibFile(m_sWeightsFile);

  ATH_MSG_DEBUG("InputWeightsPath: " << weight_file);

  m_bdt = tauRecTools::configureMVABDT( m_mIDVariables, weight_file.c_str() );
  if(m_bdt==0) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------
void DiTauDiscriminantTool::setIDVariables(const xAOD::DiTauJet& xDiTau)
{
  switch(m_eDecayMode){
  case(DecayMode::HadHad):
    setVar("f_core_lead") = (float) xDiTau.auxdata<double>("f_core_lead");
    setVar("f_core_subl") = (float) xDiTau.auxdata<double>("f_core_subl");
    setVar("f_subjet_lead") = (float) xDiTau.auxdata<double>("f_subjet_lead");
    setVar("f_subjet_subl") = (float) xDiTau.auxdata<double>("f_subjet_subl");
    setVar("f_subjets") = (float) xDiTau.auxdata<double>("f_subjets");
    setVar("f_track_lead") = (float) xDiTau.auxdata<double>("f_track_lead");
    setVar("f_track_subl") = (float) xDiTau.auxdata<double>("f_track_subl");
    setVar("R_max_lead") = (float) xDiTau.auxdata<double>("R_max_lead");
    setVar("R_max_subl") = (float) xDiTau.auxdata<double>("R_max_subl");
    setVar("n_Subjets") = (float) xDiTau.auxdata<int>("n_subjets");
    setVar("n_track") = (float) xDiTau.auxdata<int>("n_track");
    setVar("n_tracks_lead") = (float) xDiTau.auxdata<int>("n_tracks_lead");
    setVar("n_tracks_subl") = (float) xDiTau.auxdata<int>("n_tracks_subl");
    setVar("n_isotrack") = (float) xDiTau.auxdata<int>("n_isotrack");
    setVar("n_othertrack") = (float) xDiTau.auxdata<int>("n_othertrack");
    setVar("R_track") = (float) xDiTau.auxdata<double>("R_track");
    setVar("R_track_core") = (float) xDiTau.auxdata<double>("R_track_core");
    setVar("R_track_all") = (float) xDiTau.auxdata<double>("R_track_all");
    setVar("R_isotrack") = (float) xDiTau.auxdata<double>("R_isotrack");
    setVar("R_core_lead") = (float) xDiTau.auxdata<double>("R_core_lead");
    setVar("R_core_subl") = (float) xDiTau.auxdata<double>("R_core_subl");
    setVar("R_tracks_lead") = (float) xDiTau.auxdata<double>("R_tracks_lead");
    setVar("R_tracks_subl") = (float) xDiTau.auxdata<double>("R_tracks_subl");
    setVar("m_track") = (float) xDiTau.auxdata<double>("m_track");
    setVar("m_track_core") = (float) xDiTau.auxdata<double>("m_track_core");
    setVar("m_core_lead") = (float) xDiTau.auxdata<double>("m_core_lead");
    setVar("m_core_subl") = (float) xDiTau.auxdata<double>("m_core_subl");
    setVar("m_track_all") = (float) xDiTau.auxdata<double>("m_track_all");
    setVar("m_tracks_lead") = (float) xDiTau.auxdata<double>("m_tracks_lead");
    setVar("log(m_tracks_lead)") = log(*m_mIDVariables["m_tracks_lead"]);
    setVar("m_tracks_subl") = (float) xDiTau.auxdata<double>("m_tracks_subl");
    setVar("log(m_tracks_subl)") = log(*m_mIDVariables["m_tracks_subl"]);
    setVar("E_frac_subl") = (float) xDiTau.auxdata<double>("E_frac_subl");
    setVar("E_frac_subsubl") = (float) xDiTau.auxdata<double>("E_frac_subsubl");
    setVar("R_subjets_subl") = (float) xDiTau.auxdata<double>("R_subjets_subl");
    setVar("R_subjets_subsubl") = (float) xDiTau.auxdata<double>("R_subjets_subsubl");
    setVar("d0_leadtrack_lead") = (float) xDiTau.auxdata<double>("d0_leadtrack_lead");
    setVar("log(abs(d0_leadtrack_lead))") = log(fabs(*m_mIDVariables["d0_leadtrack_lead"]));
    setVar("d0_leadtrack_subl") = (float) xDiTau.auxdata<double>("d0_leadtrack_subl");
    setVar("log(abs(d0_leadtrack_subl))") = log(fabs(*m_mIDVariables["d0_leadtrack_subl"]));
    setVar("f_isotracks") = (float) xDiTau.auxdata<double>("f_isotracks");
    setVar("log(f_isotracks)") = log(*m_mIDVariables["f_isotracks"]);
    setVar("n_iso_ellipse") = (float) xDiTau.auxdata<int>("n_iso_ellipse");
    setVar("n_antikt_subjets") = (float) xDiTau.auxdata<int>("n_antikt_subjets");
    setVar("n_ca_subjets") = (float) xDiTau.auxdata<int>("n_ca_subjets");
    setVar("mu_massdrop") = (float) xDiTau.auxdata<double>("mu_massdrop");
    setVar("y_massdrop") = (float) xDiTau.auxdata<double>("y_massdrop");
    break;
  case(DecayMode::HadEl):
    setVar("DiTauJetsHadEl.el_IDSelection"           ) = (float) xDiTau.auxdata<int>   ("el_IDSelection"           );
    setVar("DiTauJetsHadEl.n_track"                  ) = (float) xDiTau.auxdata<int>   ("n_track"                  );
    setVar("DiTauJetsHadEl.E_frac_HadEl"             ) = (float) xDiTau.auxdata<float>("E_frac_HadEl"             );
    setVar("DiTauJetsHadEl.tau_f_subjet"             ) = (float) xDiTau.auxdata<float>("tau_f_subjet"             );
    setVar("DiTauJetsHadEl.tau_n_tracks"             ) = (float) xDiTau.auxdata<int>   ("tau_n_tracks"             );
    setVar("DiTauJetsHadEl.m_track"                  ) = (float) xDiTau.auxdata<float>("m_track"                  );
    setVar("DiTauJetsHadEl.tau_f_core"               ) = (float) xDiTau.auxdata<float>("tau_f_core"               );
    setVar("DiTauJetsHadEl.R_track_all"              ) = (float) xDiTau.auxdata<float>("R_track_all"              );
    setVar("DiTauJetsHadEl.tau_m_tracks"             ) = (float) xDiTau.auxdata<float>("tau_m_tracks"             );
    setVar("DiTauJetsHadEl.tau_d0_leadtrack"         ) = (float) xDiTau.auxdata<float>("tau_d0_leadtrack"         );
    setVar("DiTauJetsHadEl.tau_R_max"                ) = (float) xDiTau.auxdata<float>("tau_R_max"                );
    setVar("DiTauJetsHadEl.tau_leadingElIDSelection" ) = (float) xDiTau.auxdata<int>   ("tau_leadingElIDSelection" );
    setVar("DiTauJetsHadEl.tau_R_tracks"             ) = (float) xDiTau.auxdata<float>("tau_R_tracks"             );
    setVar("DiTauJetsHadEl.tau_m_core"               ) = (float) xDiTau.auxdata<float>("tau_m_core"               );
    setVar("DiTauJetsHadEl.tau_R_core"               ) = (float) xDiTau.auxdata<float>("tau_R_core"               );
    setVar("DiTauJetsHadEl.E_frac_subsubl"           ) = (float) xDiTau.auxdata<float>("E_frac_subsubl"           );
    setVar("DiTauJetsHadEl.tau_leadingElDeltaR"      ) = (float) xDiTau.auxdata<float>("tau_leadingElDeltaR"      );
    setVar("DiTauJetsHadEl.el_isoGL"                 ) = (float) (bool) xDiTau.auxdata<char>   ("el_isoGL"         );
    setVar("DiTauJetsHadEl.tau_f_track"              ) = (float) xDiTau.auxdata<float>("tau_f_track"              );
    break;
  case(DecayMode::HadMu):
    setVar("DiTauJetsHadMu.MuonQuality"       ) = (float) xDiTau.auxdata<int>  ("MuonQuality"       );
    setVar("DiTauJetsHadMu.dRmax"             ) = (float) xDiTau.auxdata<float>("dRmax"             );
    setVar("DiTauJetsHadMu.mEflowApprox"      ) = (float) xDiTau.auxdata<float>("mEflowApprox"      );
    setVar("DiTauJetsHadMu.centFrac"          ) = (float) xDiTau.auxdata<float>("centFrac"          );
    setVar("DiTauJetsHadMu.tau_ntrack"        ) = (float) xDiTau.auxdata<int>  ("tau_ntrack"        );
    setVar("DiTauJetsHadMu.innerTrkAvgDist"   ) = (float) xDiTau.auxdata<float>("innerTrkAvgDist"   );
    setVar("DiTauJetsHadMu.SumPtTrkFrac"      ) = (float) xDiTau.auxdata<float>("SumPtTrkFrac"      );
    setVar("DiTauJetsHadMu.ptRatioEflowApprox") = (float) xDiTau.auxdata<float>("ptRatioEflowApprox");
    setVar("DiTauJetsHadMu.massTrkSys"	      ) = (float) xDiTau.auxdata<float>("massTrkSys"	    );
    setVar("DiTauJetsHadMu.etOverPtLeadTrk"   ) = (float) xDiTau.auxdata<float>("etOverPtLeadTrk"   );
    setVar("DiTauJetsHadMu.mu_isoGL"          ) = (float) (bool) xDiTau.auxdata<char>  ("mu_isoGL"  );
    setVar("DiTauJetsHadMu.EMPOverTrkSysP"    ) = (float) xDiTau.auxdata<float>("EMPOverTrkSysP"    );
    setVar("DiTauJetsHadMu.trFlightPathSig"   ) = (float) xDiTau.auxdata<float>("trFlightPathSig"   );
    setVar("DiTauJetsHadMu.absipSigLeadTrk"   ) = (float) xDiTau.auxdata<float>("absipSigLeadTrk"   );
    setVar("DiTauJetsHadMu.ChPiEMEOverCaloEME") = (float) xDiTau.auxdata<float>("ChPiEMEOverCaloEME");
    {
      float measEnergyLoss = xDiTau.auxdata<float>("mu_MeasEnergyLoss");
      float paramEnergyLoss = xDiTau.auxdata<float>("mu_ParamEnergyLoss");
      setVar("(DiTauJetsHadMu.mu_MeasEnergyLoss-DiTauJetsHadMu.mu_ParamEnergyLoss)/(DiTauJetsHadMu.mu_MeasEnergyLoss+DiTauJetsHadMu.mu_ParamEnergyLoss)") = (float) (measEnergyLoss - paramEnergyLoss)/ (measEnergyLoss + paramEnergyLoss);
    }
    break;
  default:
    ATH_MSG_ERROR("Invalid DecayMode.");
    break;
  }

  for (const auto var: m_vVarNames)
  {
    ATH_MSG_DEBUG(var << ": " << m_mIDVariables[var]);
  }
}
