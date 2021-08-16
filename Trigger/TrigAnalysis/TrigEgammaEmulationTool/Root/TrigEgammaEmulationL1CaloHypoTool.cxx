
 
#include "TrigEgammaEmulationTool/TrigEgammaEmulationL1CaloHypoTool.h"
#include "boost/algorithm/string.hpp"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;
//**********************************************************************

TrigEgammaEmulationL1CaloHypoTool::TrigEgammaEmulationL1CaloHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}


//!==========================================================================


bool TrigEgammaEmulationL1CaloHypoTool::emulate(  const Trig::TrigData &input, bool &pass ) const
{
  pass=false;
  if( !input.l1 ){
    ATH_MSG_WARNING("L1 not found. poinet is null");
    return false;
  }

  pass = decide( input );
  return true;
}


//!==========================================================================
bool TrigEgammaEmulationL1CaloHypoTool::decide(  const Trig::TrigData &input ) const
{
  auto l1 = input.l1;
  std::string l1item = m_l1item;

  //for(const auto& l1 : *l1Cont){
  if (l1->roiType() != xAOD::EmTauRoI::EMRoIWord){
    ATH_MSG_INFO("This roi is not EMRoIWord!");
    return true;
  }
  float emE     = l1->emClus()/Gaudi::Units::GeV;   // Cluster energy
  float eta     = l1->eta();                        // eta
  float hadCore = l1->hadCore()/Gaudi::Units::GeV;  // Hadronic core energy
  float emIsol  = l1->emIsol()/Gaudi::Units::GeV;   // EM Isolation energy

  ATH_MSG_INFO("emE     = " << emE);
  ATH_MSG_INFO("eta     = " << eta);
  ATH_MSG_INFO("hadCore = " << hadCore);
  ATH_MSG_INFO("emIsol  = " << emIsol);
  
  if (boost::contains(l1item,"H")) {
    ATH_MSG_INFO("L1 (H) CUT");
    if (!isolationL1(m_hadCoreCutMin,m_hadCoreCutOff,m_hadCoreCutSlope,hadCore,emE)) {
      ATH_MSG_INFO("rejected");
      return false;
    }
    ATH_MSG_INFO("accepted");
  }

  if (boost::contains(l1item,"I")) {
    ATH_MSG_INFO("L1 (I) CUT");
    if (!isolationL1(m_emIsolCutMin,m_emIsolCutOff,m_emIsolCutSlope,emIsol,emE)) {
      ATH_MSG_INFO("rejected");
      return false;
    }
    ATH_MSG_INFO("accepted");
  }

  ATH_MSG_INFO("Apply L1 Et cut " << m_l1threshold << " cluster emE " << emE << " eta " << eta);
  if (boost::contains(l1item,"V")) {
    ATH_MSG_INFO("L1 (V) CUT");
    if (!variableEtL1(l1item,emE,eta)) {
      ATH_MSG_INFO("rejected");
      return false;
    }
    ATH_MSG_INFO("accepted");
  }
  // add new method for this also
  else if (emE <= m_l1threshold) { // this cut is confirmed to be <=
    return false;
  }
   
  return true;
}


//!==========================================================================

// (H) and (I) Hadronic core and electromagnetic isolation
bool TrigEgammaEmulationL1CaloHypoTool::isolationL1(float min, float offset, float slope, float energy, float emE) const
{
  if (emE > m_isolMaxCut) {
    ATH_MSG_INFO("L1 Isolation skipped, ET > Maximum isolation");
    return true;
  }
  float isolation = offset + emE*slope;
  if (isolation < min) isolation = min;
  /*
   from CaloDef.py
   isolation <= ET/m + offset
  */
  bool value = (energy <= isolation);


  ATH_MSG_INFO("L1 Isolation ET = " << energy << " ISOLATION CUT " << isolation);
  return value;
}


//!==========================================================================
// (V) Variable Et cut
bool TrigEgammaEmulationL1CaloHypoTool::variableEtL1(std::string L1item, float l1energy, float l1eta) const
{
  float cut = emulationL1V(L1item,l1eta);
  return l1energy>cut;
}
//!==========================================================================
// Eta dependant cuts for (V)
float TrigEgammaEmulationL1CaloHypoTool::emulationL1V(std::string L1item, float l1eta) const
{
 // Values updated from TriggerMenu-00-13-26
 // Now they all look symmetric in negative and positive eta
 // look that in general que can remove the first region since it is the defaul value
  float cut=0.;
  // float eta = fabs((int)l1eta*10);
  float eta = std::abs(l1eta);
  if (L1item=="50V") {
    if      (eta >= 0.8 && eta < 1.2) cut = 51.0;
    else if (eta >= 1.2 && eta < 1.6) cut = 50.0;
    else if (eta >= 1.6 && eta < 2.0) cut = 51.0;
    else cut = 52;
  }
  else if (L1item=="8VH") {
    if      (eta > 0.8 && eta <= 1.1) cut = 7.0;
    else if (eta > 1.1 && eta <= 1.4) cut = 6.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 5.0;
    else if (eta > 1.5 && eta <= 1.8) cut = 7.0;
    else if (eta > 1.8 && eta <= 2.5) cut = 8.0;
    else cut = 9.0;
  }
  else if (L1item=="10VH") {
    if      (eta > 0.8 && eta <= 1.1) cut = 9.0;
    else if (eta > 1.1 && eta <= 1.4) cut = 8.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 7.0;
    else if (eta > 1.5 && eta <= 1.8) cut = 9.0;
    else if (eta > 1.8 && eta <= 2.5) cut = 10.0;
    else cut = 11.;
  }
  else if (L1item=="13VH") {
    if      (eta > 0.7 && eta <= 0.9) cut = 14.0;
    else if (eta > 0.9 && eta <= 1.2) cut = 13.0;
    else if (eta > 1.2 && eta <= 1.4) cut = 12.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 11.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 13.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 14.0;
    else cut = 15.0;
  }
  else if (L1item=="15VH") {
    if      (eta > 0.7 && eta <= 0.9) cut = 16.0;
    else if (eta > 0.9 && eta <= 1.2) cut = 15.0;
    else if (eta > 1.2 && eta <= 1.4) cut = 14.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 13.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 15.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 16.0;
    else cut = 17.0;
  }
  else if (L1item == "18VH") {
    if      (eta > 0.7 && eta <= 0.8) cut = 19.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 18.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 17.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 16.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 15.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 17.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 19.0;
    else cut = 20.0;
  }
  else if (L1item == "20VH") {
    if      (eta > 0.7 && eta <= 0.8) cut = 21.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 20.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 19.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 18.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 17.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 19.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 21.0;
    else cut = 22.0;
  }
  else if (L1item == "20VHI") { // Same as 20VH
    if      (eta > 0.7 && eta <= 0.8) cut = 21.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 20.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 19.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 18.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 17.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 19.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 21.0;
    else cut = 22.0;
  }
  else if (L1item == "22VHI") {
    if      (eta > 0.7 && eta <= 0.8) cut = 23.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 22.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 21.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 20.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 19.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 21.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 23.0;
    else cut = 24.0;
  }
  return cut;
}
