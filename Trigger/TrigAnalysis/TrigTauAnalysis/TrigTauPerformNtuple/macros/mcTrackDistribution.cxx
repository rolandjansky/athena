/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int mcTrackDistribution(TTree * tree)//int id)
{
  gROOT->cd();

  TH1F * hpttrue  = new TH1F("hpttrue" ,"True pT tau",   100,0, 100);
  TH1F * hptvis   = new TH1F("hptvis" , "True visible pT tau",   100,0, 100);
  TH1F * hphi = new TH1F("hphi","True #phi tau", 100,-3.2,3.2);
  TH1F * heta = new TH1F("heta","True #eta tau", 100,-5,5); 
  
  TH1F * hpphi = new TH1F("hdphi","#Delta #phi", 100, -3.2,3.2);
  TH1F * hpeta = new TH1F("hdeta","#Delta #eta", 100,-5,5);
  TH1F * hppt  = new TH1F("hdpt" ,"#Delta pT/pT",   100,0, 2);

  TH1F * hdphi = new TH1F("hdphi","#Delta #phi", 100,-0.5,0.5);
  TH1F * hdeta = new TH1F("hdeta","#Delta #eta", 100,-0.5,0.5); 
  TH1F * hdpt  = new TH1F("hdpt" ,"#Delta pT/pT",   100,0, 1.05);
  TH1F * htr  = new TH1F("htr" ,"N true charged tracks",   10,-0.5,9.5);
  TH1F * hdec  = new TH1F("hdec" ,"tau decay type",   10,-0.5,9.5);
  hdec->GetXaxis()->SetLabelSize(0.07);
  hdec->GetXaxis()->SetBinLabel(1,"e #nu #nu");
  hdec->GetXaxis()->SetBinLabel(2,"#mu #nu #nu");
  hdec->GetXaxis()->SetBinLabel(3,"#rho #nu");
  hdec->GetXaxis()->SetBinLabel(4,"#pi #nu");
  hdec->GetXaxis()->SetBinLabel(5,"3#pi #nu");
  hdec->GetXaxis()->SetBinLabel(6,"K #nu");
  hdec->GetXaxis()->SetBinLabel(7,"other 1-prong");
  hdec->GetXaxis()->SetBinLabel(8,"other");


  TH1F* hW = new TH1F("hW","pT of W",100,0,100000);
  TH1F* hnu = new TH1F("hnu","pT of #nu from W",100,0,100000);

  // -- Initialization ----------------------
  TrigTauEvent * event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch",&event) ;

  tree->SetBranchStatus("*",1) ;

  int nev = int(tree->GetEntries() );
  for(int iev = 0; iev<nev; ++iev)
    {
      event->reset() ;
      tree->GetEvent(iev) ;

      //=============================== Loop over event =================
      //event->mcEvent->print();

      for(unsigned int imc = 0; imc < event->mcEvent.nParticles(); ++imc)
	{
	  TrigTauTrueParticle * tau =  &event->mcEvent.particleListEntry(imc);
	  if( abs(tau->pdgId()) != 15 || // not tau
	      
	      tau->nDaughters() < 2 || tau->nSavedDaughters() ==1 ) // documentary tau
	    continue;

	  // check mother decay
	  TrigTauTrueParticle * moth = tau->mother();
	  if( moth && abs(moth->pdgId()) == 15 ) moth = moth->mother();
	  if( moth && abs(moth->pdgId()) == 24 ) // this is W
	    {
	      hW->Fill(moth->Pt() );
	      for(unsigned int iWdau = 0; iWdau < moth->nSavedDaughters(); ++iWdau)
		{
		  TrigTauTrueParticle * neutrino = &(moth->daughter(iWdau));
		  if(abs(neutrino->pdgId()) != 16 )continue;
		  hnu->Fill(neutrino->Pt() );
		}
	    }


	  hdec->Fill(float(int(tau->decayType())));
	  htr->Fill(float( tau->nSavedDaughters() ));
	  hphi->Fill( float(tau->Phi() ));
	  //heta->Fill( float(tau->Theta() ));
	  heta->Fill( float(tau->Eta() ));
	  hpttrue->Fill( float(tau->Pt()/1000. ));
	  float pxVis = 0;
	  float pyVis = 0;
	  bool strangeDecay = false;
	  //int totCharge = 0;
	  for(unsigned int idau = 0; idau < tau->nSavedDaughters(); ++idau)
	    {
	      TrigTauTrueParticle * dau =  &tau->daughter(idau);
	      int lund = abs(dau->pdgId());
	      //totCharge+= dau->charge();

	      if( lund == 11 || lund== 13 || lund == 211 || lund == 321 || lund == 22 || 
		  lund == 310 || lund ==130 )
		{ pxVis += dau->Px(); pyVis += dau->Py() ; }
	      else if (lund != 12 && lund != 14 && lund != 16 )
		{
		  strangeDecay = true;
		  //cout << "Id="<< dau->pdgId() << ", pt" << dau->Pt() << " eta=" << dau->Eta() 
		  //   << " Moth=" << dau->mother()->pdgId() << endl;
		}

	      float dPhi = tau->Phi() - dau->Phi();
	      float dEta = tau->Eta() - dau->Eta();
	      float dPt  = (tau->Pt() - dau->Pt())/fabs( tau->Pt());
	      hdphi->Fill(dPhi); hdeta->Fill(dEta); hdpt->Fill(dPt);

	      float pPhi = dau->Phi();
	      float pEta = dau->Eta();
	      float pPt  =  dau->Pt();
	      hpphi->Fill(pPhi); hpeta->Fill(pEta); hppt->Fill(pPt/1000.);

	    }
	  float ptVis = sqrt(pxVis*pxVis + pyVis*pyVis);
	  hptvis->Fill(  ptVis/1000. );

// 	  if( strangeDecay )
// 	    // || totCharge != tau->charge)
// 	    { cout << iev <<  " Event with strange decay \n"; event->mcEvent.print(); }


	}

      // end event 
    }// end loop over events



  hdec->Scale(1./float(hdec->GetEntries()));

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(2,2);
  c1->cd(1)  ; hdec->Draw();
  hptvis->SetLineColor(4);
  c1->cd(2)  ; hptvis->Draw();
  c1->cd(2)  ; hpttrue->Draw("same");

  c1->cd(3)  ; heta->Draw();
  c1->cd(4)  ; hphi->Draw();
  c1->Update();
  char answer;
  cout << "Continue (y/n)?" ; cin >> answer;
  if( answer != 'N' && answer != 'n')
    {
      c1->cd(1)  ; htr->Draw();
      //c1->cd(2)  ; hdpt->Draw();
      //c1->cd(3)  ; hdeta->Draw();
      //c1->cd(4)  ; hdphi->Draw();
      c1->cd(2)  ; hppt->Draw();
      //c1->cd(3)  ; hpeta->Draw();
      //c1->cd(4)  ; hpphi->Draw();
      c1->cd(3)  ; hW->Draw();
      c1->cd(4)  ; hnu->Draw();
    }


  return 0;
}
