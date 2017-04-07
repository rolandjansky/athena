/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Macro that makes the range of a hisogram symmetric
//
void SymmetricYaxis(TProfile *myHist, bool IncludingErrors, bool AroundZero)
{
  double CurrentMax = -99999999.; // initialization to a silly value
  double CurrentMin =  99999999.;
  double FinalMax, FinalMin;
  double thismax, thismin;
  for (int bin=1; bin<= myHist->GetNbinsX(); bin++){
    thismax = myHist->GetBinContent(bin);
    thismin = thismax;

    if (IncludingErrors) thismax += myHist->GetBinError(bin);
    if (IncludingErrors) thismin -= myHist->GetBinError(bin);

    if (thismax > CurrentMax) CurrentMax = thismax;
    if (thismin < CurrentMin) CurrentMin = thismin;
  }

  // cout << " <SymmetricYaxis> " << CurrentMax << " --> " << CurrentMin << endl;
  double gap = CurrentMax - CurrentMin;
  
  if (AroundZero) {
    CurrentMax += 0.05*gap;
    CurrentMin -= 0.05*gap;
    // cout << " <SymmetricYaxis> " << CurrentMax << " --> " << CurrentMin << endl;
    double absoluteMax = fabs(CurrentMax);
    if (fabs(CurrentMin)> absoluteMax) absoluteMax = fabs(CurrentMin);
    FinalMax =  absoluteMax;
    FinalMin = -absoluteMax;
  }
  else {
    double average = (CurrentMax+ CurrentMin)/2;
    FinalMax = average+0.55*gap;
    FinalMin = average-0.55*gap;
  }

  // cout << " <SymmetricYaxis> Final " << FinalMin << " --> " << FinalMax << endl;
  myHist->SetMaximum(FinalMax);
  myHist->SetMinimum(FinalMin);

  return;
}

