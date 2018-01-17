/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Fill histograms for some of HLT xAOD containers from converted Run 1 data, to
// compare them with the AOD reference.
//

//#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h"

#include "TrigAnalysisTest/Run1BStoxAODTrigger.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// to read HLT containers
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

// Root headers
#include "TMath.h"
#include "TH1F.h"
#include "TFile.h"
#include <iostream>

using namespace std;
using namespace Trig;
using namespace xAOD;

namespace TrigAnalysisTest {
  //
  // Init counters and get ready to run.
  //
  Run1BStoxAODTrigger::Run1BStoxAODTrigger()
    : m_passed_l1(0),
      m_passed_hlt(0)
      //m_first_call(true)
  {
    m_histFile = new TFile("hist.root", "RECREATE");
    if (m_histFile == nullptr || !m_histFile->IsOpen()) {
      cerr << "unable to open file for writing, exiting" << endl;
      exit(1);
    }

    // bphys
    m_HLT_EFBMuMuXFex_eta = new TH1F( "HLT_EFBMuMuXFex_eta", "HLT_EFBMuMuXFex;#eta;No. of TrigEFBphys objects", 100, -4., 4. );
    m_HLT_EFBMuMuXFex_mass = new TH1F( "HLT_EFBMuMuXFex_mass", "HLT_EFBMuMuXFex;#M (GeV);No. of TrigEFBphys objects", 100, 0., 20. );
    // taus
    m_HLT_TrigTauJet = new TH1F( "HLT_TrigTauJet", "HLT_TrigTauJet;p_{T} (GeV);Jets", 100, 0., 500. );
    // muons
    m_HLT_MuonEF_NSegments = new TH1F( "HLT_MuonEF_NSegments", "HLT_MuonEF_NSegments;NSegments;Muons", 100, 0., 100.);
    m_HLT_MuonEF_pt = new TH1F( "HLT_MuonEF_pt", "HLT_MuonEF_pt;p_{T} (GeV);Muons", 100, 0., 100.);
    // MET
    m_HLT_TrigEFMissingET = new TH1F( "HLT_TrigEFMissingET", "HLT_TrigEFMissingET;E_{T} (GeV);Entries", 100, 0., 200.);
    m_HLT_TrigEFMissingET_FEB = new TH1F( "HLT_TrigEFMissingET_FEB", "HLT_TrigEFMissingET_FEB;E_{T} (GeV);Entries", 100, 0., 200. );
    m_HLT_TrigEFMissingET_topocl = new TH1F( "HLT_TrigEFMissingET_topocl", "HLT_TrigEFMissingET_topocl;E_{T} (GeV);Entries", 100, 0., 200. );
    // all trigger bits. The binning is changed in finalize() when the total number of triggers is known
    m_all_triggers = new TH1F("all_triggers", ";Trigger;No. of events", 1, 0., 1.);
  }

  // Actually process an event.
  void Run1BStoxAODTrigger::processEvent( TrigDecisionTool &trigDecTool) {

    // Did we pass a trigger level?
    if (trigDecTool.isPassed("L1_.*"))
      m_passed_l1++;
    if (trigDecTool.isPassed("EF_.*"))
      m_passed_hlt++;

    auto chainGroups = trigDecTool.getChainGroup(".*");
    for(auto &trig : chainGroups->getListOfTriggers()) {
      if(!m_trigger_counts.count(trig))
	m_trigger_counts[trig] = 0;
      if(trigDecTool.isPassed(trig)) {
	m_trigger_counts[trig] += 1;
	cout << "Trigger passed: " << trig << endl;
      }
    }

    //
    //extract trigger containers and fill histograms
    //

    //    bphys
    // HLT_EFBMuMuXFex
    const xAOD::TrigBphysContainer* HLT_xAOD__TrigBphysContainer_EFBMuMuXFex = 0;
    eventStore()->retrieve( HLT_xAOD__TrigBphysContainer_EFBMuMuXFex, "HLT_xAOD__TrigBphysContainer_EFBMuMuXFex" );
    cout << "size of HLT_xAOD__TrigBphysContainer_EFBMuMuXFex is " << HLT_xAOD__TrigBphysContainer_EFBMuMuXFex->size() << endl;
    xAOD::TrigBphysContainer::const_iterator TrigBphys_iter;
    for(TrigBphys_iter = HLT_xAOD__TrigBphysContainer_EFBMuMuXFex->begin(); TrigBphys_iter != HLT_xAOD__TrigBphysContainer_EFBMuMuXFex->end(); ++TrigBphys_iter) {
      cout << "eta, mass = "<< (*TrigBphys_iter)->eta() << ", "<< (*TrigBphys_iter)->mass() << endl;
      m_HLT_EFBMuMuXFex_eta->Fill( (*TrigBphys_iter)->eta() );
      m_HLT_EFBMuMuXFex_mass->Fill( (*TrigBphys_iter)->mass() );
    }

    //    taus
    // HLT_TrigTauJet
    const xAOD::JetContainer* HLT_xAOD__JetContainer_TrigTauJet = 0;
    eventStore()->retrieve( HLT_xAOD__JetContainer_TrigTauJet, "HLT_xAOD__JetContainer_TrigTauJet" );//weird name
    cout << "HLT_xAOD__JetContainer_TrigTauJet successfully retrieved - size is " << HLT_xAOD__JetContainer_TrigTauJet->size() << " jets" << endl;
    xAOD::JetContainer::const_iterator Jet_iter;
    for(Jet_iter = HLT_xAOD__JetContainer_TrigTauJet->begin(); Jet_iter != HLT_xAOD__JetContainer_TrigTauJet->end(); ++Jet_iter) {
      cout <<  "tau jet eta, pt: " << (*Jet_iter)->eta() << ", " << (*Jet_iter)->pt() << endl;
      m_HLT_TrigTauJet->Fill( (*Jet_iter)->pt() / 1000. );
    }


    //    muons
    // HLT_MuonEFInfo
    const xAOD::MuonContainer* HLT_xAOD__MuonContainer_MuonEFInfo = 0;
    eventStore()->retrieve( HLT_xAOD__MuonContainer_MuonEFInfo, "HLT_xAOD__MuonContainer_MuonEFInfo");
    cout << "size of HLT_xAOD__MuonContainer_MuonEFInfo is " << HLT_xAOD__MuonContainer_MuonEFInfo->size() << endl;
    xAOD::MuonContainer::const_iterator Muon_iter;
    for(Muon_iter = HLT_xAOD__MuonContainer_MuonEFInfo->begin(); Muon_iter!= HLT_xAOD__MuonContainer_MuonEFInfo->end(); ++Muon_iter) {
      cout << "muon n segments, pt: " << (*Muon_iter)->nMuonSegments() << ", " << (*Muon_iter)->pt() << endl;
      m_HLT_MuonEF_NSegments->Fill( float((*Muon_iter)->nMuonSegments()) );
      m_HLT_MuonEF_pt->Fill( (*Muon_iter)->pt() / 1000. );
    }


    //    MET
    // HLT_xAOD__TrigMissingETContainer_TrigEFMissingET
    const xAOD::TrigMissingETContainer* HLT_xAOD__TrigMissingETContainer_TrigEFMissingET = 0;
    eventStore()->retrieve( HLT_xAOD__TrigMissingETContainer_TrigEFMissingET, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET" );
    xAOD::TrigMissingETContainer::const_iterator TrigMissingET_iter;
    cout << "Size of TrigMissingETContainer is " << HLT_xAOD__TrigMissingETContainer_TrigEFMissingET->size() << endl;
    for(TrigMissingET_iter = HLT_xAOD__TrigMissingETContainer_TrigEFMissingET->begin(); TrigMissingET_iter != HLT_xAOD__TrigMissingETContainer_TrigEFMissingET->end(); ++TrigMissingET_iter) {
      Double_t ex = (*TrigMissingET_iter)->ex();
      Double_t ey = (*TrigMissingET_iter)->ey();
      Double_t et = TMath::Sqrt(ex*ex + ey*ey);
      cout << " missing et = " << et << endl;
      m_HLT_TrigEFMissingET->Fill( et / 1000. );
    }
    // HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB
    const xAOD::TrigMissingETContainer* HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB = 0;
    eventStore()->retrieve( HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB" );
    cout << "Size of TrigMissingETContainer is " << HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB->size() << endl;
    for(TrigMissingET_iter = HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB->begin(); TrigMissingET_iter != HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB->end(); ++TrigMissingET_iter) {
      Double_t ex = (*TrigMissingET_iter)->ex();
      Double_t ey = (*TrigMissingET_iter)->ey();
      Double_t et = TMath::Sqrt(ex*ex + ey*ey);
      cout << " missing et = " << et << endl;
      m_HLT_TrigEFMissingET_FEB->Fill( et / 1000. );
    }
    // HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl
    const xAOD::TrigMissingETContainer* HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl = 0;
    eventStore()->retrieve( HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl" );
    cout << "Size of TrigMissingETContainer is " << HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl->size() << endl;
    for(TrigMissingET_iter = HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl->begin(); TrigMissingET_iter != HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl->end(); ++TrigMissingET_iter) {
      Double_t ex = (*TrigMissingET_iter)->ex();
      Double_t ey = (*TrigMissingET_iter)->ey();
      Double_t et = TMath::Sqrt(ex*ex + ey*ey);
      cout << " missing et = " << et << endl;
      m_HLT_TrigEFMissingET_topocl->Fill( et / 1000. );
    }

    cout << "    --- Event processing done ---   \n" << endl;
  }

  // Dump everything and return a status.
  int Run1BStoxAODTrigger::finalize()
  {
    //athena doesn't store m_all_triggers without this
    m_histFile->cd();

    // Dump for debugging.
    cout << "L1 triggers passed: " << m_passed_l1 << endl;
    cout << "EF triggers passed: " << m_passed_hlt << endl;
    cout << "START SUMMARY: Run1BStoxAODTrigger" << endl;
    cout << "creating histogram with one bin per trigger, n of bins is " << m_trigger_counts.size() << endl;
    // Set the correct binning
    m_all_triggers->SetBins( m_trigger_counts.size(), 0.0, m_trigger_counts.size() );
    cout << "histogram created" << endl;
    int trigger_counter = 0;
    for (auto it = m_trigger_counts.begin(); it!=m_trigger_counts.end(); it++) {
      cout << (*it).first << "\t" << (*it).second <<endl;
      trigger_counter++;
      m_all_triggers->GetXaxis()->SetBinLabel( trigger_counter, ((*it).first).c_str() );
      m_all_triggers->SetBinContent( trigger_counter, (*it).second );
    }
    cout << "END SUMMARY  : Run1BStoxAODTrigger" << endl;

    m_histFile->Write();
    m_histFile->Close();

    // our aim here is to test the conversion of variables
    return 0;
  }

}
