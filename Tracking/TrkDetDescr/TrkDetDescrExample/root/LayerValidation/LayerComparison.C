/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void LayerComparison(TString& file1name, TString& file2name, TString& layername, int bins)
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
   Float_t         f1_DensedHitRZ;
   Float_t         f1_DensedHitPhi;
   Float_t         f1_CorrFactor;
   Float_t         f1_PathInX0;
   Float_t         f1_Rho;


   

 // File 2 declarations ====================================
   Float_t         f2_DensedHitRZ;
   Float_t         f2_DensedHitPhi;
   Float_t         f2_CorrFactor;
   Float_t         f2_PathInX0;
   Float_t         f2_Rho;

   TFile* file2 = TFile::Open(file2name);

   TString  treeName           = layername;
   TString  moduleCanvasName   = "ModuleCanvas_";
            moduleCanvasName  += treeName;
   TString  moduleCanvasTitle  =  "Layer ";
            moduleCanvasTitle += treeName;

   bool isCylinder = true;

   TCanvas* moduleCanvas     = new TCanvas(moduleCanvasName,moduleCanvasTitle,100,100,600,920);
   moduleCanvas->Divide(2,3);

   if (file1 && file2) {
     std::cout << " [m] both files loaded - trying to get TTree(s) '" << treeName << "'." << std::endl;
   } else {
     throw std::runtime_error("File could not be loaded in LayerComparison constructor");
   }

   TTree* tree1 = file1->Get(treeName);
   TTree* tree2 = file2->Get(treeName);


   if (tree1 && tree2){

     std::cout << " [m] both trees loaded ! " << std::endl;

     moduleCanvas->cd(1);
        // get the min/max values for histogram booking: rz
        if (!isCylinder)
          tree1->Draw("DensedHitR>>hrz", "CorrFactor>0.");
        else
          tree1->Draw("DensedHitZ>>hrz", "CorrFactor>0.");
        TH1F* hrz = (TH1F*)gDirectory->Get("hrz");
        double minRZ = hrz->GetXaxis()->GetXmin();
        double maxRZ = hrz->GetXaxis()->GetXmax();
        delete hrz; hrz = 0;

        // get the min/max vlues for histogram bookin: phi
        double minPhi = -3.18;
        double maxPhi = +3.18;

      // (1) the Profile x0 vs rz
      TString f1_tVSrzProfileName  = "f1_thicknessInX0";
              f1_tVSrzProfileName += "_vs_rz_";
              f1_tVSrzProfileName += treeName;
      TString f1_tVSrzProfileTitle            = "t/X_0 vs. ";
              f1_tVSrzProfileTitle += "rz ";
      TProfile* f1_tVSrzProfile = new TProfile(f1_tVSrzProfileName, f1_tVSrzProfileTitle,bins,minRZ,maxRZ);

      // (2) the Profile x0 vs phi
      TString f1_tVSphiProfileName  = "f1_thicknessInX0";
              f1_tVSphiProfileName += "_vs_phi_";
              f1_tVSphiProfileName += treeName;
      TString f1_tVSphiProfileTitle            = "t/X_0 vs. ";
              f1_tVSphiProfileTitle += "phi ";
      TProfile* f1_tVSphiProfile = new TProfile(f1_tVSphiProfileName, f1_tVSphiProfileTitle,bins,minPhi,maxPhi);

      // (3) the Profile rho vs rz
      TString f1_rhoVSrzProfileName  = "f1_rho";
              f1_rhoVSrzProfileName += "_vs_rz_";
              f1_rhoVSrzProfileName += treeName;
      TString f1_rhoVSrzProfileTitle            = "#rho vs. ";
              f1_rhoVSrzProfileTitle += "rz ";
      TProfile* f1_rhoVSrzProfile = new TProfile(f1_rhoVSrzProfileName, f1_rhoVSrzProfileTitle,bins,minRZ,maxRZ);

      // (4) the Profile rho vs phi
      TString f1_rhoVSphiProfileName  = "f1_rho";
              f1_rhoVSphiProfileName += "_vs_phi_";
              f1_rhoVSphiProfileName += treeName;
      TString f1_rhoVSphiProfileTitle            = "#rho vs. ";
              f1_rhoVSphiProfileTitle += "phi ";
      TProfile* f1_rhoVSphiProfile = new TProfile(f1_rhoVSphiProfileName, f1_rhoVSphiProfileTitle,bins,minPhi,maxPhi);

      // (5) the 2D Profile x0 vs. phi, (z || r) 
      TString f1_tVSrzphiProfileName  = "f1_thicknessInX0";
              f1_tVSrzphiProfileName += "_vs_rz_vs_phi";
              f1_tVSrzphiProfileName += treeName;
      TString f1_tVSrzphiProfileTitle            = "t/X_0 vs. ";
              f1_tVSrzphiProfileTitle += "( rz, phi ), ";
              f1_tVSrzphiProfileTitle += treeName;
      TProfile2D* f1_tVSrzphiProfile = new TProfile2D(f1_tVSrzphiProfileName, 
                                               f1_tVSrzphiProfileTitle,
                                               bins,minRZ,maxRZ,
                                               bins,minPhi,maxPhi);

      // (1) the Profile x0 vs rz
      TString f2_tVSrzProfileName  = "f2_thicknessInX0";
              f2_tVSrzProfileName += "_vs_rz_";
              f2_tVSrzProfileName += treeName;
      TString f2_tVSrzProfileTitle            = "t/X_0 vs. ";
              f2_tVSrzProfileTitle += "rz ";
      TProfile* f2_tVSrzProfile = new TProfile(f2_tVSrzProfileName, f2_tVSrzProfileTitle,bins,minRZ,maxRZ);

      // (2) the Profile x0 vs phi
      TString f2_tVSphiProfileName  = "f2_thicknessInX0";
              f2_tVSphiProfileName += "_vs_phi_";
              f2_tVSphiProfileName += treeName;
      TString f2_tVSphiProfileTitle            = "t/X_0 vs. ";
              f2_tVSphiProfileTitle += "phi ";
      TProfile* f2_tVSphiProfile = new TProfile(f2_tVSphiProfileName, f2_tVSphiProfileTitle,bins,minPhi,maxPhi);

      // (3) the Profile rho vs rz
      TString f2_rhoVSrzProfileName  = "f2_rho";
              f2_rhoVSrzProfileName += "_vs_rz_";
              f2_rhoVSrzProfileName += treeName;
      TString f2_rhoVSrzProfileTitle = "#rho vs. ";
              f2_rhoVSrzProfileTitle += "rz ";
      TProfile* f2_rhoVSrzProfile = new TProfile(f2_rhoVSrzProfileName, f2_rhoVSrzProfileTitle,bins,minRZ,maxRZ);

      // (4) the Profile rho vs phi
      TString f2_rhoVSphiProfileName  = "f2_rho";
              f2_rhoVSphiProfileName += "_vs_phi_";
              f2_rhoVSphiProfileName += treeName;
      TString f2_rhoVSphiProfileTitle            = "#rho vs. ";
              f2_rhoVSphiProfileTitle += "phi ";
      TProfile* f2_rhoVSphiProfile = new TProfile(f2_rhoVSphiProfileName, f2_rhoVSphiProfileTitle,bins,minPhi,maxPhi);

      // (5) the 2D Profile x0 vs. phi, (z || r) 
      TString f2_tVSrzphiProfileName  = "f2_thicknessInX0";
              f2_tVSrzphiProfileName += "_vs_rz_vs_phi";
              f2_tVSrzphiProfileName += treeName;
      TString f2_tVSrzphiProfileTitle            = "t/X_0 vs. ";
              f2_tVSrzphiProfileTitle += "( rz, phi ), ";
              f2_tVSrzphiProfileTitle += treeName;
      TProfile2D* f2_tVSrzphiProfile = new TProfile2D(f2_tVSrzphiProfileName, 
                                               f2_tVSrzphiProfileTitle,
                                               bins,minRZ,maxRZ,
                                               bins,minPhi,maxPhi);


     // set branch address
     if (!isCylinder)
       tree1->SetBranchAddress("DensedHitR",&f1_DensedHitRZ);
     else
       tree1->SetBranchAddress("DensedHitZ",&f1_DensedHitRZ);
     tree1->SetBranchAddress("DensedHitPhi",&f1_DensedHitPhi);
     tree1->SetBranchAddress("CorrFactor",&f1_CorrFactor);
     tree1->SetBranchAddress("PathInX0",&f1_PathInX0);
     tree1->SetBranchAddress("Rho",&f1_Rho);

     // set branch address
     if (!isCylinder)
       tree2->SetBranchAddress("DensedHitR",&f2_DensedHitRZ);
     else
     tree2->SetBranchAddress("DensedHitZ",&f2_DensedHitRZ);
     tree2->SetBranchAddress("DensedHitPhi",&f2_DensedHitPhi);
     tree2->SetBranchAddress("CorrFactor",&f2_CorrFactor);
     tree2->SetBranchAddress("PathInX0",&f2_PathInX0);
     tree2->SetBranchAddress("Rho",&f2_Rho);
     
     // the 2 entries      
     
   Long64_t nentries1 = tree1->GetEntriesFast();

   Long64_t nentries2 = tree2->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries2;jentry++) {
      tree1->GetEntry(jentry);
      if (f1_CorrFactor<=0.) continue;
      f1_tVSrzProfile->Fill(f1_DensedHitRZ,f1_PathInX0/f1_CorrFactor);
      f1_tVSphiProfile->Fill(f1_DensedHitPhi,f1_PathInX0/f1_CorrFactor);
      f1_rhoVSrzProfile->Fill(f1_DensedHitRZ,f1_Rho/f1_CorrFactor);
      f1_rhoVSphiProfile->Fill(f1_DensedHitPhi,f1_Rho/f1_CorrFactor);
      f1_tVSrzphiProfile->Fill(f1_DensedHitRZ,f1_DensedHitPhi,f1_PathInX0/f1_CorrFactor);
   }

   for (Long64_t jentry=0; jentry<nentries2;jentry++) {
      tree2->GetEntry(jentry);
      if (f2_CorrFactor<=0.) continue;
      f2_tVSrzProfile->Fill(f2_DensedHitRZ,f2_PathInX0/f2_CorrFactor);
      f2_tVSphiProfile->Fill(f2_DensedHitPhi,f2_PathInX0/f2_CorrFactor);
      f2_rhoVSrzProfile->Fill(f2_DensedHitRZ,f2_Rho/f2_CorrFactor);
      f2_rhoVSphiProfile->Fill(f2_DensedHitPhi,f2_Rho/f2_CorrFactor);
      f2_tVSrzphiProfile->Fill(f2_DensedHitRZ,f2_DensedHitPhi,f2_PathInX0/f2_CorrFactor);
   }

// ------------ Draw section ----------------------
     moduleCanvas->cd(1);
       f1_tVSrzProfile->SetMarkerStyle(2);
       f1_tVSrzProfile->SetMarkerSize(1.);
       f1_tVSrzProfile->Draw();
       f2_tVSrzProfile->SetMarkerStyle(4);
       f2_tVSrzProfile->SetMarkerSize(0.8);
       f2_tVSrzProfile->Draw("same,p");

     moduleCanvas->cd(2);
       f1_tVSphiProfile->SetMarkerStyle(2);
       f1_tVSphiProfile->SetMarkerSize(1.0);
       f1_tVSphiProfile->Draw();
       f2_tVSphiProfile->SetMarkerStyle(4);
       f2_tVSphiProfile->SetMarkerSize(0.8);
       f2_tVSphiProfile->Draw("same,p");

    moduleCanvas->cd(3);
       f1_rhoVSrzProfile->SetMarkerStyle(2);
       f1_rhoVSrzProfile->SetMarkerSize(1.0);
       f1_rhoVSrzProfile->Draw();
       f2_rhoVSrzProfile->SetMarkerStyle(4);
       f2_rhoVSrzProfile->SetMarkerSize(0.6);
       f2_rhoVSrzProfile->Draw("same");

    moduleCanvas->cd(4);
       f1_rhoVSphiProfile->SetMarkerStyle(4);
       f1_rhoVSphiProfile->SetMarkerSize(0.8);
       f1_rhoVSphiProfile->Draw("hist");
       f2_rhoVSphiProfile->SetMarkerStyle(8);
       f2_rhoVSphiProfile->SetMarkerSize(0.8);
       f2_rhoVSphiProfile->Draw("same");

    moduleCanvas->cd(5);
       f1_tVSrzphiProfile->Draw("Cont4");

    moduleCanvas->cd(6);
       f2_tVSrzphiProfile->Draw("Cont4");


    


   } // tree1 && tree2

}
