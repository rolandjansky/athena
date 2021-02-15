/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*! \file BinContentComp.cxx compares bins of histogram wrt to reference histogram and counts number of bins N Sigma away from ref; returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinContentComp.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TClass.h>
#include <math.h>
#include <ers/ers.h>


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::BinContentComp myInstance;


dqm_algorithms::BinContentComp::BinContentComp()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "BinContentComp", this );
}

dqm_algorithms::BinContentComp::~BinContentComp()
{
}

dqm_algorithms::BinContentComp * 
dqm_algorithms::BinContentComp::clone()
{
  return new BinContentComp();
}


dqm_core::Result *
dqm_algorithms::BinContentComp::execute(	const std::string & name, 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
{
  TH1 * histogram;
  TH1 * refhist;

  if(object.IsA()->InheritsFrom( "TH1" )) {
     histogram = (TH1*)&object;
     if (histogram->GetDimension() > 2 ){ 
       throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
     }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1");
  }
  
  double value=-99999;
  try {
  refhist = static_cast<TH1 *>( config.getReference() );
  }
  catch (dqm_core::BadConfig &ex ) {
  refhist=0;
    try {
     value = dqm_algorithms::tools::GetFirstFromMap( "Value", config.getParameters() );
    }
    catch( dqm_core::Exception & ex ) {
      throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
    }
  } 
  if ( ! refhist ) {
    try {
     value = dqm_algorithms::tools::GetFirstFromMap( "Value", config.getParameters() );
    }
    catch( dqm_core::Exception & ex ) {
      throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
    }
  }  else {
 
    if (histogram->GetDimension() != refhist->GetDimension() ) {
      throw dqm_core::BadRefHist( ERS_HERE, name, "Dimension" );
    }
  
    if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
      throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
    }
}

  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }

  const TProfile* profile(nullptr);
  const TProfile2D* profile2D(nullptr);
  const double minBinEntries = dqm_algorithms::tools::GetFirstFromMap( "MinBinEntries", config.getParameters(), -1);
  if(minBinEntries > 0) {
   if (object.InheritsFrom("TProfile"))        profile   = dynamic_cast<const TProfile*>(&object);
   else if (object.InheritsFrom("TProfile2D")) profile2D = dynamic_cast<const TProfile2D*>(&object);
  }

  const bool ignorezero = (bool) dqm_algorithms::tools::GetFirstFromMap( "Ignore0", config.getParameters(), 0);
  const bool ignoreInputZero = (bool) dqm_algorithms::tools::GetFirstFromMap( "IgnoreInput0", config.getParameters(), 0);
  bool greaterthan = (bool) dqm_algorithms::tools::GetFirstFromMap( "GreaterThan", config.getParameters(), 0);
  bool lessthan = (bool) dqm_algorithms::tools::GetFirstFromMap( "LessThan", config.getParameters(), 0);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0);
  const int  maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  const bool normref = (bool) dqm_algorithms::tools::GetFirstFromMap( "NormRef", config.getParameters(), 0);
  const double maxdiffabs = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffAbs", config.getParameters(), -1); 
  const double maxdiffrel = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffRel", config.getParameters(), -1);
  const double fixerr = dqm_algorithms::tools::GetFirstFromMap( "FixedError", config.getParameters(), 0);
  const bool increferr = (bool) dqm_algorithms::tools::GetFirstFromMap( "IncludeRefError", config.getParameters(), 0);
  
  if (greaterthan && lessthan) {
    ERS_INFO("Both GreaterThan and LessThan parameters set: Will check for for both");
    greaterthan = false;
    lessthan = false;
  }
    
  double bin_threshold;
  double gthreshold;
  double rthreshold;
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "NSigma", config.getParameters() );
    rthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  

  int count = 0; 
  
  
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  
  dqm_core::Result* result = new dqm_core::Result();
  double refcont =0;
  TH1* resulthisto;
  if (histogram->InheritsFrom("TH2")) {
        resulthisto=(TH1*)(histogram->Clone());
  } else if (histogram->InheritsFrom("TH1")) {
        resulthisto=(TH1*)(histogram->Clone());
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  } 

     resulthisto->Reset();

   if (refhist && normref) {
      double ratio=histogram->GetEntries()/refhist->GetEntries();
      refhist->Scale(ratio);
   }

   int nSkippedBins(0);
  for ( int i = range[0]; i <= range[1]; ++i ) {
    for ( int j = range[2]; j <= range[3]; ++j ) {
      if (minBinEntries > 0) {
        int bin = histogram->GetBin(i, j);
        if (profile) {
          if (profile->GetBinEntries(bin) < minBinEntries) {
            ++nSkippedBins;
            continue;
          }
        } else if (profile2D) {
          if (profile2D->GetBinEntries(bin) < minBinEntries) {
            ++nSkippedBins;
            continue;
          }
        }
      }

     if ( ! refhist ){
	 refcont=value;
     } else {
        refcont = refhist->GetBinContent(i,j);
     }
     
     double histerr = histogram->GetBinError(i,j);
     double inputerr=0;
     

     if (increferr && refhist ) {
        double referr = refhist->GetBinError(i,j);
	inputerr = sqrt(pow(histerr,2)+pow(referr,2));
     } else {
        inputerr = histerr;
     }     
     
     if (fixerr) {
       inputerr = fixerr;
     }
     
      double inputcont = histogram->GetBinContent(i,j);
      double diff=inputcont - refcont;
      double reldiff=1;
      if(refcont!=0) reldiff=diff/refcont;
      else if(diff==0) reldiff=0;

      if (ignorezero && refcont==0) continue;
      if (ignorezero && !refhist && inputcont==0) continue;
      if (ignoreInputZero && inputcont==0) continue;

      if (inputerr !=0){
	double sigma=diff/inputerr;
	if (greaterthan && diff < 0. ) continue;  
	if (lessthan && diff > 0. ) continue;  
	
	if ( (fabs(sigma) > bin_threshold) && (fabs(diff) > maxdiffabs) && (fabs(reldiff) > maxdiffrel) ){
          resulthisto->SetBinContent(i,j,inputcont);
	  ++count;
	  if (publish && count<maxpublish){
            dqm_algorithms::tools::PublishBin(histogram,i,j,inputcont,result);
	  }
	}
      }
    }
  }
  
  if (value == -99999) {
      ERS_DEBUG(1, "Number of bins " << bin_threshold << " Sigma away from reference is " << count);
  }else {
      ERS_DEBUG(1, "Number of bins " << bin_threshold << " Sigma away from "<<value<<" is " << count);
  }

  ERS_DEBUG(1,  "Green threshold: "<< gthreshold << " bin(s);   Red threshold : " << rthreshold << " bin(s) ");
  
  
  result->tags_["NBins"] = count;
  result->tags_["NSkippedBins"] = nSkippedBins;
  result->object_ =  (boost::shared_ptr<TObject>)(TObject*)(resulthisto);

  if (gthreshold > rthreshold) {
     if ( count >= gthreshold ) {
        result->status_ = dqm_core::Result::Green;
     } else if ( count > rthreshold ) {
        result->status_ = dqm_core::Result::Yellow;
     } else {
        result->status_ = dqm_core::Result::Red;
     }
  } else {
     if ( count <= gthreshold ) {
        result->status_ = dqm_core::Result::Green;
     } else if ( count < rthreshold ) {
        result->status_ = dqm_core::Result::Yellow;
     } else {
        result->status_ = dqm_core::Result::Red;
     }
  }
  return result;

}
void
dqm_algorithms::BinContentComp::printDescription(std::ostream& out)
{
  
  out<<"BinContentComp: Checks number of bins N sigma away from reference histogram bin value or given Value\n"<<std::endl;
                                                                                                                                            
  out<<"Mandatory Parameter: NSigma: Number of sigma each bin must be within reference histogram value\n"<<std::endl;
  out<<"Mandatory Parameter (If no reference): Value: Value to compare each bin\n"<<std::endl;
  
  
  out<<"Mandatory Green/Red Threshold: NBins: number of bins N sigma away from reference histogram bin to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: Ignore0: Ignore bins which have zero entries in reference histogram"<<std::endl;
  out<<"Optional Parameter: IgnoreInput0: Ignore bins which have zero entries in tested histogram"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: ymin: minimum y range"<<std::endl;
  out<<"Optional Parameter: ymax: maximum y range\n"<<std::endl;
  out<<"Optional Parameter: GreaterThan: check only for bins which are GreaterThan average (set to 1)"<<std::endl; 
  out<<"Optional Parameter: LessThan: check only for bins which are LessThan average (set to 1)"<<std::endl; 
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
   out<<"Optional Parameter: NormRef: Normalize reference histogram to checked histogram statistics before checking bin contents (set to 1)"<<std::endl;
  out<<"Optional Parameter: MaxDiffAbs: test fails if NBins more than NSigma away and NBins more than MaxDiffAbs (absolut difference) away"<<std::endl; 
  out<<"Optional Parameter: MaxDiffRel: test fails if NBins more than NSigma away and NBins more than MaxDiffRel (relative difference) away\n"<<std::endl; 
  out<<"Optional Parameter: FixedError: override the histogram errors with this value"<<std::endl;
  out<<"Optional Parameter: IncludeRefError: use both the histogram and reference histogram errors in calculation"<<std::endl;
  out<<"Optional Parameter: MinBinEntries: Minimum bin entries in profile histogram needed to check this bin (by default: -1)"<<std::endl;

}

