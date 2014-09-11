/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /*
 *  Class definition of GSCTool - see Root/GSC.cxx for more details
 *  Joe Taenzer (joseph.taenzer@cern.ch), Nov 2012
 */

#ifndef _GLOBALSEQUENTIALCALIBRATION_
#define _GLOBALSEQUENTIALCALIBRATION_

#include <iostream>
#include <fstream>
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TMath.h>
#include <TString.h>
#include <TH2F.h>
#include <TGraph2D.h>
#include <TLorentzVector.h>
#include <vector>

class GSCTool : public TNamed {

 public:

  GSCTool();
  GSCTool(TString jetAlgo, TString GSCFactorsFile);
  virtual ~GSCTool();

#ifdef ROOTCORE
      ClassDef(GSCTool,1);
#endif

  //shared typedefs with Jet Calibration Tool
  typedef std::vector<TH2F*> VecTH2F;
  typedef std::vector<double> VecD;
  typedef std::vector<TString> StrV;
  typedef unsigned int uint;
  enum _GSCseq { ApplyTile0 = 1, ApplyEM3 = 2, ApplynTrk = 4, ApplytrackWIDTH = 8, ApplyPunchThrough = 16 };
  //end shared typedefs

  void initGSC(TString jetAlgo, TString GSCFile);

  double GettrackWIDTHResponse(double pT, uint etabin, double trackWIDTH);
  double GetnTrkResponse(double pT, uint etabin, double nTrk);
  double GetTile0Response(double pT, uint etabin, double Tile0);
  double GetEM3Response(double pT, uint etabin, double EM3);
  double GetPunchThroughResponse(double E, double eta_det, int Nsegments);

  double GetGSCCorrection(TLorentzVector jet, double eta,
			  double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments,
			  int depth = ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH | ApplyPunchThrough);

  double GetjetPropertyMax(TString jetPropName, unsigned int etabin) {
    if ( jetPropName.Contains("EM3") && etabin < _EM3MaxEtaBin ) return _respFactorsEM3[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("Tile0") && etabin < _Tile0MaxEtaBin ) return _respFactorsTile0[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("nTrk") && etabin < _nTrkMaxEtaBin ) return _respFactorsnTrk[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("trackWIDTH") && etabin < _trackWIDTHMaxEtaBin ) return _respFactorstrackWIDTH[etabin]->GetYaxis()->GetXmax();
    else return 1;
  }

  void SetPunchThroughEtaBins(VecD etabins) { 
    if (etabins.size()==0) error("Please check that the punch through eta binning is properly set in your config file");
    _punchThroughEtaBins=etabins;
  }

  void SetPunchThroughMinPt(double minPt) { 
    if (minPt < 0) error("Error determining the punch through minimum pT");
    _punchThroughMinPt = minPt;
  }

 private:

  double ReadPtjetPropertyHisto(double pT, double jetProperty,
				TH2F *respFactors);

  bool FileExist(TString fn) { return gSystem->AccessPathName(fn)==false; };

  TFile* openInputFile(TString fn) { 
    if ( !FileExist(fn)) error("Cannot open GSC factors file"+fn);
    TFile* fileHandle = new TFile(fn);
    if (fileHandle == NULL) error( "Found but could not open file %s"+fn);
    return fileHandle;
  };

  TH2F *GetHisto(TFile *file, TString hname) {
    TH2F *h = (TH2F*)file->Get(hname);
    if (h==NULL) printf("WARNING: Cannot access histogram \"%s\" in file %s",hname.Data(),file->GetName());
    return h;
  }

  //shared functions/variables with JetCalibrationTool
  void error(TString msg) 
  { printf("\nERROR - GSCTool:\n\n  %s\n\n",msg.Data()); abort(); }

  TString FindFile(TString fn);
  //end shared functions
  TString _basePath;

  VecTH2F _respFactorsEM3, _respFactorsnTrk, _respFactorstrackWIDTH, _respFactorsTile0, _respFactorsPunchThrough;

  double _binSize;
  uint _trackWIDTHMaxEtaBin, _nTrkMaxEtaBin, _Tile0MaxEtaBin, _EM3MaxEtaBin;
  double _etaGapMin, _etaGapMax;
  VecD _punchThroughEtaBins;
  double _punchThroughMinPt;

};

#endif
