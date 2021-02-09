/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetSubStructureHistos.h"
#include "TProfile2D.h"
#include "TString.h"
#include <map>

#define toGeV 1/1000.

JetSubStructureHistos::JetSubStructureHistos(const std::string &t) : JetHistoBase(t) 
                                                             ,m_tau21(0)
                                                             ,m_jetScale("JetAssignedScaleMomentum")
{
    declareProperty("JetScale", m_jetScale);
}

int JetSubStructureHistos::buildHistos(){

  // -------------- Modify histo names/titles --------
  //  if we're plotting a defined scale, modify the Histo titles and names
  
  std::map<std::string, std::string> scale2str( { 
      { "JetEMScaleMomentum" , "EMScale" } , 
      { "JetConstitScaleMomentum" , "ConstitScale" } } );
  TString scaleTag = scale2str[ m_jetScale ] ; // defaults to ""

  TString prefixn = scaleTag;
  if(prefixn != "") prefixn +="_";
  // -------------- 

  // Build and register the histos in this group : 
  TH1::AddDirectory(kFALSE); // Turn off automatic addition to gDirectory to avoid Warnings. Histos are anyway placed in their own dir later.
  m_tau21 = bookHisto( new TH1F(prefixn+"Tau21"  ,  "Jet Tau21 ;Entries", 100, 0, 1) );
  m_tau32 = bookHisto( new TH1F(prefixn+"Tau32"  ,  "Jet Tau32 ;Entries", 100, 0, 1) );
  m_C1    = bookHisto( new TH1F(prefixn+"C1"  ,  "Jet C1 (GeV);Entries", 100, -1, 1) );
  m_C2    = bookHisto( new TH1F(prefixn+"C2"  ,  "Jet C2 (GeV);Entries", 100, -1, 1) );
  m_D2    = bookHisto( new TH1F(prefixn+"D2"  ,  "Jet D2 (GeV);Entries", 100, -1, 1) );
  


  TH1::AddDirectory(kTRUE); // Turn on automatic addition to gDirectory in case others needs it.

  // -------------- Modify histo titles.
  if(prefixn != "" ){

    // build a qualifier in the form "(EMScale, Leading Jet, ...)"
    TString qualif = "(";
    TString tags[] = { scaleTag};
    for(auto t : tags ) { if(qualif != "(") qualif+=",";qualif += t; }
    qualif += ")";
    // reset all titles :
    for(auto& hdata : m_vBookedHistograms ){
      TString t = hdata.hist->GetTitle(); t+=" "+qualif;
      hdata.hist->SetTitle(t );
    }
  }

  
  return 0;
}



int JetSubStructureHistos::fillHistosFromJet(const xAOD::Jet &j){
  //For definitions see JetSubStructureMomentTools
  
  if( j.getAttribute<float>("Tau1") != 0 ) m_tau21->Fill( j.getAttribute<float>("Tau2") / j.getAttribute<float>("Tau1") );
  if( j.getAttribute<float>("Tau2") != 0 ) m_tau32->Fill( j.getAttribute<float>("Tau3") / j.getAttribute<float>("Tau2") );

  if( j.getAttribute<float>("ECF1") > 1e-8 ) m_C1->Fill( j.getAttribute<float>("ECF2") / pow( j.getAttribute<float>("ECF1"), 2.0) );
  if( j.getAttribute<float>("ECF2") > 1e-8 ) m_C2->Fill( ( j.getAttribute<float>("ECF3") * j.getAttribute<float>("ECF1") ) / pow( j.getAttribute<float>("ECF2"), 2.0) );
  if( j.getAttribute<float>("ECF2") > 1e-8 ) m_D2->Fill( ( j.getAttribute<float>("ECF3") * pow( j.getAttribute<float>("ECF1"), 3.0 ) ) / pow( j.getAttribute<float>("ECF2"), 3.0) );

  return 0;
}

