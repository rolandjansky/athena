/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Either load the library before running ".x testContainer.C+" (i.e. compiled),
// then you'll need the include. Or run ".x testContainer.C" and call
// gSystem->Load("libCalibrationDataInterface.so"); in here - but then the #include
// is in the way: CINT *replaces* elements from the dictionary when loading
// the #include :-(
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
#  include "CalibrationDataInterface/CalibrationDataContainer.h"
#endif

#include "CalibrationDataInterface/CalibrationDataBackend.h"
#include "TMatrixDSym.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TEnv.h"
#include "TString.h"
#include <string>
#include <iostream>

using Analysis::CalibrationDataContainer;
using Analysis::CalibrationDataHistogramContainer;
using Analysis::CalibrationDataFunctionContainer;
using Analysis::CalibrationDataVariables;
using Analysis::UncertaintyResult;
using std::string;

void testContainer(const char* fname = "testContainer.root")
{
  // You seem to prefer histograms to behave like all other objects when it
  // comes to object lifetime, i.e. you want to delete them instead of TFile
  // deleting them:
  TH1::AddDirectory(kFALSE);

#ifdef __CINT__
  gSystem->Load("libCalibrationDataInterface.so");
#endif

  // Part 1: creation of (example) scale factor information.

  // calibration result for the MV1 70% working point (w > 0.8119)
  Double_t xBSF[11]   = { 20., 30., 40., 50., 60., 75., 90., 110., 140., 200., 300. };
  Double_t valBSF[10] = { 0.984697, 0.988803, 0.977494, 0.978509, 0.968031, 0.979164, 0.961528, 0.994656, 0.977403, 0.94092 };
  Double_t errBSF[10] = { 0.0154226, 0.0101299, 0.00966202, 0.00827876, 0.0089128, 0.00852312, 0.00788726, 0.00857929, 0.0112111, 0.025609 };
  // the binning for extrapolation histograms _must_ include that for other histograms,
  // but it may extend their range by more (under- or overflow) bins
  Double_t xBSFExtrap[14] = { 20., 30., 40., 50., 60., 75., 90., 110., 140., 200., 300., 500., 800., 1200. };
  Double_t BSFExtrapUnc[13] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.12, 0.33, 0.27 };
  TH1F* hB = new TH1F("b_SF", "MV1 (w>0.8119) b-jet efficiency;pt", 10, xBSF);
  for (int ix = 1; ix < 11; ++ix) {
    hB->SetBinContent(ix, valBSF[ix-1]);
    hB->SetBinError  (ix, errBSF[ix-1]);
  }
  // add an extrapolation uncertainties histogram (this would never be incorporated as part of the total systematics)
  TH1F* hBExtrap = new TH1F("b_SF_extrap", "corresponding extrapolation uncertainty;pt", 13, xBSFExtrap);
  for (int ix = 1; ix < 14; ++ix) {
    hBExtrap->SetBinContent(ix, BSFExtrapUnc[ix-1]);
  }
  // and a dummy 'total systematic uncertainty' histogram
  TH1F* hBSys = new TH1F("b_SF_syst", "MV1 (w>0.8119) b-jet efficiency;pt", 10, xBSF);

  // bin boundaries for the negative-tag measurements (note that this is for an older calibration;
  // but for the purpose of a demonstration, the only relevant point is that the binning for different objects can be different)
  Double_t xMisTag[9] = { 20., 25., 40., 60., 90., 140., 200., 300., 500. };
  Double_t yMisTag[3] = { 0.0, 1.2, 2.5 };
  // calibration result for the SV0 5.85 working point
  Double_t valMisTag[2][8] = { { 1.023, 0.891, 0.935, 0.864, 0.866, 0.757, 0.917, 0.888 },
			       { 1.340, 0.877, 1.200, 0.914, 1.024, 0.984, 0.686, 1.044 } };
  Double_t errMisTag[2][8] = { { 0.206, 0.185, 0.182, 0.196, 0.140, 0.142, 0.126, 0.120 },
			       { 0.450, 0.274, 0.275, 0.287, 0.257, 0.244, 0.114, 0.168 } };
  TH2F* hLight = new TH2F("negtag_SF", "SV0 (w>5.85) mistag rate;pt;abseta", 8, xMisTag, 2, yMisTag);
  for (int ix = 1; ix < 9; ++ix)
    for (int iy = 1; iy < 3; ++iy) {
      hLight->SetBinContent(ix, iy, valMisTag[iy-1][ix-1]);
      hLight->SetBinError  (ix, iy, errMisTag[iy-1][ix-1]);
    }
  TH2F* hLightSys = new TH2F("negtag_SF_syst", "dummy corresponding scale factor systematics;pt;abseta", 8, xMisTag, 2, yMisTag);

  // Part 2: create the actual containers using the above information
  //         notes: - scale factor calibrations should always be stored with object names ending in "_SF"
  //                - here no MC efficiency examples are used. They would follow the same procedure but the object names have to end in "_Eff"

  CalibrationDataHistogramContainer* chSF = new CalibrationDataHistogramContainer("negtag_SF");
  // ensure that no rebinning is performed to these histograms later
  hLight->ResetBit(TH1::kCanRebin);
  hLightSys->ResetBit(TH1::kCanRebin);
  std::cout << "light-jet SF container: about to fill" << std::endl;
  chSF->setResult(hLight);
  std::cout << "light-jet SF container: result filled" << std::endl;
  chSF->setUncertainty("systematics", hLightSys);
  std::cout << "light-jet SF container: systematics filled" << std::endl;
  chSF->setComment("test comment for custom mistag rate scale object");
  // The statistical uncertainties are expected to be stored with the central values.
  // However, for the purpose of "continuous" b-tagging, the covariance matrix corresponding to
  // the statistical uncertainties becomes nontrivial (in particular, non-diagonal).
  // In this case we store the matrix _in addition to the bin-by-bin uncertainties_.
  // The example below fills a dummy covariance matrix just to show the mechanics
  // (the container is anyway not for the storage of a continuous calibration object).
  Int_t nbx = hLight->GetNbinsX()+2, nby = hLight->GetNbinsY()+2;
  TMatrixDSym* sCov = new TMatrixDSym(nbx*nby); // make sure to keep track of the under- and overflow bins
  for (Int_t binx = 1; binx < nbx-1; ++binx)
    for (Int_t biny = 1; biny < nby-1; ++biny) {
      Int_t bin = hLight->GetBin(binx, biny);
      for (Int_t binx2 = 1; binx2 < nbx-1; ++binx2)
	for (Int_t biny2 = 1; biny2 < nby-1; ++biny2) {
	  Int_t bin2 = hLight->GetBin(binx2, biny2);
	  // here it's assumed that the covariance matrix elements have been computed; they would replace the 0 in the following line
	  (*sCov)(bin,bin2) = 0;
	}
    }
  chSF->setUncertainty("statistics", sCov); // the keyword "statistics" is mandatory (when the statistical uncertainties' covariance matrix is concerned)
  chSF->restrictToRange(true);
  std::cout << "light-jet SF container: all done" << std::endl;

  CalibrationDataHistogramContainer* chB = new CalibrationDataHistogramContainer("ttbar_pdf_dijet_SF");
  hB->ResetBit(TH1::kCanRebin);
  hBExtrap->ResetBit(TH1::kCanRebin);
  chB->setResult(hB);
  chB->setUncertainty("systematics", hBSys);
  chB->setUncertainty("extrapolation", hBExtrap); // the keyword "extrapolation" is mandatory for extrapolation uncertainties
  chB->restrictToRange(true);
  
  // test to see what happens if first computation is done here...
  CalibrationDataVariables x1;
  x1.jetAuthor = string("AntiKt4Topo"); // not used if the container objects are accessed directly
  x1.jetPt     = 25000.; // pt in MeV
  x1.jetEta    = 1.1;
  CalibrationDataVariables x2;
  x2.jetAuthor = string("AntiKt4Topo"); // not used if the container objects are accessed directly
  x2.jetPt     = 350000.; // pt in MeV; here choose a value beyond the calibration range 
  x2.jetEta    = 1.1;
  double resTmp; CalibrationDataContainer::CalibrationStatus statTmp = chSF->getResult(x1, resTmp);
  if (statTmp != CalibrationDataContainer::kError) {
    std::cout << "\t chSF result: " << resTmp << ", status = " << statTmp << std::endl;
  } else std::cout << "\t error retrieving chSF result" << std::endl;

  // create a new ROOT file
  TFile* f = TFile::Open(fname, "RECREATE");
  if (f->IsZombie()) {
    std::cout << "error: cannot create file " << fname << std::endl;
    delete f;
    return;
  }

  // addContainer(cfSF, f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "B", "ptrel_SF");
  addContainer(chB,  f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "B");
  addContainer(chB,  f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "B", "default_SF"); // example of saving the container (also) under a different name
  addContainer(chSF, f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "Light");
  addContainer(chSF, f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "Light", "default_SF");
  // close the file, and delete all objects
  f->Close();
  // delete cfSF;
  delete chB;
  delete chSF;

  delete f;

  // Part 3: re-open the ROOT file and retrieve (and inspect) the container objects

  f = TFile::Open(fname);
  std::cout << "after re-opening the file: " << std::endl;
  f->ls();
  // Type-safe version without (slow) dynamic_cast:
  CalibrationDataContainer* cSF = 0;
  f->GetObject("MV1/AntiKt4TopoEMJVF0_5/0_8119/Light/default_SF", cSF);
  CalibrationDataContainer* cb = 0;
  f->GetObject("MV1/AntiKt4TopoEMJVF0_5/0_8119/B/default_SF", cb);
  if (!cSF)  std::cout << " error: null pointer for object cSF" << std::endl;
  if (!cb)  std::cout << " error: null pointer for object cb" << std::endl;

  double res1; CalibrationDataContainer::CalibrationStatus stat = cSF->getResult(x1, res1);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cSF result: " << res1 << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cSF result" << std::endl;
  double statUnc; stat = cSF->getStatUncertainty(x1,statUnc);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cSF stat unc: " << statUnc << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cSF stat unc" << std::endl;
  UncertaintyResult systUnc; stat = cSF->getSystUncertainty(x1,systUnc);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cSF syst unc: (+)" << systUnc.first << " (-)" << systUnc.second << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cSF syst unc" << std::endl;
  std::cout << "\t cSF comment: " << cSF->getComment() << std::endl;
  for (unsigned int i = 0; i < 3; ++i) {
    std::cout << "\tboundaries for variable " << i << ": (-)" << cSF->getLowerBound(i)
	      << " (+)" << cSF->getUpperBound(i) << std::endl;
  }
  CalibrationDataHistogramContainer* ch2SF = dynamic_cast<CalibrationDataHistogramContainer*>(cSF);
  if (ch2SF) {
    std::vector<double> bins = ch2SF->getBinBoundaries(CalibrationDataContainer::kPt);
    if (bins.size() > 0) {
      std::cout << "\tpt bin boundaries:"; for (unsigned int i = 0; i < bins.size(); ++i) std::cout << " " << bins[i]; std::cout << std::endl;
    }
    bins = ch2SF->getBinBoundaries(CalibrationDataContainer::kAbsEta);
    if (bins.size() > 0) {
      std::cout << "\t|eta| bin boundaries:"; for (unsigned int i = 0; i < bins.size(); ++i) std::cout << " " << bins[i]; std::cout << std::endl;
    }
  }

  // test the b-jet SF calibration object with the out-of-bounds pt value
  std::cout << "\t cb testing with out-of-bounds pt value (should return status value = "
	    << CalibrationDataContainer::kRange << ")" << std::endl;
  double resb; stat = cb->getResult(x2, resb);
  if (stat !=  CalibrationDataContainer::kError) {
    std::cout << "\t cb result: " << resb << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cb result" << std::endl;
  double statb; stat = cb->getStatUncertainty(x2,statb);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cb stat unc: " << statb << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cb stat unc" << std::endl;
  UncertaintyResult systb; stat = cb->getSystUncertainty(x2,systb);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cb syst unc: (+)" << systb.first << " (-)" << systb.second << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cb syst unc" << std::endl;
  for (unsigned int i = 0; i < 3; ++i) {
    std::cout << "\tboundaries for variable " << i << ": (-)" << cb->getLowerBound(i)
	      << " (+)" << cb->getUpperBound(i) << std::endl;
  }
  CalibrationDataHistogramContainer* chb = dynamic_cast<CalibrationDataHistogramContainer*>(cb);
  if (chb) {
    std::vector<double> bins = chb->getBinBoundaries(CalibrationDataContainer::kPt);
    if (bins.size() > 0) {
      std::cout << "\tpt bin boundaries:"; for (unsigned int i = 0; i < bins.size(); ++i) std::cout << " " << bins[i]; std::cout << std::endl;
    }
    bins = chb->getBinBoundaries(CalibrationDataContainer::kAbsEta);
    if (bins.size() > 0) {
      std::cout << "\t|eta| bin boundaries:"; for (unsigned int i = 0; i < bins.size(); ++i) std::cout << " " << bins[i]; std::cout << std::endl;
    }
  }
  UncertaintyResult extrap1; stat = cb->getUncertainty("extrapolation", x1, extrap1);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cb extrap unc (should be 0): " << extrap1.first << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cb extrap unc" << std::endl;
  UncertaintyResult extrap2; stat = cb->getUncertainty("extrapolation", x2, extrap2);
  if (stat != CalibrationDataContainer::kError) {
    std::cout << "\t cb extrap unc (should be > 0): " << extrap2.first << ", status = " << stat << std::endl;
  } else std::cout << "\t error retrieving cb extrap unc" << std::endl;

  delete cSF;
  delete cb;
  delete f;
}

