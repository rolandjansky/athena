{

gROOT->Reset();

TCanvas PrintCanvas("RunSum", "Run Summary",400,100,600,600);

// 111 = portrait ps, 112 = landscape ps, 113 = eps
TPostScript *ps = new TPostScript("RunSum.ps",111); 

PrintCanvas->Divide(2,2);
ps->NewPage();

gDirectory->cd("/pedsamp0/febAdc");

if(gDirectory->Get("feb0")) {
  PrintCanvas->cd(1);
  feb0->GetXaxis()->SetLabelSize(0.05);
  feb0->GetYaxis()->SetLabelSize(0.04);
  feb0->Draw("HIST");
}

if(gDirectory->Get("feb1")) {
  feb1->GetXaxis()->SetLabelSize(0.05);
  feb1->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(2);
  feb1->Draw("HIST");
}

if(gDirectory->Get("feb2")) {
  feb2->GetXaxis()->SetLabelSize(0.05);
  feb2->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(3);
  feb2->Draw("HIST");
}

if(gDirectory->Get("feb3")) {
  feb3->GetXaxis()->SetLabelSize(0.05);
  feb3->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(4);
  feb3->Draw("HIST");
}

PrintCanvas->Update();
ps->NewPage();

gDirectory->cd("/pedsamp0/febMaxSamp");

if(gDirectory->Get("feb0")) {
  PrintCanvas->cd(1);
  feb0->GetXaxis()->SetLabelSize(0.05);
  feb0->GetYaxis()->SetLabelSize(0.04);
  feb0->Draw("HIST");
}

if(gDirectory->Get("feb1")) {
  feb1->GetXaxis()->SetLabelSize(0.05);
  feb1->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(2);
  feb1->Draw("HIST");
}

if(gDirectory->Get("feb2")) {
  feb2->GetXaxis()->SetLabelSize(0.05);
  feb2->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(3);
  feb2->Draw("HIST");
}

if(gDirectory->Get("feb3")) {
  feb3->GetXaxis()->SetLabelSize(0.05);
  feb3->GetYaxis()->SetLabelSize(0.04);
  PrintCanvas->cd(4);
  feb3->Draw("HIST");
}

PrintCanvas->Update();
ps->Close();

}
