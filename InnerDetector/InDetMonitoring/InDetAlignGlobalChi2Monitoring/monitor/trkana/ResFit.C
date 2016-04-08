/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// ResFit.C
// 
#include <ResFit.h>
//
void ResFit(TH1F* histo=NULL)
{
  int PrintLevel= 10;
  if (PrintLevel>=10) cout << " Bon dia Mon !" << endl;
  if (PrintLevel>=1) cout << " ** ResFit ** fitting " << histo->GetName() << endl;
 
  // Aquesta es la variable que tornem
  // 0 = vol dir que no s'ha fet o que no convergeix
  // 1 = gaussiana
  // 2 = G+G
  int AjustBo = 0; 

  double TestMean;
  double TestSigma;
  double TestRange;

  bool LetsFit = 1;

  int FitNdof;
  float FitChi2;
  float SigWidth;

  float OG_Mean;
  float OG_Sigma;
  float OG_Range;
  float OG_X2;
  int   OG_Ndof=1;
  float OG_X2overNdof;
  int   OG_BestPass=0; // ens diu quina passada dona millor

  float DG_X2;
  int   DG_Ndof=1;

  float OneGaussFitProb = -99.;
  float TwoGaussFitProb = -99.;
  
  float Pass1_X2overNdof;
  float Pass2_X2overNdof;
  float Pass3_X2overNdof;

  bool AjustFet=0;

  if (histo!=NULL) {
    // Primera pasada: traguem alguns parametres de l'histograma que valen per a definir la regio
    // inicial on ajustar. Aixi evitarem que falle perque hi haja doble pics o cues molt llargues.


    // com a valor inicial de la mitja prendrem aquell on esta el pic i no la mitja del histograma
    // aixi evitarem que les cues o distorsions.
    int chpeak = (int) histo->GetMaximumBin(); // canal on hi mes entrades
    TestMean = histo->GetBinCenter(chpeak); // probablement prop del centre de la gaussiana

    // per a la sigma si que prenem la rms
    TestSigma = histo->GetRMS();

    if (PrintLevel>=2) {
      cout << " *** Ajustant " << histo->GetTitle()
	   << " amb " << histo->GetEntries() 
	   << " entrades. Maxim en el canal " << chpeak << endl  
	   << "     Valors inicials: " << TestMean << " +- " << TestSigma << endl;
    }

    // Vegem quantes entrades hi ha en el canal del pic
    int npeak = (int) histo->GetBinContent(chpeak); 

    // demanem que no totes les entrades estiguen en un canal
    if (npeak == histo->GetEntries()) LetsFit = 0; 

    // que hi haja un minim d'entrades
    if (histo->GetEntries()<25) LetsFit = 0;

    // i que el canal del maxim no estiga en un extrem
    if (chpeak == histo->GetNbinsX()) LetsFit = 0;
    if (chpeak == 0) LetsFit = 0;

    if (LetsFit) { 
      AjustFet = 1;

      // Anem a per la primera pasada en un rang restringit
      if (PrintLevel>=3) TCanvas *CanPass1 = new TCanvas("CanPass1","CanPass1"); 
      SigWidth = 1.00;
      TestRange = SigWidth*TestSigma;
      if (PrintLevel>=4) cout << "     --> go for 1st pass. Range: " << TestRange << endl;
      histo->Fit("gaus","Q0","",(TestMean-TestRange),(TestMean+TestRange));
      histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
      if (PrintLevel>=3) histo->DrawCopy();

      FitChi2 = histo->GetFunction("gaus")->GetChisquare();
      FitNdof = histo->GetFunction("gaus")->GetNDF(); if (FitNdof==0) FitNdof = 1;
      Pass1_X2overNdof = FitChi2/FitNdof;

      TestMean = histo->GetFunction("gaus")->GetParameter("Mean");
      TestSigma = histo->GetFunction("gaus")->GetParameter("Sigma");    

      if (PrintLevel>=2) {
	cout << "     *** 1st Pass ** '" << histo->GetTitle()
	     << "' amb " << histo->GetEntries() 
	     << " entrades. Maxim en el canal " << chpeak  << endl
	     << "     valor ajust: " << TestMean 
	     << " +- " << TestSigma << endl;
	cout << "     X2= " << FitChi2
	     << " ndof= " << FitNdof
	     << " X2/Ndof= " << Pass1_X2overNdof << endl;
      }
      
      // i de moment prenem aquest resultat com a bo
      OG_Mean  = TestMean;      
      OG_Sigma = TestSigma;
      OG_Range = TestRange;
      OG_X2    = FitChi2;
      OG_Ndof  = FitNdof;
      OG_X2overNdof = Pass1_X2overNdof;
      OG_BestPass = 1;

      // Ara anem a fer una segona pasada en un rang mes ample pero partint
      // del resultat de la primera
      
      if (PrintLevel>=3) TCanvas *CanPass2 = new TCanvas("CanPass2","CanPass2"); 
      SigWidth = 2.0;
      TestRange = SigWidth*TestSigma;
      if (PrintLevel>=3) cout << "     --> go for 2nd pass. Range: " << TestRange << endl;
      histo->Fit("gaus","Q0","",(TestMean-TestRange),(TestMean+TestRange));
      histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
      if (PrintLevel>=3) histo->DrawCopy();

      // extraguem els parametres de l'ajust
      TestMean = histo->GetFunction("gaus")->GetParameter("Mean");
      TestSigma = histo->GetFunction("gaus")->GetParameter("Sigma");    
      FitChi2 = histo->GetFunction("gaus")->GetChisquare();
      FitNdof = histo->GetFunction("gaus")->GetNDF(); if (FitNdof==0) FitNdof = 1;
      Pass2_X2overNdof = FitChi2/FitNdof;
      
      if (PrintLevel>=1) {
	cout << "     *** 2nd Pass ** '" << histo->GetTitle()
	     << "' amb " << histo->GetEntries() 
	     << " entrades. Maxim en el canal " << chpeak << endl 
	     << "     valor ajust: " << TestMean 
	     << " +- " << TestSigma << endl
	     << "     X2= " << FitChi2
	     << " ndof= " << FitNdof
	     << " X2/Ndof= " << Pass2_X2overNdof << endl;
      }  

      // I si la segona pasada dona millor aleshores ens quedem amb ella
      if (Pass2_X2overNdof < Pass1_X2overNdof) {
	OG_Mean  = TestMean;      
	OG_Sigma = TestSigma;
	OG_Range = TestRange;
	OG_X2    = FitChi2;
	OG_Ndof  = FitNdof;
	OG_X2overNdof = Pass2_X2overNdof;
	OG_BestPass = 2;
      }

      // Ara anem a fer una tercera pasada en un rang mes ample pero partint
      // del resultat de la segona
      
      if (PrintLevel>=3) TCanvas *CanPass3 = new TCanvas("CanPass3","CanPass3"); 
      SigWidth = 5.0;
      TestRange = SigWidth*TestSigma;
      if (PrintLevel>=3) cout << "     --> go for 3rd pass. Range: " << TestRange << endl;
      histo->Fit("gaus","Q0","",(TestMean-TestRange),(TestMean+TestRange));
      histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
      if (PrintLevel>=3) histo->DrawCopy();

      // extraguem els parametres de l'ajust
      TestMean = histo->GetFunction("gaus")->GetParameter("Mean");
      TestSigma = histo->GetFunction("gaus")->GetParameter("Sigma");    
      FitChi2 = histo->GetFunction("gaus")->GetChisquare();
      FitNdof = histo->GetFunction("gaus")->GetNDF(); if (FitNdof==0) FitNdof = 1;
      Pass3_X2overNdof = FitChi2/FitNdof;
      
      if (PrintLevel>=1) {
	cout << "     *** 3rd Pass ** '" << histo->GetTitle()
	     << "' amb " << histo->GetEntries() 
	     << " entrades. Maxim en el canal " << chpeak << endl 
	     << "     valor ajust " << TestMean 
	     << " +- " << TestSigma << endl
	     << "     X2= " << FitChi2
	     << " ndof= " << FitNdof
	     << " X2/Ndof= " << Pass3_X2overNdof << endl;
      }  
      
      // I si la tercera pasada dona millor aleshores ens quedem amb ella
      if (Pass3_X2overNdof < OG_X2overNdof) {
	OG_Mean  = TestMean;      
	OG_Sigma = TestSigma;
	OG_Range = TestRange;
	OG_X2    = FitChi2;
	OG_Ndof  = FitNdof;
	OG_X2overNdof = Pass3_X2overNdof;
	OG_BestPass = 3;
      }
      
      // calcul de la probabilitat de l'ajust a una gaussiana
      OneGaussFitProb = TMath::Prob(OG_X2,OG_Ndof);
      

      // Ara anem a decidir si ens pareix be lo que teinm o si hi ha
      // que fer una doble gaussiana
      bool FitDoubleGauss = 0;
      if ( OneGaussFitProb < 0.50 ) FitDoubleGauss = 1; // si la probabilitat no es molt alta
      if ( OG_X2overNdof > 2.00 ) FitDoubleGauss = 1; // si el chi2 no es ideal
      if ( OG_BestPass == 1) FitDoubleGauss = 1; // si les pasades amb rang mes gran no van be.
      if ( OG_Sigma < 0.66*histo->GetRMS()) FitDoubleGauss = 1; // si la sigma del ajust es prou menor que la rms

      if ( FitDoubleGauss ) { // No est mal del tol'ajust pero probem amb la doble gaussiana
	if (PrintLevel>=4) cout << "    --> Lets try a G+G fit. Cross fingers ! " << endl;
	// per a traure els parametres de la primera reajustem amb un rang menor
	if (PrintLevel>=3) TCanvas *CanPass3N = new TCanvas("CanPass3Narrow","CanPass3Narrow"); 
	SigWidth = 0.25;
	histo->Fit("gaus","Q0","",(TestMean - SigWidth*TestSigma),(TestMean + SigWidth*TestSigma));
	histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
	if (PrintLevel>=3) histo->DrawCopy();
	
	// resultats de l'ajust en rang curt
	float mean_1 = histo->GetFunction("gaus")->GetParameter("Mean");
	float sigma_1 = histo->GetFunction("gaus")->GetParameter("Sigma");    
	float norm_1 = histo->GetFunction("gaus")->GetParameter(0); 

	// check if fit failed
	if (TMath::IsNaN(norm_1)) {mean_1 = histo->GetMean(); sigma_1 = histo->GetRMS(); norm_1 = 1;}

	if (PrintLevel>=3) TCanvas *CanPass3W = new TCanvas("CanPass3Wide","CanPass3Wide"); 
	SigWidth = 3.0;  // first guess
	if (sigma_1 < 0.5*histo->GetRMS()) SigWidth = 5.0;  // si la sigma es masa estreta, li donem un poc mes;
	histo->Fit("gaus","Q0","",(TestMean - SigWidth*TestSigma),(TestMean + SigWidth*TestSigma));
	histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
	if (PrintLevel>=3) histo->DrawCopy();
	
	// resultats de l'ajust en rang llarg
	float mean_2 = histo->GetFunction("gaus")->GetParameter("Mean");
	float sigma_2 = histo->GetFunction("gaus")->GetParameter("Sigma");    
	float norm_2 = histo->GetFunction("gaus")->GetParameter(0); 
	
	if (PrintLevel>=2) {
	  cout << "     *** 4rt Pass ** '" << histo->GetTitle()
	       << "' amb " << histo->GetEntries() 
	       << " entrades. Maxim en el canal " << chpeak << endl 
	       << "     Ajust estret " << mean_1 
	       << " +- " << sigma_1 
	       << "   Norm " << norm_1 << endl 
	       << "     Ajust ample: " << mean_2 
	       << " +- " << sigma_2 
	       << "   Norm " << norm_2 
	       << endl;
	}
	
	// Ara definim ja la doble gaussiana i utilitcem com a valors inicials
	// els dels ajusts estrets i ample
	TF1* DoubleGauss = new TF1("GaussPlusGauss","gaus(0)+gaus(3)");	
	DoubleGauss->SetParameter(0,norm_1);
	DoubleGauss->SetParameter(1,mean_1);
	DoubleGauss->SetParameter(2,sigma_1);
	DoubleGauss->SetParameter(3,norm_2/2.); // little push
	DoubleGauss->SetParameter(4,mean_2);
	DoubleGauss->SetParameter(5,sigma_2);
	
	if (PrintLevel>=3) TCanvas *CanPass4 = new TCanvas("CanPass4","CanPass4"); 
	histo->Fit("GaussPlusGauss","Q0");	
	histo->GetFunction("GaussPlusGauss")->SetLineColor(histo->GetLineColor());
	if (PrintLevel>=3) histo->DrawCopy();

	FitChi2 = histo->GetFunction("GaussPlusGauss")->GetChisquare();
	FitNdof = histo->GetFunction("GaussPlusGauss")->GetNDF(); if (FitNdof==0) FitNdof = 1;
	DG_X2 = FitChi2;
	DG_Ndof = FitNdof;
	
	norm_1 = histo->GetFunction("GaussPlusGauss")->GetParameter(0); 
	mean_1 = histo->GetFunction("GaussPlusGauss")->GetParameter(1);
	sigma_1 = histo->GetFunction("GaussPlusGauss")->GetParameter(2);    
	norm_2 = histo->GetFunction("GaussPlusGauss")->GetParameter(3); 
	mean_2 = histo->GetFunction("GaussPlusGauss")->GetParameter(4);
	sigma_2 = histo->GetFunction("GaussPlusGauss")->GetParameter(5);    

	// calcul de la probabilitat de l'ajust a una gaussiana
	TwoGaussFitProb = TMath::Prob(FitChi2,FitNdof);
	if (PrintLevel>=2) {
	  // resultats de l'ajust en rang llarg
	  cout << "     *** 5e Pass ** '" << histo->GetTitle()
	       << "' amb " << histo->GetEntries() 
	       << " entrades. Maxim en el canal " << chpeak << endl 
	       << "     Ajust estret " << mean_1 
	       << " +- " << sigma_1 
	       << "   Norm " << norm_1 << endl 
	       << "     Ajust ample: " << mean_2 
	       << " +- " << sigma_2 
	       << "   Norm " << norm_2 
	       << endl;
	}
      }
      else {
	DG_X2 = 999999999999.;
	DG_Ndof = 1;
      }
    }
  } 
  else {
    if (PrintLevel>=1) cout << " ** warning ** Histogram '"<< histo->GetTitle()
			    << "' can not be fitted. Desole. " << endl;
  }

  // vegem quin ajust es millor;
  //AjustBo =  (TwoGaussFitProb > OneGaussFitProb) ? 2: 1;
  AjustBo =  ((DG_X2 / DG_Ndof) < (OG_X2 / OG_Ndof)) ? 2: 1;
  // if (OG_BestPass==1) AjustBo = 2;  // si la gausina simple no pasa del primer prenem la doble gauss

  if (PrintLevel>=1) {
    cout << "  --- ResFit --- fit probabilities "
	 << "    Gauss = " << OneGaussFitProb*100
	 << "    G + G = " << TwoGaussFitProb*100
	 << "    Prenem " << AjustBo << endl
         << "                 X2 / Ndof         "
	 << "    Gauss = " << OG_X2 / OG_Ndof
	 << "    G + G = " << DG_X2 / DG_Ndof
	 << "    Prenem " << AjustBo 
	 << endl;
  }
  bool DeleteFunc=0;
  // Ara si cap dels ajusts es bo, borrem la funcio
  if (AjustFet == 1 && AjustBo==1 && (OG_X2 / OG_Ndof>200)) {
    //    DeleteFunc = 1;
  }
  if (AjustFet == 1 && AjustBo==2 && (DG_X2 / DG_Ndof>200)) {
    // DeleteFunc = 1;
  }
  if (DeleteFunc) {
    if (PrintLevel>=1) cout << "  --> ResFit --> very poor fit. DELETE fit for histo" << histo->GetTitle() << endl; 
    TList *FunList;
    FunList = histo->GetListOfFunctions();
    FunList->First()->Delete();
  }

  // I si finalment ens quedem amb una gaussiana cal reajustar-la
  if (AjustBo==1 && AjustFet) {
    histo->Fit("gaus","Q0","",(OG_Mean-OG_Range),(OG_Mean+OG_Range));
    histo->GetFunction("gaus")->SetLineColor(histo->GetLineColor());
  }

  
  //
  return;
}

