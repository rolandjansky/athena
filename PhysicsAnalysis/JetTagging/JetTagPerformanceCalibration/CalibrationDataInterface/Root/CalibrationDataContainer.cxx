/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CalibrationDataInterface/CalibrationDataContainer.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cmath>

#include "TH1.h"
#include "TAxis.h"
#include "TF1.h"
#include "TVectorT.h"
#include "TMatrixT.h"
#include "TMatrixDSym.h"
#include "TMath.h"
#include "TString.h"
#include "TObjString.h"

using Analysis::CalibrationStatus;
using Analysis::UncertaintyResult;
using Analysis::CalibrationDataContainer;
using Analysis::CalibrationDataHistogramContainer;
using Analysis::CalibrationDataMappedHistogramContainer;
using Analysis::CalibrationDataFunctionContainer;

// Things that are best hidden from the outside world...
namespace {

  // The value below is added to (subtracted from) the lower (upper) bound of validity
  // when a variable is restricted to be within its validity range.
  const double rangeEpsilon = 1.e-5;

  // array size for boundary specifications
  const int maxParameters = 10;

}




//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataContainer                                                                 //
//                                                                                          //
// These container classes represent the basic b-tagging calibration information            //
// stored in ROOT files. The abstract base class is the CalibrationDataContainer,           //
// and this "container" is meant to be used for basic jet-by-jet usage. The idea            //
// is that the container object itself contains sufficient information to determine         //
// how b-tagging information (mostly efficiencies for given working points, but in          //
// the case of so-called "continuous tagging" it can be the fractions of jets in            //
// tag weight bins) is parametrised. The retrieval of the requested information is          //
// then done by passing a (reference to a) CalibrationDataVariables struct, which is        //
// a very simple object containing information that can potentially be used for the         //
// parametrisation. It is then up to the container object to figure out what subset         //
// of the information then to use in reality. The user may request a fairly wide            //
// range of information, ranging from mere central values (typically,                       //
// CalibrationDataContainer::getResult() will be used for this) to individual               //
// contributions to the total systematic uncertainty (using                                 //
// CalibrationDataContainer::getUncertainty()). The container class derives from a          //
// TMap, with keys being strings (TObjString, for technical reasons) ensuring significant   //
// flexibility as to the specification of uncertainty information.                          //
//                                                                                          //
// The underlying information can be presented in two forms: in histogram form (this        //
// corresponds to the CalibrationDataHistogramContainer derived class), and as a            //
// functional parametrisation (the CalibrationDataFunctionContainer derived class).         //
// So far, the CalibrationDataHistogramContainer has been used for the storage of           //
// data/MC calibration scale factor information, while up to recently the                   //
// CalibrationDataFunctionContainer was used to store MC information. Recently, the         //
// storage of MC information also moved to the use of histograms, because of biases         //
// in the functional parametrisations (in addition to the fact that in order to             //
// derive these parametrisations in the first place, a non-negligible amount of             //
// manual intervention is required).                                                        //
//                                                                                          //
// CalibrationDataContainer.cxx, (c) ATLAS Detector software                                //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataContainer)
#endif

//________________________________________________________________________________
CalibrationDataContainer::CalibrationDataContainer(const char* name) :
  TMap(), m_objResult(0), m_objSystematics(0), m_restrict(false)
{
  // default constructor
  SetName(name);
}

//________________________________________________________________________________
CalibrationDataContainer::~CalibrationDataContainer()
{
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataContainer::getSystUncertainty(const CalibrationDataVariables& x,
					     UncertaintyResult& result, TObject* obj) const
{
  // short-hand for the total systematic uncertainty retrieval.
  // For "normal" usage (retrieval of central values and total uncertainties), the total systematic
  // uncertainty object needs to be accessed frequently. In order to avoid nee

  // cache the pointer to the "systematics" object (to avoid string comparisons)
  if (!obj) {
    if (! m_objSystematics) {
      // std::cout << "retrieving total systematics pointer" << std::endl;
      m_objSystematics = GetValue("systematics");
    }
    obj = m_objSystematics;
  }
  return getUncertainty("systematics", x, result, obj);
}

//________________________________________________________________________________
std::vector<std::string>
CalibrationDataContainer::listUncertainties() const
{
  // Retrieve the list of uncertainties for this calibration.
  // Note that this is an un-pruned list: it contains also entries that
  // are not proper uncertainties (e.g. "result", "comment")

  std::vector<std::string> uncertainties;
  TIter it(GetTable());
  while (TPair* pair = (TPair*) it()) {
    std::string spec(pair->Key()->GetName());
    uncertainties.push_back(spec);
  }
  return uncertainties;
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataContainer::getUncertainties(const CalibrationDataVariables& x,
					   std::map<std::string, UncertaintyResult>& all) const
{
  // Retrieve all uncertainties for this calibration.

  CalibrationStatus mycode(Analysis::kSuccess);
  UncertaintyResult result;

  // first treat the "result" entry separately
  double single_result;
  CalibrationStatus code = getResult(x, single_result);
  if (code == Analysis::kError) {
    std::cerr << "in CalibrationDataContainer::getUncertainties(): error retrieving result!" << std::endl;
    return code;
  }
  else if (code != Analysis::kSuccess) mycode = code;
  result.first = single_result;
  result.second = 0;
  all[std::string("result")] = result;

  // similar for the "statistics" entry
  code = getStatUncertainty(x, single_result);
  if (code == Analysis::kError) {
    std::cerr << "in CalibrationDataContainer::getUncertainties(): error retrieving stat. uncertainty!" << std::endl;
    return code;
  }
  else if (code != Analysis::kSuccess) mycode = code;
  result.first  =  single_result;
  result.second = -single_result;
  all[std::string("statistics")] = result;

  // then cycle through the other (systematic) uncertainties
  TIter it(GetTable());
  while (TPair* pair = (TPair*) it()) {
    std::string spec(pair->Key()->GetName());
    // ignore these specific entries
    if (spec == "comment" || spec == "result" || spec == "statistics" || spec == "MChadronisation") continue;
    code = getUncertainty(spec, x, result, pair->Value());
    // we should never be finding any errors
    if (code == Analysis::kError) {
      std::cerr << "in CalibrationDataContainer::getUncertainties(): error retrieving named uncertainty "
		<< spec << "!" << std::endl;
      return code;
    }
    // this assumes that non-success codes are likely to be correlated between uncertainty sources
    else if (code != Analysis::kSuccess) mycode = code;
    all[spec] = result;
  }
  return mycode;
}

//________________________________________________________________________________
std::string
CalibrationDataContainer::getComment() const
{
  // Retrieve the comments for this calibration (if any)

  TObject* obj = GetValue("comment");
  if (! obj) return std::string("");
  TObjString* s = dynamic_cast<TObjString*>(obj);
  if (! s ) return std::string("");
  return std::string(s->GetName());
}

//________________________________________________________________________________
std::string
CalibrationDataContainer::getHadronisation() const
{
  // Retrieve the hadronisation reference for this calibration (if any)
  const static std::string null("");

  TObject* obj = GetValue("MChadronisation");
  if (! obj) return null;
  TObjString* s = dynamic_cast<TObjString*>(obj);
  if (! s ) return null;
  return std::string(s->GetName());
}

//________________________________________________________________________________
void
CalibrationDataContainer::setUncertainty(const std::string& unc, TObject* obj)
{
  // Insert (or replace) the given object at the position indicated by the given index.
  //
  //     unc:       uncertainty index
  //     obj:       object to be entered (needs to inherit from TObject)

  if (TPair* p = (TPair*) FindObject(unc.c_str())) DeleteEntry(p->Key());
  Add(new TObjString(unc.c_str()), obj);
}

//________________________________________________________________________________
void
CalibrationDataContainer::setResult(TObject* obj)
{
  // Specialization of the setUncertainty() method: insert the calibration result
  //
  //     obj:       object to be entered (needs to inherit from TObject)

  setUncertainty(std::string("result"), obj);
}

//________________________________________________________________________________
void
CalibrationDataContainer::setComment(const std::string& text)
{
  // Insert (or replace) the comment field. This needs to be handled somewhat
  // specially as TString itself doesn't inherit from TObject.
  //
  //    text:       comment field (will be converted to TObjString)

  if (TPair* p = (TPair*) FindObject("comment")) DeleteEntry(p->Key());
  Add(new TObjString("comment"), new TObjString(text.c_str()));
}

//________________________________________________________________________________
void
CalibrationDataContainer::setHadronisation(const std::string& text)
{
  // Insert (or replace) the hadronisation reference.
  //
  //    text:       hadronisation reference in string form (will be converted to TObjString)

  if (TPair* p = (TPair*) FindObject("MChadronisation")) DeleteEntry(p->Key());
  Add(new TObjString("MChadronisation"), new TObjString(text.c_str()));
}

//________________________________________________________________________________
int
CalibrationDataContainer::typeFromString(const std::string& key) const
{
  // Small utility function collecting the correspondence between axis labels and (integer) variable indices.
  // In case of an unknown label, a negative number will be returned to flag the issue.

  if      (key == "eta") return kEta;
  else if (key == "abseta") return kAbsEta;
  else if (key == "pt") return kPt;
  else if (key == "tagweight") return kTagWeight;
  // return value for unknown keywords
  else return -1;
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataContainer::computeVariables(const CalibrationDataVariables& x, bool extrapolate) const
{
  // Determine which variables are to be used, and insert them in a separate array (which is only used internally).
  // The return value is used to indicate whether any input co-ordinate was out of bounds; where a distinction
  // is made between being outside the extrapolation region (kExtrapolatedRange) or merely the calibration region
  // (kRange).
  // The "extrapolate" variable is used to flag whether an extrapolation uncertainty applies
  // (this should anyway occur only for histogram containers).
  // This is also the place where any computations are being done (e.g. jet pt values are divided by 1000
  // to convert them from MeV to GeV).

  // ensure that the variable types have been computed properly
  if (m_variables.size() == 0) computeVariableTypes();

  // also keep track of whether the variables are within bounds
  CalibrationStatus status(kSuccess);

  // std::cout << "computeVariables(): input jet pt: " << x.jetPt << ", eta " << x.jetEta << ", tag weight " << x.jetTagWeight << std::endl;

  for (unsigned int var = 0; var < m_variables.size(); ++var) {
    switch (m_variables[var]) {
    case kPt:
      // assume that the input values are given in MeV but the performance calibration in GeV!
      m_vars[var] = x.jetPt * 0.001;
      break;
    case kEta:
      m_vars[var] = x.jetEta;
      break;
    case kAbsEta:
      m_vars[var] = x.jetEta;
      if (m_vars[var] < 0) m_vars[var] *= -1.0;
      break;
    case kTagWeight:
      m_vars[var] = x.jetTagWeight;
    }
    if (m_vars[var] < getLowerBound(m_variables[var], extrapolate)) {
      if (status != kExtrapolatedRange) {
	status  = (extrapolate || (m_vars[var] < getLowerBound(m_variables[var], true))) ? kExtrapolatedRange : kRange;
	// std::cout << "computeVariables(): variable " << var << ", value: " << m_vars[var] << ", setting status to " << status << std::endl;
      }
      if (m_restrict) m_vars[var] = getLowerBound(m_variables[var], extrapolate) + rangeEpsilon;
    } else if (m_vars[var] >= getUpperBound(m_variables[var], extrapolate)) {
      if (status != kExtrapolatedRange) {
	status  = (extrapolate || (m_vars[var] >= getUpperBound(m_variables[var], true))) ? kExtrapolatedRange : kRange;
	// std::cout << "computeVariables(): variable " << var << ", value: " << m_vars[var] << ", extrapolate? " << extrapolate
	// 	  << ", upper bound: " << getUpperBound(m_variables[var],extrapolate)
	// 	  << " (extrapolation bound: " << getUpperBound(m_variables[var],true) << "), setting status to " << status << std::endl;
      }
      if (m_restrict) m_vars[var] = getUpperBound(m_variables[var], extrapolate) - rangeEpsilon;
    }
  }

  // std::cout << "computeVariables(): output variables: " << m_vars[0] << ", " << m_vars[1] << ", " << m_vars[2] << std::endl;

  return status;
}

//________________________________________________________________________________
double
CalibrationDataContainer::getLowerBound(unsigned int vartype, bool extrapolate) const
{
  // Utility function returning the lower validity bound for the given variable type.
  // The "extrapolate" variable flags whether normal validity bounds are to be used,
  // or instead those relevant for the extrapolation uncertainty.

  double minDefault = (vartype == kAbsEta || vartype == kPt) ? 0 : -std::numeric_limits<double>::max();
  if (! (vartype < m_lowerBounds.size())) return minDefault;
  return extrapolate ? m_lowerBoundsExtrapolated[vartype] : m_lowerBounds[vartype];
}

//________________________________________________________________________________
double
CalibrationDataContainer::getUpperBound(unsigned int vartype, bool extrapolate) const
{
  // Utility function returning the upper validity bound for the given variable type.
  // The "extrapolate" variable flags whether normal validity bounds are to be used,
  // or instead those relevant for the extrapolation uncertainty.

  if (! (vartype < m_lowerBounds.size())) return std::numeric_limits<double>::max();
  return extrapolate ? m_upperBoundsExtrapolated[vartype] : m_upperBounds[vartype];
}

//________________________________________________________________________________
std::vector<std::pair<double, double> >
CalibrationDataContainer::getBounds() const
{
  // List the validity bounds relevant to this container.

  // ensure that the variable types have been computed properly
  if (m_variables.size() == 0) computeVariableTypes();

  std::vector<std::pair<double, double> > bounds;
  for (unsigned int t = 0; t < m_lowerBounds.size() && t <= kAbsEta; ++t) {
    bounds.push_back(std::make_pair(m_lowerBounds[t], m_upperBounds[t]));
  }
  return bounds;
}


//________________________________________________________________________________
std::vector<unsigned int>
CalibrationDataContainer::getVariableTypes() const
{
  // List the variable types used for this calibration object.
  // The meaning of the types is encapsulated by the CalibrationParametrization enum.

  // ensure that the variable types have been computed properly
  if (m_variables.size() == 0) computeVariableTypes();

  return m_variables;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataHistogramContainer                                                        //
//                                                                                          //
/* Begin_Html
<p> The CalibrationDataHistogramContainer class inherits from the CalibrationDataContainer
 abstract class. It covers the cases where the relevant information is presented in
 binned form.
</p>
<p> This class allows for the following features:
<ul>
  <li>Access to individual uncertainty contributions, in addition to total (or separate
      statistical and systematic) uncertainties.</li>
  <li>Access to correlations between calibration bins. Note that exploiting this
      requires using the CalibrationDataEigenVariations class.</li>
  <li>Histogram interpolation (note that this implementation is not complete; however it
      should work at least for the most common implementation of 2D MC efficiencies. For
      this case, interpolation has been demonstrated to be more accurate than
      parametrisations).</li>
  <li>Use with "continuous tagging" (in which the -binned- tag weight distribution is
      considered in different kinematic bins) in addition to the more straightforward
      consideration of the efficiency to pass a given tag weight cut.</li>
</ul>
</p>
End_Html */
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataHistogramContainer)
#endif

//________________________________________________________________________________
CalibrationDataHistogramContainer::CalibrationDataHistogramContainer(const char* name) :
  CalibrationDataContainer(name), m_interpolate(false)
{
  // default constructor.

  // // Reset 'regular' bin ranges to a nonsensical value
  // for (unsigned int t = 0; t < 3; ++t) {
  //   m_binmin[t] = m_binmax[t] = 0;
  //   m_noExtrapolation[t] = false;
  // }

  // Reset the validity bounds (including those for extrapolation uncertainties) to reflect 'no bounds'.
  // They will be re-determined upon the first computation.

  m_lowerBounds.clear();
  m_lowerBounds.resize(maxParameters, -std::numeric_limits<double>::max());
  m_lowerBounds[kPt] = m_lowerBounds[kAbsEta] = 0;
  m_upperBounds.clear();
  m_upperBounds.resize(maxParameters, std::numeric_limits<double>::max());

  m_lowerBoundsExtrapolated.clear();
  m_lowerBoundsExtrapolated.resize(maxParameters, -std::numeric_limits<double>::max());
  m_lowerBoundsExtrapolated[kPt] = m_lowerBounds[kAbsEta] = 0;
  m_upperBoundsExtrapolated.clear();
  m_upperBoundsExtrapolated.resize(maxParameters, std::numeric_limits<double>::max());

  // But by default, switch on the range checking
  restrictToRange(true);
}

//________________________________________________________________________________
CalibrationDataHistogramContainer::~CalibrationDataHistogramContainer()
{
}


//________________________________________________________________________________
void
CalibrationDataHistogramContainer::computeVariableTypes() const
{
  // Compute the variable types for this container object, using the histogram axis labels.
  // Valid axis labels can be found in the CalibrationDataContainer::typeFromString() method.
  // Note that only the "result" histogram is inspected; it is assumed that all
  // histograms provided use the same binning (a small exception is the "extrapolation"
  // uncertainty histogram, which may have additional bins beyond the usual validity bounds).
  //
  // This function will be called upon first usage, and its results cached internally.
  // It also calls checkBounds() to determine the validity bounds.

  // cache pointer to central values histogram
  if (! m_objResult) m_objResult = GetValue("result");

  // histograms need a special treatment, as the coordinate titles are not actually stored
  // with the title itself, but instead moved off to the axis titles...
  const TH1* hobj = dynamic_cast<const TH1*>(m_objResult);

  // no protection against null pointers here -- should not be necessary?
  int dims = hobj->GetDimension();
  for (int dim = 0; dim < dims; ++dim) {
    const TAxis* axis = 0;
    switch (dim) {
    case 0: axis = hobj->GetXaxis(); break;
    case 1: axis = hobj->GetYaxis(); break;
    default: axis = hobj->GetZaxis();
    }
    int vartype = typeFromString(axis->GetTitle());
    if (vartype < 0) {
      // Only flag the issue but otherwise take no action (assume non-argument use of a semicolon)
      std::cerr << "in CalibrationDataHistogramContainer::computeVariableTypes(): cannot construct variable type from name "
		<< axis->GetTitle() << std::endl;
    } else {
      m_variables.push_back((unsigned int) vartype);
    }
  }

  // After doing this, we should always have a non-null vector!
  assert(m_variables.size() > 0);

  // also compute the validity bounds for this calibration object
  const_cast<CalibrationDataHistogramContainer*>(this)->checkBounds();
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataHistogramContainer::getResult(const CalibrationDataVariables& x,
					     double& result, TObject* obj, bool extrapolate) const
{
  // Retrieve the central value for the given input variables. There are cases where
  // it may be useful to provide an alternative histogram rather than the original
  // one; in such cases (notably used with eigenvector variations) it is possible to
  // provide a pointer to this alternative histogram.
  //
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative results histogram
  //     extrapolate: set to true if bounds checking is to be carried out to looser
  //                  validity bounds as relevant for extrapolation uncertainties
  if (!obj) {
    if (! m_objResult) {
      // std::cout << "retrieving central value pointer" << std::endl;
      m_objResult = GetValue("result");
    }
    obj = m_objResult;
  }
  TH1* hist = dynamic_cast<TH1*>(obj);
  if (! hist) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x, extrapolate);
  // find the relevant "global" bin.
  // Note the limitation: at most three dimensions are supported.
  // TH1::FindFixBin() will ignore the variables not needed.
  // Note: FindFixBin() is only available in "recent" ROOT versions (FindBin() is appropriate for older versions)
  // (otherwise we need to rely on the ResetBit(TH1::kCanRebin) method having been used)
  if (m_interpolate) {
    // std::cout << "retrieving interpolated result" << std::endl;
    result = getInterpolatedResult(hist);
  } else {
    // std::cout << "retrieving binned result" << std::endl;
    Int_t bin = hist->FindFixBin(m_vars[0], m_vars[1], m_vars[2]);
    // Int_t bin = findBin(hist, false);
    result = hist->GetBinContent(bin);
  }

  return status;
}

// statistical uncertainty retrieval (special since it is stored with the result itself)

//________________________________________________________________________________
CalibrationStatus
CalibrationDataHistogramContainer::getStatUncertainty(const CalibrationDataVariables& x,
						      double& result) const
{
  // Retrieve the statistical uncertainty for the given input variables.
  //
  //     x:           input variables
  //     result:      result

  if (! m_objResult) {
    // std::cout << "retrieving central value pointer" << std::endl;
    m_objResult = GetValue("result");
  }
  TH1* hist = dynamic_cast<TH1*>(m_objResult);
  if (! hist) {
    std::cout << " getStatUncertainty error: no (valid) central values object!" << std::endl;
    return Analysis::kError;
  }

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);
  // find the relevant "global" bin.
  // Note the limitation: at most three dimensions are supported.
  // TH1::FindFixBin() will ignore the variables not needed.
  // Note: FindFixBin() is only available in "recent" ROOT versions (FindBin() is appropriate for older versions)
  // (otherwise we need to rely on the ResetBit(TH1::kCanRebin) method having been used)
  if (m_interpolate) {
    // interpolating the uncertainties doesn't seem very sensible..
    result = getInterpolatedUncertainty(hist);
  } else {
    Int_t bin = hist->FindFixBin(m_vars[0], m_vars[1], m_vars[2]);
    // Int_t bin = findBin(hist, false);
    result = hist->GetBinError(bin);
  }

  return status;
}

// general uncertainty retrieval

//________________________________________________________________________________
CalibrationStatus
CalibrationDataHistogramContainer::getUncertainty(const std::string& unc,
						  const CalibrationDataVariables& x,
						  UncertaintyResult& result, TObject* obj) const
{
  // Retrieve the uncertainty for the given input variables.
  //
  //     unc:         keyword indicating requested source of uncertainty. This should
  //                  correspond to one of the histograms added explicitly as a systematic
  //                  uncertainty or the keyword "statistics" (statistical uncertainties are
  //                  accessed differently, see method getStatUncertainty()).
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative or cached histogram

  if (unc == "statistics") {
    // treat statistical uncertainties separately (they are stored with the actual result)
    double res;
    CalibrationStatus code = getStatUncertainty(x, res);
    if (code == Analysis::kError) return code;
    result.first =   res;
    result.second = -res;
    return code;
  }

  if (!obj) obj = GetValue(unc.c_str());
  TH1* hist = dynamic_cast<TH1*>(obj);
  if (! hist) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x, (unc == "extrapolation") );

  if (m_interpolate) {
    result.first = getInterpolatedResult(hist);
    // symmetrise the uncertainty (as there is no code to interpolate the bin errors)
    result.second = -result.first;
  } else {
    // TH1::FindFixBin() will ignore the variables not needed.
    // Note: FindFixBin() is only available in "recent" ROOT versions (FindBin() is appropriate for older versions)
    // (otherwise we need to rely on the ResetBit(TH1::kCanRebin) method having been used)
    Int_t bin = hist->FindFixBin(m_vars[0], m_vars[1], m_vars[2]);
    // the "first" and "second" entries are filled with the
    // "positive" and "negative" uncertainties, respectively.
    result.first = hist->GetBinContent(bin);
    result.second = hist->GetBinError(bin);
  }

  return status;
}

//________________________________________________________________________________
void
CalibrationDataHistogramContainer::checkBounds()
{
  // Determine the bounds of validity for this calibration object. If an "extrapolation"
  // uncertainty histogram exists, it is used to determine the (typically) looser bounds
  // of validity appropriate for extrapolation uncertainties.

  const TH1* hist = dynamic_cast<const TH1*>(m_objResult);
  if (!hist) {
    std::cerr << "in CalibrationDataHistogramContainer::checkBounds(): object type does not derive from TH1" << std::endl;
    return;
  } else if (hist->GetDimension() != int(m_variables.size())) {
    std::cerr << "in CalibrationDataHistogramContainer::checkBounds(): given number of variable types ("
	      << m_variables.size() << ") doesn't match histogram dimension ("
	      << hist->GetDimension() << ")!" << std::endl;
    return;
  }
  // if an extrapolation uncertainty histogram was provided, use this to determine a second set of validity bounds
  const TH1* hExtrapolate = dynamic_cast<const TH1*>(GetValue("extrapolation"));
  // if (hExtrapolate) std::cout << "debug: found extrapolation histogram" << std::endl;
  for (unsigned int t = 0; int(t) < hist->GetDimension(); ++t) {
    const TAxis* axis; const TAxis* axis2 = 0;
    switch (t) {
    case 0: axis = hist->GetXaxis(); if (hExtrapolate) axis2 = hExtrapolate->GetXaxis(); break;
    case 1: axis = hist->GetYaxis(); if (hExtrapolate) axis2 = hExtrapolate->GetYaxis(); break;
    default: axis = hist->GetZaxis(); if (hExtrapolate) axis2 = hExtrapolate->GetZaxis();
    }
    
    // for (unsigned int t = 0; t < m_variables.size(); ++t) {
    //   if (m_variables[t] > m_upperBounds.size()) {
    // 	std::cerr << "in CalibrationDataHistogramContainer::checkBounds(): variable " << t << "type ("
    // 		  << m_variables[t] << "exceeds maximum type number (" << m_upperBounds.size() << ")!"
    // 		  << std::endl;
    // 	return;
    //   }
    // }
    m_upperBounds[m_variables[t]] = axis->GetXmax();
    m_lowerBounds[m_variables[t]] = axis->GetXmin();
    m_upperBoundsExtrapolated[m_variables[t]] = (axis2) ? axis2->GetXmax() : m_upperBounds[m_variables[t]];
    m_lowerBoundsExtrapolated[m_variables[t]] = (axis2) ? axis2->GetXmin() : m_lowerBounds[m_variables[t]];
    // std::cout << "debug: min = " << m_lowerBounds[m_variables[t]] << ", max = " << m_upperBounds[m_variables[t]]
    // 	      << ", extrap min = " << m_lowerBoundsExtrapolated[m_variables[t]] << ", extrap max = "
    // 	      << m_upperBoundsExtrapolated[m_variables[t]] << std::endl;
  }
}

//________________________________________________________________________________
bool
CalibrationDataHistogramContainer::isBinCorrelated(const std::string& unc) const
{
  // Indicate whether the given uncertainty is correlated from bin to bin or not
  //  (note that this function is to be used only for _systematic_ uncertainties)
  return (m_uncorrelatedSyst.FindObject(unc.c_str()) == 0);
}

//________________________________________________________________________________
void
CalibrationDataHistogramContainer::setUncorrelated(const std::string& unc)
{
  // Indicate that the given uncertainty is to be treated uncorrelated from bin to bin
  // (the default is for all systematic uncertainties to be treated as correlated).
  // This method is not normall intended to be used during physics analysis; this
  // information is written to and read back from the calibration file.

  m_uncorrelatedSyst.Add(new TObjString(unc.c_str()));
}

//________________________________________________________________________________
void
CalibrationDataHistogramContainer::setInterpolated(bool doInterpolate)
{
  // Indicate whether results are to be interpolated between bins or not
  // (this feature is thought to be useful mostly for MC efficiencies).
  // The default is not to use any interpolation.

  m_interpolate = doInterpolate;
}

//________________________________________________________________________________
bool
CalibrationDataHistogramContainer::isInterpolated() const
{
  // Indicate whether histogram interpolation is used or not.

  return m_interpolate;
}

//________________________________________________________________________________
double
CalibrationDataHistogramContainer::getInterpolatedResult(TH1* hist) const
{
  // Small utility function (intended for internal use only) for the retrieval of interpolated results

  switch (hist->GetDimension()) {
  case 3:
    return hist->Interpolate(m_vars[0], m_vars[1], m_vars[2]);
  case 2:
    return hist->Interpolate(m_vars[0], m_vars[1]);
  case 1:
  default:
    return hist->Interpolate(m_vars[0]);
  }
}

//________________________________________________________________________________
double
CalibrationDataHistogramContainer::getInterpolatedUncertainty(TH1* hist) const
{
  TAxis* xAxis = hist->GetXaxis();
  TAxis* yAxis = 0; TAxis* zAxis = 0;
  Double_t x0,x1,y0,y1;

  Int_t ndim = hist->GetDimension();
  if (ndim == 1) {
    // Code copied from TH1::Interpolate()

    Int_t xbin = hist->FindBin(m_vars[0]);

    if(m_vars[0] <= hist->GetBinCenter(1)) {
      return hist->GetBinError(1);
    } else if(m_vars[0] >= hist->GetBinCenter(hist->GetNbinsX())) {
      return hist->GetBinError(hist->GetNbinsX());
    }

    if(m_vars[0] <= hist->GetBinCenter(xbin)) {
      y0 = hist->GetBinError(xbin-1);
      x0 = hist->GetBinCenter(xbin-1);
      y1 = hist->GetBinError(xbin);
      x1 = hist->GetBinCenter(xbin);
    } else {
      y0 = hist->GetBinError(xbin);
      x0 = hist->GetBinCenter(xbin);
      y1 = hist->GetBinError(xbin+1);
      x1 = hist->GetBinCenter(xbin+1);
    }
    return y0 + (m_vars[0]-x0)*((y1-y0)/(x1-x0));

  } else if (ndim == 2) {

    // Code copied from TH2::Interpolate()

    Double_t f=0;
    x1 = y1 = 0;
    Double_t x2=0,y2=0;
    Double_t dx,dy;
    yAxis = hist->GetYaxis();
    Int_t bin_x = xAxis->FindBin(m_vars[0]);
    Int_t bin_y = yAxis->FindBin(m_vars[1]);
    if (bin_x<1 || bin_x>hist->GetNbinsX() || bin_y<1 || bin_y>hist->GetNbinsY()) {
      Error("Interpolate","Cannot interpolate outside histogram domain.");
      return 0;
    }
    // Int_t quadrant = 0; // CCW from UR 1,2,3,4
    // which quadrant of the bin (bin_P) are we in?
    dx = xAxis->GetBinUpEdge(bin_x)-m_vars[0];
    dy = yAxis->GetBinUpEdge(bin_y)-m_vars[1];
    if (dx<=xAxis->GetBinWidth(bin_x)/2 && dy<=yAxis->GetBinWidth(bin_y)/2) {
      // quadrant = 1; // upper right
      x1 = xAxis->GetBinCenter(bin_x);
      y1 = yAxis->GetBinCenter(bin_y);
      x2 = xAxis->GetBinCenter(bin_x+1);
      y2 = yAxis->GetBinCenter(bin_y+1);
    } else if (dx>xAxis->GetBinWidth(bin_x)/2 && dy<=yAxis->GetBinWidth(bin_y)/2) {
      // quadrant = 2; // upper left
      x1 = xAxis->GetBinCenter(bin_x-1);
      y1 = yAxis->GetBinCenter(bin_y);
      x2 = xAxis->GetBinCenter(bin_x);
      y2 = yAxis->GetBinCenter(bin_y+1);
    } else if (dx>xAxis->GetBinWidth(bin_x)/2 && dy>yAxis->GetBinWidth(bin_y)/2) {
      // quadrant = 3; // lower left
      x1 = xAxis->GetBinCenter(bin_x-1);
      y1 = yAxis->GetBinCenter(bin_y-1);
      x2 = xAxis->GetBinCenter(bin_x);
      y2 = yAxis->GetBinCenter(bin_y);
    } else {
      // quadrant = 4; // lower right
      x1 = xAxis->GetBinCenter(bin_x);
      y1 = yAxis->GetBinCenter(bin_y-1);
      x2 = xAxis->GetBinCenter(bin_x+1);
      y2 = yAxis->GetBinCenter(bin_y);
    }
    Int_t bin_x1 = xAxis->FindBin(x1);
    if(bin_x1<1) bin_x1=1;
    Int_t bin_x2 = xAxis->FindBin(x2);
    if(bin_x2>hist->GetNbinsX()) bin_x2=hist->GetNbinsX();
    Int_t bin_y1 = yAxis->FindBin(y1);
    if(bin_y1<1) bin_y1=1;
    Int_t bin_y2 = yAxis->FindBin(y2);
    if(bin_y2>hist->GetNbinsY()) bin_y2=hist->GetNbinsY();
    Int_t bin_q22 = hist->GetBin(bin_x2,bin_y2);
    Int_t bin_q12 = hist->GetBin(bin_x1,bin_y2);
    Int_t bin_q11 = hist->GetBin(bin_x1,bin_y1);
    Int_t bin_q21 = hist->GetBin(bin_x2,bin_y1);
    Double_t q11 = hist->GetBinError(bin_q11);
    Double_t q12 = hist->GetBinError(bin_q12);
    Double_t q21 = hist->GetBinError(bin_q21);
    Double_t q22 = hist->GetBinError(bin_q22);
    Double_t d = 1.0*(x2-x1)*(y2-y1);
    f = 1.0*q11/d*(x2-m_vars[0])*(y2-m_vars[1])
      + 1.0*q21/d*(m_vars[0]-x1)*(y2-m_vars[1])
      + 1.0*q12/d*(x2-m_vars[0])*(m_vars[1]-y1)
      + 1.0*q22/d*(m_vars[0]-x1)*(m_vars[1]-y1);
    return f;

  } else {

    // Copied from TH3::Interpolate()

    zAxis = hist->GetZaxis();

    Int_t ubx = xAxis->FindBin(m_vars[0]);
    if ( m_vars[0] < xAxis->GetBinCenter(ubx) ) ubx -= 1;
    Int_t obx = ubx + 1;

    Int_t uby = yAxis->FindBin(m_vars[1]);
    if ( m_vars[1] < yAxis->GetBinCenter(uby) ) uby -= 1;
    Int_t oby = uby + 1;

    Int_t ubz = zAxis->FindBin(m_vars[2]);
    if ( m_vars[2] < zAxis->GetBinCenter(ubz) ) ubz -= 1;
    Int_t obz = ubz + 1;


    //    if ( IsBinUnderflow(GetBin(ubx, uby, ubz)) ||
    //         IsBinOverflow (GetBin(obx, oby, obz)) ) {
    if (ubx <=0 || uby <=0 || ubz <= 0 ||
	obx > xAxis->GetNbins() || oby > yAxis->GetNbins() || obz > zAxis->GetNbins() ) {
    }

    Double_t xw = xAxis->GetBinCenter(obx) - xAxis->GetBinCenter(ubx);
    Double_t yw = yAxis->GetBinCenter(oby) - yAxis->GetBinCenter(uby);
    Double_t zw = zAxis->GetBinCenter(obz) - zAxis->GetBinCenter(ubz);

    Double_t xd = (m_vars[0] - xAxis->GetBinCenter(ubx)) / xw;
    Double_t yd = (m_vars[1] - yAxis->GetBinCenter(uby)) / yw;
    Double_t zd = (m_vars[2] - zAxis->GetBinCenter(ubz)) / zw;


    Double_t v[] = { hist->GetBinError( ubx, uby, ubz ), hist->GetBinError( ubx, uby, obz ),
		     hist->GetBinError( ubx, oby, ubz ), hist->GetBinError( ubx, oby, obz ),
		     hist->GetBinError( obx, uby, ubz ), hist->GetBinError( obx, uby, obz ),
		     hist->GetBinError( obx, oby, ubz ), hist->GetBinError( obx, oby, obz ) };


    Double_t i1 = v[0] * (1 - zd) + v[1] * zd;
    Double_t i2 = v[2] * (1 - zd) + v[3] * zd;
    Double_t j1 = v[4] * (1 - zd) + v[5] * zd;
    Double_t j2 = v[6] * (1 - zd) + v[7] * zd;


    Double_t w1 = i1 * (1 - yd) + i2 * yd;
    Double_t w2 = j1 * (1 - yd) + j2 * yd;


    Double_t result = w1 * (1 - xd) + w2 * xd;

   return result;
  };
}


//________________________________________________________________________________
int
CalibrationDataHistogramContainer::getTagWeightAxis() const
{
  // Test whether this calibration object is one for "continuous" calibration
  // (this has some subtle consequences for the treatment of bin-to-bin correlations).
  // The return value will be -1 in case this is not a "continuous" calibration object,
  // and the axis number (0 for X, 1 for Y, 2 for Z) otherwise.

  // Ensure that the variable types have been computed at this point
  if (m_variables.size() == 0) computeVariableTypes();

  for (unsigned int type = 0; type < m_variables.size(); ++type)
    if (m_variables[type] == kTagWeight) return int(type);
  return -1;
}

//________________________________________________________________________________
std::vector<double>
CalibrationDataHistogramContainer::getBinBoundaries(unsigned int vartype) const
{
  // Retrieve the bin boundaries for the specified variable type (which should be a CalibrationParametrization enum).
  // An empty vector will be returned if the specified variable is not actually used.

  // Ensure that the variable types have been computed at this point
  if (m_variables.size() == 0) computeVariableTypes();

  // Check whether the variable type is actually being used
  std::vector<double> boundaries;
  if (std::find(m_variables.begin(), m_variables.end(), vartype) == m_variables.end()) return boundaries;

  // use cached information if available
  std::map<unsigned int, std::vector<double> >::iterator it = m_binBoundaries.find(vartype);
  if (it != m_binBoundaries.end()) return it->second;

  // Retrieve the appropriate histogram axis
  if (! m_objResult) m_objResult = GetValue("result");
  const TH1* hobj = dynamic_cast<const TH1*>(m_objResult);
  const TAxis* axis = 0;
  if (m_variables[0] == vartype) axis = hobj->GetXaxis();
  else if (m_variables[1] == vartype) axis = hobj->GetYaxis();
  else axis = hobj->GetZaxis();

  // Retrieve the actual bin boundaries
  const TArrayD* bins = axis->GetXbins(); int nb = bins->GetSize();
  for (int b = 0; b < nb; ++b) boundaries.push_back(bins->At(b));

  m_binBoundaries[vartype] = boundaries;
  return boundaries;
}

//________________________________________________________________________________
int
CalibrationDataHistogramContainer::getEigenvectorReduction(unsigned int choice) const
{
  TObject* obj = GetValue("ReducedSets");
  if (! obj) return -1;
  TVectorT<double>* v = dynamic_cast<TVectorT<double>* >(obj);
  if (! (v && v->GetNoElements() > int(choice)) ) return -1;
  return int((*v)[choice]);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataMappedHistogramContainer                                                  //
//                                                                                          //
// The CalibrationDataMappedHistogramContainer class inherits from the                      //
// CalibrationDataHistogramContainer class. It covers the special case (for at least two    //
// dimensions) where the calibration is not done in a rectangular grid as would be implied  //
// by the use of a TH2 or TH3. Instead, the class implements a mapping from a set of        //
// general bins to bins on a given TH2 or TH3 axis. This generality implies that this class //
// in principle could be used also for the storage of higher-dimensional results. The only  //
// assumptions made are that                                                                //
//        - all bins have the same dimensions                                               //
//        - and the bins cover the 'mapped' dimensions completely, without any overlaps     //
// Necessarily, the added flexibility makes access slower (even if caching of the mapped    //
// bin is used).                                                                            //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataMappedHistogramContainer)
#endif

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::CalibrationDataMappedHistogramContainer(const char* name) :
  CalibrationDataHistogramContainer(name), m_lastBin(0)
{
}

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::~CalibrationDataMappedHistogramContainer()
{
}

//________________________________________________________________________________
void
CalibrationDataMappedHistogramContainer::computeVariableTypes() const
{
  // Compute the variable types for this container object.
  // The computation differs from that used for the parent CalibrationDataHistogramContainer
  // class, as also the 'mapped' variables (the variables that are mapped onto a single histogram
  // axis) need to be accounted for properly. This is handled as a special case.

  // cache pointer to central values histogram
  if (! m_objResult) m_objResult = GetValue("result");

  // histograms need a special treatment, as the coordinate titles are not actually stored
  // with the title itself, but instead moved off to the axis titles...
  const TH1* hobj = dynamic_cast<const TH1*>(m_objResult);

  // no protection against null pointers here -- should not be necessary?
  int dims = hobj->GetDimension();
  for (int dim = 0; dim < dims; ++dim) {
    const TAxis* axis = 0;
    switch (dim) {
    case 0: axis = hobj->GetXaxis(); break;
    case 1: axis = hobj->GetYaxis(); break;
    default: axis = hobj->GetZaxis();
    }
    std::string var(axis->GetTitle());
    if (var == "mapped") {
      // Special case: mapped variables, so make sure to specify the original variables (not the mapped ones).
      // Note that the code here assumes that the mapping is identical for all objects..
      for (unsigned int m = 0; m < m_mapped.size(); ++m) {
	int vartype = typeFromString(m_mapped[m]);
	// this check should never fail; therefore, bail out if this does happen
	assert (! (vartype < 0));
	m_variables.push_back((unsigned int)vartype);
      }
      // In this case, also flag _where_ in the resulting list of variables the mapping starts
      m_beginMapped = dim;
    } else {
      int vartype = typeFromString(var);
      if (vartype < 0) {
	// Only flag the issue but otherwise take no action (assume non-argument use of a semicolon)
	std::cerr << "in CalibrationDataMappedHistogramContainer::computeVariableTypes(): cannot construct variable type from name "
		  << var << std::endl;
      } else {
	m_variables.push_back((unsigned int)vartype);
      }
    }
  }

  // After doing this, we should always have a non-null vector!
  assert(m_variables.size() > 0);

  // Also compute the validity bounds for this calibration object
  const_cast<CalibrationDataMappedHistogramContainer*>(this)->checkBounds();
}


//________________________________________________________________________________
void
CalibrationDataMappedHistogramContainer::checkBounds()
{
  // Check the bounds of validity for this calibration object.
  // See the CalibrationDataHistogramContainer::checkBounds() method. The difference is
  // that the 'mapped' dimensions need to be handled separately (this is carried out by
  // looping over the mapped bins and inspecting each bin's validity bounds individually).
  // Note that extrapolation uncertainties are not covered at this point.

  const TH1* hist = dynamic_cast<const TH1*>(m_objResult);
  if (!hist) {
    std::cerr << "in CalibrationDataHistogramContainer::checkBounds(): object type does not derive from TH1" << std::endl;
    return;
  } else if (hist->GetDimension() + int(m_mapped.size()) - 1 != int(m_variables.size())) {
    std::cerr << "in CalibrationDataMappedHistogramContainer::checkBounds(): given number of variable types ("
	      << m_variables.size() << ") doesn't match (mapped) histogram dimension ("
	      << hist->GetDimension() + m_mapped.size() - 1 << ")!" << std::endl;
    return;
  }

  // Carry out the only cross-check that's possible for the binning: check that the dimensionality
  // for all bins matches the number of variables specified for the mapping
  for (unsigned int bin = 0; bin < m_bins.size(); ++bin)
    assert(m_bins[bin].getDimension() == m_mapped.size());

  for (unsigned int t = 0, t2 = 0; int(t) < hist->GetDimension(); ++t) {
    const TAxis* axis = 0;
    switch (t) {
    case 0: axis = hist->GetXaxis(); break;
    case 1: axis = hist->GetYaxis(); break;
    default: axis = hist->GetZaxis();
    }

    // Special case for the mapped dimension: here the only thing that can be done is to
    // cycle through all Bins and inspect the boundaries of each bin manually.
    if (t == m_beginMapped) {
      for (unsigned int mapped = 0; mapped < m_mapped.size(); ++mapped) {
	for (unsigned int bin = 0; bin < m_bins.size(); ++bin) {
	  double amax = m_bins[bin].getUpperBound(mapped), amin = m_bins[bin].getLowerBound(mapped);
	  if (bin == 0 || amax > m_upperBounds[m_variables[t2]]) m_upperBounds[m_variables[t2]] = amax;
	  if (bin == 0 || amin < m_lowerBounds[m_variables[t2]]) m_lowerBounds[m_variables[t2]] = amin;
	}
	++t2;
      }
    } else {
      // for (unsigned int t = 0; t < m_variables.size(); ++t) {
      //   if (m_variables[t] > m_upperBounds.size()) {
      // 	std::cerr << "in CalibrationDataHistogramContainer::checkBounds(): variable " << t << "type ("
      // 		  << m_variables[t] << "exceeds maximum type number (" << m_upperBounds.size() << ")!"
      // 		  << std::endl;
      // 	return;
      //   }
      // }
      double amax = axis->GetXmax(), amin = axis->GetXmin();
      if (amax < m_upperBounds[m_variables[t2]]) m_upperBounds[m_variables[t2]] = amax;
      if (amin > m_lowerBounds[m_variables[t2]]) m_lowerBounds[m_variables[t2]] = amin;
      ++t2;
    }
  }
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataMappedHistogramContainer::getResult(const CalibrationDataVariables& x,
						   double& result, TObject* obj, bool /* extrapolate */) const
{
  // Retrieve the central value for the given input variables. There are cases where
  // it may be useful to provide an alternative histogram rather than the original
  // one; in such cases (notably used with eigenvector variations) it is possible to
  // provide a pointer to this alternative histogram.
  // The method here differs from CalibrationDataHistogramContainer::getResult()
  // since histogram interpolation does not make sense for mapped bins.
  //
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative results histogram

  if (!obj) {
    if (! m_objResult) {
      // std::cout << "retrieving central value pointer" << std::endl;
      m_objResult = GetValue("result");
    }
    obj = m_objResult;
  }
  TH1* hist = dynamic_cast<TH1*>(obj);
  if (! hist) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);
  // find the relevant "global" bin and retrieve its contents
  result = hist->GetBinContent(findBin());

  return status;
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataMappedHistogramContainer::getStatUncertainty(const CalibrationDataVariables& x,
							    double& result) const
{
  // Retrieve the statistical uncertainty for the given input variables.
  //
  //     x:           input variables
  //     result:      result

  if (! m_objResult) {
    // std::cout << "retrieving central value pointer" << std::endl;
    m_objResult = GetValue("result");
  }
  TH1* hist = dynamic_cast<TH1*>(m_objResult);
  if (! hist) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);
  // find the relevant "global" bin and retrieve its contents
  result = hist->GetBinError(findBin());

  return status;
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataMappedHistogramContainer::getUncertainty(const std::string& unc,
							const CalibrationDataVariables& x,
							UncertaintyResult& result, TObject* obj) const
{
  // Retrieve the uncertainty for the given input variables.
  //
  //     unc:         keyword indicating requested source of uncertainty. This should
  //                  correspond to one of the histograms added explicitly as a systematic
  //                  uncertainty or the keyword "statistics" (statistical uncertainties are
  //                  accessed differently, see method getStatUncertainty()).
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative or cached histogram

  // treat statistical uncertainties separately (they are stored with the actual result)
  if (unc == "statistics") {
    double res;
    CalibrationStatus code = getStatUncertainty(x, res);
    if (code == Analysis::kError) return code;
    result.first =   res;
    result.second = -res;
    return code;
  }

  if (!obj) obj = GetValue(unc.c_str());
  TH1* hist = dynamic_cast<TH1*>(obj);
  if (! hist) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);
  // find the relevant "global" bin and retrieve its contents
  Int_t bin = findBin();
  result.first = hist->GetBinError(bin);
  result.second = hist->GetBinError(bin);

  return status;
}

//________________________________________________________________________________
int
CalibrationDataMappedHistogramContainer::getTagWeightAxis() const
{
  // Test whether this calibration object is one for "continuous" calibration
  // (this has some subtle consequences for the treatment of bin-to-bin correlations).
  // The return value will be -1 in case this is not a "continuous" calibration object,
  // and the axis number (0 for X, 1 for Y, 2 for Z) otherwise.

  for (unsigned int type = 0; type < m_variables.size(); ++type)
    if (m_variables[type] == kTagWeight) {
      int hist_type = int(type);
      return (hist_type > int(m_beginMapped)) ? hist_type - m_mapped.size() + 1 : hist_type;
    }
  return -1;
}

//________________________________________________________________________________
void
CalibrationDataMappedHistogramContainer::setMappedVariables(const std::vector<std::string>& variables)
{
  // Set (by hand) the variables that will be mapped onto a single histogram axis

  m_mapped = variables;
}

//________________________________________________________________________________
std::vector<std::string>
CalibrationDataMappedHistogramContainer::getMappedVariables() const
{
  // List which variables get mapped onto a single histogram axis

  return m_mapped;
}

//________________________________________________________________________________
unsigned int
CalibrationDataMappedHistogramContainer::addBin(const Bin& bin)
{
  // Add a bin to the present list
  // Note the absence of a -1 in the return value: this is because ROOT's histogram axes start counting from 1

  m_bins.push_back(bin);
  return m_bins.size();
}

//________________________________________________________________________________
unsigned int
CalibrationDataMappedHistogramContainer::getNMappedBins() const
{
  // Return the number of mapped bins
  // Note the absence of a -1 in the return value: this is because ROOT's histogram axes start counting from 1

  return m_bins.size();
}

//________________________________________________________________________________
Int_t
CalibrationDataMappedHistogramContainer::findMappedBin(double* x) const
{
  // Find the mapped bin corresponding to the variables used for the mapping

  if (m_bins[m_lastBin].contains(x)) return m_lastBin + 1; // First check quickly whether the last bin (cached) matches

  // Search the whole array for a match
  for (unsigned int bin = 0; bin < m_bins.size(); ++bin)
    if (m_bins[bin].contains(x)) {
      m_lastBin = bin;
      return m_lastBin + 1;
    }
  std::cerr << "CalibrationDataMappedHistogramContainer::findMappedBin(): unable to find bin for mapping variables:";
  for (unsigned int d = 0; d < m_mapped.size(); ++d) std::cerr << "\t" << x[d];
  std::cerr << std::endl;
  // -1 means invalid..
  return -1;
}

//________________________________________________________________________________
Int_t
CalibrationDataMappedHistogramContainer::findBin() const
{
  // Find the bin corresponding to the computed variables (the computation is assumed to have just
  // taken place and resulted in the m_vars array having been filled appropriately)

  Int_t mapped[3] = {0};
  const TH1* hist = dynamic_cast<const TH1*>(m_objResult);
  Int_t ndim = hist->GetDimension();
  // Push the mapped variables onto an array.
  // Since we derive from TH1 this need never be more than 3 elements long.
  for (unsigned int dim = 0; dim < (unsigned int) ndim; ++dim) {
    if (dim == m_beginMapped) {
      if ((mapped[dim] = findMappedBin(&(m_vars[dim]))) < 0) return -1;
    } else {
      const TAxis* axis = 0;
      switch (dim) {
      case 0: axis = hist->GetXaxis(); break;
      case 1: axis = hist->GetYaxis(); break;
      default: axis = hist->GetZaxis();
      }
      mapped[dim] = axis->FindFixBin((dim < m_beginMapped) ? m_vars[dim] : m_vars[dim+m_mapped.size()-1]);
    }
  }
  return hist->GetBin(mapped[0], mapped[1], mapped[2]);
}

//________________________________________________________________________________
std::vector<double>
CalibrationDataMappedHistogramContainer::getBinBoundaries(unsigned int vartype) const
{
  // Retrieve the bin boundaries for the specified variable type (which should be a CalibrationParametrization enum).
  // An empty vector will be returned if the specified variable is not actually used.

  // Ensure that the variable types have been computed at this point
  if (m_variables.size() == 0) computeVariableTypes();

  // use cached information if available
  std::map<unsigned int, std::vector<double> >::iterator it = m_binBoundaries.find(vartype);
  if (it != m_binBoundaries.end()) return it->second;

  // Check whether the variable type is actually being used
  std::vector<double> boundaries;
  int var = -1;
  for (unsigned int v = 0; v < m_variables.size(); ++v)
    if (m_variables[v] == vartype) var = (int) v;
  if (var == -1) return boundaries;

  // Retrieve the appropriate histogram axis
  if (! m_objResult) m_objResult = GetValue("result");
  const TH1* hobj = dynamic_cast<const TH1*>(m_objResult);

  if (var >= int(m_beginMapped) && var < int(m_beginMapped + m_mapped.size())) {
    // Special case of a mapped variable. In this case, test all Bins for their boundaries
    unsigned int mapped = var - m_beginMapped;
    for (unsigned int bin = 0; bin < m_bins.size(); ++bin) {
      double binBoundaries[2];
      binBoundaries[0] = m_bins[bin].getLowerBound(mapped);
      binBoundaries[1] = m_bins[bin].getUpperBound(mapped); 
      // Insert the bin boundaries, if not already present (the test is whether the relative difference
      // is smaller than 1e-8)
      if (boundaries.size() == 0) {
	boundaries.push_back(binBoundaries[0]); boundaries.push_back(binBoundaries[1]);
      } else {
	for (unsigned int ib = 0; ib < 2; ++ib) {
	  double newvalue = binBoundaries[ib];
	  bool done = false;
	  for (std::vector<double>::iterator it = boundaries.begin(); it != boundaries.end(); ++it) {
	    if (isNearlyEqual(newvalue, *it)) {
	      // consider this value to have been inserted already
	      done = true; break;
	    } else if (newvalue < *it) {
	      // the interesting case: insert the new value
	      boundaries.insert(it, newvalue);
	      done = true; break;
	    }
	  }
	  // last case: new value is larger than any of the values in the array so far
	  if (! done) boundaries.push_back(newvalue);
	}
      }
    }
  } else {
    // Normal case:
    unsigned int dim = (var < int(m_beginMapped)) ? var : var + 1 - m_mapped.size();
    const TAxis* axis = 0;
    switch (dim) {
    case 0: axis = hobj->GetXaxis(); break;
    case 1: axis = hobj->GetYaxis(); break;
    default: axis = hobj->GetZaxis();
    }
    // Retrieve the actual bin boundaries
    const TArrayD* bins = axis->GetXbins(); int nb = bins->GetSize();
    for (int b = 0; b < nb; ++b) boundaries.push_back(bins->At(b));
  }

  m_binBoundaries[vartype] = boundaries;
  return boundaries;
}

// Bin helper class methods

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataMappedHistogramContainer::Bin                                             //
//                                                                                          //
// This is a small nested class collecting 'mapped' bin information. Its purpose is mostly  //
// to store binning information in a structured way. The only relevant event-level method   //
// is the contains() method.                                                                //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataMappedHistogramContainer::Bin)
#endif

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::Bin::Bin():
  m_dimension(0), m_low(0), m_up(0)
{
  // Default constructor (for persistency)
}

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::Bin::Bin(unsigned int dimension, double* low, double* up):
  m_dimension(dimension)
{
  // Normal constructor, containing a full specification of the bin boundaries

  m_up  = new double[dimension];
  m_low = new double[dimension];
  for (unsigned int dim = 0; dim < dimension; ++dim) {
    m_up[dim]  = up[dim];
    m_low[dim] = low[dim];
  }
}

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::Bin::Bin(const CalibrationDataMappedHistogramContainer::Bin& other):
  m_dimension(other.m_dimension)
{
  m_up  = new double[m_dimension];
  m_low = new double[m_dimension];
  for (unsigned int dim = 0; dim < m_dimension; ++dim) {
    m_up[dim]  = other.m_up[dim];
    m_low[dim] = other.m_low[dim];
  }
}

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::Bin&
CalibrationDataMappedHistogramContainer::Bin::operator=(const CalibrationDataMappedHistogramContainer::Bin& other)
{
  if (this != &other) {
    m_dimension = other.m_dimension;
    delete[] m_up;
    delete[] m_low;
    m_up  = new double[m_dimension];
    m_low = new double[m_dimension];
    for (unsigned int dim = 0; dim < m_dimension; ++dim) {
      m_up[dim]  = other.m_up[dim];
      m_low[dim] = other.m_low[dim];
    }
  }
  return *this;
}

//________________________________________________________________________________
CalibrationDataMappedHistogramContainer::Bin::~Bin()
{
  delete[] m_up;
  delete[] m_low;
}

//________________________________________________________________________________
bool
CalibrationDataMappedHistogramContainer::Bin::contains(double* x) const
{
  // Determine whether the given set of variables is within the bin boundaries.

  for (unsigned int dim = 0; dim < m_dimension; ++dim)
    if (x[dim] < m_low[dim] || x[dim] > m_up[dim]) return false;
  return true;
}

//________________________________________________________________________________
double
CalibrationDataMappedHistogramContainer::Bin::getUpperBound(unsigned int dim) const
{
  // Return the upper bound for the specified dimension
  return m_up[dim];
}

//________________________________________________________________________________
double
CalibrationDataMappedHistogramContainer::Bin::getLowerBound(unsigned int dim) const
{
  // Return the lower bound for the specified dimension
  return m_low[dim];
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataFunctionContainer                                                         //
//                                                                                          //
// The CalibrationDataFunctionContainer class inherits from the CalibrationDataContainer    //
// abstract class. It covers the cases where the relevant information is presented in       //
// parametrised form.                                                                       //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(CalibrationDataFunctionContainer)
#endif

//________________________________________________________________________________
CalibrationDataFunctionContainer::CalibrationDataFunctionContainer(const char* name) :
  CalibrationDataContainer(name), m_objStatistics(0)
{
  // Default constructor

  // Reset the validity bounds to reflect 'no bounds'.
  m_lowerBounds.clear();
  m_lowerBounds.resize(maxParameters, -std::numeric_limits<double>::max());
  m_lowerBounds[kPt] = m_lowerBounds[kAbsEta] = 0;
  m_upperBounds.clear();
  m_upperBounds.resize(maxParameters, std::numeric_limits<double>::max());
  // and do the same for the validity bounds associated with extrapolation uncertainties
  // (this should anyway not be relevant for function containers)
  m_lowerBoundsExtrapolated.clear();
  m_lowerBoundsExtrapolated.resize(maxParameters, -std::numeric_limits<double>::max());
  m_lowerBoundsExtrapolated[kPt] = m_lowerBoundsExtrapolated[kAbsEta] = 0;
  m_upperBoundsExtrapolated.clear();
  m_upperBoundsExtrapolated.resize(maxParameters, std::numeric_limits<double>::max());
}

//________________________________________________________________________________
CalibrationDataFunctionContainer::~CalibrationDataFunctionContainer()
{
}

//________________________________________________________________________________
void
CalibrationDataFunctionContainer::computeVariableTypes() const
{
  // Determine which variable types are to be used.
  // This needs to be done only once per calibration object, as the results will be
  // cached (even if not persistified).
  // This method should normally only be used internally.

  if (! m_objResult) m_objResult = GetValue("result");

  std::string title(m_objResult->GetTitle());
  std::string::size_type pos = title.find(";");
  while (pos != std::string::npos && pos != title.size()) {
    title = title.substr(pos+1);
    pos = title.find(";");
    std::string var = title.substr(0, pos);
    int vartype = typeFromString(var);
    if (vartype < 0) {
      // Only flag the issue but otherwise take no action (assume non-argument use of a semicolon)
      std::cerr << "in CalibrationDataFunctionContainer::computeVariableTypes(): cannot construct variable type from name "
		<< var << std::endl;
    } else {
      m_variables.push_back((unsigned int)vartype);
    }
  }

  // After doing this, we should always have a non-null vector!
  assert(m_variables.size() > 0);
}

// result retrieval

//________________________________________________________________________________
CalibrationStatus
CalibrationDataFunctionContainer::getResult(const CalibrationDataVariables& x,
					    double& result, TObject* obj, bool /* extrapolate */) const
{
  // Retrieve the central value for the given input variables. There are cases where
  // it may be useful to provide an alternative parametrisation rather than the original
  // one; in such cases it is possible to provide a pointer to this alternative parametrisation.
  //
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative results histogram
  if (!obj) {
    if (! m_objResult) m_objResult = GetValue("result");
    obj = m_objResult;
  }
  TF1* func = dynamic_cast<TF1*>(obj);
  if (! func) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);
  result = func->EvalPar(m_vars);

  return status;
}

// general uncertainty retrieval

//________________________________________________________________________________
CalibrationStatus
CalibrationDataFunctionContainer::getUncertainty(const std::string& unc,
						 const CalibrationDataVariables& x,
						 UncertaintyResult& result, TObject* obj) const
{
  // Retrieve the uncertainty for the given input variables.
  // Note that the uncertainties returned will be symmetrised.
  //
  //     unc:         keyword indicating requested source of uncertainty. This should
  //                  correspond to one of the parametrisations added explicitly as a systematic
  //                  uncertainty or the keyword "statistics" (statistical uncertainties are
  //                  accessed differently, see method getStatUncertainty()).
  //     x:           input variables
  //     result:      result
  //     obj:         pointer to alternative or cached parametrisation
  if (unc == "statistics") {
    // treat statistical uncertainties separately (they are stored with the actual result)
    double res;
    CalibrationStatus code = getStatUncertainty(x, res);
    if (code == Analysis::kError) return code;
    result.first =   res;
    result.second = -res;
    return code;
  }

  if (!obj) obj = GetValue(unc.c_str());
  TF1* func = dynamic_cast<TF1*>(obj);
  if (! func) return Analysis::kError;

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);

  // the "first" and "second" entries are filled with the
  // "positive" and "negative" uncertainties, respectively.
  // Note: no "negative" uncertainties implemented as yet!
  result.first = func->EvalPar(m_vars);
  result.second = -result.first;

  return status;
}

//________________________________________________________________________________
CalibrationStatus
CalibrationDataFunctionContainer::getStatUncertainty(const CalibrationDataVariables& x,
						     double& result) const
{
  // Retrieve the statistical uncertainty for the given input variables.
  // The model that is assumed here is that statistical uncertainties follow from
  // a fit of the function to other information, and that the parameter covariance matrix
  // resulting from the fit are stored in a TMatrixDSym object identified by the
  // keyword "statistics". The effect of a change of fit parameters is then used to
  // evaluate the change in function value at the given co-ordinates.
  //
  //     x:           input variables
  //     result:      result

  if (! m_objResult) m_objResult = GetValue("result"); // ensure that the requested objects exist
  TF1* func = dynamic_cast<TF1*>(m_objResult);
  if (! func) {
    // std::cerr << "... unable to retrieve the result" << std::endl;
    return Analysis::kError;
  }

  if (! m_objStatistics) m_objStatistics = GetValue("statistics");
  //  m_objStatistics->Dump();
  TMatrixTSym<double>* cov = dynamic_cast<TMatrixTSym<double>*>(m_objStatistics);
  if (! cov) {
    // std::cerr << "... unable to retrieve the covariance matrix" << std::endl;
    return Analysis::kError;
  }

  // select the relevant kinematic variables
  CalibrationStatus status = computeVariables(x);

  // use a large value for "eps": this multiplies the uncertainties that
  // are expected to be associated with the parameters. Choosing a large
  // value expresses the fact that we are not primarily interested in the
  // parabolic behaviour at the minimum
  // const Double_t eps = 1.0;
  // test: set to 0.5
  const Double_t eps = 0.5;

  int npar = func->GetNpar();
  if (npar == 0) {
    result = 0.;
    return status;
  }

  TMatrixT<double> gradients(npar,1);
  //  std::cout << "parametricVariance: gradients:";
  for (int ipar = 0; ipar < npar; ++ipar) {
    gradients(ipar,0) = func->GradientPar(ipar, m_vars, eps);
    // std::cout << " " << gradients(ipar,0);
  }
  //  std::cout << std::endl;

  // carry out the matrix multiplication
  TMatrixT<double> gradientsTransposed(TMatrixT<double>::kTransposed, gradients);
  //  std::cout << "parametricVariance: transposed gradients:";
  //  for (int ipar = 0; ipar < npar; ++ipar)
  //    std::cout << " " << gradients(0,ipar);
  //  std::cout << std::endl;
  TMatrixT<double> tmp1(*cov, TMatrixT<double>::kMult, gradients);
  //  std::cout << "parametricVariance: cov * gradients:";
  //  for (int ipar = 0; ipar < npar; ++ipar)
  //    std::cout << " " << tmp1(ipar,0);
  TMatrixT<double> tmp2(gradientsTransposed, TMatrixT<double>::kMult, tmp1);

  result = TMath::Sqrt(tmp2(0,0));

  return status;
}

//________________________________________________________________________________
bool
CalibrationDataContainer::isNearlyEqual(double a, double b) {
  // Simple utility function testing whether two double values are sufficiently similar.
  // The test carried out is on their relative difference, which should be within a given tolerance.

  static const double tolerance = 1.e-8;

  double diff = a - b;
  double ref = std::fabs(a) + std::fabs(b);
  return (ref == 0 || std::fabs(diff) < tolerance*ref);
}
