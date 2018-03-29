/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/DiTauIDVarCalculator.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"

#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODTau/TauJetContainer.h"

// fastjet
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/tools/MassDropTagger.hh"

using namespace tauRecTools;
using namespace fastjet;

typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >  TrackParticleLinks_t;
typedef ElementLink< xAOD::JetContainer > JetLink_t;

/*
/// Helper macros for checking and retrieving xAOD containers
#define CHECK( ARG )                    \
  do {                                                  \
    const bool result = ARG;                \
    if( ! result ) {                    \
      ::Error( "calcDiTauVariables", "Failed to execute: \"%s\"",  \
           #ARG );                  \
      return 1;                     \
    }                           \
  } while( false )
*/

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
DiTauIDVarCalculator::DiTauIDVarCalculator( const std::string& name )
  : AsgTool(name)
  , m_sDiTauContainerName("DiTauJets")
{
  declareProperty( "DefaultValue", m_dDefault = 0);
  declareProperty( "DiTauContainerName", m_sDiTauContainerName = "DiTauJets");
  declareProperty( "doCalcCluserVariables", m_bCalcCluserVariables = false);
}

//______________________________________________________________________________
DiTauIDVarCalculator::~DiTauIDVarCalculator( )
{
}

//______________________________________________________________________________
StatusCode DiTauIDVarCalculator::initialize()
{
  ATH_MSG_INFO( "Initializing DiTauIDVarCalculator" );
  m_DiTauContainerNameAux = m_sDiTauContainerName + "Aux.";

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode DiTauIDVarCalculator::initializeEvent()
{
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//                              Wrapper functions                             //
////////////////////////////////////////////////////////////////////////////////
StatusCode DiTauIDVarCalculator::calculateIDVariables(const xAOD::DiTauJet& xDiTau)
{
  ATH_MSG_DEBUG("Calculate DiTau ID variables");
  
  xDiTau.auxdecor< int >("n_subjets") = n_subjets(xDiTau);
  ATH_CHECK( decorNtracks(xDiTau) );
  xDiTau.auxdecor< double >( "ditau_pt") = ditau_pt(xDiTau);
  xDiTau.auxdecor< double >( "f_core_lead" ) = f_core(xDiTau, 0);
  xDiTau.auxdecor< double >( "f_core_subl" ) = f_core(xDiTau, 1);
  xDiTau.auxdecor< double >( "f_subjet_lead" ) = f_subjet(xDiTau, 0);
  xDiTau.auxdecor< double >( "f_subjet_subl" ) = f_subjet(xDiTau, 1);
  xDiTau.auxdecor< double >( "f_subjets") = f_subjets(xDiTau);
  xDiTau.auxdecor< double >( "f_track_lead") = f_track(xDiTau, 0);
  xDiTau.auxdecor< double >( "f_track_subl") = f_track(xDiTau, 1);
  xDiTau.auxdecor< double >( "R_max_lead") = R_max(xDiTau, 0);
  xDiTau.auxdecor< double >( "R_max_subl") = R_max(xDiTau, 1);
  xDiTau.auxdecor< int >( "n_track" ) = n_track(xDiTau);
  xDiTau.auxdecor< int >( "n_tracks_lead" ) = n_tracks(xDiTau, 0);
  xDiTau.auxdecor< int >( "n_tracks_subl" ) = n_tracks(xDiTau, 1);
  xDiTau.auxdecor< int >( "n_isotrack" ) = n_isotrack(xDiTau);
  xDiTau.auxdecor< int >( "n_othertrack" ) = n_othertrack(xDiTau);
  xDiTau.auxdecor< double >( "R_track" ) = R_track(xDiTau);
  xDiTau.auxdecor< double >( "R_track_core" ) = R_track_core(xDiTau);
  xDiTau.auxdecor< double >( "R_track_all" ) = R_track_all(xDiTau);
  xDiTau.auxdecor< double >( "R_isotrack" ) = R_isotrack(xDiTau);
  xDiTau.auxdecor< double >( "R_core_lead" ) = R_core(xDiTau, 0);
  xDiTau.auxdecor< double >( "R_core_subl" ) = R_core(xDiTau, 1);
  xDiTau.auxdecor< double >( "R_tracks_lead" ) = R_tracks(xDiTau, 0);
  xDiTau.auxdecor< double >( "R_tracks_subl" ) = R_tracks(xDiTau, 1);
  xDiTau.auxdecor< double >( "m_track" ) = mass_track(xDiTau);
  xDiTau.auxdecor< double >( "m_track_core" ) = mass_track_core(xDiTau);
  xDiTau.auxdecor< double >( "m_core_lead" ) = mass_core(xDiTau, 0);
  xDiTau.auxdecor< double >( "m_core_subl" ) = mass_core(xDiTau, 1);
  xDiTau.auxdecor< double >( "m_track_all" ) = mass_track_all(xDiTau);
  xDiTau.auxdecor< double >( "m_tracks_lead" ) = mass_tracks(xDiTau, 0);
  xDiTau.auxdecor< double >( "m_tracks_subl" ) = mass_tracks(xDiTau, 1);
  xDiTau.auxdecor< double >( "E_frac_subl" ) = E_frac(xDiTau,1);
  xDiTau.auxdecor< double >( "E_frac_subsubl") = E_frac(xDiTau, 2);
  xDiTau.auxdecor< double >( "R_subjets_subl") = R_subjets(xDiTau, 1);
  xDiTau.auxdecor< double >( "R_subjets_subsubl") = R_subjets(xDiTau, 2);
  xDiTau.auxdecor< double >( "d0_leadtrack_lead") = d0_leadtrack(xDiTau, 0);
  xDiTau.auxdecor< double >( "d0_leadtrack_subl") = d0_leadtrack(xDiTau, 1);
  xDiTau.auxdecor< double >( "f_isotracks" ) = f_isotracks(xDiTau);
  xDiTau.auxdecor< int >( "n_iso_ellipse" ) = n_iso_ellipse(xDiTau);
  
  if (m_bCalcCluserVariables)
  {
    std::vector<PseudoJet> vClusters = getClusters(xDiTau);
    
    xDiTau.auxdecor< int >( "n_antikt_subjets" ) = n_antikt_subjets(vClusters);
    xDiTau.auxdecor< int >( "n_ca_subjets" ) = n_ca_subjets(vClusters);
    xDiTau.auxdecor< double >( "f_clusters" ) = f_clusters(xDiTau, vClusters);
    mass_drop(xDiTau, vClusters);
  }
  else 
  {
    xDiTau.auxdecor< int >( "n_antikt_subjets" ) = m_dDefault;
    xDiTau.auxdecor< int >( "n_ca_subjets" ) = m_dDefault;
    xDiTau.auxdecor< double >( "f_clusters" ) = m_dDefault;
    xDiTau.auxdecor< double >( "mu_massdrop" ) = m_dDefault;
    xDiTau.auxdecor< double >( "y_massdrop" ) = m_dDefault;
  }


  return StatusCode::SUCCESS;
}



//=================================PRIVATE-PART=================================
//______________________________________________________________________________
double DiTauIDVarCalculator::n_subjets(const xAOD::DiTauJet& xDiTau) const
{
  int nSubjet = 0;
  while (xDiTau.subjetPt(nSubjet) > 0. )
  {
    nSubjet++;
  }

  return nSubjet;
}


double DiTauIDVarCalculator::ditau_pt(const xAOD::DiTauJet& xDiTau) const
{
  if (xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  return xDiTau.subjetPt(0)+xDiTau.subjetPt(1);
}


//______________________________________________________________________________;
double DiTauIDVarCalculator::f_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const 
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.fCore(iSubjet);
}


//______________________________________________________________________________;
double DiTauIDVarCalculator::f_subjet(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.subjetPt(iSubjet) / xDiTau.pt();
}


//______________________________________________________________________________;
double DiTauIDVarCalculator::f_subjets(const xAOD::DiTauJet& xDiTau) const
{
  if (xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  return (xDiTau.subjetPt(0) + xDiTau.subjetPt(1))/ xDiTau.pt();
}


//______________________________________________________________________________;
double DiTauIDVarCalculator::f_track(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
      
  TLorentzVector tlvTrack;
  TLorentzVector tlvLeadTrack;
  tlvLeadTrack.SetPtEtaPhiE( 0,0,0, 0);

  for (const auto &xTrack: xTracks) 
  { 
    if (!xTrack) 
    {
      ATH_MSG_ERROR("Could not read Track");
      continue;
    }
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      if (tlvLeadTrack.Pt() < tlvTrack.Pt()) 
      {
        tlvLeadTrack = tlvTrack;
      }
    }
  }

  return tlvLeadTrack.Pt() / tlvSubjet.Pt();
}


//______________________________________________________________________________;
double DiTauIDVarCalculator::R_max(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{ 
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
      
  TLorentzVector tlvTrack;
  TLorentzVector tlvRmaxTrack;
  double Rmax = 0;
  for (const auto &xTrack: xTracks) 
  {
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_subjet") )
    {
      if (tlvTrack.DeltaR(tlvSubjet) > Rmax) 
      {
        Rmax = tlvTrack.DeltaR(tlvSubjet);
      }
    }
  }

  return Rmax;
}


//______________________________________________________________________________;
int DiTauIDVarCalculator::n_track(const xAOD::DiTauJet& xDiTau) const
{ 
  return xDiTau.nTracks();
}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  if (!xDiTau.isAvailable<std::vector<int>>("n_tracks"))
  {
    ATH_MSG_WARNING("n_tracks decoration not available. Try with track links.");

    if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
    {
      ATH_MSG_WARNING("Track links not available. Return 0.");
      return (int)m_dDefault;
    } 

    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                            xDiTau.subjetEta(iSubjet),
                            xDiTau.subjetPhi(iSubjet),
                            xDiTau.subjetE(iSubjet) );
        
    TLorentzVector tlvTrack;
    int nTracks = 0;
    for (const auto &xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 ) nTracks++;
    }

    return nTracks;
  }

  return xDiTau.auxdata<std::vector<int>>("n_tracks").at(iSubjet);

}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_isotrack(const xAOD::DiTauJet& xDiTau) const
{ 
  return xDiTau.nIsoTracks();
}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_othertrack(const xAOD::DiTauJet& xDiTau) const
{ 
  return xDiTau.nOtherTracks();
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }

  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
 
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      //ATH_MSG_DEBUG("smaller");
      R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
      pt += tlvTrack.Pt();
    }
  }
  
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
 
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >( "R_core" ) )
    {
      R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
      pt += tlvTrack.Pt();
    }
  }
  
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_track_core(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 

  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
      {
        //ATH_MSG_DEBUG("smaller");
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_track(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 
  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );

      if (tlvSubjet.DeltaR(tlvTrack) < 0.2)
      {
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}
//______________________________________________________________________________;
double DiTauIDVarCalculator::R_track_all(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }

  for (int i = 0; i<xDiTau.auxdata<int>("n_subjets"); i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );

      if (tlvSubjet.DeltaR(tlvTrack) <= 0.2)
      {
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }

  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_isotrack(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 
  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvIsoTrack;

    for (auto xIsoTrack: xIsoTracks) 
    { 
      tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                                (*xIsoTrack)->eta(),
                                (*xIsoTrack)->phi(),
                                (*xIsoTrack)->e() );

      if (tlvSubjet.DeltaR(tlvIsoTrack) < 0.4)
      {
        R_sum += tlvSubjet.DeltaR(tlvIsoTrack)*tlvIsoTrack.Pt();
        pt += tlvIsoTrack.Pt();
      }
    }
  }

  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::mass_track_core(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;

  for (int i = 0; i<=1; i++)
  {

    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
    
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
      {
        //ATH_MSG_DEBUG("smaller");
        tlvallTracks += tlvTrack;
      }
    }
  }
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::mass_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;


  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );
    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
    {
      //ATH_MSG_DEBUG("smaller");
      tlvallTracks += tlvTrack;
    }
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::mass_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );
    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      tlvallTracks += tlvTrack;
    }
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}
//______________________________________________________________________________;
double DiTauIDVarCalculator::mass_track(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    tlvallTracks += tlvTrack;
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }
  return tlvallTracks.M();
}
//______________________________________________________________________________;
double DiTauIDVarCalculator::mass_track_all(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    tlvallTracks += tlvTrack;
  }

  
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                             (*xIsoTrack)->eta(),
                             (*xIsoTrack)->phi(),
                             (*xIsoTrack)->e() );

    tlvallTracks += tlvIsoTrack;
  }

  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::E_frac(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{ 
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.subjetE(iSubjet) / xDiTau.subjetE(0);
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::R_subjets(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
  
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  TLorentzVector tlvLeadSubjet;
  tlvLeadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(0),
                              xDiTau.subjetEta(0),
                              xDiTau.subjetPhi(0),
                              xDiTau.subjetE(0) );

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  return tlvLeadSubjet.DeltaR(tlvSubjet);
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::d0_leadtrack(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double pt_leadtrk = 0;
  double d0 = m_dDefault;
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  } 

  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
			  
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if (tlvTrack.DeltaR(tlvSubjet) < xDiTau.auxdata< float >("R_core")) 
    {
      if (tlvTrack.Pt() > pt_leadtrk)
      {
        pt_leadtrk = tlvTrack.Pt();
        d0 = (*xTrack)->d0();
      }
    }
  }
  return d0;
}

//______________________________________________________________________________;
double DiTauIDVarCalculator::f_isotracks(const xAOD::DiTauJet& xDiTau) const
{
  double iso_pt = 0;
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                              (*xIsoTrack)->eta(),
                              (*xIsoTrack)->phi(),
                              (*xIsoTrack)->e() );

    iso_pt += tlvIsoTrack.Pt();
  }

  return iso_pt / xDiTau.pt();
}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_iso_ellipse(const xAOD::DiTauJet& xDiTau) const
{
  int N_iso = 0;
  
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  if ( xDiTau.auxdata<int>("n_subjets") < 2 ) 
  {
    return m_dDefault;
  }

  TLorentzVector tlvLeadSubjet;
  tlvLeadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(0),
                              xDiTau.subjetEta(0),
                              xDiTau.subjetPhi(0),
                              xDiTau.subjetE(0) );

  
  TLorentzVector tlvSubleadSubjet;
  tlvSubleadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(1),
                                 xDiTau.subjetEta(1),
                                 xDiTau.subjetPhi(1),
                                 xDiTau.subjetE(1) );
				 
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                              (*xIsoTrack)->eta(),
                              (*xIsoTrack)->phi(),
                              (*xIsoTrack)->e() );
			      
    if ((tlvIsoTrack.DeltaR(tlvLeadSubjet) + tlvIsoTrack.DeltaR(tlvSubleadSubjet)) <= (tlvLeadSubjet.DeltaR(tlvSubleadSubjet)+ 0.4))
    {
      N_iso += 1;
    }
  }
  return N_iso;
}
  
// ----------------------------------------------------------------------------
std::vector<PseudoJet> DiTauIDVarCalculator::getClusters (const xAOD::DiTauJet& xDiTau) const
{
  // do some checks and retrieve the antikt 10 seed jet
  if (!xDiTau.isAvailable< JetLink_t >("jetLink") )
  {
    ATH_MSG_WARNING("Jet link not available");
  }

  JetLink_t xJetLink = xDiTau.jetLink();
  if (!xJetLink.isValid())
  {
    ATH_MSG_WARNING("Jet link is not valid");
  }

  // get clusters from the jet
  const xAOD::Jet* xJet = (*xJetLink);
  const xAOD::JetConstituentVector vConst = xJet->getConstituents();
  if (vConst.size() == 0) {
    ATH_MSG_WARNING("Cluster constituents could not be retrieved from seed jet.");
  }

  // convert clusters to pseudojets
  std::vector<PseudoJet> vClusters;
  vClusters.clear();

  for (const auto *cl: vConst) 
  {
    double pt = cl->pt();
    double px = pt*cos(cl->phi());  
    double py = pt*sin(cl->phi());  
    double pz = pt*sinh(cl->eta()); 
    double e  = sqrt(px*px + py*py + pz*pz);
    PseudoJet c( px, py, pz, e);

    vClusters.push_back(c);
  }

  return vClusters;
}

// ----------------------------------------------------------------------------
int DiTauIDVarCalculator::n_antikt_subjets(std::vector<PseudoJet> vClusters)
{
  // build anti-kt-2 subjets
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition subjet_def = JetDefinition(antikt_algorithm, 0.2);
  ClusterSequenceArea subjet_cs(vClusters, subjet_def, area_def); 
  std::vector<PseudoJet> vSubjet = sorted_by_pt( subjet_cs.inclusive_jets(10*1000) );

  return vSubjet.size();
}

// ----------------------------------------------------------------------------
int DiTauIDVarCalculator::n_ca_subjets(std::vector<PseudoJet> vClusters)
{
  // build anti-kt-10 fatjet
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition fatjet_def = JetDefinition(antikt_algorithm, 1.0);
  ClusterSequenceArea fatjet_cs(vClusters, fatjet_def, area_def); 
  std::vector<PseudoJet> vJet = sorted_by_pt( fatjet_cs.inclusive_jets(10*1000) );

  PseudoJet jet = (*vJet.begin());

  // define subjet filter
  Filter filter(0.2, SelectorPtMin(10*1000)); 

  // filter seed jet for subjets
  PseudoJet filtered_jet = filter(jet);
  std::vector<PseudoJet> vSubjets = filtered_jet.pieces();

  return vSubjets.size();
}

// ----------------------------------------------------------------------------
void DiTauIDVarCalculator::mass_drop(const xAOD::DiTauJet& xDiTau, std::vector<PseudoJet> vClusters) const
{
  // build cambridge aachen fatjet
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition fatjet_def = JetDefinition(cambridge_algorithm, 1.0);
  ClusterSequenceArea fatjet_cs(vClusters, fatjet_def, area_def); 
  std::vector<PseudoJet> vJet = sorted_by_pt( fatjet_cs.inclusive_jets(10*1000) );

  PseudoJet jet = (*vJet.begin());

  // define mass drop tagger
  MassDropTagger mdtagger(0.9, 0.01);

  PseudoJet tagged_jet = mdtagger(jet);

  double mu = m_dDefault;
  double y = m_dDefault;
  if (tagged_jet != 0)
  {
    mu = tagged_jet.structure_of<MassDropTagger>().mu();
    y = tagged_jet.structure_of<MassDropTagger>().y();
  }
  
  // check if mu = NaN, if NaN it is set to 0 
  if (mu != mu){
    mu = 0;
  }
  xDiTau.auxdecor< double >( "mu_massdrop" ) = mu;
  xDiTau.auxdecor< double >( "y_massdrop" ) = y;

}

//______________________________________________________________________________;
double DiTauIDVarCalculator::f_clusters(const xAOD::DiTauJet& xDiTau, std::vector<PseudoJet> vClusters) const
{
  double e_clust = 0;
  double e_all = 0;
  
  if ( xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  double pt0 = xDiTau.subjetPt(0);
  double px0 = pt0*cos(xDiTau.subjetPhi(0));  
  double py0 = pt0*sin(xDiTau.subjetPhi(0));  
  double pz0 = pt0*sinh(xDiTau.subjetEta(0)); 
  double e0  = sqrt(px0*px0 + py0*py0 + pz0*pz0);
  PseudoJet PLeadSubjet( px0, py0, pz0, e0);

  double pt1 = xDiTau.subjetPt(1);
  double px1 = pt1*cos(xDiTau.subjetPhi(1));  
  double py1 = pt1*sin(xDiTau.subjetPhi(1));  
  double pz1 = pt1*sinh(xDiTau.subjetEta(1)); 
  double e1  = sqrt(px1*px1 + py1*py1 + pz1*pz1);
  PseudoJet PSubleadSubjet( px1, py1, pz1, e1);

  for (const auto cl: vClusters) {
    PseudoJet PCluster( cl.px(),
      cl.py(),
      cl.pz(),
      cl.e());

    e_all += PCluster.E();
    
    double r_clus_lead = PCluster.delta_R(PLeadSubjet);
    double r_clus_subl = PCluster.delta_R(PSubleadSubjet);
    double r_lead_subl = PLeadSubjet.delta_R(PSubleadSubjet);
    if (r_clus_lead>0.2 && r_clus_subl>0.2 && ((r_clus_lead + r_clus_subl) <= (r_lead_subl +0.4))){
      e_clust += PCluster.E();
    }
  }
  return e_clust/e_all;
}


//______________________________________________________________________________;
StatusCode DiTauIDVarCalculator::decorNtracks (const xAOD::DiTauJet& xDiTau)
{
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    Warning("decorNtracks()", "Track links not available.");
    return StatusCode::FAILURE;
  } 

  int nSubjets = xDiTau.auxdata<int>("n_subjets");

  double Rsubjet = xDiTau.auxdata<float>("R_subjet");
  std::vector<int> nTracks(nSubjets, 0);

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
  for (const auto xTrack: xTracks)
  {
    double dRmin = 1111;
    double itrmin = -1;

    for (int i=0; i<nSubjets; ++i)
    {
      TLorentzVector tlvSubjet = TLorentzVector();
      tlvSubjet.SetPtEtaPhiE(xDiTau.subjetPt(i),
                             xDiTau.subjetEta(i),
                             xDiTau.subjetPhi(i),
                             xDiTau.subjetE(i));
      double dR = tlvSubjet.DeltaR((*xTrack)->p4());


      if ((dR < Rsubjet) && (dR < dRmin))
      {
        dRmin = dR;
        itrmin = i;
      }
    } // loop over subjets
    if (itrmin > -1) nTracks[itrmin]++;
  } // loop over tracks

  xDiTau.auxdecor< std::vector<int> >("n_tracks") = nTracks;

  return StatusCode::SUCCESS;
}
