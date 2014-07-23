/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{


    TFile* f = new TFile("SF_higheta_MUON.root","RECREATE");
    f->cd();

    float lumi = 0;
    std::string period = "";
    TTree* t = new TTree("LumiData","Lumis");
    t->Branch("IntLumi",&lumi);
    t->Branch("Period",&period);

    period = "All";
    lumi = 1;
    t->Fill();

    // sf: latest run 1 version
    Double_t ptbins [7] = {0,20,30,40,50,60,1e12};
    Double_t sf [6] = {0.995,1.016,0.996,0.997,1.008,0.984};
    Double_t sfstat [6] = {0.016,0.010,0.006,0.006,0.015,0.022};
    Double_t sfsys [6] = {0.003,0.001,0.002,0.002,0.001,0.003};

    // eff: placeholders with uncertainties covering the rough magnitude
    Double_t eff [6] = {1,1,1,1,1,1};
    Double_t effstat [6] = {0.1,0.1,0.1,0.1,0.1,0.1};
    Double_t effsys [6] = {0.1,0.1,0.1,0.1,0.1,0.1};

    int nptbins = sizeof(ptbins)/sizeof(Double_t) - 1;

    TH1F* Eff = new TH1F("Eff_All","Eff_All",nptbins,ptbins);
    Eff->GetXaxis()->SetTitle("p_{T} [GeV]");

    TH1F* SF = new TH1F("SF_All","SF_A",nptbins,ptbins);
    SF->GetXaxis()->SetTitle("p_{T} [GeV]");

    TH1F* Eff_sys = new TH1F("Eff_sys_All","Eff_sys_A",nptbins,ptbins);
    Eff_sys->GetXaxis()->SetTitle("p_{T} [GeV]");

    TH1F* SF_sys = new TH1F("SF_sys_All","SF_sys_A",nptbins,ptbins);
    SF_sys->GetXaxis()->SetTitle("p_{T} [GeV]");

    for (int k = 0; k < 7;k++){
        Eff->SetBinContent(k+1, eff[k]);
        SF->SetBinContent(k+1, sf[k]);
        Eff_sys->SetBinContent(k+1, effsys[k]);
        SF_sys->SetBinContent(k+1, sfsys[k]);
        Eff->SetBinError(k+1, effstat[k]);
        SF->SetBinError(k+1, sfstat[k]);
    }


    f->Write();
    f->Close();
}
