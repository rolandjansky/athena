/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this class keeps track of chisquare derivatives. it makes
// histograms of the projections to the derivatives, which is kine of
// useful, sometimes.

namespace TRTAlign
{

  class HistoChisqProjection
  {
  public:
    HistoChisqProjection(const char* name, int dim, int nbins, float xmin, float xmax) ;
    ~HistoChisqProjection() ;

    // use this if the weight matrix for first and second derivative are equal
    void add( const HepMatrix& deriv, const HepSymMatrix& weight,  
	      const HepSymMatrix& variance, const HepVector& residual ) {
      addfirst( deriv, weight, variance, residual ) ;
      addsecond( deriv, weight ) ;
    }

    // use this if they are not (like in global chisquare method)
    void add( const HepMatrix& deriv, const HepSymMatrix& weight, 
	      const HepSymMatrix& variance, const HepVector& residual, const HepSymMatrix& secondweight) {
      addfirst( deriv, weight, variance, residual ) ;
      addsecond( deriv, secondweight ) ;
    }
    
    HepVector getFittedHalfDChisqDX(std::ostream& os=std::cout) const ;
    HepVector getNormalHalfDChisqDX() const { return m_halfdChisqdX ; } 

    // set the TDirectory where the histograms are kept
    void setDirectory(TDirectory* dir) { m_h2->SetDirectory(dir) ; m_unweightedh2->SetDirectory(dir) ; }

  private:
    void addfirst( const HepMatrix& deriv, const HepSymMatrix& weight, const HepSymMatrix& variance, const HepVector& residual ) ;
    void addsecond( const HepMatrix& deriv, const HepSymMatrix& weight) ;
    
  private:
    int m_dim ;
    TH2* m_h2 ;
    TH2* m_unweightedh2 ;
    std::vector<double> m_integrals ;
    HepVector m_halfdChisqdX ;
    HepSymMatrix m_halfd2ChisqdX2 ;
  } ;
  
  HistoChisqProjection::HistoChisqProjection(const char* name, int dim, int nbins, float xmin, float xmax) 
    : m_dim(dim), m_integrals(dim,0),m_halfdChisqdX(dim,0)
  {
    m_h2 = new TH2F("h2",name,dim,-0.5,dim-0.5,nbins,xmin,xmax) ;
    m_unweightedh2 = new TH2F("unweightedh2",name,dim,-0.5,dim-0.5,nbins,xmin,xmax) ;
    m_h2->Sumw2() ;
  } ;
  
  void HistoChisqProjection::addfirst(const HepMatrix& deriv, const HepSymMatrix& weight, 
				      const HepSymMatrix& variance, const HepVector& residual )
  {
    int dimr = weight.num_row() ;
    HepMatrix derivtimesweight(deriv*weight) ;
    
    for(int irow=1; irow<=dimr; ++irow) {
      double thissigma = sqrt( variance.fast(irow,irow) ) ;
      
      for(int ipar=0; ipar<m_dim; ++ipar) {
	double thisweight = derivtimesweight(ipar+1, irow ) * thissigma ;
	double thispull   = residual(irow) / thissigma ;
	if( thisweight< 0 ) {
	  thisweight*=-1 ;
	  thispull*=-1 ;
	}
	m_h2->Fill( double(ipar), thispull, thisweight ) ;
	m_unweightedh2->Fill( double(ipar), thispull ) ;
	m_integrals[ipar] += thisweight ;
	m_halfdChisqdX(ipar+1) += thisweight*thispull ;
      }
    }
  }

  void HistoChisqProjection::addsecond(const HepMatrix& deriv, const HepSymMatrix& weight )
  {
    m_halfd2ChisqdX2 += weight.similarity(deriv) ;
  }

  double binwidth(1) ;
  double gausfunc(double* x, double* par)
  {
    double integral = par[0] ;
    double mean  = par[1] ;
    double sigma = par[2] ;
    
    double xi   = (x[0] - mean)/sigma ;
    static double sqrt2pi = sqrt(2*M_PI) ;
    double norm = integral * binwidth / ( sigma*sqrt2pi ) ;
    
    return norm * exp(-0.5*xi*xi) ;
  } ;

 
  HepVector
  HistoChisqProjection::getFittedHalfDChisqDX(std::ostream& os) const {
    HepVector halfDChisqDX(m_dim,0) ;
    // perform a fit to each hist
    TF1 f1("mygaus",gausfunc,m_h2->GetYaxis()->GetXmin(),m_h2->GetYaxis()->GetXmax(),3) ;
    for(int ipar=1; ipar<=m_dim; ++ipar) {
      char hisname[256] ;
      sprintf(hisname,"tmph1_%d",ipar) ;
      TH1* tmph1 = m_h2->ProjectionY(hisname,ipar,ipar,"e") ; // the 'e' triggers the error calculation (quite important!)
      binwidth = tmph1->GetXaxis()->GetBinWidth(1) ;
      tmph1->SetDirectory(m_h2->GetDirectory()) ;
      f1.SetParameter(0,tmph1->Integral());
      f1.SetParLimits(0,0.5*tmph1->Integral(),2*tmph1->Integral()) ;
      f1.SetParameter(1,0) ;
      f1.SetParameter(2,tmph1->GetRMS()) ;
      tmph1->Fit(&f1,"Q0I") ;
      tmph1->GetListOfFunctions()->Add(f1.Clone()) ;
      halfDChisqDX(ipar) = f1.GetParameter(1) * m_integrals[ipar-1] ;
      os << "fitresult: " << f1.GetParameter(1) << " weight: " << m_integrals[ipar-1] << " integrated mean: "
	 << m_halfdChisqdX(ipar) / m_integrals[ipar-1] << std::endl ;
    }
    return halfDChisqDX ;
  }
