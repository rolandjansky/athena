/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void align_dia() {

  if (fileResults!=NULL) {

    cout << "Diagonalization information..." << endl;

    cDia = new TCanvas("AlignDia","Diagonalization Info",CanvasSizeX6[0],CanvasSizeX6[1]);
    cDia->Divide(3,2);

    sprintf(name,"mode>=%d",iModCut);
    TCut m_modCut = name;
    sprintf(name,"mode<%d",iModCut);
    TCut m_modCut2 = name;

    cDia->cd(1);
    TLine *TLineBorder;
    TLineBorder = new TLine(0.02,0.98,0.98,0.98);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.02,0.02,0.98,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.02,0.98,0.02,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.98,0.98,0.98,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();

    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(font);
    t->SetTextSize(0.05);
    t->SetTextAlign(12);
    sprintf(name,"Solving and Diagonalization information");
    t->DrawLatex(0.08,0.90,name);
    t->SetTextSize(0.035);

    TLine *line1;
    line1 = new TLine(0.06,0.87,0.95,0.87);
    line1->Draw();

    // Information
    t->SetTextSize(0.04);
    sprintf(name," + %s",aSetup);
    t->DrawLatex(0.08,0.82,name);
    sprintf(name,"AlignProcessLevel: %d",iAlignProcessLevel);
    t->DrawLatex(0.08,0.77,name);
    
    if (treeResultsSummary!=NULL) {

      sprintf(name,"AlignSolveLevel: %d",iAlignSolveLevel);
      t->DrawLatex(0.08,0.73,name);
      sprintf(name,"HitCut: %d",iHitCut);
      t->DrawLatex(0.08,0.69,name);
      sprintf(name,"Number of aligned parameters: %d",iAlignPars);
      t->DrawLatex(0.08,0.65,name);
      sprintf(name,"Number of aligned modules: %d",alignedModules);
      t->DrawLatex(0.08,0.61,name);
      sprintf(name," - Pixel aligned modules: %d",alignedPIXModules);
      t->DrawLatex(0.08,0.57,name);
      sprintf(name," - SCT aligned modules: %d",alignedSCTModules);
      t->DrawLatex(0.08,0.53,name);
      sprintf(name,"Number of DoFs: %d",iDoF);
      t->DrawLatex(0.08,0.49,name);
      sprintf(name,"ModCut: %d",iModCut);
      t->DrawLatex(0.08,0.45,name);
    }

    if (iSoftScaled!=999) {
      sprintf(name,"SoftModeCut applied");
      t->DrawLatex(0.08,0.40,name);

      /*      
      sprintf(name,"SoftModeCut Values (SoftCut = %2.2f)", iSoftCut);
      t->DrawLatex(0.23,0.39,name);
      
      TLine *line1;
      line1 = new TLine(0.26,0.35,0.75,0.35);
      line1->Draw();

      sprintf(name,"#sigma(Tx)_{pixel}");
      t->DrawLatex(0.08,0.31,name);
      */
    }
    else {
      sprintf(name,"No SoftModeCut applied");
      t->DrawLatex(0.08,0.40,name);
    }
 

    //
    // -----------------------------------------------------------------
    //

    // Get minimum values
    if (treeResultsSummary!=NULL) {
      float m_eigenvalue = 0.0;
      float m_error = 0.0;
    }
    else {
      double m_eigenvalue = 0.0;
      double m_error = 0.0;
    }

    int ivalue=0;
    treeDia->SetBranchAddress("eigenvalue", &m_eigenvalue);
    treeDia->SetBranchAddress("error_eigenmode", &m_error);
    treeDia->GetEntry(ivalue);
    // printf("%d: eigenvalue: %2.10f\n",ivalue,m_eigenvalue);
    while (m_eigenvalue<0.0) {
      ivalue++;
      treeDia->GetEntry(ivalue);
      // printf("%d: eigenvalue: %2.10f\n",ivalue,m_eigenvalue);
    }
    // treeDia->GetEntry(ivalue);
    // printf("%d: eigenvalue: %2.10f\n",ivalue,m_eigenvalue);
    // treeDia->Scan();
    float m_min = pow(10,floor(log10(m_eigenvalue)));
    float m_min2 = pow(10,floor(log10(m_error)))*0.1;
    // printf("error: %2.6f\n",m_error);
    // printf("min: %2.6f\n",m_min);
    // printf("min2: %2.6f\n",m_min2);
    // treeDia->Show(0);
   
    //
    // -----------------------------------------------------------------
    //

    cDia->cd(2);
    treeDia->Draw("eigenvalue:mode","","goff");
    TGraph *histo_eigen = new TGraph(treeDia->GetSelectedRows(), treeDia->GetV2(), treeDia->GetV1());
    histo_eigen->SetTitle("Eingenvalue spectrum");
    histo_eigen->GetXaxis()->SetTitle("index (mode)");
    histo_eigen->GetYaxis()->SetTitle("eingenvalue");
    histo_eigen->SetMarkerStyle(4);
    histo_eigen->SetMarkerSize(0.2);
    histo_eigen->SetLineWidth(1);
    histo_eigen->GetHistogram()->SetMinimum(m_min);
    histo_eigen->GetXaxis()->SetRangeUser(0.0,(float) iDoF-1);
    gPad->SetLogy();
    histo_eigen->Draw("AL");
    setTitleFont(font);
    drawVerticalLine(histo_eigen,iModCut);

    //
    // -----------------------------------------------------------------
    //

    cDia->cd(3);
    treeDia->Draw("eigenmode:mode","","goff");
    TGraph *histo_value = new TGraph(treeDia->GetSelectedRows(), treeDia->GetV2(), treeDia->GetV1());
    histo_value->SetTitle("#deltab Vs index (eingenmode Vs mode)");
    histo_value->GetXaxis()->SetTitle("index (mode)");
    histo_value->GetYaxis()->SetTitle("#deltab (eigenmode)");
    histo_value->GetXaxis()->SetRangeUser(0.0,(float) iDoF-1);
    histo_value->Draw("AL");
    setTitleFont(font);
    setTitleSize(0.61);
    drawVerticalLine(histo_value,iModCut);

    //
    // -----------------------------------------------------------------
    //

    cDia->cd(5);
    treeDia->Draw("(eigenmode/error_eigenmode):mode","","goff");
    TGraph *histo_pull = new TGraph(treeDia->GetSelectedRows(), treeDia->GetV2(), treeDia->GetV1());
    histo_pull->SetTitle("#deltab/#epsilon(#deltab) Vs index (Pull spectrum)");
    histo_pull->GetXaxis()->SetTitle("index (mode)");
    histo_pull->GetYaxis()->SetTitle("#deltab/#epsilon(#deltab)");
    histo_pull->GetXaxis()->SetRangeUser(0.0,(float) iDoF-1);
    histo_pull->Draw("AL");
    setTitleFont(font);
    setTitleSize(0.61);
    drawVerticalLine(histo_pull,iModCut);

    //
    // -----------------------------------------------------------------
    //

    cDia->cd(6);
    treeDia->Draw("eigenmode/error_eigenmode>>hDiaPull","","goff");
    hDiaPull->SetTitle("#deltab/#epsilon(#deltab) (eingenmode pulls)");
    hDiaPull->GetYaxis()->SetTitle("entries");
    hDiaPull->GetXaxis()->SetTitle("#deltab/#epsilon(#deltab)");
    hDiaPull->SetLineColor(kRed+2);
    hDiaPull->SetFillColor(kRed);
    hDiaPull->SetFillStyle(3001);
    hDiaPull->SetLineWidth(1);
    hDiaPull->DrawCopy();
    setTitleFont(font);

    treeDia->Draw("eigenmode/error_eigenmode>>hDiaPull2",m_modCut,"goff");
    hDiaPull2->SetLineWidth(1);
    hDiaPull2->SetFillColor(kGray);
    hDiaPull2->SetLineColor(kGray+2);
    hDiaPull2->SetFillStyle(3001);
    hDiaPull2->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.67,0.65,0.98,0.78);
    sprintf(name,"All modes");
    leg0->AddEntry(hDiaPull,name,"F");
    sprintf(name,"Included modes");
    leg0->AddEntry(hDiaPull2,name,"F");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    //
    // -----------------------------------------------------------------
    //

    cDia->cd(4);
    treeDia->Draw("error_eigenmode:mode","","goff");
    TGraph *histo_errorEigenmode = new TGraph(treeDia->GetSelectedRows(), treeDia->GetV2(), treeDia->GetV1());
    histo_errorEigenmode->SetTitle("#epsilon(#deltab) spectrum");
    histo_errorEigenmode->GetXaxis()->SetTitle("index (mode)");
    histo_errorEigenmode->GetYaxis()->SetTitle("#epsilon(#deltab)");
    histo_errorEigenmode->SetMarkerStyle(4);
    histo_errorEigenmode->SetMarkerSize(0.2);
    histo_errorEigenmode->SetLineWidth(1);
    histo_errorEigenmode->GetHistogram()->SetMinimum(m_min*0.1);
    histo_errorEigenmode->GetXaxis()->SetRangeUser(0.0,(float) iDoF-1);
    gPad->SetLogy();
    histo_errorEigenmode->Draw("AL");
    setTitleFont(font);
    drawVerticalLine(histo_errorEigenmode,iModCut);

    cDia->Update();
  }

  return;
}

