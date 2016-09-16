/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************************
      MultiComponentStateModeCalculator.cxx  -  description
      -----------------------------------------------------
begin                : Thursday 6th July 2006
author               : atkinson , amorley
email                : amorley@cern.ch
description          : Implementation code for MultiComponentStateModeCalculator class
****************************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateModeCalculator.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
//#include "EventPrimitives/AmgMatrixPlugin.h"

#include "TrkParameters/TrackParameters.h"


#include <map>

Trk::MultiComponentStateModeCalculator::MultiComponentStateModeCalculator( const std::string& type, const std::string& name, const IInterface* parent )
  :
  AthAlgTool( type, name, parent ),
  m_outputlevel(1)
{

  declareInterface<IMultiComponentStateModeCalculator>(this);
  
}

Trk::MultiComponentStateModeCalculator::~MultiComponentStateModeCalculator()
{}

StatusCode Trk::MultiComponentStateModeCalculator::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  m_NumberOfCalls = 0;
  m_ConverganceFilures = 0;
  m_NoErrorMatrix = 0;
  m_MixtureSizeZero = 0;
  
  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::MultiComponentStateModeCalculator::finalize()
{

  msg(MSG::INFO) << "-----------------------------------------------"<< endmsg;
  msg(MSG::INFO) << "         GSF ModeCalulator Statistics          "<< endmsg;
  msg(MSG::INFO) << "-----------------------------------------------"<< endmsg;
  msg(MSG::INFO) << "Number of Calls    " << m_NumberOfCalls          << endmsg;
  msg(MSG::INFO) << "No Error Matrix    " << m_NoErrorMatrix          << endmsg;
  msg(MSG::INFO) << "MixtureSize = Zero " << m_MixtureSizeZero        << endmsg;
  msg(MSG::INFO) << "Failed to converge " << m_ConverganceFilures     << endmsg;
  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;
 
  return StatusCode::SUCCESS;

}

Amg::VectorX Trk::MultiComponentStateModeCalculator::calculateMode( const Trk::MultiComponentState& multiComponentState ) const
{
  ++m_NumberOfCalls;
  
  Amg::VectorX modes(10) ;
  modes.setZero();
  
  // Check to see if the multi-component state is measured
  if ( !multiComponentState.isMeasured() ){
    ATH_MSG_DEBUG( "Mixture has no error matricies... Exiting ");
    ++m_NoErrorMatrix;
    return modes;
  }

  fillMixture( multiComponentState );
  
  for (int i=0 ; i<5 ; i++){
    std::multimap< double, double, std::greater<double> > para_startMap;

    // Loop over the mixture and find the starting point
    std::vector<Mixture>::const_iterator component = m_mixture[i].begin();
 
    if (m_mixture[i].size() == 0) ++m_MixtureSizeZero;
 
    for( ; component != m_mixture[i].end() ; ++component )
      para_startMap.insert( std::pair<double, double>( pdf( component->mean, i), component->mean ) );
    
    double para_start = para_startMap.begin()->second;

    modes[i] =  findMode( para_start, i ) ;

    // Calculate the FWHM and return this back so that it can be used to correct the covariance matrix 
    if( para_start != modes[i] ){
      // mode calculation was successful now calulate FWHM
      double currentWidth =  width(i);
      modes[i+5] = -1; // Failure is flagged with a value less than 0;
      
      double pdfVal = pdf( modes[i], i );      
      double highX(0); 
      double lowX(0);      

      double upperbound =modes[i] + 1.5 * currentWidth;
      while(true){
        if( pdf( upperbound, i ) > pdfVal*0.5 ){
          upperbound += currentWidth;
        } else {
          break;
        }
      }

      ATH_MSG_VERBOSE ( "HighX  PDFval, high val, low val [ " << pdfVal << ", " << pdf( modes[i], i ) << ", " << pdf( upperbound, i ) << "]") ;    

      bool highXFound = findRoot( highX, modes[i], upperbound, pdfVal*0.5, i);
      
      double lowerbound =modes[i] - 1.5 * currentWidth;
      while(true){
        if( pdf( lowerbound, i ) > pdfVal*0.5 ){
          lowerbound -= currentWidth;
        } else {
          break;
        }
      }

      ATH_MSG_VERBOSE ( "LowX   PDFval, high val, low val [ " << pdfVal << ", " << pdf( lowerbound, i ) << ", " << pdf( modes[i], i ) << "]") ;    

      bool lowXFound  = findRoot( lowX , lowerbound, modes[i], pdfVal*0.5, i);
      
      if (highXFound && lowXFound ){
        double FWHM = highX - lowX;
        ATH_MSG_DEBUG ( "PDFval, high val, low val [ " << pdfVal << ", " << pdf( highX, i ) << ", " << pdf( lowX, i ) << "]") ;    
        
        if( FWHM <= 0 ) {
          ATH_MSG_DEBUG(i << " Width is neagtive? " << highX << " " << lowX << " " <<  modes[i] );
        } else {
          ATH_MSG_DEBUG(i << " Width is positive? " << highX << " " << lowX << " " <<  modes[i] );
          ATH_MSG_DEBUG("Old & New width " << currentWidth << "  " << FWHM/2.35  << " High side only " << 2*( highX -  modes[i])/2.355 );
          modes[i+5] = FWHM/2.35482;  //2 * sqrt( 2* log(2))
        }
      } else {
        ATH_MSG_DEBUG( i << " Failed to find 1/2 width "  );
        
      }
      //Ensure that phi is between -pi and pi
      if(i==2){
        if(  modes[i] > M_PI ){
          modes[i] -= 2 * M_PI;
        } else if (  modes[i] < -M_PI ){
          modes[i] += 2 * M_PI;
        }
      }
    }
    

    para_startMap.clear();
  }
  return modes;

}

void Trk::MultiComponentStateModeCalculator::fillMixture( const Trk::MultiComponentState& multiComponentState ) const
{
  
  for (int i=0; i<5; i++){
    m_mixture[i].clear();
  }
  
  // Loop over all the components in the multi-component state
  Trk::MultiComponentState::const_iterator component  = multiComponentState.begin();
  Trk::ParamDefs parameter[5]={Trk::d0,Trk::z0,Trk::phi,Trk::theta,Trk::qOverP};
  for( ; component != multiComponentState.end() ; ++component ){
   for (int i=0; i<5; ++i){  
    const Trk::TrackParameters* componentParameters = component->first;

    const AmgSymMatrix(5)* measuredCov = componentParameters->covariance();

    if ( !measuredCov ) return;
//Enums for Perigee //
//                           d0=0, z0=1, phi0=2, theta=3, qOverP=4,
    double weight = component->second;
    double mean   = componentParameters->parameters()[parameter[i]];    
    //FIXME ATLASRECTS-598 this fabs() should not be necessary... for some reason cov(qOverP,qOverP) can be negative
    double sigma  = sqrt(fabs((*measuredCov)(parameter[i],parameter[i])));

    
    //Ensure that we don't have any problems with the cyclical nature of phi
    //Use first state as reference point
    if(i==2){ //phi
      double deltaPhi = multiComponentState.begin()->first->parameters()[2] -mean;  
      if( deltaPhi > M_PI ){
        mean += 2 * M_PI;
      } else if ( deltaPhi < -M_PI ){
        mean -= 2 * M_PI;
      }
    }



    Mixture mixture(weight, mean, sigma );

    m_mixture[i].push_back( mixture );
   }
  }

  return;

}

double Trk::MultiComponentStateModeCalculator::findMode( double xStart, int i ) const
{

  int iteration(0);

  double tolerance(1.);

  double mode( xStart );

    while( iteration < 20 && tolerance > 1.e-8 ){

      double previousMode(mode);
      
      if (d2pdf( previousMode, i) != 0.0) {
	      mode = previousMode - d1pdf( previousMode, i ) / d2pdf( previousMode, i );
      }
      else {
        ATH_MSG_DEBUG( "Second derivative is zero ... Returning the original value" );
        return xStart;
      }

      if ( ( pdf(mode, i) + pdf(previousMode, i)) != 0.0 ) {
        tolerance = fabs( pdf(mode, i) - pdf(previousMode, i) ) / ( pdf(mode, i) + pdf(previousMode, i) );
      }
      else {
        ATH_MSG_DEBUG( "Dividing by zero ... Returning the original value" );
        return xStart;
      }

      ++iteration;

    }

    if ( iteration >= 20 ){
      ATH_MSG_DEBUG( "Could not converge to the mode within allowed iterations... Returning the original value" );
      ++m_ConverganceFilures;
      return xStart;
    }

    return mode;

}

double Trk::MultiComponentStateModeCalculator::findModeGlobal(double mean,int i) const
{
  
  double start(-1);
  double end(1);
  if (mean > 0.0) {
    start = mean/2;
    end = 3*mean/2;
  }
  else if (mean < 0.0) {
    start = 3*mean/2;
    end = mean/2;
  }

  double mode(0);
  double maximum(-1);
  double iterate(fabs(mean/1000));
 
  for (double counter(start); counter < end; counter+=iterate) {
    double value(pdf(counter,i));
    if (value > maximum) {
      maximum = value;
      mode = counter;
    }
  }
  return mode;
}

double Trk::MultiComponentStateModeCalculator::pdf( double x, int i ) const
{

  double pdf(0.);

  std::vector< Mixture >::const_iterator component = m_mixture[i].begin();

  for ( ; component != m_mixture[i].end() ; ++component )
    pdf += component->weight * gaus( x, component->mean, component->sigma );

  return pdf;

}

double Trk::MultiComponentStateModeCalculator::d1pdf( double x, int i  ) const
{

  double result(0.);

  std::vector< Mixture >::const_iterator component = m_mixture[i].begin();

  for( ; component != m_mixture[i].end() ; ++component ){

    double z = ( x - component->mean ) / component->sigma;

    result += -1. * component->weight * z * gaus( x, component->mean, component->sigma ) / component->sigma;

  }

  return result;

}

double Trk::MultiComponentStateModeCalculator::d2pdf( double x, int i ) const
{


  double result(0.);

  std::vector< Mixture >::const_iterator component = m_mixture[i].begin();

  for( ; component != m_mixture[i].end() ; ++component ){

    double z = ( x - component->mean ) / component->sigma;

    result += component->weight / ( component->sigma * component->sigma ) * ( z * z - 1. ) * gaus( x, component->mean, component->sigma );

  }

  return result;

}

double Trk::MultiComponentStateModeCalculator::gaus( double x, double mean, double sigma ) const
{

  double normalisation = 1. / sqrt( 2. * M_PI );

  double z = ( x - mean ) / sigma;

  double result = normalisation / sigma * exp( -0.5 * z * z );
  //double result = exp( -0.5 * z * z);

  return result;

}


double Trk::MultiComponentStateModeCalculator::width( int i ) const
{

  double pdf(0.);

  std::vector< Mixture >::const_iterator component = m_mixture[i].begin();

  for ( ; component != m_mixture[i].end() ; ++component )
    pdf += component->weight * component->sigma;

  return pdf;

}



double Trk::MultiComponentStateModeCalculator::findRoot(double &result, double xlo, double xhi, double value, double i) const
{
 // Do the root finding using the Brent-Decker method. Returns a boolean status and
 // loads 'result' with our best guess at the root if true.
 // Prints a warning if the initial interval does not bracket a single
 // root or if the root is not found after a fixed number of iterations.

 
  double a(xlo),b(xhi);
  double fa= pdf(a, i) - value;
  double fb= pdf(b, i) - value;
  
  if(fb*fa > 0) {
    ATH_MSG_DEBUG( "BrentRootFinder::findRoot: initial interval does not bracket a root: ("
				                                        << a << "," << b << "), value = " << value << " f[xlo] = " << fa << " f[xhi] = " << fb );
    return false;
  }

  bool ac_equal(false);
  double fc= fb;
  double c(0),d(0),e(0);
  int MaxIterations = 20;
  double tolerance  =  1.e-6;
  
  for(int iter= 0; iter <= MaxIterations; iter++) {

    if ((fb < 0 && fc < 0) || (fb > 0 && fc > 0)) {
      // Rename a,b,c and adjust bounding interval d
      ac_equal = true;
      c = a;
      fc = fa;
      d = b - a;
      e = b - a;
    }
  
    if (fabs (fc) < fabs (fb)) {
      ac_equal = true;
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }

    double tol = 0.5 * tolerance * fabs(b);
    double m = 0.5 * (c - b);


    if (fb == 0 || fabs(m) <= tol) {
      ATH_MSG_DEBUG( "BrentRootFinder: iter = " << iter << " m = " << m << " tol = " << tol );
      ATH_MSG_DEBUG( "BrentRootFinder: xlo = " << xlo << " <  " << b << " <  " << xhi );
      result= b;
      return true;
    }
  
    if (fabs (e) < tol || fabs (fa) <= fabs (fb)) {
      // Bounds decreasing too slowly: use bisection
      d = m;
      e = m;
    }
    else {
      // Attempt inverse cubic interpolation
      double p, q, r;
      double s = fb / fa;
      
      if (ac_equal) {
        p = 2 * m * s;
        q = 1 - s;
      }
      else {
        q = fa / fc;
        r = fb / fc;
        p = s * (2 * m * q * (q - r) - (b - a) * (r - 1));
        q = (q - 1) * (r - 1) * (s - 1);
      }
      // Check whether we are in bounds
      if (p > 0) {
        q = -q;
      }
      else {
        p = -p;
      }
      
      double min1= 3 * m * q - fabs (tol * q);
      double min2= fabs (e * q);
      if (2 * p < (min1 < min2 ? min1 : min2)) {
        // Accept the interpolation
        e = d;
        d = p / q;
      }
      else {
        // Interpolation failed: use bisection.
        d = m;
        e = m;
      }
    }
    // Move last best guess to a
    a = b;
    fa = fb;
    // Evaluate new trial root
    if (fabs (d) > tol) {
      b += d;
    }
    else {
      b += (m > 0 ? +tol : -tol);
    }
    fb= pdf(b, i) - value;

  }
  // Return our best guess if we run out of iterations
  ATH_MSG_DEBUG( "BrentRootFinder::findRoot: maximum iterations exceeded." );
  result= b;

  return false;
}
