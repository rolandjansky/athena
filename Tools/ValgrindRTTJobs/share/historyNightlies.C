
{
  TGraph *g[8];
  g[0] = new TGraph("report_def.log");
  g[1] = new TGraph("report_ind.log");
  g[2] = new TGraph("report_pos.log");
  g[3] = new TGraph("report_str.log");
  
  g[4] = new TGraph("report_mfd.log");
  g[5] = new TGraph("report_cjm.log");
  g[6] = new TGraph("report_inv.log");
  g[7] = new TGraph("report_stc.log");
  
  TH1D* h[8];
  
  int N;
  
  int mxx(-1);
  int mxn(-1);
  int myx(-1);
  int myn(-1);
  double Xmax(-1);
  double Xmin(1000);
  double Ymax(-1);
  double Ymin(1000);
  for ( int ng=0; ng<8; ++ng )
    {
      N = g[ng]->GetN();
      for ( int nb=0; nb<N; ++nb )
	{
	  double x,y;
	  if ( g[ng]->GetPoint(nb,x,y) == nb )
	    {
	      if ( y > Ymax ) Ymax = y, myx = ng;
	      if ( y < Ymin ) Ymin = y, myn = ng;
	      if ( x > Xmax ) Xmax = x, mxx = ng;
	      if ( x < Xmin ) Xmin = x, mxn = ng;
	    }
	  else
	    {
	      std::cout << "ERROR : " << ng << " " << nb << "\n";
	    }
	}
    }
  int nbins = Xmax-Xmin+1;
  
  for ( int ng=0; ng<8; ++ng )
    {
      h[ng] = new TH1D(g[ng]->GetTitle(),g[ng]->GetName(),14, Xmax-13.5,Xmax+0.5);
      h[ng]->SetBarWidth(0.12);
      h[ng]->SetBarOffset(0.02+0.12*ng);
      h[ng]->SetFillColor(1+ng);
      
      // shift down, if some nightlies weren't available
      for ( int nb=0; nb<14; ++nb )
	h[ng]->SetBinContent(nb+1,-20);
      
      N = g[ng]->GetN();
      for ( int nb=0; nb<N; ++nb )
	{
	  double x,y;
	  if ( g[ng]->GetPoint(nb,x,y) == nb )
	    h[ng]->Fill(x,20+y);
	}
    }
  h[7]->SetFillColor(11);
  
  TCanvas c("nightlies","nightlies",1200,450);
  
  c.SetLeftMargin(0.05);
  c.SetRightMargin(0.04);
  c.SetTopMargin(0.15);
  c.SetBottomMargin(0.1);
  
  h[myx]->SetTitle("Results for last 14 nightlies");
  h[myx]->SetMinimum(-5);
  h[myx]->SetStats(0);
  h[myx]->Draw("bar2");
  
  double hym = 1.05 * h[myx]->GetMaximum();
  TLine tl;
  TText tt; tt.SetTextSize(0.03);
  TString rel(gSystem->Getenv("AtlasVersion"));
  
  char relc = rel(4);
  int reli = 7 + int(relc) - int ('0') + 1;
  std::cout << "rel : " << rel << " " << relc << " " << reli << "\n";
  
  for ( int ng=Xmax-14; ng<Xmax; ++ng )
    {
      TString reln("rel_"); reln += reli % 7; ++reli;
      tl.DrawLine(ng+0.5,0,ng+0.5,hym);
      tt.DrawText(ng+0.75,hym*0.96,reln);
    }
  
  TLegend *leg1=new TLegend(0.25,0.88,0.38,0.98);
  leg1->AddEntry(h[0],"definitely losts","F");
  leg1->AddEntry(h[1],"indirectly losts","F");
  leg1->Draw();
  TLegend *leg1=new TLegend(0.45,0.88,0.58,0.98);
  leg1->AddEntry(h[2],"possibly losts","F");
  leg1->AddEntry(h[3],"still reachable","F");
  leg1->Draw();
  TLegend *leg1=new TLegend(0.65,0.88,0.78,0.98);
  leg1->AddEntry(h[4],"mix. free/delete","F");
  leg1->AddEntry(h[5],"jmp/mv on uninit","F");
  leg1->Draw();
  TLegend *leg1=new TLegend(0.85,0.88,0.98,0.98);
  leg1->AddEntry(h[6],"invalid reads","F");
  leg1->AddEntry(h[7],"athena returns","F");
  leg1->Draw();
  for ( int ng=0; ng<8; ++ng )
    {
      h[ng]->Draw("bar2same");
      for ( int nb=0; nb<14; ++nb )
	{
	  double y = h[ng]->GetBinContent(nb+1);
	  TString cont(""); cont += y;
	  if ( y >= 0 && y < 10 )
	    tt.DrawText(Xmax-13.8+nb+0.12*ng,y*1.03+0.13,cont);
	}
    }
  c.Print("history.eps");
  c.Print("history.gif");
}
