// Root Macro for ripping out stuff from iPatRec histogramming
// Author: Me (= Tom)

{
    gROOT->Reset();
    gStyle->SetOptFit(111111);

    TFile ntuple("iPatHist.root");

    TH1F* diffa0B      		= (TH1F*)gDirectory->Get("151");
    TH1F* diffa0E      		= (TH1F*)gDirectory->Get("251");
    TH1F* diffz0B      		= (TH1F*)gDirectory->Get("152");
    TH1F* diffz0E      		= (TH1F*)gDirectory->Get("252");
    TH1F* diffiPtB     		= (TH1F*)gDirectory->Get("155");
    TH1F* diffiPtE     		= (TH1F*)gDirectory->Get("255");
    TH1F* pull_a0B 		= (TH1F*)gDirectory->Get("141");
    TH1F* pull_a0E 		= (TH1F*)gDirectory->Get("241");
    TH1F* pull_z0B 		= (TH1F*)gDirectory->Get("142");
    TH1F* pull_z0E 		= (TH1F*)gDirectory->Get("242");
    TH1F* pull_phi0B 		= (TH1F*)gDirectory->Get("143");
    TH1F* pull_phi0E 		= (TH1F*)gDirectory->Get("243");
    TH1F* pull_cotThetaB       	= (TH1F*)gDirectory->Get("144");
    TH1F* pull_cotThetaE       	= (TH1F*)gDirectory->Get("244");
    TH1F* pull_iPtB	       	= (TH1F*)gDirectory->Get("145");
    TH1F* pull_iPtE	       	= (TH1F*)gDirectory->Get("245");

    diffa0B->Fit("gaus", " ", " " , -0.1, 0.1);
    diffa0E->Fit("gaus", " ", " " , -0.1, 0.1);
    diffz0B->Fit("gaus", " ", " " , -0.8, 0.8);
    diffz0E->Fit("gaus", " ", " " , -0.8, 0.8);
    diffiPtB->Fit("gaus", " ", " " , 0.90, 1.10);
    diffiPtE->Fit("gaus", " ", " " , 0.80, 1.20);
    pull_a0B->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_z0B->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_phi0B->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_cotThetaB->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_iPtB->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_a0E->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_z0E->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_phi0E->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_cotThetaE->Fit("gaus", " ", " " , -3.0, 3.0);
    pull_iPtE->Fit("gaus", " ", " " , -3.0, 3.0);

    TH2F* btresid               = (TH2F*)gDirectory->Get("164");
    btresid->ProfileX();
    TH2F* etresid               = (TH2F*)gDirectory->Get("264");
    etresid->ProfileX();
 
    TBrowser b;
}

