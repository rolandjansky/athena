/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int plot_rejection() {
  gStyle->SetOptStat(0);
  
  int nlvl = 5;
  int nitem = 7;
  
  int i_colors[5] = {kRed, kBlue, kGreen+2, kBlack, kMagenta+2};
  
  TH1F* h_counts[2][5];//[repro][lvl]
  TH1F* h_rej[2][4];//[iRepro][lvl-1]

  TString lvl_str[5] = {"input","L1","L2Cal","L2Trk","EF"};
  TString item_str[7] ={"tau16_loose",
                        "tau20_medium1",
                        "tau29_loose",
                        "tau29_medium1",
                        "tau100_medium",
                        "tau100_medium1",
                        "tau125_medium"};

  //with pileup noise correction  
  int evtCount_newConstants_newL2_c596_179939[7][5] = {{1223150,336738,240379,101129,60225},
                                                       {1223150,275518,143717, 43824,16064},
                                                       {1223150,216154, 88985, 34842,24561},
                                                       {1223150,216154, 78127, 22001, 7907},
                                                       {1223150,  3838,   791,   248,  103},
                                                       {1223150,  3838,   791,   159,   42},
                                                       {1223150,  3838,   409,   134,   55}};

  //without pileup noise correction
  int evtCount_newConstants_newL2_c600_179939[7][5] = {{1216667,335034,234414,97773,57593},
                                                       {1216667,274114,134206,40751,14442},
                                                       {1216667,215029, 84892,33276,23189},
                                                       {1216667,215029, 73185,20794, 7332},
                                                       {1216667,  3822,   745,  234,   97},
                                                       {1216667,  3822,   745,  151,   43},
                                                       {1216667,  3822,   393,  126,   51}};



  //what to plot
  TString header[2]      = {"New constants, c596", "New constants, c600"};//With and Without noise correction
  TString outfilename[2] = {"_newConstants_c596" , "_newConstants_c600"};
  TString sRepro[2]      = {"c596"               ,"c600"};
  TString runs           = "run 179939";
  int     evtCounts_0[7][5];
  int     evtCounts_1[7][5];

  for (int item=0; item<nitem; item++) for (int lvl=0; lvl<nlvl; lvl++) evtCounts_0[item][lvl] = evtCount_newConstants_newL2_c596_179939[item][lvl];
  for (int item=0; item<nitem; item++) for (int lvl=0; lvl<nlvl; lvl++) evtCounts_1[item][lvl] = evtCount_newConstants_newL2_c600_179939[item][lvl];



  //--------------------
  //global count histogram
  //--------------------
  for (int iRepro=0; iRepro<2; iRepro++) {
    TString canvasname = "counts_";
    canvasname += sRepro[iRepro];
    TCanvas* c_counts = new TCanvas(canvasname,canvasname,1200,610);

    for (int lvl=0; lvl<nlvl; lvl++) {
      TString countsname = "h_";
      countsname += sRepro[iRepro];
      countsname += "_";
      countsname += lvl_str[lvl];

      h_counts[iRepro][lvl] = new TH1F(countsname,countsname,nitem+1,0,nitem+1);
      h_counts[iRepro][lvl]->SetTitle("");
      h_counts[iRepro][lvl]->SetFillColor(i_colors[lvl]);
      h_counts[iRepro][lvl]->SetLineColor(i_colors[lvl]);
      h_counts[iRepro][lvl]->GetXaxis()->SetLabelSize(0.04);
      h_counts[iRepro][lvl]->GetYaxis()->SetTitleSize(0.05);
      h_counts[iRepro][lvl]->GetYaxis()->SetTitle("Event count");

      for (int item=0; item<nitem; item++) {
        h_counts[iRepro][lvl]->Fill(item_str[item],item);
        int counts = evtCounts_0[item][lvl];
        if (iRepro == 1 ) counts = evtCounts_1[item][lvl];
        h_counts[iRepro][lvl]->SetBinContent(item+1,counts);
      }
    }
  

    TLegend* lc = new TLegend(0.81,0.31,0.93,0.86);
    lc->SetHeader(header[iRepro]);
    lc->SetFillColor(0);
    lc->SetFillStyle(0);
    lc->SetLineColor(0);
    lc->SetLineStyle(0);
    

    h_counts[iRepro][0]->Draw();
    lc->AddEntry(h_counts[iRepro][0],lvl_str[0],"f");
    for (int lvl=1; lvl<nlvl; lvl++) {
      h_counts[iRepro][lvl]->Draw("same");
      lc->AddEntry(h_counts[iRepro][lvl],lvl_str[lvl],"f");
    }
    lc->Draw();


    canvasname += outfilename[iRepro];
    canvasname += ".gif";
    c_counts->SaveAs(canvasname);
  }


  //--------------------
  //global rejection histogram
  //--------------------
  for (int iRepro=0; iRepro<2; iRepro++) {
    canvasname = "rejection_";
    canvasname += sRepro[iRepro];
    TCanvas* c_rej = new TCanvas(canvasname,canvasname,1200,610);

    for (int lvl=0; lvl<nlvl-1; lvl++) {
      TString rejname = "hrej_";
      rejname += sRepro[iRepro];
      rejname += "_";
      rejname +=  lvl_str[lvl];

      h_rej[iRepro][lvl] = new TH1F(rejname,rejname,nitem+1,0,nitem+1);
      for (int item=0; item<nitem; item++) h_rej[iRepro][lvl]->Fill(item_str[item],item);

      h_rej[iRepro][lvl]->SetTitle("");
      h_rej[iRepro][lvl]->Sumw2();
      h_rej[iRepro][lvl]->Divide(h_counts[iRepro][lvl],h_counts[iRepro][lvl+1],1.0,1.0,"b");
      h_rej[iRepro][lvl]->SetBinContent(nitem+1,0.0);
      h_rej[iRepro][lvl]->SetBinError(nitem+1,0.0);
      h_rej[iRepro][lvl]->SetMarkerStyle(0);
      h_rej[iRepro][lvl]->SetLineColor(i_colors[lvl+1]);
      h_rej[iRepro][lvl]->SetLineWidth(3);
      h_rej[iRepro][lvl]->SetAxisRange(1.0,12.0,"Y");
      h_rej[iRepro][lvl]->GetXaxis()->SetLabelSize(0.04);
      h_rej[iRepro][lvl]->GetYaxis()->SetTitleSize(0.05);
      h_rej[iRepro][lvl]->GetYaxis()->SetTitle("Rejection factor");
    }
  
    TLegend* lr = new TLegend(0.80,0.11,0.97,0.51);
    lr->SetHeader(header[iRepro]);
    lr->SetFillColor(0);
    lr->SetFillStyle(0);
    lr->SetLineColor(0);
    
    h_rej[iRepro][0]->Draw("E1");
    for (int lvl=0; lvl<nlvl-1; lvl++) {
      h_rej[iRepro][lvl]->Draw("E1same");
      lr->AddEntry(h_rej[iRepro][lvl],lvl_str[lvl+1],"l");
    }
    c_rej->SetGridx();
    c_rej->SetGridy();
    lr->Draw();

    canvasname += outfilename[iRepro];
    canvasname += ".gif";
    c_rej->SaveAs(canvasname);
  }



  //--------------------
  //compare rejections
  // one plot per level
  //--------------------
  int maxRange[4] = {400, 12, 6, 5};
  for (int lvl=0; lvl<nlvl-1; lvl++) {
    canvasname = "rejection_";
    canvasname += lvl_str[lvl+1];
    TCanvas* c_rej = new TCanvas(canvasname,canvasname,1200,610);
    if (lvl == 0 ) c_rej->SetLogy();
  
    TH1F* h_rejec[2];
    for (int iRepro=0; iRepro<2; iRepro++) {
      TString rejname = "hrej_";
      rejname += lvl_str[lvl+1];
      rejname += "_";
      rejname += sRepro[iRepro];
      h_rejec[iRepro] = new TH1F(rejname,rejname,nitem,0,item);

      for (int item=0; item<nitem; item++) {
        h_rejec[iRepro]->Fill(item_str[item],item);
        h_rejec[iRepro]->SetBinContent(item+1,h_rej[iRepro][lvl]->GetBinContent(item+1));
        //h_rejec[iRepro]->SetBinError  (item+1,h_rej[iRepro][lvl]->GetBinError(item+1));
      }

      h_rejec[iRepro]->SetTitle("");
      h_rejec[iRepro]->SetAxisRange(1.0,maxRange[lvl],"Y");
      h_rejec[iRepro]->GetXaxis()->SetLabelSize(0.04);
      h_rejec[iRepro]->GetYaxis()->SetTitleSize(0.05);
      h_rejec[iRepro]->GetYaxis()->SetTitle("Rejection factor");
      h_rejec[iRepro]->SetBarWidth(0.4);
      h_rejec[iRepro]->SetBarOffset(0.05 + iRepro * 0.40);
      h_rejec[iRepro]->SetFillColor(i_colors[iRepro]);
    }    

    h_rejec[0]->Draw("bar1");
    h_rejec[1]->Draw("bar1same");


    TLegend* l = new TLegend(0.13,0.66,0.57,0.88);
    TString head = lvl_str[lvl+1];
    head        += " rejection";
    head        += ", " + runs;
    l->SetHeader(head);
    l->SetFillColor(0);
    l->SetFillStyle(0);
    l->SetLineColor(0);
    l->AddEntry(h_rejec[0],"With pile-up noise correction","f");
    l->AddEntry(h_rejec[1],"Without pile-up noise correction","f");
    l->Draw();

    canvasname += ".gif";
    c_rej->SaveAs(canvasname); 

  } 

  
  return 1;
  
}      


