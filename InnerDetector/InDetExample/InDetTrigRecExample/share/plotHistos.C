void plotHistos()
{
 const Color_t padColor = 0; 
 const Color_t canColor = 920;
 const Color_t histoColor = 600;

 TFile f("EFID_RTT_Histos.root");

 //------------------------------- Efficiency plots ----------------------------------------------------------

 TCanvas *c1 = new TCanvas("c1","Histogram Drawing Options",200,10,700,900);
 c1->SetFillColor(canColor);

 TPad *pad1 = new TPad("pad1","The pad with the function",0.03,0.51,0.97,0.92,21);
 pad1->SetFillColor(padColor);

 TPad *pad2 = new TPad("pad2","The pad with the histogram",0.03,0.02,0.97,0.49,21);
 pad2->SetFillColor(padColor);

 pad1->Draw();
 pad2->Draw();

 pad2->cd();
 TH1F* effvsd0 = (TH1F*)f.Get("Efficiency vs. d0");
 effvsd0->GetXaxis()->SetTitle("d0 [mm]");
 effvsd0->GetYaxis()->SetTitle("Efficiency");
 effvsd0->SetLineColor(histoColor);
 effvsd0->SetLineWidth(2);
 effvsd0->Draw();
 
 pad1->cd();
 TH1F* effvsz0 = (TH1F*)f.Get("Efficiency vs. z0");
 effvsz0->GetXaxis()->SetTitle("z0 [mm]");
 effvsz0->GetYaxis()->SetTitle("Efficiency");
 effvsz0->SetLineColor(histoColor);
 effvsz0->SetLineWidth(2);
 effvsz0->GetYaxis()->SetRangeUser(0.4,1.1);
 effvsz0->Draw();
 
 c1->Update();
 c1->Print("Efficiency.eps");
 
 //------------------------------- Fake rate plots ----------------------------------------------------------
 pad2->cd();
 TH1F* fakeRatevsd0 = (TH1F*)f.Get("Fake_Rate vs. d0");
 fakeRatevsd0->GetXaxis()->SetTitle("d0 [mm]");
 fakeRatevsd0->GetYaxis()->SetTitle("Fake rate");
 fakeRatevsd0->SetLineColor(histoColor);
 fakeRatevsd0->SetLineWidth(2);
 fakeRatevsd0->GetYaxis()->SetRangeUser(0.,1.1);
 fakeRatevsd0->Draw();
 pad2->Update(); 

 pad1->cd();
 TH1F* fakeRatevsz0 = (TH1F*)f.Get("Fake_Rate vs. z0");
 fakeRatevsz0->GetXaxis()->SetTitle("z0 [mm]");
 fakeRatevsz0->GetYaxis()->SetTitle("Fake rate");
 fakeRatevsz0->SetLineColor(histoColor);
 fakeRatevsz0->SetLineWidth(2);
 fakeRatevsz0->GetYaxis()->SetRangeUser(0.,0.4);
 fakeRatevsz0->Draw();
 pad1->Update();

 c1->Update();
 c1->Print("FakeRate.eps");
 
 //------------------------------------- Resolution ----------------------------------------------------------
 TH1F* resEta = (TH1F*)f.Get("dEta_of_Matched_Tracks");
 resEta->GetXaxis()->SetTitle("#eta_{EF} - #eta_{Off}");
 resEta->GetYaxis()->SetTitle("Number of tracks");

 pad1->cd();
 resEta->SetFillColor(histoColor);
 resEta->Draw();
 pad1->Update(); 

 TH1F* resPhi = (TH1F*)f.Get("dPhi_of_Matched_Tracks");
 resPhi->GetXaxis()->SetTitle("#phi_{EF} - #phi_{Off}");
 resPhi->GetYaxis()->SetTitle("Number of tracks");

 pad2->cd();
 resPhi->SetFillColor(histoColor);
 resPhi->Draw();
 pad2->Update();

 c1->Update();
 c1->Print("Resolution.eps");
}
