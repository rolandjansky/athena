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
  m_tau21 = bookHisto( new TH1F(prefixn+"tau21"  ,  "Jet tau21 (GeV);Entries", 100, 0, 200) );


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

//int JetSubStructureHistos::fillHistosFromContainer(const xAOD::JetContainer & cont){
//  // fill the N if needed. 
//  m_tau21->Fill( cont.Tau1 / cont.Tau2() );
//  // Perform the loop over jets in the base class :
//  return JetHistoBase::fillHistosFromContainer(cont);
//}



int JetSubStructureHistos::fillHistosFromJet(const xAOD::Jet &j){
//  xAOD::JetConstituentVector vec1 = j.getConstituents();
  m_tau21->Fill( j.getAttribute<float>("Tau1") / j.getAttribute<float>("Tau2") );

  return 0;
}

