/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         Resplot.h  
 **
 **                 A class to do useful book keeping and efficiency 
 **                 stuff. Like a TH1D in root, only it keeps a series 
 **                 of histograms with a value, very useful for making 
 **                 resolutions and so on.
 **
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Mon Jun 21 18:35:22 BST 2004
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef RESPLOT_RESPLOT_H
#define RESPLOT_RESPLOT_H

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TObject.h"


#include "StatVal.h"


// stupid root dictionary BOLLOX!! just because we inherit from a 
// TH2D, but I bet it *still* won't work because I use the 
// lovely "vector" class from the stl.
#include "TObject.h"



class Resplot  : public TObject { 
  
private:
  
  class ResException { 
  public:
    ResException(const std::string& s) { std::cerr<<s<<std::endl; }
  };

  enum ERROR { OK, HISTOWIDTH, BINWIDTH, EMPTY }; 
  
public:
  
  // NB, the destructor MUST NOT delete all the histograms, since root will
  //     when closing the file, and it will crash if we let the destructor
  //     try to clean up properly afterwards - LIKE WE SHOULD BE ABLE TO!
  // NB: NB!! Actually, now we explicitly *remove* them from root managerment
  //          so we can do *proper* garbage collection, not like the 
  //          alleged garbage collection in root

  Resplot() : 
    mSet(false),  m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL), 
    m_h2d(NULL), m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(""), m_yaxis(""), m_fitname(""), m_finalised(false),
    m_uniform(true)
  {  } 
  

  Resplot(const std::string& name, 
	  int n1, double a1, double b1, 
	  int n2, double a2, double b2, const std::string& xaxis="") :
    //    TH2D( "2d", "2d", n1, a1, b1, n2, a2, b2), 
    mSet(false),  m_name(name), 
    m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL), 
    m_h2d(NULL),  m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(xaxis), m_yaxis(""), m_fitname(""), m_finalised(false),
    m_uniform(true)
  { Initialise(name, n1, a1, b1, n2, a2, b2); } 

  
  Resplot(const std::string& name, 
	  int n1, const double* a1,
	  int n2, double a2, double b2, const std::string& xaxis="") :
    //    TH2D( "2d", "2d", n1, a1, n2, a2, b2),   
    mSet(false),  m_name(name), 
    m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL),  
    m_h2d(NULL),  m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(xaxis), m_yaxis(""), m_fitname(""), m_finalised(false),
    m_uniform(true)
  { Initialise(name, n1, a1, n2, a2, b2); }   

  Resplot(const std::string& name, 
	  std::vector<double> a,
	  int n2, double a2, double b2, const std::string& xaxis="") :
    //   TH2D( "2d", "2d", a.size()-1, &a[0], n2, a2, b2),   
    mSet(false),  m_name(name), 
    m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL),   
    m_h2d(NULL),  m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(xaxis), m_yaxis(""), m_slices(), m_fitname(""), m_finalised(false),
    m_uniform(true)
  { Initialise(name, a, n2, a2, b2); }


  Resplot(const std::string& name, 
	  std::vector<double> a,
	  std::vector<double> b, 
	  const std::string& xaxis="") :
    //   TH2D( "2d", "2d", a.size()-1, &a[0], n2, a2, b2),   
    mSet(false),  m_name(name), 
    m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL),   
    m_h2d(NULL),  m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(xaxis), m_yaxis(""), m_slices(), m_fitname(""), m_finalised(false),
    m_uniform(false)
  { Initialise(name, a, b); }



  Resplot(const Resplot& r, const std::string& tag="") : 
    //    TH2D( (TH2D)r ),
    TObject(r),
    mSet(r.mSet),  m_name(r.m_name+tag), 
    m_Nentries(new TH1D(*r.m_Nentries)), 
    m_mean(new TH1D(*r.m_mean)), m_sigma(new TH1D(*r.m_sigma)), m_chi2(new TH1D(*r.m_chi2)),  
    m_h2d(new TH2D(*r.m_h2d)),   m_h1d(new TH1D(*r.m_h1d)),
    m_dir(NULL),
    n_primary(r.n_primary),  //   a_primary(r.a_primary),     b_primary(r.b_primary),
    n_secondary(r.n_secondary), a_secondary(r.a_secondary), b_secondary(r.b_secondary), 
    m_xaxis(r.m_xaxis), m_yaxis(r.m_yaxis), m_fitname(""), m_finalised(false),
    m_uniform(true)
  { 
    //    TH2D::SetDirectory(0);
    skip(m_h2d);
    skip(m_h1d);
    skip(m_mean);
    skip(m_sigma);
    skip(m_chi2);
    skip(m_Nentries);
  }

  Resplot(const std::string& name, const TH2* hin, const bool flip=false) :
    mSet(false),  m_name(name), 
    m_Nentries(NULL), 
    m_mean(NULL), m_sigma(NULL), m_chi2(NULL),  
    m_h2d(NULL),  m_h1d(NULL), 
    m_dir(NULL),
    m_xaxis(""), m_yaxis(""), m_fitname(""), m_finalised(false),
    m_uniform(true)
  { 

    TH2D* h = 0;

    if ( flip ) h = rotate( hin ); 
    else        h = (TH2D*)hin;

    // get the major bin edges
    std::vector<double> a1;

    TAxis* axe = h->GetXaxis();

    for ( int i=1 ; i<=axe->GetNbins()+1 ; i++ ) a1.push_back(axe->GetBinLowEdge(i));
  
    // get the y bin limits
    TH1D* s = h->ProjectionY("_duff", 1, 1, "e");

    int    n2 = s->GetNbinsX();
    double a2 = s->GetBinLowEdge(1);
    double b2 = s->GetBinLowEdge(s->GetNbinsX()+1);

    delete s;

    Initialise(name, a1, n2, a2, b2 ); 

    // replace default histograms
    delete m_h2d;
    m_h2d = (TH2D*)h->Clone("2d");
    skip(m_h2d);
    
    delete m_h1d;
    m_h1d = h->ProjectionY("1d", 1, h->GetNbinsX(), "e");
    m_h1d->SetTitle("1d");
    skip(m_h1d);
  } 


  // hmmmm, this constructor doesn't work for some 
  // reason. Can't work out why, must be due to kak root directory
  // object ownership, I shouldn't wonder
  Resplot(const std::string& name) : 
    m_name(name)
    //   m_Nentries ( new TH1D(*(TH1D*)gDirectory->Get((name+"/fractional uncertainty").c_str())) ),
    //   m_mean     ( new TH1D(*(TH1D*)gDirectory->Get((name+"/1d").c_str()))    ),
    //   m_sigma    ( new TH1D(*(TH1D*)gDirectory->Get((name+"/sigma").c_str())) ),
    //   m_chi2     ( new TH1D(*(TH1D*)gDirectory->Get((name+"/chi2").c_str()))  ),
    //   m_h2d      ( new TH2D(*(TH2D*)gDirectory->Get((name+"/2d").c_str()))    ),
    //   m_h1d      ( new TH1D(*(TH1D*)gDirectory->Get((name+"/1d").c_str()))    )
  { 

    m_name     = name;

    m_Nentries = new TH1D(*(TH1D*)gDirectory->Get((name+"/fractional uncertainty").c_str()));
    m_mean     = new TH1D(*(TH1D*)gDirectory->Get((name+"/mean").c_str()));
    m_sigma    = new TH1D(*(TH1D*)gDirectory->Get((name+"/sigma").c_str()));
    // m_sigma    = new TH1D(*(TH1D*)gDirectory->Get((name+"/RMS").c_str()));
    m_chi2     = new TH1D(*(TH1D*)gDirectory->Get((name+"/chi2").c_str()));
    m_h2d      = new TH2D(*(TH2D*)gDirectory->Get((name+"/2d").c_str()));
    m_h1d      = new TH1D(*(TH1D*)gDirectory->Get((name+"/1d").c_str()));

    skip(m_h2d);
    skip(m_h1d);
    skip(m_mean);
    skip(m_sigma);
    skip(m_chi2);
    skip(m_Nentries);
    
    n_primary = m_h2d->GetNbinsX();

    m_finalised = true;

    m_uniform = true;

    //    std::cout << "Resplot(std::string)" << name << "\t n_primary " << n_primary << std::endl;

    // deep copy
    for ( int i=1 ; i<=m_h2d->GetNbinsX() ; i++ ) { 
      char slicename[512];
      sprintf(slicename, "/slices/slice[%03d]", i);
      //   std::cout << i << " " << (name+slicename) << std::endl; 
      m_slices.push_back((TH1D*)gDirectory->Get((name+slicename).c_str()));
      skip(m_slices.back()); //->SetDirectory(0);
    }
    
  }


  virtual ~Resplot();
  
  
  void Initialise(const std::string& name, 
		  int n1, double a1, double b1, 
		  int n2, double a2, double b2);
 
  void Initialise(const std::string& name, 
		  int n1, const double* a1, 
		  int n2, double a2, double b2);

  void Initialise(const std::string& name, 
		  int n1, const double* a1, 
		  int n2, const double* a2);

  void Initialise(const std::string& name, 
		  std::vector<double> a, 
		  int n2, double a2, double b2);

  void Initialise(const std::string& name, 
		  std::vector<double> a, 
		  std::vector<double> b ); 
 
  
  // Fill the helper histograms
  int Fill(double x, double y, double w=1) { 
    //    m_h2d->Fill(x, y, w); 
    // TH2D::Fill(x, y, w); 
    m_h2d->Fill(x, y, w); 
    m_h1d->Fill(y, w); 
    return 0;
  }


  // Get the efficiency for a single slice of the major ordinate
  StatVal GetEfficiency(TH1D* h, double lower, double upper);


  // Get the global efficiency
  StatVal GetGlobalEfficiency(double lower, double upper) {
    return GetEfficiency(m_h1d, lower, upper);
  }


  // Get all efficiencies for all the major bins
  // within the range lower .. upper
  TH1D* GetEfficiencies(const std::string& hname, double lower, double upper);

  // Get all efficiencies for all the major bins
  // within the range += Nsigma*sigma about the fitted 
  // mean, where the mean and sigma are taken from the fit, 
  // either gaussian of cauchy 
  TH1D* GetEfficiencies(const std::string& hname, double Nsigma);
  
  // set the name of the x axis variable
  void SetXAxis(const std::string& xaxis) {
    m_xaxis = xaxis;
    m_mean->SetXTitle(m_xaxis.c_str());
    m_sigma->SetXTitle(m_xaxis.c_str());
    m_chi2->SetXTitle(m_xaxis.c_str());
    m_Nentries->SetXTitle(m_xaxis.c_str());
    m_h2d->SetXTitle(m_xaxis.c_str());
    m_h1d->SetXTitle(m_xaxis.c_str());
  } 

  // and the y axis variable
  void SetYAxis(const std::string& yaxis) {
    m_yaxis = yaxis;
    m_h2d->SetXTitle(m_yaxis.c_str());
    m_h1d->SetXTitle(m_yaxis.c_str());
  } 

  // get the limits of the slices in the x coordinate
  std::vector<double> GetPrimaryLimits() const {
    std::vector<double> d;
    for ( int i=1 ; i<=m_mean->GetNbinsX()+1 ; i++ ) d.push_back(m_mean->GetBinLowEdge(i)); 
    return d;
  }

  // get the central values for each slice in the x coordinate
  std::vector<double> GetPrimaryCentres() const  {
    std::vector<double> d;
    for ( int i=1 ; i<=m_mean->GetNbinsX() ; i++ ) d.push_back(m_mean->GetBinCenter(i)); 
    return d;
  }
    
  const std::string& Name() const { return m_name; }  

  const std::string& Name(const std::string& s) { return m_name=s; }  

  // get the histogram of the mean of the distributions
  TH1D*       Mean()        { return m_mean; } 
  const TH1D* Mean() const  { return m_mean; } 

  // and the "width"
  TH1D* Sigma() { return m_sigma; }

  // and the chi2
  TH1D*       Chi2()       { return m_chi2; }
  const TH1D* Chi2() const { return m_chi2; }

  // and the overall projection in y 
  TH1D*       H1D()        { return m_h1d; } 
  const TH1D* H1D() const  { return m_h1d; } 

  // and the overall 2D histo 
  TH2D*       H2D()         { return m_h2d; } 
  const TH2D* H2D() const   { return m_h2d; } 

  // return 1/sqrt(Nentries) approximate stat precision 
  TH1D*       Uncertainty()         { return m_Nentries; } 
  const TH1D* Uncertainty() const   { return m_Nentries; } 

  bool finalised() const { return m_finalised; }

  // and the fitted slices
  std::vector<TH1D*> Slices() { return m_slices; }

  // and the name of the fitted function 
  // NB. this is useful for setting plot paramaters
  // of the fit for plotting, such as the line colour etc 
  const std::string FitName() const { return m_fitname; }

  // Static fit functions!!!!
  // NB. *Any* fit function can be used to fit the distributions
  // as long as it has this form of method, it does not need 
  // to be a member of the class Resplot, but using static 
  // member functions here, gaurantees there are a few simple
  // examples that will be safely encapsulated inside the 
  // class so there will be no interference with other functions

  // fit Generic to histogram
  static TF1* FitInternal(TH1D* s, double a=-999, double b=-999, TF1* f1=0);

  // fit Gaussian to histogram
  static TF1* FitGaussian(TH1D* s, double a=-999, double b=-999);

  // fit Gaussian with radial dependence to histogram 
  static TF1* FitRGaussian(TH1D* s, double a=-999, double b=-999);
  
  // or fit a Breit Wigner
  static TF1* FitBreit(TH1D* s, double a=-999, double b=-999);

  // or fit a Breit Wigner with radial dependence 
  static TF1* FitRBreit(TH1D* s, double a=-999, double b=-999);
  
  // or fit an inverse tan turn on curve 
  static TF1* FitATan(TH1D* s, double a=-999, double b=-999);

  // use histo mean and rms
  static TF1* FitNull(TH1D* s, double a=-999, double b=-999); 

  // fit a poisson distribution 
  static TF1* FitPoisson(TH1D* s, double a=-999, double b=-999); 

  // fit a  ax^b exp(c*x)+d distribution 
  static TF1* FitXExp(TH1D* s, double a=-999, double b=-999); 

  static TF1* FitLandauGauss(TH1D* s, double a=-999, double b=-999);

  // fit Central Gaussian to histogram
  static TF1* FitCentralGaussian(TH1D* s, double a=-999, double b=-999);

  // fit histo mean and rms only to central 95% of histogram
  static TF1* FitNull95(TH1D* s, double a=0, double b=0);
  //  static TF1* FitNull95(TH1D* s, double frac=0.95, bool useroot=false);
  static TF1* FitNull95New(TH1D* s, double frac=0.95, bool useroot=false);
  static TF1* FitNull95Obsolete(TH1D* s, double frac=0.95, bool useroot=false);

  static TF1* FitNull95Central(TH1D* s);



  // Get the offsets and resolutions overall and for each slice 
  //  int Finalise(double a=-999, double b=-999, TF1* (*func)(TH1D* s, double a, double b)=Resplot::FitGaussian);
  int Finalise(double a=-999, double b=-999, TF1* (*func)(TH1D* s, double a, double b)=Resplot::FitGaussian);

  int Finalise(TF1* (*func)(TH1D* s, double a, double b), double a=-999, double b=-999 ) { 
    return Finalise( a, b, func);
  }

  int Fit(TF1* (*func)(TH1D* s, double a, double b) ) { 
    if ( m_finalised ) return -1;
    return Finalise( func );
  }
  
  int Refit(TF1* (*func)(TH1D* s, double a, double b) ) { 
    clear();
    return Finalise( func );
  }


  TDirectory* Dir() { return m_dir; } 

  /// Hooray, this stupidity is to overwride both the const and non-const 
  /// TObject Write methods
  /// Fixme: should probably remove all of these so root can write out 
  ///        using the dictionary but I could never get that to work
  Int_t Write(const char* =0, Int_t =0, Int_t =0) const { return DWrite(); } 
  Int_t Write(const char* =0, Int_t =0, Int_t =0)       { return DWrite(); } 
  Int_t DWrite(TDirectory* g=0) const;

  static bool AddDirectoryStatus() { return mAddDirectoryStatus; } 
  static void AddDirectory(bool t=false) { mAddDirectoryStatus=t; } 

  void SetDirectory(TDirectory* =0) {  }

  static void setInterpolateFlag(bool b) { interpolate_flag = b; }


  /// operators 

  Resplot& operator+=(const Resplot& r) { 

    clear();
    
    //    std::cout << "Resplot::operator+= " << Name() << std::endl;

    if ( r.H1D()->GetNbinsX()!=H1D()->GetNbinsX()) throw ResException("histogram limits do not match");
    if ( r.H2D()->GetNbinsX()!=H2D()->GetNbinsX()) throw ResException("histogram limits do not match");
    if ( r.H2D()->GetNbinsY()!=H2D()->GetNbinsY()) throw ResException("histogram limits do not match");

    for ( int i=0 ; i<=r.H2D()->GetNbinsX()+1 ; i++ ) { 
      for ( int j=0 ; j<=r.H2D()->GetNbinsY()+1 ; j++ ) { 
	H2D()->SetBinContent( i, j, H2D()->GetBinContent(i,j) + r.H2D()->GetBinContent(i,j) );
	H2D()->SetBinError( i, j, std::sqrt(H2D()->GetBinError(i,j)*H2D()->GetBinError(i,j) + r.H2D()->GetBinError(i,j)*r.H2D()->GetBinError(i,j) ) );
      }
    }

    for ( int i=0 ; i<=r.H1D()->GetNbinsX()+1 ; i++ ) { 
      H1D()->SetBinContent( i, H1D()->GetBinContent(i) + r.H1D()->GetBinContent(i) );
      H1D()->SetBinError( i, std::sqrt(H1D()->GetBinError(i)*H1D()->GetBinError(i) + r.H1D()->GetBinError(i)*r.H1D()->GetBinError(i) ) );
    }

    m_finalised = false;

    //    std::cout << "Resplot::operator+= done" << std::endl;

    return *this;
  }



  Resplot& operator*=(double d) { 

    std::cout << "Resplot::operator*= " << Name() << " " << m_finalised << " " << m_slices.size() << std::endl;
    
    clear();

    std::cout << "Resplot::operator*= " << Name() << " " << m_finalised  << " " << m_slices.size() << std::endl;
    std::cout << "Resplot::operator*= " << Name() << "\tH2D " << H2D() << std::endl;

    H2D()->DrawCopy();

    std::cout << "Resplot::operator*= " << Name() << "\tDraw " << std::endl;

    for ( int i=0 ; i<=H2D()->GetNbinsX()+1 ; i++ ) { 
      for ( int j=0 ; j<=H2D()->GetNbinsY()+1 ; j++ ) { 
	H2D()->SetBinContent( i, j, H2D()->GetBinContent(i,j)*d);
	H2D()->SetBinError( i, j, H2D()->GetBinError(i,j)*d);
      }
    }


    std::cout << "Resplot::operator*= " << Name() << "\tH1D " << H1D() << std::endl;

    for ( int i=0 ; i<=H1D()->GetNbinsX()+1 ; i++ ) { 
      H1D()->SetBinContent( i, H1D()->GetBinContent(i)*d );
      H1D()->SetBinError( i, H1D()->GetBinError(i)*d );
    }

    m_finalised = false;

    std::cout << "Resplot::operator*= done" << std::endl;

    return *this;
  }

  Resplot operator*(double d) const { 
    Resplot r(*this);
    return (r*=d);
  }

  Resplot operator+(const Resplot& r) const { 
    Resplot r0(*this);
    return (r0+=r);
  }


  void Integrate() { 
    if ( !m_finalised ) return;

    /// integrate all the slices and refit the mean etc.
    /// not yet ...
  }

  void setUniform(bool t=false) { m_uniform=t; }

  static std::string version()      { return rversion; } 
  static bool setoldrms95(bool b)   { return oldrms95=b; } 
  static bool setscalerms95(bool b) { return scalerms95=b; } 
  static bool setnofit(bool b)      { return nofit=b; } 

  /// flip the x and y axes
  static TH2D* rotate(const TH2* h);

  /// combine bins along the x axis
  /// after value x, combine each N bins into a single bin
  static TH2D* combine(const TH2* h, double x, int N);
  /// after value x, combine each N bins into a single bin
  static TH1D* combine(const TH1* h, double x, int N);

  /// combine bins along the x axis
  /// combine bins such that the stat precision is better than epsilon
  static TH2D* combine(const TH2* h, double epsilon);
  static TH1D* combine(const TH1* h, double epsilon);

  static std::vector<double> findbins(const TH1* h, double epsilon);
  static TH1D*               rebin(const TH1* h, const std::vector<double>& bins);
 
  static std::vector<double> getbins(const TH1* h);

private:

  void MakeSlices() { }

private:

  // stop the root default histogram ownership nonsense  
  void skip(TH1D* t)       { if ( t ) t->SetDirectory(0); }
  void delskip(TH1D* t)    { if ( t ) { t->SetDirectory(0); delete t; } }
  void deletehist(TH1D* t) { if ( t ) delete t; }
 
  void skip(TH2D* t)       { if ( t ) t->SetDirectory(0); }
  void delskip(TH2D* t)    { if ( t ) { t->SetDirectory(0); delete t; } }
  void deletehist(TH2D* t) { if ( t ) delete t; }
  

  void SetPrimary(int n, const double* )     {  n_primary=n; }
  void SetPrimary(int n, double , double )   {  n_primary=n; } // a_primary=a;  b_primary=b;  }
  void SetSecondary(int n, double a, double b) { n_secondary=n; a_secondary=a; b_secondary=b; } 
  

  // stuff for calculating efficiencies

  StatVal get_mean()  { return g_mean; }
  StatVal get_sigma() { return g_sigma; }
  
  
  void AddEfficiency(TH1D* h, int i, double lower, double upper) {
    StatVal v = GetGlobalEfficiency( lower, upper);
    h->SetBinContent(i, v.value);
    h->SetBinError(i, v.error);
  }

  void AddResolution(TH1D* h, int i) {
    StatVal v = get_sigma();
    h->SetBinContent(i, v.value);
    h->SetBinError(i, v.error);
  } 

  void AddMean(TH1D* h, int i) {
    StatVal v = get_mean();
    h->SetBinContent(i, v.value);
    h->SetBinError(i, v.error);
  } 

private:

  void clear() {
    // std::cout << "clear " << m_slices.size() << std::endl;
    m_finalised = false;
    for ( int i=m_slices.size() ; i-- ; ) if ( m_slices[i] ) delete m_slices[i];
    m_slices.clear();
    //    std::cout << "cleared" << std::endl;
  }
  
private:

  static bool mAddDirectoryStatus;
  static bool interpolate_flag;

  static const std::string rversion;

  bool mSet; 

  std::string m_name;

  TH1D* m_Nentries;
  TH1D* m_mean;
  TH1D* m_sigma;
  TH1D* m_chi2;

  TH2D* m_h2d;
  TH1D* m_h1d;

  StatVal  g_mean;
  StatVal  g_sigma;

  TDirectory* m_dir;

  int     n_primary;
  //  double  a_primary, b_primary;

  int    n_secondary;
  double a_secondary, b_secondary;

  std::string  m_xaxis;
  std::string  m_yaxis;

  std::vector<TH1D*> m_slices;
  std::string        m_fitname;

  bool        m_finalised;
  TDirectory* m_slicedir;

  bool        m_uniform;

  /// temporarily allow toggeling between old 
  /// and new rms95 error estimates
  static bool oldrms95;
  static bool scalerms95;

  static bool nofit;


  static ERROR ErrorSet; //! don't persistify this 

  ClassDef(Resplot, 1)

};


inline Resplot operator*(double d, Resplot r) { return (r*=d); }


#endif  /* RESPLOT_RESPLOT_H */

