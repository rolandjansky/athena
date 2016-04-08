void track_trt_res(int SaveOpt=0) {

    // Get ntuple file
    TFile *file = new TFile("ntuple.root");
    TTree* tree = (TTree*)gDirectory->Get("FitterValidation/DAF_Validation");
    char name[80],name2[80];

    gStyle->SetCanvasBorderMode(0);

    gStyle->SetPadBorderMode(0);
    gStyle->SetOptFit(1111);

    const int MAX_TRT_HITS = 100;           //max no. of trt hits per track
    //  const int MAX_TRACKS = 60;            //max no. of tracks per event
    const int ITERATIONS = 6;

    // Enabling only tracking branches
    //  tree->SetBranchStatus("*",0);
    //  tree->SetBranchStatus("trk_*",1);
    //  tree->SetBranchStatus("Run",1);

    // Get number of entries
    int entries = tree->GetEntries();
    cout<<entries<<" entries"<<endl;

    // Get pointer to data
    //  int event, ntracks=0;
    //  int run;
    int trt_nhits;
    float trt_res[MAX_TRT_HITS];
    float trt_pull[MAX_TRT_HITS];
    int trk_iteration;

    //  tree->SetBranchAddress("Run", &run);
    //  tree->SetBranchAddress("trk_nTracks", &ntracks);
    tree->SetBranchAddress("nTRTHits", &trt_nhits);
    tree->SetBranchAddress("TRTRes", trt_res);
    tree->SetBranchAddress("TRTPull", trt_pull);
    tree->SetBranchAddress("IterationIndex", &trk_iteration);

    TCanvas *cres = new TCanvas("cres","TRT Residuals");
    cres->Divide(2,3);

    TCanvas *cpull = new TCanvas("cpull","TRT Pulls");
    cpull->Divide(2,3);


    /*
    TPad* c11_1 = (TPad*)(c11->GetPrimitive("c11_1"));
    c11_1->SetFillStyle(4000);
    TPad* c11_2 = (TPad*)(c11->GetPrimitive("c11_2"));
    c11_2->SetFillStyle(4000);
    */

    TH1F *histoRes[ITERATIONS];
    TH1F *histoPull[ITERATIONS];

    //    for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (int iteration=0; iteration<ITERATIONS;iteration++) {



        // get current subcanvas
        sprintf(name,"cres_%d",iteration);
        cres->cd(iteration+1);
        TPad* ressubcanvas = (TPad*)(cres->GetPrimitive(name));
        //subcanvas->SetLogz();
        //ressubcanvas->SetFillStyle(4000);

        // set name for histo
        sprintf(name,"TRT Residual - iteration %d",iteration);
        sprintf(name2,"residual[%d]",iteration);
        //gStyle->SetOptStat(100000);

        // get histo for hits
        //histo2 = new TH2F(name2,name,20,0.,144.,328/ROW_RESOLUTION,0.,328.);
        //histo2->SetStats(kFALSE); // statistics in hostogram off

        histoRes[iteration] = new TH1F(name2,name,100,-4.,4.);
        histoRes[iteration]->SetXTitle("r = R_{track} - R_{hit} [mm]");
        histoRes[iteration]->GetXaxis()->SetTitleFont(22);


        // update canvas
        cres->Update();



        sprintf(name,"cpull_%d",iteration);
        cpull->cd(iteration+1);
        TPad* pullsubcanvas = (TPad*)(cpull->GetPrimitive(name));
        //pullsubcanvas->SetFillStyle(4000);

        //subcanvas->SetLogz();

        // set name for histo
        sprintf(name,"TRT Pulls - iteration %d",iteration);
        sprintf(name2,"pull[%d]",iteration);
        //gStyle->SetOptStat(100000);

        // get histo for hits
        //histo2 = new TH2F(name2,name,20,0.,144.,328/ROW_RESOLUTION,0.,328.);
        //histo2->SetStats(kFALSE); // statistics in hostogram off

        histoPull[iteration] = new TH1F(name2,name,100,-8.,8.);
        histoPull[iteration]->SetXTitle("#frac{ r }{ #sqrt{ #sigma_{track}^{2} + #sigma_{hit}^{2} } }");
        histoPull[iteration]->GetXaxis()->SetTitleSize(0.03);
        histoPull[iteration]->GetXaxis()->SetTitleOffset(1.);
        histoPull[iteration]->GetXaxis()->SetTitleFont(22);


        // update canvas
        cpull->Update();

    }
    //}




    //int hit;

    for (int entry=0; entry<entries; entry++) {
        // Get data for current event
        tree->GetEntry(entry);
        cout<<trt_nhits<<" TRT hits for Iteration "<<trk_iteration<<endl;
        // loop over all trt hits of current track
        for (int hit=0; hit<trt_nhits; hit++) {
            histoRes[trk_iteration]->Fill(trt_res[hit]);
            histoPull[trk_iteration]->Fill(trt_pull[hit]);
        }
    }

    for (int iteration=0; iteration<ITERATIONS;iteration++) {

        cres->cd(iteration+1);
        histoRes[iteration]->SetFillStyle(4000);
        histoRes[iteration]->Draw();
        histoRes[iteration]->Fit("gaus");
        histoRes[iteration]->GetFunction("gaus")->SetLineColor(kRed);
        histoRes[iteration]->GetFunction("gaus")->SetLineWidth(2);
        cpull->cd(iteration+1);
        histoPull[iteration]->SetFillStyle(4000);
        histoPull[iteration]->Draw();
        histoPull[iteration]->Fit("gaus");
        histoPull[iteration]->GetFunction("gaus")->SetLineColor(kRed);
        histoPull[iteration]->GetFunction("gaus")->SetLineWidth(2);
    }


    //   cres->cd();
    //   //sprintf(name,"%d events, Run %d", entries, run);
    //   //sprintf(name,"Run %d", run);
    //   ptres = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
    //   ptres->SetBorderSize(0);
    //   ptres->SetFillColor(20);
    //   text = ptres->AddText(name);
    //
    //   sprintf(name,"%d events", entries);
    //   text = ptres->AddText(name);
    //   ptres->Draw();
    //   cres->Update();
    //
    //   cpull->cd();
    //   //sprintf(name,"%d events, Run %d", entries, run);
    //   //sprintf(name,"Run %d", run);
    //   ptpull = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
    //   ptpull->SetBorderSize(0);
    //   ptpull->SetFillColor(20);
    //   text = ptpull->AddText(name);
    //
    //   sprintf(name,"%d events", entries);
    //   text = ptpull->AddText(name);
    //   ptpull->Draw();
    //   cpull->Update();

    //  if (SaveOpt==1) c11->Print("TRT_track_res_pulls.ps");
    //  else if(SaveOpt==2) {sprintf(sname,"TRT_track_res_pulls.gif"); c11->Print(sname); }
    //  else if(SaveOpt==3) c11->Print(psfilename);
}

