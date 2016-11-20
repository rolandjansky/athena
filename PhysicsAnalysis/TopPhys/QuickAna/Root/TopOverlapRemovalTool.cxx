/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "QuickAna/TopOverlapRemovalTool.h"

namespace ana
{

const float invGeV = 0.001;

//-----------------------------------------------------------------------------
// Standard constructor
//-----------------------------------------------------------------------------
TopOverlapRemovalTool::TopOverlapRemovalTool(const std::string& name)
        : OverlapRemovalTool(name)
{
  // dR cones for defining overlap
  declareProperty("ElectronJetDRConeEtaPhi",    m_electronJetDREtaPhi    = 0.2);
  declareProperty("JetElectronDRConeEtaPhi",    m_jetElectronDREtaPhi    = 0.4);
  declareProperty("MuonJetDRConeEtaPhi",        m_muonJetDREtaPhi        = 0.4);

  declareProperty("MuORJetVariDR", m_muORJetVariDR = false);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TopOverlapRemovalTool::~TopOverlapRemovalTool()
{
}

//-----------------------------------------------------------------------------
// Remove overlapping electrons and jets
// Need two steps so as to avoid using rejected jets in the 2nd step.
//-----------------------------------------------------------------------------
StatusCode TopOverlapRemovalTool::removeEleJetOverlap
(const xAOD::ElectronContainer& electrons, const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing overlapping electrons and jets");

  // Initialize output decoration if necessary
  initializeDecorations(electrons);
  initializeDecorations(jets);

  // Remove single closest jet within dR of an electron
  for (const auto electron : electrons)
  {
    // Check that this electron passes the input selection
    if ( !isSurvivingObject(electron) ) continue;
    double closestdr = 100.;
    int closestJetIndex = -1;
    for ( auto jetItr = jets.cbegin(); jetItr != jets.cend(); ++jetItr )
    {
      // Check that this jet passes the input selection
      const xAOD::Jet* jet = *jetItr;
      if ( !isSurvivingObject(jet) ) continue;
      double dR = dREtaPhi(jet, electron);
      if (dR<m_electronJetDREtaPhi && dR<closestdr)
      {
        ATH_MSG_DEBUG("  Found overlap jet: " << jet->pt()*invGeV);
        closestdr = dR;
        closestJetIndex = std::distance(jets.begin(), jetItr);
      }
    }
    if (closestJetIndex > -1) setObjectFail(jets[closestJetIndex]);
  }

  // Remove electrons that overlap with surviving jets
  // Maybe this should get its own method.
  for (const auto electron : electrons)
  {
    // Check that this electron passes the input selection
    if ( !isSurvivingObject(electron) ) continue;
    // Use the generic OR method
    for(const auto jet : jets)
    {
      // Check that this jet passes the selection (at the current stage)
      if ( !isSurvivingObject(jet) ) continue;
      if (objectsOverlapEtaPhi(electron, jet, m_jetElectronDREtaPhi))
      {
        ATH_MSG_DEBUG("  Found overlap ele: " << electron->pt()*invGeV);
        setObjectFail(electron);
      }
    }
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove muons overlapping jets
//-----------------------------------------------------------------------------
StatusCode TopOverlapRemovalTool::removeMuonJetOverlap
(const xAOD::MuonContainer& muons, const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing muons overlapping jet");

  // Initialize output decoration if necessary
  initializeDecorations(muons);
  initializeDecorations(jets);

  // Remove muons that overlap with jets
  for(const auto muon : muons){
    if(isSurvivingObject(muon)){
      double muonJetDR = m_muORJetVariDR
        ? m_muonJetDREtaPhi+10.e3/muon->pt()
        : m_muonJetDREtaPhi;
      for(const auto jet : jets){
        // Note: need to make sure no b-tag cut with InputLabel
        //if(isSurvivingObject(jet)){
        if(isInputObject(jet)){
        //if(isJetForRemovingMuons(jet)){ // from Run 1: no eta cut; no randomly dropped (mistake?)
          if(objectsOverlapEtaPhi(jet, muon, muonJetDR)){
            ATH_MSG_DEBUG("  Found overlap muon: " << muon->pt()*invGeV);
            setObjectFail(muon);
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Check if two objects overlap in a dR window
//-----------------------------------------------------------------------------
bool TopOverlapRemovalTool::objectsOverlapEtaPhi(const xAOD::IParticle* p1,
                                                     const xAOD::IParticle* p2,
                                                     double dRMax, double dRMin)
{
  double dR2 = dR2EtaPhi(p1, p2);
  // TODO: use fpcompare utilities
  if(dR2 < (dRMax*dRMax) && dR2 > (dRMin*dRMin)){
    /*// Is this the best place for setting the link?
    if(m_linkOverlapObjects){
      ATH_CHECK( addOverlapObjectLink(p1, p2) );
    }*/
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Calculate delta R between two particles
//-----------------------------------------------------------------------------
double TopOverlapRemovalTool::dR2EtaPhi(const xAOD::IParticle* p1,
                                        const xAOD::IParticle* p2)
{
  double dEta = p1->eta() - p2->eta();
  double dPhi = TVector2::Phi_mpi_pi(p1->phi() - p2->phi());
  return dEta*dEta + dPhi*dPhi;
}
//-----------------------------------------------------------------------------
double TopOverlapRemovalTool::dREtaPhi(const xAOD::IParticle* p1,
                                       const xAOD::IParticle* p2)
{ return sqrt(dR2EtaPhi(p1, p2)); }

}
