/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file CSCNoisyDead.cxx
 * \author J. Veatch
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/CSCNoisyDead.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>
#include <ers/ers.h>

#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::CSCNoisyDead myInstance;


dqm_algorithms::CSCNoisyDead::CSCNoisyDead()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "CSCNoisyDead", this );
}

dqm_algorithms::CSCNoisyDead::~CSCNoisyDead()
{
}

dqm_algorithms::CSCNoisyDead * 
dqm_algorithms::CSCNoisyDead::clone()
{
  return new CSCNoisyDead();
}


dqm_core::Result *
dqm_algorithms::CSCNoisyDead::execute(	const std::string & name, 
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

  double high = dqm_algorithms::tools::GetFirstFromMap( "HighRange", config.getParameters(), 0.2);
  double low = dqm_algorithms::tools::GetFirstFromMap( "LowRange", config.getParameters(), 0.2);
  
  int gthreshold;
  int rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "NStrips", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NStrips", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  double phi_s_sum, phi_l_sum, eta_s_sum, eta_l_sum;

  double phi_s_avg[48], phi_l_avg[48], eta_s_avg[192], eta_l_avg[192];

  int NphiDead = 0;
  int NphiNoisy = 0;
  int NetaDead = 0;
  int NetaNoisy = 0;
  int NBad=0;

  for (int i = 0; i < 48; i++) {
    phi_s_avg[i] = 0;
    phi_l_avg[i] = 0;
  }

  for (int i = 0; i < 192; i++) {
    eta_s_avg[i] = 0;
    eta_l_avg[i] = 0;
  }

  // Average over Phi strips
  for(size_t x = 0; x < 48; x++) {
    int xbin = histogram->GetXaxis()->FindBin(x-48);
    phi_s_sum = 0;
    phi_l_sum = 0;
    if (histogram->GetNbinsY() >= 85){
      for(size_t y = 6; y < 86; y++) {
        if( y%5 != 0 ) {

	  //          int lay = y%5;

          double content = histogram->GetBinContent(xbin,y);

          if(y%10 > 5) {
            phi_l_sum += content;
          }else {
            phi_s_sum += content;
          }
        }
      }
    }
    phi_s_avg[x] = (phi_s_sum/32.0);
    phi_l_avg[x] = (phi_l_sum/32.0);
  } // End phi strips average

  // Average over Eta strips
  for(size_t x = 0; x < 192; x++) {
    int xbin = histogram->GetXaxis()->FindBin(x+1);
    eta_s_sum = 0;
    eta_l_sum = 0;
    for(size_t y = 6; y < 86; y++) {
      if( y%5 != 0 ) {

	//        int lay = y%5;

        double content = histogram->GetBinContent(xbin,y);

        if(y%10 > 5) {
          eta_l_sum += content;
        }else {
          eta_s_sum += content;
        }
      }
    }
    eta_s_avg[x] = (eta_s_sum/32.0);
    eta_l_avg[x] = (eta_l_sum/32.0);
  } // End eta strips average

  // Compare Phi strips to average
  for(size_t x = 0; x < 48; x++) {
    int xbin = histogram->GetXaxis()->FindBin(int(x)-48);
    phi_s_sum = 0;
    phi_l_sum = 0;
    for(size_t y = 6; y < 86; y++) {
      if( y%5 != 0 ) {

	//        int lay = y%5;

        double content = histogram->GetBinContent(xbin,y);

        if(y%10 > 5) {
          if(content > (1+high)*phi_l_avg[x]){
            NphiNoisy++;
          }else if(content < (1-low)*phi_l_avg[x]){
            NphiDead++;
          }
        }else {
          if(content > (1+high)*phi_s_avg[x]){
            NphiNoisy++;
          }else if(content < (1-low)*phi_s_avg[x]){
            NphiDead++;
          }
        }
      }
    }
  } // End phi strips comparison

  // Compare Eta strips to average
  for(size_t x = 0; x < 192; x++) {
    int xbin = histogram->GetXaxis()->FindBin(x+1);
    eta_s_sum = 0;
    eta_l_sum = 0;
    for(size_t y = 6; y < 86; y++) {
      if( y%5 != 0 ) {

	//        int lay = y%5;

        double content = histogram->GetBinContent(xbin,y);

        if(y%10 > 5) {
          if(content > (1+high)*eta_l_avg[x]){
            NetaNoisy++;
          }else if(content < (1-low)*eta_l_avg[x]){
            NetaDead++;
          }
        }else {
          if(content > (1+high)*eta_s_avg[x]){
            NetaNoisy++;
          }else if(content < (1-low)*eta_s_avg[x]){
            NetaDead++;
          }
        }
      }
    }
  } // End phi strips comparison

  dqm_core::Result* result = new dqm_core::Result();
  NBad = NphiNoisy + NphiDead + NetaNoisy + NetaDead;
  result->tags_.insert(std::make_pair("NBad",NBad));
  result->tags_.insert(std::make_pair("NphiNoisy",NphiNoisy));
  result->tags_.insert(std::make_pair("NphiDead",NphiDead));
  result->tags_.insert(std::make_pair("NetaNoisy",NetaNoisy));
  result->tags_.insert(std::make_pair("NetaDead",NetaDead));

  if (NBad >= rthreshold){
    result->status_ = dqm_core::Result::Red;
  }else if (NBad < gthreshold){
    result->status_ = dqm_core::Result::Green;
  }else{
    result->status_ = dqm_core::Result::Yellow;
  }

  return result;

/*

  dqm_core::Result* result = new dqm_core::Result();
  int max = 1;
  if(testrows){ max += histogram->GetNbinsY(); } else {  max += histogram->GetNbinsX(); }
  std::vector<float> rowtotal;
  rowtotal.clear();
  for(int i =0; i<max; ++i){ rowtotal.push_back(0.0); }
  
  if(checkstrip==0 &&  redcount >= yellowcount && redcount !=0 ){
    result->tags_["RedBins"] = redcount;
    result->status_ = dqm_core::Result::Red;
    return result;
  }
  if(checkstrip==0 &&  yellowcount !=0 ){
    result->tags_["YellowBins"] = yellowcount;
    result->status_ = dqm_core::Result::Yellow;
    return result;
  } 

  if(checkstrip && dorate &&  redrows >= yellowrows && redrows !=0 ){
    result->tags_["RedRows"] = redrows;
    result->status_ = dqm_core::Result::Red;
    return result;
  }
  if(checkstrip && dorate &&  yellowrows !=0 ){
    result->tags_["YellowRows"] = yellowrows;
    result->status_ = dqm_core::Result::Yellow;
    return result;
  } 


  result->status_ = dqm_core::Result::Green;
  return result;
*/
}

void
dqm_algorithms::CSCNoisyDead::printDescription(std::ostream& out)
{
  
  out<<"CSCNoisyDead: Checks to see how many CSC layers are dead or noisy"<<std::endl;

  out<<"Mandatory Parameter: HighRange : Fraction above average considered noisy"<<std::endl;
  out<<"Mandatory Parameter: LowRange : Fraction below average considered dead"<<std::endl;

  out<<"Mandatory Green/Red Threshold : NStrips : "<<std::endl;
                                                                                                                                            
}

