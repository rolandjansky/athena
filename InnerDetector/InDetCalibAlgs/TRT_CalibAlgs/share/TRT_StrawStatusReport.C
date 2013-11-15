void print(char *figname){

	char filename[1000]; 
	sprintf(filename, "output/%s", figname);
	
	c1->Print( Form("%s.png", filename) );
	c1->Print( Form("%s.pdf", filename) );
	c1->Print( Form("%s.eps", filename));				
}


void TRT_StrawStatusReport(){		
	
	// read the content from a default txt file name into an ntuple
	
	int run = 0;
	TNtuple *ntuple = new TNtuple("ntuple", "data", "side:phi:straw:status:hits:occ:htocc:eff");;
	
	int var[15];
	double par[5];
	int count = 0;
	
	ifstream in;
	in.open( "TRT_StrawStatusReport.txt" );
		
	while(1) {
		
		for (int i=0; i<5; i++) in >> var[i];		   
		for (int i=0; i<3; i++) in >> par[i];		   
		if (!in.good()) break;	   
		count++;
		
		if (var[0]==0) {
			run = var[4];
			continue;
		}
		ntuple->Fill( var[0], var[1], var[2], var[3], var[4], par[0], par[1], par[2] );
	}
	
	printf("read %d lines from file\n", count);
	in.close();		
	
	// make the efficiency distribution plot
	
	TCanvas *c1 = new TCanvas();
	c1->SetLogy(1);
	
	TH1F *h1 = new TH1F("h1", "", 100, 0., 1.);
	ntuple->Project("h1", "eff");
	h1->GetXaxis()->SetTitle( "Straw hit efficiency" );
	h1->GetYaxis()->SetTitle( Form("Number of straws (run %d)", run) );
	h1->Draw();
	
	TH1F *h1a = new TH1F("h1a", "", 100, 0., 1.);
	ntuple->Project("h1a", "eff", "status>0");	
	h1a->SetFillStyle(1001);
	h1a->SetFillColor(kOrange);
	h1a->Draw("same");
	
	TH1F *h1b = new TH1F("h1b", "", 100, 0., 1.);
	ntuple->Project("h1b", "eff", "status==42");	
	h1b->SetFillStyle(1001);
	h1b->SetFillColor(2);
	h1b->Draw("same");
	
	gPad->RedrawAxis();
	
	print( Form("strawHitEfficiency_%d", run) );
//	return;		
	
	delete h1; 
	delete h1a;
	delete h1b;	
	
	// make the HT occupancy distribution plot
	
	double htrange = 0.1;
	TH1F *h1 = new TH1F("h1", "", 100, 0., htrange);
	ntuple->Project("h1", "htocc");
	h1->GetXaxis()->SetTitle( "Straw HT occupancy" );
	h1->GetYaxis()->SetTitle( Form("Number of straws (run %d)", run) );
	h1->SetBinContent(100, h1->GetBinContent(100)+h1->GetBinContent(101) );
	h1->Draw();
	
	TH1F *h1a = new TH1F("h1a", "", 100, 0., htrange);
	ntuple->Project("h1a", "htocc", "status>0");	
	h1a->SetFillStyle(1001);
	h1a->SetFillColor(kOrange);
	h1a->SetBinContent(100, h1->GetBinContent(100)+h1->GetBinContent(101) );
	h1a->Draw("same");
	
	TH1F *h1b = new TH1F("h1b", "", 100, 0., htrange);
	ntuple->Project("h1b", "htocc", "status==52");	
	h1b->SetFillStyle(1001);
	h1b->SetFillColor(2);
	h1b->SetBinContent(100, h1->GetBinContent(100)+h1->GetBinContent(101) );
	h1b->Draw("same");
	
	gPad->RedrawAxis();
	
	print( Form("strawHToccupancy_%d", run) );

	delete h1; 
	delete h1a;
	delete h1b;	

	
	gStyle->SetPalette(1);	
	c1->SetRightMargin(0.2);
	c1->SetLogy(0);
		
	ntuple->Draw("hits>>h7", "straw>=147&&straw<200&&eff>0.7");
	double range = h7->GetMean();
	TH2F *h2 = new TH2F("h2", "", 110, -0.5, 5481.5, 100, 0., range*1.2);
	ntuple->Project("h2", "hits:straw", "status==0");
	h2->GetXaxis()->SetTitle("Straw index");
	h2->GetYaxis()->SetTitle("Number of hits on track");
	h2->GetZaxis()->SetTitleOffset(1.4);
	h2->GetZaxis()->SetTitle( Form("Number of straws (run %d)", run) );
	h2->Draw("colz");
	ntuple->SetMarkerStyle(20);
	ntuple->SetMarkerSize(0.8);
	ntuple->SetMarkerColor(1);
	ntuple->Draw("hits:straw", "status==52||status==51", "same");
	
	TLatex l; 
	l.SetTextSize(0.03);		
	l.DrawLatex(800, range*1.1, "black points: straws excluded due to low / high HT occupancy");				
	
	print( Form("numberOfHitsOnTrack_%d", run) );
	
	delete h2;
	
	
	TH2F *h2 = new TH2F("h2", "", 5, -2.5, 2.5, 32, -0.5, 31.5);
	ntuple->Project("h2", "phi:side", "status>1");
	h2->GetXaxis()->SetTitle( Form("Run %d              Detector side (athena numbering)", run) );
	h2->GetYaxis()->SetTitle("Detector phi (athena 0-31 range)");
	h2->GetZaxis()->SetTitleOffset(1.4);
	h2->GetZaxis()->SetTitle("Number of additional excluded straws");
	h2->Draw("colz");
	
	print( Form("additionalExcludedStraws_%d", run) );
	
	return;	
}
