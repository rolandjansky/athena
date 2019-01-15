/**
 **     @file    generate.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>
#include <vector>
#include <string>
#include <iostream>

#include "TFile.h"
#include "TH1D.h"
#include "TPad.h"
#include "TStyle.h"



#include "BasicRandom.h"

#include "generate.h"
#include "rmsFrac.h"

namespace generate { 

static const bool printPad = false;

void Zero(TH1D* hin) {
  for ( int i=0 ; i<=hin->GetNbinsX()+1 ; i++ ) { 
    hin->SetBinContent(i, 0);
    hin->SetBinError(i, 0);
  }
}


double Entries(TH1D* hin) {
  double N = 0;
  for ( int i=1 ; i<=hin->GetNbinsX() ; i++ ) N += hin->GetBinContent(i);
  return N;
}


double get_mean( const std::vector<double>& m ) {   
  double mean = 0;
  for ( unsigned i=0 ; i<m.size() ; i++ ) mean += m[i];
  return mean/m.size();
}


double get_rms( const std::vector<double>& m ) {   
  double mean = get_mean( m );
  double rms = 0; 
  for ( unsigned i=0 ; i<m.size() ; i++ ) {
    double r = (m[i]-mean);
    rms += r*r;
  }
  return std::sqrt( rms/m.size() );
}





TH1D* integrate( TH1D* hin ) { 

  TH1D* hint = (TH1D*)hin->Clone( (std::string(hin->GetName())+"-cumulative").c_str() );

  Zero(hint);

  hint->SetBinContent(0,0);
  hint->SetBinContent(hin->GetNbinsX()+1,0);

  for ( int i=1 ; i<=hin->GetNbinsX()+1 ; i++ ) { 
    double c = hin->GetBinContent(i) + hint->GetBinContent(i-1);
    hint->SetBinContent(i, c); 
    hint->SetBinError(i, std::sqrt(c) );
  }

  hint->SetBinContent(hin->GetNbinsX()+1, hint->GetBinContent(hin->GetNbinsX()) );

  return hint;
}





void ScaleIntegral(TH1D* hin ) {

  double N = hin->GetBinContent(hin->GetNbinsX());
  
  for ( int i=0 ; i<=hin->GetNbinsX()+1 ; i++ ) { 
    
    double n = hin->GetBinContent(i);

    double e = n/N;

    double ee = std::sqrt(e*(1-e)/N);

    hin->SetBinContent(i, e);
    hin->SetBinError(i, ee);
  }
}




TH1D* PDF(TH1D* hin) {
  TH1D* h = integrate( hin );
  h->SetDirectory(0);
  ScaleIntegral( h );  
  return h;
}



double Gradient(TH1D* hin) { 
  double gmax = 0;
  double imax = 0;
  for ( int i=2 ; i<hin->GetNbinsX() ; i++ ) { 
    double g = hin->GetBinContent(i+1)-hin->GetBinContent(i-1);
    if ( std::fabs(gmax)<std::fabs(g) ) {
      gmax = g; 
      imax = i;
    }
  }
  return gmax/hin->GetBinError(imax);
}



int iGradient(TH1D* hin) { 
  double gmax = 0;
  double imax = 0;
  for ( int i=2 ; i<hin->GetNbinsX() ; i++ ) { 
    double g = hin->GetBinContent(i+1)-hin->GetBinContent(i-1);
    if ( std::fabs(gmax)<std::fabs(g) ) {
      gmax = g; 
      imax = i;
    }
  }
  return imax;
}








/// smooth a distribution about the maximum
/// NB: maximum is not smoothed
TH1D* smooth( TH1D* hin, bool sym ) { 

  TH1D* hsmooth = (TH1D*)hin->Clone( (std::string(hin->GetName())+"-smooth").c_str() );
  hsmooth->SetDirectory(0);

  Zero( hsmooth );

  //  double _w[5] = { -3, 12, 17, 12, -3 };
  //  double w[7] = { -2, 3, 6, 7, 6, 3, -2 };
  //  double _w[11] = { -36, 9, 44, 69, 84, 89, 84, 69, 44, 9, -36 };
  //  double* w = _w+5;

  double mg = 0;
  int   img = 0;

  if ( sym ) { 

    TH1D* hint = integrate( hin ) ; 
    hint->SetDirectory(0);

    for ( int i=2 ; i<hsmooth->GetNbinsX() ; i++ ) { 
      double gradient =  std::fabs(0.5*(hint->GetBinContent(i+1) - hint->GetBinContent(i-1)));
      if ( gradient>mg ) { 
	mg = gradient;
	img = i; 
      }
    }

    delete hint;
 
  }
  
  //  double sfac = std::sqrt(Entries(hin)/1000)*0.1;
  const double sfac = 0.1;

  //  std::cout << "max gradient " << img << " " << mg << std::endl;
    
  for ( int i=1 ; i<=hsmooth->GetNbinsX() ; i++ ) { 
  
    /// a bit ad hoc, but seems to work  
    int Nbins = 2*sfac*std::fabs( i-img ); 

    //    Nbins = 0;
    
    //    std::cout << "bin " << i << "\tn " << Nbins << std::endl; 
    
    double  sum = 0;
    int    wsum = 0;
    
    for ( int j=i-Nbins ; j<=i+Nbins ; j++ ) { 
      if ( j<1 ) continue;
      if ( j>hin->GetNbinsX() ) break;
      sum  += hin->GetBinContent(j);
      wsum += 1;
    }
    
    if ( wsum>0 ) sum /= wsum;
    
    //    std::cout << i << " smoothed " << sum << " " << hin->GetBinContent(i) << std::endl;
    
    hsmooth->SetBinContent(i, sum); 
    hsmooth->SetBinError(i, 0 );
    
  }
  
  return hsmooth;

}





  hist_generator::hist_generator(TH1D* h, bool _smooth ) : mrandom(0) { 
    
  /// save the original histogram (should save a copy ho hum) 
  mraw = h;
  
  if ( _smooth ) {
    /// smooth the original
    msmooth = smooth( h );
  }
  else { 
    /// else simply clone the original
    msmooth = (TH1D*)h->Clone( (std::string(h->GetName())+"-smooth").c_str() );
    msmooth->SetDirectory(0);
  }

  
  /// generate a pdf from the smoothed distribution
  ms = PDF( msmooth );
  delete msmooth;
  
  ms->SetMinimum(0);
  ms->SetMaximum(1);
  
  my.push_back( 0 );
  mx.push_back( ms->GetBinLowEdge(1) );
  
  
  for ( int i=1 ; i<=ms->GetNbinsX() ; i++ ) { 
    mx.push_back( ms->GetBinLowEdge(i+1) );
    my.push_back( ms->GetBinContent(i) );
  }
  
  mdy.resize(mx.size());
  mdx.resize(mx.size());
  mdxdy.resize(mx.size());
  
  //    std::cout << "calculating gradients ..." << std::endl;
  
  for ( unsigned i=0 ; i<my.size()-1 ; i++ ) { 
      double dy = my[i+1] - my[i];
      double dx = mx[i+1] - mx[i];

      double dxdy = dx/dy;

      mdy[i] = dy;
      mdx[i] = dx;
      mdxdy[i] = dxdy;
  }
  
  /// assign the actual generator
  /// use a shared generator
  mrandom = new BasicRandom(true);
  /// use a dedicated generator
  /// mrandom = new BasicRandom(false);
  
}
  




int Nevent_min = 0;
int Nevent_max = 5000;

double _frac = 0.95;

/// given a distribution, gnerate some number of pseudo 
/// experiments to estimate the uncertainties in the mean 
/// and rms more appropriately (root errors assume a 
/// gaussian approximation)

experiment::experiment( TH1D* h, int Nexperiments, int fevents ) 
  : m_N(Nexperiments) { 
    
    std::vector<double>& mean = m_mean;
    std::vector<double>& rms  = m_rms;
    
    mean.clear();
    rms.clear();

    double fnscale = 1;

#if 0
    std::cout << "mean "  << h->GetMean() << " +- " << h->GetMeanError() << " (" << findMean(h,0.95) << ")"
	      << "\trms " << h->GetRMS()  << " +- " << h->GetRMSError()  << " (" << rms95(h) << ")"
	      << "\thistogram" << std::endl;
#endif    

    //    std::cout << "experiment() actual histogram " << std::endl;
    
    //    m_hmean = h->GetMean();
    //    m_hrms  = rms95(h, findMean(h,0.95) );
    m_hmean = findMean(h, _frac);
    m_hrms  = rmsFrac(h, _frac, m_hmean );
    //   m_hrms  = rms95(h, m_hmean );

    //    std::cout << "central mean " << m_hmean << "\trms95 " << m_hrms << std::endl; 

    h->GetXaxis()->SetRange(1,h->GetNbinsX());

    //    mg = new hist_generator( h, false ); 
    mg = new hist_generator( h ); 
    generator_base& g = *mg;
    
    
    
    //    int Nevents = Entries(h);
    int Nevents = h->GetEffectiveEntries()+0.5; // Entries(h);

    //    std::cout << "Nevents " << Nevents << "\tfevents " << fevents << std::endl;

    if ( fevents!=0 ) Nevents = fevents;


    //    std::cout << "Nevents: " << Entries(h) << "\tweights: " << Nevents << " ( max " << Nevent_max << "  min " << Nevent_min << ")"; // << std::endl; 

    /// NB: Here we need to keep track of the number of events in the 
    ///     histogram. To correctly estimate the uncertainty on the rms, 
    ///     we should use the same number of entries N, for each 
    ///     pseudo-experiment but this can be computaionally very 
    ///     expensive and time consuming, so we generate in this 
    ///     case with fewer events, n, and estimate the uncertainty
    ///     by scaling by sqrt( n/N )
     
    double _Nevents = Nevents; 

    if ( Nevents>Nevent_max ) { 
      ///      std::cout << "generate() " << h->GetName() << " requested " << Nevents << " using default max of " << Nevent_max << std::endl;
      Nevents = Nevent_max;
    }

    if ( Nevents<Nevent_min ) Nevents = Nevent_min;

    /// set the correct error scaling to the correct number of events
    fnscale = std::sqrt( Nevents/_Nevents );

    //    std::cout << "\tusing: " << Nevents << std::endl;

    TH1D* hmean = 0;
    TH1D* hrms  = 0;
    
    if ( printPad ) { 
      gStyle->SetOptStat(1111);    
      hmean = new TH1D("mean", "", 500, -0.001, 0.001);
      hrms  = new TH1D("rms",  "", 500, 0.001, 0.01);
      hmean->SetDirectory(0);
      hrms->SetDirectory(0);
    }

    //    std::cout << "experiment() pseudo data " << std::endl;

    /// loop over the pseudo experiments 

    m_THrms = new TH1D("rms95", "", 50, 0.0006, 0.0009 );
    m_THrms->SetDirectory(0);

    for ( int j=0 ; j<m_N ; j++ ) { 
      
      TH1D* h3 = (TH1D*)h->Clone();
      Zero( h3 );
      
      for ( int i=Nevents ; i-- ; )  h3->Fill( g.generate() );
      
      double _mean95  = findMean( h3, _frac );
      //      double _rms95 = 1.1479538518*rmsFrac( h3, 0.95, _mean95 ); 
      double _rms95 = rmsFrac( h3, _frac, _mean95 ); 


#if 0
      std::cout << j << "\texpt mean " << _mean95 << "\trms95 " << _rms95 << std::endl; 

      if ( std::string(h->GetTitle()).find("[ 34.4747 - 42.1697 ]")!=std::string::npos ) { 

      std::cout << "mean " << h3->GetMean() << " +- " << h3->GetMeanError() 
		<< " (" << _mean95 << ") " 
		<< "\trms " << h3->GetRMS() << " +- " << h3->GetRMSError()  
		<< " (" << _rms95 << ") " 
		<< "\t pseudo data (" << Nevents << " events)"  
		<< std::endl;
    } 

#endif    
      

      //  mean.push_back( h3->GetMean() );
      mean.push_back( _mean95 );
      rms.push_back( _rms95 );
      
      m_THrms->Fill( _rms95 );

      delete h3;
    }
    
    //   std::cout << "averages" << std::endl;
    
#if 0
    if ( std::string(h->GetTitle()).find("[ 34.4747 - 42.1697 ]")!=std::string::npos ) { 
    /// original values
    std::cout << "mean "  << h->GetMean() << " +- " << h->GetMeanError() << " (" << findMean(h,0.95) << ")"
	      << "\trms " << h->GetRMS()  << " +- " << h->GetRMSError()  << " (" << rms95(h) << ")"
	      << "\thistogram" << std::endl;

    /// pseudo experiment values
    std::cout << "mean " << get_mean(mean) << " +- " << get_rms(mean);
    std::cout << "\trms " << get_mean(rms) << " +- " << get_rms(rms) 
	      << " after " << Nexperiments << " experiment" << ( Nexperiments==1 ? "" : "s" ) 
	      << std::endl;
    }
#endif
    
    m_global_mean       = get_mean(mean);
    m_global_mean_error = get_rms(mean)*fnscale;
    
    m_global_rms       = get_mean(rms);
    m_global_rms_error = get_rms(rms)*fnscale;
    delete mg;
    

    //    std::cout << "\t\t mean " << m_global_mean << " +- " <<  m_global_mean_error 
    //	      << "\t mean " << m_global_rms  << " +- " <<  m_global_rms_error
    //	      << std::endl;

}

} // namespace generate

