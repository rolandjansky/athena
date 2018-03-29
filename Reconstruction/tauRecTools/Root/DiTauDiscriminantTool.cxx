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
{
  declareProperty( "WeightsFile", m_sWeightsFile = "tauRecTools/00-02-00/DiTau_JetBDT_spring2017.weights.root");
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

   m_mIDSpectators = {
      {"ditau_pt", new float(0)},
      {"mu", new float(0)},
      {"pt_weight", new float(0)},
      {"isSignal", new float(0)}
   };

   m_mIDVariables = {
      {"f_core_lead", new float(0)},
      {"f_core_subl", new float(0)},
      {"f_subjet_lead", new float(0)},
      {"f_subjet_subl", new float(0)},
      {"f_subjets", new float(0)},
      {"f_track_lead", new float(0)},
      {"f_track_subl", new float(0)},
      {"R_max_lead", new float(0)},
      {"R_max_subl", new float(0)},
      {"n_Subjets", new float(0)},
      {"n_track", new float(0)},
      {"n_tracks_lead", new float(0)},
      {"n_tracks_subl", new float(0)},
      {"n_isotrack", new float(0)},
      {"n_othertrack", new float(0)},
      {"R_track", new float(0)},
      {"R_track_core", new float(0)},
      {"R_track_all", new float(0)},
      {"R_isotrack", new float(0)},
      {"R_core_lead", new float(0)},
      {"R_core_subl", new float(0)},
      {"R_tracks_lead", new float(0)},
      {"R_tracks_subl", new float(0)},
      {"m_track", new float(0)},
      {"m_track_core", new float(0)},
      {"m_core_lead", new float(0)},
      {"log(m_core_lead)", new float(0)},
      {"m_core_subl", new float(0)},
      {"log(m_core_subl)", new float(0)},
      {"m_track_all", new float(0)},
      {"m_tracks_lead", new float(0)},
      {"log(m_tracks_lead)", new float(0)},
      {"m_tracks_subl", new float(0)},
      {"log(m_tracks_subl)", new float(0)},
      {"E_frac_subl", new float(0)},
      {"E_frac_subsubl", new float(0)},
      {"R_subjets_subl", new float(0)},
      {"R_subjets_subsubl", new float(0)},
      {"d0_leadtrack_lead", new float(0)},
      {"log(abs(d0_leadtrack_lead))", new float(0)},
      {"d0_leadtrack_subl", new float(0)},
      {"log(abs(d0_leadtrack_subl))", new float(0)},
      {"f_isotracks", new float(0)},
      {"log(f_isotracks)", new float(0)},
      {"n_iso_ellipse", new float(0)},
      {"n_antikt_subjets", new float(0)},
      {"n_ca_subjets", new float(0)},
      {"mu_massdrop", new float(0)},
      {"y_massdrop", new float(0)}
   };

   ATH_CHECK(parseWeightsFile());

   // m_bIsInitialized = true;
   return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode DiTauDiscriminantTool::initializeEvent()
{
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//                              Wrapper functions                             //
////////////////////////////////////////////////////////////////////////////////
double DiTauDiscriminantTool::getJetBDTScore(const xAOD::DiTauJet& xDiTau)
{
  setIDVariables(xDiTau);

  double bdtScore = m_bdt->GetClassification();

  xDiTau.auxdecor<double>("JetBDT") = bdtScore;

  ATH_MSG_DEBUG("Jet BDT score: " << bdtScore);
  return bdtScore;
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
  setVar("log(m_core_lead)") = log(*m_mIDVariables["m_core_lead"]);
  setVar("m_core_subl") = (float) xDiTau.auxdata<double>("m_core_subl");
  setVar("log(m_core_subl)") = log(*m_mIDVariables["m_core_subl"]);
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

  for (const auto var: m_vVarNames)
  {
    ATH_MSG_DEBUG(var << ": " << m_mIDVariables[var]);
  }
}
