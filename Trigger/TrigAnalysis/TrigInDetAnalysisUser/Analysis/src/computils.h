//
//   @file    computils.h         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: comparitor.cxx, v0.0   Sat Aug 30 2014 14:38:03 CEST  sutt $

#ifndef COMPUTILS_H
#define COMPUTILS_H

#include <iostream>
#include <string>
#include <vector>

#include "label.h"
#include "DrawLabel.h" 

#include "TStyle.h"
#include "TPad.h"
#include "TH1D.h"
#include "TH1.h"

#include "TLegend.h"


void ATLASFORAPP_LABEL( double x, double y, int color, double size ); 

void myText( Double_t x, Double_t y, Color_t color, const std::string& text, Double_t tsize);


/// return the current data and time
std::string stime();
static std::string release;



/// does a string contain the substring
bool contains( const std::string& s, const std::string& p);

/// does a file exist
bool exists( const std::string& filename );

/// tail of a string 
std::string tail( std::string s, const std::string& pattern );

/// match a file name
std::string globbed( const std::string& s );

void contents( std::vector<std::string>& keys, 
	       TDirectory* td, 
	       const std::string& directory="", 
	       const std::string& pattern="", 
	       const std::string& path="" );

double realmax( TH1* h, bool include_error=true );
double realmin( TH1* h, bool include_error=true );

template<typename T>
std::ostream& operator<<( std::ostream& s, std::vector<T>& v) { 
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << v[i];
  return s;
}


/// automatically set the range on a histogram
void xrange(TH1* h, bool symmetric=false );


/// slightly more convenient legend class
class Legend { 

public:

  Legend() : mleg(0) { }  

  Legend(double x1, double x2, double y1, double y2) { 
    mleg = new TLegend(x1, y1, x2, y2);
    mleg->SetBorderSize(0);
    mleg->SetTextFont(42);
    mleg->SetTextSize(0.04);
    mleg->SetFillStyle(3000);
    mleg->SetFillColor(0);
    mleg->SetLineColor(0);
  } 


  // Legend( const Legend& leg ) : mleg((TLegend*)leg.mleg->Clone()) { } 

 Legend(const Legend& legend) : mleg(legend.mleg) { } 

  ~Legend() { } 

  TLegend* legend() { return mleg; } 

  TLegend* operator->() { return mleg; }

private:

  TLegend* mleg;

}; 


static const int colours[6] = { 1, 2, kBlue-4, 6, kCyan-2, kGreen+2 };
static const int markers[6] = { 20, 24, 25, 26, 21, 22 };

/// generic plotter class - better to have one of these - make 
/// sure it can be configured however you like, line styles, 
/// marker types, legends etc 
/// now a template so can be used for TH1D and TH2D etc
template<typename T>
class tPlotter { 

public: 

  tPlotter(T* _htest=0, T* _href=0 , const std::string& s="") : 
    m_htest(_htest), m_href(_href), 
    m_plotfilename(s) { 
  }

  
  tPlotter(const tPlotter& p) : 
    m_htest(p.m_htest), m_href(p.m_href), 
    m_plotfilename(p.m_plotfilename){     
  }


  /// sadly, root manages all the histograms (does it really? 
  /// who can tell) so we mustn't delete anything just in case
  ~tPlotter() { } 

  std::string plotfilename() const { return m_plotfilename; }

  //  void Draw(const std::string& chain, unsigned int i, Legend& leg ) { 
  void Draw( unsigned int i, Legend& _leg, bool mean=false ) { 

    Legend leg = _leg;

    if ( htest() ) {
      gStyle->SetOptStat(0);
      if ( href() ) { 
	href()->SetLineColor(colours[i%6]);
	href()->SetLineStyle(2);
	href()->SetMarkerStyle(0);
      }
      //      href()->SetMarkerColor(href()->GetLineColor());
      htest()->SetLineColor(colours[i%6]);
      htest()->SetLineStyle(1);
      htest()->SetMarkerColor(htest()->GetLineColor());
      htest()->SetMarkerStyle(markers[i%6]);

      if(i==0)  { 
	if ( href() ) href()->Draw("hist][");
	else          htest()->Draw("ep");
      }

      // if(i==0)  htest()->Draw("ep");
     
      //      if ( contains(href()->GetName(),"sigma") ) href()->SetMinimum(0);

      if ( href() ) { 
	if ( contains(href()->GetName(),"_vs_")  || 
	     contains(href()->GetName(),"sigma") || 
	     contains(href()->GetName(),"_eff") ) href()->Draw("hist same][");
	else                                      href()->Draw("hist same");
      }

      htest()->Draw("ep same");

      // href()->Draw("lhistsame");
      // htest()->Draw("lhistsame");

      std::string key = m_plotfilename;

      char _mean[64];
      std::sprintf( _mean, " <t> = %3.2f ms", htest()->GetMean() );


      //      std::cout << "adding key " << key << std::endl; 

      // if      ( contains( key, "FastTrack" ) )  leg->AddEntry( htest(), "  Run 2: Fast Tracking", "p" );
      // else if ( contains( key, "EFID_" ) )      leg->AddEntry( htest(), "  Run 1: EFID Tracking", "p" );
      // else if ( contains( key, "_EFID" ) )      leg->AddEntry( htest(), "  Run 2: Precision Tracking", "p" );
      
      //      std::cout << "cost size " << key.size() << std::endl;

      static TH1D* hnull = new TH1D("hnull", "", 1, 0, 1);
      hnull->SetMarkerColor(kWhite);
      hnull->SetLineColor(kWhite);
      hnull->SetMarkerStyle(0);
      hnull->SetLineStyle(0);
      hnull->SetLineWidth(1e-5);
      hnull->SetMarkerSize(1e-5);

      
      if ( mean ) { 
	key += std::string(" : ");
       	if ( key.size()<47 ) { 
	  key += _mean;
	  leg->AddEntry( htest(), key.c_str(), "p" );
	}
	else { 
	  leg->AddEntry( htest(), key.c_str(), "p" );
	  leg->AddEntry( hnull, _mean, "p" );
	}
      }
      else leg->AddEntry( htest(), key.c_str(), "p" );

      leg->Draw();

    }
  }
  

  /// print the output 
  void Print(const std::string& s="") {
    if ( s!="" ) gPad->Print(s.c_str());
    else         gPad->Print(m_plotfilename.c_str());
  } 

  T* htest() { return m_htest; }
  T* href()  { return m_href; }

private:

  T* m_htest;
  T* m_href;


  std::string m_plotfilename;

};



typedef tPlotter<TH1> Plotter;



/// set of generic plots
class Plots : public std::vector<Plotter> { 
  
public:

 Plots(const std::string& s="") : m_name(s), m_logx(false), m_logy(false) { }
  
  void Max(double scale=1.1) {  
    
    if ( size()<1 ) return;

    double tmax = 0;
    for ( unsigned i=0 ; i<size() ; i++ ) {
      //   double rmref  = realmax( at(i).href(), false );
      double rmtest = realmax( at(i).htest() );

      if ( i==0 || tmax<rmtest ) tmax = rmtest; 
      //   if         ( tmax<rmref )  tmax = rmref;
 
      //      if ( i==0 || tmax<at(i).href()->GetMaximum() )  tmax = at(i).href()->GetMaximum(); 
      //      if         ( tmax<at(i).htest()->GetMaximum() ) tmax = at(i).htest()->GetMaximum(); 
    }
    
    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMaximum(scale*tmax);
      at(i).htest()->SetMaximum(scale*tmax);
      //   std::cout << "refmax  : " << at(i).href()->GetMaximum()  << std::endl;
      //   std::cout << "testmax : " << at(i).htest()->GetMaximum() << std::endl;
    } 

  }

  void Min( double scale ) { 

    if ( size()<1 ) return;

    if ( scale==0 ) { 
      for ( unsigned i=0 ; i<size() ; i++ ) {
	at(i).href()->SetMinimum(0);
	at(i).htest()->SetMinimum(0);
      }  
      return;
    }

    double tmin = 0;

    for ( unsigned i=0 ; i<size() ; i++ ) {
      // double rmref  = realmin( at(i).href(), false );
      double rmtest = realmin( at(i).htest() );

      if ( i==0 || tmin>rmtest ) tmin = rmtest;
      //      if         ( tmin>rmref  )  tmin = rmref; 
    }
    
    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMinimum(scale*tmin);
      at(i).htest()->SetMinimum(scale*tmin);
      if ( m_logy ) { 
	if ( at(i).href()->GetMinimum()<=0 )  at(i).href()->GetMinimum(1e-4);
	if ( at(i).htest()->GetMinimum()<=0 ) at(i).htest()->GetMinimum(1e-4);
      }

      if ( contains( at(i).href()->GetName(), "sigma" ) ) { 
	std::cout << "refmin  : " << at(i).href()->GetMinimum()  << std::endl;
	std::cout << "testmin : " << at(i).htest()->GetMinimum() << std::endl;
      } 
    }
  }
 

  void xrange(bool symmetric=false) { 
    for ( unsigned i=0 ; i<size() ; i++ ) { 
      ::xrange( at(i).href(), symmetric );
      // ::xrange( at(i).htest(), symmetric );
    }
  }
   
  void Draw( Legend& leg, bool means=false ) {  
    //   Max();
    for ( unsigned i=0 ; i<size() ; i++ ) at(i).Draw( i, leg, means );
    DrawLabel(0.1, 0.02, "built on "+stime()+release, kBlack, 0.03 );
    gPad->SetLogy(m_logy);
    gPad->SetLogx(m_logx);
  }

  void SetLogx( bool b=true ) { m_logx=b; } 
  void SetLogy( bool b=true ) { m_logy=b; } 

  std::string GetXaxisTitle() { 
    if ( size()>0 ) return at(0).htest()->GetXaxis()->GetTitle();
    return "";
  }

  std::string GetYaxisTitle() { 
    if ( size()>0 ) return at(0).htest()->GetYaxis()->GetTitle();
    return "";
  }

  void SetXaxisTitle(std::string s) { 
    if ( size()>0 ) at(0).htest()->GetXaxis()->SetTitle(s.c_str());
  }

  void SetYaxisTitle(std::string s) { 
    if ( size()>0 ) at(0).htest()->GetYaxis()->SetTitle(s.c_str());
  }


private:
  
  std::string m_name;

  bool m_logx;
  bool m_logy;
  

};



#endif // COMPUTILS_H
