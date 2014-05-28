/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  Algorithm: AlgHLTMETComponents does the following
 *             Check if any sub-detector component is empty. Empty means the following:
 *             overflow or underflow bins are filled or integral of histogram is zero.
 *  Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 *  Date     : March 2010
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/HLTMETComponents.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>


#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::HLTMETComponents myInstance;

dqm_algorithms::HLTMETComponents::HLTMETComponents()

{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("AlgHLTMETComponents", this);
}

dqm_algorithms::HLTMETComponents * 
dqm_algorithms::HLTMETComponents::clone()
{
  
  return new HLTMETComponents();
}


dqm_core::Result *
dqm_algorithms::HLTMETComponents::execute(const std::string & name,
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config)
{  
  TH2 *histogram;
  
  if( object.IsA()->InheritsFrom( "TH2" ) ) {
    histogram = (TH2*)&object;
    int dimension = histogram->GetDimension();
    if (dimension != 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension != 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH2" );
  }
///////////////////////////////////////////////////////////////////////////////////////////
  
  int nXBins = histogram -> GetNbinsX(); // components
  if(nXBins > 25) {
    nXBins = 25;
  }
  const int nYBins = histogram -> GetNbinsY(); // SumEt per sampling


  // read thresholds
  int colflags[4];
  colflags[0] = static_cast<int>(dqm_algorithms::tools::GetFirstFromMap("METYellowMin", config.getParameters(), 1));
  colflags[1] = static_cast<int>(dqm_algorithms::tools::GetFirstFromMap("METYellowMax", config.getParameters(), nXBins));
  colflags[2] = static_cast<int>(dqm_algorithms::tools::GetFirstFromMap("METRedMin",    config.getParameters(), 1));
  colflags[3] = static_cast<int>(dqm_algorithms::tools::GetFirstFromMap("METRedMax",    config.getParameters(), nXBins));
  // read options
  bool doflags[2];
  doflags[0] = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("DoYellowFlag", config.getParameters(), 1));
  doflags[1] = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("DoRedFlag",    config.getParameters(), 1));

  dqm_core::Result* result = new dqm_core::Result();
  colflags[0] = ((colflags[0] > 0) && (colflags[0] <= nXBins)) ? colflags[0] : 1;
  colflags[1] = ((colflags[1] >= colflags[0]) && (colflags[1] <= nXBins)) ? colflags[1] : nXBins;
  colflags[2] = ((colflags[2] > 0) && (colflags[2] <= nXBins)) ? colflags[2] : 1;
  colflags[3] = ((colflags[3] >= colflags[2]) && (colflags[3] <= nXBins)) ? colflags[3] : nXBins;
  bool yellowFlagComp = false, redFlagComp = false;
  int ytot = 0, ycnt = 0, rtot = 0, rcnt = 0;
  
  // yellow flag
  float epsilon = 1.e-3;
  if(doflags[0]) {
    for( int j = colflags[0]; j <= colflags[1]; j++ ) {
      TString binLabel = histogram->GetXaxis()->GetBinLabel(j);
      TH1D *hprojy = histogram->ProjectionY(Form("hpy1yf%02d_%s",j,binLabel.Data()),j,j+1,"egoff");
      if(hprojy) {
        float uflow = fabs(hprojy->GetBinContent(0));
        float oflow = fabs(hprojy->GetBinContent(nYBins+1));
        float btotal = hprojy->Integral();

        // total bins requested for yellow flags
        ytot++;
        // flag true if overflow/underflow has entries or if histo is empty
        // denoting missing component
        if(((uflow > epsilon) || (oflow > epsilon)) && (btotal < epsilon)) {
          yellowFlagComp = true;
          // count bins flagged as yellow
          ycnt++;
        } // if uflow
        delete hprojy; hprojy = 0;
      } // if hprojy
    } // for
  } // if doflags[0]

  // red flag
  if(doflags[1]) {
    for( int j = colflags[2]; j <= colflags[3]; j++ ) {
      TString binLabel = histogram->GetXaxis()->GetBinLabel(j);
      TH1D *hprojy = histogram->ProjectionY(Form("hpy1rf%02d_%s",j,binLabel.Data()),j,j+1,"egoff");
      if(hprojy) {
        float uflow = fabs(hprojy->GetBinContent(0));
        float oflow = fabs(hprojy->GetBinContent(nYBins+1));
        float btotal = hprojy->Integral();
        rtot++;
        // flag true if overflow/underflow has entries or if histo is empty
        // denoting missing component
        if(((uflow > epsilon) || (oflow > epsilon)) && (btotal < epsilon)) {
          redFlagComp = true;
          rcnt++;
        } // if uflow
        delete hprojy; hprojy = 0;
      } // if hprojy
    } // for
  } // if doflags[1]

  // set results (default green, 
  result->status_ = dqm_core::Result::Green;
  if(doflags[0] && yellowFlagComp) {
    result->status_ = dqm_core::Result::Yellow;
  }
  if(doflags[1] && redFlagComp) {
    result->status_ = dqm_core::Result::Red;
  }
  result->tags_["YellowMETComponents"] = ycnt; 
  result->tags_["RedMETComponents"] = rcnt;
 
///////////////////////////////////////////////////////////////////////////////////////////
  
  return result;
}

void
dqm_algorithms::HLTMETComponents::printDescription(std::ostream& out)
{
  
  out << "HLT MET Components: Of the 25 components (24 calo samplings and 1 muon component of MET) ";
  out << "Check if any sub-detector component is empty. Empty means any of the following:";
  out << " overflow/underflow bins are filled, or integral of histogram is zero. " << std::endl;
  out<<  "Each component (25 of them) are bins on abscissa and the 1d histograms represent the quantity (SumEt)" << std::endl;
  
}

