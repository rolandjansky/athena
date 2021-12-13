/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Method To Calculate MDT Dead Noisy Tubes
//Author Orin Harris omh@cern.ch University of Washington
//       Justin Griffiths griffith@cern.ch University of Washington
//Aug 2009
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "DataQualityUtils/MdtDeadNoisyUtils.h"
#include <cmath>
#include <algorithm>
#include <TLine.h>
#include <TText.h>

using std::sort;
using std::vector;

#define STDDEV_POS 1.0
#define MARKER_POS_RED 0.6
#define MEZZ_POS 0.4
#define LAYER_POS 0.3
#define ML_POS 0.22
#define MARKER_POS_BLUE 0.15

namespace dqutils_mdtdeadnoisy {

  double sqr( double x ) {
    return x*x;
  }

  void displayList( std::vector<int> & v ) {
    std::cout << "{ ";
    if(v.size()>1) {
      if( v.at(0)+1 != v.at(1) ) std::cout << v.at(0) << ", ";
      else if( v.at(0)+1 == v.at(1) ) std::cout << v.at(0) << "-";
      for(unsigned int i=1; i<v.size()-1; i++) {
	if( v.at(i)+1 != v.at(i+1) ) std::cout << v.at(i) << ", ";
	else if( v.at(i-1)+1 != v.at(i) && v.at(i)+1 == v.at(i+1) ) std::cout << v.at(i) << "-";
      }
      std::cout << v.back();
    }
    else if(v.size()>0)
      std::cout << v.at(0);
    std::cout << " }" << std::endl;
  }

  void displayList( std::vector<int> & v, std::ofstream & outputfile ) {
    outputfile << "{ ";
    if(v.size()>1) {
      if( v.at(0)+1 != v.at(1) ) outputfile << v.at(0) << ", ";
      else if( v.at(0)+1 == v.at(1) ) outputfile << v.at(0) << "-";
      for(unsigned int i=1; i<v.size()-1; i++) {
	if( v.at(i)+1 != v.at(i+1) ) outputfile << v.at(i) << ", ";
	else if( v.at(i-1)+1 != v.at(i) && v.at(i)+1 == v.at(i+1) ) outputfile << v.at(i) << "-";
      }
      outputfile << v.back();
    }
    else if(v.size()>0)
      outputfile << v.at(0);
    outputfile << " }" << std::endl;
  }

  void getDeadList( TH1F* h, std::vector<int> & v ) {
    v.clear();
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( h->At(i) <= 0 )
	v.push_back( i );
    }
  }

  bool find( std::vector<int> & v, unsigned int x ) {
    for(unsigned int i=0; i<v.size(); i++)
      if( (int)x == v.at(i) )
	return true;
    return false;
  }

  void getSurelyDeadList( TH1F* h, std::vector<int> & v, std::vector<int> & betweenlist ) {
    v.clear();
    if( h->At(1) <= 0 && h->At(2) != 0 && !find(betweenlist, 1) )
      v.push_back( 1 );
    for(int i=2; i<=h->GetNbinsX()-1; i++) {
      if( h->At(i-1) > 0 && h->At(i) <= 0 && h->At(i+1) > 0 && !find(betweenlist, i) )
	v.push_back( i );
    }
    if( h->At(h->GetNbinsX()) <= 0 && h->At(h->GetNbinsX()-1) != 0 && !find(betweenlist, h->GetNbinsX()) )
      v.push_back( h->GetNbinsX() );
  }

  void getNotBetweenList( std::vector<int> & v, std::vector<int> & surely_v, std::vector<int> & betweenlist ) {
    surely_v.clear();  
    for(unsigned int i=0; i<v.size(); i++) {
      if( !find(betweenlist, v.at(i)) ) {
	surely_v.push_back( v.at(i) );
      }
    }
  }

  void getAllBins( TH1F* h, std::vector<int> & v ) {
    v.clear();
    for(int i=1; i<=h->GetNbinsX(); i++)
      v.push_back( (int)(h->At(i)) );
  }

  void getAllBinsInRange( TH1F* h, std::vector<int> & v, int x1, int x2 ) {
    v.clear();
    if( x1 < 1 || x2 > h->GetNbinsX() ) {
      std::cout << "WARNING in " << ((TString)h->GetName())(0,7) << ": in getAllBinsInRange: (" << x1 << "," << x2 << ") outside histo range!" << std::endl;
      return;
    }
    for(int i=x1; i<=x2; i++)
      v.push_back( (int)(h->At(i)) );
  }

  void getAllBinsInRangeBelowCrustCutoff( TH1F* h, std::vector<int> & v, int crustCutoff, int x1, int x2 ) {
    v.clear();
    if( x1 < 1 || x2 > h->GetNbinsX() ) {
      std::cout << "WARNING in " << ((TString)h->GetName())(0,7) << ": in getAllBinsInRangeBelowCrustCutoff: (" << x1 << "," << x2 << ") outside histo range!" << std::endl;
      return;
    }
    for(int i=x1; i<=x2; ++i) {
      if( h->At(i) < crustCutoff ) v.push_back( (int)(h->At(i)) );
      else v.push_back( crustCutoff );
    }
  }

  void getAllUnmaskedBinsInRangeBelowCrustCutoff( TH1F* h, std::vector<int> & v, int crustCutoff, int x1, int x2 ) {
    vector<int> maskedTubes = GetMaskedTubesForDead(h);
    v.clear();
    if( x1 < 1 || x2 > h->GetNbinsX() ) {
      std::cout << "WARNING in " << ((TString)h->GetName())(0,7) << ": in getAllUnmaskedBinsInRangeBelowCrustCutoff: (" << x1 << "," << x2 << ") outside histo range!" << std::endl;
      return;
    }
    for(int i=x1; i<=x2; ++i) {
      if( (h->At(i)<crustCutoff) && !AinB(i,maskedTubes) ) v.push_back( (int)(h->At(i)) );
      else v.push_back( crustCutoff );
    }
  }

  void getBins( TH1F* h, std::vector<int> & indices, std::vector<int> & content ) {
    content.clear();
    for(unsigned int i=0; i<indices.size(); ++i) {
      content.push_back( (int)(h->At(indices.at(i))) );
    }
  }

  void removeZeros( std::vector<int> & v ) {
    std::vector<int> tmp;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( v.at(i) > 0 )
	tmp.push_back( v.at(i) );
    }
    v = tmp;
  }

  void getNonEmptyBins( TH1F* h, std::vector<int> & v ) {
    v.clear();
    for(int i=1; i<=h->GetNbinsX(); ++i) {
      if( h->At(i) > 0 )
	v.push_back( (int)(h->At(i)) );
    }
  }

  void getAllTubes( TH1F* h, std::vector<int> & v ) {
    v.clear();
    for(int i=1; i<=h->GetNbinsX(); ++i) {
      v.push_back( i );
    }
  }

  void getNonEmptyTubes( TH1F* h, std::vector<int> & v ) {
    v.clear();
    for(int i=1; i<=h->GetNbinsX(); ++i) {
      if( h->At(i) > 0 )
	v.push_back( i );
    }
  }

  void getNotMasked( std::vector<int> & v, TH1F* h ) {
    vector<int> maskedTubes = GetMaskedTubesForDead(h);

    if(maskedTubes.empty())
      return;

    std::vector<int> tmp;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( !AinB( v.at(i), maskedTubes ) ) {
	tmp.push_back( v.at(i) );
      }
    }
    v = tmp;
  }

  void getNotInDeadMezz( std::vector<int> & v, std::vector<int> & deadMezz_v, const TString & hardware_name, int totalTubes) {
    if(deadMezz_v.empty())
      return;
//     std::cout << "Mezz dead! Trying to mask out...\n";

    int groupsPerLayer;
    std::vector<TubeRange> tubeRanges = getMezzRanges( hardware_name, totalTubes, groupsPerLayer);

    std::vector<int> tmp;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( !AinB( Get_Mezz_of_Tube(v.at(i),hardware_name/*,totalTubes*/,tubeRanges,groupsPerLayer), deadMezz_v ) ) {
//  	std::cout <<  "Mezz of tube " << v.at(i) << ": " << Get_Mezz_of_Tube(v.at(i),hardware_name/*,totalTubes*/,tubeRanges,groupsPerLayer) << ", dead Mezz: " << deadMezz_v.at(0) << std::endl; 
	tmp.push_back( v.at(i) );
      }
    }
    v = tmp;
  }

  void getNotInDeadLayer( std::vector<int> & v, std::vector<int> & deadLayer_v, const TString & hardware_name, int totalTubes) {
    if(deadLayer_v.empty())
      return;
    std::vector<int> tmp;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( !AinB( Get_Layer_of_Tube(v.at(i),hardware_name,totalTubes), deadLayer_v ) ) {
	tmp.push_back( v.at(i) );
      }
    }
    v = tmp;
  }

  void getNotInDeadML( std::vector<int> & v, std::vector<int> & deadML_v, const TString & hardware_name, int totalTubes) {
    if(deadML_v.empty())
      return;
//     std::cout << "ML dead! Trying to mask out...\n";
    std::vector<int> tmp;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( !AinB( Get_ML_of_Tube(v.at(i),hardware_name,totalTubes), deadML_v ) ) {
// 	std::cout <<  "ML of tube " << v.at(i) << ": " << Get_ML_of_Tube(v.at(i),hardware_name,totalTubes) << ", dead ML: " << deadML_v.at(0) << std::endl; 
	tmp.push_back( v.at(i) );
      }
    }
    v = tmp;
  }

  template <class T>
  double getMean( std::vector<T> & v ) {
    double runningMean = 0;
    for(unsigned int i=0; i<v.size(); i++) {
      runningMean = ( runningMean*(i) + v.at(i) ) / (double)(i+1);
    }
    return runningMean;
  }

  double getMean_WithCut( std::vector<int> & v, double cut ) {
    double runningMean = 0;
    int counter = 0;
    for(unsigned int i=0; i<v.size(); ++i) {
      if( (double)v.at(i) < cut ) {
	runningMean = ( runningMean*(counter) + v.at(i) ) / (double)(counter+1);
	counter++;
      }
    }
    return runningMean;
  }

  void getNoBetweens( TH1F* h, std::vector<int> & no_dead, std::vector<int> & no_betweens, std::vector<int> & betweenlist, float minBinContent ) {
    no_betweens.clear();
    betweenlist.clear();
    std::vector<int> tmp_no_dead_pruned;
    getNoBetweens_middle_aggressive(no_dead, tmp_no_dead_pruned);
    double no_dead_mean = getMean( tmp_no_dead_pruned );
    std::vector<int> tmp_no_betweens, tmp_betweenlist;
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( h->At(i) > minBinContent * no_dead_mean ) {
	tmp_no_betweens.push_back( (int)(h->At(i)) );
	tmp_betweenlist.push_back(i);
      }
      else {
	if( tmp_no_betweens.size() >= 1 && getMean_WithCut( tmp_no_betweens, 2.*no_dead_mean ) < 0.25 * no_dead_mean ) {
	  betweenlist.insert(betweenlist.end(), tmp_betweenlist.begin(), tmp_betweenlist.end());
	  tmp_no_betweens.clear();
	  tmp_betweenlist.clear();
	}
	else if( tmp_no_betweens.size() >= 1 ) {
	  no_betweens.insert(no_betweens.end(), tmp_no_betweens.begin(), tmp_no_betweens.end());
	  tmp_no_betweens.clear();
	  tmp_betweenlist.clear();
	}
	betweenlist.push_back(i);        // look into this -- shouldn't have 'else' for this somewhere? displayList(betweenlist) should get to the bottom if duplicates
      }
    }
    if( tmp_no_betweens.size() >= 1 && getMean_WithCut( tmp_no_betweens, 2.*no_dead_mean ) < 0.25 * no_dead_mean ) {
      betweenlist.insert(betweenlist.end(), tmp_betweenlist.begin(), tmp_betweenlist.end());
    }
    else if( tmp_no_betweens.size() >= 1 ) {
      no_betweens.insert(no_betweens.end(), tmp_no_betweens.begin(), tmp_no_betweens.end());
    }
  }

  void removeNonContiguous( std::vector<int> & betweenlist, unsigned int sz ) {
    std::vector<int> betweenlist_copy = betweenlist;
    std::vector<int> tmp_betweenlist;
    betweenlist.clear();
    if( betweenlist_copy.size() <= 2 ) return;
    for(unsigned int i=0; i<betweenlist_copy.size()-1; i++) {
      if( betweenlist_copy.at(i)+1 == betweenlist_copy.at(i+1) ) {
	tmp_betweenlist.push_back(betweenlist_copy.at(i));
	if( i == betweenlist_copy.size()-2 ) tmp_betweenlist.push_back(betweenlist_copy.at(i+1));
      } 
      else {
	if( tmp_betweenlist.size() >= 1 ) tmp_betweenlist.push_back(betweenlist_copy.at(i));
	if( tmp_betweenlist.size() >= sz ) betweenlist.insert(betweenlist.end(), tmp_betweenlist.begin(), tmp_betweenlist.end());
	tmp_betweenlist.clear();
      }
    }
    if( tmp_betweenlist.size() >= sz ) betweenlist.insert(betweenlist.end(), tmp_betweenlist.begin(), tmp_betweenlist.end());
  }

  void getNoBetweens_middle( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle ) {
    no_betweens_middle.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 20;
    no_betweens_middle.insert( no_betweens_middle.begin(), no_betweens.begin()+crustSize, no_betweens.end()-crustSize );
  }

  void getNoBetweens_lowmiddle( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle ) {
    no_betweens_lowmiddle.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 4;
    no_betweens_lowmiddle.insert( no_betweens_lowmiddle.begin(), no_betweens.begin()+crustSize, no_betweens.begin()+2*crustSize );
  }

  void getNoBetweens_middle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle_aggressive ) {
    no_betweens_middle_aggressive.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 4;
    no_betweens_middle_aggressive.insert( no_betweens_middle_aggressive.begin(), no_betweens.begin()+crustSize, no_betweens.end()-crustSize );
  }

  void getNoBetweens_middle_semiaggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle_aggressive ) {
    no_betweens_middle_aggressive.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 8;
    no_betweens_middle_aggressive.insert( no_betweens_middle_aggressive.begin(), no_betweens.begin()+crustSize, no_betweens.end()-crustSize );
  }

  void getNoBetweens_lowmiddle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle_aggressive ) {
    no_betweens_lowmiddle_aggressive.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 10;
    no_betweens_lowmiddle_aggressive.insert( no_betweens_lowmiddle_aggressive.begin(), no_betweens.begin()+crustSize, no_betweens.begin()+2*crustSize );
  }

  void getNoBetweens_highmiddle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle_aggressive ) {
    no_betweens_lowmiddle_aggressive.clear();
    sort(no_betweens.begin(), no_betweens.end());
    int crustSize = no_betweens.size() / 10;
    no_betweens_lowmiddle_aggressive.insert( no_betweens_lowmiddle_aggressive.begin(), no_betweens.end()-2*crustSize, no_betweens.end()-crustSize );
  }

  double getStandardDevFromMean( std::vector<int> & v, double mean ) {
    double sum = 0;
    for(unsigned int i=0; i<v.size(); i++) {
      double diff = (double)v.at(i) - mean;
      if( diff > 100.0 * mean )    // lines to avoid bias due to very noisy tubes
	diff = 100.0 * mean;
      sum += sqr( diff );
    }
    return std::sqrt(sum/(double)v.size());
  }

  double getStandardDevFromMean_neg( std::vector<int> & v, double mean ) {
    double sum = 0;
    for(unsigned int i=0; i<v.size(); i++) {
      double diff = (double)v.at(i) - mean;
      if( diff > 100.0 * mean )    // avoid bias due to very noisy tubes
	diff = 100.0 * mean;
      if( diff < 0. )
	sum += sqr( diff );
    }
    return std::sqrt(sum/(double)v.size());
  }

  double getStandardDevFromMean_pos( std::vector<int> & v, double mean ) {
    double sum = 0;
    for(unsigned int i=0; i<v.size(); i++) {
      double diff = (double)v.at(i) - mean;
      if( diff > 100.0 * mean )    // avoid bias due to very noisy tubes
	diff = 100.0 * mean;
      if( diff > 0. )
	sum += sqr( diff );
    }
    return std::sqrt(sum/(double)v.size());
  }

  void getNoisyList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive, double num_std_dev, std::vector<int> & noisylist) {
    noisylist.clear();
    double mean = getMean(no_betweens_middle_aggressive); // a fancy median
    double stddev = getStandardDevFromMean(no_betweens_middle, mean);
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( h->At(i) - mean > num_std_dev*stddev )
	noisylist.push_back(i);
    }
  }

  void getNoisyList2( TH1F* h, [[maybe_unused]] std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive, double num_pct, std::vector<int> & noisylist) {
    noisylist.clear();
    double mean = getMean(no_betweens_middle_aggressive); // a fancy median
    if( mean > 0 ) {
      for(int i=1; i<=h->GetNbinsX(); i++) {
	if( h->At(i) > (mean) * ( (num_pct/100.) + (500.-25.*(num_pct/100.))/(mean*mean) ) ) {
	  noisylist.push_back(i);
	}
      }
    }
  }

  void getIneffList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_std_dev, std::vector<int> & inefflist) {
    inefflist.clear();
    double mean = getMean(no_betweens_lowmiddle_aggressive); // a fancy median
    double stddev = getStandardDevFromMean(no_betweens_middle, mean);
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( mean - h->At(i) > num_std_dev*stddev )
	inefflist.push_back(i);
    }
  }

  void getIneffList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_std_dev_1, double num_std_dev_2, std::vector<int> & inefflist) {
    inefflist.clear();
    double mean = getMean(no_betweens_lowmiddle_aggressive); // a fancy median
    double stddev = getStandardDevFromMean(no_betweens_middle, mean);
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( mean - h->At(i) > num_std_dev_1*stddev && mean - h->At(i) <= num_std_dev_2*stddev )
	inefflist.push_back(i);
    }
  }

  void getIneffList2( TH1F* h, [[maybe_unused]] std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct, std::vector<int> & inefflist) {
    inefflist.clear();
    double mean = getMean(no_betweens_lowmiddle_aggressive); // a fancy median
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( h->At(i) < mean*num_pct/100. )
	inefflist.push_back(i);
    }
  }

  void getIneffList2( std::vector<int> & indices, TH1F* h, [[maybe_unused]] std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct, std::vector<int> & inefflist) {
    inefflist.clear();
    double mean = getMean(no_betweens_lowmiddle_aggressive); // a fancy median
    for(unsigned int i=0; i<indices.size(); ++i) {
      int index = indices.at(i);
      if( h->At(index) < mean*num_pct/100. )
	inefflist.push_back(index);
    }
  }

  void getIneffList2( TH1F* h, [[maybe_unused]] std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct_1, double num_pct_2, std::vector<int> & inefflist) {
    inefflist.clear();
    double mean = getMean(no_betweens_lowmiddle_aggressive); // a fancy median
    for(int i=1; i<=h->GetNbinsX(); i++) {
      if( h->At(i) < mean*num_pct_2/100. && h->At(i) > mean*num_pct_1/100. )
	inefflist.push_back(i);
    }
  }

  // for deads
  bool validityCheckDead( std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle ) {
    bool isOK = true;
    double mean = getMean( no_betweens_lowmiddle );

    if( no_betweens_middle.size() < 60 ) {
//       std::cout << "    WARNING: few non-empty bins" << std::endl;
      isOK = false;
    }
//     std::cout << "   " << mean << "   " << getStandardDevFromMean_neg( no_betweens_middle, mean ) << std::endl; 
    if( mean < 25. || (mean < 50. && getStandardDevFromMean_neg( no_betweens_middle, mean ) > 0.2 * mean) || (mean < 100. && getStandardDevFromMean_neg( no_betweens_middle, mean ) > 0.3 * mean) ) {
//       std::cout << "    WARNING: low Mean" << std::endl;
//       std::cout << "       MEAN: " << mean << std::endl;
//       std::cout << "       STDV: " << getStandardDevFromMean_neg( no_betweens_middle, mean ) << std::endl;
      isOK = false;
    }
    if( getStandardDevFromMean_neg( no_betweens_middle, mean ) > 0.5 * mean ) {
//       std::cout << "    WARNING: large Standard Dev relative to mean" << std::endl;
      isOK = false;
    }

    return isOK;
  }

  // for noisy: this inlcudes max for the noisies only (even at low stat could have very noisy tube)
  // the max parameter is not currently used.
  bool validityCheckNoise( std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive
/*, int max*/ ) {
    bool isOK = true;
    double mean = getMean( no_betweens_middle_aggressive );

//     std::cout << " val check mean: " << mean << std::endl; 
//     std::cout << " val check stdev: " << getStandardDevFromMean( no_betweens_middle, mean ) << std::endl; 

    if( no_betweens_middle.size() < 60 ) {
//       std::cout << "    WARNING: few non-empty bins" << std::endl;
      isOK = false;
    }
    if( mean < 5. ) {
//       std::cout << "    WARNING: low Mean" << std::endl;
      isOK = false;
    }
    if( getStandardDevFromMean( no_betweens_middle, mean ) > 2. * mean ) {
//       std::cout << "    WARNING: large Standard Dev relative to mean" << std::endl;
      isOK = false;
    }

    return isOK;
  }

  void getNonEmptyNonNoisyTubes( TH1F* h_tube_fornoise, TH1F* h_tube, std::vector<int> & non_dead_non_noisy) {
    std::vector<int> no_deads, non_noisy, no_betweens, no_betweens_middle, no_betweens_middle_aggressive, no_betweens_highmiddle_aggressive, betweenlist, noisylist;
    non_dead_non_noisy.clear();

    getNonEmptyBins( h_tube_fornoise, no_deads );
    getNoBetweens( h_tube_fornoise, no_deads, no_betweens, betweenlist, .05 );
    getNoBetweens_middle( no_betweens, no_betweens_middle );
    getNoBetweens_middle_aggressive( no_betweens, no_betweens_middle_aggressive );
    getNoBetweens_highmiddle_aggressive( no_betweens, no_betweens_highmiddle_aggressive );
    getNoisyList2( h_tube_fornoise, no_betweens_middle, no_betweens_highmiddle_aggressive, 300.0, noisylist);
  
    //   std::cout << "h_tube->GetNbinsX(): " << h_tube->GetNbinsX() << ", noisylist.size(): " << noisylist.size() << std::endl;
    std::vector<int> tmp_non_dead_non_noisy;
    for(int i=1; i<=h_tube->GetNbinsX(); i++) {
      if( h_tube->At(i) > 0 ) {
	bool dopush = true;
	for(unsigned int j=0; j<noisylist.size(); j++) {
	  if(i == noisylist.at(j)) {
	    dopush = false;
	    break;
	  }
	}
	if(dopush) tmp_non_dead_non_noisy.push_back((int)h_tube->At(i));      
      }
    }
    non_dead_non_noisy = tmp_non_dead_non_noisy;
  }

  void CatalogNoisyEff( TH1F* h_tube_bkd, TH1F* h_tube_fornoise, TH1F* num, TH1F* den, int nTriggers, TH1F* EffVSNoise_num, TH1F* EffVSNoise_den, TH1F* EffVSNoise_ADCCut_num, TH1F* EffVSNoise_ADCCut_den, TH1F* EffVSBkd_num, TH1F* EffVSBkd_den) {
    TH1F* h = (TH1F*) h_tube_bkd->Clone();
    h->Scale(1./nTriggers/300.*1e9);
    TH1F* h2 = (TH1F*) h_tube_fornoise->Clone();
    h2->Scale(1./nTriggers/300.*1e9);

    for(int i = 1; i != h->GetNbinsX()+1; ++i) {
      if(den->At(i)>0) {
	EffVSNoise_ADCCut_num->Fill(h->At(i)/1000.,num->At(i));
	EffVSNoise_ADCCut_den->Fill(h->At(i)/1000.,den->At(i));
	if(h2->At(i)<2000) {
	  EffVSBkd_num->Fill(h->At(i)/1000.,num->At(i));
	  EffVSBkd_den->Fill(h->At(i)/1000.,den->At(i));
	}
	EffVSNoise_num->Fill(h2->At(i)/1000.,num->At(i));
	EffVSNoise_den->Fill(h2->At(i)/1000.,den->At(i));
      }
    }

    delete h;
    delete h2;
  }

  std::vector<int> GetNoisyTubes( TH1F* h_tube_fornoise, int nTriggers, float frequency){ //Simply determine noisy tubes from freq.
    TH1F* h = (TH1F*) h_tube_fornoise->Clone();
    h->Scale(1./nTriggers/300.*1e9);
    std::vector<int> noisyTubes;
    for(int i = 1; i != h->GetNbinsX()+1; ++i) if(h->At(i)>frequency) noisyTubes.push_back(i);
    delete h;
    return noisyTubes;
  }

  std::vector<int> GetNoisyTubes( TH1F* h_tube_fornoise, TH1F* h_tube, TString chamber, int & validity) {
    validity = 1;
    
    //   std::cout << "In GetNoisyTubes" << std::endl; 
    if(chamber=="TEST") std::cout << chamber << std::endl;
    std::vector<int> no_deads, no_betweens, no_betweens_middle, no_betweens_middle_aggressive, no_betweens_highmiddle_aggressive, betweenlist, noisylist;

    getNonEmptyBins( h_tube_fornoise, no_deads );
    //   std::cout << " no_deads for " << chamber << ": " << no_deads.size() << std::endl; 
    getNoBetweens( h_tube_fornoise, no_deads, no_betweens, betweenlist, .05 );
    //   std::cout << " no_betweens for " << chamber << ": " << no_betweens.size() << std::endl; 
    //   std::cout << " betweenlist for " << chamber << ": " << betweenlist.size() << std::endl; 
    getNoBetweens_middle( no_betweens, no_betweens_middle );
    //   std::cout << "no_betweens_middle for " << chamber << ": " << no_betweens_middle.size() << std::endl; 
    getNoBetweens_middle_aggressive( no_betweens, no_betweens_middle_aggressive );
    //   std::cout << "no_betweens_middle_aggressive for " << chamber <<": " << no_betweens_middle_aggressive.size() << std::endl; 
    getNoBetweens_highmiddle_aggressive( no_betweens, no_betweens_highmiddle_aggressive );
    //   std::cout << "no_betweens_highmiddle_aggressive for " << chamber <<": " << no_betweens_highmiddle_aggressive.size() << std::endl; 
    getNoisyList2( h_tube_fornoise, no_betweens_middle, no_betweens_highmiddle_aggressive, 300.0, noisylist);
  
    //   std::cout << "Before Validity Check for " << chamber <<": " << std::endl; 
    //   displayList(noisylist); 
    if( validityCheckNoise(no_betweens_middle, no_betweens_middle_aggressive /*, (int) h_tube_fornoise->GetMaximum()*/ ) ) {
      return noisylist;
    }  
    else {
      validity = 2;
      //     std::cout << " ... validity check failed for noisy, trying with h_tube" << std::endl;
      //    return GetNoisyTubes_WithoutForNoise( h_tube_fornoise, h_tube, chamber);      
      return GetNoisyTubes_WithoutForNoise( h_tube, validity, chamber );
    }
  }

  std::vector<int> GetNoisyTubes_WithoutForNoise( TH1F* h_tube, int & validity, TString chamber ) {
    //   std::cout << "In GetNoisyTubes_withoutfornoise" << std::endl; 
    if(chamber=="TEST") std::cout << chamber << std::endl;

    std::vector<int> no_deads, no_betweens, no_betweens_middle, no_betweens_middle_aggressive, no_betweens_highmiddle_aggressive, betweenlist, noisylist;

    getNonEmptyBins( h_tube, no_deads );
    //   std::cout << "no_deads for " << chamber << ": " << no_deads.size() << std::endl; 
    getNoBetweens( h_tube, no_deads, no_betweens, betweenlist, .05 );
    //   std::cout << "no_betweens for " << chamber << ": " << no_betweens.size() << std::endl; 
    getNoBetweens_middle( no_betweens, no_betweens_middle );
    //   std::cout << "no_betweens_middle for " << chamber << ": " << no_betweens_middle.size() << std::endl; 
    getNoBetweens_middle_aggressive( no_betweens, no_betweens_middle_aggressive );
    //   std::cout << "no_betweens_middle_aggressive for " << chamber <<": " << no_betweens_middle_aggressive.size() << std::endl; 
    getNoBetweens_highmiddle_aggressive( no_betweens, no_betweens_highmiddle_aggressive );
    //   std::cout << "no_betweens_highmiddle_aggressive for " << chamber <<": " << no_betweens_highmiddle_aggressive.size() << std::endl; 
    getNoisyList2( h_tube, no_betweens_middle, no_betweens_highmiddle_aggressive, 500.0, noisylist);    // <--- use a higher tolerance

//     std::cout << "(Without fornoise) Before Validity Check for " << chamber <<": " << std::endl; 
//     displayList(noisylist);   
    if( validityCheckNoise(no_betweens_middle, no_betweens_middle_aggressive /*, (int) h_tube->GetMaximum()*/ ) ) {
      return noisylist;
    }      
    else {
      validity = 0;
//       std::cout << " ... validity check failed for noisy." << std::endl;
    }
  
    return std::vector<int>();
  }

  int GetNumNonMaskedTubes( TH1F* h_tube ) {
    std::vector<int> tubes;

    // index-based
    getAllTubes( h_tube, tubes );
    getNotMasked( tubes, h_tube );

    return tubes.size();
  }

  std::vector<int> GetDeadTubes( TH1F* h_tube, int & validity, std::vector<int> & deadML_v, std::vector<int> & deadLayer_v, std::vector<int> & deadMezz_v) {
    validity = 1;
    TString chamber = ((TString) h_tube->GetName())(0,7);
    int totalTubes = h_tube->GetNbinsX();

    std::vector<int> non_empty, non_empty_non_noisy, no_betweens, no_betweens_middle, no_betweens_lowmiddle, no_betweens_middle_aggressive, no_betweens_lowmiddle_aggressive, deadlist, surelydeadlist, betweenlist, inefflist, surelyinefflist;
    std::vector<int> all_tubes_DeadRegionsRemoved;

    // index-based
    getAllTubes( h_tube, all_tubes_DeadRegionsRemoved );
    getNotInDeadML( all_tubes_DeadRegionsRemoved, deadML_v, chamber, totalTubes );
    getNotInDeadLayer( all_tubes_DeadRegionsRemoved, deadLayer_v, chamber, totalTubes );
    getNotInDeadMezz( all_tubes_DeadRegionsRemoved, deadMezz_v, chamber, totalTubes );
    getNotMasked( all_tubes_DeadRegionsRemoved, h_tube );

    // bin-content-based (for calculation of mean)
    getBins( h_tube, all_tubes_DeadRegionsRemoved, no_betweens );
    removeZeros( no_betweens );
    getNoBetweens_middle( no_betweens, no_betweens_middle );
    getNoBetweens_lowmiddle( no_betweens, no_betweens_lowmiddle );
    getNoBetweens_middle_aggressive( no_betweens, no_betweens_middle_aggressive );
    getNoBetweens_lowmiddle_aggressive( no_betweens, no_betweens_lowmiddle_aggressive );

    // give indicies to consider, the histo (for bin-content), a couple vectors for mean calculation, the definition of a dead tube (percent of mode), and the vector to fill
    getIneffList2( all_tubes_DeadRegionsRemoved, h_tube, no_betweens_middle, no_betweens_lowmiddle_aggressive, 10.0, deadlist );

    // make sure the histo has enough statistics, etc
    if( validityCheckDead(no_betweens_middle, no_betweens_lowmiddle) ) {
      return deadlist;
    } 
    else {
      validity = 0;
//       std::cout << " ... validity check failed for dead tubes in " << std::endl;
    }

    return std::vector<int>();
  }

  std::vector<int> GetMaskedTubesForDead(TH1F* h_tube) {
    //   std::cout << "In GetMaskedTubesForDead" << std::endl; 

    TString hardware_name = ((TString) h_tube->GetName())(0,7);
    int numLayers = GetNumLayersPerML( hardware_name );

    if(hardware_name(0,4)=="BMS4" || hardware_name(0,4)=="BMS6"){//layer 1-4 tubeId 41-48 cut out
      if( numLayers <= 3 ) {
	int cutouts[] = {41,42,43,44,45,46,47,48,89,90,91,92,93,94,95,96,137,138,139,140,141,142,143,144};
	return std::vector<int>(cutouts,cutouts+24);
      }	
    }
    if(hardware_name(0,3)=="BIR" && numLayers <= 4 ){
      if(hardware_name(5,2)=="11"||hardware_name(5,2)=="15") {
	if(hardware_name(3,1)=="1") { //layer 1-4 tube id 1-6 cut out
	  int cutouts[] = {1,2,3,4,5,6,31,32,33,34,35,36,61,62,63,64,65,66,91,92,93,94,95,96};
	  return std::vector<int>(cutouts,cutouts+24);
	}
	if(hardware_name(3,1)=="2") { //layer 1-4 tube id 28-30 cut out
	  int cutouts[] = {28,29,30,58,59,60,88,89,90,118,119,120};
	  return std::vector<int>(cutouts,cutouts+12);
	}
	if(hardware_name(3,1)=="4") { //layer 1-4 tube id 1-3 cut out
	  int cutouts[] = {1,2,3,31,32,33,61,62,63,91,92,93};
	  return std::vector<int>(cutouts,cutouts+12);
	}
	if(hardware_name(3,1)=="5") {//layer 1-4 tube id 22-24 cut out
	  int cutouts[] = {22,23,24,46,47,48,70,71,72,94,95,96};
	  return std::vector<int>(cutouts,cutouts+12);
	}
      }
    }
    if( hardware_name(0,3)=="BIR" && hardware_name(3,1)=="3") { //cut out on both ML
      int cutouts[] = {34,35,36,70,71,72,106,107,108,142,143,144,178,179,180,214,215,216,250,251,252,286,287,288};
      return std::vector<int>(cutouts,cutouts+24);
    }

    return std::vector<int>();
  }

  std::vector<int> GetMaskedTubesForNoisy( TH1F* h_tube_fornoise){
    //   std::cout << "In GetMaskedTubesForNoisy" << std::endl; 

    std::vector<int> no_deads, no_betweens, no_betweens_middle, no_betweens_middle_aggressive, betweenlist;

    getNonEmptyBins( h_tube_fornoise, no_deads );
    //   std::cout << " no_deads for " << chamber << ": " << no_deads.size() << std::endl; 
    getNoBetweens( h_tube_fornoise, no_deads, no_betweens, betweenlist, .05 );
    //   std::cout << " no_betweens for " << chamber << ": " << no_betweens.size() << std::endl; 
    //   std::cout << " betweenlist for " << chamber << ": " << betweenlist.size() << ": "; displayList(betweenlist); std::cout << std::endl; 

    //   if( validityCheckNoise(no_betweens_middle, no_betweens_middle_aggressive, (int) h_tube_fornoise->GetMaximum() ) ) { 
    return betweenlist;
    //   } else {
    //       return std::vector<int>();
    //   }
  }

  double GetMeanFromHist( TH1F* h_tube ) {
    //   std::cout << "In GetMeanForDead" << std::endl; 

    std::vector<int> non_empty, no_betweens, no_betweens_middle, no_betweens_middle_aggressive, betweenlist;

    getNonEmptyBins( h_tube, non_empty );
    getNoBetweens( h_tube, non_empty, no_betweens, betweenlist, .05 );
    getNoBetweens_middle( no_betweens, no_betweens_middle );
    getNoBetweens_middle_aggressive( no_betweens, no_betweens_middle_aggressive );

    double mean = getMean( no_betweens_middle_aggressive );
    return mean;
  }

  double GetStandardDevFromHist( TH1F* h_tube, double mean ) {
    //   std::cout << "In GetStandardDevFromHist" << std::endl; 

    std::vector<int> non_empty, no_betweens, no_betweens_middle, betweenlist;

    getNonEmptyBins( h_tube, non_empty );
    getNoBetweens( h_tube, non_empty, no_betweens, betweenlist, .05 );
    getNoBetweens_middle( no_betweens, no_betweens_middle );

    return getStandardDevFromMean( no_betweens_middle, mean );
  }

  int GetNumLayersPerML(const TString & hardware_name) {
    // Get Number of X
    int derived_NumLayerPerML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI"  ) {
      derived_NumLayerPerML = 4;
    }
    else {
      derived_NumLayerPerML = 3;
    }

    return derived_NumLayerPerML;
  }

  int GetNumTubesPerMezz(const TString &hardware_name, int mezz){
    if( hardware_name(0,3)=="BIR"){
      if( hardware_name(3,1)=="2" && mezz==5 ) return 12;
      else if( hardware_name(3,1)=="3" && (mezz==6 || mezz==12)) return 12;
      else if( hardware_name(3,1)=="4" && mezz==1 ) return 12;
      else if( hardware_name(3,1)=="5" && mezz==4 ) return 12;
    }
    return 24;    
  }

  int GetNumTubesPerML(const TString &hardware_name, int ML, int TotalTubes){
    if( hardware_name(0,3)=="BIR" || hardware_name(0,4)=="BMS4" || hardware_name(0,4)=="BMS6"){
      if( hardware_name(0,4)=="BIR3" ) return 132;
      if( ML == 1){
	if( hardware_name(0,3)=="BMS" )	return 120;
	if( hardware_name(3,1)=="1" ) return 120;
	if( hardware_name(3,1)=="2" ) return 108;
	if( hardware_name(3,1)=="4" ) return 108;
	if( hardware_name(3,1)=="5" ) return 84;
      }
      return (int) TotalTubes/GetNumML(hardware_name);
    }
    return (int) TotalTubes/GetNumML(hardware_name);    
  }

  int GetNumTubesPerLayer(const TString &hardware_name, int layer, int TotalTubes){
    if( hardware_name(0,3)=="BIR" || hardware_name(0,4)=="BMS4" || hardware_name(0,4)=="BMS6"){
      if( hardware_name(0,4)=="BIR3" ) return 33;
      if( layer <= 4){
	if( hardware_name(0,3)=="BMS" )	return 40;
	if( hardware_name(3,1)=="1" ) return 24;
	if( hardware_name(3,1)=="2" ) return 27;
	if( hardware_name(3,1)=="4" ) return 27;
	if( hardware_name(3,1)=="5" ) return 21;
      }
      return (int) TotalTubes/GetNumML(hardware_name)/GetNumLayersPerML(hardware_name);
    }
    return (int) TotalTubes/GetNumML(hardware_name)/GetNumLayersPerML(hardware_name);    
  }

  int GetNumML(const TString & hardware_name) {
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumML = 2;
    }
    else {
      derived_NumML = 2;
    }
  
    return derived_NumML;
  }

  int getLastTubeInLayer(int & firstTubeInLayer, const TString & hardware_name, int totalTubes) {
    int tubeID = firstTubeInLayer;

    // int derived_tube = 1;
    int derived_layer = 1;
    // int derived_ML = 1;

    // Get Number of X
    int derived_NumTubePerLayer = 1;
    // int derived_NumTubePerML = 1;
    int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    // derived_NumTubePerML = totalTubes / derived_NumML;
    derived_NumTubePerLayer = totalTubes / derived_NumLayer;

    // Corrections for derived_NumTubePerLayer
    if( hardware_name(0,4) == "BMS4" || hardware_name(0,4) == "BMS6" )
      derived_NumTubePerLayer = 48;
    if((hardware_name(5,2) == "11" || hardware_name(5,2) == "15")) {
      if( hardware_name(0,4) == "BIR1" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR2" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR3" )
	derived_NumTubePerLayer = 36;
      if( hardware_name(0,4) == "BIR4" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR5" )
	derived_NumTubePerLayer = 24;
    }
//     if( hardware_name(0,4)=="EIL4" && ( hardware_name(5,2)=="09" || hardware_name(5,2)=="01" ) ) // possible MdtIdHelper problem
//       derived_NumTubePerLayer = 54;

    // Now get X
    // derived_ML = (tubeID-1)/derived_NumTubePerML + 1;
    derived_layer = (tubeID-1)/derived_NumTubePerLayer + 1;
    // derived_tube = tubeID - (derived_layer - 1) * derived_NumTubePerLayer;
    //   if(derived_ML==2) derived_layer -= derived_NumLayerPerML; 

    // final info  
    int lastTubeInLayer = -1;
    if(derived_layer <= derived_NumLayer) 
      lastTubeInLayer = tubeID + derived_NumTubePerLayer - 1;
  
    return lastTubeInLayer;
  }

  int Get_ML_of_Layer(int & layer, const TString & hardware_name) {
    int derived_ML = 1;

    // Get Number of X
    // int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    // int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      // derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      // derived_NumML = 1;    
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      // derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      // derived_NumML = 2;
    }
    // derived_NumLayer = derived_NumLayerPerML * derived_NumML;
  
    // Now get X
    derived_ML = (layer-1)/derived_NumLayerPerML + 1;

    return derived_ML;
  }

  int getLastTubeInML(int & firstTubeInML, const TString & hardware_name, int totalTubes) {
    int tubeID = firstTubeInML;

    // int derived_tube = 1;
    // int derived_layer = 1;
    int derived_ML = 1;

    // Get Number of X
    // int derived_NumTubePerLayer = 1;
    int derived_NumTubePerML = 1;
    // int derived_NumLayer = 1;
    // int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      //derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      //derived_NumLayerPerML = 4;
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      //derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      //derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    // derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    derived_NumTubePerML = totalTubes / derived_NumML;
    //derived_NumTubePerLayer = totalTubes / derived_NumLayer;

#if 0
    // Corrections for derived_NumTubePerLayer
    if( hardware_name(0,4) == "BMS4" || hardware_name(0,4) == "BMS6" )
      derived_NumTubePerLayer = 48;
    if((hardware_name(5,2) == "11" || hardware_name(5,2) == "15")) {
      if( hardware_name(0,4) == "BIR1" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR2" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR3" )
	derived_NumTubePerLayer = 36;
      if( hardware_name(0,4) == "BIR4" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR5" )
	derived_NumTubePerLayer = 24;
    }
//     if( hardware_name(0,4)=="EIL4" && ( hardware_name(5,2)=="09" || hardware_name(5,2)=="01" ) ) // possible MdtIdHelper problem
//       derived_NumTubePerLayer = 54;
#endif

    // Now get X
    derived_ML = (tubeID-1)/derived_NumTubePerML + 1;
    // derived_layer = (tubeID-1)/derived_NumTubePerLayer + 1;
    // derived_tube = tubeID - (derived_layer - 1) * derived_NumTubePerLayer;
    // if(derived_ML==2) derived_layer -= derived_NumLayerPerML;

    // final info  
    int lastTubeInML = -1;
    if(derived_ML <= derived_NumML) 
      lastTubeInML = tubeID + derived_NumTubePerML - 1;
  
    return lastTubeInML;
  }

  int GetNumMezz( TH1F* h_tube ) {
    int totalTubes = h_tube->GetNbinsX();
    TString hardware_name = ((TString) h_tube->GetName())(0,7);
    int groupsPerLayer;    
    std::vector<TubeRange> tubeRange = getMezzRanges(hardware_name, totalTubes, groupsPerLayer);    
    return tubeRange.size();
  }

  std::vector<TubeRange> getMezzRanges(const TString & hardware_name, int totalTubes, int & groupsPerLayer) {
    // Get Number of X
    int derived_NumTubePerLayer = 1;
    // int derived_NumTubePerML = 1;
    int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    // derived_NumTubePerML = totalTubes / derived_NumML;
    derived_NumTubePerLayer = totalTubes / derived_NumLayer;

    // below is for the tube range
    int groupSize = 6;
    if( derived_NumLayerPerML == 3 )
      groupSize = 8;

    groupsPerLayer = derived_NumTubePerLayer / groupSize;

//     if( hardware_name == "EIL4A01" ) std::cout << "Chamber: " << hardware_name << ", #T: " << totalTubes << ", #L: " << derived_NumLayer << ", #T/ML: " << derived_NumTubePerML << ", #T/L: " << derived_NumTubePerLayer << ", #G/L: " << groupsPerLayer << ", GrpSz: " << groupSize << std::endl;

    int firstTubeID = 1;
    int lastTubeID = firstTubeID + groupSize - 1;
    std::vector<TubeRange> ranges;
    while ( lastTubeID <= totalTubes ) {
      TubeRange r(firstTubeID,lastTubeID);
      ranges.push_back(r);
      firstTubeID = lastTubeID+1;
      lastTubeID += groupSize;
    }

    return ranges;
  }

  int Get_ML_of_Mezz_degenerate(int &mezz, const TString & hardware_name, int totalTubes) {
    // Get Number of X
    int derived_NumTubePerLayer = 1;
    // int derived_NumTubePerML = 1;
    int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    // derived_NumTubePerML = totalTubes / derived_NumML;
    derived_NumTubePerLayer = totalTubes / derived_NumLayer;

    // below is for the tube range
    int groupSize = 6;
    if( derived_NumLayerPerML == 3 )
      groupSize = 8;

    int groupsPerLayer = derived_NumTubePerLayer / groupSize;

    if(mezz > groupsPerLayer) {
      mezz -= groupsPerLayer;
      return 2;
    }

    return 1;
  }

  // This version does not account for multilayers (on purpose, so it is single-valued)
  int Get_ML_of_Mezz(int &mezz, const TString & hardware_name, int totalTubes) {
    // Get Number of X
    int derived_NumTubePerLayer = 1;
    // int derived_NumTubePerML = 1;
    int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 1;
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    // derived_NumTubePerML = totalTubes / derived_NumML;
    derived_NumTubePerLayer = totalTubes / derived_NumLayer;

    // below is for the tube range
    int groupSize = 6;
    if( derived_NumLayerPerML == 3 )
      groupSize = 8;

    int groupsPerLayer = derived_NumTubePerLayer / groupSize;

    if(mezz > groupsPerLayer) {
      return 2;
    }

    return 1;
  }

  // This function does not account for multilayers (on purpose, so it is single-valued)
  int Get_Mezz_of_Tube(int & tubeID, const TString & hardware_name/*, int totalTubes*/, std::vector<TubeRange> & tubeRanges, int groupsPerLayer) {
    unsigned int rangeIndexOfTube = 9999;

    for(unsigned int i=0; i<tubeRanges.size(); i++) {
      if( tubeRanges.at(i).InRange(tubeID) ) {
	rangeIndexOfTube = i;
	break;
      }
    }
    if( rangeIndexOfTube == 9999 ) {
      std::cout << "ERROR: tubeRange problem in Get_Mezz_of_Tube()!" << std::endl;
      return 0;
    }

    unsigned int mezz = rangeIndexOfTube % groupsPerLayer + 1;
    unsigned int numML = GetNumML(hardware_name);
    if(numML==2 && rangeIndexOfTube>=tubeRanges.size()/2) {
      mezz += groupsPerLayer;
    }

    return mezz;
  }

  // This function does not account for multilayers (on purpose, so it is single-valued)
  int Get_Layer_of_Tube(int & tubeID, const TString & hardware_name, int totalTubes) {
    int derived_layer = 1;

    // Get Number of X
    int derived_NumTubePerLayer = 1;
    int derived_NumLayer = 1;
    int derived_NumLayerPerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumLayerPerML = 3;
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 1;    
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumLayerPerML = 4;
      derived_NumML = 2;
    }
    else {
      derived_NumLayerPerML = 3;
      derived_NumML = 2;
    }

    derived_NumLayer = derived_NumLayerPerML * derived_NumML;
    derived_NumTubePerLayer = totalTubes / derived_NumLayer;

    // Corrections for derived_NumTubePerLayer
    if( hardware_name(0,4) == "BMS4" || hardware_name(0,4) == "BMS6" )
      derived_NumTubePerLayer = 48;
    if((hardware_name(5,2) == "11" || hardware_name(5,2) == "15")) {
      if( hardware_name(0,4) == "BIR1" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR2" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR3" )
	derived_NumTubePerLayer = 36;
      if( hardware_name(0,4) == "BIR4" )
	derived_NumTubePerLayer = 30;
      if( hardware_name(0,4) == "BIR5" )
	derived_NumTubePerLayer = 24;
    }
//     if( hardware_name(0,4)=="EIL4" && ( hardware_name(5,2)=="09" || hardware_name(5,2)=="01" ) ) // possible MdtIdHelper problem
//       derived_NumTubePerLayer = 54;
  
    // Now get X
    derived_layer = (tubeID-1)/derived_NumTubePerLayer + 1;
    
    return derived_layer;
  }

  int Get_ML_of_Tube(int & tubeID, const TString & hardware_name, int totalTubes) {
    int derived_ML = 1;

    // Get Number of X
    int derived_NumTubePerML = 1;
    int derived_NumML = 1;
    if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
      derived_NumML = 1;
    }
    else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
      derived_NumML = 1;    
    }
    else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
      derived_NumML = 2;
    }
    else {
      derived_NumML = 2;
    }

    derived_NumTubePerML = totalTubes / derived_NumML;

    // Now get X
    derived_ML = (tubeID-1)/derived_NumTubePerML + 1;

    return derived_ML;
  }

  std::vector<TubeRange> getLayerRanges(const TString & hardware_name, int totalTubes) {
    int firstTubeID = 1;
    int lastTubeID;
    std::vector<TubeRange> ranges;
    while ( (lastTubeID = getLastTubeInLayer(firstTubeID, hardware_name, totalTubes)) != -1 ) {
      TubeRange r(firstTubeID,lastTubeID);
      ranges.push_back(r);
      firstTubeID = lastTubeID+1;
    }
    return ranges;
  }

  std::vector<TubeRange> getMLRanges(const TString & hardware_name, int totalTubes) {
    int firstTubeID = 1;
    int lastTubeID;
    std::vector<TubeRange> ranges;
    while ( (lastTubeID = getLastTubeInML(firstTubeID, hardware_name, totalTubes)) != -1 ) {
      TubeRange r(firstTubeID,lastTubeID);
      ranges.push_back(r);
      firstTubeID = lastTubeID+1;
    }
    return ranges;
  }

  std::vector<int>
  /*void*/ FindDeadMezz( TH1F* h_tube, const TString & hardware_name, std::vector<int> & deadML_v) {
//     std::cout << "In FindDeadMezz for " << hardware_name << std::endl;

    int totalTubes = h_tube->GetNbinsX();
    int groupsPerLayer;
    int numML = GetNumML(hardware_name);
    std::vector<TubeRange> tubeRange = getMezzRanges(hardware_name, totalTubes, groupsPerLayer);
    int numMezz = groupsPerLayer * numML;
    std::vector<double> means;
    std::vector<int> valuesAll;
    getAllBins(h_tube, valuesAll);
    sort(valuesAll.begin(), valuesAll.end());
    int crustSize = (valuesAll.size()) / 3;
    int crustCutoff = 0;
    if( valuesAll.size() > 0 ) 
      crustCutoff = valuesAll.at(valuesAll.size()-1-crustSize);
    for(unsigned int i=0; i<tubeRange.size(); i++) {
      std::vector<int> valuesInRange_crusted;
      getAllUnmaskedBinsInRangeBelowCrustCutoff(h_tube, valuesInRange_crusted, crustCutoff, tubeRange.at(i).x1, tubeRange.at(i).x2);    
//       std::cout << " values in range:"; for(int j=0; j<valuesInRange_crusted.size(); ++j) std::cout << " " << valuesInRange_crusted.at(j); std::cout << std::endl;
      if(valuesInRange_crusted.size() > 0) {
	means.push_back(getMean(valuesInRange_crusted));
      }
      else means.push_back(crustCutoff);
//       std::cout << " mean in range (" << tubeRange.at(i).x1 << "-" << tubeRange.at(i).x2 << "), #" << i << ": " << means.at(i) << ", (crustCutoff = " << crustCutoff << ")" << std::endl; 
    }

    // order into mezzes
    std::vector<std::vector<double> > mezz_means(numMezz); //outer vector indexes vectors of groups that belong to the mezz with that index
//     std::cout << " mezz_means: " << mezz_means.size() << std::endl; 
    for(unsigned int k=0; k<means.size(); k++) {    
      if(numML==2 && k>=means.size()/2) {
// 	std::cout << " k: " << k << ", (k % groupsPerLayer): " << (k % groupsPerLayer) << ", groupsPerLayer: " << groupsPerLayer << " (this is 2nd ML)" << std::endl; 
	mezz_means.at( (k % groupsPerLayer) + groupsPerLayer ).push_back(means.at(k));
      }
      else {
// 	std::cout << " k: " << k << ", (k % groupsPerLayer): " << (k % groupsPerLayer) << ", groupsPerLayer: " << groupsPerLayer << std::endl; 
	mezz_means.at( (k % groupsPerLayer) ).push_back(means.at(k));
      }
    }

    double mean_of_means = getMean(means);
    std::vector<int> deviant_mezz;
    for(unsigned int i=0; i<mezz_means.size(); ++i) {
      int mezz = i+1;
      bool deviant = true;
      for(unsigned int j=0; j<mezz_means.at(i).size(); j++ ) {
// 	std::cout << " mezz_means.at(" << i << ").at(" << j << "): " << mezz_means.at(i).at(j) << ",  mean_of_means: " <<  mean_of_means << std::endl;
	if( !(mezz_means.at(i).at(j) < 0.25 * mean_of_means) )
	  deviant = false;
      }
      if( deviant && !AinB(Get_ML_of_Mezz(mezz,hardware_name,totalTubes), deadML_v) )
	deviant_mezz.push_back(mezz);
    }
//     if( deviant_mezz.size() > 0 ) {
//       std::cout << "Dead mezz: "; displayList(deviant_mezz);
//     }

    return deviant_mezz;
  }

  std::vector<int>
  /*void*/ FindDeadLayer( TH1F* h_tube, const TString & hardware_name, std::vector<int> & deadML_v) {
    //   std::cout << "In FindDeadLayer" << std::endl;

    int totalTubes = h_tube->GetNbinsX();
    std::vector<TubeRange> tubeRange = getLayerRanges(hardware_name, totalTubes);
    std::vector<double> means;
    std::vector<int> valuesAll;

    getAllBins(h_tube, valuesAll);
    sort(valuesAll.begin(), valuesAll.end());

    int crustSize = (valuesAll.size()) / 3;
    int crustCutoff = 0;
    if( valuesAll.size() > 0 ) 
      crustCutoff = valuesAll.at(valuesAll.size()-1-crustSize);

    for(unsigned i=0; i<tubeRange.size(); i++) {
      std::vector<int> valuesInRange_crusted;
      getAllUnmaskedBinsInRangeBelowCrustCutoff(h_tube, valuesInRange_crusted, crustCutoff, tubeRange.at(i).x1, tubeRange.at(i).x2);    
      if(valuesInRange_crusted.size() > 0) {
	means.push_back(getMean(valuesInRange_crusted));
      }
      else means.push_back(crustCutoff);
      //     std::cout << " mean " << i << ": " << means.at(i) << std::endl; 
    }

    double mean_of_means = getMean(means);
    std::vector<int> deviant_layers;
    for(unsigned i=0; i<means.size(); ++i) {
      if( means.at(i) < 0.25 * mean_of_means ) {
	int layer = i+1;
	if( !AinB(Get_ML_of_Layer(layer,hardware_name), deadML_v) ) 
	  deviant_layers.push_back(layer);
      }
    }
    if( deviant_layers.size() > 0 ) {
      //     std::cout << "Dead layers: "; displayList(deviant_layers);
    }

    return deviant_layers;
  }

  bool AinB( int A, std::vector<int> & B ) {
    for(unsigned int i=0; i<B.size(); ++i) {
      if( B.at(i) == A )
	return true;
    }
    return false;
  }

  bool AinB( int A, const std::vector<int> * B ) {
    for(unsigned int i=0; i<B->size(); ++i) {
      if( B->at(i) == A )
	return true;
    }
    return false;
  }

  std::vector<int>
  /*void*/ FindDeadML( TH1F* h_tube, const TString & hardware_name) {
    //   std::cout << "In FindDeadML" << std::endl;

    int totalTubes = h_tube->GetNbinsX();
    std::vector<TubeRange> tubeRange = getMLRanges(hardware_name, totalTubes);
    std::vector<double> means;
    std::vector<int> valuesAll;
    getAllBins(h_tube, valuesAll);
    sort(valuesAll.begin(), valuesAll.end());
    int crustSize = (valuesAll.size()) / 3;
    int crustCutoff = 0;
    if( valuesAll.size() > 0 ) 
      crustCutoff = valuesAll.at(valuesAll.size()-1-crustSize);
    for(unsigned int i=0; i<tubeRange.size(); i++) {
      std::vector<int> valuesInRange_crusted;
      getAllUnmaskedBinsInRangeBelowCrustCutoff(h_tube, valuesInRange_crusted, crustCutoff, tubeRange.at(i).x1, tubeRange.at(i).x2);    
      if(valuesInRange_crusted.size() > 0) {
	means.push_back(getMean(valuesInRange_crusted));
      }
      else means.push_back(crustCutoff);
      //     std::cout << " mean " << i << ": " << means.at(i) << std::endl; 
    }

    double mean_of_means = getMean(means);
    std::vector<int> deviant_ML;
    for(unsigned i=0; i<means.size(); ++i) {
      if( means.at(i) < 0.25 * mean_of_means )
	deviant_ML.push_back(i+1);
    }
    if( deviant_ML.size() > 0 ) {
    }

  
    return deviant_ML;
  }

  void FillPDF(const std::string & inFilename, TH1F* hDead, TH1F* hNoise, TCanvas* c, 
	       const vector<int>* deadTubes, const vector<int>* deadASD, const vector<int>* deadMEZZ, const vector<int>* deadLayer, const vector<int>* deadML,
	       const vector<int>* noisyTubes, const vector<int>* noisyASD, const vector<int>* noisyMEZZ, const vector<int>* noisyLayer, const vector<int>* noisyML, 
	       int deadTubesChamberValidity, int noisyTubesChamberValidity,
	       bool draw_Mezz_L_ML_guidlines, bool draw_mean, bool draw_masked_tubes, bool separate_dead_noisy_histos, bool draw_validity_message, bool draw_histo_if_nothing_to_report) {

    if(!noisyML) return;
    if(!noisyLayer) return;
    if(!noisyMEZZ) return;
    if(!noisyASD) return;
    if(!deadASD) return;

    if( !draw_histo_if_nothing_to_report && deadTubes->size()==0 && noisyTubes->size()==0 && deadML->size()==0 && deadLayer->size()==0 && deadMEZZ->size()==0 ) return;

    if(!hDead) return;
    if(!hNoise) return;

    c->Clear();
    if( separate_dead_noisy_histos )
      c->Divide(2,1);
    TString chamberName = ((TString) hDead->GetName())(0,7);
    c->SetTitle(chamberName);

    vector<int> maskedTubesForDead = GetMaskedTubesForDead(hDead);
    TH1F* hOnlyDead = new TH1F(hDead->GetName(), hDead->GetTitle(), hDead->GetNbinsX(), 1, hDead->GetNbinsX());
    TH1F* hOnlyNoise = new TH1F(hNoise->GetName(), hNoise->GetTitle(), hNoise->GetNbinsX(), 1, hNoise->GetNbinsX());

    TString maskedStr = hDead->GetName();
    maskedStr += "_masked";
    TH1F* hOnlyMaskedDead = new TH1F(maskedStr, hDead->GetTitle(), hDead->GetNbinsX(), 1, hDead->GetNbinsX());

    // fill the layer range histo
    std::vector<TLine> rangeLines;
    std::vector<TLine> rangeLines_onlyMLLMezz;
    std::vector<TubeRange> tubeRange = getLayerRanges( chamberName, hDead->GetNbinsX());
    for(unsigned int i=0; i<tubeRange.size(); i++) {
      TLine l( hDead->GetBinLowEdge(tubeRange.at(i).x1)+hDead->GetBinWidth(tubeRange.at(i).x1)/2.0, LAYER_POS, hDead->GetBinLowEdge(tubeRange.at(i).x2)+hDead->GetBinWidth(tubeRange.at(i).x2)/2.0, LAYER_POS);
      if( i % 2 == 0 )
	l.SetLineColor(kViolet);
      else
	l.SetLineColor(kViolet+6);
      int layer = i+1;
      l.SetLineWidth(3);
      rangeLines_onlyMLLMezz.push_back(l);
      if( AinB(layer,deadLayer) ) {   // mark dead if in dead list
	l.SetLineColor(kRed);
	l.SetLineWidth(6);
	rangeLines.push_back(l);
      }
      else if( draw_Mezz_L_ML_guidlines ) {
	rangeLines.push_back(l);
      }
    }
    // fill the ML range histo
    tubeRange = getMLRanges( chamberName, hDead->GetNbinsX());
    for(unsigned int i=0; i<tubeRange.size(); i++) {
      TLine l( hDead->GetBinLowEdge(tubeRange.at(i).x1)+hDead->GetBinWidth(tubeRange.at(i).x1)/2.0, ML_POS, hDead->GetBinLowEdge(tubeRange.at(i).x2)+hDead->GetBinWidth(tubeRange.at(i).x2)/2.0, ML_POS);
      if( i % 2 == 0 )
	l.SetLineColor(kViolet);
      else
	l.SetLineColor(kViolet+6);
      int multilayer = i+1;
      l.SetLineWidth(3);
      rangeLines_onlyMLLMezz.push_back(l);
      if( AinB(multilayer,deadML) ) {   // mark dead if in dead list
	l.SetLineColor(kRed);
	l.SetLineWidth(6);
	rangeLines.push_back(l);
      }
      else if( draw_Mezz_L_ML_guidlines ) {
	l.SetLineWidth(3);
	rangeLines.push_back(l);
      }
    }
    // fill the mezz range histo
    int groupsPerLayer;
    tubeRange = getMezzRanges( chamberName, hDead->GetNbinsX(), groupsPerLayer);
    for(unsigned int i=0; i<tubeRange.size(); i++) {
      TLine l( hDead->GetBinLowEdge(tubeRange.at(i).x1)+hDead->GetBinWidth(tubeRange.at(i).x1)/2.0, MEZZ_POS, hDead->GetBinLowEdge(tubeRange.at(i).x2)+hDead->GetBinWidth(tubeRange.at(i).x2)/2.0, MEZZ_POS);
      l.SetLineColor(kViolet+(i % groupsPerLayer));
      int mezz = i % groupsPerLayer + 1;
      int numML = GetNumML(chamberName);
      if(numML==2 && i>=tubeRange.size()/2) {
	mezz += groupsPerLayer;
      }
      l.SetLineWidth(3);
      rangeLines_onlyMLLMezz.push_back(l);
      if( AinB(mezz,deadMEZZ) ) {   // mark dead if in dead list
	l.SetLineColor(kRed);
	l.SetLineWidth(6);
	rangeLines.push_back(l);
      }
      else if( draw_Mezz_L_ML_guidlines ) {
	l.SetLineWidth(3);
	rangeLines.push_back(l);
      }
    }

    // draw the mean/stddev
    std::vector<TLine> rangeLines_dead;
    double mean = GetMeanFromHist(hDead);
    double stddev = GetStandardDevFromHist(hDead, mean);
    double dev_low = mean - stddev;
    double dev_high = mean + stddev;
    if( dev_low < 1. ) dev_low = STDDEV_POS;
    if( dev_high > 2.*hDead->GetMaximum() ) dev_high = 2.*hDead->GetMaximum() - 1. ;
    if( mean > 1. ) {
      TLine l_mean( hDead->GetBinLowEdge(1), mean, hDead->GetBinLowEdge(hDead->GetNbinsX()), mean);
      l_mean.SetLineColor(kGreen+3);
      l_mean.SetLineWidth(2);
      rangeLines_dead.push_back(l_mean);
      TLine l_dev_low( hDead->GetBinLowEdge(1), dev_low, hDead->GetBinLowEdge(hDead->GetNbinsX()), dev_low);
      l_dev_low.SetLineColor(kGreen);
      l_dev_low.SetLineWidth(2);
      rangeLines_dead.push_back(l_dev_low);
      TLine l_dev_high( hDead->GetBinLowEdge(1), dev_high, hDead->GetBinLowEdge(hDead->GetNbinsX()), dev_high);
      l_dev_high.SetLineColor(kGreen);
      l_dev_high.SetLineWidth(2);
      rangeLines_dead.push_back(l_dev_high);
    }
    std::vector<TLine> rangeLines_noise;
    mean = GetMeanFromHist(hNoise);
    stddev = GetStandardDevFromHist(hNoise, mean);
    dev_low = mean - stddev;
    dev_high = mean + stddev;
    if( dev_low < 1. ) dev_low = STDDEV_POS;
    if( dev_high > 2.*hNoise->GetMaximum() ) dev_high = 2.*hNoise->GetMaximum() - 1. ;
    if( mean > 1. ) {
      TLine l_mean( hNoise->GetBinLowEdge(1), mean, hNoise->GetBinLowEdge(hNoise->GetNbinsX()), mean);
      l_mean.SetLineColor(kGreen+3);
      l_mean.SetLineWidth(2);
      rangeLines_noise.push_back(l_mean);
      TLine l_dev_low( hNoise->GetBinLowEdge(1), dev_low, hNoise->GetBinLowEdge(hNoise->GetNbinsX()), dev_low);
      l_dev_low.SetLineColor(kGreen);
      l_dev_low.SetLineWidth(2);
      rangeLines_noise.push_back(l_dev_low);
      TLine l_dev_high( hNoise->GetBinLowEdge(1), dev_high, hNoise->GetBinLowEdge(hNoise->GetNbinsX()), dev_high);
      l_dev_high.SetLineColor(kGreen);
      l_dev_high.SetLineWidth(2);
      rangeLines_noise.push_back(l_dev_high);
    }

    // noisy tubes
    for(unsigned i = 0 ; i!= noisyTubes->size(); ++i){
      int TubeId = noisyTubes->at(i);
      if( hNoise->At(TubeId) < 1. ) {
	hOnlyNoise->SetBinContent(TubeId, MARKER_POS_RED);
	hOnlyNoise->SetBinError(TubeId, 0.001);
      }
      else {
	if( separate_dead_noisy_histos )
	  hOnlyNoise->SetBinContent(TubeId, hNoise->GetBinContent(TubeId));
	else
	  hOnlyNoise->SetBinContent(TubeId, hDead->GetBinContent(TubeId));
	hOnlyNoise->SetBinError(TubeId, 0.001);
      }
    }
    // dead tubes
    for(unsigned i = 0; i!=deadTubes->size(); ++i){
      int TubeId = deadTubes->at(i);
      if( hDead->At(TubeId) < 1. ) {
	hOnlyDead->SetBinContent(TubeId, MARKER_POS_RED);
	hOnlyDead->SetBinError(TubeId, 0.001);
      }
      else {
	hOnlyDead->SetBinContent(TubeId, hDead->GetBinContent(TubeId));
	hOnlyDead->SetBinError(TubeId, 0.001);
      }
    }
    // masked (cutouts)
    for(unsigned i = 0; i!=maskedTubesForDead.size(); ++i){
      int TubeId = maskedTubesForDead.at(i);
      if( hDead->At(TubeId) < 1. ) {
	hOnlyMaskedDead->SetBinContent(TubeId, MARKER_POS_BLUE);
	hOnlyMaskedDead->SetBinError(TubeId, 0.001);
      }
      else {
	hOnlyMaskedDead->SetBinContent(TubeId, hDead->GetBinContent(TubeId));
	hOnlyMaskedDead->SetBinError(TubeId, 0.001);
      }
    }

//     for(unsigned i = 0; i!=maskedTubesForNoisy.size(); ++i){
//       int TubeId = maskedTubesForNoisy.at(i);
//       if( hNoise->At(TubeId) < 1. ) {
// 	hOnlyMaskedNoisy->SetBinContent(TubeId, MARKER_POS_BLUE);
// 	hOnlyMaskedNoisy->SetBinError(TubeId, 0.01);
//       }
//       else {
// 	hOnlyMaskedNoisy->SetBinContent(TubeId, hNoise->GetBinContent(TubeId));
// 	hOnlyMaskedNoisy->SetBinError(TubeId, 0.01);
//       }
//     }

    double maxy_dead = hDead->GetMaximum()+1.1;
    hDead->SetAxisRange(0.1,2*maxy_dead,"y");
    hOnlyDead->SetMarkerColor(kRed); hOnlyDead->SetMarkerStyle(22); hOnlyDead->SetMarkerSize(1); hOnlyDead->SetAxisRange(0.1,2*maxy_dead,"y");
    hOnlyMaskedDead->SetMarkerColor(kBlue); hOnlyMaskedDead->SetMarkerStyle(21); hOnlyMaskedDead->SetMarkerSize(.5); hOnlyMaskedDead->SetAxisRange(0.1,2*maxy_dead,"y");

    double maxy_noise = hNoise->GetMaximum()+1.1;
    hNoise->SetAxisRange(0.1,2*maxy_noise,"y");
    hOnlyNoise->SetMarkerColor(kRed); hOnlyNoise->SetMarkerStyle(22); hOnlyNoise->SetMarkerSize(1); hOnlyNoise->SetAxisRange(0.1,2*maxy_noise,"y");
//     hOnlyMaskedNoisy->SetMarkerColor(kBlue); hOnlyMaskedNoisy->SetMarkerStyle(21); hOnlyMaskedNoisy->SetMarkerSize(.5); hOnlyMaskedNoisy->SetAxisRange(0.1,2*maxy_noise,"y");

    TText validityFailAll(1,1,"FAILED VALIDITY CHECKS");
    TText validityFailDead(1,1,"FAILED VALIDITY CHECKS FOR DEAD TUBES");
    TText validityFailNoise(2,2,"FAILED VALIDITY CHECKS FOR NOISY TUBES");
    TText validityFailNoiseRetry(1,1,"VALIDITY: USING ALL HITS HISTO");
      
    c->cd(1);
    gPad->SetLogy();
    hDead->Draw(); 
    if( draw_masked_tubes ) 
      hOnlyMaskedDead->Draw("same"); 
    hOnlyDead->Draw("same");
    for(unsigned int i=0; i<rangeLines.size(); i++) rangeLines.at(i).Draw("same"); 
    if( draw_mean ) { for(unsigned int i=0; i<rangeLines_dead.size(); i++) rangeLines_dead.at(i).Draw("same"); }
    if( deadTubesChamberValidity != 1 && draw_validity_message )
      validityFailDead.Draw("same");
    if( separate_dead_noisy_histos ) {
      c->cd(2);
      gPad->SetLogy();
      hNoise->Draw();
      hOnlyNoise->Draw("same"); 
//       if( draw_masked_tubes ) hOnlyMaskedNoisy->Draw("same");
      for(unsigned int i=0; i<rangeLines.size(); i++) rangeLines_onlyMLLMezz.at(i).Draw("same"); 
      if( draw_mean ) { for(unsigned int i=0; i<rangeLines_dead.size(); i++) rangeLines_noise.at(i).Draw("same"); }
      if( draw_validity_message ) {
	if( noisyTubesChamberValidity == 2 )
	  validityFailNoiseRetry.Draw("same");
	else if( noisyTubesChamberValidity != 1 )
	  validityFailAll.Draw("same");
      }
    }
    else {
      if( noisyTubesChamberValidity != 1 && draw_validity_message )
	validityFailNoise.Draw("same");
      hOnlyNoise->Draw("same"); 
    }

    std::ostringstream Out;
    
    TString ecapStr = "MDTBADead";
    if(chamberName(0,1)=="B" && chamberName(4,1)=="C") ecapStr = "MDTBCDead";
    else if(chamberName(0,1)=="E" && chamberName(4,1)=="A") ecapStr = "MDTEADead";
    else if(chamberName(0,1)=="E" && chamberName(4,1)=="C") ecapStr = "MDTECDead";

    Out << inFilename << "." << ecapStr << ".pdf";

    c->Print(Out.str().c_str(), "pdf");
    delete hOnlyNoise;
    delete hOnlyDead;
    delete hOnlyMaskedDead;

  }

}
