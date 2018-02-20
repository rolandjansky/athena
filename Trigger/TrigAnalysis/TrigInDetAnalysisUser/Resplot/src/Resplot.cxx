
/**************************************************************************
 **
 **   File:         Resplot.cxx  
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Mon Jun 21 18:35:22 BST 2004
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#include <iostream>
#include <sstream>
#include <cmath>


#include "Directory.h"
#include "tagname.h"
#include "Resplot.h"

#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"

#include "generate.h"
#include "rmsFrac.h"

ClassImp(Resplot)


bool Resplot::mAddDirectoryStatus = true;
bool Resplot::interpolate_flag = true;
bool Resplot::nofit = false;

/// use the new error estimates
bool Resplot::oldrms95   = true;
bool Resplot::scalerms95 = true;

Resplot::ERROR Resplot::ErrorSet = Resplot::OK;


void binwidth(TH1D* h) { 
  
  for (int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    double d =  h->GetBinLowEdge(i+1) - h->GetBinLowEdge(i);

    h->SetBinContent(i, h->GetBinContent(i)/d ); 
    h->SetBinError(i, h->GetBinError(i)/d ); 
  }

}


// set the errors on bins with zero entries to be one so they
// are not ignored in the fit 
// USE WITH CARE - root screws up the mean and rms error calculation 
//                 if these are used

void ZeroErrors(TH1D* h) {
    for (int i=1 ; i<=h->GetNbinsX() ; i++ ) if (h->GetBinContent(i)==0) h->SetBinError(i,1);
}

// and put them back to zero again afterwards

void unZeroErrors(TH1D* h) {
    for (int i=1 ; i<=h->GetNbinsX() ; i++ ) if (h->GetBinContent(i)==0) h->SetBinError(i,0);
}



double getWeights(TH1D* h) { 
  double sum   = 0;
  double sume2 = 0;
  for ( int i=0 ; i<h->GetNbinsX()+1 ; i++ ) { 
    sum   += h->GetBinContent(i);
    sume2 += h->GetBinError(i)*h->GetBinError(i);
  }
  if ( sume2!=0 ) return sum*sum/sume2;
  return 0;
}


double getWeights(TH2D* h) { 

  double sum   = 0;
  double sume2 = 0;

  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    for ( int j=1 ; j<=h->GetNbinsY() ; j++ ) { 
      sum   += h->GetBinContent(i,j);
      sume2 += h->GetBinError(i,j)*h->GetBinError(i,j);
    }
  }

  if ( sume2!=0 ) return sum*sum/sume2;

  return 0;
}




const std::string Resplot::rversion = "resplot-v29";

void GetStats( TH1D* h, std::vector<double>& _s ) { 

  _s.clear();
  _s.resize(4);

  TAxis& fXaxis = *h->GetXaxis();
  
  double s   = 0;
  double sx  = 0;
  double sx2 = 0;
  double sx4 = 0;
  

  //  if ( fXaxis.TestBit(TAxis::kAxisRange)  ) {
  //    for (bin=0;bin<4;bin++) stats[bin] = 0;

  Int_t firstBinX = fXaxis.GetFirst();
  Int_t lastBinX  = fXaxis.GetLast();
  // include underflow/overflow if TH1::StatOverflows(kTRUE) in case no range is set on the axis
  //      if (fgStatOverflows && !fXaxis.TestBit(TAxis::kAxisRange)) {
  //   if (firstBinX == 1) firstBinX = 0;
  //   if (lastBinX ==  fXaxis.GetNbins() ) lastBinX += 1;
  // }
  for (int binx = firstBinX; binx <= lastBinX; binx++) {
    double x   = fXaxis.GetBinCenter(binx);
    double w   = TMath::Abs(h->GetBinContent(binx));
    //    err = TMath::Abs(GetBinError(binx));
    s += w;
    // stats[1] += err*err;
    sx  += w*x;
    sx2 += w*x*x;
    sx4 += w*x*x*x*x;
  }
  
  
  double mu = sx/s;

  double v = sx2/s-mu*mu;

  double rms       = std::sqrt(v);
  double rmserror  = std::sqrt(0.25*(sx4/v-v*s))/s;
  //  double rmserror1 = std::sqrt(0.4*(sx4/s-v*v)/s);  << is this the correct one? it seems about 2* too large 

  _s[0] = mu;
  _s[1] = std::sqrt(v/s);

  _s[2] = rms;
  _s[3] = rmserror;

  //  double duff = std::sqrt(0.5*v/s);

#if 0
  std::cout << "GetStats() "  
    //   << "\tmean " << _s[0] << " +- " << _s[1]
	    << "\trms "  << _s[2] << " +- " << _s[3] << "\t(" << rmserror1 << ")" << "\t root " << duff << std::endl;
#endif

} 



Resplot::~Resplot() { 

  //  std::cout << "Resplot::~Resplot() " << Name() << std::endl;
    
    delskip( m_Nentries );
    delskip( m_h2d ); 
    delskip( m_h1d ); 
    delskip( m_mean );
    delskip( m_sigma );
    delskip( m_chi2 );

    //    std::cout << "Resplot::~Resplot() deleteing slices" << Name() << std::endl;

    for ( unsigned i=0 ; i<m_slices.size() ; i++ ) delskip(m_slices[i]);
      
    //    TH2D::SetDirectory(0);
}



void Resplot::Initialise(const std::string& name, 
			 int n1, double a1, double b1, 
			 int n2, double a2, double b2) {

  SetPrimary(n1, a1, b1);
  SetSecondary(n2, a2, b2);

  mSet = true;
  //  TH2D::SetDirectory(0);

  m_name = name;
  const std::string name_2d = "2d";
  const std::string name_1d = "1d";

  //  std::cout << "Resplot::Initialise() Name " << Name() << std::endl;

  m_h2d = new TH2D( name_2d.c_str(), name_2d.c_str(), n1, a1, b1, n2, a2, b2);
  m_h1d = new TH1D( name_1d.c_str(), name_1d.c_str(),             n2, a2, b2);
  m_h1d->Sumw2();
  skip(m_h2d);
  skip(m_h1d);
  
  const std::string name_mean    = "mean";
  const std::string name_sigma   = "sigma";
  const std::string name_chi2    = "chi2";
  //  const std::string name_entries = "significance";
  const std::string name_entries = "fractional uncertainty";

  m_Nentries  = new TH1D( name_entries.c_str(),  name_entries.c_str(),  n1, a1, b1 );
  skip(m_Nentries);

  m_mean  = new TH1D( name_mean.c_str(),  name_mean.c_str(),  n1, a1, b1 );
  m_sigma = new TH1D( name_sigma.c_str(), name_sigma.c_str(), n1, a1, b1 );
  m_chi2  = new TH1D( name_chi2.c_str(),  name_chi2.c_str(),  n1, a1, b1 );
  skip(m_mean);
  skip(m_sigma);
  skip(m_chi2);

} 



void Resplot::Initialise(const std::string& name, 
			 std::vector<double> a,
			 int n2, double a2, double b2) {
  Initialise(name, a.size()-1, &a[0], n2, a2, b2);
} 


void Resplot::Initialise(const std::string& name, 
			 std::vector<double> a,
			 std::vector<double> b ) { 
  Initialise(name, a.size()-1, &a[0], b.size()-1, &b[0] );
} 



void Resplot::Initialise(const std::string& name, 
			 int n1, const double* a1, 
			 int n2, double a2, double b2) {  
  SetPrimary(n1, a1);
  SetSecondary(n2, a2, b2);

  mSet = true;
  
  m_name = name;
  const std::string name_2d = "2d";
  const std::string name_1d = "1d";

  //  std::cout << "Resplot::Initialise() Name " << Name() << std::endl;

  //  TH2D::SetDirectory(0);

  m_h2d  = new TH2D( name_2d.c_str(), name_2d.c_str(), n1, a1, n2, a2, b2);
  m_h1d  = new TH1D( name_1d.c_str(), name_1d.c_str(),         n2, a2, b2);
  m_h1d->Sumw2();
  skip(m_h2d);
  skip(m_h1d);
  
  const std::string name_mean = "mean";
  const std::string name_sigma = "sigma";
  const std::string name_chi2  = "chi2";

  m_mean  = new TH1D( name_mean.c_str(),  name_mean.c_str(),  n1, a1 );
  m_sigma = new TH1D( name_sigma.c_str(), name_sigma.c_str(), n1, a1 );
  m_chi2  = new TH1D( name_chi2.c_str(),  name_chi2.c_str(),  n1, a1);
  
  skip(m_mean);  
  skip(m_sigma);
  skip(m_chi2);

  //  const std::string name_entries = "significance";
  const std::string name_entries = "fractional uncertainty";

  m_Nentries  = new TH1D( name_entries.c_str(),  name_entries.c_str(), n1, a1 );
  skip(m_Nentries);

}




void Resplot::Initialise(const std::string& name, 
			 int n1, const double* a1, 
			 int n2, const double* a2) {  
  SetPrimary(n1, a1);
  SetSecondary(n2, a2[0], a2[n2+1]);

  mSet = true;
  
  m_name = name;
  const std::string name_2d = "2d";
  const std::string name_1d = "1d";

  //  std::cout << "Resplot::Initialise() Name " << Name() << std::endl;

  //  TH2D::SetDirectory(0);

  m_h2d  = new TH2D( name_2d.c_str(), name_2d.c_str(), n1, a1, n2, a2 );
  m_h1d  = new TH1D( name_1d.c_str(), name_1d.c_str(),         n2, a2 );
  m_h1d->Sumw2();
  skip(m_h2d);
  skip(m_h1d);
  
  const std::string name_mean = "mean";
  const std::string name_sigma = "sigma";
  const std::string name_chi2  = "chi2";

  m_mean  = new TH1D( name_mean.c_str(),  name_mean.c_str(),  n1, a1 );
  m_sigma = new TH1D( name_sigma.c_str(), name_sigma.c_str(), n1, a1 );
  m_chi2  = new TH1D( name_chi2.c_str(),  name_chi2.c_str(),  n1, a1);
  
  skip(m_mean);  
  skip(m_sigma);
  skip(m_chi2);

  //  const std::string name_entries = "significance";
  const std::string name_entries = "fractional uncertainty";

  m_Nentries  = new TH1D( name_entries.c_str(),  name_entries.c_str(), n1, a1 );
  skip(m_Nentries);

}


/// boooo, need to use the *stupid* Int_t class because I foolishly
/// gave this function the same name as TObject::Write

Int_t Resplot::DWrite(TDirectory* g) const {

    //   std::cout << "Resplot::Write() Name " << Name() << "; " << gDirectory->GetName();

    TDirectory* _cwd = gDirectory;
    if ( g ) g->cd();
    Directory d(Name());
    d.push();

    //    std::cout << "/" << gDirectory->GetName() << std::endl;

    //    TH2D::Write();
    m_h2d->Write();
    m_h1d->Write();
    m_mean->Write();
    m_sigma->Write();
    m_chi2->Write();
    m_Nentries->Write();
    //   std::cout << "Resplot::Write() Name() " << Name() << " " << m_slices.size() << std::endl;  
    gDirectory->mkdir("slices")->cd();
    for ( unsigned i=0 ; i<m_slices.size() ; i++ ) m_slices[i]->Write(); 
    d.pop();
    _cwd->cd();

    return 0;
} 



// Fill the helper histograms

std::string number(const double& x) { 
  std::ostringstream s;
  s << x;
  return s.str();
}


double NCounter(TH1D* h) { 
  double N=0;
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) N += h->GetBinContent(i);
  return N; 
}


// Get the offsets and resolutions overall and for each slice 
int Resplot::Finalise(double a, double b, TF1* (*func)(TH1D* s, double a, double b)) {
  
  //  gDirectory->pwd();

  ErrorSet = OK;

  //  std::cout << "Resplot::Finalise() Name " << Name() << "\tn_primary " << n_primary << std::endl;

  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(2211);

  if ( m_finalised ) { 
    std::cerr << "Resplot::Finalise() " << m_name << " already called" << std::endl;
    return -1;
  }

  //  std::cout << "Resplot::Finalise() " << m_name << std::endl;  

  clear();

  m_mean->SetStats(false);
  m_sigma->SetStats(false);

#if 1

  for ( int i=1 ; i<=n_primary ; i++ ) {    

    if ( i%1000==0 ) std::cout << "slice " << i << " of " << n_primary << std::endl;

    //    std::string projname = tagname(m_name,i).c_str();
    double pllim = m_mean->GetBinLowEdge(i);
    double pulim = m_mean->GetBinLowEdge(i+1);

    //    std::cout << "Finalise " << i << " : " << pllim << " - " << pulim << std::endl;

    std::string projname;
    if ( m_xaxis=="" ) projname = "[ " + number(pllim) + " - " + number(pulim) + " ]"; 
    else               projname = "[ " + number(pllim) + " < " + m_xaxis + " < " + number(pulim) + " ]"; 

    TH1D* s;
    if ( !m_finalised ) { 
      s = m_h2d->ProjectionY(tagname("slice",i), i, i, "e");
  
      //      std::cout << i << " fitnull " << getWeights( s ) << std::endl; 

      if ( !m_uniform ) binwidth(s);
      s->SetTitle(projname.c_str());
      if ( m_yaxis!="" ) s->SetXTitle(m_yaxis.c_str()); 
    }
    else  s = (TH1D*)gDirectory->Get(tagname(m_name,i));
    skip(s);

    //    ZeroErrors(s);

    TF1* f1 = 0;


    if ( !nofit ) f1 = func(s, a, b); 

    //    std::cout << "nofit " << nofit << "\tf1 " << f1 << std::endl;


    //    unZeroErrors(s);


    if ( f1!=0 ) {
 
      f1->SetLineWidth(1);
      f1->SetNpx(5000);
      m_fitname = f1->GetName();

      //      double N = NCounter(s);
      double N = s->GetEffectiveEntries();

      if ( N!=0 ) { 
	m_Nentries->SetBinContent(i,1/sqrt(N)); 
	m_Nentries->SetBinError(i,0); 
      }
      else { 
	m_Nentries->SetBinContent(i,0); 
	m_Nentries->SetBinError(i,0); 
      }
       
      m_mean->SetBinContent(i, f1->GetParameter(1) );
      m_mean->SetBinError(i, f1->GetParError(1) );
      
      m_sigma->SetBinContent(i, std::fabs(f1->GetParameter(2)) );
      m_sigma->SetBinError(i, f1->GetParError(2) );

      int     Ndof   = f1->GetNDF();
      if ( Ndof )  m_chi2->SetBinContent( i, std::fabs(f1->GetChisquare())/Ndof );
      m_chi2->SetBinError(i, 0 );
      delete f1;
    }
    else {
      m_Nentries->SetBinContent(i,0); 
      m_mean->SetBinContent(i,0);
      m_mean->SetBinError(i,0);
      m_sigma->SetBinContent(i,0);
      m_sigma->SetBinError(i,0);
      m_chi2->SetBinContent(i,0);
      m_chi2->SetBinError(i,0);
    }

    //    std::cout << "Resplot::Finalise() slice " << s->GetName() << std::endl;
    
    m_slices.push_back(s);
  }
  


#endif


  m_chi2->SetMinimum(1e-5);

  // get the overall resolution and offset

  if ( !m_uniform ) binwidth(m_h1d);

  TF1* f2 = 0;

  if ( !nofit ) { 
    ZeroErrors(m_h1d);
    f2 = func(m_h1d, a, b);
    unZeroErrors(m_h1d);  
  }

  if ( f2!=0 ) {

    //    std::cout << "  fit = " << f2 << std::endl;
    f2->SetLineWidth(1);
    f2->SetNpx(5000);

    g_mean  = StatVal( f2->GetParameter(1), f2->GetParError(1) );
    g_sigma = StatVal( f2->GetParameter(2), f2->GetParError(2) );

    //    std::cout << gDirectory->GetName() << "\tgmean " << g_mean << "\tgsigma " << g_sigma << std::endl;  
    
    std::string mname = std::string(f2->GetParName(1));
    std::string sname = std::string(f2->GetParName(2));
    
    //    if ( mname!=std::string("") ) { m_mean->SetName(mname.c_str());  m_mean->SetTitle(mname.c_str()); }
    //    if ( sname!=std::string("") ) { m_sigma->SetName(sname.c_str()); m_sigma->SetTitle(sname.c_str()); }
    if ( mname!=std::string("") ) { m_mean->SetTitle(mname.c_str()); }
    if ( sname!=std::string("") ) { m_sigma->SetTitle(sname.c_str()); }
    
    delete f2;
  }
  else {
    if ( !nofit ) std::cerr << "null overall fit Resplot:" << m_name << std::endl;
    g_mean  = StatVal(0,0);
    g_sigma = StatVal(0,0);
  }


  if ( AddDirectoryStatus() ) SetDirectory(gDirectory);
  
  m_finalised = true;

  if ( ErrorSet!=OK ) {  
    if ( ErrorSet == HISTOWIDTH ) { 
      std::cerr << __FUNCTION__ << " for " << m_name 
		<< " :\tdistribution wider than histogram width " << std::endl;
    }
    if ( ErrorSet == BINWIDTH ) {
      std::cerr << __FUNCTION__ << " for " << m_name 
		<< " :\tbins too wide: cannot calculate rms95 " << std::endl;
    }
  }

  return 0;
}


/// rotate a TH2D
TH2D* Resplot::rotate(const TH2* h) { 
    
    std::vector<double> xbins;
    std::vector<double> ybins;
    
    TH1D* hy = (TH1D*)h->ProjectionX("1dx", 1, h->GetNbinsY());
    TH1D* hx = (TH1D*)h->ProjectionY("1dy", 1, h->GetNbinsX());

    hy->SetDirectory(0);
    hx->SetDirectory(0);
    
    for ( int i=0 ; i<=hx->GetNbinsX()+1 ; i++ ) xbins.push_back( hx->GetBinLowEdge(i+1) );
    for ( int i=0 ; i<=hy->GetNbinsX()+1 ; i++ ) ybins.push_back( hy->GetBinLowEdge(i+1) );
    
    std::cout << "rotate:" << std::endl;
    
    std::cout << "x: " << xbins[0] << " - " << xbins.back() << std::endl;  
    std::cout << "y: " << ybins[0] << " - " << ybins.back() << std::endl;  
    
    TH2D* h2 = new TH2D("duff","duff", hx->GetNbinsX(), &xbins[0],  hy->GetNbinsX(), &ybins[0] );  
    
    for ( int i=0 ; i<hx->GetNbinsX() ; i++ ) { 
      for ( int j=0 ; j<hy->GetNbinsX() ; j++ ) { 
	h2->SetBinContent(j+1,i+1, h->GetBinContent(i+1,j+1));
	h2->SetBinError(j+1,i+1, h->GetBinError(i+1,j+1));
      }
    }

    h2->SetEntries( h->GetEntries() );

    delete hy;
    delete hx;
    
    return h2;
}




/// combine bins in a 2D histogram
/// it takes a value, x and all the bins with BinLowEdge > x get 
/// combined N bins at a time. 
TH2D* Resplot::combine(const TH2* h, double x, int N) { 
    
    if ( N==0 ) return 0; 

    std::vector<double> xbins;
    std::vector<double> ybins;
    
    TH1D* hx = (TH1D*)h->ProjectionX("r1dx", 1, h->GetNbinsY()); 
    TH1D* hy = (TH1D*)h->ProjectionY("r1dy", 1, h->GetNbinsX());
    
    hy->SetDirectory(0);
    hx->SetDirectory(0);
 
    for ( int i=0 ; i<=hx->GetNbinsX()+1 ; i++ ) {
      xbins.push_back( hx->GetBinLowEdge(i+1) );
      if ( hx->GetBinLowEdge(i+1)>x ) for ( int k=1 ; k<N ; k++, i++ );
    }
 
    for ( int i=0 ; i<=hy->GetNbinsX()+1 ; i++ ) ybins.push_back( hy->GetBinLowEdge(i+1) );
   
    std::cout << "x: " << xbins[0] << " - " << xbins.back() << std::endl;  
    std::cout << "y: " << ybins[0] << " - " << ybins.back() << std::endl;  
    
    if ( xbins.size()==0 || ybins.size()==0 ) { 
      std::cerr << "Resplot::combine() bad limits for histogram: N xbins: " << xbins.size() << "\tN ybins: " << ybins.size() << std::endl;
      return 0;
    }

    TH2D* h2 = new TH2D("duff","duff", xbins.size()-1, &xbins[0],  ybins.size()-1, &ybins[0] ); 
    h2->SetDirectory(0); 
 

    for ( int j=0 ; j<hy->GetNbinsX() ; j++ ) { 
      int xbin = 1;
      for ( int i=0 ; i<hx->GetNbinsX() ; i++ ) { 
        double entries = h->GetBinContent(i+1,j+1);
	double errorsq = h->GetBinError(i+1,j+1)*h->GetBinError(i+1,j+1);
        if ( hx->GetBinLowEdge(i+1)>x ) { 
          for ( int k=1 ; k<N ; k++, i++ ) { 
	    entries += h->GetBinContent(i+2,j+1);
	    errorsq += h->GetBinError(i+2,j+1)*h->GetBinError(i+2,j+1);
	  }
	}
        h2->SetBinContent(xbin, j+1, entries);
        h2->SetBinError(xbin++, j+1, std::sqrt(errorsq) );
      }
    }

    h2->SetEntries( h->GetEntries() );

    delete hy;
    delete hx;
    
    return h2;
}



/// combine bins in a 2D histogram
/// it takes a value, x and all the bins with BinLowEdge > x get 
/// combined N bins at a time. 
TH1D* Resplot::combine(const TH1* h, double x, int N) { 
    
    if ( N==0 ) return 0; 

    std::vector<double> xbins;
    
    for ( int i=0 ; i<=h->GetNbinsX()+1 ; i++ ) {
      xbins.push_back( h->GetBinLowEdge(i+1) );
      if ( h->GetBinLowEdge(i+1)>x ) for ( int k=1 ; k<N ; k++, i++ );
    }
 
    std::cout << "x: " << xbins[0] << " - " << xbins.back() << std::endl;  
    
    if ( xbins.size()==0 ) { 
      std::cerr << "Resplot::combine() bad limits for histogram: N xbins: " << xbins.size() << std::endl;
      return 0;
    }

    TH1D* h2 = new TH1D(  (std::string(h->GetName())+"-duff").c_str(),"duff", xbins.size()-1, &xbins[0] );
    h2->SetDirectory(0); 
 

    int xbin = 1;
    for ( int i=0 ; i<h->GetNbinsX() ; i++ ) { 
      double entries = h->GetBinContent(i+1);
      double errorsq = h->GetBinError(i+1)*h->GetBinError(i+1);
      if ( h->GetBinLowEdge(i+1)>x ) { 
	for ( int k=1 ; k<N ; k++, i++ ) { 
	  entries += h->GetBinContent(i+2);
	  errorsq += h->GetBinError(i+2)*h->GetBinError(i+2);
	}
      }

      h2->SetBinContent(xbin, entries);
      h2->SetBinError(xbin++, std::sqrt(errorsq) );
    }

    h2->SetEntries( h->GetEntries() );

    return h2;
}



/// combine bins in a 2D histogram
/// it takes a value, x and all the bins with BinLowEdge > x get 
/// combined N bins at a time. 
// TH2D* Resplot::combine(const TH2* h, double epsilon) { 
TH2D* Resplot::combine(const TH2* h, double inveps2) { 
    
  std::cout << "combine" << std::endl;
  
  //    if ( epsilon==0 ) return 0; 

    std::vector<double> xbins;
    std::vector<double> ybins;
    
    std::cout << "projection" << std::endl;

    TH1D* hx = (TH1D*)h->ProjectionX("r1dx", 1, h->GetNbinsY()); 
    hx->SetDirectory(0);


    //    hy->SetDirectory(0);
    //    hx->SetDirectory(0);

    //    double inveps2 = 1/(epsilon*epsilon);


    //    hx->DrawCopy();
    //    gPad->Print("cock.png");


    double N = 0;

    bool newbin = true;

    std::cout << "combining bins " << std::endl;

    for ( int i=1 ; i<=hx->GetNbinsX() ; i++ ) {

      TH1D* hy = (TH1D*)h->ProjectionY("r1dy", i, i );
      hy->SetDirectory(0);
      N += generate::GetEntries(hy);
      delete hy;
      
      if ( xbins.size()==0 ) { 
	if ( N==0 ) continue;
	else xbins.push_back( hx->GetBinLowEdge(i) );
      }

      if ( newbin ) xbins.push_back( hx->GetBinLowEdge(i+1) );
      else          xbins.back() = hx->GetBinLowEdge(i+1);

      newbin = false;

      if ( xbins.size()>0 ) std::cout << i << "\tN " << N << " " << " (" << inveps2 << ")" << "\t" << xbins.back() << std::endl;
      
      if ( N >= inveps2 ) { 
	newbin = true;
	N = 0;
      }
    }
    
    std::cout << "finishsed " << xbins.size() << std::endl; 
    
    TH1D* hy = (TH1D*)h->ProjectionY("1dy", 1, h->GetNbinsX());
    hy->SetDirectory(0);
    for ( int i=0 ; i<=hy->GetNbinsX()+1 ; i++ ) ybins.push_back( hy->GetBinLowEdge(i+1) );
    
    std::cout << "combine" << std::endl;
    std::cout << "x bins " << hx->GetNbinsX() << "\t y bins " << hy->GetNbinsX() << std::endl;
    
    std::cout << "x: " << xbins.size() << " " << xbins[0] << " - " << xbins.back() << std::endl;  
    std::cout << "y: " << ybins.size() << " " << ybins[0] << " - " << ybins.back() << std::endl;  
    
    if ( xbins.size()==0 || ybins.size()==0 ) { 
      std::cerr << "Resplot::combine() bad limits for histogram: N xbins: " << xbins.size() << "\tN ybins: " << ybins.size() << std::endl;
      return 0;
    }

    TH2D* h2 = new TH2D( (std::string(h->GetName())+"-duff").c_str(), "duff", xbins.size()-1, &xbins[0],  ybins.size()-1, &ybins[0] ); 
    h2->SetDirectory(0); 
    

    for ( int j=1 ; j<=hy->GetNbinsX() ; j++ ) { 

      unsigned xbin = 0;

      for ( int i=1 ; i<=hx->GetNbinsX() ; i++ ) { 

	while ( hx->GetBinCenter(i)>xbins[xbin+1] && xbin<(xbins.size()-1) ) xbin++; 

	if ( hx->GetBinCenter(i)>=xbins[xbin] && hx->GetBinCenter(i)<xbins[xbin+1] ) { 

	  /// bin contentes
	  double n  = h->GetBinContent(i,j);
	  double nh = h2->GetBinContent(xbin+1, j);
	  h2->SetBinContent(xbin+1, j, nh+n);

	  /// bin error
	  double ne  = h->GetBinError(i,j);
	  double nhe = h2->GetBinError(xbin+1, j);
	  double _ne = std::sqrt( ne*ne + nhe*nhe );
	  h2->SetBinError(xbin+1, j, _ne );

	}
      }

    }

    h2->SetEntries( h->GetEntries() );
        
    delete hy;
    delete hx;
    
    return h2;
}










/// combine bins in a 2D histogram
/// it takes a value, x and all the bins with BinLowEdge > x get 
/// combined N bins at a time. 
// TH2D* Resplot::combine(const TH2* h, double epsilon) { 
TH1D* Resplot::combine(const TH1* h, double inveps2) { 
  return rebin( h, findbins(h, inveps2) );
}


std::vector<double> Resplot::findbins(const TH1* h, double inveps2) { 
    
  std::cout << "combine" << std::endl;
  
  //    if ( epsilon==0 ) return 0; 

    std::vector<double> xbins;


    //    hy->SetDirectory(0);
    //    hx->SetDirectory(0);

    //    double inveps2 = 1/(epsilon*epsilon);


    //    hx->DrawCopy();
    //    gPad->Print("cock.png");


    double N = 0;

    bool newbin = true;

    std::cout << "combining bins " << std::endl;

    for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) {

      N += h->GetBinContent(i);
      
      if ( xbins.size()==0 ) { 
	if ( N==0 ) continue;
	else xbins.push_back( h->GetBinLowEdge(i) );
      }

      if ( newbin ) xbins.push_back( h->GetBinLowEdge(i+1) );
      else          xbins.back() = h->GetBinLowEdge(i+1);

      newbin = false;

      if ( xbins.size()>0 ) std::cout << i << "\tN " << N << " " << " (" << inveps2 << ")" << "\t" << xbins.back() << std::endl;
      
      if ( N >= inveps2 ) { 
	newbin = true;
	N = 0;
      }
    }
    
    std::cout << "finishsed " << xbins.size() << std::endl; 
    
    std::cout << "combine" << std::endl;
    std::cout << "x bins " << h->GetNbinsX() << std::endl;
    
    std::cout << "x: " << xbins.size() << " " << xbins[0] << " - " << xbins.back() << std::endl;  
    
    return xbins;
}


TH1D* Resplot::rebin( const TH1* h, const std::vector<double>& xbins ) { 

    if ( xbins.size()==0 ) { 
      std::cerr << "Resplot::combine() bad limits for histogram: N xbins: " << xbins.size() << std::endl;
      return 0;
    }

    TH1D* h2 = new TH1D("duff","duff", xbins.size()-1, &xbins[0] ); 
    h2->SetDirectory(0); 
    
    unsigned xbin = 0;
    for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 

	while ( h->GetBinCenter(i)>xbins[xbin+1] && xbin<(xbins.size()-1) ) xbin++; 

	if ( h->GetBinCenter(i)>=xbins[xbin] && h->GetBinCenter(i)<xbins[xbin+1] ) { 
	  double n  = h->GetBinContent(i);
	  double nh = h2->GetBinContent(xbin+1);
	  h2->SetBinContent(xbin+1, nh+n);
	}
    }

    return h2;
}






std::vector<double> Resplot::getbins(const TH1* h) {
  std::vector<double> bins(h->GetNbinsX()+1);
  for ( int i=0 ; i<h->GetNbinsX()+1 ; i++ ) bins[i] = h->GetBinLowEdge(i+1);
  return bins;
}




// Get the efficiency for a single slice of the major ordinate

StatVal Resplot::GetEfficiency(TH1D* h, double lower, double upper) {
  double total = 0;
  double good = 0;
  StatVal v;    
  
  for (int i=0 ; i<=h->GetNbinsX()+1 ; i++ ) {
    total += h->GetBinContent(i);
  }
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) {
    double x = h->GetBinCenter(i);
    if ( x>lower && x<= upper ) {
      good += h->GetBinContent(i);
    }
  }
  if ( total>0 ) {
    v.value = good/total;
    v.error = sqrt(v.value*(1-v.value)/total);
  }
  
  return v;
}



// Get all efficiencies for all the major bins

TH1D* Resplot::GetEfficiencies(const std::string& hname, double lower, double upper) {
  //  TDirectory* cwd = cd(); 
  
  //  push();

  TH1D* e = (TH1D*) m_mean->Clone();
  e->SetName(hname.c_str()); e->SetMarkerStyle(20);
  if ( m_xaxis!="" ) e->SetXTitle(m_xaxis.c_str());
  

  for ( int i=1 ; i<=n_primary ; i++ ) {
    TH1D* s = m_h2d->ProjectionY(tagname(hname,i), i, i, "e");
    StatVal v = GetEfficiency(s, lower, upper);
    e->SetBinContent(i, v.value);
    e->SetBinError(i, v.error);
    delete s;
  }
  
  //  cwd->cd();
  
  //  pop();

  return e;
}

  



// Get all efficiencies for all the major bins

TH1D* Resplot::GetEfficiencies(const std::string& hname, double Nsigma ) {
  //  TDirectory* cwd = cd(); 
  //  push();

  // check that the fits have been done for this 
  // plot  
  if ( !m_finalised ) Finalise();  

  TH1D* e = (TH1D*) m_mean->Clone();
  e->SetName(hname.c_str()); e->SetMarkerStyle(20);
  if ( m_xaxis!="" ) e->SetXTitle(m_xaxis.c_str());
  

  for ( int i=1 ; i<=n_primary ; i++ ) {
    
    double mean  = m_mean->GetBinContent(i);
    double sigma = m_sigma->GetBinContent(i);

    TH1D* s = m_h2d->ProjectionY(tagname(hname,i), i, i, "e");
    StatVal v = GetEfficiency(s, mean-Nsigma*sigma, mean+Nsigma*sigma);
    e->SetBinContent(i, v.value);
    e->SetBinError(i, v.error);
    delete s;
  }
  
  //  cwd->cd();
  
  //  pop();

  return e;
}

  

// fit Gaussian to histogram

TF1* Resplot::FitGaussian(TH1D* s, double a, double b) {
  TF1* f1 = new TF1("gausr", "gaus"); 
  return FitInternal( s, a, b, f1 );
  //  s->GetXaxis()->SetRangeUser(-1, 1);
}

TF1* Resplot::FitRGaussian(TH1D* s, double a, double b) {
  TF1* f1 = new TF1("rgausr", "[0]*x*exp((x-[1])([1]-x)/(2*[2]*[2]))"); 
  return FitInternal( s, a, b, f1 );
}

TF1* Resplot::FitRBreit(TH1D* s, double a, double b) {
  TF1* f1 = new TF1("rbreitr", "x*[0]*[2]*[2]/([2]*[2]+(x-[1])*(x-[1]))");

  f1->SetParName(0, "Maximum");
  f1->SetParName(1, "Median");
  f1->SetParName(2, "Gamma");
    
  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,s->GetMean());
  f1->SetParameter(2,s->GetRMS());

  return FitInternal( s, a, b, f1 );
}


TF1* Resplot::FitInternal(TH1D* s, double a, double b, TF1* f1) {

  if ( f1==0 ) return f1;

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  double mean  = 0;
  double sigma = 0;


  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,mean=s->GetBinCenter(s->GetMaximumBin()));
  f1->SetParameter(2,sigma=s->GetRMS());

  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++) if (s->GetBinContent(j)) nbins++;
  
  if (nbins>2) {
    if ( a==-999 || b==-999 )  s->Fit(f1,"Q");
    else                       s->Fit(f1,"Q", "", a, b);
  }
  else for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j, 0);
  
  return f1;
}



TF1* Resplot::FitCentralGaussian(TH1D* s, double , double ) {
  TF1* f1 = new TF1("gausr", "gaus"); 

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  double mean  = 0;
  double sigma = 0;


  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,mean=s->GetBinCenter(s->GetMaximumBin()));
  f1->SetParameter(2,sigma=s->GetRMS());

  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++) if (s->GetBinContent(j)) nbins++;
  
  if (nbins>2) {

    TH1D* _s = (TH1D*)s->Clone("duff");
    _s->SetDirectory(0);
    _s->Fit(f1,"Q");
   
    if ( f1 ) { 
      /// if the fit is "bad" fit only to the central region
      
      /// calculate difference from maximum of fit, and hist maximum 
      
      //      double sig =  std::sqrt( f1->GetParError(0)*f1->GetParError(0) + s->GetBinError(s->GetMaximumBin())*s->GetBinError(s->GetMaximumBin()) );
      //      double diff = std::fabs( (f1->GetParameter(0) - s->GetBinContent(s->GetMaximumBin()) ) )/sig;

      //      std::cout << "diff " << diff << "\tfit " <<  f1->GetParameter(0) << "\thist " << s->GetBinContent(s->GetMaximumBin()) << std::endl;
      
      //      if ( diff>2 ) { 
      

      //      double n = s->GetBinContent(s->GetMaximumBin()); 

      double frac = 1.25;
  
      //      if ( n<25 ) { 
      //	frac = 1.2*n/25.0 + 4.0*(25.0-n)/25.0;
      //     }

      double llim = f1->GetParameter(1)-frac*f1->GetParameter(2);
      double ulim = f1->GetParameter(1)+frac*f1->GetParameter(2);


      
      int _nbins=0;
      for (int j=1 ; j<=s->GetNbinsX() ; j++) if ( s->GetBinCenter(j)>llim && s->GetBinCenter(j)<ulim  ) _nbins++;
      
      if ( _nbins>2 ) { 
	
	//      if ( frac>3 )  s->Fit(f1,"Q");
	//      else           s->Fit(f1,"Q", "", llim, ulim);
	
	if ( frac>3 )  s->Fit(f1,"Q");
	else           s->Fit(f1,"Q", "", llim, ulim);
	
      }
      else for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j, 0);

      delete _s;
 
    }

    //    std::cout << "Resplot::Finalise() " << s->GetName() << "\trange " << lower << " - " << upper << std::endl; 
    
  }
  else for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j, 0);
  
  return f1;
}



// or fit a Breit Wigner

TF1* Resplot::FitBreit(TH1D* s, double a, double b) {

  //  TF1* f1 = new TF1("breitr", "[0]*[2]*[2]/([2]*[2]+(x-[1])*(x-[1]))", -100, 100 );
  TF1* f1 = new TF1("breitr", "[0]*[2]*[2]/([2]*[2]+(x-[1])*(x-[1]))");

  f1->SetParName(0, "Maximum");
  f1->SetParName(1, "Median");
  f1->SetParName(2, "Gamma");
    
  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,s->GetMean());
  f1->SetParameter(2,s->GetRMS()*0.5);
  
  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  for (int j=1 ; j<=s->GetNbinsX() ; j++)  if (s->GetBinContent(j)==0) s->SetBinError(j, 1);


  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++)  if (s->GetBinContent(j)>0) nbins++;

  if (nbins>2) {
    // std::cout << "fitting " << f1 << std::endl;
    if ( a==-999 || b==-999 )  s->Fit(f1,"Q");
    else                       s->Fit(f1,"Q", "", a, b);
  }
  else { 
    for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j,0);
  }

  for (int j=1 ; j<=s->GetNbinsX() ; j++)  if (s->GetBinContent(j)==0) s->SetBinError(j, 0);
  
  return f1;
}


























TF1* Resplot::FitATan(TH1D* s, double , double ) {
  static int counter = 0;

  std::cout << "Resplot::FitATan() " << s->GetName() << " " << s->GetTitle() << std::endl;

  char name[8];
  sprintf(name,"atan_%02d", counter); counter++;
  
  //  static TF1* f1 = new TF1("atan", "(100-sqrt([2]*[2]))*(0.5*3.14159-atan([1]*pow((x-[0]),[4])))/3.14159+sqrt([2]*
  // [2])", -100, 100);
  //  static TF1* f1 = new TF1("atan", "   100*pow(0.01*([3]-sqrt([2]*[2]))*(0.5*3.14159-atan( [1]*(x-[0]) ))/3.14159+
  //						   sqrt([2]*[2]),[4])   ", -100, 100);
  
  TF1* f1 = new TF1(name, "100*pow( (0.01*[3]-sqrt(0.0001*[2]*[2]))*(0.5-(1/3.14159)*atan( [1]*(x-[0]) ) )+sqrt(0.0001*[2]*[2]) , [4] )   ", -100, 100);
  
  f1->SetParName(0, "Mean");
  f1->SetParName(1, "Gradient");
  f1->SetParName(2, "Pedestal");
  f1->SetParName(3, "Norm");
  f1->SetParName(4, "Power");
  
  int maxn=1;
  for ( int i=1 ; i<=s->GetNbinsX() ; i++ ) if ( s->GetBinContent(maxn)<s->GetBinContent(i) ) maxn = i;  
  
  
  //  f1->SetParameter(0,s->GetBinContent(maxn) );
  //  f1->SetParameter(1,s->GetBinCenter(s->GetNbinsX()/2) );
  //  f1->SetParameter(2,0.25);
  
  f1->SetParameter(0, 2 );
  f1->SetParameter(1, 10 );
  f1->SetParameter(2, 0.001 );
  f1->SetParameter(3, 100 );
  f1->SetParameter(4, 1 );

  f1->FixParameter(2, 0);
  f1->FixParameter(3, 100);
  // f1->FixParameter(4, 1);

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  
  //  int nbins=0;

 
  //  ZeroErrors(s);
  s->SetMarkerStyle(20);
  

  // int n=0;
  // for ( int i=1 ; i<=s->GetNbinsX() ; i++ ) if ( s->GetBinContent() ) n++;
  // if ( n>2 ) s->Fit(f1,"Q");

  s->Fit(f1,"Q");
  
  std::cout << "par0 = " <<  f1->GetParameter(0) << std::endl;
  std::cout << "par1 = " <<  f1->GetParameter(1) << std::endl;
  std::cout << "par2 = " <<  f1->GetParameter(2) << std::endl;
  std::cout << "par3 = " <<  f1->GetParameter(3) << std::endl;
  std::cout << "par4 = " <<  f1->GetParameter(4) << std::endl;

  
  //  unZeroErrors(s);
  //  s->SetMaximum(f1->GetParameter(0)*1.1);  
  //  double range = s->GetBinLowEdge(s->GetNbinsX()+1) - s->GetBinLowEdge(1) ; 
  
  return f1;
}



TF1* Resplot::FitNull(TH1D* s, double , double ) {

  // unZeroErrors(s);

  TF1* f=new TF1("null", "[0]+[1]+[2]");
  
  f->SetParName(0, "Maximum");
  f->SetParName(1, "Mean");
  f->SetParName(2, "RMS");

  f->FixParameter(0, s->GetMaximum()); f->SetParError(0, sqrt(s->GetMaximum()));
  f->FixParameter(1, s->GetMean());    f->SetParError(1, s->GetMeanError());
  f->FixParameter(2, s->GetRMS());     f->SetParError(2, s->GetRMSError());

  //  std::cout << gDirectory->GetName() << "  " << s->GetName() << "\tFitNull mean: " <<  f->GetParameter(1) << "\tsigma: " << f->GetParameter(2) << std::endl;

  return f;
}



double FindMean(TH1D* s, double frac=0.95) {

  //  double entries = s->GetEntries() + s->GetBinContent(0) + s->GetBinContent(s->GetNbinsX()+1);
  double entries = generate::GetEntries(s,0,s->GetNbinsX()+1);

  //  std::cout << s->GetName() << "\tentries: " << entries << std::endl;

  if ( entries==0 ) return 0;  
  
  //  s->GetXaxis()->SetRangeUser(-100, 100);
  //  s->GetXaxis()->UnZoom();
  s->GetXaxis()->SetRange(1,s->GetNbinsX());

  double mean  = s->GetMean();
  double meane = s->GetMeanError();

  int upperbin = 0;
  int lowerbin = 0;
    

  for ( int it=0 ; it<20 ; it++ ) { 

    //    std::cout << it << "\tmean " << mean << " +- " << meane << std::endl; 

    int imax =  s->GetXaxis()->FindBin(mean);
    
    double sumn  = s->GetBinContent(imax); 
    
    //    double uppersum = 0; 
    //    double lowersum = 0; 
    
    upperbin = imax;
    lowerbin = imax;
    
  
    int i=1;
    while ( true ) { 
      
      int _upperbin = imax+i;
      int _lowerbin = imax-i;
      
      if ( _upperbin>s->GetNbinsX() || _lowerbin<1 ) break; 
      
      double tsum = sumn + s->GetBinContent(_upperbin) + s->GetBinContent(_lowerbin);
      
      if ( tsum>entries*frac ) { 
	//	uppersum = tsum/entries; 
	break; 
      }
      
      //      lowersum = tsum/entries;
      
      sumn = tsum;
      
      upperbin = _upperbin;
      lowerbin = _lowerbin;
      
      i++;
    }
     
    s->GetXaxis()->SetRange(lowerbin, upperbin);
    
    double m  = s->GetMean();
    double me = s->GetMeanError(); 
    
    if ( it>0 ) { 
      if ( mean==m || 
	   std::fabs(mean-m)<me*1e-5 || 
	   std::fabs(mean-m)<meane*1e-5 ) { 
	mean = m;
	meane = me;
	break; 
      }
    }
    
    mean = m;
    meane = me;
    
  }
  
  //  std::cout << "mean " << mean << " +- " << meane << "\tentries: " << generate::GetEntries(s) << "\tlower " << lowerbin << " - " << upperbin << std::endl; 

  
  return mean;
}


int findmax(TH1D* s) { 
  int imax = 1;
  for ( int i=2 ; i<=s->GetNbinsX() ; i++ ) {
    /// find maximum bin
    if ( s->GetBinContent(i)>s->GetBinContent(imax) ) imax = i;
  } 
  return imax;
}



// TF1* Resplot::FitNull95Obsolete(TH1D* s, double frac, bool useroot ) {
TF1* Resplot::FitNull95Obsolete(TH1D* s, double frac, bool useroot ) {

  TF1* f=new TF1("null", "[0]+[1]+[2]");
  
  f->SetParName(0, "Maximum");
  f->SetParName(1, "Mean");
  f->SetParName(2, "RMS");

  f->SetParameter(0, 0);
  f->SetParameter(1, 0);
  f->SetParameter(2, 0);

  if ( s==0 )            { std::cerr << "Resplot::FitNull95() histogram s = " << s << std::endl; return 0; }
  if ( s->GetXaxis()==0) { std::cerr << "Resplot::FitNull95() histogram s->GetXaxis() not definied for histogram " << s->GetName() << std::endl; return 0; }

  unZeroErrors(s);

  s->GetXaxis()->SetRange(1,s->GetNbinsX());

#if 0
  std::cout << "FitNull95 " << s->GetName() 
	    << " " << generate::GetEntries(s)   
	    << " " << generate::GetEntries(s, 0, s->GetNbinsX()+1)  
	    << std::endl;
#endif


  if  ( generate::GetEntries(s)<10 ) return f;
 
  //  std::cout << "---------------------------------------\nFN95O frac: " << frac << " " << s << std::endl;

  //  std::cout << __LINE__ << std::endl;
  


  //  std::cout << __LINE__ << " " << s->GetXaxis() << std::endl;

  /// this line causes a crash, even though s->GetXaxis() returns 
  /// the x axis ok
  // s->GetXaxis()->UnZoom();

  //  std::cout << __LINE__ << std::endl;
  
  //  double m = s->GetMean();
  double m = FindMean(s,frac);

  //  std::cout << __LINE__ << std::endl;

  int imax =  s->GetXaxis()->FindBin(m);

  //  std::cout << "mean " << m << " " << imax << " max " << s->GetBinCenter(findmax(s)) << " " << findmax(s) << std::endl;

  f->FixParameter(0, s->GetMaximum()); f->SetParError(0, std::sqrt(s->GetMaximum()));
  //  f->FixParameter(1, s->GetMean());    f->SetParError(1, s->GetMeanError());

  //  double entries = s->GetEntries() + s->GetBinContent(0) + s->GetBinContent(s->GetNbinsX()+1);
  double entries = generate::GetEntries(s,0,(s->GetNbinsX()+1));

  //  std::cout << "entries: " << entries << "\t" << generate::GetEntries(s) << "\t" << generate::GetEntries(s,0,s->GetNbinsX()+1) << std::endl;

  //  double sumx  = s->GetBinContent(imax)*s->GetBinCenter(imax);
  //  double sumx2 = s->GetBinContent(imax)*s->GetBinCenter(imax)*s->GetBinCenter(imax);

  double sumn  = s->GetBinContent(imax); 
  
  int upperbin = imax;
  int lowerbin = imax;
  
  double uppersum = 0; 
  double lowersum = 0; 
  
  if ( sumn>entries*frac ) {
    s->GetXaxis()->SetRange(1,s->GetNbinsX());
    ErrorSet = BINWIDTH;
    return f;
  }

  if ( entries>0 ) {  

    int i=1;
    while ( true ) { 
      
      int _upperbin = imax+i;
      int _lowerbin = imax-i;
      
      if ( _upperbin>s->GetNbinsX() || _lowerbin<1 ) break; 
      
      double tsum = sumn + s->GetBinContent(_upperbin) + s->GetBinContent(_lowerbin);
      
      //      std::cout << i << " frac: " << lowersum 
      //		<< "\tx " << s->GetBinCenter(lowerbin) 
      //		<< "\t "  << s->GetBinCenter(upperbin)
      //		<< "\ttsum " << tsum
      //		<< "\tentries " << entries
      //		<< std::endl;

      if ( tsum>entries*frac ) { uppersum = tsum/entries; break; }
      
      lowersum = tsum/entries;

      sumn = tsum;
      
      upperbin = _upperbin;
      lowerbin = _lowerbin;
      
      //      if ( std::fabs(lowersum-frac)<0.1 ) 

      i++;
    }
  }    

  if ( uppersum==0 ) { 
    s->GetXaxis()->SetRange(1,s->GetNbinsX());
    ErrorSet = HISTOWIDTH;
    return f;
  } 

  //  std::cout << "FitNull95 " << s->GetName() << "\tlower bin " << lowerbin << "\t upper bin " << upperbin << std::endl;

  //  std::cout << "full rms " << s->GetRMS() << " +- " << s->GetRMSError() << "\t n=" << s->GetNbinsX() 
  //	    << "\tfrac " << ( entries>0 ? sumn/entries : 0 ) << " - " << uppersum 
  //	    << std::endl;

  //  std::cout << "upper " << upperbin << " " << lowerbin << " " << interpolate_flag << std::endl;

  //  if ( upperbin!=lowerbin ) { 
  if ( true ) { // technically wrong - 95% contained in only 3 bins, so RMS of the "central" bin is not defined

    double llim = s->GetBinLowEdge(1);
    double ulim = s->GetBinLowEdge(s->GetNbinsX()+1);

    double rms  = 0;
    double erms = 0;

    std::vector<double> stats;

    //    double sentries = generate::GetEntries(s);

    //    std::cout << "GetEntries " << s->GetName() << " " << sentries << std::endl; 

    if ( interpolate_flag ) { 
      s->GetXaxis()->SetRange(lowerbin, upperbin);

      //      std::cout << "GetEntries " << s->GetName() << " " << generate::GetEntries(s,lowerbin,upperbin)/sentries << std::endl; 

      double rlower;
      double erlower;

      double rupper;
      double erupper;
      

      /// root does not calculate the uncertainty on the rms correctly

      if ( !useroot ) { 
	GetStats( s, stats );
	rlower  = stats[2];
	erlower = stats[3];
      }
      //      else { 
	rlower  = s->GetRMS();     
	erlower = s->GetRMSError();    
	//   }

	//      std::cout <<  "FitNULL95()  " << s->GetName() << "\tmean " << m << "\t lower rms " << rlower << " +- " << s->GetRMSError() << " (wrong)\t " << stats[2] << " +- " << stats[3] << " (correct)" << std::endl;
	//      std::cout << s->GetName() << " " << s->GetTitle() << "\t lower 95% rms " << rlower << " +- " << erlower << "\t( " << lowerbin << " - " << upperbin << " )" << std::endl;
 
      s->GetXaxis()->SetRange(lowerbin-1, upperbin+1);

      //      std::cout << "GetEntries " << s->GetName() << " " << generate::GetEntries(s,lowerbin-1,upperbin+1)/sentries << std::endl; 

      //      std::cout << "Range " << s->GetBinLowEdge(lowerbin) << " - " << s->GetBinLowEdge(upperbin+1) << std::endl; 

      if ( !useroot ) { 
	GetStats( s, stats );
	rupper  = stats[2];
	erupper = stats[3];
      }
      //      else { 
	rupper  = s->GetRMS();     
	erupper = s->GetRMSError();     
	//   }
      
      //      std::cout <<  "FitNULL95()  " << s->GetName() << "\t upper rms " << rupper << " +- " << s->GetRMSError() << " (wrong)\t " << stats[2] << " +- " << stats[3] << " (correct)" << std::endl;

      //      std::cout << s->GetName() << " " << s->GetTitle() << "\t upper 95% rms " << rupper << " +- " << erupper << "\t( " << lowerbin-1 << " - " << upperbin+1 << " )" << std::endl;

      
      

      rms  = rlower + (frac-lowersum)*(rupper-rlower)/(uppersum-lowersum);
      erms = erlower + (frac-lowersum)*(erupper-erlower)/(uppersum-lowersum);

    }
    else { 
      s->GetXaxis()->SetRange(lowerbin, upperbin);
      if ( !useroot ) { 
	GetStats( s, stats );
	rms  = stats[2];
	erms = stats[3];
      }
      else { 
	rms  = s->GetRMS();     
	erms = s->GetRMSError();     
      }
    }
    
#if 0
      
      std::cout << "numbers " << s->GetName() 
		<< " "   <<  generate::GetEntries(s,lowerbin,upperbin)
		<< " - " <<  generate::GetEntries(s,lowerbin-1,upperbin+1) << "\ttotal " << generate::GetEntries(s,0,s->GetNbinsX()+1) << std::endl;
      
      std::cout << "fractions " << s->GetName() 
		<< "\t"  << generate::GetEntries(s,lowerbin,upperbin)/generate::GetEntries(s,0,s->GetNbinsX()+1) 
		<< " - " << generate::GetEntries(s,lowerbin-1,upperbin+1)/generate::GetEntries(s,0,s->GetNbinsX()+1) << std::endl;
      
      std::cout << "lowersum " << s->GetName() << "\t" << lowersum << "\tuppersum " << uppersum << std::endl;
      
      std::cout << "limits " << s->GetName() << "\t" << s->GetBinLowEdge(lowerbin)  << " - " <<  s->GetBinLowEdge(upperbin+1) << std::endl;
      
      
      
      std::cout <<  "FitNULL95Obsolete()  " << s->GetName() << "\t rms " << rms << " +- " << erms << "\t inv " << 1/rms << std::endl;
      
      printf("rms %12.10lf   inv %12.10lf\n", rms, 1/rms ); 
      
      printf("rms68 %12.10lf   inv %12.10lf\n", rms/0.5369760683, rms*1.8622803865 ); 
#endif


    /// a "corrected" rms of the inner 0.95% of the data, 
    /// corrected such that for a gaussian, will return 1.
    double scale = 1;
    if ( scalerms95 ) scale = 1.1479538518; 


    
    //    f->FixParameter(2, s->GetRMS());     
    //    f->SetParError(2, s->GetRMSError());
    f->FixParameter(2, scale*rms);     
    f->SetParError(2, scale*erms);
    f->FixParameter(1, s->GetMean());     
    f->SetParError(1, s->GetMeanError());
    // std::cout << s->GetName() << " " << s->GetTitle() << "\t 95% inter rms " << rms << " +- " << erms << std::endl;
    s->GetXaxis()->SetRangeUser(llim, ulim);

  }
  else {  
    f->FixParameter(1, 0);  
    f->SetParError(1, 0);
    f->FixParameter(2, 0);  
    f->SetParError(2, 0);
    // std::cout << " 95% rms " << 0 << " +- " << 0 << "\t( " << lowerbin << " - " << upperbin << " )" << std::endl;
  }

  s->GetXaxis()->SetRange(1,s->GetNbinsX());

  return f;
}





TF1* Resplot::FitNull95(TH1D* s, double , double  ) {
  //  if ( oldrms95 ) return Resplot::FitNull95Obsolete(s, 0.95, true ); /// use approximate root errors for speed
  if ( oldrms95 ) return Resplot::FitNull95Obsolete( s, 0.95 ); /// use approximate root errors for speed
  else            return Resplot::FitNull95New(s);
} 



int _nexperiments     = 0;
int  nexperiments_max = 40;
int  nexperiments_min = 20;



TF1* Resplot::FitNull95New(TH1D* s, double, bool ) { // double frac, bool useroot ) {

  //  std::cout << "---------------------------------------\nFN95 " << std::endl;
 
  TF1* f=new TF1("null", "[0]+[1]+[2]");
  
  f->SetParName(0, "Maximum");
  f->SetParName(1, "Mean");
  f->SetParName(2, "RMS");

  f->SetParameter(0, 0);
  f->SetParameter(1, 0);
  f->SetParameter(2, 0);
  
  if ( s->GetEffectiveEntries()==0 ) return f; 


  //  double _entries = getWeights( s );
  double _entries = s->GetEffectiveEntries();


  //  int nexperiments = 20+int(800/std::sqrt(s->GetEntries()));
  //  int nexperiments = 20+int(800/std::sqrt(s->GetEntries()));
  int nexperiments = 20+int(1000/_entries);
  
  if ( _nexperiments!=0 ) nexperiments = _nexperiments;


  if ( nexperiments>nexperiments_max ) nexperiments = nexperiments_max;
  if ( nexperiments<nexperiments_min ) nexperiments = nexperiments_min;
  
  //  std::cout << "FitNull95  entries " << _entries << "\tnexperiments " << nexperiments << std::endl; 

  //  std::cout << "experiments " << _nexperiments << std::endl;

  //  std::cout << "h entries " << s->GetEntries() << "\tn experiments " << nexperiments << "\tN " << s->GetEntries()*nexperiments  << std::endl;

  /// calculate the 
  generate::experiment e( s, nexperiments, _entries );
  
  /// a "corrected" rms of the inner 0.95% of the data, 
  /// corrected such that for a gaussian, will return 1.
  double scale = 1;
  if ( scalerms95 ) scale = 1.1479538518; 

  //  f->FixParameter( 1, s->GetMean() );     
  f->FixParameter( 1, e.hmean() );     
  f->SetParError(  1, e.mean_error() );

  f->FixParameter( 2, scale*e.hrms() );     
  f->SetParError(  2, scale*e.rms_error() );

  //  s->GetXaxis()->SetRange(1, s->GetNbinsX());

  //  s->GetXaxis()->SetRangeUser(-1, 1);
  
  return f;
}









TF1* Resplot::FitNull95Central(TH1D* s) {

  TF1* f=new TF1("null", "[0]+[1]+[2]");
  
  f->SetParName(0, "Maximum");
  f->SetParName(1, "Mean");
  f->SetParName(2, "RMS");

  f->SetParameter(0, 0);
  f->SetParameter(1, 0);
  f->SetParameter(2, 0);

  if ( s==0 )            { std::cerr << "Resplot::FitNull95() histogram s = " << s << std::endl; return 0; }
  if ( s->GetXaxis()==0) { std::cerr << "Resplot::FitNull95() histogram s->GetXaxis() not definied for histogram " << s->GetName() << std::endl; return 0; }

  //  std::cout << "FitNull " << s->GetName() << " " << generate::GetEntries(s) << std::endl;

  if  ( generate::GetEntries(s)<20 ) return f;
  // if  ( s->GetEffectiveEntries<20 ) return f;

  s->GetXaxis()->SetRange(1,s->GetNbinsX());
  //  s->GetXaxis()->UnZoom();


  /// NB: try to find with respect to maximum bin, rather than the mean.
  ///  double m = s->GetMean();

  ///  double m = FindMean(s);
  ///  int imax =  s->GetXaxis()->FindBin(m);

  int imax = findmax(s);

  f->FixParameter(0, s->GetMaximum()); f->SetParError(0, sqrt(s->GetMaximum()));
  //  f->FixParameter(1, s->GetMean());    f->SetParError(1, s->GetMeanError());


  double entries = s->GetEntries() + s->GetBinContent(0) + s->GetBinContent(s->GetNbinsX()+1);

  //  double sumx  = s->GetBinContent(imax)*s->GetBinCenter(imax);
  //  double sumx2 = s->GetBinContent(imax)*s->GetBinCenter(imax)*s->GetBinCenter(imax);
  double sumn  = s->GetBinContent(imax); 
  
  double frac = 0.95;
  
  int upperbin = imax;
  int lowerbin = imax;
  
  double uppersum = 0; 
  double lowersum = 0; 
  
  if ( entries>0 ) {  

    int i=1;
    while ( true ) { 
      
      int _upperbin = imax+i;
      int _lowerbin = imax-i;
      
      if ( _upperbin>s->GetNbinsX() || _lowerbin<1 ) break; 
      
      double tsum = sumn + s->GetBinContent(_upperbin) + s->GetBinContent(_lowerbin);
      
      if ( tsum>entries*frac ) { uppersum = tsum/entries; break; }
      
      lowersum = tsum/entries;

      sumn = tsum;
      
      upperbin = _upperbin;
      lowerbin = _lowerbin;
      
      i++;
    }
  }    

  //  std::cout << "FitNull95 " << s->GetName() << " " << upperbin << " " << lowerbin << std::endl;

  //  std::cout << "full rms " << s->GetRMS() << " +- " << s->GetRMSError() << "\t n=" << s->GetNbinsX() 
  //	    << "\tfrac " << ( entries>0 ? sumn/entries : 0 ) << " - " << uppersum 
  //	    << std::endl;

  //  std::cout << "upper " << upperbin << " " << lowerbin << " " << interpolate_flag << std::endl;

  if ( upperbin!=lowerbin ) { 

    double llim = s->GetBinLowEdge(1);
    double ulim = s->GetBinLowEdge(s->GetNbinsX()+1);

    double rms  = 0;
    double erms = 0;

    std::vector<double> stats;

    if ( interpolate_flag ) { 
      s->GetXaxis()->SetRange(lowerbin, upperbin);


      double rlower;
      double erlower;

      double rupper;
      double erupper;
      

      /// root does not calculate the uncertainty on the rms correctly

      GetStats( s, stats );
      rlower  = stats[2];
      erlower = stats[3];
   
      //      std::cout <<  "FitNULL95()  " << s->GetName() << "\t lower rms " << rlower << " +- " << s->GetRMSError() << " (wrong)\t " << stats[2] << " +- " << stats[3] << " (correct)" << std::endl;
      //      std::cout << s->GetName() << " " << s->GetTitle() << "\t lower 95% rms " << rlower << " +- " << erlower << "\t( " << lowerbin << " - " << upperbin << " )" << std::endl;
 
      s->GetXaxis()->SetRange(lowerbin-1, upperbin+1);

      GetStats( s, stats );
      rupper  = stats[2];
      erupper = stats[3];
         
      //      std::cout <<  "FitNULL95()  " << s->GetName() << "\t upper rms " << rupper << " +- " << s->GetRMSError() << " (wrong)\t " << stats[2] << " +- " << stats[3] << " (correct)" << std::endl;

      //      std::cout << s->GetName() << " " << s->GetTitle() << "\t upper 95% rms " << rupper << " +- " << erupper << "\t( " << lowerbin-1 << " - " << upperbin+1 << " )" << std::endl;

      rms  = rlower + (0.95-lowersum)*(rupper-rlower)/(uppersum-lowersum);
      erms = erlower + (0.95-lowersum)*(erupper-erlower)/(uppersum-lowersum);

    }
    else { 
      s->GetXaxis()->SetRange(lowerbin, upperbin);
      GetStats( s, stats );
      rms  = stats[2];
      erms = stats[3];
  
    }


    //    f->FixParameter(2, s->GetRMS());     
    //    f->SetParError(2, s->GetRMSError());
    f->FixParameter(2, rms);     
    f->SetParError(2, erms);
    f->FixParameter(1, s->GetMean());     
    f->SetParError(1, s->GetMeanError());
    // std::cout << s->GetName() << " " << s->GetTitle() << "\t 95% inter rms " << rms << " +- " << erms << std::endl;
    s->GetXaxis()->SetRangeUser(llim, ulim);
  }
  else {  
    f->FixParameter(1, 0);  
    f->SetParError(1, 0);
    f->FixParameter(2, 0);  
    f->SetParError(2, 0);
    // std::cout << " 95% rms " << 0 << " +- " << 0 << "\t( " << lowerbin << " - " << upperbin << " )" << std::endl;
  }

  s->GetXaxis()->SetRange(1, s->GetNbinsX());

  return f;
}













// or fit a Poissonian

TF1* Resplot::FitPoisson(TH1D* s, double a, double b) {

  char fstring[1024];  
  sprintf(fstring, "sqrt([0]*[0])*pow(sqrt([1]*[1]*([2]*[2])),x*[2]-[3])*exp(-sqrt([1]*[1]*([2]*[2])))/TMath::Gamma((x*[2]-[3])+1)");
  TF1 *f1 = new TF1("poisson", fstring );
  // TF1* f1 = new TF1("poiss","[0]*TMath::Poisson(x*[2],[1])");

  f1->SetParName(0, "Maximum");
  f1->SetParName(1, "Mean");
  f1->SetParName(2, "Scale");
  f1->SetParName(3, "Offset");

  f1->SetNpx(5000);
  f1->SetLineWidth(1);
  f1->SetLineColor(s->GetLineColor());
    
  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++)  if (s->GetBinContent(j)>0) nbins++;
  
  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,s->GetBinCenter(s->GetMaximumBin()));
  f1->SetParameter(2,1);
  f1->FixParameter(3,0);
  
  if (nbins>2) {
    // std::cout << "fitting " << f1 << std::endl;
    //    if ( a==-999 || b==-999 )  s->Fit(f1,"Q");
    //    else                       s->Fit(f1,"Q", "", a, b);
    if ( a==-999 || b==-999 )  s->Fit(f1);
    else                       s->Fit(f1,"", "", a, b);
  }
  else { 
    for ( int j=0 ; j<4 ; j++ ) f1->SetParameter(j,0);
  } 

  return f1;
}



TF1* Resplot::FitXExp(TH1D* s, double a, double b) {

  //  TF1* f1 = new TF1("breitr", "[0]*[2]*[2]/([2]*[2]+(x-[1])*(x-[1]))", -100, 100 );
  //  TF1* f1 = new TF1("xexpr", "[0]*(pow([2]*x,[1])*exp(-[2]*x))+[3]");
  //  TF1* f1 = new TF1("xexpr", "[0]*(pow((1.0/[2])*(x-[3]),[1])*exp(-(1.0/[2])*(x-[3])))");
  TF1* f1 = new TF1("xexpr", "[0]*(pow((1.0/[2])*x,[1])*exp(-(1.0/[2])*x))+[3]");

  f1->SetParName(0, "Normalisation");
  f1->SetParName(1, "Power");
  f1->SetParName(2, "Lifetime");
  f1->SetParName(3, "offset");
    
  //  f1->SetParameter(0, 1e-156);
  f1->SetParameter(0, s->GetBinContent(s->GetMaximumBin()) );
  //  f1->SetParameter(1, -1*(s->GetBinCenter(s->GetMaximumBin())) );
  f1->SetParameter(2, s->GetRMS() );
  // f1->SetParameter(1, 0);
  f1->SetParameter(1, 0);
  f1->SetParameter(3, 0);
  
  //  f1->FixParameter(0, 1);
  //  f1->FixParameter(1, 25);
  //  f1->FixParameter(1, 0);
  //  f1->FixParameter(3, 0);

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++)  if (s->GetBinContent(j)>0) nbins++;

  //  std::cout << "starting exp fit p1=" << f1->GetParameter(1) << std::endl;

  if (nbins>2) {
    // std::cout << "fitting " << f1 << std::endl;
    if ( a==-999 || b==-999 )  s->Fit(f1, "Q");
    else                       s->Fit(f1, "Q", "", a, b);
  }
  else { 
    for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j,0);
  }
  
  //  std::cout << "done" << std::endl;

  return f1;
}






Double_t langaufun(Double_t *_x, Double_t *par) {         

  //Fit parameters:
  //par[0]=Total area (integral -inf to inf, normalization constant)
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Width (scale) parameter of Landau density
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 500;   // number of convolution steps
      Double_t sc =   5;   // convolution extends to +-sc Gaussian sigmas

      double& x = _x[0];     

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;

      //      if ( par[2]<0.1*par[3] ) return 0;

      // correct the most probable bin position as described above
      mpc = par[1] - mpshift*par[2];

      // Range of convolution integral

      //      if ( par[3]<par[2] ) np = np*par[2]/par[3];
      
      xlow = x - sc * par[3];
      xupp = x + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-0.5)*step;
         fland = TMath::Landau(xx,mpc,par[2]) / par[2];
         sum += fland * TMath::Gaus(x,xx,par[3]);

         xx = xupp - (i-0.5)*step;
         fland = TMath::Landau(xx,mpc,par[2]) / par[2];
         sum += fland * TMath::Gaus(x,xx,par[3]);
      }

      return (par[0] * step * sum * invsq2pi / par[3]);
}



// fit Gaussian to histogram

TF1* Resplot::FitLandauGauss(TH1D* s, double a, double b) {
  //  TF1* f1 = new TF1("landgaus", "landgaus");
  // root is STUPID and you have to specify the FIT RANGE if you 
  // want to specify the number of fit parameters!!! 
  TF1* f1 = new TF1("landgaus", langaufun, 0, 1, 4); 

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  // parameter starting values
  double start_vals[4]; // = { 0.1, 3.0, 1.0, 0.3 };

  start_vals[0] = s->GetEntries();
  start_vals[1] = s->GetBinCenter(s->GetMaximumBin());
  start_vals[2] = s->GetRMS();
  start_vals[3] = 0.5*start_vals[2];
    
  f1->SetParNames("Area", "MP", "Width", "GSigma");
  f1->SetParameters(start_vals);
  
  // parameter low and high values for the fit
  // double par_lo[4] = {     0.0,   -20.0,  0.0, 0.00 };
  // double par_hi[4] = { 20000.0, 50000.0, 50.0, 0.01 };  
  //  for ( int i=0 ; i<4 ; i++ ) f1->SetParLimits(i, par_lo[i], par_hi[i]); 
  f1->SetParLimits( 2, 0.1*start_vals[2], 10*start_vals[2] ); 
  f1->SetParLimits( 3, 0.0,               10*start_vals[2] ); 
 
  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++) if (s->GetBinContent(j)) nbins++;
  
  if (nbins>2) {
    if ( a==-999 || b==-999 )  s->Fit(f1,"Q");
    else                       s->Fit(f1,"Q", "", a, b);
    // if ( a==-999 || b==-999 )  s->Fit(f1);
    // else                       s->Fit(f1,"", "", a, b);
  }
  else for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j, 0);
  
  return f1;
}


