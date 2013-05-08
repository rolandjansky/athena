/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1); gStyle->SetPadTickX(1); gStyle->SetPadTickY(1);
  gStyle->SetLineWidth(1);
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(11111111);
  gStyle->SetHistLineWidth(1);


  // Open the ROOT File containing the tree
  //---------------------------------------

  TFile *_file0 = TFile::Open("TauCalibrationTools.root");
  

  // Set the ET and eta binning
  //---------------------------

  std::vector<float> ETBins;
  std::vector<float> etaBins;

  ETBins.push_back(0);
  ETBins.push_back(20);
  ETBins.push_back(30);
  ETBins.push_back(45);
  ETBins.push_back(60);
  ETBins.push_back(100);

  etaBins.push_back(0);
  etaBins.push_back(0.5);
  etaBins.push_back(1.5);
  etaBins.push_back(2.5);


  // Creating vectors to store the fitting parameters
  //-------------------------------------------------

  std::vector<float> MeanValues1p;
  std::vector<float> MeanValues23p;



  //****************************
  // The Master Loop over bins *
  //****************************

  for(unsigned int i = 0; i < (etaBins.size()-1); i++)
    {
      for(unsigned int j = 0; j < ETBins.size(); j++)
	{

	  // Making the names of the histograms
	  //-----------------------------------

	  TString ss1p;

	  ss1p += "ET";
	  ss1p += j;
	  ss1p += "eta";
	  ss1p += i;
	  ss1p += "_1p_plot";
	  
	  TString ss23p;
	  
	  ss23p += "ET";
	  ss23p += j;
	  ss23p += "eta";
	  ss23p += i;
	  ss23p += "_23p_plot";


	  // Making the titles of the histograms
	  //------------------------------------

	  TString ss1ptitle;

	  if(j != (ETBins.size()-1))
	    {
	      ss1ptitle += ETBins[j];
	      ss1ptitle += " < ET <";
	      ss1ptitle += ETBins[j+1];
	      ss1ptitle += ",";
	      ss1ptitle += etaBins[i];
	      ss1ptitle += " < eta <";
	      ss1ptitle += etaBins[i+1];
	      ss1ptitle += ", 1 Track";
	    }
	  else
	    {
	      ss1ptitle += ">";
	      ss1ptitle += ETBins[j];
	      ss1ptitle += ",";
	      ss1ptitle += etaBins[i];
	      ss1ptitle += " < eta <";
	      ss1ptitle += etaBins[i+1];
	      ss1ptitle += ", 1 Track";
	    }
	  
	  TString ss23ptitle;
	  
       	  if(j != (ETBins.size()-1))
	    {
	      ss23ptitle += ETBins[j];
	      ss23ptitle += " < ET <";
	      ss23ptitle += ETBins[j+1];
	      ss23ptitle += ",";
	      ss23ptitle += etaBins[i];
	      ss23ptitle += " < eta <";
	      ss23ptitle += etaBins[i+1];
	      ss23ptitle += ", 2-3 Tracks";
	    }
	  else
	    {
	      ss23ptitle += "<";
	      ss23ptitle += ETBins[j];
	      ss23ptitle += ",";
	      ss23ptitle += etaBins[i];
	      ss23ptitle += " < eta <";
	      ss23ptitle += etaBins[i+1];
	      ss23ptitle += ", 2-3 Tracks";
	    }


	  
	  // Creating the histograms
	  //------------------------

	  TH1F* histo1p = new TH1F(ss1p, ss1ptitle, 100, 0, 2);
	  TH1F* histo23p = new TH1F(ss23p, ss23ptitle, 100, 0, 2);



	  // Defining the conditions to project the histograms from the tree
	  //----------------------------------------------------------------

	  TString conditions1p;

	  if(j != (ETBins.size()-1))
	    {
	      conditions1p += "(ET>=";
	      conditions1p += ETBins[j];
	      conditions1p += ")&&(ET<";
	      conditions1p += ETBins[j+1];
	      conditions1p += ")&&(eta>=";
	      conditions1p += etaBins[i];
	      conditions1p += ")&&(eta<";
	      conditions1p += etaBins[i+1];
	      conditions1p += ")&&(ntracks==1)";
	    }
	  else
	    {
	      conditions1p += "(ET>=";
	      conditions1p += ETBins[j];
	      conditions1p += ")&&(eta>=";
	      conditions1p += etaBins[i];
	      conditions1p += ")&&(eta<";
	      conditions1p += etaBins[i+1];
	      conditions1p += ")&&(ntracks==1)";
	    }

	  TString conditions23p;

	  if(j != (ETBins.size()-1))
	    {
	      conditions23p += "(ET>=";
	      conditions23p += ETBins[j];
	      conditions23p += ")&&(ET<";
	      conditions23p += ETBins[j+1];
	      conditions23p += ")&&(eta>=";
	      conditions23p += etaBins[i];
	      conditions23p += ")&&(eta<";
	      conditions23p += etaBins[i+1];
	      conditions23p += ")&&(ntracks==2||ntracks==3)";
	    }
	  else
	    {
	      conditions23p += "(ET>=";
	      conditions23p += ETBins[j];
	      conditions23p += ")&&(eta>=";
	      conditions23p += etaBins[i];
	      conditions23p += ")&&(eta<";
	      conditions23p += etaBins[i+1];
	      conditions23p += ")&&(ntracks==2||ntracks==3)";
	    }



	  // Projecting the histograms
	  //--------------------------

	  ETRatios->Project(ss1p, "ETratio", conditions1p);
	  ETRatios->Project(ss23p, "ETratio", conditions23p);



	  // Fitting the histograms
	  //-----------------------

	  float mean1p = histo1p->GetMean();
	  float rms1p  = histo1p->GetRMS();

	  float mean23p = histo23p->GetMean();
	  float rms23p  = histo23p->GetRMS();

	  TF1* fit1p  = new TF1("fit1p", "gaus", mean1p - 2*rms1p, mean1p + 2*rms1p);
	  TF1* fit23p = new TF1("fit23p", "gaus", mean23p - 2*rms23p, mean23p + 2*rms23p);

	  fit1p->SetLineColor(2);
	  fit23p->SetLineColor(2);

	  histo1p->Fit(fit1p, "R");
	  histo23p->Fit(fit23p, "R");


	  // Recording the fitting parameters

	  MeanValues1p.push_back(fit1p->GetParameter(1));
	  MeanValues23p.push_back(fit23p->GetParameter(1));



	  // Drawing the histograms
	  //-----------------------

	  TCanvas* canvas = new TCanvas("canvas", "Calibration Factors", 200, 10, 700, 500);
	  
	  histo1p->Draw();
	  canvas->Update();
	  canvas->Print(ss1p += ".png");

	  histo23p->Draw();
	  canvas->Update();
	  canvas->Print(ss23p += ".png");
	}
    }
  



  //*****************************************
  // Printing the parameters to a text file *
  //*****************************************
  
  std::ofstream outputfile;
  outputfile.open("CalibrationFactors.txt");
  
  outputfile << "\t\t\ttauCalibrate = tauCalibrateWeightTool(" << std::endl;
  outputfile << "\t\t\t\tCellWeightTool=\"H1WeightToolCSC12\"," << std::endl;
  outputfile << "\t\t\t\tApplyCellWeightEM=True," << std::endl;
  outputfile << "\t\t\t\tApplyCellWeightHad=True," << std::endl;
  outputfile << "\t\t\t\tApplyPtEtaCorrFactors=True," << std::endl;
  outputfile << "\t\t\t\tpTNumberOfBins = " << ETBins.size() << "," << std::endl;
  outputfile << "\t\t\t\tetaNumberOfBins = " << (etaBins.size()-1) << "," << std::endl;
  
  
  // Calculating and outputing the pTpoints
  //---------------------------------------
  
  std::stringstream ptpoints;
  
  ptpoints << "\t\t\t\tpTPoints = [ ";
  
  for(unsigned int i = 0; i < ETBins.size(); i++)
    {
      if(i == (ETBins.size()-1))
	ptpoints << (1000*(ETBins.back() + 50));
      else
	ptpoints << (1000*(ETBins[i+1]+ETBins[i])/2) << ", ";
    }
  
  ptpoints << " ],";
  
  outputfile << ptpoints.str() << std::endl;
  
  
  // Calculating and outputing the etapoints
  //----------------------------------------
  
  std::stringstream etapoints;
  
  etapoints << "\t\t\t\tetaPoints = [ ";
  
  for(unsigned int j = 0; j < (etaBins.size()-1); j++)
    {
      if(j == (etaBins.size()-2))
	etapoints << (etaBins[j+1]+etaBins[j])/2;
      else
	etapoints << (etaBins[j+1]+etaBins[j])/2 << ", ";
    }
  
  etapoints << " ],";
  
  outputfile << etapoints.str() << std::endl;


  
  // The Calibration Factors
  //------------------------

  outputfile << "\t\t\t\tpTetaCorrectionsNtr1= [" << std::endl;
  outputfile << "\t\t\t\t## made with histo pT(tauMC)/pT(tauRec) vs pT vs eta, tauCutSafeLoose is true, ntr==1" << std::endl;
  outputfile << "\t\t\t\t## eta=0.0..0.5  0.5..1.5  1.5..2.5" << std::endl;

  for(unsigned int i = 0; i < ETBins.size(); i++)
    {
      std::stringstream ss;

      ss << "\t\t\t\t\t";

      for(unsigned int j = 0; j < (etaBins.size()-1); j++)
	{

	  if((j == (etaBins.size()-2))&&(i == (ETBins.size()-1)))
	    ss << MeanValues1p[j*ETBins.size() + i];
	  else
	    ss << MeanValues1p[j*ETBins.size() + i] << ",  ";
	}

      if(i == (ETBins.size()-1))
	ss << "   ## pt >" << ETBins.back();
      else
	ss << " ## pt=" << ETBins[i] << " .. " << ETBins[i+1];
      
      outputfile << ss.str() << std::endl;
    }

  outputfile << "\t\t\t\t]," << std::endl;


  outputfile << "\t\t\t\tpTetaCorrectionsNtr23= [" << std::endl;
  outputfile << "\t\t\t\t## made with histo pT(tauMC)/pT(tauRec) vs pT vs eta, tauCutSafeLoose is true, ntr==2||3" << std::endl;
  outputfile << "\t\t\t\t## eta=0.0..0.5  0.5..1.5  1.5..2.5" << std::endl;

  for(unsigned int i = 0; i < ETBins.size(); i++)
    {
      std::stringstream ss;

      ss << "\t\t\t\t\t";

      for(unsigned int j = 0; j < (etaBins.size()-1); j++)
	{

	  if((j == (etaBins.size()-2))&&(i == (ETBins.size()-1)))
	    ss << MeanValues23p[j*ETBins.size() + i];
	  else
	    ss << MeanValues23p[j*ETBins.size() + i] << ",  ";
	}

      if(i == (ETBins.size()-1))
	ss << "   ## pt >" << ETBins.back();
      else
	ss << " ## pt=" << ETBins[i] << " .. " << ETBins[i+1];
      
      outputfile << ss.str() << std::endl;
    }

  outputfile << "\t\t\t\t]," << std::endl;

  outputfile << "\t\t\tFudgeFactor = 1.00," << std::endl;
  outputfile << "\t\t\tCellCone = 0.4" << std::endl;
  outputfile << "\t\t\t)" << std::endl;

  outputfile.close();

}
