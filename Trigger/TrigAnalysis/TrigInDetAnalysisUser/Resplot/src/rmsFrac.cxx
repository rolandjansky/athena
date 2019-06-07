/**
 **     @file    rmsFrac.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:27 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/



#include <cmath>

#include "rmsFrac.h"

namespace generate {



double GetEntries(TH1D* h, int ilow, int ihi) { 
  double sum = 0;
  for ( int i=ilow ; i<=ihi ; i++ ) sum += h->GetBinContent(i);
  return sum;
}

double GetEntries(TH1D* h) { 
  return GetEntries(h,1,h->GetNbinsX());
}



void getRange(TH1D* s, int imax, double frac, 
	      int& lowerbin, int& upperbin, double& lowerfrac, double& upperfrac ) { 
  
  upperbin = imax;
  lowerbin = imax;
  
  upperfrac = 0; 
  lowerfrac = 0; 
  
  double entries  = GetEntries(s,0,int(s->GetNbinsX())+1);

  if ( entries>0 ) {  

    double sumn  = s->GetBinContent(imax); 
    double tsum  = s->GetBinContent(imax); 
    
    int i=1;
    while ( true ) { 
      
      int _upperbin = imax+i;
      int _lowerbin = imax-i;
      
      if ( _upperbin>s->GetNbinsX() || _lowerbin<1 ) break; 
      
      tsum += s->GetBinContent(_upperbin) + s->GetBinContent(_lowerbin);
      
      //      std::cout << i << " frac: " << lowersum 
      //		<< "\tx " << s->GetBinCenter(lowerbin) 
      //		<< "\t "  << s->GetBinCenter(upperbin)
      //		<< "\ttsum " << tsum
      //		<< "\tentries " << entries
      //		<< std::endl;

      if ( tsum>=entries*frac ) break;

      sumn = tsum;

      lowerfrac = sumn/entries;
            
      upperbin = _upperbin;
      lowerbin = _lowerbin;

      i++;
    }

    upperfrac = tsum/entries;
  }
  
}



double findMean(TH1D* s, double frac=0.95) {

  const bool interpolate_flag = true;

  //  double entries  = GetEntries(s,0,int(s->GetNbinsX())+1);
  double entries  = s->GetEffectiveEntries(); // s,0,int(s->GetNbinsX())+1);

  //  std::cout << "\nfindMean() " << s->GetName() << "\tentries: " << entries << std::endl;

  /// not enough entries for this fraction, 
  /// ie we want 0.95 then need 5% to be 1 
  /// events, so need at least 20 events 
  if ( (1-frac)*entries<1 ) return 0;
  
  s->GetXaxis()->SetRange(1,s->GetNbinsX());

  double mean  = s->GetMean();
  double meane = s->GetMeanError();

  int upperbin = 0;
  int lowerbin = 0;
  
  double upperfrac = 0; 
  double lowerfrac = 0; 

  int imax = 0;
 
  int it=0;

  /// maximum 20 iterations, calculating the mean of 95% until stable
  for ( ; it<20 ; it++ ) { 


    //    std::cout << it << "\tmean " << mean << " +- " << meane << std::endl; 

    imax =  s->GetXaxis()->FindBin(mean);

    upperbin = imax;
    lowerbin = imax;

    upperfrac = 0; 
    lowerfrac = 0; 
    
    getRange( s, imax, frac, lowerbin, upperbin, lowerfrac, upperfrac );

    s->GetXaxis()->SetRange(lowerbin, upperbin);
    
    double m  = s->GetMean();
    double me = s->GetMeanError(); 
    
    if ( it>0 ) { 
      if ( mean==m || 
	   std::fabs(mean-m)<me*1e-5 || 
	   std::fabs(mean-m)<meane*1e-5 ) { 
	mean = m;
	meane = me;
       	//  std::cout << "break after " << it << " iterations" << std::endl;
	break; 
      }
    }
    
    if ( it>=20 ) { 
      std::cerr << s->GetName() << "\tMax iterations " << it << " reached" << std::endl;
    }
    
    mean = m;
    meane = me;
    
  }
  
  //  std::cout << s->GetName() << "\tmean " << mean << " +- " << meane << std::endl;

  if  ( interpolate_flag ) { 
  
    s->GetXaxis()->SetRange(lowerbin-1, upperbin+1);
    
    double m  = s->GetMean();
    //  double me = s->GetMeanError(); 
    
    s->GetXaxis()->SetRange(1,s->GetNbinsX());
    
    if ( upperfrac==lowerfrac ) return 0;
    
    double inter_mean  = mean + (frac-lowerfrac)*(m-mean)/(upperfrac-lowerfrac);
    
    return inter_mean;
  }
  else { 
    s->GetXaxis()->SetRange(1,s->GetNbinsX());
    return mean; 
  }
  
}
  
  
  
int findMax(TH1D* s) { 
  int imax = 1;
  for ( int i=2 ; i<=s->GetNbinsX() ; i++ ) {
    /// find maximum bin
    if ( s->GetBinContent(i)>s->GetBinContent(imax) ) imax = i;
  } 
  return imax;
}
  
  
  
  
double rmsFrac(TH1D* s, double frac, double mean) {

  double rms  = 0;
  double erms = 0;

  const bool interpolate_flag = true;

  if ( s==0 )            { std::cerr << "rmsFrac() histogram s = " << s << std::endl; return 0; }
  if ( s->GetXaxis()==0) { std::cerr << "rmsFrac() histogram s->GetXaxis() not definied for histogram " << s->GetName() << std::endl; return 0; }

  //  std::cout << "rmsFrac() " << s->GetName() << " " << GetEntries(s)   << " " << GetEntries(s, 0, s->GetNbinsX()+1)  << std::endl;

  //  double entries = GetEntries(s,0,int(s->GetNbinsX())+1);
  double entries = s->GetEffectiveEntries(); // s,0,int(s->GetNbinsX())+1);

  /// not enough entries for this fraction, 
  /// ie we want 0.95 then need 5% to be 1 
  /// events, so need at least 20 events 
  if ( (1-frac)*entries<1 ) return 0;

  s->GetXaxis()->SetRange(1,s->GetNbinsX());
 
  double m = mean;  /// lazyness

  int imax =  s->GetXaxis()->FindBin(m);
  
  //  std::cout << "rmsFrac() mean " << m << " " << imax << " max " << s->GetBinCenter(findMax(s)) << " " << findMax(s) << std::endl;
  //  std::cout << "rmsFrac() entries: " << entries << "\t" << GetEntries(s) << "\t" << GetEntries(s,0,s->GetNbinsX()+1) << std::endl;
  
  int upperbin = imax;
  int lowerbin = imax;
  
  double upperfrac = 0; 
  double lowerfrac = 0; 
  
  if ( entries>0 ) {  
    getRange( s, imax, frac, lowerbin, upperbin, lowerfrac, upperfrac );
  }    

  //  std::cout << "rmsFrac() " << s->GetName() << "\tlower bin " << lowerbin << "\t upper bin " << upperbin << std::endl;

  //  if ( upperbin!=lowerbin ) { 
  if ( true ) {  /// technically not correct, since the rms is not definid for only one bin - really need NARROWER BINS!! 
 
    std::vector<double> stats;

    //    std::cout << "rmsFrac() GetEntries " << s->GetName() << " " << GetEntries(s) << std::endl; 

    if ( interpolate_flag ) { // && upperfrac!=lowerfrac ) { 

      double rlower  = 0;
      double erlower = 0;
      s->GetXaxis()->SetRange(lowerbin, upperbin);
      rlower  = s->GetRMS();     
      //      std::cout << "rmsFrac() GetEntries " << s->GetName() << " " << GetEntries(s,lowerbin,upperbin)/sentries << "\trlower " << rlower << std::endl; 

      double rupper  = 0;
      double erupper = 0;
      s->GetXaxis()->SetRange(lowerbin-1, upperbin+1);
      rupper  = s->GetRMS();     
      //      std::cout << "rmsFrac() GetEntries " << s->GetName() << " " << GetEntries(s,lowerbin-1,upperbin+1)/sentries << "\trupper " << rupper << std::endl; 

      //      std::cout << "rmsFrac() Range " << s->GetBinLowEdge(lowerbin) << " - " << s->GetBinLowEdge(upperbin+1) << std::endl; 

      if ( upperfrac!=lowerfrac ) { 
	rms  =  rlower + (frac-lowerfrac)*(rupper-rlower)/(upperfrac-lowerfrac);
	erms = erlower + (frac-lowerfrac)*(erupper-erlower)/(upperfrac-lowerfrac);
      }
      else { 
	rms = erms = 0;
      }

    }
    else {
      s->GetXaxis()->SetRange(lowerbin, upperbin);      
      rms  = s->GetRMS();     
    }
  }


  s->GetXaxis()->SetRange(1,s->GetNbinsX());
  
  return rms;

}

  
double rmsFrac(TH1D* s, double frac) {
  return rmsFrac(s, frac, findMean( s, frac ) );
}


double rms95(TH1D* s, double mean) {
  double rms = rmsFrac(s, 0.95, mean);
  //  printf("rms95 %12.10lf  %12.10lf inv %12.10lf\n", rms,  rms/0.8711151572, rms*1.1479538518 ); 
  // rms *= 1.1479538518;  
  s->GetXaxis()->SetRange(1,s->GetNbinsX());
  return rms;
}

double rms95(TH1D* s) {
  double rms = rmsFrac(s, 0.95);
  //  printf("rms95 %12.10lf  %12.10lf inv %12.10lf\n", rms,  rms/0.8711151572, rms*1.1479538518 ); 
  // rms *= 1.1479538518;  
  s->GetXaxis()->SetRange(1,s->GetNbinsX());
  return rms;
}

}
