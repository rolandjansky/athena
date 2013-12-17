// ROOTSYS/tutorials
{
	const unsigned int SBIN=500, RESBIN=60, RBIN=30;
	const float RESRANGE=6.0, SRANGE=0.75, RRANGE=15.0;

gStyle->SetCanvasColor(10);
gStyle->SetPadColor(10);
gStyle->SetCanvasBorderMode(0);
gStyle->SetPadBorderMode(0);

TCanvas *c2 = new TCanvas("c2","c2",1000,1000);
c2->Divide(2,3);
gStyle->SetOptStat(1);
gStyle->SetOptFit(1);

	TFile *top = 0;
//	top = new TFile("run_26462/sprBOL5.root","read");
	top = new TFile("../spr_out.root","read");
	if(!top->IsOpen()) {
		printf("no such file\n");
		return 0;
	}

//	TTree *tree = 0;
//	tree = (TTree *)top->Get("ntuple");

	c2->cd(1); gPad->SetGrid();
	TH2F *residuals2 = 0;
	residuals2 = (TH2F *)top->Get("m_residual2");
	residuals2->SetTitle("INITIAL RESIDUALS");
	residuals2->Draw();


	TH2F *sigma_track ;
	sigma_track = (TH2F *)top->Get("m_sigma_track");

	gStyle->SetOptStat(1);	
	c2->cd(3); gPad->SetGrid();

	sigma_track->SetTitle("TRACK FIT ERROR");
	sigma_track->Draw();

	c2->cd(4); gPad->SetGrid();
	TH2F *sigma_track_lim = (TH2F *)top->Get("m_sigma_track_lim");
	TProfile *tr_lim_prf = (TProfile *)sigma_track_lim->ProfileX();
	sigma_track_lim->SetTitle("TRACK FIT ERROR (SELECTED REGION)");
	sigma_track_lim->Draw();
	tr_lim_prf->SetLineColor(7);
	tr_lim_prf->SetLineWidth(3);
	tr_lim_prf->Draw("SAME");

	c2->cd(5); gPad->SetGrid();
	TH2F *residuals = 0;
	residuals = (TH2F *)top->Get("m_residual");
	residuals->SetTitle("RESIDUALS AFTER REFIT (hit excluded)");
	residuals->Draw();

	c2->cd(6); gPad->SetGrid();
	TH2D *empty = new TH2D("empty","Resolution",1000, 0.0, RRANGE,1000, 0, SRANGE);
	empty->SetTitle("RESOLUTION vs DRIFT RADIUS");
	empty->DrawCopy();
//	tree->Draw("SigmaDriftRadius:DriftRadius>>empty","","SAME P");
	TGraphErrors *sprgr_r=0;
	sprgr_r = (TGraphErrors*)top->Get("sprgr_r");
	sprgr_r->SetMarkerStyle(20);
	sprgr_r->SetMarkerSize(0.7);
	sprgr_r->SetMarkerColor(6);
	sprgr_r->Draw("P SAME");
	TF1 *sprf_r = 0;
	sprf_r = (TF1*)top->Get("sprfun_r");
	sprf_r->SetLineColor(6);
	sprf_r->DrawCopy("SAME");
/*
TCanvas *c1 = new TCanvas("c1","c1",1024,800);
	c1->cd(); gPad->SetGrid();
	TH2F *residuals2 = 0;
	residuals2 = (TH2F *)top->Get("m_residual2");
	residuals2->SetTitle("RESIDUALS vs DRIFT RADIUS");
	residuals2->Draw();
*/
}
