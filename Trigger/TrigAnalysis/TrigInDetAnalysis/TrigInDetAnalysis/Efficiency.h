// emacs: this is -*- c++ -*-
//
//   @file    Efficiency.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: Efficiency.h, v0.0   Mon 26 Oct 2009 01:22:40 GMT sutt $


#ifndef __EFFICIENCY_H
#define __EFFICIENCY_H

#include <iostream>
#include <string>
#include <cmath>

#include "TH1F.h"
#include "TPad.h"
#include "TGraphAsymmErrors.h"

class Efficiency {

public:

  Efficiency(TH1F* h, const std::string& n="") {

    std::string effname = n;
    if ( effname=="" )  { 
      effname = std::string(h->GetName())+"_eff";
    }
    
    m_name = effname;

    m_hnumer = (TH1F*)h->Clone( (effname+"_n").c_str() );
    m_hdenom = (TH1F*)h->Clone( (effname+"_d").c_str() );
    m_heff   = (TH1F*)h->Clone( effname.c_str() );

    m_hmissed = (TH1F*)h->Clone( (effname+"_missed").c_str() );

    m_hnumer->Reset();
    m_hdenom->Reset();
    m_heff->Reset();
    m_hmissed->Reset();
    
  } 


  Efficiency(TH1F* hnum, TH1F* hden, const std::string& n, double scale=100) {

    std::string effname = n+"_eff";
    
    m_name = effname;

    m_hnumer = (TH1F*)hnum->Clone( (effname+"_n").c_str() );
    m_hdenom = (TH1F*)hden->Clone( (effname+"_d").c_str() );
    m_heff   = (TH1F*)hnum->Clone( effname.c_str() );

    m_hmissed = (TH1F*)hnum->Clone( (effname+"_missed").c_str() );

    m_heff->Reset();
    m_hmissed->Reset();

    finalise(scale);
  } 


  ~Efficiency() { } 

  std::string name() const { return m_name; } 

  void Fill( double x, double w=1) { 
    m_hnumer->Fill(float(x),float(w));
    m_hdenom->Fill(float(x),float(w));
  }

  void FillDenom( double x, float w=1) { 
    m_hdenom->Fill(float(x),float(w));
    m_hmissed->Fill(float(x),float(w));
  }


  void SetDenominator( TH1F* h ) {
    for ( int i=0 ; i<=h->GetNbinsX()+1 ; i++ ) {
      m_hdenom->SetBinContent( i, h->GetBinContent(i) );
      m_hdenom->SetBinError( i, h->GetBinError(i) );
    }
  }


  void SetNumerator( TH1F* h ) {
    for ( int i=0 ; i<=h->GetNbinsX()+1 ; i++ ) {
      m_hnumer->SetBinContent( i, h->GetBinContent(i) );
      m_hnumer->SetBinError( i, h->GetBinError(i) );
    }
  }

  double findTotalEfficiency() {
    double n_tot = 0;
    double d_tot = 0;
    
    for ( int i=1 ; i<=m_hdenom->GetNbinsX() ; i++ ) { 
      double n = m_hnumer->GetBinContent(i);
      double d = m_hdenom->GetBinContent(i);
      
      n_tot += n;
      d_tot += d;
    }

    if ( d_tot!=0 ) {
      return n_tot / d_tot;
    }

    return 0.;
  }

  void finalise(double scale=100) { 
    m_heff->Reset();
    for ( int i=1 ; i<=m_hdenom->GetNbinsX() ; i++ ) { 
      double n = m_hnumer->GetBinContent(i);
      double d = m_hdenom->GetBinContent(i);
      
      double e  = 0;
      double ee = 0;
      if ( d!=0 ) { 
	e  = n/d; 
	ee = e*(1-e)/d; 
      } 
      
      // need proper error calculation...
      m_heff->SetBinContent( i, scale*e );
      m_heff->SetBinError( i, scale*std::sqrt(ee) );

    }
  }

  TH1F* Hist() { return m_heff; }

  TGraphAsymmErrors* Bayes(double scale=100) const { 
    TGraphAsymmErrors* tg = new TGraphAsymmErrors( m_hnumer, m_hdenom, "cl=0.683 b(1,1) mode" );

    double* x      = tg->GetX();
    double* y      = tg->GetY();
  
    int n = tg->GetN();

    for ( int i=0 ; i<n ; i++ ) { 
    
      y[i]      *= scale;
 
      double yeup   = tg->GetErrorYhigh(i);
      double yedown = tg->GetErrorYlow(i);
      
      yeup   *= scale;
      yedown *= scale;
   
      /// ***REMOVED*** - can't just get/set specific y values
      /// I mean - why *some* functions to get x or y only get *all* the x values, 
      /// but the functions to return the errors only get ONE AT A TIME ?
      /// why isn't there a simple ScaleX() function? ***REMOVED*** 
      /// cares about, and basic functions missing
 
      tg->SetPoint( i, x[i], y[i] ); 
      
      tg->SetPointEYhigh( i, yeup ); 
      tg->SetPointEYlow( i, yedown ); 

      tg->SetPointEXhigh( i, 0 ); 
      tg->SetPointEXlow( i, 0 ); 
      
    } 
    
    return tg;

  }

private:

  std::string m_name;

  TH1F* m_hnumer;
  TH1F* m_hdenom;

  TH1F* m_hmissed;

  TH1F* m_heff;
  
};



inline std::ostream& operator<<( std::ostream& s, const Efficiency&  ) { 
  return s;
}


#endif  // __EFFICIENCY_H 










