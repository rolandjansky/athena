/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @fn test
 * @brief creates monitored.roo file
 */
void test() {

	TFile * mon = new TFile("monitored.root", "RECREATE");

	TDirectory * histosDir = mon->mkdir("histos");
 	TDirectory * errorsDir = mon->mkdir("errors");
	TDirectory * graphsDir = mon->mkdir("graphs");
	TDirectory * profilesDir = mon->mkdir("profiles");


	// histos
	histosDir->cd();
	TH1D * th1d = new TH1D("1-dim hist", "1-dim hist", 50,  -3,  3);
	TH2D * th2d = new TH2D("2-dim hist", "2-dim hist", 50, -3,  3, 50, -3,  3);

	th1d->FillRandom("gaus", 5500 );

	TF2 * xygaus = new TF2( "f2", "xygaus" );
	xygaus->SetParameters( 10.0, 0.0, 1.0, 0.0, 1.0 );
	th2d->FillRandom("f2", 12000 );

	TH1D * normth1d = new TH1D("norm 1-dim hist", "norm 1-dim hist", 50, -3, 3);
	normth1d->FillRandom("gaus", 5500);

	TH2D * normth2d = new TH2D("norm 2-dim hist", "norm 2-dim hist", 50, -3, 3, 50, -3, 3 );
	normth2d->FillRandom("f2", 12000);

	TH1D * logyth1d = new TH1D("logy 1-dim hist", "logy 1-dim hist", 50, -3, 3);
	logyth1d->FillRandom("gaus", 5500);
	
	TH2D * logzth2d = new TH2D("logz 2-dim hist", "logz 2-dim hist", 50, -3, 3, 50, -3, 3 );
	logzth2d->FillRandom("f2", 5500);

	// graphs
	graphsDir->cd();

	TGraph * tGraph = new TGraph();
	tGraph->SetName("1-dim graph");
	tGraph->SetTitle("1-dim graph");
	TRandom rnd;
	for ( int i = 0; i < 20; i++) {
	  Double_t y = rnd.Rndm() * i;
	  tGraph->SetPoint( i, i, y);
	}
	tGraph->Write();

	TGraphErrors * tGraphErr = new TGraphErrors();
	tGraphErr->SetTitle("1-dim with symmetric errors");
	tGraphErr->SetName("1-dim with symmetric errors");
	for ( int i = 0; i < 20; i++ ) {
	  Double_t x = 0.1 * i;
	  Double_t y = x*x + 1.5;
	  Double_t ex = x * rnd.Rndm();
	  Double_t ey = y * rnd.Rndm();
	  tGraphErr->SetPoint(i, x, y );
	  tGraphErr->SetPointError( i, ex, ey );
	}
	tGraphErr->Write();


	TGraphAsymmErrors * tGraphAsymmErrors = new TGraphAsymmErrors();
	tGraphAsymmErrors->SetTitle( "1-dim with asymmetric errors");
	tGraphAsymmErrors->SetName( "1-dim with asymmetric errors");
	
	for ( int i = 0; i < 20; i++) {
	  Double_t x = i;
	  Double_t y = x*x + 1.5;
	  Double_t exl = rnd.Rndm() * x;
	  Double_t exr = rnd.Rndm() * x;
	  Double_t eyl = rnd.Rndm() * y;
	  Double_t eyh = rnd.Rndm() * y;
	  tGraphAsymmErrors->SetPoint( i, x, y );
	  tGraphAsymmErrors->SetPointError( i, exl, exr, eyl, eyh );
       	}
	tGraphAsymmErrors->Write();


	/*TGraph2D * tGraph2D = new TGraph2D();
	tGraph2D->SetName( "2-dim" );
	tGraph2D->SetTitle( "2-dim" );
	int k = 0;
	for (int i = -10; i < 10; i++ ) {
	  for (int j =-10; j < 10; j++ ) {
	    Double_t x = i;
	    Double_t y = j*0.9;
	    Double_t z = sin( x*x + y*y - 0.75 );
	    tGraph2D->SetPoint(k, x, y, z);
	    k++;
	  }
	}
	tGraph2D->Write();
	*/

	
	// profiles
	profilesDir->cd();
	TProfile * tprof1d = new TProfile( "1-dim profile", "1-dim profile", 100, -4, 4 , 0, 20);
	TProfile2D * tprof2d = new TProfile2D( "2-dim profile", "2-dim profile", 100, -4, 4, 100, -4, 4 );
    
	Double_t x,y,z;
	for( int i =0; i < 22000; i++) {
	  gRandom->Rannor( x, y );
	  z = x*x + y*y;
	  tprof1d->Fill( x, z, 1 );
	  tprof2d->Fill( x, y, z );
	}
	//tprof1d->Write();
	//tprof2d->Write();
	
       
	// errors
	errorsDir->cd();
	
	TH1D * wrongBin = new TH1D("bad binning", "bad binning", 50, 0, 100);
	//wrongBin->Write();

	TH2D * wrongClass = new TH2D("diff class", "diff class", 100, 0, 100, 100, 0, 100);
	//wrongClass->Write();
	
	mon->Write();
	mon->Close();

}
