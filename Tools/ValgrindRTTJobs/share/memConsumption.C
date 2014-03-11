
{
  TGraph gm("mem");
  gm.SetTitle("Memory [Mb] allocated by memcheck / #color[3]{CPU percentage};approx. runtime [s]");
  double xm(0), ym(0);
  for( int i=0; i<gm.GetN(); i++ )
    {
      double x,y;
      gm.GetPoint(i,x,y);
      gm.SetPoint(i,x,y*0.001);
      if ( y > ym )
	ym=y, xm=x;
    }
  ym/=1000;
  // std::cout << ym << " " << xm << "\n";
  /*
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetFillColor(42);
  pad->SetGrid();
  pad->Draw();
  pad->cd();
  
  TH1F *hr = pad->DrawFrame(-0.4,0,1.2,12);
  hr->SetXTitle("X title");
  hr->SetYTitle("Y title");
  pad->GetFrame()->SetFillColor(21);
  pad->GetFrame()->SetBorderSize(12);
  */
  TLine tl;
  tl.SetLineColor(kRed);
  gm.Draw("AC");
  tl.DrawLine(0,ym,xm,ym);
  tl.DrawLine(xm,ym,xm,0);
  TText tt;
  tt.SetTextSize(0.025);
  tt.SetTextColor(kRed);
  TString s("max="); s+=ym;
  tt.DrawText(xm/100,ym,s);
  
  TGraph gc("cpu");
  c1->cd();
  double pxl,pyl,pxh,pyh;
  gPad->GetRange(pxl,pyl,pxh,pyh);
  // TPad *overlay = new TPad("overlay","",0,0,1,1);
  TPad *overlay = new TPad("overlay","",pxl,pyl,pxh,pyh);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  overlay->Draw();
  overlay->cd();
  Double_t xmin = gPad->GetUxmin();
  Double_t ymin = 0;
  Double_t xmax = gPad->GetUxmax();
  Double_t ymax = 120;
  xmax=gm.GetXaxis().GetXmax();
  std::cout << "R : " << xmax << " - " << xmin << "\n";
  TH1F *hframe = overlay->DrawFrame(xmin,ymin,xmax,ymax);
  hframe->GetXaxis()->SetLabelOffset(99);
  hframe->GetYaxis()->SetLabelOffset(99);
  gc.SetLineColor(kGreen);
  gc->Draw("LP");
  TGaxis *axis = new TGaxis(xmax,ymin,xmax, ymax,ymin,ymax,510,"+L");
  axis->SetLineColor(kGreen);
  axis->SetLabelColor(kGreen);
  axis->Draw();
  c1->Print("mem.eps");
  c1->Print("mem.gif");
}
