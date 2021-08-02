/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/MonitorBase.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/OFC.h"
#include "LArSamplesMon/ShapeFitter.h"
#include "LArCafJobs/Geometry.h"
#include "LArSamplesMon/Averager.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <map>
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


const History* MonitorBase::getCellHistory(unsigned int i) const 
{ 
  return interface().getCellHistory(i); 
}

const CellInfo* MonitorBase::getCellInfo(unsigned int i) const 
{ 
  return interface().getCellInfo(i);
}


unsigned int MonitorBase::nChannels() const 
{ 
  return interface().nChannels(); 
}


TString  MonitorBase::str(CombinationType comb)
{
  switch (comb) {
    case AverageValue : return "average";
    case MaxValue     : return "max value";
    case MinValue     : return "min value";
    case TotalValue   : return "total value";
    case RMSValue     : return "RMS";
  }
  return "";
}


TH1D* MonitorBase::dist(const DataFuncSet& func, const DataFuncArgs& args, 
                        TString name, int nBins, double xMin, double xMax, 
                        TString title, TString xTitle, TString yTitle, const FilterParams& f) const
{
  TH1D* h = new TH1D(name, title, nBins, xMin, xMax);
  if (xTitle != "") h->GetXaxis()->SetTitle(xTitle);
  if (yTitle != "") h->GetYaxis()->SetTitle(yTitle);
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    if ((i+1) % printPeriodicity == 0) cout << "Cell # " << i+1 << "/" << m_interface->nChannels() << endl;
    const History* history = pass(i, f);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (!f.passEvent(*history->data(j))) continue;
      h->Fill(func.val(*history->data(j), args));
    }
  }
  return h;
}


TH2D* MonitorBase::dist(const DataFuncSet& funcX, const DataFuncArgs& argsX,
                        const DataFuncSet& funcY, const DataFuncArgs& argsY,
                        TString name, 
                        int nBinsX, double xMin, double xMax, 
                        int nBinsY, double yMin, double yMax, 
                        TString title, TString xTitle, TString yTitle, 
                        const FilterParams& f) const
{
  TH2D* h = new TH2D(name, title, nBinsX, xMin, xMax, nBinsY, yMin, yMax);
  if (xTitle != "") h->GetXaxis()->SetTitle(xTitle);
  if (yTitle != "") h->GetYaxis()->SetTitle(yTitle);
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    if ((i+1) % printPeriodicity == 0) cout << "Cell # " << i+1 << "/" << m_interface->nChannels() << endl;
    const History* history = pass(i, f);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (!f.passEvent(*history->data(j))) continue;
      h->Fill(funcX.val(*history->data(j), argsX), 
	      funcY.val(*history->data(j), argsY));
    }
  }
  return h;
}

TH2D* MonitorBase::partitionMap(const DataFuncSet& func, const DataFuncArgs& args, TString name, PartitionId partition, 
                                TString title, CombinationType comb, const FilterParams& f) const
{
  TH2D* h = Geo::partitionHist(partition, name, title + " " + Id::str(partition));
  unsigned int nValues = 0;
  FilterParams ff(f);
  ff.addPartition(partition);
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    if ((i+1) % printPeriodicity == 0) cout << "Cell # " << i+1 << "/" << m_interface->nChannels() << endl;
    const History* history = pass(i, ff);
    if (!history) continue;
    h->SetBinContent(history->cellInfo()->feb() + 1, history->cellInfo()->channel() + 1, 
		     history_value(*history, func, args, comb, f, nValues));
  }
  return h;
}


TH2D* MonitorBase::etaPhiMap(const DataFuncSet& func, const DataFuncArgs& args,
                             TString name, CaloId calo, short layer,
                             TString title, CombinationType comb, const FilterParams& f) const
{
  if (title == "") title = Form("%s %s (%s, layer %d)", name.Data(), str(comb).Data(), Id::str(calo).Data(), layer);
  TH2D* h = Geo::etaPhiHist(calo, layer, name, title);
  FilterParams ff(f);
  ff.addCalo(calo);
  ff.addLayer(layer);
  unsigned int nValues = 0;
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    if ((i+1) % printPeriodicity == 0) cout << "Cell # " << i+1 << "/" << m_interface->nChannels() << endl;
    const History* history = pass(i, ff);
    if (!history) continue;
    h->Fill(history->cellInfo()->eta(), history->cellInfo()->phi(), 
	    history_value(*history, func, args, comb, f, nValues));
  }
  return h;
}


bool MonitorBase::parseVariables(TString varStr, std::vector<TString>& vars, 
                                 std::vector<DataFuncSet>& funcs, std::vector<DataFuncArgs>& args)
{
  varStr.ReplaceAll(" ", "");
  TObjArray* varList = varStr.Tokenize(":");
  for (int i = 0; i < varList->GetEntries(); i++) {
    TString varAndSample = ((TObjString*)varList->At(i))->String();
    TString var = varAndSample;
    short sample1 = -1, sample2 = -1;
    double par = Definitions::none;
    TString str = "";
    if (varAndSample.CountChar('[') + varAndSample.CountChar(']') > 0) {
      if (varAndSample.CountChar('[') != 1 || varAndSample.CountChar(']') != 1 || varAndSample[varAndSample.Length() - 1] != ']') { 
	cout << "Invalid use of [ ] delimiters" << endl;
	delete varList; 
	return false; 
      }
      TObjArray* varSampList = varAndSample.Tokenize("[,]");
      if (varSampList->GetEntries() == 2) { 
        var = ((TObjString*)varSampList->At(0))->String();
        TString sampStr = ((TObjString*)varSampList->At(1))->String();
        delete varSampList;
        if (!sampStr.IsDigit()) { 
          cout << "Sample argument [sample] should be an integer" << endl;        
          delete varList; 
          return false; 
        }
        sample1 = sampStr.Atoi();
      }
      else if (varSampList->GetEntries() == 3) { 
        var = ((TObjString*)varSampList->At(0))->String();
        TString sampStr1 = ((TObjString*)varSampList->At(1))->String();
        TString sampStr2 = ((TObjString*)varSampList->At(2))->String();
        delete varSampList;
        if (!sampStr1.IsDigit() || !sampStr2.IsDigit()) { 
          cout << "Sample arguments [sample1, sample2] should be an integer" << endl;
          delete varList; 
          return false; 
        }
        sample1 = sampStr1.Atoi();
        sample2 = sampStr2.Atoi();
      }
      else {
	cout << "Invalid specification of sample argument (expected var[sample])" << endl;	
	delete varSampList; 
	delete varList; 
	return false; 
      }
    }
    if (varAndSample.CountChar('(') + varAndSample.CountChar(')') > 0) {
      if (varAndSample.CountChar('(') != 1 || varAndSample.CountChar(')') != 1 || varAndSample[varAndSample.Length() - 1] != ')') { 
        cout << "Invalid use of ( ) delimiters" << endl;
        delete varList; 
        return false; 
      }
      TObjArray* varSampList = varAndSample.Tokenize("(,)");
      if (varSampList->GetEntries() == 2) { 
        var = ((TObjString*)varSampList->At(0))->String();
        TString sampStr = ((TObjString*)varSampList->At(1))->String();
        delete varSampList;
        if (!sampStr.IsFloat()) { 
          cout << "floating-point argument expected between ()" << endl;        
          delete varList; 
          return false; 
        }
        par = sampStr.Atof();
      }
      else {
        cout << "Invalid specification of floating-point argument (expected func(arg))" << endl;      
        delete varSampList; 
        delete varList; 
        return false; 
      }
    }
    if (varAndSample(0, 5) == "pass_") {
      str = varAndSample(5, varAndSample.Length() - 5);
      var = "pass";
    }
    DataFuncSet fcn = func(var);
    if (fcn.isNull()) {
      cout << "Unknown variable " << var << endl;
      return false;
    }
    vars.push_back(varAndSample);
    funcs.push_back(fcn);
    args.push_back(DataFuncArgs(sample1, sample2, par, Definitions::none, str));
  }
  delete varList;
  return true;
}


bool MonitorBase::prepareDumpParams(const TString& vars, int verbosity, 
                                    std::vector<TString>& variables, std::vector<DataFuncSet>& funcs, 
                                    std::vector<DataFuncArgs>& args,
                                    std::vector<TString>& formats, TString& locFormat, 
                                    TString& locHeader, TString& varHeader) const
{
  if (!parseVariables(vars, variables, funcs, args)) {
    cout << "Invalid variable specification " << vars << endl;
    return false;
  }

  locFormat = "| %6d | %-";
  if (verbosity <= 1)
    locFormat += "26";
  else if (verbosity == 2)
    locFormat += "80";
  else if (verbosity == 3)
    locFormat += "108";
  else
    locFormat += "120";
  locFormat += "s |";
  
  TString locFormatHeader = locFormat; locFormatHeader.ReplaceAll("d", "s");
  locHeader = Form(locFormatHeader, "hash", "location");
    
  for (unsigned int i = 0; i < variables.size(); i++) {
    formats.push_back(" %-9.8g |");
    TString fH = formats[i]; fH.ReplaceAll("g", "s");
    TString varName = variables[i];
    if (args[i].i2 != -1) varName += Form("[%d, %d]", args[i].i1, args[i].i2);
    else if (args[i].i1 != -1) varName += Form("[%d]", args[i].i1);
    if (!Definitions::isNone(args[i].x1)) varName += Form("[%.5f]", args[i].x1);
    if (args[i].str != "") varName += Form("_%s", args[i].str.Data());
    varHeader += Form(fH, varName.Data());
  }
  return true;
}



bool MonitorBase::dump(const TString& vars, const FilterParams& f, unsigned int verbosity) const
{
  std::vector<TString> variables, formats;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  TString locFormat, locHeader, varHeader;
  if (!prepareDumpParams(vars, verbosity, variables, funcs, args,
                         formats, locFormat, locHeader, varHeader)) return false;
  TString header = locHeader + " idx |" + varHeader;
  TString separator = "";
  for (int i = 0; i < header.Length(); i++) separator += "-";
  cout << separator << endl << header << endl << separator << endl;
  
  unsigned int nEntries = 0;
  
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    const History* history = pass(i, f);
    if (!history) continue;
    TString part1 = Form(locFormat, i, history->cellInfo()->location(verbosity).Data());
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (!f.passEvent(*history->data(j))) continue;
      TString line = part1 + Form(" %3d |", j);
      for (unsigned int k = 0; k < variables.size(); k++)
        line += Form(formats[k].Data(), funcs[k].val(*history->data(j), args[k]));
      cout << line << endl;
      nEntries++;
    }
  }
  cout << nEntries << " entries selected" << endl;
  return true;
}


bool MonitorBase::dump(const TString& vars, CombinationType comb, const FilterParams& f, 
                       const TString& ranges, unsigned int verbosity) const
{
  std::vector<TString> variables, formats;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  TString locFormat, locHeader, varHeader;
  if (!prepareDumpParams(vars, verbosity, variables, funcs, args,
                         formats, locFormat, locHeader, varHeader)) return false;
 
  std::vector<double> minVals, maxVals;
  TObjArray* rangeList = ranges.Tokenize(":");
  for (unsigned int k = 0; k < variables.size(); k++) {
    TString range = (k < (unsigned int)rangeList->GetEntries() ? ((TObjString*)rangeList->At(k))->String() : "");
    if (range == "") {
      minVals.push_back(-DBL_MAX);
      maxVals.push_back(+DBL_MAX);
      continue;
    }
    TObjArray* minMaxList = range.Tokenize(",");
    if (minMaxList->GetEntries() == 2) {
      minVals.push_back(((TObjString*)minMaxList->At(0))->String().Atof());
      maxVals.push_back(((TObjString*)minMaxList->At(1))->String().Atof());
    }
    else {
      cout << "Invalid range specification " << range << endl;
      return false;
    }
    delete minMaxList;
  }
  delete rangeList;
 
 TString header = locHeader + varHeader;
  TString separator = "";
  for (int i = 0; i < header.Length(); i++) separator += "-";
  cout << separator << endl << header << endl << separator << endl;
  
  unsigned int nEntries = 0, nValues = 0;
  
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    const History* history = pass(i, f);
    if (!history) continue;
    TString line = Form(locFormat, i, history->cellInfo()->location(verbosity).Data());
    bool ok = true;
    for (unsigned int k = 0; k < variables.size(); k++) {
      double val = history_value(*history, funcs[k], args[k], comb, f, nValues);
      if (nValues == 0 || val < minVals[k] || val >= maxVals[k]) { ok = false; break; }
      line += Form(formats[k].Data(), val);
    }
    if (!ok) continue;
    cout << line << endl;
    nEntries++;
  }
  cout << nEntries << " entries selected" << endl;
  return true;
}


bool MonitorBase::statParams(const std::vector<DataFuncSet>& funcs, 
                             const std::vector<DataFuncArgs>& args,
                             TVectorD& mean, TVectorD& meanErr, TMatrixD& covMatrix, TMatrixD& covMatrixErr, 
                             const FilterParams& f) const
{
  Averager avg(funcs.size());
    
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    if ((i+1) % printPeriodicity == 0) cout << "Cell # " << i+1 << "/" << m_interface->nChannels() << endl;
    const History* history = pass(i, f);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      if (!f.passEvent(*history->data(j))) continue;
      TVectorD vals(funcs.size());
      for (unsigned int k = 0; k < funcs.size(); k++)
        vals(k) = funcs[k].val(*history->data(j), args[k]);
      avg.fill(vals);
    }
  }
  
  mean = avg.means();
  meanErr = avg.meanErrors();
  covMatrix = avg.covarianceMatrix();
  covMatrixErr = avg.covarianceMatrixErrors();

  return true;
}


double MonitorBase::history_value(const History& history, const DataFuncSet& func, const DataFuncArgs& args,
                                  CombinationType comb, const FilterParams& f, unsigned int& nValues)
{
  double val = 0, sum1 = 0, sum2 = 0;
  nValues = 0;
  if (comb == MaxValue) val = DBL_MIN;
  if (comb == MinValue) val = DBL_MAX;
  for (unsigned int j = 0; j < history.nData(); j++) {
    if (!f.passEvent(*history.data(j))) continue;
    double thisVal = func.val(*history.data(j), args);
    nValues++;
    switch (comb) {
      case AverageValue : val += thisVal; break;
      case MaxValue     : if (thisVal > val) val = thisVal; break;
      case MinValue     : if (thisVal < val) val = thisVal; break;
      case TotalValue   : val += thisVal; break;
      case RMSValue     : sum1 += thisVal; sum2 += thisVal*thisVal; break; 
    }
  }
  if (comb == AverageValue && nValues > 0) val /= nValues;
  if (comb == RMSValue) 
    val = (nValues > 0 ? TMath::Sqrt((sum2 - sum1*sum1/nValues)/nValues) : -1);
  if (TMath::Abs(val) > 0.9*DBL_MAX) val = 0;
  return val;
}


DataFuncSet MonitorBase::func(TString var)
{
  if (var == "sample")         return DataFuncSet(&Data::_sample);
  if (var == "pedSubSample")   return DataFuncSet(&Data::_pedestalSubstractedSample);
  if (var == "energy")         return DataFuncSet(&Data::_energy);
  if (var == "adcMax")         return DataFuncSet(&Data::_adcMax);
  if (var == "adcMax_new")     return DataFuncSet(&Data::_adcMax_new);
  if (var == "gain")           return DataFuncSet(&Data::_gain);
  if (var == "peakSignif")     return DataFuncSet(&Data::_peakSignif);
  if (var == "energySignif")   return DataFuncSet(&Data::_energySignif);
  if (var == "ofcTime")        return DataFuncSet(&Data::_ofcTime);
  if (var == "sampleMax")      return DataFuncSet(&Data::_maxValue);
  if (var == "maxPosition")    return DataFuncSet(&Data::_maxPosition);
  if (var == "quality")        return DataFuncSet(&Data::_quality);
  if (var == "noise")          return DataFuncSet(&Data::_noise);
  if (var == "pedestal")       return DataFuncSet(&Data::_pedestal);
  if (var == "pedestalRMS")    return DataFuncSet(&Data::_pedestalRMS);
  if (var == "maxValue")       return DataFuncSet(&Data::_maxValue);
  if (var == "minValue")       return DataFuncSet(&Data::_minValue);
  if (var == "count")          return DataFuncSet(&Data::_count);
  if (var == "chi2")           return DataFuncSet(&Data::_chi2);
  if (var == "chi2_noCorr")    return DataFuncSet(&Data::_chi2_noCorr);
  if (var == "chi2_ringCorr")  return DataFuncSet(&Data::_chi2_ringCorr);
  if (var == "chi2_cellCorr")  return DataFuncSet(&Data::_chi2_cellCorr);
  if (var == "chi2_k")         return DataFuncSet(&Data::_chi2_k);
  if (var == "refitScale")     return DataFuncSet(&Data::_refitScale);
  if (var == "refitDeltaT")    return DataFuncSet(&Data::_refitDeltaT);
  if (var == "refitChi2")      return DataFuncSet(&Data::_refitChi2);
  if (var == "adjScale")       return DataFuncSet(&Data::_adjScale);
  if (var == "adjDeltaT")      return DataFuncSet(&Data::_adjDeltaT);
  if (var == "run")            return DataFuncSet(&Data::_run);
  if (var == "event")          return DataFuncSet(&Data::_event);
  if (var == "lumiBlock")      return DataFuncSet(&Data::_lumiBlock);
  if (var == "bunchId")        return DataFuncSet(&Data::_bunchId);
  if (var == "x")              return DataFuncSet(&Data::_x);
  if (var == "y")              return DataFuncSet(&Data::_y);
  if (var == "z")              return DataFuncSet(&Data::_z);
  if (var == "rt")             return DataFuncSet(&Data::_rt);
  if (var == "eta")            return DataFuncSet(&Data::_eta);
  if (var == "phi")            return DataFuncSet(&Data::_phi);
  if (var == "iEta")           return DataFuncSet(&Data::_iEta);
  if (var == "iPhi")           return DataFuncSet(&Data::_iPhi);
  if (var == "region")         return DataFuncSet(&Data::_region);
  if (var == "calo")           return DataFuncSet(&Data::_calo);
  if (var == "layer")          return DataFuncSet(&Data::_layer);
  if (var == "feedthrough")    return DataFuncSet(&Data::_feedThrough);
  if (var == "FT")             return DataFuncSet(&Data::_feedThrough);
  if (var == "slot")           return DataFuncSet(&Data::_slot);
  if (var == "channel")        return DataFuncSet(&Data::_channel);
  if (var == "ring")           return DataFuncSet(&Data::_ring);
  if (var == "index")          return DataFuncSet(&Data::_index);
  if (var == "hash")           return DataFuncSet(&Data::_hash);
  if (var == "nData")          return DataFuncSet(&Data::_nData);
  if (var == "timeNoTOF")      return DataFuncSet(&Data::_timeNoTOF);
  if (var == "timeForSplash1") return DataFuncSet(&Data::_timeForSplash1);
  if (var == "timeForSplash2") return DataFuncSet(&Data::_timeForSplash2);
  if (var == "delta")          return DataFuncSet(&Data::_delta);
  if (var == "residual")       return DataFuncSet(&Data::_residual);
  if (var == "residualOffset") return DataFuncSet(&Data::_residualOffset);
  if (var == "resCorrN")       return DataFuncSet(&Data::_resCorrN);
  if (var == "xi")             return DataFuncSet(&Data::_xi);
  if (var == "xiCell")         return DataFuncSet(&Data::_xiCell);
  if (var == "xiRing")         return DataFuncSet(&Data::_xiRing);
  if (var == "xiNormCell")     return DataFuncSet(&Data::_xiNormCell);
  if (var == "xiNormRing")     return DataFuncSet(&Data::_xiNormRing);
  if (var == "xip")            return DataFuncSet(&Data::_xip);
  if (var == "residualError")  return DataFuncSet(&Data::_residualError);
  if (var == "ofcSigma")       return DataFuncSet(&Data::_ofcSigma);
  if (var == "ofcGamma")       return DataFuncSet(&Data::_ofcGamma);
  if (var == "normResidualOffset") return DataFuncSet(&Data::_normResidualOffset);
  if (var == "normResidualError")  return DataFuncSet(&Data::_normResidualError);
  if (var == "pass")               return DataFuncSet(&Data::_pass);
  if (var == "goodForShapeCorr")   return DataFuncSet(&Data::_goodForShapeCorr);
  if (var == "upstreamE")          return DataFuncSet(&Data::_upstreamE);
  if (var == "upstreamERatio")     return DataFuncSet(&Data::_upstreamERatio);
  if (var == "chi2Anomaly")        return DataFuncSet(&Data::_chi2Anomaly);
  if (var == "timeECorr")      return DataFuncSet(&Data::_timeECorr);

  return DataFuncSet();
}

