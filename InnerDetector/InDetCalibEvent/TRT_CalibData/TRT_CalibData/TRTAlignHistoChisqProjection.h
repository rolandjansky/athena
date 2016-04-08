/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRTALIGNHISTOCHISQPROJECTION_HH__
#define __TRTALIGNHISTOCHISQPROJECTION_HH__

// this class keeps track of chisquare derivatives. it makes
// histograms of the projections to the derivatives, which is kind of
// useful, sometimes.

#include <vector>
#include <iostream>
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

class TDirectory ;
class TH2 ;
class HepMatrix ;

namespace TRTAlign
{
  
  class HistoChisqProjection
  {
  public:
    HistoChisqProjection(const char* name, int dim, int nbins, float xmin, float xmax) ;
    ~HistoChisqProjection() ;
    
    // use this if the weight matrix for first and second derivative are equal
    void add( const CLHEP::HepMatrix& deriv, const CLHEP::HepSymMatrix& weight,  
    	      const CLHEP::HepSymMatrix& variance, const CLHEP::HepVector& residual ) {
      addfirst( deriv, weight, variance, residual ) ;
      addsecond( deriv, weight ) ;
    }
    
    // use this if they are not (like in global chisquare method)
    void add( const CLHEP::HepMatrix& deriv, const CLHEP::HepSymMatrix& weight, 
    	      const CLHEP::HepSymMatrix& variance, const CLHEP::HepVector& residual, 
	      const CLHEP::HepSymMatrix& secondweight) {
      addfirst( deriv, weight, variance, residual ) ;
      addsecond( deriv, secondweight ) ;
    }
    
    CLHEP::HepVector getFittedHalfDChisqDX(std::ostream& os=std::cout) const ;
    CLHEP::HepVector getNormalHalfDChisqDX() const { return m_halfdChisqdX ; } 
    
    // set the TDirectory where the histograms are kept
    void setDirectory(TDirectory* dir)  ;
    
  private:
    void addfirst( const CLHEP::HepMatrix& derivative, const CLHEP::HepSymMatrix& weight,
		   const CLHEP::HepSymMatrix& variance, const CLHEP::HepVector& residual ) ;
    void addsecond( const CLHEP::HepMatrix& derivative, const CLHEP::HepSymMatrix& weight) ;

  private:
    int m_dim ;
    TH2* m_h2 ;
    TH2* m_unweightedh2 ;
    std::vector<double> m_integrals ;
    CLHEP::HepVector m_halfdChisqdX ;
    CLHEP::HepSymMatrix m_halfd2ChisqdX2 ;
  } ;

}

#endif
