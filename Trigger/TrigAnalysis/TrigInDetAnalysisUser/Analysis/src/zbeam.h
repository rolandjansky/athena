/* emacs: this is -*- c++ -*- */
/**
 **     @file    zbeam.h
 **
 **     @author  mark sutton
 **     @date    Thu 30 Jul 2015 10:50:19 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  ZBEAM_H
#define  ZBEAM_H

#include <iostream>

#include "TProfile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TPad.h"
#include "TStyle.h"


class zbeam {

public:

  zbeam( const std::vector<std::pair<int,double> >& refz,
	 const std::vector<std::pair<int,double> >& testz ) { 
    
    if ( refz.size()!=testz.size() || refz.size()==0 ) return;

    double _min = refz.back().first;
    double _max = refz[0].first;

    for ( unsigned i=refz.size() ; i-- ; )     if ( refz[i].first>_max ) _max = refz[i].first;
    for ( unsigned i=0 ; i<refz.size() ; i++ ) if ( refz[i].first<_min ) _min = refz[i].first;
  
    double minz = refz.back().second;
    double maxz = refz.back().second;

    for ( unsigned i=refz.size() ; i-- ; ) { 
      if ( refz[i].second<minz )  minz = refz[i].second;
      if ( testz[i].second<minz ) minz = testz[i].second;
      if ( refz[i].second>maxz )  maxz = refz[i].second;
      if ( testz[i].second>maxz ) maxz = testz[i].second;
    }

    double dz = maxz-minz;
    
    gStyle->SetOptStat(0);

    TProfile* hr = new TProfile( "refz",  ";lumi block;z reference [mm]", _max-_min+1, _min-0.5, _max+0.5 );
    TProfile* ht = new TProfile( "testz", ";lumi block;z reference [mm]", _max-_min+1, _min-0.5, _max+0.5 );

    TH1D* h = new TH1D("zres", ";#Delta z [mm]", 100, -10, 10 ); 

    TH2D* h2 = new TH2D( "zcor", ";z ref [mm]; ztest [mm]", 20, minz, maxz, 20, minz, maxz );  

    for ( unsigned i=refz.size() ; i-- ; ) { 
      hr->Fill( refz[i].first, refz[i].second );
      ht->Fill( testz[i].first, testz[i].second );

      h->Fill( testz[i].second-refz[i].second );
      
      h2->Fill( refz[i].second, testz[i].second );
    } 

    hr->SetMarkerColor(kRed);
    hr->SetLineColor(kRed);

    hr->SetMarkerStyle(24);
    ht->SetMarkerStyle(20);

    hr->SetMarkerSize(0.5);
    ht->SetMarkerSize(0.5);

    hr->SetMaximum( maxz+dz/10 );
    hr->SetMinimum( minz-dz/10 );

    hr->DrawCopy();
    ht->DrawCopy("same");
    
    gPad->Print( "zbeam.pdf");

    h->DrawCopy();

    gPad->Print( "zbeam-res.pdf" );


    h2->DrawCopy( "colz" );
    gPad->Print( "zbeam-cor.pdf" );

    TFile f( "zbeam.root", "recreate" );
    h->Write();
    hr->Write();
    ht->Write();
    h2->Write();
    f.Close();

    delete hr;
    delete ht;

    delete h;

  }

  virtual ~zbeam() { } 

private:

};

inline std::ostream& operator<<( std::ostream& s, const zbeam& ) { 
  return s;
}


#endif  // ZBEAM_H 










