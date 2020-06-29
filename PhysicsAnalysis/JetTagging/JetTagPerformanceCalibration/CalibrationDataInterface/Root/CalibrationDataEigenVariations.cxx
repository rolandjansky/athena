/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// CalibrationDataEigenVariations.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "CalibrationDataInterface/CalibrationDataEigenVariations.h"
#include "CalibrationDataInterface/CalibrationDataInternals.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <set>
#include <algorithm>
#include <string>
#include <cmath>

#include "TH1.h"
#include "TVectorT.h"
#include "TDecompSVD.h"
#include "TMatrixDSymEigen.h"
#include "TROOT.h"
#include "TFile.h"

using Analysis::CalibrationDataEigenVariations;
using Analysis::CalibrationDataInterface::split;
using std::string;
using std::map;
using std::vector;
using std::set;
using std::pair;
using std::setw;
using std::setprecision;

namespace {
  // The covariance matrices constructed by the following two functions adhere to the TH1 binning conventions.
  // But in order to avoid unnecessary overhead, the actual dimensionality of the histograms is accounted for.

  // Construct the (diagonal) covariance matrix for the statistical uncertainties on the "ref" results
  // Note that when statistical uncertainties are stored as variations, they are already accounted for and hence should not be duplicated; hence they are dummy here
  // (this is not very nice, in that the result of this function will technically be wrong,
  // but the statistical uncertainty covariance matrix is anyway not separately visible to the outside world)
  TMatrixDSym getStatCovarianceMatrix(const TH1* hist, bool zero) {
    Int_t nbinx = hist->GetNbinsX()+2, nbiny = hist->GetNbinsY()+2, nbinz = hist->GetNbinsZ()+2;
    Int_t rows = nbinx;
    if (hist->GetDimension() > 1) rows *= nbiny;
    if (hist->GetDimension() > 2) rows *= nbinz;

    // // the "2" below doesn't actually imply that two bins are used...
    // // this is just to make the loops below work
    // if (ref->GetDimension() <= 1) nbiny = 2;
    // if (ref->GetDimension() <= 2) nbinz = 2;

    TMatrixDSym stat(rows);
    for (Int_t binx = 1; binx < nbinx-1; ++binx)
      for (Int_t biny = 1; biny < nbiny-1; ++biny)
	for (Int_t binz = 1; binz < nbinz-1; ++binz) {
	  Int_t bin = hist->GetBin(binx, biny, binz);
	  double err = zero ? 0 : hist->GetBinError(bin);
	  stat(bin, bin) = err*err;
	}
    return stat;
  }

  // Construct the covariance matrix assuming that histogram "unc" contains systematic uncertainties
  // pertaining to the "ref" results, and that the uncertainties are fully correlated from bin to bin
  // (unless option "doCorrelated" is false, in which bins are assumed uncorrelated).
  // One exception is made for the uncorrelated case: if we are dealing with a "continuous" calibration
  // object, then a full correlation is still assumed between different tag weight bins.
  TMatrixDSym getSystCovarianceMatrix(const TH1* ref, const TH1* unc,
				      bool doCorrelated, int tagWeightAxis) {
    Int_t nbinx = ref->GetNbinsX()+2, nbiny = ref->GetNbinsY()+2, nbinz = ref->GetNbinsZ()+2;
    Int_t rows = nbinx;
    if (ref->GetDimension() > 1) rows *= nbiny;
    if (ref->GetDimension() > 2) rows *= nbinz;

    TMatrixDSym cov(rows);

    // Carry out a minimal consistency check
    if (unc->GetNbinsX()+2 != nbinx || unc->GetNbinsY()+2 != nbiny || unc->GetNbinsZ()+2 != nbinz
	|| unc->GetDimension() != ref->GetDimension()) {
      std::cout << "getSystCovarianceMatrix: inconsistency found in histograms "
		<< ref->GetName() << " and " << unc->GetName() << std::endl;
      return cov;
    }

    // // the "2" below doesn't actually imply that two bins are used...
    // // this is just to make the loops below work
    // if (ref->GetDimension() <= 1) nbiny = 2;
    // if (ref->GetDimension() <= 2) nbinz = 2;

    // Special case: uncertainties not correlated from bin to bin.
    // The exception here is for tag weight bins, which are always assumed to be fully correlated.
    if (! doCorrelated) {
      if (tagWeightAxis < 0) {
	// truly uncorrelated uncertainties
	for (Int_t binx = 1; binx < nbinx-1; ++binx)
	  for (Int_t biny = 1; biny < nbiny-1; ++biny)
	    for (Int_t binz = 1; binz < nbinz-1; ++binz) {
	      Int_t bin = ref->GetBin(binx, biny, binz);
	      double err = unc->GetBinContent(bin);
	      cov(bin,bin) = err*err;
	    }
	return cov;
      } else if (tagWeightAxis == 0) {
	// continuous histogram with tag weight X axis
	for (Int_t biny = 1; biny < nbiny-1; ++biny)
	  for (Int_t binz = 1; binz < nbinz-1; ++binz)
	    for (Int_t binx = 1; binx < nbinx-1; ++binx) {
	      Int_t bin = ref->GetBin(binx, biny, binz);
	      double err = unc->GetBinContent(bin);
	      for (Int_t binx2 = 1; binx2 < nbinx-1; ++binx2) {
		Int_t bin2 = ref->GetBin(binx2, biny, binz);
		double err2 = unc->GetBinContent(bin2);
		cov(bin,bin2) = err*err2;
	      }
	    }
	return cov;
      } else if (tagWeightAxis == 1) {
	// continuous histogram with tag weight Y axis
	for (Int_t binx = 1; binx < nbinx-1; ++binx)
	  for (Int_t binz = 1; binz < nbinz-1; ++binz)
	    for (Int_t biny = 1; biny < nbiny-1; ++biny) {
	      Int_t bin = ref->GetBin(binx, biny, binz);
	      double err = unc->GetBinContent(bin);
	      for (Int_t biny2 = 1; biny2 < nbiny-1; ++biny2) {
		Int_t bin2 = ref->GetBin(binx, biny2, binz);
		double err2 = unc->GetBinContent(bin2);
		cov(bin,bin2) = err*err2;
	      }
	    }
	return cov;
      } else if (tagWeightAxis == 2) {
	// continuous histogram with tag weight Z axis
	for (Int_t binx = 1; binx < nbinx-1; ++binx)
	  for (Int_t biny = 1; biny < nbiny-1; ++biny)
	    for (Int_t binz = 1; binz < nbinz-1; ++binz) {
	      Int_t bin = ref->GetBin(binx, biny, binz);
	      double err = unc->GetBinContent(bin);
	      for (Int_t binz2 = 1; binz2 < nbinz-1; ++binz2) {
		Int_t bin2 = ref->GetBin(binx, biny, binz2);
		double err2 = unc->GetBinContent(bin2);
		cov(bin,bin2) = err*err2;
	      }
	    }
	return cov;
      }
    }

    for (Int_t binx = 1; binx < nbinx-1; ++binx)
      for (Int_t biny = 1; biny < nbiny-1; ++biny)
	for (Int_t binz = 1; binz < nbinz-1; ++binz) {
	  Int_t bin = ref->GetBin(binx, biny, binz);
	  double err = unc->GetBinContent(bin);
	  for (Int_t binx2 = 1; binx2 < nbinx-1; ++binx2)
	    for (Int_t biny2 = 1; biny2 < nbiny-1; ++biny2)
	      for (Int_t binz2 = 1; binz2 < nbinz-1; ++binz2) {
		Int_t bin2 = ref->GetBin(binx2, biny2, binz2);
		double err2 = unc->GetBinContent(bin2);
		cov(bin, bin2) = err*err2;
	      }
	}
    return cov;
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataEigenVariations                                                           //
//                                                                                          //
// This class is intended to provide a more proper way to deal with correlations between    //
// calibration bins than would be possible using directly the calibration containers'       //
// methods (the general issue is with the fact that the interface methods are intended to   //
// be use on a jet by jet basis; in this context it is not straightforward to account for   //
// correlations).                                                                           //
//                                                                                          //
// A CalibrationDataEigenVariations object is associated with a specific                    //
// CalibrationDataHistogramContainer. It starts by constructing the covariance matrix from  //
// the information provided by the container. Subsequently it diagonalises this covariance  //
// matrix (this is a standard eigenvalue problem, hence the name of the class), and stores  //
// the result as 'variation' histograms (representing the eigenvectors multiplied by the    //
// square root of the corresponding eigenvalues).                                           //
// Since it is possible for systematic uncertainty contributions to be correlated with      //
// corresponding uncertainties in physics analyses, it is possible to exclude such sources  //
// of uncertainty from being used in the construction of the covariance matrix (this is     //
// since effects from the original sources of uncertainty cannot be traced anymore after    //
// application of the eigenvalue decomposition). It is still possible to evaluate correctly //
// these uncertainties in the form of so-called 'named variations' (see class               //
// CalibrationDataInterfaceROOT); however this will always treat uncertainties as being     //
// fully correlated (or anti-correlated) between calibration bins, so it is recommended not //
// to exclude uncertainties that are not correlated between bins from the eigenvector       //
// decomposition.                                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataEigenVariations)
#endif

//________________________________________________________________________________
CalibrationDataEigenVariations::CalibrationDataEigenVariations(const CalibrationDataHistogramContainer* cnt,
							       bool excludeRecommendedUncertaintySet) :
m_cnt(cnt), m_initialized(false), m_namedExtrapolation(-1), m_statVariations(false)
{
  // if specified, add items recommended for exclusion from EV decomposition by the calibration group to the 'named uncertainties' list
  if (excludeRecommendedUncertaintySet) {
    std::vector<std::string> to_exclude = split(m_cnt->getExcludedUncertainties());
    for (auto name : to_exclude) excludeNamedUncertainty(name);
    if (to_exclude.size() == 0) {
      std::cerr << "CalibrationDataEigenVariations warning: exclusion of pre-set uncertainties list requested but no (or empty) list found" << std::endl;
    }
  }
  // also flag if statistical uncertainties stored as variations (this typically happens as a result of smoothing / pruning of SF results)
  vector<string> uncs = m_cnt->listUncertainties();
  for (auto name : uncs) {
    if (name.find("stat_np") != string::npos) m_statVariations = true;
  }
}

//________________________________________________________________________________
CalibrationDataEigenVariations::~CalibrationDataEigenVariations()
{
  // delete all variation histograms owned by us
  for (vector<pair<TH1*, TH1*> >::iterator it = m_named.begin();
       it != m_named.end(); ++it) {
    delete it->first;
    delete it->second;
  }
  std::cout << "eigenvariations destructor: removing " << m_eigen.size() << " eigenvariations" << std::endl;
  for (vector<pair<TH1*, TH1*> >::iterator it = m_eigen.begin();
       it != m_eigen.end(); ++it) {
    delete it->first;
    delete it->second;
  }
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::excludeNamedUncertainty(const std::string& name)
{
  // Exclude the source of uncertainty identified by the given name from being used
  // in the construction of the covariance matrix to be diagonalised.
  // Notes:
  // - Some names returned by CalibrationDataContainer::listUncertainties() are not
  //   meaningful in this context, and specifying them is not allowed.
  // - Once the eigenvector diagonalisation has been carried out, this method may
  //   not be used anymore.

  if (m_initialized)
    std::cerr << "CalibrationDataEigenVariations::excludeNamedUncertainty error:"
	      << " initialization already done" << std::endl;
  else if (name == "comment"    || name == "result"   || name == "systematics" ||
	   name == "statistics" || name == "combined" || name == "extrapolation" ||
	   name == "MCreference" || name == "MChadronisation" || name == "ReducedSets" || name == "excluded_set")
    std::cerr << "CalibrationDataEigenVariations::excludeNamedUncertainty error:"
	      << " name " << name << " not allowed" << std::endl;
  else if (! m_cnt->GetValue(name.c_str()))
    std::cerr << "CalibrationDataEigenVariations::excludeNamedUncertainty error:"
	      << " uncertainty named " << name << " not found" << std::endl;
  // only really add if the entry is not yet in the list
  else if (m_namedIndices.find(name) == m_namedIndices.end()) {
    m_named.push_back(std::pair<TH1*, TH1*>(0, 0));
    m_namedIndices[name] = m_named.size()-1;
  }
}

//________________________________________________________________________________
TMatrixDSym
CalibrationDataEigenVariations::getEigenCovarianceMatrix() const
{
  // Construct the covariance matrix that is to be diagonalised.
  // Note that extrapolation uncertainties (identified by the keyword "extrapolation,
  // this will pertain mostly to the extrapolation to high jet pt) are always excluded
  // since by definition they will not apply to the normal calibration bins. Instead
  // this uncertainty has to be dealt with as a named variation. In addition there are
  // other items ("combined", "systematics") that will not be dealt with correctly
  // either and hence are excluded as well).
  //
  // Note that if an explicit covariance matrix is supplied (at present this may be
  // the case only for statistical uncertainties: in the case of "continuous tagging",
  // multinomial statistics applies so bin-to-bin correlations exist) this will be
  // used instead of constructing the statistical uncertainties' covariance matrix on
  // the fly.

  // retrieve the central calibration
  TH1* result = dynamic_cast<TH1*>(m_cnt->GetValue("result"));

  // loop over the uncertainties to construct the covariance matrix for all uncertainties
  // to be addressed using the eigenvector method.

  // First, treat the statistics separately.
  // Account for the possibility that this is handled as a (non-trivial) covariance matrix
  TMatrixDSym* sCov = dynamic_cast<TMatrixDSym*>(m_cnt->GetValue("statistics"));
  // Alternatively, statistical uncertainties may be accounted for as variations (i.e., much like systematic uncertainties).
  // In that case, we create a null matrix here and add the statistical contributions along with the systematic ones.
  TMatrixDSym cov = (sCov) ? *sCov : getStatCovarianceMatrix(result, m_statVariations);

  // Then loop through the list of (other) uncertainties
  std::vector<string> uncs = m_cnt->listUncertainties();
  for (unsigned int t = 0; t < uncs.size(); ++t) {
    // entries that should never be included
    if (uncs[t] == "comment" || uncs[t] == "result" ||
	uncs[t] == "combined" || uncs[t] == "statistics" ||
        uncs[t] == "systematics" || uncs[t] == "MCreference" ||
        uncs[t] == "MChadronisation" || uncs[t] == "extrapolation" ||
        uncs[t] == "ReducedSets" || uncs[t] == "excluded_set") continue;
    // entries that can be excluded if desired
    if (m_namedIndices.find(uncs[t]) != m_namedIndices.end()) continue;

    TH1* hunc = dynamic_cast<TH1*>(m_cnt->GetValue(uncs[t].c_str()));
    cov += getSystCovarianceMatrix(result, hunc, m_cnt->isBinCorrelated(uncs[t]), m_cnt->getTagWeightAxis());
  }

  return cov;
}

//________________________________________________________________________________
TMatrixDSym
CalibrationDataEigenVariations::getEigenCovarianceMatrixFromVariations() const
{
  // Construct the (Eigen-variation part of the) covariance matrix from the individual variations.
  // This must be called _after_ initialize()!

  // retrieve the central calibration
  TH1        *result = dynamic_cast<TH1*>(m_cnt->GetValue("result"));
  TMatrixD    jac = getJacobianReductionMatrix();
  int         nbins = jac.GetNcols();
  TMatrixDSym cov(nbins);
  auto variation = std::make_unique<double[]>(nbins);

  for (std::vector<std::pair<TH1*, TH1*> >::const_iterator it = m_eigen.begin();
       it != m_eigen.end(); ++it) {
    TH1* resultVariedUp = it->first;
    for (unsigned int u = 0; u < (unsigned int) nbins; ++u)
      variation[u] = resultVariedUp->GetBinContent(u) - result->GetBinContent(u);
    for (int u = 0; u < nbins; ++u)
      for (int v = 0; v < nbins; ++v)
        cov(u, v) += variation[u]*variation[v];
  }

  return cov;
}

//________________________________________________________________________________
TMatrixD
CalibrationDataEigenVariations::getJacobianReductionMatrix() const
{
  // Construct the matrix that removes the rows and columns that fail to influence
  // the eigen-variations. To reduce the covariance matrix, do the following:
  // 
  // TMatrixDSym cov = getEigenCovarianceMatrix();
  // TMatrixDSym jac = getJacobianReductionMatrix();
  // TMatrixDSym redSystematicCovMatrix = cov.Similarity(jac);

  // retrieve the central calibration
  TH1* result = dynamic_cast<TH1*>(m_cnt->GetValue("result"));

  // loop over the uncertainties to construct the covariance matrix for all uncertainties
  // to be addressed using the eigenvector method.

  // Retrieve the un-compressed Eigenvector variation covariance matrix
  // (only needed to check for unexpected singularities)
  TMatrixDSym cov = getEigenCovarianceMatrix();

  // Compute the original number of bins
  int nbins = result->GetNbinsX()+2;
  int ndim  = result->GetDimension();
  if (ndim > 1) nbins*= (result->GetNbinsY()+2);
  if (ndim > 2) nbins*= (result->GetNbinsZ()+2);

  // Start by "compressing" the covariance matrix (removing columns/rows containing zeros only)
  int nZeros=0;
  std::vector<int> zeroComponents;
  if (cov.GetNrows() != nbins) {
    std::cerr << " error: covariance matrix size (" << cov.GetNrows() << ") doesn't match histogram size (" << nbins << ")" << std::endl;
    return TMatrixDSym();
  }

  // First flag all the zeros
  for (int i = 0; i < nbins; ++i) {
    // Directly identify the under- and overflow bins
    Int_t binx, biny, binz;
    result->GetBinXYZ(i, binx, biny, binz);
    if ((binx == 0 || binx == result->GetNbinsX()+1) ||
        (ndim > 1 && (biny == 0 || biny == result->GetNbinsY()+1)) ||
        (ndim > 2 && (binz == 0 || binz == result->GetNbinsZ()+1))) {
      ++nZeros;
      zeroComponents.push_back(i);
      // std::cout << "flagging bin " << i << " as under- or overflow" << std::endl;
    }
    // Try a first (quick) identification of rows/columns of zeros by the first element in each row
    // If "successful", check the whole row in more detail
    else if (fabs(cov(i,0)) < 1e-10) {
      bool isThereANonZero(false);
      for (int j = 0; j < nbins; ++j) {
        if (fabs(cov(i,j)) > 1e-10) {
          isThereANonZero=true; break;
        }
      }
      if (! isThereANonZero) {
        ++nZeros;
        zeroComponents.push_back(i);
      }
    }
  }

  // Determine whether the container is for "continuous" calibration.
  // This is important since the number of independent scale factors (for each pt or eta bin)
  // is reduced by 1 compared to the number of tag weight bins (related to the fact that the fractions
  // of events in tag weight bins have to sum up to unity).
  // int axis = m_cnt->getTagWeightAxis();
  // bool doContinuous = false;  unsigned int weightAxis = 0;

  // if (axis >= 0) {
  //   doContinuous = true;
  //   // weightAxis = (unsigned int) axis;
  //   // In this case, verify that the special "uncertainty" entry that is in fact the reference MC tag
  //   // weight fractions is present. These tag weight fractions are needed in order to carry out the
  //   // diagonalisation successfully.
  //   if (! dynamic_cast<TH1*>(m_cnt->GetValue("MCreference"))) {
  //     std::cerr << " Problem: continuous calibration object found without MC reference tag weight histogram " << std::endl;
  //     return TMatrixDSym();
  //   }
  // }

  // Only relevant for continuous calibration containers, but in order to void re-computation we
  // retrieve them here
  // Int_t nbinx = result->GetNbinsX()+2, nbiny = result->GetNbinsY()+2, nbinz = result->GetNbinsZ()+2;

  // // If we are indeed dealing with a "continuous" calibration container, ignore one tag weight row
  // const int skipTagWeightBin = 1; // NB this follows the histogram's bin numbering
  // if (doContinuous) {
  //   for (Int_t binx = 1; binx < nbinx-1; ++binx)
  //     for (Int_t biny = 1; biny < nbiny-1; ++biny)
  //       for (Int_t binz = 1; binz < nbinz-1; ++binz) {
  //         if ((weightAxis == 0 && binx == skipTagWeightBin) ||
  //             (weightAxis == 1 && biny == skipTagWeightBin) ||
  //             (weightAxis == 2 && binz == skipTagWeightBin)) {
  //           // At this point we simply add these to the 'null' elements
  //           ++nZeros;
  //           zeroComponents.push_back(result->GetBin(binx, biny, binz));
  //         }
  //       }
  // }

  if (nZeros >= nbins) {
    std::cerr << " Problem. Found n. " << nZeros << " while size of matrix is " << nbins << std::endl;
    return TMatrixDSym();
  }

  int size = nbins - nZeros;

  TMatrixT<double> matrixVariationJacobian(size,nbins);
  int nMissed=0;
  for (int i = 0; i < nbins; ++i) { //full size
    bool missed=false;
    for (unsigned int s = 0; s < zeroComponents.size(); ++s) {
      if (zeroComponents.at(s) == i) {
        missed = true;
        break;
      }
    }
    if (missed) {
      ++nMissed;
      continue;
    }
    matrixVariationJacobian(i-nMissed,i)=1;
  }

  return matrixVariationJacobian;
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::initialize(double min_variance)
{
  // This is this class's most important method, in the sense that it does all the
  // math and constructs all "variation" histograms (for both eigenvector and named
  // named variations). This constitutes the full initialisation of the class.
  // This method is meant to be called only after all calls (if any) to the
  // CalibrationDataEigenVariations::excludeNamedUncertainty() method.

  // retrieve the central calibration
  TH1* result = dynamic_cast<TH1*>(m_cnt->GetValue("result"));

  // First step: construct the original covariance matrix
  TMatrixDSym cov = getEigenCovarianceMatrix();

  // Second step: create the variations for the named sources of uncertainty (easy...)
  for (map<string, unsigned int>::iterator it = m_namedIndices.begin();
       it != m_namedIndices.end(); ++it) {
    pair<TH1*, TH1*>& p = m_named[it->second];
    TH1* hunc = (TH1*) m_cnt->GetValue(it->first.c_str());
    TString namedvar("namedVar");
    namedvar += it->first.c_str();
    TString namedvarUp(namedvar);   namedvarUp   += "_up";
    TString namedvarDown(namedvar); namedvarDown += "_down";
    TH1* resultVariedUp   = (TH1*)result->Clone(namedvarUp);   resultVariedUp->Add(hunc, 1.0);    resultVariedUp->SetDirectory(0);
    TH1* resultVariedDown = (TH1*)result->Clone(namedvarDown); resultVariedDown->Add(hunc, -1.0); resultVariedDown->SetDirectory(0);
    p.first  = resultVariedUp;
    p.second = resultVariedDown;
  }
  // Refinement: add the "extrapolation" uncertainty as a named uncertainty, if the histogram is provided
  // This is a bit special, since the extrapolation uncertainty histogram has a different size than other histograms
  if (TH1* hunc = (TH1*) m_cnt->GetValue("extrapolation")) {
    TH1* resultVariedUp   = (TH1*) hunc->Clone("extrapolation_up");   resultVariedUp->SetDirectory(0);
    TH1* resultVariedDown = (TH1*) hunc->Clone("extrapolation_down"); resultVariedDown->SetDirectory(0);
    Int_t nbinx = hunc->GetNbinsX()+2, nbiny = hunc->GetNbinsY()+2, nbinz = hunc->GetNbinsZ()+2;
    Int_t firstbinx = hunc->GetXaxis()->FindFixBin(result->GetXaxis()->GetBinCenter(1));
    Int_t firstbiny = result->GetDimension() > 1 ? hunc->GetYaxis()->FindFixBin(result->GetYaxis()->GetBinCenter(1)) : 1;
    Int_t firstbinz = result->GetDimension() > 2 ? hunc->GetZaxis()->FindFixBin(result->GetZaxis()->GetBinCenter(1)) : 1;
    for (Int_t binx = 1; binx < nbinx-1; ++binx) {
      Int_t binxResult = binx - firstbinx + 1;
      if (binxResult < 1) binxResult = 1;
      if (binxResult > result->GetNbinsX()) binxResult = result->GetNbinsX();
      for (Int_t biny = 1; biny < nbiny-1; ++biny) {
	Int_t binyResult = biny - firstbiny + 1;
	if (binyResult > result->GetNbinsY()) binyResult = result->GetNbinsY();
	for (Int_t binz = 1; binz < nbinz-1; ++binz) {
	  Int_t binzResult = binz - firstbinz + 1;
	  if (binzResult > result->GetNbinsZ()) binzResult = result->GetNbinsZ();
	  Int_t bin = hunc->GetBin(binx, biny, binz);
	  double contResult = result->GetBinContent(binxResult, binyResult, binzResult);
	  resultVariedUp->SetBinContent(bin, contResult + hunc->GetBinContent(bin));
	  resultVariedDown->SetBinContent(bin, contResult - hunc->GetBinError(bin));
	}
      }
    }
    m_named.push_back(std::make_pair(resultVariedUp, resultVariedDown));
    m_namedExtrapolation = m_namedIndices["extrapolation"] = m_named.size()-1;
  }

  // Third step: compute the eigenvector variations corresponding to the remaining sources of uncertainty
  int nbins = result->GetNbinsX()+2;
  int ndim  = result->GetDimension();
  if (ndim > 1) nbins*= (result->GetNbinsY()+2);
  if (ndim > 2) nbins*= (result->GetNbinsZ()+2);

  // // Determine whether the container is for "continuous" calibration.
  // // This is important since the number of independent scale factors (for each pt or eta bin)
  // // is reduced by 1 compared to the number of tag weight bins (related to the fact that the fractions
  // // of events in tag weight bins have to sum up to unity).
  int axis = m_cnt->getTagWeightAxis();
  // bool doContinuous = false; unsigned int weightAxis = 0;

  if (axis >= 0) {
  //   doContinuous = true;
  //   weightAxis = (unsigned int) axis;
  //   // In this case, verify that the special "uncertainty" entry that is in fact the reference MC tag
  //   // weight fractions is present. These tag weight fractions are needed in order to carry out the
  //   // diagonalisation successfully.
    if (! dynamic_cast<TH1*>(m_cnt->GetValue("MCreference"))) {
      std::cerr << " Problem: continuous calibration object found without MC reference tag weight histogram " << std::endl;
      return;
    }
  }

  // Only relevant for continuous calibration containers, but in order to void re-computation we
  // retrieve them here
  // Int_t nbinx = result->GetNbinsX()+2, nbiny = result->GetNbinsY()+2, nbinz = result->GetNbinsZ()+2;

  TMatrixT<double> matrixVariationJacobian = getJacobianReductionMatrix();
  int size = matrixVariationJacobian.GetNrows();

  // Reduce the matrix to one without the zeros, using a "similarity" transformation
  const TMatrixDSym matrixCovariance = cov.Similarity(matrixVariationJacobian);

  // Carry out the Eigenvector decomposition on this matrix
  TMatrixDSymEigen eigenValueMaker (matrixCovariance);
  TVectorT<double> eigenValues   = eigenValueMaker.GetEigenValues();
  TMatrixT<double> eigenVectors  = eigenValueMaker.GetEigenVectors();
  TMatrixT<double> matrixVariations (size,size);

  //TDecompSVD       covSVD (matrixCovariance, 1.0E-3);
  //TVectorD         sSVD = covSVD.GetSig();

  //std::cout << sSVD.GetNrows() << std::endl;
  //for (int i = 0; i < sSVD.GetNrows(); ++i)
  //  std::cout << sSVD(i) << std::endl;

  for (int i = 0; i < size; ++i) {
    //now go back
    for (int r = 0; r < size; ++r)
      //first index is the variation number, second corresponds to the pT bin
      matrixVariations(i,r) = -1.0*eigenVectors[r][i]*sqrt(fabs(eigenValues[i]));
  }

  TMatrixT<double> matrixVariationsWithZeros = matrixVariations * matrixVariationJacobian;

  // //now setup the real ucertainties
  // std::vector<TObject*> eigenvectorsUncUpProvider;
  // std::vector<TObject*> eigenvectorsUncDownProvider; 
  // std::vector<TObject*> eigenvectorsUncProvider;

  // TH1* MC = (doContinuous) ? dynamic_cast<TH1*>(m_cnt->GetValue("MCreference")) : 0;
  // // first carry out basic cross-checks: data and MC fractions summed over tag weight bins must always be 1
  // if (doContinuous) {
  //   if (weightAxis == 2) {
  //     for (Int_t binx = 1; binx < nbinx-1; ++binx)
  // 	for (Int_t biny = 1; biny < nbiny-1; ++biny) {
  // 	  double effMC = 0, effData = 0;
  // 	  for (Int_t binz = 1; binz < nbinz-1; ++binz) {
  // 	    Int_t bin = result->GetBin(binx, biny, binz);
  // 	    effData += result->GetBinContent(bin) * MC->GetBinContent(bin);
  // 	    effMC   +=                              MC->GetBinContent(bin);
  // 	  }
  // 	  std::cout << "\t(x, y) = (" << setw(3) << binx << "," << setw(3) << biny
  // 		    << ") summed MC: " << setw(6) << effMC << ", summed data: " << setw(6) << effData << std::endl;
  // 	}
  //   } else if (weightAxis == 1) {
  //     for (Int_t binx = 1; binx < nbinx-1; ++binx)
  // 	for (Int_t binz = 1; binz < nbinz-1; ++binz) {
  // 	  double effMC = 0, effData = 0;
  // 	  for (Int_t biny = 1; biny < nbiny-1; ++biny) {
  // 	    Int_t bin = result->GetBin(binx, biny, binz);
  // 	    effData += result->GetBinContent(bin) * MC->GetBinContent(bin);
  // 	    effMC   +=                              MC->GetBinContent(bin);
  // 	    // std::cout << "\tcnt = (" << setw(3) << binx << "," << setw(3) << biny << "," << setw(3) << binz
  // 	    // 		<< ") MC: " << setw(10) << MC->GetBinContent(bin) << " SF: (nom = " << setw(8) << result->GetBinContent(bin)
  // 	    // 		<< ", up = " << setw(8) << resultVariedUp->GetBinContent(bin) << "), up(cumulative): "
  // 	    // 		<< setw(8) << effUp << ", nom(cumulative): " << setw(8) << effNom << std::endl;
  // 	  }
  // 	  std::cout << "\t(x, z) = (" << setw(3) << binx << "," << setw(3) << binz
  // 		    << ") summed MC: " << setw(6) << effMC << ", summed data: " << setw(6) << effData << std::endl;
  // 	}
  //   } else {
  //     for (Int_t biny = 1; biny < nbiny-1; ++biny)
  // 	for (Int_t binz = 1; binz < nbinz-1; ++binz) {
  // 	  double effMC = 0, effData = 0;
  // 	  for (Int_t binx = 1; binx < nbinx-1; ++binx) {
  // 	    Int_t bin = result->GetBin(binx, biny, binz);
  // 	    effData += result->GetBinContent(bin) * MC->GetBinContent(bin);
  // 	    effMC   +=                              MC->GetBinContent(bin);
  // 	  }
  // 	  std::cout << "\t(y, z) = (" << setw(3) << biny << "," << setw(3) << binz
  // 		    << ") summed MC: " << setw(6) << effMC << ", summed data: " << setw(6) << effData << std::endl;
  // 	}
  //   }
  // }

  // Construct the initial set of variations from this
  for (int i = 0; i < matrixVariationsWithZeros.GetNrows(); ++i) {
    // TString superstring(result->GetName());
    // superstring += "_eigenVar";
    TString superstring("eigenVar");
    superstring+=i;

    TString nameUp(superstring);   nameUp   += "_up";
    TString nameDown(superstring); nameDown += "_down";
    // TString nameUnc(superstring);  nameUnc+= "_unc";

    TH1* resultVariedUp   = (TH1*)result->Clone(nameUp);   resultVariedUp->SetDirectory(0);
    TH1* resultVariedDown = (TH1*)result->Clone(nameDown); resultVariedDown->SetDirectory(0);

    for (int u = 0; u < nbins; ++u) {
      resultVariedUp->SetBinContent(u,result->GetBinContent(u)+
				    matrixVariationsWithZeros(i,u));
      resultVariedDown->SetBinContent(u,result->GetBinContent(u)-
				      matrixVariationsWithZeros(i,u));
    }

    // // The "continuous" case is special, since we need to re-insert the variations for the tag weight bins that were
    // // removed to make the covariance matrix non-singular
    // if (doContinuous) {
    //   // std::cout << "assuming a continuous calibration object, weight axis = " << weightAxis << std::endl;

    //   // Ugly code duplication since nfortunately there doesn't seem to be a compact way to go
    //   // through the following manipulations without a priori knowledge of the histogram axes!
    //   if (weightAxis == 2) {
    // 	for (Int_t binx = 1; binx < nbinx-1; ++binx)
    // 	  for (Int_t biny = 1; biny < nbiny-1; ++biny) {
    // 	    double effUp = 1, effDown = 1;
    // 	    for (Int_t binz = 1; binz < nbinz-1; ++binz) {
    // 	      if (binz == skipTagWeightBin) continue;
    // 	      Int_t bin = result->GetBin(binx, biny, binz);
    // 	      effUp   -= resultVariedUp->GetBinContent(bin)   * MC->GetBinContent(bin);
    // 	      effDown -= resultVariedDown->GetBinContent(bin) * MC->GetBinContent(bin);
    // 	    }
    // 	    // What remains is the "data" tag weight fraction in the skipped bin.
    // 	    // Divide it by its corresponding MC tag weight fraction to obtain the varied scale factor
    // 	    Int_t bin = result->GetBin(binx, biny, skipTagWeightBin);
    // 	    resultVariedUp->SetBinContent(bin,  effUp/MC->GetBinContent(bin));
    // 	    resultVariedDown->SetBinContent(bin,effDown/MC->GetBinContent(bin));
    // 	  }
    //   } else if (weightAxis == 1) {
    // 	for (Int_t binx = 1; binx < nbinx-1; ++binx)
    // 	  for (Int_t binz = 1; binz < nbinz-1; ++binz) {
    // 	    double effUp = 1, effDown = 1; //, effNom = 1;
    // 	    for (Int_t biny = 1; biny < nbiny-1; ++biny) {
    // 	      if (biny == skipTagWeightBin) continue;
    // 	      Int_t bin = result->GetBin(binx, biny, binz);
    // 	      // effNom  -= result->GetBinContent(bin)           * MC->GetBinContent(bin);
    // 	      effUp   -= resultVariedUp->GetBinContent(bin)   * MC->GetBinContent(bin);
    // 	      effDown -= resultVariedDown->GetBinContent(bin) * MC->GetBinContent(bin);
    // 	      // std::cout << "\tcnt = (" << setw(3) << binx << "," << setw(3) << biny << "," << setw(3) << binz
    // 	      // 		<< ") MC: " << setw(10) << MC->GetBinContent(bin) << " SF: (nom = " << setw(8) << result->GetBinContent(bin)
    // 	      // 		<< ", up = " << setw(8) << resultVariedUp->GetBinContent(bin) << "), up(cumulative): "
    // 	      // 		<< setw(8) << effUp << ", nom(cumulative): " << setw(8) << effNom << std::endl;
    // 	    }
    // 	    // What remains is the "data" tag weight fraction in the skipped bin.
    // 	    // Divide it by its corresponding MC tag weight fraction to obtain the varied scale factor
    // 	    Int_t bin = result->GetBin(binx, skipTagWeightBin, binz);
    // 	    resultVariedUp->SetBinContent(bin,  effUp/MC->GetBinContent(bin));
    // 	    resultVariedDown->SetBinContent(bin,effDown/MC->GetBinContent(bin));
    // 	    // std::cout << "\tcnt = (" << setw(3) << binx << "," << setw(3) << skipTagWeightBin << "," << setw(3) << binz
    // 	    // 	      << ") MC: " << setw(10) << MC->GetBinContent(bin) << ", resulting SF ( nom = " << setw(8) << effNom/MC->GetBinContent(bin)
    // 	    // 	      << ", check = " << setw(8) << result->GetBinContent(bin) << ", up = " << setw(8) << effUp/MC->GetBinContent(bin) << ")"
    // 	    // 	      << std::endl;
    // 	  }
    //   } else {
    // 	for (Int_t biny = 1; biny < nbiny-1; ++biny)
    // 	  for (Int_t binz = 1; binz < nbinz-1; ++binz) {
    // 	    double effUp = 1, effDown = 1;
    // 	    for (Int_t binx = 1; binx < nbinx-1; ++binx) {
    // 	      if (binx == skipTagWeightBin) continue;
    // 	      Int_t bin = result->GetBin(binx, biny, binz);
    // 	      effUp   -= resultVariedUp->GetBinContent(bin)   * MC->GetBinContent(bin);
    // 	      effDown -= resultVariedDown->GetBinContent(bin) * MC->GetBinContent(bin);
    // 	    }
    // 	    // What remains is the "data" tag weight fraction in the skipped bin.
    // 	    // Divide it by its corresponding MC tag weight fraction to obtain the varied scale factor
    // 	    Int_t bin = result->GetBin(skipTagWeightBin, biny, binz);
    // 	    resultVariedUp->SetBinContent(bin,  effUp/MC->GetBinContent(bin));
    // 	    resultVariedDown->SetBinContent(bin,effDown/MC->GetBinContent(bin));
    // 	  }
    //   }

    // }

    m_eigen.push_back(std::make_pair(resultVariedUp, resultVariedDown));

  } //end eigenvector size

  // Remove variations that are below the given tolerance (effectively meaning that they don't have any effect)
  IndexSet final_set;
  size_t current_set = 0;
  for (size_t index = 0; index < m_eigen.size(); ++index) {
    bool keep_variation = false;
    TH1 *up = static_cast<TH1*>(m_eigen[index].first->Clone()); up->SetDirectory(0);

    up->Add(result, -1.0);
    for (int bin = 1; bin <= nbins; ++bin) {
      if (fabs(up->GetBinContent(bin)) > min_variance) {
	keep_variation = true;
	break;
      }
    }
    delete up;
    if (!keep_variation)
      final_set.insert(current_set);
    ++current_set;
  }
  if (final_set.size() > 0) std::cout << "CalibrationDataEigenVariations: Removing " << final_set.size()
				      << " eigenvector variations leading to sub-tolerance effects, retaining "
				      << m_eigen.size()-final_set.size() << " variations" << std::endl;
  removeVariations(final_set);

  m_initialized = true;
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::removeVariations(const IndexSet &set)
{
  if (set.size() == 0) return;

  std::vector<std::pair<TH1*, TH1*> > new_eigen;
  for (size_t index = 0; index < m_eigen.size(); ++index)
    {
      if (set.count(index) == 0) new_eigen.push_back(m_eigen[index]);
      else { delete m_eigen[index].first; delete m_eigen[index].second; }
    }
  m_eigen = new_eigen;
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::removeVariations(const IndexSuperSet &set)
{
  IndexSet simple_set;

  for (IndexSuperSet::iterator set_it = set.begin();
       set_it != set.end(); ++set_it) {
    for (IndexSet::iterator subset_it = set_it->begin();
	 subset_it != set_it->end(); ++subset_it)
      simple_set.insert(*subset_it);
  }

  removeVariations(simple_set);
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::mergeVariationsFrom(const size_t& index)
{
  // Merge all systematic variation starting from the given index.
  // The resulting merged variation replaces the first entry in the list
  // (i.e., the entry specified by the index).
  IndexSet simple_set;

  for (size_t it = index; it < m_eigen.size(); ++it)
    simple_set.insert(it);
  mergeVariations(simple_set);
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::mergeVariations(const IndexSet &set)
{
  IndexSuperSet sset;
  sset.insert(set);
  mergeVariations(sset);
}

//________________________________________________________________________________
void
CalibrationDataEigenVariations::mergeVariations(const IndexSuperSet &set)
{
  // check for overlap
  IndexSet checker;
  for (IndexSuperSet::iterator set_it = set.begin();
       set_it != set.end(); ++set_it) {
    for (IndexSet::iterator subset_it = set_it->begin();
	 subset_it != set_it->end(); ++subset_it){
      if (checker.count(*subset_it) == 0 && *subset_it <= m_eigen.size())
        checker.insert(*subset_it);
      else {
        std::cerr << "Error in CalibrationDataEigenVariations::mergeVariations: \
          IndexSets must not overlap and must lie between 1 and "
		  << m_eigen.size() << ". Aborting!" << std::endl;
        return;
      }
    }
  }

  // retrieve the central calibration
  TH1 *result = static_cast<TH1*>(m_cnt->GetValue("result"));
  IndexSet toDelete;
  int nbins = result->GetNbinsX()+2;
  int ndim  = result->GetDimension();
  if (ndim > 1) nbins *= (result->GetNbinsY()+2);
  if (ndim > 2) nbins *= (result->GetNbinsZ()+2);

  // TH1 *var_up_final = static_cast<TH1*>(result->Clone()),
  //   *var_down_final = static_cast<TH1*>(result->Clone());

  // var_up_final->Reset();
  // var_down_final->Reset();

  // complex sum
  for (IndexSuperSet::iterator set_it = set.begin();
       set_it != set.end(); ++set_it) {
    if (set_it->empty()) continue;

    double sum_H_up = 0.0,
      sum_H_down = 0.0;
    size_t lowest_index = *set_it->lower_bound(0);
    TH1 *total_var_up = static_cast<TH1*>(m_eigen[lowest_index].first->Clone()),
      *total_var_down = static_cast<TH1*>(m_eigen[lowest_index].second->Clone());
    total_var_up->SetDirectory(0);
    total_var_down->SetDirectory(0);

    total_var_up->Reset();
    total_var_down->Reset();

    // sum all other variations
    for (IndexSet::iterator subset_it = set_it->begin();
	 subset_it != set_it->end(); ++subset_it) {
      size_t actual_index = *subset_it;

      if (actual_index != lowest_index) toDelete.insert(*subset_it);

      TH1 *partial_var_up = static_cast<TH1*>(m_eigen[actual_index].first->Clone()),
	*partial_var_down = static_cast<TH1*>(m_eigen[actual_index].second->Clone());
      partial_var_up->SetDirectory(0);
      partial_var_down->SetDirectory(0);
      
      partial_var_up->Add(result, -1.0);
      partial_var_down->Add(result, -1.0);
      for (int i = 0; i < nbins; ++i) {
        partial_var_down->SetBinContent(i, -1.0*partial_var_down->GetBinContent(i));
      }

      for (int u = 0; u < nbins; ++u) {
        double sum_up = total_var_up->GetBinContent(u),
	  sum_down = total_var_down->GetBinContent(u);
        for (int v = 0; v < nbins; ++v) {
          sum_up += partial_var_up->GetBinContent(u)*partial_var_up->GetBinContent(v);
          sum_H_up += partial_var_up->GetBinContent(u)*partial_var_up->GetBinContent(v);
          sum_down += partial_var_down->GetBinContent(u)*partial_var_down->GetBinContent(v);
          sum_H_down += partial_var_down->GetBinContent(u)*partial_var_down->GetBinContent(v);
        }
        total_var_up->SetBinContent(u, sum_up);
        total_var_down->SetBinContent(u, sum_down);
      }
      delete partial_var_up;
      delete partial_var_down;
    }

    // final part of complex summing
    for (int i = 0; i < nbins; ++i) {
      if (sum_H_up != 0.0)
        total_var_up->SetBinContent(i, total_var_up->GetBinContent(i)/sqrt(sum_H_up));
      else
        total_var_up->SetBinContent(i, 0.0);
      if (sum_H_down != 0.0)
        total_var_down->SetBinContent(i, -1.0*total_var_down->GetBinContent(i)/sqrt(sum_H_down));
      else
        total_var_down->SetBinContent(i, 0.0);
    }

    total_var_up->Add(result);
    total_var_down->Add(result);

    m_eigen[lowest_index].first = total_var_up;
    m_eigen[lowest_index].second = total_var_down;
  }

  removeVariations(toDelete);
}

//________________________________________________________________________________
unsigned int
CalibrationDataEigenVariations::getNumberOfNamedVariations() const
{
  // Returns the number of named variations

  return m_namedIndices.size();
}

//________________________________________________________________________________
vector<string>
CalibrationDataEigenVariations::listNamedVariations() const
{
  // Provides the list of named variations

  vector<string> names;
  for (map<string, unsigned int>::const_iterator it = m_namedIndices.begin();
       it != m_namedIndices.end(); ++it)
    names.push_back(it->first);
  return names;
}

//________________________________________________________________________________
unsigned int
CalibrationDataEigenVariations::getNumberOfEigenVariations() const
{
  if (! m_initialized) const_cast<CalibrationDataEigenVariations*>(this)->initialize();
  return m_eigen.size();
}

//________________________________________________________________________________
bool
CalibrationDataEigenVariations::getEigenvectorVariation(unsigned int variation,
							TH1*& up, TH1*& down) const
{
  // Return the pointers to the up- and downward variation histogram for the specified
  // eigenvector variation. In case of an invalid variation number, null pointers will
  // be returned and the return value will be false.
  //
  //     variation:   eigenvector variation number
  //     up:          (reference to) pointer to upward variation histogram
  //     down:        (reference to) pointer to downward variation histogram

  if (! m_initialized) const_cast<CalibrationDataEigenVariations*>(this)->initialize();
  if (variation < m_eigen.size()) {
    up   = m_eigen[variation].first;
    down = m_eigen[variation].second;
    return true;
  }

  up = down = 0;
  return false;
}

//________________________________________________________________________________
bool
CalibrationDataEigenVariations::getNamedVariation(const string& name,
						  TH1*& up, TH1*& down) const
{
  // Return the pointers to the up- and downward variation histogram for the specified
  // named variation. In case of an invalid named variation, null pointers will
  // be returned and the return value will be false.
  //
  //     name:        named variation
  //     up:          (reference to) pointer to upward variation histogram
  //     down:        (reference to) pointer to downward variation histogram

  map<string, unsigned int>::const_iterator it = m_namedIndices.find(name);
  if (it != m_namedIndices.end()) return getNamedVariation(it->second, up, down);

  up = down = 0;
  return false;
}

//________________________________________________________________________________
bool
CalibrationDataEigenVariations::getNamedVariation(unsigned int nameIndex,
						  TH1*& up, TH1*& down) const
{
  // Return the pointers to the up- and downward variation histogram for the specified
  // named variation. In case of an invalid named variation number, null pointers will
  // be returned and the return value will be false.
  //
  //     nameIndex:   named variation number
  //     up:          (reference to) pointer to upward variation histogram
  //     down:        (reference to) pointer to downward variation histogram

  if (! m_initialized) const_cast<CalibrationDataEigenVariations*>(this)->initialize();

  if (nameIndex < m_named.size()) {
    up   = m_named[nameIndex].first;
    down = m_named[nameIndex].second;
    return true;
  }

  up = down = 0;
  return false;
}

//________________________________________________________________________________
unsigned int
CalibrationDataEigenVariations::getNamedVariationIndex(const std::string& name) const
{
  // Return the integer index corresponding to the named variation.
  // Note that no checks are made on the validity of the name provided.

  map<string, unsigned int>::const_iterator it = m_namedIndices.find(name);
  return it->second;
}

//________________________________________________________________________________
bool
CalibrationDataEigenVariations::isExtrapolationVariation(unsigned int nameIndex) const
{
  // Verifies whether the given named variation index corresponds to the extrapolation
  // uncertainty.

  return (m_namedExtrapolation == int(nameIndex));
}

//________________________________________________________________________________
bool
CalibrationDataEigenVariations::EigenVectorRecomposition(const std::string label, 
							 std::map<std::string, std::map<std::string, float>> &coefficientMap) const
{
  // Calculating eigen vector recomposition coefficient map and pass to
  // user by reference. Return true if method success. Return false and
  // will not modify coefficientMap if function failed.
  //
  //     label:          flavour label
  //     coefficientMap: (reference to) coefficentMap which will be used as return value.
  if (! m_initialized) const_cast<CalibrationDataEigenVariations*>(this)->initialize();

  std::vector<TH1*> originSF_hvec;
  std::vector<TH1*> eigenSF_hvec;

  // Retrieving information for calculation
  std::vector<string>fullUncList = m_cnt->listUncertainties();
  std::vector<string> uncList;
  for (unsigned int t = 0; t < fullUncList.size(); ++t) {
    // entries that should never be included
    if (fullUncList[t] == "comment" || fullUncList[t] == "result" ||
	fullUncList[t] == "combined" || fullUncList[t] == "statistics" ||
        fullUncList[t] == "systematics" || fullUncList[t] == "MCreference" ||
        fullUncList[t] == "MChadronisation" || fullUncList[t] == "extrapolation" ||
        fullUncList[t] == "ReducedSets" || fullUncList[t] == "excluded_set") continue;
    // entries that can be excluded if desired
    if (m_namedIndices.find(fullUncList[t]) != m_namedIndices.end()) continue;
    
    TH1* hunc = dynamic_cast<TH1*>(m_cnt->GetValue(fullUncList[t].c_str()));

    Int_t nx = hunc->GetNbinsX();
    Int_t ny = hunc->GetNbinsY();
    Int_t nz = hunc->GetNbinsZ();
    Int_t bin = 0;
    bool retain = false; // Retain the histogram?

    // discard empty histograms
    // Read all bins without underflow&overflow
    for(Int_t binx = 1; binx <= nx; binx++)
      for(Int_t biny = 1; biny <= ny; biny++)
	for(Int_t binz = 1; binz <= nz; binz++){
	  bin = hunc->GetBin(binx, biny, binz);  
	  if (fabs(hunc->GetBinContent(bin)) > 1E-20){
	    retain = true;
	    break;
	  }
	}// end hist bin for-loop
    if (!retain){
      std::cout<<"Eigenvector Recomposition: Empty uncertainty "<<fullUncList.at(t)<<" is discarded."<<std::endl;
      continue; // discard the vector
    }

    uncList.push_back(fullUncList.at(t));
    originSF_hvec.push_back(hunc);
  }

  TH1* nom = dynamic_cast<TH1*>(m_cnt->GetValue("result")); // Nominal SF hist
  int dim = nom->GetDimension();
  Int_t nx = nom->GetNbinsX();
  Int_t ny = nom->GetNbinsY();
  Int_t nz = nom->GetNbinsZ();
  Int_t nbins = nx;
  if(dim > 1) nbins *= ny;
  if(dim > 2) nbins *= nz;
  TMatrixD matSF(uncList.size(), nbins);
  Int_t col = 0; // mark the column number
  // Fill the Delta SF Matrix
  for(unsigned int i = 0; i < uncList.size(); i++){
    col = 0;
    // Loop all bins except underflow&overflow bin
    for(int binz = 1; binz <= nz; binz++)
      for(int biny = 1; biny <= ny; biny++)
	for(int binx = 1; binx <= nx; binx++){
 	  Int_t bin = originSF_hvec.at(i)->GetBin(binx, biny, binz);
	  TMatrixDRow(matSF,i)[col] = originSF_hvec[i]->GetBinContent(bin);
	  col++;
	}
  }

  // get eigen vectors of scale factors. Note that this is not the original eigen-vector.
  int nEigen = getNumberOfEigenVariations();
  TH1* up = nullptr;
  TH1* down = nullptr;
  for (int i = 0; i < nEigen; i++){
    if (!getEigenvectorVariation(i, up, down)){
       std::cerr<<"EigenVectorRecomposition: Error on retrieving eigenvector "<<i<<std::endl;
      return false;
    }
    //Need uncertainty value so subtract central calibration here.
    up->Add(nom, -1);
    eigenSF_hvec.push_back(up);
  }
  TMatrixD matEigen(nEigen, nbins);

  // Fill the Eigen Matrix
  for(int i = 0; i < nEigen; i++){
    col = 0;
    // Read 300 bins without underflow&overflow
    for(int binz = 1; binz <= nz; binz++)
      for(int biny = 1; biny <= ny; biny++)
	for(int binx = 1; binx <= nx; binx++){
	  Int_t bin = eigenSF_hvec.at(i)->GetBin(binx, biny, binz);
	  TMatrixDRow(matEigen,i)[col] = eigenSF_hvec[i]->GetBinContent(bin);
	  col++;
	}
  }

  // Sanity check:
  TMatrixD matEigenOriginal = matEigen;
  TMatrixD matEigenTranspose = matEigen;
  matEigenTranspose = matEigenTranspose.T();
  TMatrixD matOriginalTimesTranspose = matEigenOriginal*matEigenTranspose;
  TMatrixD matEigenInvert = matEigenTranspose*matOriginalTimesTranspose.Invert();
  //(matEigenOriginal*matEigenInvert).DrawClone("colz"); // This should give us an identity matrix

  TMatrixD matCoeff = matSF*matEigenInvert;
  int nRows = matCoeff.GetNrows();
  int nCols = matCoeff.GetNcols();
  std::map<std::string, float> temp_map;
  for (int col = 0; col < nCols; col++){
    temp_map.clear();
    for(int row = 0; row < nRows; row++){
      temp_map[uncList[row]] = TMatrixDRow(matCoeff, row)[col];
    }
    coefficientMap["Eigen_"+label+"_"+std::to_string(col)] = temp_map;
  }
  
  return true;
}
