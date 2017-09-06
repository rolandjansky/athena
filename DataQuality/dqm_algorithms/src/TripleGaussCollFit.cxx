/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*** Collisions monitoring algorithm by Tudor Costin & Peter Onyisi 

Revision history:

v1.1a (TC): 

-multLB_writeTags() will now query the bins to get lower and upper good LBs 
instead of assuming a 1-1 correspondence between bins and LBs
-added a couple of #ifdef statements:
  TC_WORKBENCH  - when set, alg. uses a different heade in local mode 
                  This is used for testing purposes ONLY and should never 
		  be used in the live environment.

  PARAM_IS_AREA - when set, the three-Gaussian formula is defined so that the
                  parameters multiplying the Gaussian exponents represent the 
		  integrated area under the Gaussian; otherwise, they
		  represent the height of the peaks.
		 
v1.1 (TC):

-improved code to pick starting height of Gaussians. Will now query the 1D
histogram for given ranges of ns (was: bin ranges)
-starting zeros are added to the output LB_(#S)_(#F) to improve formatting
-Mean_zero_gaus parameter will now translate the entire function, not just middle peak
-Changed names of some configurable parameters to reflect this.
-All parameters except the heights/areas of gaussians are now fixed by default
-Reasonable(?) limits imposed in case the user overrides the above fixing.  
  (cannot be overriden at present)

v1.0a,b(TC)

-various bug fixes.

v1.0 (TC): 
-first release with multiple LB functionality
-This should actually be labelled v2.0, but too late now


*/

#ifndef TC_WORKBENCH
#include "dqm_algorithms/TripleGaussCollFit.h"
#include <dqm_core/AlgorithmManager.h>
#include <dqm_core/exceptions.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include "ers/ers.h"

namespace {
    dqm_algorithms::TripleGaussCollFit three_gauss_fit( "default" );  
}

#else
#include "TripleGaussCollFit_standalone.h"
#endif

#include "TH2.h"
#include "TClass.h" //shocking that this doesn't fall in by now
#include <iostream>
#include "TMath.h"



//for some reason, FindBin() is not a const method, so I have to discard the const part. Sigh.
Double_t getStartValue(TH1* thehist, Double_t minx, Double_t maxx) {

  Double_t result = 0.0;
  int minidx = thehist->FindBin(minx), maxidx = thehist->FindBin(maxx);
  for (int idx = minidx; idx <= maxidx; idx++) {
    Double_t val = thehist->GetBinContent(idx);
    if (val > result)
      result = val;
  }

  return result;
}


const char* fmt = "%0*d";

void multLB_writeTags(std::vector<int>* goodLB, dqm_core::Result* result, TH1* thehist) {

  /* This method assumes that the entries in goodLB are in ascending order!! */

  char buf[10]; 
  std::string lb_str = "LB_";
  int firstLB = (*goodLB)[0], offset = 1; 
  unsigned int c = 1, sz = goodLB->size(), psz = (unsigned int) TMath::Log10(sz)+1;
  Double_t lo, hi;
	
  while (c < sz) {
    if ((*goodLB)[c] == firstLB + offset) 
      offset++;
	  
    else {
      std::string tagname = lb_str;
      //find the low LB
      
      lo = thehist->GetXaxis()->GetBinLowEdge(firstLB);
      //out << "bin " << firstLB << " lower edge is " << lo << std::endl;

      if (lo == TMath::Floor(lo))
	sprintf(buf, fmt, psz, (Int_t)lo);
      else 
	sprintf(buf, fmt, psz, (Int_t)(TMath::Floor(lo))+1);

      tagname +=  buf;
      tagname += "_";

      hi = thehist->GetXaxis()->GetBinUpEdge(firstLB+offset-1);
      //out << "bin " << firstLB+offset-1 << " high edge is " << hi << std::endl;

      if (hi == TMath::Floor(hi))
	sprintf(buf, fmt, psz, (Int_t)(TMath::Floor(hi))-1);
      else
	sprintf(buf, fmt, psz, (Int_t)TMath::Floor(hi));

      //sprintf(buf, fmt, psz, firstLB+offset-1);
      tagname += buf;
      result->tags_ [tagname] = 1.0;
      firstLB = (*goodLB)[c];
      offset = 1;
    }

    c++;  //I've always wanted to write this
  }

  lo = thehist->GetXaxis()->GetBinLowEdge(firstLB);
      //out << "bin " << firstLB << " lower edge is " << lo << std::endl;

  if (lo == TMath::Floor(lo))
    sprintf(buf, fmt, psz, (Int_t)lo);
  else 
    sprintf(buf, fmt, psz, (Int_t)(TMath::Floor(lo))+1);
  //sprintf(buf, fmt, psz, firstLB);

  lb_str += buf;
  lb_str += "_";
  hi = thehist->GetXaxis()->GetBinUpEdge(firstLB+offset-1);
  if (hi == TMath::Floor(hi))
    sprintf(buf, fmt, psz, (Int_t)(TMath::Floor(hi))-1);
  else
    sprintf(buf, fmt, psz, (Int_t)TMath::Floor(hi));
  //sprintf(buf, fmt, psz, (*goodLB)[c-1]);
  lb_str += buf;
  result->tags_ [lb_str] = 1.0;

}

/****

The following parameters are held fixed by default (whether user specified or using built-in values):
  -Mean_zero_gaus      (default is 0.0)
  -Mean_offzero_gaus   (default is 23.2, obtained by empirical fits. Should be overriden for certain detectors!)
  -Width_all           (default is 4.3, also obtained empirically)

If they are allowed to float, the following limits are imposed. This is currently not configurable.
  -Mean_zero_gaus:     specified or default value, plus or minus 5
  -Mean_offzero_gaus:  specified or default value, plus or minus 10
  -Width_all:          between 1.0 and 10.0
**/

typedef std::map<std::string, double> param_map;
typedef std::map<std::string, double> err_map;

#define PARAM_IS_AREA   //if this is not in, the constants multiplying the Gaussians are defined as max_ht, not integr. area 

namespace dqm_algorithms {

  TripleGaussCollFit::TripleGaussCollFit(const std::string& name) : m_name(name) {
 
    //Register our instance with the Alg. Manager
    m_gaus3_fn = 0;
    dqm_core::AlgorithmManager::instance().registerAlgorithm( "Gaus3_" + m_name +"_Fit", this );

  }

  TripleGaussCollFit::~TripleGaussCollFit() {

    if (m_gaus3_fn != 0) {
      delete m_gaus3_fn;
      m_gaus3_fn = 0;
    }

  } 

  TripleGaussCollFit* TripleGaussCollFit::clone() {
    return new TripleGaussCollFit(m_name);
  }

// This method probably need not be implemented, but I should have it anyway...
  void TripleGaussCollFit::printDescription(std::ostream& out) {

    out << "Gaus3_"+ m_name +"_Fit: specialized fit for the Tile timining difference plot." << std::endl;
    out << "Current parameter values: " << std::endl;

    if  (m_gaus3_fn != 0)
      for (int i=0; i < m_gaus3_fn->GetNpar(); i++) 
	out << " Param. " << i << ": " << m_gaus3_fn->GetParName(i) << " , current value: " 
		  << m_gaus3_fn->GetParameter(i) << std::endl; 

    else 
      out << "    execute() has not been called" << std::endl;

    out << "Parameter list: " << std::endl
	      << " allowFitMean: 1 allows fitting for a shift in the entire function around zero by up to 5ns" << std::endl
	      << " allowFitMeanNonzeroPeaks: 1 allows fitting of the distance between center and off-center Gaussians" << std::endl
	      << " allowFitResolution: 1 allows fitting the (shared) width of the Gaussians" << std::endl
	      << " reportSignificance: 1 reports the central peak significance for each LB as a tag" << std::endl
              << " meanZeroPeak: starting or fixed mean of the Gaussian centered at zero" << std::endl
	      << " meanNonZeroPeaks: starting or fixed mean of the other Gaussians" << std::endl      
	      << " resolutionAll: starting or fixed value of the width of the Gaussians" << std::endl       
	      << " minSignificance: number of events required for checks to be valid, undefined if not set." << std::endl
              << " centralPeakMinSignificance: significance of central peak required for a LB to pass threshold; default is 3.0" << std::endl
	      << " minEvents: number of events required to attempt a fit; default is 5." << std::endl ;
    
    out << "Currently, no thresholds are defined." << std::endl;

  }

  dqm_core::Result* TripleGaussCollFit::execute (const std::string& /* s */, const TObject& object, const dqm_core::AlgorithmConfig& cfg) {
  
    TH1* histogram;
    if(object.IsA()->InheritsFrom("TH1")) //or this can be done via dynamic casting
      { 
	histogram = (TH1*) &(object);
	if (histogram->GetDimension() > 2 ){
	  throw dqm_core::BadConfig( ERS_HERE, m_name, "called with histogram of dimension > 2"  );
	}
      }
    else 
      throw dqm_core::BadConfig( ERS_HERE, m_name, "called with object that does not inherit from TH1" );

    //Setup our TF1: find the binwidth
    Double_t binwidth = 0; 
    if (histogram->GetDimension() == 1)   //running in 1D mode
      if (histogram->GetNbinsX() >= 1) {
	binwidth = histogram->GetBinWidth(1);
      } else {
	throw dqm_core::BadConfig( ERS_HERE, m_name, "called with histogram with no bins");
      }

    else {   //running in 2D mode {
      TH1* proj =  ((TH2*) histogram)->ProjectionY("", 0, 0);
      if (proj->GetNbinsX() >= 1) {
	binwidth = proj ->GetBinWidth(1);
      } else {
	throw dqm_core::BadConfig( ERS_HERE, m_name, "called with histogram with no bins");
      }
    }
    #ifdef PARAM_IS_AREA
    m_gaus3_fn = new TF1("gaus3", Form("[3]/[2]/2.5066*%f*exp(-0.5*((x-[0]+[1])/[2])**2) + [4]/[2]/2.5066*%f*exp(-0.5*((x-[0])/[2])**2) + [5]/[2]/2.5066*%f*exp(-0.5*((x-[0]-[1])/[2])**2)", binwidth, binwidth, binwidth), -100, 100);
    #else
    m_gaus3_fn = new TF1("gaus3", "[3]*exp(-0.5*((x-[0]+[1])/[2])**2) + [4]*exp(-0.5*((x-[0])/[2])**2) + [5]*exp(-0.5*((x-[0]-[1])/[2])**2)", -100, 100);
    #endif
    m_gaus3_fn->SetParName(0, "Mean_zero_gaus");	//mean of gaussian centered at zero, should be close to zero
    m_gaus3_fn->SetParName(1, "Mean_offzero_gaus"); //mean of other two gaussians, should be close to 20
    m_gaus3_fn->SetParName(2, "Width_all"); //shared width, should be about 5
    m_gaus3_fn->SetParName(3, "N_negative_gaus"); //normalization for negative-centered gaussian
    m_gaus3_fn->SetParName(4, "N_zero_gaus"); //normalization for zero-centered gaussian
    m_gaus3_fn->SetParName(5, "N_positive_gaus"); //normalization for positive-centered gaussian 

    Double_t resolutionAll, meanZeroPeak, meanNonzeroPeaks, minEvents;
    param_map mymap = cfg.getParameters();

    meanZeroPeak = dqm_algorithms::tools::GetFirstFromMap("meanZeroPeak", mymap, 0.0);
    meanNonzeroPeaks = dqm_algorithms::tools::GetFirstFromMap("meanNonzeroPeaks", mymap, 23.2);
    resolutionAll = dqm_algorithms::tools::GetFirstFromMap("resolutionAll", mymap, 4.57);
    minEvents = dqm_algorithms::tools::GetFirstFromMap("minEvents", mymap, 5.0);

    //we DO NOT allow fits by default. If the parameter is not found, the retval is 0
    if ( dqm_algorithms::tools::GetFirstFromMap("allowFitMean", mymap, 0) == 1 ) {

      //according to Peter, this can behave strangely, so we don't allow too much horizontal translation
      m_gaus3_fn->SetParameter(0, meanZeroPeak);
      m_gaus3_fn->SetParLimits(0, meanZeroPeak-5, meanZeroPeak+5);
    }
    else
      m_gaus3_fn->FixParameter(0, meanZeroPeak);
    
    if ( dqm_algorithms::tools::GetFirstFromMap("allowFitMeanNonzeroPeaks", mymap, 0) == 1 ) {
      m_gaus3_fn->SetParameter(1, meanNonzeroPeaks);
      m_gaus3_fn->SetParLimits(1, meanNonzeroPeaks-10, meanNonzeroPeaks+10);
    }
    else 
      m_gaus3_fn->FixParameter(1, meanNonzeroPeaks);
     
    if ( dqm_algorithms::tools::GetFirstFromMap("allowFitResolution", mymap, 0) == 1 ) {
      m_gaus3_fn->SetParameter(2, resolutionAll);
      m_gaus3_fn->SetParLimits(2, 1, 10); 
    }
    else {
      m_gaus3_fn->FixParameter(2, resolutionAll);

    }
    // get some starting values for the histogram

    dqm_core::Result* result = new dqm_core::Result(); 
    
    if (histogram->GetDimension() == 1) {
      
      fitSingle(histogram);
      // double minSig = dqm_algorithms::tools::GetFirstFromMap("minSignificance", mymap, 0);

      try {
	
	result->tags_ = dqm_algorithms::tools::GetFitParams(m_gaus3_fn);
	err_map paramErrors = dqm_algorithms::tools::GetFitParamErrors(m_gaus3_fn);
	for (err_map::const_iterator peit = paramErrors.begin(); peit != paramErrors.end(); ++peit) {
	  result->tags_[(*peit).first + " Error"] = (*peit).second;
	}

      } 
      catch ( dqm_core::Exception & ex) {
	throw dqm_core::BadConfig( ERS_HERE, m_name, ex.what(), ex);
      }
    }
    else { //working with a 2D histogram

      int nxbin = histogram->GetNbinsX();
      Double_t minsig = dqm_algorithms::tools::GetFirstFromMap("centralPeakMinSignificance", mymap, 3.0);

      bool writeSig = (dqm_algorithms::tools::GetFirstFromMap("reportSignificance", mymap, 0) == 1);
      
      
      unsigned int psz = (unsigned int) TMath::Log10(nxbin)+1;

      std::vector<int> goodLB;
      char buf[10];
      const std::string sig_str = "SIG_LB_";
      
      for (int lb = 0; lb < nxbin; lb++) {
    
	sprintf(buf, fmt, psz, lb); //to avoid possible leaks (?) - should see how DQ framework behaves
	TH1D* proj = ((TH2*) histogram)->ProjectionY(buf, lb, lb);
	Double_t good_entries = proj->GetEntries() - proj->GetBinContent(0) - proj->GetBinContent(proj->GetNbinsX()+1); 
	if (good_entries < minEvents) {
	  if (writeSig)
	    result->tags_[sig_str+buf] = 0;
	  //delete proj; //// possible HACK, NOT SURE if needed
	  continue; //nothing to fit here, move along
	}

	// I probably need to reset parameters 0-2 if they are not fixed
	if ( dqm_algorithms::tools::GetFirstFromMap("allowFitMeanZeroPeak", mymap, 0) == 1 )
	  m_gaus3_fn->SetParameter(0, meanZeroPeak);
       
	if ( dqm_algorithms::tools::GetFirstFromMap("allowFitMeanNonzeroPeaks", mymap, 0) == 1 )
	  m_gaus3_fn->SetParameter(1, meanNonzeroPeaks);
    
	if ( dqm_algorithms::tools::GetFirstFromMap("allowFitResolution", mymap, 0) == 1 )
	  m_gaus3_fn->SetParameter(2, resolutionAll);
    
	fitSingle(proj);

#ifdef TC_WORKBENCH
	result->histos_.push_back((TH1D*)proj->Clone());
#endif

	if (m_gaus3_fn->GetParameter(4) == 0) {
	  if (writeSig)
	    result->tags_[sig_str+buf] = 0;
	  continue;
	}
	Double_t sig = fabs(m_gaus3_fn->GetParameter(4) / m_gaus3_fn->GetParError(4));
	if (writeSig)
	  result->tags_[sig_str+buf] = sig;

	if (sig > minsig) 
	  goodLB.push_back(lb); //this is grossly non-optimal

	//delete proj;
	
      } //end loop over lumi blocks

      if (goodLB.size() != 0) 
	multLB_writeTags(&goodLB, result, histogram);

      //goodLB->Set(count);
      // result->object_ = goodLB;  this needs to be done at construction time apparently
    }
    return result; 
  }


#ifdef PARAM_IS_AREA
#define ADJUST_VAL startVal = startVal * hist1D->GetBinWidth(1) / m_gaus3_fn->GetParameter(2) / 2.5066
#else
#define ADJUST_VAL
#endif 

  void TripleGaussCollFit::fitSingle(TH1* hist1D) {
     
    Double_t startVal;
    const Double_t window = 5.0;

    Double_t offset_all = m_gaus3_fn->GetParameter(0);
    Double_t offset_nz  = m_gaus3_fn->GetParameter(1);

    startVal = getStartValue(hist1D, offset_all-offset_nz-window, offset_all-offset_nz+window); ADJUST_VAL; 
    if (startVal == 0.0)
      m_gaus3_fn->FixParameter(3, 0.0);
    else {
      m_gaus3_fn->SetParameter(3, startVal);
      // DO NOT do something like this if we are using areas!!
      // m_gaus3_fn->SetParLimits(3, 0, 5 * startVal);  
    }

    startVal = getStartValue(hist1D, offset_all-window, offset_all+window); ADJUST_VAL; 
    if (startVal == 0.0)
      m_gaus3_fn->FixParameter(4, 0.0);
    else {
      m_gaus3_fn->SetParameter(4, startVal);
      //   m_gaus3_fn->SetParLimits(4, 0, 100 * startVal); 
    }
    startVal = getStartValue(hist1D, offset_all+offset_nz-window , offset_all+offset_nz+window); ADJUST_VAL; 
    if (startVal == 0.0)
      m_gaus3_fn->FixParameter(5, 0.0);
    else {
      m_gaus3_fn->SetParameter(5, startVal);
      //  m_gaus3_fn->SetParLimits(5, 0, 5 * startVal);
    }

    /*  This is the old and obsolete method to extract limits
    Double_t startVal = getStartValue(hist1D, 46, 58);   //was 51-53
    if (startVal == 0.0)
	m_gaus3_fn->FixParameter(3, 0.0);
      else {
	m_gaus3_fn->SetParameter(3, startVal);
        m_gaus3_fn->SetParLimits(3, 0, 5 * startVal);   //not sure if this sort of bound is necessary, but who knows...
      }
      startVal = getStartValue(hist1D, 70, 82); // was 75-77
      if (startVal == 0.0)
	m_gaus3_fn->FixParameter(4, 0.0);
      else {
	m_gaus3_fn->SetParameter(4, startVal);
	m_gaus3_fn->SetParLimits(4, 0, 100 * startVal);  //for some reason this seems to want more
      }
      startVal = getStartValue(hist1D, 93, 106); //was 98-101
      if (startVal == 0.0)
	m_gaus3_fn->FixParameter(5, 0.0);
      else {
	m_gaus3_fn->SetParameter(5, startVal);
	m_gaus3_fn->SetParLimits(5, 0, 5 * startVal);
      }  */
      
      hist1D->Fit(m_gaus3_fn, "QN");

  }

} //close namespace
