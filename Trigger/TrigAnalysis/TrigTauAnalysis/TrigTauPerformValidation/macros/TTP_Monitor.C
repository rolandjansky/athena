/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_Monitor()
{
  TChain chain("eventTree");

  TrigTauEvent* event = new TrigTauEvent();
  chain.SetBranchAddress("EventBranch", &event);
  chain.SetBranchStatus("*", 1);
  chain.Add("*.ttp.root");

  // Create output file
  TFile * out = new TFile("Monitor_histograms_ttp.root","RECREATE");

  //declare histograms
  TH1F* h_NL1=new TH1F("h_NL1","N L1 ",10, 0, 10);
  TH1F* h_L1Eta=new TH1F("h_L1Eta","L1 eta",100, -3, 3);
  TH1F* h_L1Phi=new TH1F("h_L1Phi","L1 phi",100, -3.5, 3.5);
  TH1F* h_L1Et=new TH1F("h_L1Et","L1 et",100, 0, 100);

  TH1F* h_NL2=new TH1F("h_NL2","N L2 ",10, 0, 10);
  TH1F* h_L2Eta=new TH1F("h_L2Eta","L2 eta",100, -3, 3);
  TH1F* h_L2Phi=new TH1F("h_L2Phi","L2 phi",100, -3.5, 3.5);
  TH1F* h_L2Et=new TH1F("h_L2Et","L2 et",100, 0, 100);
  TH1F* h_L2EmEn=new TH1F("h_L2EmEn","L2 EM energy",100, 0, 100);
  TH1F* h_L2HadEn=new TH1F("h_L2HadEn","L2 Had energy",100, 0, 100);
  TH1F* h_L2nStrips=new TH1F("h_L2nStrips","L2 nStrips",2000, 0, 2000);
  TH1F* h_L2stripWidth2=new TH1F("h_L2stripWidth2","L2 stripsWidth2",100, 0, 0.3);
  TH1F* h_L2EMRadius=new TH1F("h_L2EMRadius","L2 EM Rad",100, 0, 1);
  TH1F* h_L2isoFrac=new TH1F("h_L2isoFrac","L2 IsoFrac",100, 0, 1);

  TH1F* h_NEF=new TH1F("h_NEF","N EF ",10, 0, 10);
  TH1F* h_EFEta=new TH1F("h_EFEta","EF eta",100, -3, 3);
  TH1F* h_EFPhi=new TH1F("h_EFPhi","EF phi",100, -3.5, 3.5);
  TH1F* h_EFEt=new TH1F("h_EFEt","EF et",100, 0, 100);
  TH1F* h_EFEmEn=new TH1F("h_EFEmEn","EF EM energy",100, 0, 100);
  TH1F* h_EFHadEn=new TH1F("h_EFHadEn","EF Had energy",100, 0, 100);
  TH1F* h_EFnStrips=new TH1F("h_EFnStrips","EF nStrips",2000, 0, 2000);
  TH1F* h_EFstripWidth2=new TH1F("h_EFstripWidth2","EF stripsWidth2",100, 0, 0.3);
  TH1F* h_EFEMRadius=new TH1F("h_EFEMRadius","EF EM Rad",100, 0, 1);
  TH1F* h_EFisoFrac=new TH1F("h_EFisoFrac","EF IsoFrac",100, 0, 1);


  //loop over events
  int nev = int(chain.GetEntries() );

  for(int iev = 0; iev<nev; ++iev)
    {
      event->reset() ;
      chain.GetEvent(iev) ;

      //at L1
      int nL1 = event->nTauL1();
      int ntauL1=0;
      for(unsigned int iL1 = 0; iL1 < nL1 ; ++iL1)
	{
	  TrigTauL1Object * l1 = &event->tauL1Entry(iL1);
	  h_L1Eta->Fill( l1->Eta() );
          h_L1Phi->Fill( l1->Phi() );
	  h_L1Et->Fill( l1->m_TauEnergy/1000 );
	  ntauL1++;
	}
      h_NL1->Fill(ntauL1);
      //at L2
      int nL2 = event->nTauL2();
      int ntauL2=0;
      for(unsigned int iL2 = 0; iL2 < nL2 ; ++iL2)
        {
          TrigTauL2Object * l2 = &event->tauL2Entry(iL2);
          h_L2Eta->Fill( l2->Eta() );
          h_L2Phi->Fill( l2->Phi() );
          h_L2Et->Fill( l2->m_EtCalib/1000 );
          h_L2EmEn->Fill( l2->m_EMEnergy/1000 );
          h_L2HadEn->Fill( l2->m_HadEnergy/1000 );

	  h_L2EMRadius->Fill( l2->m_EMRadius );
	  h_L2isoFrac->Fill( l2->m_isoFrac );
	  h_L2nStrips->Fill( l2->m_nStrips );
	  h_L2stripWidth2->Fill( l2->m_stripWidth2 );

	  ntauL2++;
        }
      h_NL2->Fill(ntauL2);
      //at EF
      int nEF = event->nTauEF();
      int ntauEF=0;
      for(unsigned int iEF = 0; iEF < nEF ; ++iEF)
        {
          TrigTauRecEFObject * lef = &event->tauEFEntry(iEF);
          h_EFEta->Fill( lef->Eta() );
          h_EFPhi->Fill( lef->Phi() );
          h_EFEt->Fill( lef->m_ETcalo/1000 );

	  h_EFEmEn->Fill( lef->m_etEMCalib/1000 );
          h_EFHadEn->Fill( lef->m_etHadCalib/1000 );

          h_EFEMRadius->Fill( lef->m_EMRadius );
          h_EFisoFrac->Fill( lef->m_isoFrac );
          h_EFnStrips->Fill( lef->m_nStrips );
          h_EFstripWidth2->Fill( lef->m_stripWidth2 );

	  ntauEF++;
       }
      h_NEF->Fill(ntauEF);

     }//end of loop over entries
   out->Write();  out->Close();

}
