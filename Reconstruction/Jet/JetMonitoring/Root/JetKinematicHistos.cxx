/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetKinematicHistos.h"
#include "TProfile2D.h"
#include "TString.h"
#include <map>

#define toGeV 1/1000.

JetKinematicHistos::JetKinematicHistos(const std::string &t) : JetHistoBase(t) 
                                                             ,m_pt(0)
                                                             ,m_eta(0)
                                                             ,m_phi(0)
                                                             ,m_m(0)
                                                             ,m_e(0)
                                                             , m_jetScale(xAOD::JetAssignedScaleMomentum)
{
  declareProperty("JetScale", m_jetScale);
  declareProperty("PlotNJet", m_doN = false);
  declareProperty("PlotM", m_doM = true);
  declareProperty("PlotE", m_doE = false);
  declareProperty("PlotOccupancy", m_doOccupancy = false);
  declareProperty("PlotAveragePt", m_doAveragePt = false);
  declareProperty("PlotAverageE", m_doAverageE = false);
  declareProperty("PlotNConstit", m_doNConstit = true);

}


int JetKinematicHistos::buildHistos(){

  // -------------- Modify histo names/titles --------
  //  if we're plotting a defined scale, modify the Histo titles and names
  
  std::map<xAOD::JetScale, std::string> scale2str( { 
      { xAOD::JetEMScaleMomentum , "EMScale" } , 
      { xAOD::JetConstitScaleMomentum , "ConstitScale" } } );
  TString scaleTag = scale2str[ (xAOD::JetScale) m_jetScale ] ; // defaults to ""

  
  TString prefixn = scaleTag;
  if(prefixn != "") prefixn +="_";
  // -------------- 

  // Build and register the histos in this group : 
  TH1::AddDirectory(kFALSE); // Turn off automatic addition to gDirectory to avoid Warnings. Histos are anyway placed in their own dir later.
  m_pt  = bookHisto( new TH1F(prefixn+"pt"  ,  "Jet P_{T};P_{T} (GeV);Entries", 100,0,200) );
  m_eta = bookHisto( new TH1F(prefixn+"eta" ,  "Jet #eta;#eta;Entries", 50,-6,6) );
  m_phi = bookHisto( new TH1F(prefixn+"phi" ,  "Jet #phi;#phi;Entries", 50,-3.3,3.3) );

  // high pT
  m_pt_high  = bookHisto( new TH1F(prefixn+"pt_high"  ,  "Jet P_{T} pT.gt.200 GeV;P_{T} (GeV);Entries", 100,0,2000) );
  m_eta_high = bookHisto( new TH1F(prefixn+"eta_high" ,  "Jet #eta pT.gt.200 GeV;#eta;Entries", 50,-6,6) );
  

  if(m_doM) {
               m_m     = bookHisto( new TH1F(prefixn+"M"   ,  "Jet Mass;Mass (GeV);Entries", 80,-5, 400) );
               m_m_high     = bookHisto( new TH1F(prefixn+"M_high"   ,  "Jet Mass pT.gt.200;Mass (GeV) pT.gt.200 GeV;Entries", 80,-5, 400) );
             }

  if(m_doE) {
               m_e     = bookHisto( new TH1F(prefixn+"E"   ,  "Jet Energy;Energy (GeV);Entries", 100,1, 400) );
               m_e_high     = bookHisto( new TH1F(prefixn+"E_high"   ,  "Jet Energy pT.gt.200 GeV;Energy (GeV);Entries", 100,1, 2000) );
             }

  if(m_doN) m_njet  = bookHisto( new TH1F(prefixn+"num"  ,  "Jet number;Number of jets;Entries", 40,0,40) );  
  if(m_doOccupancy) m_occupancyEtaPhi = bookHisto( new TH2F(prefixn+"OccupancyEtaPhi", "Occupancy;#eta;#phi;Entries", 50,-5,5,50,-3.1416,3.1416) );
  if(m_doAveragePt) m_averagePtEtaPhi = bookHisto( new TProfile2D(prefixn+"AveragePtEtaPhi", "Average P_{T};#eta;#phi;Entries", 50,-5,5,50,-3.1416,3.1416) );
  if(m_doAverageE) m_averageE_EtaPhi = bookHisto( new TProfile2D(prefixn+"AverageE_EtaPhi", "Average E;#eta;#phi;Entries", 50,-5,5,50,-3.1416,3.1416) );

  if(m_doNConstit) {
                    m_nConstit = bookHisto( new TH1F(prefixn+"numconstit", "Number of constituents;N;",100,0,100) );
                    m_nConstit_high = bookHisto( new TH1F(prefixn+"numconstit_high", "Number of constituents pT.gt.200 GeV;N;",100,0,200) );
                   }
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



int JetKinematicHistos::fillHistosFromContainer(const xAOD::JetContainer & cont){
  // fill the N if needed. 
  if (m_doN) m_njet->Fill( cont.size() );
  // Perform the loop over jets in the base class :
  return JetHistoBase::fillHistosFromContainer(cont);
}


int JetKinematicHistos::fillHistosFromJet(const xAOD::Jet &j){

  // m_jetScale is a property of the base tool
  const xAOD::JetFourMom_t p4 = j.jetP4( (xAOD::JetScale) m_jetScale);
  m_pt->Fill( p4.Pt()*toGeV );
  m_eta->Fill( p4.Eta() );
  m_phi->Fill( p4.Phi() );
  if (p4.Pt()*toGeV > 200.0){ // high eta 
                         m_pt_high->Fill( p4.Pt()*toGeV );
                         m_eta_high->Fill( p4.Eta() );
                         if(m_doE) m_e_high->Fill( p4.E()*toGeV );
                         if(m_doM) m_m_high->Fill( p4.M()*toGeV );
                         if(m_doNConstit) m_nConstit_high->Fill( j.numConstituents() );
                      }


  if(m_doE) m_e->Fill( p4.E()*toGeV );
  if(m_doM) m_m->Fill( p4.M()*toGeV );
  
  if(m_doOccupancy) m_occupancyEtaPhi->Fill( p4.Eta(), p4.Phi() );
  if(m_doAveragePt) m_averagePtEtaPhi->Fill( p4.Eta(), p4.Phi() , p4.Pt()*toGeV);
  if(m_doAverageE) m_averageE_EtaPhi->Fill( p4.Eta(), p4.Phi() , p4.E()*toGeV);

  if(m_doNConstit) m_nConstit->Fill( j.numConstituents() );
  return 0;
}

