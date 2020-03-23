/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

/**********************************************************************
 * AsgTool: TrigEgammaL1SelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL1SelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace Trig;
//**********************************************************************


TrigEgammaL1SelectorTool::
TrigEgammaL1SelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  // L1 configuration parameters
  declareProperty( "WPNames"      ,  m_wpNames            ); // must be: ["T","M","L"] (Tight,Medium and Loose)
  declareProperty( "HadCoreCutMin",  m_hadCoreCutMin      ); // must be a list with for values: (default,tight,medium and loose)
  declareProperty( "HadCoreCutOff",  m_hadCoreCutOff      );
  declareProperty( "HadCoreSlope" ,  m_hadCoreSlope       );
  declareProperty( "EmIsolCutMin" ,  m_emIsolCutMin       );
  declareProperty( "EmIsolCutOff" ,  m_emIsolCutOff       );
  declareProperty( "EmIsolSlope"  ,  m_emIsolCutSlope     );
  declareProperty( "IsolCutMax"   ,  m_isolMaxCut = 50    );
}
//**********************************************************************
StatusCode TrigEgammaL1SelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL1SelectorTool::finalize() {
  return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaL1SelectorTool::emulation( const xAOD::EmTauRoI* l1, bool &pass, const Trig::Info &info){

  pass=false;
  if(!l1){
    ATH_MSG_WARNING("L1 not found. poinet is null");
    return false;
  }



  //Retrieve L1 informations
  std::string l1type = info.L1Type;
  std::string L1Item = info.L1Item;
  float l1threshold = info.thrL1; // Is the L1 threshold +1 GeV
  ATH_MSG_DEBUG("L1 Emulation for " << info.trigName);
  ATH_MSG_DEBUG("L1 threshold   = " << l1threshold);
  ATH_MSG_DEBUG("L1 type        = " << l1type);
  ATH_MSG_DEBUG("L1 Item        = " << L1Item);
  

 
  unsigned c=0;
  if(boost::contains(l1type,m_wpNames[0]))  c=1; // Tight
  if(boost::contains(l1type,m_wpNames[1]))  c=2; // Medium
  if(boost::contains(l1type,m_wpNames[2]))  c=3; // Loose
  ATH_MSG_DEBUG("Using position "<< c << " to apply the cuts.");
  float hadCoreCutMin  = m_hadCoreCutMin[c];
  float hadCoreCutOff  = m_hadCoreCutOff[c];
  float hadCoreSlope   = m_hadCoreSlope[c];
  float emIsolCutMin   = m_emIsolCutMin[c]; 
  float emIsolCutOff   = m_emIsolCutOff[c];
  float emIsolCutSlope = m_emIsolCutSlope[c];
  
  /*
  // float hadCoreCut = 0.0;
  float hadCoreCutMin = 1.0; // This could be defined somewhere else
  float hadCoreCutOff = -0.2;
  float hadCoreSlope = 1/23.0;
  // float emIsolCut = 0.0;
  float emIsolCutMin = 2.0; // This could be defined somewhere else
  float emIsolCutOff = -1.8;
  float emIsolCutSlope = 1/8.0;
  // float emEClusVCut = 0.0;
  */

  float emE = 0.0;
  float emIsol = 0.0;
  float hadCore = 0.0;
  float eta = 0.0;
    
  dressDecision(l1, info.trigName, pass); 
  //for(const auto& l1 : *l1Cont){
  if (l1->roiType() != xAOD::EmTauRoI::EMRoIWord) return true;
  emE     = l1->emClus()/1.e3;   // Cluster energy
  eta     = l1->eta();           // eta
  hadCore = l1->hadCore()/1.e3;  // Hadronic core energy
  emIsol  = l1->emIsol()/1.e3;   // EM Isolation energy
  ATH_MSG_DEBUG("emE     = " << emE);
  ATH_MSG_DEBUG("eta     = " << eta);
  ATH_MSG_DEBUG("hadCore = " << hadCore);
  ATH_MSG_DEBUG("emIsol  = " << emIsol);
  
  if (boost::contains(l1type,"H")) {
    ATH_MSG_DEBUG("L1 (H) CUT");
    if (!isolationL1(hadCoreCutMin,hadCoreCutOff,hadCoreSlope,hadCore,emE)) {
      ATH_MSG_DEBUG("rejected");
      return true;
    }
    ATH_MSG_DEBUG("accepted");
  }
  if (boost::contains(l1type,"I")) {
    ATH_MSG_DEBUG("L1 (I) CUT");
    if (!isolationL1(emIsolCutMin,emIsolCutOff,emIsolCutSlope,emIsol,emE)) {
      ATH_MSG_DEBUG("rejected");
      return true;
    }
    ATH_MSG_DEBUG("accepted");
  }
  ATH_MSG_DEBUG("Apply L1 Et cut " << l1threshold << " cluster emE " << emE << " eta " << eta);
  if (boost::contains(l1type,"V")) {
    ATH_MSG_DEBUG("L1 (V) CUT");
    if (!variableEtL1(L1Item,emE,eta)) {
      ATH_MSG_DEBUG("rejected");
      return true;
    }
    ATH_MSG_DEBUG("accepted");
  }
  // add new method for this also
  else if (emE <= l1threshold) { // this cut is confirmed to be <=
    return true;
  }
  pass = true;
   
  dressDecision(l1, info.trigName, pass); 
  ATH_MSG_DEBUG("Emulation L1 Finished " << info.trigName);
  return true;
}
//!==========================================================================
// (H) and (I) Hadronic core and electromagnetic isolation
bool TrigEgammaL1SelectorTool::isolationL1(const float min, const float offset, const float slope, const float energy, const float emE) {
  if (emE > m_isolMaxCut) {
    ATH_MSG_DEBUG("L1 Isolation skipped, ET > Maximum isolation");
    return true;
  }
  float isolation = offset + emE*slope;
  if (isolation < min) isolation = min;
  /*
   from CaloDef.py
   isolation <= ET/m + offset
  */
  bool value = (energy > isolation) ? false : true;
  ATH_MSG_DEBUG("L1 Isolation ET = " << energy << " ISOLATION CUT " << isolation);
  return value;
}
//!==========================================================================
// (V) Variable Et cut
bool TrigEgammaL1SelectorTool::variableEtL1(const std::string L1item, const float l1energy, const float l1eta) {
  float cut = emulationL1V(L1item,l1eta);
  float energy = l1energy;
  ATH_MSG_DEBUG("L1 V " << L1item << " cut = " << cut << " eta " << l1eta << " energy " << l1energy << " rounded to " << energy);
  // if (energy <= cut) return false;
  bool value = (energy <= cut) ? false : true;
  return value;
}
//!==========================================================================
// Eta dependant cuts for (V)
float TrigEgammaL1SelectorTool::emulationL1V(const std::string L1item, const float l1eta) {
 // Values updated from TriggerMenu-00-13-26
 // Now they all look symmetric in negative and positive eta
 // look that in general que can remove the first region since it is the defaul value
  float cut=0.;
  // float eta = fabs((int)l1eta*10);
  float eta = fabs(l1eta);
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
