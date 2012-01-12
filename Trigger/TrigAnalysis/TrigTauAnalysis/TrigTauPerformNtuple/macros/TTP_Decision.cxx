/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_Decision(TTree * tree)
{
  TCut cut = "Level1==1";
  

  cout << " AAN_Decision.cxx draws Tau Trigger Decision for different levels\n";
 
  // the following code is not really very beautiful but it gets proper info out.
  // if you have a better approach - please share it with me (Olga.Igonkina@cern.ch)

  //  gROOT->cd();
  TH1F * hL1 = new TH1F("hL1","Level1 Tau trigger",9,-0.5,8.5);
  TH1F * hL2 = new TH1F("hL2","Level2 Tau trigger",9,-0.5,8.5);
  TH1F * hEF = new TH1F("hEF","Event Filter Tau trigger",9,-0.5,8.5);

  hL2->SetLineColor(4);
  hEF->SetLineColor(2);

  // axis
  TAxis * axis = hL1->GetXaxis();
  axis->SetBinLabel(1,"All triggers");
  axis->SetBinLabel(2,"tauNoCut");
  axis->SetBinLabel(3,"tau10");
  axis->SetBinLabel(4,"tau10i");
  axis->SetBinLabel(5,"tau15");
  axis->SetBinLabel(6,"tau15i");
  axis->SetBinLabel(7,"tau20i");
  axis->SetBinLabel(8,"tau25i");
  axis->SetBinLabel(9,"tau35i");

  TrigTauEvent * event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch",&event) ;
  tree->SetBranchStatus("*",1) ;

  int nEvt = tree->GetEntries();
  if( nEvt == 0 ) return 1;
  for(int iev = 0; iev<nEvt; ++iev)
    {
      event->reset() ;
      tree->GetEvent(iev) ;

      //event->decision.print();
      
      if( event->decision.passedL1(TrigTauFlags::ANYTRIGGER)) hL1->Fill(0);
      if( event->decision.passedL2(TrigTauFlags::ANYTRIGGER)) hL2->Fill(0);
      if( event->decision.passedEF(TrigTauFlags::ANYTRIGGER)) hEF->Fill(0);

      if( event->decision.passedL1(TrigTauFlags::TAUNOCUT)) hL1->Fill(1);
      if( event->decision.passedL2(TrigTauFlags::TAUNOCUT)) hL2->Fill(1);
      if( event->decision.passedEF(TrigTauFlags::TAUNOCUT)) hEF->Fill(1);

      if( event->decision.passedL1(TrigTauFlags::TAU10)) hL1->Fill(2);
      if( event->decision.passedL2(TrigTauFlags::TAU10)) hL2->Fill(2);
      if( event->decision.passedEF(TrigTauFlags::TAU10)) hEF->Fill(2);

      if( event->decision.passedL1(TrigTauFlags::TAU10I)) hL1->Fill(3);
      if( event->decision.passedL2(TrigTauFlags::TAU10I)) hL2->Fill(3);
      if( event->decision.passedEF(TrigTauFlags::TAU10I)) hEF->Fill(3);

      if( event->decision.passedL1(TrigTauFlags::TAU15)) hL1->Fill(4);
      if( event->decision.passedL2(TrigTauFlags::TAU15)) hL2->Fill(4);
      if( event->decision.passedEF(TrigTauFlags::TAU15)) hEF->Fill(4);

      if( event->decision.passedL1(TrigTauFlags::TAU15I)) hL1->Fill(5);
      if( event->decision.passedL2(TrigTauFlags::TAU15I)) hL2->Fill(5);
      if( event->decision.passedEF(TrigTauFlags::TAU15I)) hEF->Fill(5);

      if( event->decision.passedL1(TrigTauFlags::TAU20I)) hL1->Fill(6);
      if( event->decision.passedL2(TrigTauFlags::TAU20I)) hL2->Fill(6);
      if( event->decision.passedEF(TrigTauFlags::TAU20I)) hEF->Fill(6);

      if( event->decision.passedL1(TrigTauFlags::TAU25I)) hL1->Fill(7);
      if( event->decision.passedL2(TrigTauFlags::TAU25I)) hL2->Fill(7);
      if( event->decision.passedEF(TrigTauFlags::TAU25I)) hEF->Fill(7);

      if( event->decision.passedL1(TrigTauFlags::TAU35I)) hL1->Fill(8);
      if( event->decision.passedL2(TrigTauFlags::TAU35I)) hL2->Fill(8);
      if( event->decision.passedEF(TrigTauFlags::TAU35I)) hEF->Fill(8);

    }
  //hL1->Sumw2();

  hL1->Scale(1./float(nEvt));
  hL2->Scale(1./float(nEvt));
  hEF->Scale(1./float(nEvt));

  setErrors(hL1, nEvt);
  setErrors(hL2, nEvt);
  setErrors(hEF, nEvt);

  hL1->SetTitle("Tau Trigger Efficiencies");
  hL1->Draw();
  hL2->Draw("same");
  hEF->Draw("same");

  TLatex * latex = new TLatex();
  
  float ymax = hL1->GetMaximum();
  latex->DrawText(5,0.9*ymax,"L1");
  latex->SetTextColor(4); latex->DrawText(5,0.8*ymax,"L2");
   latex->SetTextColor(2); latex->DrawText(5,0.7*ymax,"EF");

  return 0;
}


void setErrors(TH1* hist, int nEvt )
{
  if( nEvt < 0.5 ) return;
  int nbin = hist->GetXaxis()->GetNbins();
  for( unsigned ibin = 1; ibin <= nbin; ++ibin )
    {
      float val = hist->GetBinContent(ibin);
      if( val > 1 ) continue;
      float eval = sqrt(val*(1-val)/float(nEvt));
      hist->SetBinError(ibin, eval);      
    }

}
