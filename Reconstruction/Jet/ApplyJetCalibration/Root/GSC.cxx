/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Calculates global sequential jet calibration factors
 *   -Requires jet branches trackWIDTH, nTrk, Tile0, EM3
 *   -Apply using
 *    1. No jet area correction: JetCalibrationTool::ApplyOffsetEtaJESGSC
 *    2. With jet area correction (not yet supported): JetCalibrationTool::ApplyJetAreaOffsetEtaJESGSC
 *   -GSC correction factor is returned by JetCalibrationTool::GetGSC
 *    TFile* inputFile = NULL;
    inputFile = openInputFile(m_fileName);
 *
 *  Extension of the ApplyJetCalibrationTool
 *
 *  Authors: Joe Taenzer (joseph.taenzer@cern.ch), Reina Camacho
 *
 */

#include "ApplyJetCalibration/GSC.h"
#include <TMath.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TEnv.h>
#include <TKey.h>
#include <TObjString.h>

GSCTool::GSCTool()
  :   _binSize(0.1), _depth(0), _trackWIDTHMaxEtaBin(25), _nTrkMaxEtaBin(25), _Tile0MaxEtaBin(17), _EM3MaxEtaBin(35), _etaGapMin(0), _etaGapMax(0), _punchThroughMinPt(50)
{

}

GSCTool::GSCTool(TString jetAlgo, TString GSCFactorsFile, TString DepthString)
  :   _binSize(0.1), _depth(0), _trackWIDTHMaxEtaBin(25), _nTrkMaxEtaBin(25), _Tile0MaxEtaBin(17), _EM3MaxEtaBin(35), _etaGapMin(0), _etaGapMax(0), _punchThroughMinPt(50)
{
  initGSC(jetAlgo, GSCFactorsFile, DepthString);
}

GSCTool::~GSCTool()
{

}

double GSCTool::ReadPtjetPropertyHisto(double pT, double jetProperty, TH2F *respFactors) {
 int pTbin = respFactors->GetXaxis()->FindBin(pT);
 int pTMinbin = respFactors->GetXaxis()->GetFirst();
 int pTMaxbin = respFactors->GetXaxis()->GetLast();
 int jetPropbin = respFactors->GetYaxis()->FindBin(jetProperty);
 int jetPropMinbin = respFactors->GetYaxis()->GetFirst();
 int jetPropMaxbin = respFactors->GetYaxis()->GetLast();
 //Protection against input values that are outside the histogram range, which would cause TH2::Interpolate to throw an error
 if (pTbin < pTMinbin) pT = respFactors->GetXaxis()->GetBinLowEdge(pTMinbin)+1e-6;
 else if (pTbin > pTMaxbin) pT = respFactors->GetXaxis()->GetBinUpEdge(pTMaxbin)-1e-6;
 if (jetPropbin < jetPropMinbin) jetProperty = respFactors->GetYaxis()->GetBinLowEdge(jetPropMinbin)+1e-6;
 else if (jetPropbin > jetPropMaxbin) jetProperty = respFactors->GetYaxis()->GetBinUpEdge(jetPropMaxbin)-1e-6;
 //TH2::Interpolate is a bilinear interpolation from the bin centers.
 return respFactors->Interpolate(pT, jetProperty);
}

double GSCTool::GettrackWIDTHResponse(double pT, uint etabin, double trackWIDTH) {
  if (trackWIDTH<=0) return 1;
  if ( etabin >= _respFactorstrackWIDTH.size() ) return 1.;
  //jets with no tracks are assigned a trackWIDTH of -1, we use the trackWIDTH=0 correction in those cases
  double trackWIDTHResponse = ReadPtjetPropertyHisto(pT, trackWIDTH, _respFactorstrackWIDTH[etabin]);
  return trackWIDTHResponse;
}

double GSCTool::GetnTrkResponse(double pT, uint etabin, double nTrk) {
  if (nTrk<=0) return 1; //nTrk < 0 is unphysical, nTrk = 0 is a special case, so return 1 for nTrk <= 0
  if ( etabin >= _respFactorsnTrk.size() ) return 1.;
  double nTrkResponse = ReadPtjetPropertyHisto(pT, nTrk, _respFactorsnTrk[etabin]);
  return nTrkResponse;
}

double GSCTool::GetTile0Response(double pT, uint etabin, double Tile0) {
  if (Tile0<0) return 1; //Tile0 < 0 is unphysical, so we return 1
  if ( etabin >= _respFactorsTile0.size() ) return 1.;
  double Tile0Response = ReadPtjetPropertyHisto(pT, Tile0, _respFactorsTile0[etabin]);
  return Tile0Response;
}

double GSCTool::GetEM3Response(double pT, uint etabin, double EM3) {
  if (EM3<=0) return 1; //EM3 < 0 is unphysical, EM3 = 0 is a special case, so we return 1 for EM3 <= 0
  if ( etabin >= _respFactorsEM3.size() ) return 1.;
  double EM3Response = ReadPtjetPropertyHisto(pT, EM3, _respFactorsEM3[etabin]);
  return EM3Response;
}

double GSCTool::GetPunchThroughResponse(double E, double eta_det, int Nsegments) {
  int etabin=-99;
  //Check that the punch through eta binning defined in the config appears reasonable, otherwise throw an error.
  if (_punchThroughEtaBins.size()==0 || _respFactorsPunchThrough.size() != _punchThroughEtaBins.size()-1) error("Please check that the punch through eta binning is properly set in your config file");
  if ( eta_det >= _punchThroughEtaBins.back() || Nsegments < 20 ) return 1;
  for (uint i=0; i<_punchThroughEtaBins.size()-1; ++i) {
    if(eta_det >= _punchThroughEtaBins[i] && eta_det < _punchThroughEtaBins[i+1]) etabin = i;
  }
  if(etabin<0) error("There was a problem determining the eta bin to use for the punch through correction.");
  double PunchThroughResponse = ReadPtjetPropertyHisto(E,log(Nsegments),_respFactorsPunchThrough[etabin]);
  if ( PunchThroughResponse > 1 ) return 1;
  return PunchThroughResponse;
}

double GSCTool::GetGSCCorrection(TLorentzVector jet, double eta, 
				 double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments) {
  //eta bins have size _binSize=0.1 and are numbered sequentially from 0, so |eta|=2.4 is in eta bin #24
  int etabin = eta/_binSize;
  double Corr=1;
  //Using bit sequence check to determine which GS corrections to apply.
  if (_depth & ApplyTile0)      Corr*=1./GetTile0Response(jet.Pt(), etabin, Tile0);
  if (_depth & ApplyEM3)        Corr*=1./GetEM3Response(jet.Pt()*Corr, etabin, EM3);
  if (_depth & ApplynTrk)       Corr*=1/GetnTrkResponse(jet.Pt()*Corr, etabin, nTrk);
  if (_depth & ApplytrackWIDTH) Corr*=1/GettrackWIDTHResponse(jet.Pt()*Corr,etabin,trackWIDTH); 
  if ( jet.Pt() < _punchThroughMinPt ) return Corr; //Applying punch through correction to low pT jets introduces a bias, default threshold is 50 GeV
  //eta binning for the punch through correction differs from the rest of the GSC, so the eta bin is determined in the GetPunchThroughResponse method
  else if (_depth & ApplyPunchThrough) {
    jet*=Corr; //The punch through correction is binned in E instead of pT, so we determine E from the corrected jet here
    Corr*=1/GetPunchThroughResponse(jet.E(),eta,Nsegments);
  }
  return Corr;
}

void GSCTool::initGSC(TString jetAlgo, TString GSCFile, TString DepthString) {

  if (GSCFile=="") error("No GSC factors file specified.");
  if (jetAlgo=="") error("No jet algorithm specified.");

  printf("\n\n");
  printf("===================================\n\n");
  printf("  Initializing the Global Sequential Calibration tool\n");
  printf("  for %s jets\n\n",jetAlgo.Data());

  //set the depth private variable, used to determine which parts of the GS calibration are applied
      if ( DepthString.Contains("PunchThrough") || DepthString.Contains("Full") ) _depth = ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH | ApplyPunchThrough;
      else if ( DepthString.Contains("trackWIDTH") ) _depth = ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH;
      else if ( DepthString.Contains("nTrk") ) _depth = ApplyTile0 | ApplyEM3 | ApplynTrk;
      else if ( DepthString.Contains("EM3") ) _depth = ApplyTile0 | ApplyEM3;
      else if ( DepthString.Contains("Tile0") ) _depth = ApplyTile0;
      else error("GSCDepth flag not properly set, please check your config file.");

  //find the ROOT file containing response histograms, path comes from the config file.
  TString fn = FindFile(GSCFile);
  TFile *inputFile = NULL;
  inputFile = openInputFile(fn);
  if (inputFile==NULL) error("Cannot open GSC factors file"+fn);

  //Get a TList of TKeys pointing to the histograms contained in the ROOT file
  inputFile->cd();
  TList *keys = inputFile->GetListOfKeys();
  std::vector<TString> histoNames;
  //fill the names of the TKeys into a vector of TStrings
  TIter ikeys(keys);
  while ( TKey *iterobj = (TKey*)ikeys() ) { histoNames.push_back( iterobj->GetName() ); }

  //Grab the TH2Fs from the ROOT file and put them into a vectors of TH2Fs
  for (uint ihisto=0; ihisto<histoNames.size(); ++ihisto) {
    if ( !histoNames[ihisto].Contains( jetAlgo.Data() ) ) continue;
    else if ( ihisto>0 && histoNames[ihisto].Contains( histoNames[ihisto-1].Data() ) ) continue;
    else if ( histoNames[ihisto].Contains("EM3") && _respFactorsEM3.size() < _EM3MaxEtaBin) _respFactorsEM3.push_back( (TH2F*)GetHisto(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("nTrk") && _respFactorsnTrk.size() < _nTrkMaxEtaBin) _respFactorsnTrk.push_back( (TH2F*)GetHisto(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("Tile0") && _respFactorsTile0.size() < _Tile0MaxEtaBin) _respFactorsTile0.push_back( (TH2F*)GetHisto(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("trackWIDTH") && _respFactorstrackWIDTH.size() < _trackWIDTHMaxEtaBin) _respFactorstrackWIDTH.push_back( (TH2F*)GetHisto(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("PunchThrough") ) _respFactorsPunchThrough.push_back( (TH2F*)GetHisto(inputFile,histoNames[ihisto]) );
  }

  //Make sure we put something in the vectors of TH2Fs
  if ( (_depth & ApplyEM3) && _respFactorsEM3.size() < 3 ) error("Vector of EM3 histograms may be empty. Please check your GSCFactors file: "+GSCFile);
  else if ( (_depth & ApplynTrk) &&_respFactorsnTrk.size() < 3 ) error("Vector of nTrk histograms may be empty. Please check your GSCFactors file: "+GSCFile);
  else if ( (_depth & ApplyTile0) && _respFactorsTile0.size() < 3 ) error("Vector of Tile0 histograms may be empty. Please check your GSCFactors file: "+GSCFile);
  else if ( (_depth & ApplytrackWIDTH) && _respFactorstrackWIDTH.size() < 3 ) error("Vector of trackWIDTH histograms may be empty. Please check your GSCFactors file: "+GSCFile);
  else if ( (_depth & ApplyPunchThrough) && _respFactorsPunchThrough.size() < 2 ) error("Vector of PunchThrough histograms may be empty. Please check your GSCFactors file: "+GSCFile);
  else printf ("\n  GSC Tool has been initialized with binning and eta fit factors from %s\n", fn.Data());

}

// Find configuration file
TString GSCTool::FindFile(TString filename) {
    TString fn(filename); gSystem->ExpandPathName(fn); gSystem->ExpandPathName(_basePath);

    // First check the actual path
    if (gSystem->AccessPathName(fn)==false) { 
      TString path(fn); path.ReplaceAll(gSystem->BaseName(fn),"");
      path.ReplaceAll("CalibrationConfigs/",""); path.ReplaceAll("CalibrationFactors/","");
      _basePath=path;
      // account for possible subfolders, e.g. muScan2013
      std::string temp_path = path.Data();
      std::size_t found = temp_path.find("CalibrationConfigs/");
      if (found!=std::string::npos) temp_path.erase(found);
      found = temp_path.find("CalibrationFactors/");
      if (found!=std::string::npos) temp_path.erase(found);
      _basePath=temp_path;
      // path.ReplaceAll("CalibrationConfigs/",""); path.ReplaceAll("CalibrationFactors/","");
      //_basePath=path;
      return fn;
    }

    // Add / to path name if needed
    if (_basePath.Length()&&_basePath[_basePath.Length()-1]!='/') _basePath+='/';
  
    // if not there, check the directory were the last found file was ...
    TString lastpath=_basePath;
    if (gSystem->AccessPathName(_basePath+fn)==false) return _basePath+fn;

    // Let's try to pick up the calibration files in the RootCore folder
    TString RootCoreDir = gSystem->Getenv("ROOTCOREBIN");
    if (RootCoreDir == "") RootCoreDir = gSystem->Getenv("ROOTCOREDIR");
    if (RootCoreDir != "") {
      TString path=RootCoreDir+"/data/ApplyJetCalibration/"; // this should always work
      if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }
      path=RootCoreDir+"../ApplyJetCalibration/data/";
      if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }
    }

    TString TestAreaDir = gSystem->Getenv("TestArea");
    if(TestAreaDir != "") {
      TString path=TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/CalibrationConfigs/"+gSystem->BaseName(fn);
      if(gSystem->AccessPathName(path)==false) {
	_basePath = path;
	_basePath.ReplaceAll(gSystem->BaseName(fn),""); _basePath.ReplaceAll("CalibrationConfigs/","");
	return path;
      }
      path=TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/"+fn;
      if(gSystem->AccessPathName(path)==false) {
	_basePath = TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/";
	return _basePath+fn;
      }
      path=TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/CalibrationConfigs/"+gSystem->BaseName(fn);
      if(gSystem->AccessPathName(path)==false) {
	_basePath = path;
	_basePath.ReplaceAll(gSystem->BaseName(fn),""); _basePath.ReplaceAll("CalibrationConfigs/","");
	return path;
      }
      path=TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/"+fn;
      if(gSystem->AccessPathName(path)==false) {
	_basePath = TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/";
	return _basePath+fn;
      }
    }			 
  
    // getting a bit desperate here... check if its one level up
    TString path="ApplyJetCalibration/data/";
    if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }

    // if needed, can here add a loop over the gSystem->GetIncludePath() list of directories

    printf("Cannot find file %s\n",filename.Data());
    printf("Searched in:\n  ./%s\n",filename.Data());
    printf("  %s\n",(lastpath+filename).Data());
    if (RootCoreDir!="") {
      printf("  %s\n  %s\n",
	     (RootCoreDir+"/data/ApplyJetCalibration/"+fn).Data(),
	     (RootCoreDir+"/../ApplyJetCalibration/data/"+fn).Data());
    }
    if(TestAreaDir!="") { 
      printf(" %s\n",
	     (TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/CalibrationConfigs/"+gSystem->BaseName(fn)).Data());
      printf(" %s\n",
	     (TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/"+fn).Data());
      printf(" %s\n",
	     (TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/CalibrationConfigs/"+gSystem->BaseName(fn)).Data());
      printf(" %s\n",
	     (TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/"+fn).Data());
    }
    printf("  ./ApplyJetCalibration/data/%s\n",filename.Data());
    error("Cannot find file "+filename);
    return "";
}
