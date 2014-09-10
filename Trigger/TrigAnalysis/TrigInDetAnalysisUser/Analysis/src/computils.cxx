//
//   @file    computils.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: comparitor.cxx, v0.0  Sat Aug 30 2014 14:38:03 CEST  sutt $


#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h> 

#include <iostream>
#include <string>
#include <vector>

#include "label.h"
#include "DrawLabel.h" 

#include "TFile.h"
#include "TH1D.h"

#include "TLegend.h"
#include "TColor.h"

#include "computils.h"

void ATLASFORAPP_LABEL( double x, double  y, int color, double size ) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.SetTextSize(size);
  l.DrawLatex(x,y,"ATLAS");
  l.SetTextFont(52);
  l.DrawLatex(x+0.13,y,"For Approval");
}

void myText( Double_t x, Double_t y, Color_t color, const std::string& text, Double_t tsize) {

  //Double_t tsize=0.05;
  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x,y,text.c_str());
}


std::string stime() { 
  time_t t;
  time(&t);
  return label("%s", ctime(&t) );
}


bool contains( const std::string& s, const std::string& p) { 
  return (s.find(p)!=std::string::npos);
}


bool exists( const std::string& filename ) { 
  struct stat sb;
  if ( stat( filename.c_str(), &sb)==0 ) return true; // && S_ISREG(sb.st_mode ))
  else return false;
}


double integral( TH1* h ) { 
  double n=0;
  for ( int i=h->GetNbinsX() ; i>0 ; i-- ) n += h->GetBinContent(i);
  return n;
}


double realmax( TH1* h, bool include_error ) { 
  double rm = 0;
  if ( h->GetNbinsX()==0 )  return 0; 

  rm = h->GetBinContent(1);
  if ( include_error ) rm += h->GetBinError(1);

  for ( int i=2 ; i<=h->GetNbinsX() ; i++ ) { 
    double re = h->GetBinContent(i);
    if ( include_error ) re += h->GetBinError(i);
    if ( rm<re ) rm = re;
  }

  return rm;
}


double realmin( TH1* h, bool include_error ) { 
  double rm = 0;
  if ( h->GetNbinsX()==0 )  return 0; 

  bool first = true;
  for ( int i=2 ; i<=h->GetNbinsX() ; i++ ) { 

    double re = h->GetBinContent(i);

    if ( re ) {
      if ( include_error ) re -= h->GetBinError(i);
      if ( first || rm>re ) rm = re;
      first = false;
    } 
  }

  return rm;
}


double hmean( TH1* h ) {
  double N = integral(h);
  double n=0;
  for ( int i=h->GetNbinsX() ; i>0 ; i-- ) { 
    n += h->GetBinContent(i);
    if ( 2*n>N ) return h->GetBinCenter(i);
  }
  return 0;
}


void xrange(TH1* h, bool symmetric ) { 
  int ilo = 1;
  int ihi = h->GetNbinsX();

  double content = integral(h);

  if ( content == 0 ) return;

#if 1

  /// zoom on non-empty bins
  for ( ; ilo<=ihi ; ilo++ ) if ( h->GetBinContent(ilo)>0 ) break; 
  for ( ; ihi>=ilo ; ihi-- ) if ( h->GetBinContent(ihi)>0 ) break;

#else

  /// experimental zooming ...

  /// zoom on the central region containg more than some fraction X
 
  double icont = 1/content;
  
  double flo = 0;
  double fhi = 0;
  for ( ; ilo<=ihi ; ilo++ ) { 
    flo += h->GetBinContent(ilo);
    if ( (flo*icont)>0.0005 )  break;
  }
 
  for ( ; ihi>=ilo  ; ihi-- ) { 
    fhi += h->GetBinContent(ihi);
    if ( (fhi*icont)>0.0005 )  break;
  }

#endif 

  int delta_lo = ilo-1;
  int delta_hi = h->GetNbinsX()-ihi;

  if ( symmetric ) { 
    if ( delta_hi<delta_lo ) h->GetXaxis()->SetRange( 1+delta_hi, ihi );
    else                     h->GetXaxis()->SetRange( 1+delta_lo, h->GetNbinsX()-delta_lo );
  }
  else { 

    if ( ilo>1 ) ilo--;
    if ( ihi<h->GetNbinsX() ) ihi++;

    h->GetXaxis()->SetRange(ilo,ihi);
  }



}

