/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MaskedBinRow.cxx
 * \author R. Calkins
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/MaskedBinRow.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>
#include <ers/ers.h>


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::MaskedBinRow myInstance;


dqm_algorithms::MaskedBinRow::MaskedBinRow()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "MaskedBinRow", this );
}

dqm_algorithms::MaskedBinRow::~MaskedBinRow()
{
}

dqm_algorithms::MaskedBinRow * 
dqm_algorithms::MaskedBinRow::clone()
{
  return new MaskedBinRow();
}


dqm_core::Result *
dqm_algorithms::MaskedBinRow::execute(	const std::string & name, 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
  TH2 * histogram;

  if(object.IsA()->InheritsFrom( "TH2" )) {
    histogram = (TH2*)&object;
    if (histogram->GetDimension() != 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension != 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH2");
  }
 

  int maskedbin = dqm_algorithms::tools::GetFirstFromMap( "MaskedBin", config.getParameters());
  int testrows = dqm_algorithms::tools::GetFirstFromMap( "TestRows", config.getParameters(), 0);
  int okbin = dqm_algorithms::tools::GetFirstFromMap( "OkBin", config.getParameters(), -1);
  int dorate = dqm_algorithms::tools::GetFirstFromMap( "DoRate", config.getParameters(), 0);
  int checkstrip = dqm_algorithms::tools::GetFirstFromMap( "CheckStrip", config.getParameters(), 0);
  int useReference = dqm_algorithms::tools::GetFirstFromMap( "UseReference", config.getParameters(), 0);
  int useTotalEntries = dqm_algorithms::tools::GetFirstFromMap( "UseTotalEntries", config.getParameters(), 0);

  TH2* refhist=0;
  if (useReference) {
    refhist = static_cast<TH2 *>( config.getReference() );
  }

  if(refhist !=0){
    if(refhist->InheritsFrom( "TH2" )) {
      if (refhist->GetDimension() != 2 ){ 
	throw dqm_core::BadConfig( ERS_HERE, name, "reference histogram dimension != 2 " );
      }
    } else {
      throw dqm_core::BadConfig( ERS_HERE, name, "reference does not inherit from TH2");
    }
    if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
      throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
    }
  }
  //	if( refhist !=0 and okbin != -1 ){  
  //    	throw dqm_core::BadConfig( ERS_HERE, name, " MaskedBin is outside range" );
  //	}

  if ( !testrows && okbin > histogram->GetNbinsX()  ) {
    throw dqm_core::BadConfig( ERS_HERE, name, " MaskedBin is outside range" );
  }
  if ( testrows && okbin > histogram->GetNbinsY()  ) {
    throw dqm_core::BadConfig( ERS_HERE, name, " MaskedBin is outside range" );
  }
    
  double gthreshold;
  double rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "BinThreshold", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "BinThreshold", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }


  int redcount = 0; 
  int yellowcount = 0; 
  int redrows= 0; 
  int yellowrows = 0; 
  int totalEntries = 0;

  if (useTotalEntries) {
    totalEntries = histogram->GetEntries();
    if (totalEntries == 0) {
      useTotalEntries = 0;
      dorate = 0;
    }
  }

  dqm_core::Result* result = new dqm_core::Result();
  int max = 1;
  if(testrows){ max += histogram->GetNbinsY(); } else {  max += histogram->GetNbinsX(); }
  std::vector<float> rowtotal;
  rowtotal.clear();

  if (dorate && !useTotalEntries) {
    for(int i =0; i<max; ++i){ rowtotal.push_back(0.0); }
    
    for(int i =1; i< histogram->GetNbinsX()+1; ++i){
      for(int j =1; j< histogram->GetNbinsY()+1; ++j){
        if(testrows==0 && j == maskedbin ) continue;
        if(testrows==1 && i == maskedbin ) continue;
        if( testrows ) { 
          rowtotal[j-1] += histogram->GetBinContent(i,j);
        } else {
          rowtotal[i-1] += histogram->GetBinContent(i,j);
        }
      }
    }
  }

  //    for(int i =0; i< rowtotal.size();++i){ out << rowtotal[i] << " " ; }    std::cout<<std::endl;

  //  ERS_DEBUG(1,"rowtotal =" << rowtotal) ;

  for(int i =1; i< histogram->GetNbinsX()+1; ++i){
    for(int j =1; j< histogram->GetNbinsY()+1; ++j){
      float bincontent= histogram->GetBinContent(i,j);
      if (dorate) {
        if (useTotalEntries) {
          bincontent /= totalEntries;
        } else if (testrows) {
          bincontent /= rowtotal[j - 1];
        } else {
          bincontent /= rowtotal[i - 1];
        }
      }

      if(bincontent>gthreshold){
	if( testrows==1 && histogram->GetBinContent(maskedbin,j)  != 0) continue;
	if( testrows==0 && histogram->GetBinContent(i,maskedbin)  != 0) continue;
	if(refhist != 0){
	  if( refhist->GetBinContent(i,j)  != 0){
	    if(bincontent < 1.0-gthreshold && bincontent > 1.0-rthreshold  ) yellowrows++;   
	    if(bincontent < 1.0-rthreshold ) redrows++;   
	    continue; 
	  }
	}
	if( testrows==0 && j == okbin){
	  if(bincontent < 1.0-gthreshold && bincontent > 1.0-rthreshold  ) yellowrows++;   
	  if(bincontent < 1.0-rthreshold ) redrows++;   
	  continue; }
	if( testrows==1 && i == okbin){
	  if(bincontent < 1.0-gthreshold && bincontent > 1.0-rthreshold  ) yellowrows++;   
	  if(bincontent < 1.0-rthreshold ) redrows++;   
	  continue; }

	ERS_DEBUG(1,"Found bin : ("<< i<< ","<<j<<" ) = " << bincontent ) ;

	dqm_algorithms::tools::PublishBin(histogram,i,j,bincontent,result);
	if(bincontent>rthreshold){
	  ++redcount;
	}else{
	  ++yellowcount;
	}
      } 

    }
  }


  if (checkstrip) {
    if (dorate) {
      if(redrows > 0 ){
        result->tags_["RedRows"] = redrows;
        result->status_ = dqm_core::Result::Red;
        return result;
      } else if (yellowrows > 0 ){
        result->tags_["YellowRows"] = yellowrows;
        result->status_ = dqm_core::Result::Yellow;
        return result;
      } 
    }
    
  } else {
    if( redcount > 0 ) {
      result->tags_["RedBins"] = redcount;
      result->status_ = dqm_core::Result::Red;
      return result;
    } else if(yellowcount > 0 ) {
      result->tags_["YellowBins"] = yellowcount;
      result->status_ = dqm_core::Result::Yellow;
      return result;
    } 
  }
  
  
  result->status_ = dqm_core::Result::Green;
  return result;

}

void
dqm_algorithms::MaskedBinRow::printDescription(std::ostream& out)
{
  
  out<<"MaskedBinRow: Does a bins over threshold test in columns/rows but ignores columns/rows if a 'masked' bin has entries\n"<<std::endl;
                                                                                                                                            
  out<<"Mandatory Parameter: MaskedBin : This is the row or column that has the 'masked' bins. \n"<<std::endl;
  
  out<<"Optional Parameter: TestRows : If masked bin has entries, ignore entries in row rather than columns when parameter is set to 1.    default = 0 "<<std::endl;
  out<<"Optional Parameter: OkBin : This is the row or column that has the 'ok' bin and will be ignored when preforming the algorithm "<<std::endl;
  out<<"Optional Parameter: DoRate : This tells the algorithm to check the rate of digital errors rather than an absolute number"<<std::endl;
  out<<"Optional Parameter: CheckStrip : To be used with DoRate.  Rather than check individual bins, it checks that the fraction of events outside the ok bin is below the threshold."<<std::endl;
  out<<"Optional Parameter: UseReference : Reference histogram with entries in the 'ok' bins will be used to indicate that these bins should be ignored  when parameter is set to 1.    default = 0 "<<std::endl;
  out<<"Optional Parameter: UseTotalEntries : The rate of digital errors will be checked against total number of entries in histogram  : default = 0 "<<std::endl;
}

