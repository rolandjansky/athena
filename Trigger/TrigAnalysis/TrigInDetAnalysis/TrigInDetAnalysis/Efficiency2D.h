// emacs: this is -*- c++ -*-
//
//   @file    Efficiency2D.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: Efficiency2D.h, v0.0   Mon 26 Oct 2009 01:22:40 GMT sutt $


#ifndef __EFFICIENCY2D_H
#define __EFFICIENCY2D_H

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "TH2F.h"

#include "TrigInDetAnalysis/TIDDirectory.h"



class Efficiency2D {

public:

  Efficiency2D(TH2F* h, const std::string& n="") :
    m_name(n),
    m_xlimits(h->GetNbinsX()+1),  
    m_ylimits(h->GetNbinsY()+1) 
  {
    m_hnumer  = (TH2F*)h->Clone( "num" );
    m_hdenom  = (TH2F*)h->Clone( "denom" );
    m_hmissed = (TH2F*)h->Clone( "missed" );
    m_heff    = (TH2F*)h->Clone( "efficiency" );

    m_hnumer->SetDirectory(0);
    m_hdenom->SetDirectory(0);
    m_hmissed->SetDirectory(0);
    m_heff->SetDirectory(0);

    m_hnumer->Reset();
    m_hdenom->Reset();
    m_heff->Reset();
    m_hmissed->Reset();
  
    /// ***REMOVED***. why 
    /// isn't there an easy way to get the bin limits in a TH2 ?
    /// maybe there is, but it is obscure, so I extract them 
    /// myself and store them - what a complete waste of time. 
 
    TH1F* hy = (TH1F*)m_heff->ProjectionY("y",1,1,"");
    hy->SetDirectory(0);
    for ( int i=0 ; i<=hy->GetNbinsX() ; i++ ) { 
      m_ylimits[i] = hy->GetBinLowEdge(i+1);
      //      std::cout << "y " << i << " " << m_ylimits[i] << std::endl;
    }

    TH1F* hx = (TH1F*)m_heff->ProjectionX("x",1,1,"");
    hx->SetDirectory(0);
    for ( int i=0 ; i<=hx->GetNbinsX() ; i++ ) { 
      m_xlimits[i] = hx->GetBinLowEdge(i+1);
      // std::cout << "x " << i << " " << m_xlimits[i] << std::endl;      
    }

    delete hx;
    delete hy;
    
  } 

  ~Efficiency2D() { 
    delete m_heff; 
    delete m_hdenom; 
    delete m_hnumer; 
    delete m_hmissed; 
  } 

  const std::string& Name() const { return m_name; }

  void Fill( double x, double y, double w=1 ) { 
    m_hnumer->Fill(float(x),float(y),float(w));
    m_hdenom->Fill(float(x),float(y),float(w));
  }

  void FillDenom( double x, double y, float w=1 ) { 
    m_hdenom->Fill(float(x),float(y),float(w));
    m_hmissed->Fill(float(x),float(y),float(w));
  }


  void finalise() { 
    m_heff->Reset();
    for ( int i=1 ; i<=m_hdenom->GetNbinsX() ; i++ ) { 
      for ( int j=1 ; j<=m_hdenom->GetNbinsY() ; j++ ) { 
	double n = m_hnumer->GetBinContent(i,j);
	double d = m_hdenom->GetBinContent(i,j);
	
	double e  = 0;
	double ee = 0;
	if ( d!=0 ) { 
	  e  = n/d; 
	  ee = e*(1-e)/d; 
	} 
	
	// need proper error calculation...
	m_heff->SetBinContent( i, j, 100*e );
	m_heff->SetBinError( i, j, 100*std::sqrt(ee) );
      }
    }
  }    


  void Write(const std::string n="", const std::string& xy="xy") { 
    /// use the object name, but override with argument
    /// if given
    std::string  name = m_name;
    if ( n!="" ) name = n;
    if ( name!="" ) { 

      //      TDirectory* _cwd = gDirectory;

      //      std::cout << "eff dif " << gDirectory->GetName() << "/" ;

      TIDDirectory dir(name);
      dir.push();

      //      std::cout << gDirectory->GetName() << std::endl;

      m_heff->Write();
      m_hnumer->Write();
      m_hdenom->Write();
      m_hmissed->Write();

      TIDDirectory slices("slices");
      slices.push();

      if ( xy.find("x")!=std::string::npos ) { 
	for ( int i=1 ; i<=m_heff->GetNbinsX() ; i++ ) /* TH1F* h = */ SliceX(i);
	for ( int i=1 ; i<=m_heff->GetNbinsX() ; i++ ) /* TH1F* h = */ DenomSliceX(i);
       
      }

      if ( xy.find("y")!=std::string::npos ) { 
	for ( int i=1 ; i<=m_heff->GetNbinsY() ; i++ ) /* TH1F* h = */ SliceY(i);
	for ( int i=1 ; i<=m_heff->GetNbinsY() ; i++ ) /* TH1F* h = */ DenomSliceY(i);
      }

      slices.pop();

      dir.pop();

      //      _cwd->cd();
    }
   
  }
  
  TH2F* Hist() { return m_heff; }

  TH1F* SliceX(int i, const std::string& ="") { 
    TH1F* h = 0;
    std::string _s = m_name;
    char _n[64];
    if ( i>0 && i<=m_heff->GetNbinsX() ) {  
      sprintf(_n,"_X%03d", i); 
      //    h = (TH1F*)m_heff->ProjectionY( (_s+_n).c_str(), i, i, "e");
      h = (TH1F*)m_heff->ProjectionY( _n, i, i, "e");
      sprintf(_n, "[ %6f - %6f ]", m_xlimits[i-1], m_xlimits[i]); 
      h->SetTitle( _n );
    }
    return h;
  }

  TH1F* SliceY(int i, const std::string& ="") { 
    TH1F* h = 0;
    std::string _s = m_name;
    char _n[64];
    if ( i>0 && i<=m_heff->GetNbinsY() ) {  
      sprintf(_n,"_Y%03d", i); 
      //      h = (TH1F*)m_heff->ProjectionX( (_s+_n).c_str(), i, i, "e" );
      h = (TH1F*)m_heff->ProjectionX( _n, i, i, "e" );
      sprintf(_n, "[ %6f - %6f ]", m_ylimits[i-1], m_ylimits[i]); 
      h->SetTitle( _n );
    }
    return h;
  }



  TH1F* DenomSliceX(int i, const std::string& ="") {  		    
    TH1F* h = 0;
    std::string _s = m_name;
    char _n[64];
    if ( i>0 && i<=m_hdenom->GetNbinsX() ) {  
      sprintf(_n,"_DX%03d", i); 
      h = (TH1F*)m_hdenom->ProjectionY( _n, i, i, "e" );
      sprintf(_n, "[ %6f - %6f ]", m_xlimits[i-1], m_xlimits[i]); 
      h->SetTitle( _n );
    }
    return h;
  }



  TH1F* DenomSliceY(int i, const std::string& ="") {  		    
    TH1F* h = 0;
    std::string _s = m_name;
    char _n[64];
    if ( i>0 && i<=m_hdenom->GetNbinsY() ) {  
      sprintf(_n,"_DY%03d", i); 
      //      h = (TH1F*)m_heff->ProjectionX( (_s+_n).c_str(), i, i, "e" );
      h = (TH1F*)m_hdenom->ProjectionX( _n, i, i, "e" );
      sprintf(_n, "[ %6f - %6f ]", m_ylimits[i-1], m_ylimits[i]); 
      h->SetTitle( _n );
    }
    return h;
  }


  int GetNbinsX() const { return m_heff->GetNbinsX(); } 
  int GetNbinsY() const { return m_heff->GetNbinsY(); } 



private:

  std::string m_name;

  TH2F* m_hnumer;
  TH2F* m_hdenom;

  TH2F* m_hmissed;

  TH2F* m_heff;

  std::vector<float> m_xlimits;
  std::vector<float> m_ylimits;
  
};



inline std::ostream& operator<<( std::ostream& s, const Efficiency2D& _e ) { 
  return s << _e.Name();
}


#endif  // __EFFICIENCY2D_H 










