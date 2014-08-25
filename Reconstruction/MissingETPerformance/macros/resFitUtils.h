/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////Functions used by TF1s for fitting

Double_t gaussDist(Double_t *x, Double_t *par)
{
  return par[0]*TMath::Gaus(x[0], par[1], par[2], false) + 0*par[3];
}

Double_t studentsTDist(Double_t *x, Double_t *par)
{
  return (1/TMath::Student(0, par[3]))*par[0]*TMath::Student((x[0] - par[1])/par[2], par[3]);
}

Double_t lorentzian(Double_t *x, Double_t *par)
{
  return par[0]*(1/TMath::CauchyDist(0, par[1], par[2]))*TMath::CauchyDist(x[0], par[1], par[2]) + 0*par[3];
}

Double_t voigtProfile(Double_t *x, Double_t *par)
{
  return (1/TMath::Voigt(0, par[2], par[3]))*par[0]*TMath::Voigt(x[0] - par[1], par[2], par[3]);
}


Double_t studentsTDistPlusStudentsTDist(Double_t *x, Double_t *par){
  return (1/TMath::Student(0, par[3]))*par[0]*TMath::Student((x[0] - par[1])/par[2], par[3]) + (1/TMath::Student(0, par[6]))*par[4]*TMath::Student((x[0] - par[1])/par[5], par[6]);
}

Double_t studentsTDistPlusGaussDist(Double_t *x, Double_t *par){
  return (1/TMath::Student(0, par[3]))*par[0]*TMath::Student((x[0] - par[1])/par[2], par[3]) + par[4]*TMath::Gaus(x[0], par[1], par[5], false) + 0*par[6];
}

Double_t studentsTDistPlusLorentzian(Double_t *x, Double_t *par){
   return (1/TMath::Student(0, par[3]))*par[0]*TMath::Student((x[0] - par[1])/par[2], par[3]) + par[4]*(1/TMath::CauchyDist(0, par[1], par[5]))*TMath::CauchyDist(x[0], par[1], par[5]) + 0*par[6];
}

Double_t studentsTDistPlusVoigtProfile(Double_t *x, Double_t *par){
return (1/TMath::Student(0, par[3]))*par[0]*TMath::Student((x[0] - par[1])/par[2], par[3]) + (1/TMath::Voigt(0, par[5], par[6]))*par[4]*TMath::Voigt(x[0] - par[1], par[5], par[6]);
}

Double_t gaussDistPlusLorentzian(Double_t *x, Double_t *par){
return par[0]*TMath::Gaus(x[0], par[1], par[2], false) + par[4]*(1/TMath::CauchyDist(0, par[1], par[5]))*TMath::CauchyDist(x[0], par[1], par[5]) + 0*par[6]*par[3];
}

Double_t gaussDistPlusGaussDist(Double_t *x, Double_t *par){
  return par[0]*TMath::Gaus(x[0], par[1], par[2], false) +  par[4]*TMath::Gaus(x[0], par[1], par[5], false) + 0*par[6]*par[3];
}

Double_t gaussDistPlusVoigtProfile(Double_t *x, Double_t *par){
return par[0]*TMath::Gaus(x[0], par[1], par[2], false) + (1/TMath::Voigt(0, par[5], par[6]))*par[4]*TMath::Voigt(x[0] - par[1], par[5], par[6]) + 0*par[3];
}

Double_t lorentzianPlusLorentzian(Double_t *x, Double_t *par){
  return par[0]*(1/TMath::CauchyDist(0, par[1], par[2]))*TMath::CauchyDist(x[0], par[1], par[2]) + par[4]*(1/TMath::CauchyDist(0, par[1], par[5]))*TMath::CauchyDist(x[0], par[1], par[5]) + 0*par[6]*par[3];
}

Double_t lorentzianPlusVoigtProfile(Double_t *x, Double_t *par){
return par[0]*(1/TMath::CauchyDist(0, par[1], par[2]))*TMath::CauchyDist(x[0], par[1], par[2]) + (1/TMath::Voigt(0, par[5], par[6]))*par[4]*TMath::Voigt(x[0] - par[1], par[5], par[6]) + 0*par[3];
}

Double_t voigtProfilePlusVoigtProfile(Double_t *x, Double_t *par){
return (1/TMath::Voigt(0, par[2], par[3]))*par[0]*TMath::Voigt(x[0] - par[1], par[2], par[3]) + (1/TMath::Voigt(0, par[5], par[6]))*par[4]*TMath::Voigt(x[0] - par[1], par[6], par[6]);
}
///////////////////////////////////
//Functions to strip off the first half or second half of a double function.
//////////////////////////////////////////////////

TF1 *studentsClone(TF1 *fit, int order, double low, double high){
  if(order == 0) int getPars[4] = {0, 1, 2, 3};
  else if(order == 1) int getPars[4] = {4, 1, 5, 6};
 TF1 *fitClone = new TF1("studentsClone", studentsTDist, low, high, 4);
 for(Int_t i = 0; i < 4; ++i){
   fitClone->SetParameter(i, fit->GetParameter(getPars[i]));
   fitClone->SetParError(i, fit->GetParError(getPars[i]));
 }
 return fitclone;
} //returns just the student's t-dist part of a double fit.

TF1 *gausClone(TF1 *fit, int order){
  int getPars[4];
  for(int j = 0; j < 4; ++j) getPars[j] = j;
  if(order == 1){
    getPars[0] = 4;
    getPars[2] = 5;
    getPars[3] = 6;
  }
  int binCount = fit->GetHistogram()->GetNbinsX();
  TF1 *fitClone = new TF1("gausClone", gaussDist, fit->GetHistogram()->GetBinLowEdge(1), fit->GetHistogram()->GetBinLowEdge(binCount), 4);
  for(Int_t i = 0; i < 4; ++i){
    fitClone->SetParameter(i, fit->GetParameter(getPars[i]));
    fitClone->SetParError(i, fit->GetParError(getPars[i]));
  }
 return fitClone;
}

TF1 *voigtClone(TF1 *fit, int order){
  if(order == 0) int getPars[4] = {0, 1, 2, 3};
  else if(order == 1) int getPars[4] = {4, 1, 5, 6};
  TF1 *fitClone = new TF1("voigtClone", voigtProfile, low, high, 4);
  for(Int_t i = 0; i < 4; ++i){
    fitClone->SetParameter(i, fit->GetParameter(getPars[i]));
    fitClone->SetParError(i, fit->GetParError(getPars[i]));
  }
 return fitclone;
}

TF1 *lorentzianClone(TF1 *fit, int order){
  if(order == 0) int getPars[4] = {0, 1, 2, 3};
  else if(order == 1) int getPars[4] = {4, 1, 5, 6};
  TF1 *fitClone = new TF1("lorentzianClone", lorentzian, low, high, 4);
  for(Int_t i = 0; i < 4; ++i){
    fitClone->SetParameter(i, fit->GetParameter(getPars[i]));
    fitClone->SetParError(i, fit->GetParError(getPars[i]));
  }
 return fitclone;
}

////////////////////////////////////////////////
///Functions returning a modified resolution
//IE, lorentzian doesn't give a res, it gives a shape parameter, voigt gives res and shape.
//so these function compute a res, based on where the function falls to Amp*e^-1.
////////////////////////////////////////////////



Double_t equivalentRes(TF1 *fit, Int_t order)
{
  double amplitude = 0;
  double decayPoint = fit->GetParameter(0)*exp(-.5);
  if(order == 0 && fit->GetNpar() > 4){
    amplitude = fit->GetParameter(4);
    fit->SetParameter(4, 0);
  }//end of order = 0
  if(order == 1){
    decayPoint = fit->GetParameter(4)*exp(-.5);
    amplitude = fit->GetParameter(0);
    fit->SetParameter(0, 0);
  }//end of order = 1

  int binCount = fit->GetHistogram()->GetNbinsX();
  double res = .5*fabs(fit->GetX(decayPoint, fit->GetParameter(1), fit->GetHistogram()->GetBinLowEdge(binCount)) - fit->GetX(decayPoint, fit->GetHistogram()->GetBinLowEdge(1), fit->GetParameter(1)));

  if(order == 1) fit->SetParameter(0, amplitude);
  else if(order == 0 && fit->GetNpar() > 4) fit->SetParameter(4, amplitude);
  
  return res;
}


Double_t equivalentResError(TF1 *fit, double ex, int order)
{
  double par[7];
  double parError[7];
  
  for(Int_t j = 0; j < fit->GetNpar(); ++j){
      parError[j] = fit->GetParError(j);
      if(order == 0 && j > 3) parError[j] = 0;
      if(order == 1 && j < 4 && j != 1) parError[j] = 0;
  }//end of j-loop
  par[4] = 0;
  Double_t x[1];
  x[0] = ex;
  double deltaParSquared = 0;
  for(Int_t i = 2; i < fit->GetNpar(); ++i){
    //cout << fit->GradientPar(i, x) << ", ";
    if(parError[i] != 0 && fit->GradientPar(i, x) != 0) deltaParSquared += pow((parError[i]/fit->GradientPar(i, x)), 2);*errorCalcX(fit, i, order)), 2);
  }//end of i-loop
  cout << endl;
  cout << endl;
  return sqrt(deltaParSquared);
}//calcs error on res from equivalentRes function



Double_t errorCalcX(TF1 *fit, Int_t parameterN, int order)
{
  double calcPointsLeft[2];
  double calcPointsRight[2]; 
  double amplitude = 0;
  double decayPoint = 0;
  if(order == 0){
    decayPoint = fit->GetParameter(0)*exp(-.5);
    if(fit->GetNpar() > 4){
      amplitude = fit->GetParameter(4);
      fit->SetParameter(4,0);
    }
  }
  else if(fit->GetNpar() > 4 && order == 1){
    decayPoint = fit->GetParameter(4)*exp(-.5);
    amplitude = fit->GetParameter(0);
    fit->SetParameter(0,0);
  }
  else return -1.0;

  double advance = .01*fit->GetParError(parameterN);
  
  double original = fit->GetParameter(parameterN);
  double new_high = original + advance;
  double new_low = original - advance;

  if(new_low <= 1.0 && parameterN != 1) new_low = 1.0;
  double run = new_high - new_low;
  fit->SetParameter(parameterN, new_high);
  int binCount = fit->GetHistogram()->GetNbinsX();
  double center = fit->GetParameter(1);
  calcPointsRight[0] = fit->GetX(decayPoint, center, fit->GetHistogram()->GetBinLowEdge(binCount));
  calcPointsLeft[0] = fit->GetX(decayPoint, fit->GetHistogram()->GetBinLowEdge(1), center);

  fit->SetParameter(parameterN, new_low);
  center = fit->GetParameter(1);
  calcPointsRight[1] = fit->GetX(decayPoint, center, fit->GetHistogram()->GetBinLowEdge(binCount));
  calcPointsLeft[1] = fit->GetX(decayPoint, fit->GetHistogram()->GetBinLowEdge(1), center);
 
  double deltaX = sqrt(pow(((calcPointsRight[0] - calcPointsRight[1])/run),2) + pow(((calcPointsLeft[0] - calcPointsLeft[1])/run),2));
  fit->SetParameter(parameterN, original);

  if(order == 1) fit->SetParameter(0, amplitude);
  else if(order == 0 && fit->GetNpar() > 4) fit->SetParameter(4, amplitude);

  return deltaX;
}//calculates the error in x due to each parameter.

//////////////////////////////////////////////////////////////
////Functions to set up the limits on the fit functions correctly.
////////////////////////////////////////////////////////////

TF1 *generalOptimizer(TF1 *fitPrimus, TH1 *histogram, int order) //for Gaussian and Lorentzian
{
  if(order == 0)
    {
    fitPrimus->SetParLimits(0, .75*histogram->GetMaximum(), 1.25*histogram->GetMaximum());
    fitPrimus->SetParameter(0, histogram->GetMaximum());
    fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
    if(histogram->GetRMS() != 0) fitPrimus->SetParameter(2, histogram->GetRMS()); 
    fitPrimus->FixParameter(3, 0);
    }  
  else if(order == 1)
    {
      fitPrimus->SetParLimits(4, 0.0*histogram->GetMaximum(), .2*histogram->GetMaximum());
      fitPrimus->SetParameter(4, .1*histogram->GetMaximum());
      fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(5, histogram->GetRMS()); 
      fitPrimus->FixParameter(6, 0);
    }
  return fitPrimus;
}//end of generalOptimizer

TF1 *studentsOptimizer(TF1 *fitPrimus, TH1 *histogram, int order)
{
  if(order == 0)
    {
      fitPrimus->SetParLimits(0, .75*histogram->GetMaximum(), 1.25*histogram->GetMaximum());
      fitPrimus->SetParLimits(3, 1, 341); //The TMath::Student function gives nan or 0 outside these values.
      //fitPrimus->SetParLimits(2, .25*histogram->GetBinWidth(histogram->GetMaximumBin()), 100*histogram->GetRMS());
      fitPrimus->SetParameter(0, histogram->GetMaximum());
      fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(2, histogram->GetRMS()); 
      if(histogram->GetNbinsX() != 0) fitPrimus->SetParameter(3, histogram->GetNbinsX());
      
    }  
  else if(order == 1)
    {
      fitPrimus->SetParLimits(4, 0.0*histogram->GetMaximum(), .2*histogram->GetMaximum());
      fitPrimus->SetParLimits(6, 1, 341); //The TMath::Student function gives nan or 0 outside these values.
      // fitPrimus->SetParLimits(5, .25*histogram->GetBinWidth(histogram->GetMaximumBin()), 100*histogram->GetRMS());
      fitPrimus->SetParameter(4, .1*histogram->GetMaximum());
      fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(5, histogram->GetRMS()); 
      if(histogram->GetNbinsX() != 0) fitPrimus->SetParameter(6, histogram->GetNbinsX());  
    }
  return fitPrimus; 
}//end of studentsOptimizer

TF1 *voigtOptimizer(TF1 *fitPrimus, TH1 *histogram, int order)
{
  if(order == 0)
    {
      fitPrimus->SetParLimits(0, .75*histogram->GetMaximum(), 1.25*histogram->GetMaximum());
      fitPrimus->SetParameter(0, histogram->GetMaximum());
      fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(2, histogram->GetRMS()); 
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(3, histogram->GetRMS());
      fitPrimus->SetParLimits(2, 1e-5, 1e5);
      fitPrimus->SetParLimits(3, 1e-5, 1e5);
    }  
  else if(order == 1)
    {
      fitPrimus->SetParLimits(4, 0.0*histogram->GetMaximum(), .2*histogram->GetMaximum());
      fitPrimus->SetParameter(4, histogram->GetMaximum());
      fitPrimus->SetParameter(1, histogram->GetBinLowEdge(histogram->GetMaximumBin()));
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(5, histogram->GetRMS()); 
      if(histogram->GetRMS() != 0) fitPrimus->SetParameter(6, histogram->GetRMS());  
      fitPrimus->SetParLimits(5, 1e-5, 1e5);
      fitPrimus->SetParLimits(6, 1e-5, 1e5);
    }
  return fitPrimus; 
}//end of voigt Optimizer



















































































/* //////////////////Main fit function///////////// */
/* vector<double> resolutionFitter(TH1* histogram, string fitOption, float low, float high){ */

/*   vector<double> resolutions; */
/*   string fitName = "fittedThing"; */

/*   if(fitOption == "gaussian"){ */
/*     TF1 *fitted = gaussian(fitName, histogram, 0, low, high); */
/*     cout << fitted->GetParameter(2) << " versus " << equivalentRes(fitted) << endl; */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*     resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*     resolutions.push_back(0); */
/*     resolutions.push_back(0); */
/*   } */
/*   else if(fitOption == "students"){ */
/*     TF1 *fitted = students(fitName, histogram, 0, low, high); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(fitted)); */
/*      resolutions.push_back(equivalentResError(fitted)); */
/*      resolutions.push_back(0); */
/*      resolutions.push_back(0); */
/*   } */
/*   else if(fitOption == "lorentzian"){ */
/*     TF1 *fitted = lorentzian(fitName, histogram,0, low, high); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(fitted)); */
/*     resolutions.push_back(equivalentResError(fitted)); */
/*     resolutions.push_back(0); */
/*     resolutions.push_back(0); */
/*   } */
/*   else if(fitOption == "voigt"){ */
/*     TF1 *fitted = voigt(fitName, histogram,0, low, high); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(fitted)); */
/*     resolutions.push_back(equivalentResError(fitted)); */
/*     resolutions.push_back(0); */
/*     resolutions.push_back(0); */
/*   } */
/*   else if(fitOption == "gaussian+gaussian"){ */
/*     TF1 *fitted = gausPlusGaus(fitName, histogram,0, low, high); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*     resolutions.push_back(fitted->GetParameter(5)); */
/*     resolutions.push_back(fitted->GetParError(5)); */
/*   } */
/*   else if(fitOption == "gaussian+voigt"){ */
/*     TF1 *fitted = gausPlusVoigt(fitName, histogram,0, low, high); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*   } */
/*   else if(fitOption == "voigt+gaussian"){ */
/*     TF1 *fitted = gaussPlusVoigt(fitName, histogram,1, low, high); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*     resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*   } */
/*   else if(fitOption == "gaussian+lorentzian"){ */
/*     TF1 *fitted = gaussPlusLorentzian(fitName, histogram,0, low, high); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*     TF1 *lorentzianFit = lorentzianClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(lorentzianFit)); */
/*     resolutions.push_back(equivalentResError(lorentzianFit)); */
/*   } */
/*   else if(fitOption == "lorentzian+gaussian"){ */
/*     TF1 *fitted = gaussPlusLorentzian(fitName, histogram,1, low, high); */
/*     TF1 *lorentzianFit = lorentzianClone(fitted, 1); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(lorentzianFit)); */
/*     resolutions.push_back(equivalentResError(lorentzianFit)); */
/*     resolutions.push_back(fitted->GetParameter(2)); */
/*     resolutions.push_back(fitted->GetParError(2)); */
/*   } */
/*   else if(fitOption == "students+students"){ */
/*     TF1 *fitted = studentsPlusStudents(fitName, histogram,0, low, high); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     TF1 *studentsFitLow = studentsClone(fitted, 1); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*     resolutions.push_back(equivalentRes(studentsFitLow)); */
/*     resolutions.push_back(equivalentResError(studentsFitLow)); */
/*   } */
/*   else if(fitOption == "students+gaussian"){ */
/*     TF1 *fitted = studentsPlusGauss(fitName, histogram,0, low, high); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*     resolutions.push_back(fitted->GetParameter(5)); */
/*     resolutions.push_back(fitted->GetParError(5)); */
/*   } */
/*   else if(fitOption == "gaussian+students"){ */
/*     TF1 *fitted = studentsPlusGauss(fitName, histogram,1, low, high); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(fitted->GetParameter(5)); */
/*     resolutions.push_back(fitted->GetParError(5)); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*   } */
/*   else if(fitOption == "students+lorentzian"){ */
/*     TF1 *fitted = studentsPlusLorentzian(fitName, histogram,0, low, high); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*   resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*     TF1 *lorentzianFit = lorentzianClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(lorentzianFit)); */
/*     resolutions.push_back(equivalentResError(lorentzianFit)); */
/*   } */
/*   else if(fitOption == "lorentzian+students"){ */
/*     TF1 *fitted = studentsPlusLorentzian(fitName, histogram,1, low, high); */
/*     TF1 *lorentzianFit = lorentzianClone(fitted, 1); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(lorentzianFit)); */
/*     resolutions.push_back(equivalentResError(lorentzianFit)); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*   } */
/*   else if(fitOption == "students+voigt"){ */
/*     TF1 *fitted = studentsPlusVoigt(fitName, histogram,0, low, high); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*   } */
/*   else if(fitOption == "voigt+students"){ */
/*     TF1 *fitted = studentsPlusVoigt(fitName, histogram,1, low, high); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*     TF1 *studentsFitHigh = studentsClone(fitted, 0); */
/*     resolutions.push_back(equivalentRes(studentsFitHigh)); */
/*     resolutions.push_back(equivalentResError(studentsFitHigh)); */
/*   } */
/*   else if(fitOption == "lorentzian+lorentzian"){ */
/*     TF1 *fitted = lorentzianPlusLorentzian(fitName, histogram,0, low, high); */
/*     TF1 *lorentzianFitHigh = lorentzianClone(fitted, 0); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(lorentzianFitHigh)); */
/*     resolutions.push_back(equivalentResError(lorentzianFitHigh)); */
/*     TF1 *lorentzianFitLow = lorentzianClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(lorentzianFitLow )); */
/*     resolutions.push_back(equivalentResError(lorentzianFitLow )); */
/*   } */
/*   else if(fitOption == "lorentzian+voigt"){ */
/*     TF1 *fitted = lorentzianPlusVoigt(fitName, histogram,0, low, high); */
/*     TF1 *lorentzianFitHigh = lorentzianClone(fitted, 0); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(lorentzianFitHigh)); */
/*     resolutions.push_back(equivalentResError(lorentzianFitHigh)); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*   } */
/*   else if(fitOption == "voigt+lorentzian"){ */
/*     TF1 *fitted = lorentzianPlusVoigt(fitName, histogram,1, low, high); */
/*     TF1 *voigtFit = voigtClone(fitted, 1); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(voigtFit)); */
/*     resolutions.push_back(equivalentResError(voigtFit)); */
/*     TF1 *lorentzianFitHigh = lorentzianClone(fitted, 0); */
/*     resolutions.push_back(equivalentRes(lorentzianFitHigh)); */
/*     resolutions.push_back(equivalentResError(lorentzianFitHigh)); */
/*   } */
/*   else if(fitOption == "voigt+voigt"){ */
/*     TF1 *fitted = voigtPlusVoigt(fitName, histogram,0, low, high); */
/*     TF1 *voigtFitHigh = voigtClone(fitted, 0); */
/*      resolutions.push_back(fitted->GetParameter(1)); */
/*     resolutions.push_back(fitted->GetParError(1)); */
/*  resolutions.push_back(equivalentRes(voigtFitHigh)); */
/*     resolutions.push_back(equivalentResError(voigtFitHigh)); */
/*     TF1 *voigtFitLow = voigtClone(fitted, 1); */
/*     resolutions.push_back(equivalentRes(voigtFitLow)); */
/*     resolutions.push_back(equivalentResError(voigtFitLow)); */
/*   } */

  

/*   return resolutions; */
/* }//end of function resFit */




/* ///////////////////Functions returning a fit. */

/* TF1 *gaussian(string fitName, TH1 *histogram, int order, double low, double high){ */

/*   TF1 *fitPrimus = new TF1(fitName.c_str(), "gaus", low, high); */
/*   fitPrimus->SetParameters(histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()) + .5*histogram->GetBinWidth(histogram->GetMaximumBin()), histogram->GetRMS(), 0); */
/*   fitPrimus->SetParLimits(0, .75*histogram->GetMaximum(), 1.25*histogram->GetMaximum()); */
/*   fitPrimus->FixParameter(3, 0); */
/*   histogram->Fit(fitName.c_str(), "RQ"); */

/*   return fitPrimus; */
/* }//end of gaussian fit function */

/* TF1 *lorentzian(string fitName, TH1 *histogram, int order, double low, double high){ */
/*   TF1 *fitPrimus = new TF1(fitName.c_str(), lorentzianFunction, low, high, 3); */
/*   fitPrimus->SetParameters(histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0); */
/*   fitPrimus->SetParLimits(0, .9*histogram->GetMaximum(), 1.1*histogram->GetMaximum()); */
/*   fitPrimus->FixParameter(3, 0); */
/*   histogram->Fit(fitName.c_str(), "RQ"); */

/*   return fitPrimus; */
/* }//end of lorentzian fit function */

/* TF1 *students(string fitName, TH1 *histogram, int order, double low, double high){ */
/*   TF1 *fitPrimus = new TF1(fitName.c_str(), studentsTDistribution, low, high, 4); */
/*   fitPrimus->SetParameters(histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetNbinsX()); */
/*   fitPrimus->SetParLimits(0, .9*histogram->GetMaximum(), 1.1*histogram->GetMaximum()); */
/*   fitPrimus->SetParLimits(3, 1, 341); //number seems random, but any more than that and the function returns NAN. */

/*   histogram->Fit(fitName.c_str(), "RNQ"); */

/*   return fitPrimus; */
/* }//end of student's t-distribution fit function */

/* TF1 *voigt(string fitName, TH1 *histogram, int order, double low, double high){ */

/*   TF1 *fitPrimus = new TF1(fitName.c_str(), voigtProfile, low, high, 4); */
/*   fitPrimus->SetParameters(histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetRMS()); */
/*   fitPrimus->SetParLimits(0, .9*histogram->GetMaximum(), 1.1*histogram->GetMaximum()); */
/*   fitPrimus->SetParLimits(2, 1e-10, 1e10); */
/*   fitPrimus->SetParLimits(3, 1e-10, 1e10); */
/*   histogram->Fit(fitName.c_str(), "RNQ"); */

/*   return fitPrimus; */
/* }//end of voigt profile fit function */

/* TF1 *studentsPlusStudents(string fitName, TH1 *histogram, int order, double low, double high){ */

/*  TF1 *fitPrimus = new TF1(fitName.c_str(), studentsTDistPlusStudentTDist, low, high, 7); */
/*  fitPrimus->SetParameters(histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetNbinsX(), .1*histogram->GetMaximum(), histogram->GetRMS(), histogram->GetNbinsX()); */
/*  fitPrimus->SetParLimits(0, .9*histogram->GetMaximum(), 1.1*histogram->GetMaximum()); */
/*  fitPrimus->SetParLimits(3, 1, 341); */
/*  fitPrimus->SetParLimits(4, 0.0*histogram->GetMaximum(), .3*histogram->GetMaximum()); */
/*  fitPrimus->SetParLimits(6, 1, 341); */

/*  histogram->Fit(fitName.c_str(), "RNQ"); */
 
/*  return fitPrimus; */
/* } */

/* TF1 *studentsPlusGauss(string fitName, TH1 *histogram, int order, double low, double high){ */
  
/*   if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*   else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*  TF1 *fitPrimus = new TF1(fitName.c_str(), studentsTDistPlusGaussDist, low, high, 7); */
/*  fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetNbinsX(), heights[3]**histogram->GetMaximum(), histogram->GetRMS(), 0); */
/*  fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*  fitPrimus->SetParLimits(3, 1, 341); */
/*  fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*  fitPrimus->SetParLimits(6, 0, 0); */
/*  fitPrimus->FixParameter(6, 0); */
/*  histogram->Fit(fitName.c_str(), "RNQ"); */

/*   return fitPrimus; */
/* } */
/*  TF1 *studentsPlusLorentzian(string fitName, TH1 *histogram, int order, double low, double high){ */
 
/*    if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*    else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*    TF1 *fitPrimus = new TF1(fitName.c_str(), studentsTDistPlusLorentzianFunction, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetNbinsX(), heights[3]**histogram->GetMaximum(), histogram->GetRMS(), 0); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 1, 341); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(6, 0, 0); */
/*    fitPrimus->FixParameter(6, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   
/*    return fitPrimus; */
/* } */
/*  TF1 *studentsPlusVoigt(string fitName, TH1 *histogram, int order, double low, double high){ */

/*    if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*    else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*    TF1 *fitPrimus = new TF1(fitName.c_str(), studentsTDistPlusVoigtProfile, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetNbinsX(), heights[3]**histogram->GetMaximum(), histogram->GetRMS(), histogram->GetRMS()); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 1, 341); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParameter(6, histogram->GetRMS()); */
/*    fitPrimus->SetParameter(2, histogram->GetRMS()); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   
/*   return fitPrimus; */
/* } */
/*  TF1 *lorentzianPlusVoigt(string fitName, TH1 *histogram, int order, double low, double high){ */

/*    if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*    else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*    TF1 *fitPrimus = new TF1(fitName.c_str(), lorentzianFunctionPlusVoigtProfile, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0, heights[3]**histogram->GetMaximum(), histogram->GetRMS(), histogram->GetRMS()); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 0, 0); */
/*    fitPrimus->FixParameter(3, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   

/*   return fitPrimus; */
/* } */
/*  TF1 *gaussPlusLorentzian(string fitName, TH1 *histogram, int order, double low, double high){ */

   
/*    if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*    else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*    TF1 *fitPrimus = new TF1(fitName.c_str(), gaussDistPlusLorentzianFunction, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0, heights[3]**histogram->GetMaximum(), histogram->GetRMS(), 0); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(6, 0, 0); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 0, 0); */
/*    fitPrimus->FixParameter(3, 0); */
/*    fitPrimus->FixParameter(6, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   

/*   return fitPrimus; */
/* } */

/*  TF1 *lorentzianPlusLorentzian(string fitName, TH1 *histogram, int order, double low, double high){ */


/*    Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
   
/*    TF1 *fitPrimus = new TF1(fitName.c_str(), lorentzianFunctionPlusLorentzianFunction, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0, heights[3]**histogram->GetMaximum(), histogram->GetRMS(), 0); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(6, 0, 0); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 0, 0); */
/*    fitPrimus->FixParameter(6, 0); */
/*    fitPrimus->FixParameter(3, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   


/*   return fitPrimus; */
/* } */


/* TF1 *gaussPlusGauss(string fitName, TH1 *histogram, int order, double low, double high){ */


/*  Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
   
/*    TF1 *fitPrimus = new TF1(fitName.c_str(), gaussDistPlusGaussDist, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0, heights[3]**histogram->GetMaximum(), histogram->GetRMS(), 0); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(6, 0, 0); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 0, 0); */
/*     fitPrimus->FixParameter(6, 0); */
/*     fitPrimus->FixParameter(3, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   


/*   return fitPrimus; */
/* } */
/* TF1 *gaussPlusVoigt(string fitName, TH1 *histogram, int order, double low, double high){ */

/*   if(order == 0) Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
/*    else if (order == 1) Double_t heights[6] = {.1, .3, 0, 1, 1.1, .9}; */

/*    TF1 *fitPrimus = new TF1(fitName.c_str(), gaussDistPlusVoigtProfile, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), 0, heights[3]**histogram->GetMaximum(), histogram->GetRMS(), histogram->GetRMS()); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*       fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(3, 0, 0); */
/*    fitPrimus->FixParameter(3, 0); */
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   


/*   return fitPrimus; */
/* } */

/* TF1 *voigtPlusVoigt(string fitName, TH1 *histogram, int order, double low, double high){ */

/*   Double_t heights[6] = {1.0, 1.1, .9, .1, .3, 0}; */
   
/*    TF1 *fitPrimus = new TF1(fitName.c_str(), gaussDistPlusVoigtProfile, low, high, 7); */
/*    fitPrimus->SetParameters(heights[0]*histogram->GetMaximum(), histogram->GetBinLowEdge(histogram->GetMaximumBin()), histogram->GetRMS(), histogram->GetRMS(), heights[3]**histogram->GetMaximum(), histogram->GetRMS(), histogram->GetRMS()); */
/*    fitPrimus->SetParLimits(0, heights[1]*histogram->GetMaximum(), heights[2]*histogram->GetMaximum()); */
/*    fitPrimus->SetParLimits(4, heights[5]*histogram->GetMaximum(), heights[4]*histogram->GetMaximum()); */
   
   
/*    histogram->Fit(fitName.c_str(), "RNQ"); */
   

/*   return fitPrimus; */
/* } */




