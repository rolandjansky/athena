/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PixelModuleComparison(TString& file1name, TString& file2name, int bins)
{

//================ Trying out the style ===================/


   TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
   // Canvas
   plain->SetCanvasBorderMode(0);
   plain->SetCanvasColor(0);
   // Pad
   plain->SetPadBorderMode(0);
   plain->SetPadBottomMargin(0.1);
   plain->SetPadTopMargin(0.15);
   plain->SetPadLeftMargin(0.1);
   plain->SetPadRightMargin(0.15);


   plain->SetPadColor(0);
   // Title
   plain->SetTitleColor(0);
   plain->SetTitleBorderSize(0);
   plain->SetTitleX(0.25);
   plain->SetTitleY(0.98);
   // Stat
   //plain->SetStatColor(1);
   plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");

   gROOT->SetStyle("Plain");

// =======================================================/


 // File 1 declarations ===================================
   TFile* file1 = TFile::Open(file1name);
   // Declaration of leave types
   Float_t         f1_Local1;
   Float_t         f1_Local2;
   Float_t         f1_CorrFactor;
   Float_t         f1_PathInX0;
   Float_t         f1_Rho;


   

 // File 2 declarations ====================================
   Float_t         f2_Local1;
   Float_t         f2_Local2;
   Float_t         f2_CorrFactor;
   Float_t         f2_PathInX0;
   Float_t         f2_Rho;

   TFile* file2 = TFile::Open(file2name);

   TString  treeName           = "SurfaceValidation_100000500";
   TString  moduleCanvasName   = "ModuleCanvas_";
            moduleCanvasName  += treeName;
   TString  moduleCanvasTitle  =  "Pixel Module Layer ";
            moduleCanvasTitle += treeName;

   TCanvas* moduleCanvas     = new TCanvas(moduleCanvasName,moduleCanvasTitle,100,100,600,920);
   moduleCanvas->Divide(2,3);

   TTree* tree1 = file1->Get(treeName);
   TTree* tree2 = file2->Get(treeName);


   if (tree1 && tree2){

     std::cout << " [m] both trees loaded ! " << std::endl;

     moduleCanvas->cd(1);
        // get the min/max values for histogram booking: loc1
        tree1->Draw("Local1>>hl1", "CorrFactor>0.");
        TH1F* hl1 = (TH1F*)gDirectory->Get("hl1");
        double minLoc1 = hl1->GetXaxis()->GetXmin();
        double maxLoc1 = hl1->GetXaxis()->GetXmax();
        delete hl1; hl1 = 0;
        // get the min/max vlues for histogram bookin: loc2
        tree1->Draw("Local2>>hl2", "CorrFactor>0.");
        TH1F* hl2 = (TH1F*)gDirectory->Get("hl2");
        double minLoc2 = hl2->GetXaxis()->GetXmin();
        double maxLoc2 = hl2->GetXaxis()->GetXmax();
        delete hl2; hl2 = 0;

      // (1) the Profile x0 vs loc1
      TString f1_tVSl1ProfileName  = "f1_thicknessInX0";
              f1_tVSl1ProfileName += "_vs_loc1_";
              f1_tVSl1ProfileName += treeName;
      TString f1_tVSl1ProfileTitle            = "t/X_0 vs. ";
              f1_tVSl1ProfileTitle += "loc1 ";
      TProfile* f1_tVSl1Profile = new TProfile(f1_tVSl1ProfileName, f1_tVSl1ProfileTitle,bins,minLoc1,maxLoc1);

      // (2) the Profile x0 vs loc2
      TString f1_tVSl2ProfileName  = "f1_thicknessInX0";
              f1_tVSl2ProfileName += "_vs_loc2_";
              f1_tVSl2ProfileName += treeName;
      TString f1_tVSl2ProfileTitle            = "t/X_0 vs. ";
              f1_tVSl2ProfileTitle += "loc2 ";
      TProfile* f1_tVSl2Profile = new TProfile(f1_tVSl2ProfileName, f1_tVSl2ProfileTitle,bins,minLoc2,maxLoc2);

      // (3) the Profile rho vs loc1
      TString f1_rhoVSl1ProfileName  = "f1_rho";
              f1_rhoVSl1ProfileName += "_vs_loc1_";
              f1_rhoVSl1ProfileName += treeName;
      TString f1_rhoVSl1ProfileTitle            = "#rho vs. ";
              f1_rhoVSl1ProfileTitle += "loc1 ";
      TProfile* f1_rhoVSl1Profile = new TProfile(f1_rhoVSl1ProfileName, f1_rhoVSl1ProfileTitle,bins,minLoc1,maxLoc1);

      // (4) the Profile rho vs loc2
      TString f1_rhoVSl2ProfileName  = "f1_rho";
              f1_rhoVSl2ProfileName += "_vs_loc2_";
              f1_rhoVSl2ProfileName += treeName;
      TString f1_rhoVSl2ProfileTitle            = "#rho vs. ";
              f1_rhoVSl2ProfileTitle += "loc2 ";
      TProfile* f1_rhoVSl2Profile = new TProfile(f1_rhoVSl2ProfileName, f1_rhoVSl2ProfileTitle,bins,minLoc2,maxLoc2);

      // (5) the 2D Profile x0 vs. phi, (z || r) 
      TString f1_tVSl1l2ProfileName  = "f1_thicknessInX0";
              f1_tVSl1l2ProfileName += "_vs_loc1_vs_loc2";
              f1_tVSl1l2ProfileName += treeName;
      TString f1_tVSl1l2ProfileTitle            = "t/X_0 vs. ";
              f1_tVSl1l2ProfileTitle += "( loc1, loc2 ), ";
              f1_tVSl1l2ProfileTitle += treeName;
      TProfile2D* f1_tVSl1l2Profile = new TProfile2D(f1_tVSl1l2ProfileName, 
                                               f1_tVSl1l2ProfileTitle,
                                               bins,minLoc1,maxLoc1,
                                               bins,minLoc2,maxLoc2);

      // (1) the Profile x0 vs loc1
      TString f2_tVSl1ProfileName  = "f2_thicknessInX0";
              f2_tVSl1ProfileName += "_vs_loc1_";
              f2_tVSl1ProfileName += treeName;
      TString f2_tVSl1ProfileTitle            = "t/X_0 vs. ";
              f2_tVSl1ProfileTitle += "loc1 ";
      TProfile* f2_tVSl1Profile = new TProfile(f2_tVSl1ProfileName, f2_tVSl1ProfileTitle,bins,minLoc1,maxLoc1);

      // (2) the Profile x0 vs loc2
      TString f2_tVSl2ProfileName  = "f2_thicknessInX0";
              f2_tVSl2ProfileName += "_vs_loc2_";
              f2_tVSl2ProfileName += treeName;
      TString f2_tVSl2ProfileTitle            = "t/X_0 vs. ";
              f2_tVSl2ProfileTitle += "loc2 ";
      TProfile* f2_tVSl2Profile = new TProfile(f2_tVSl2ProfileName, f2_tVSl2ProfileTitle,bins,minLoc2,maxLoc2);

      // (3) the Profile rho vs loc1
      TString f2_rhoVSl1ProfileName  = "f2_rho";
              f2_rhoVSl1ProfileName += "_vs_loc1_";
              f2_rhoVSl1ProfileName += treeName;
      TString f2_rhoVSl1ProfileTitle = "#rho vs. ";
              f2_rhoVSl1ProfileTitle += "loc1 ";
      TProfile* f2_rhoVSl1Profile = new TProfile(f2_rhoVSl1ProfileName, f2_rhoVSl1ProfileTitle,bins,minLoc1,maxLoc1);

      // (4) the Profile rho vs loc2
      TString f2_rhoVSl2ProfileName  = "f2_rho";
              f2_rhoVSl2ProfileName += "_vs_loc2_";
              f2_rhoVSl2ProfileName += treeName;
      TString f2_rhoVSl2ProfileTitle            = "#rho vs. ";
              f2_rhoVSl2ProfileTitle += "loc2 ";
      TProfile* f2_rhoVSl2Profile = new TProfile(f2_rhoVSl2ProfileName, f2_rhoVSl2ProfileTitle,bins,minLoc2,maxLoc2);

      // (5) the 2D Profile x0 vs. phi, (z || r) 
      TString f2_tVSl1l2ProfileName  = "f2_thicknessInX0";
              f2_tVSl1l2ProfileName += "_vs_loc1_vs_loc2";
              f2_tVSl1l2ProfileName += treeName;
      TString f2_tVSl1l2ProfileTitle            = "t/X_0 vs. ";
              f2_tVSl1l2ProfileTitle += "( loc1, loc2 ), ";
              f2_tVSl1l2ProfileTitle += treeName;
      TProfile2D* f2_tVSl1l2Profile = new TProfile2D(f2_tVSl1l2ProfileName, 
                                               f2_tVSl1l2ProfileTitle,
                                               bins,minLoc1,maxLoc1,
                                               bins,minLoc2,maxLoc2);


     // set branch address
     tree1->SetBranchAddress("Local1",&f1_Local1);
     tree1->SetBranchAddress("Local2",&f1_Local2);
     tree1->SetBranchAddress("CorrFactor",&f1_CorrFactor);
     tree1->SetBranchAddress("PathInX0",&f1_PathInX0);
     tree1->SetBranchAddress("Rho",&f1_Rho);

     // set branch address
     tree2->SetBranchAddress("Local1",&f2_Local1);
     tree2->SetBranchAddress("Local2",&f2_Local2);
     tree2->SetBranchAddress("CorrFactor",&f2_CorrFactor);
     tree2->SetBranchAddress("PathInX0",&f2_PathInX0);
     tree2->SetBranchAddress("Rho",&f2_Rho);
     
     // the 2 entries      
     
   Long64_t nentries1 = tree1->GetEntriesFast();

   Long64_t nentries2 = tree2->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries2;jentry++) {
      tree1->GetEntry(jentry);
      if (f1_CorrFactor<=0.) continue;
      f1_tVSl1Profile->Fill(f1_Local1,f1_PathInX0/f1_CorrFactor);
      f1_tVSl2Profile->Fill(f1_Local2,f1_PathInX0/f1_CorrFactor);
      f1_rhoVSl1Profile->Fill(f1_Local1,f1_Rho/f1_CorrFactor);
      f1_rhoVSl2Profile->Fill(f1_Local2,f1_Rho/f1_CorrFactor);
      f1_tVSl1l2Profile->Fill(f1_Local1,f1_Local2,f1_PathInX0/f1_CorrFactor);
   }

   for (Long64_t jentry=0; jentry<nentries2;jentry++) {
      tree2->GetEntry(jentry);
      if (f2_CorrFactor<=0.) continue;
      f2_tVSl1Profile->Fill(f2_Local1,f2_PathInX0/f2_CorrFactor);
      f2_tVSl2Profile->Fill(f2_Local2,f2_PathInX0/f2_CorrFactor);
      f2_rhoVSl1Profile->Fill(f2_Local1,f2_Rho/f2_CorrFactor);
      f2_rhoVSl2Profile->Fill(f2_Local2,f2_Rho/f2_CorrFactor);
      f2_tVSl1l2Profile->Fill(f2_Local1,f2_Local2,f2_PathInX0/f2_CorrFactor);
   }

// ------------ Draw section ----------------------
     moduleCanvas->cd(1);
       f1_tVSl1Profile->SetMarkerStyle(2);
       f1_tVSl1Profile->SetMarkerSize(1.);
       f1_tVSl1Profile->Draw();
       f2_tVSl1Profile->SetMarkerStyle(4);
       f2_tVSl1Profile->SetMarkerSize(0.8);
       f2_tVSl1Profile->Draw("same,p");

     moduleCanvas->cd(2);
       f1_tVSl2Profile->SetMarkerStyle(2);
       f1_tVSl2Profile->SetMarkerSize(1.0);
       f1_tVSl2Profile->Draw();
       f2_tVSl2Profile->SetMarkerStyle(4);
       f2_tVSl2Profile->SetMarkerSize(0.8);
       f2_tVSl2Profile->Draw("same,p");

    moduleCanvas->cd(3);
       f1_rhoVSl1Profile->SetMarkerStyle(2);
       f1_rhoVSl1Profile->SetMarkerSize(1.0);
       f1_rhoVSl1Profile->Draw();
       f2_rhoVSl1Profile->SetMarkerStyle(4);
       f2_rhoVSl1Profile->SetMarkerSize(0.6);
       f2_rhoVSl1Profile->Draw("same");

    moduleCanvas->cd(4);
       f1_rhoVSl2Profile->SetMarkerStyle(4);
       f1_rhoVSl2Profile->SetMarkerSize(0.8);
       f1_rhoVSl2Profile->Draw("hist");
       f2_rhoVSl2Profile->SetMarkerStyle(8);
       f2_rhoVSl2Profile->SetMarkerSize(0.8);
       f2_rhoVSl2Profile->Draw("same");

    moduleCanvas->cd(5);
       f1_tVSl1l2Profile->Draw("Cont4");

    moduleCanvas->cd(6);
       f2_tVSl1l2Profile->Draw("Cont4");


    


   } // tree1 && tree2

}
