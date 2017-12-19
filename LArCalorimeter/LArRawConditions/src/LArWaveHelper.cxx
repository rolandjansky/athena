/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArWave.h"
#include <vector>
#include <iostream>

LArWave LArWaveHelper::translate(const LArWave& theWave, int nShift, 
				 double baseline ) const
{
  unsigned int nsamples = theWave.getSize() ;
  std::vector<double> trWave ;
  trWave.resize( nsamples+nShift ) ;
  for ( unsigned int i=0 ; i<theWave.getSize() ; i++ ) {
    if ( (int)i + nShift >= 0 )   trWave[i+nShift] = theWave.getSample(i) ;
  }
  if ( nShift > 0 ) {
    for ( unsigned int i=0 ; i<(unsigned int)nShift ; i++ ) {
      trWave[i] = baseline ;
    }
  }
  return LArWave( trWave , theWave.getDt() ) ;
}

LArWave LArWaveHelper::Dtranslate(const LArWave& theWave, double tShift, 
				 double baseline ) const 
{
  unsigned int nsamples = theWave.getSize() ;
  std::vector<double> trWave ;
  trWave.resize( nsamples );
  int ibin = 0;
  const double Dt = theWave.getDt();
  const double inv_Dt = 1. / Dt;
  if(fabs(tShift) > Dt) { // move first a number of bins
    ibin = int(tShift * inv_Dt);
    for ( unsigned int i=0 ; i<theWave.getSize() ; ++i ) {
        if ( (int)i + ibin >= 0 && (int)i + ibin < (int)nsamples)   trWave[i+ibin] = theWave.getSample(i) ;
    }
    if(ibin > 0) {
      for ( unsigned int i=0 ; i<(unsigned int)ibin ; ++i ) {
        trWave[i] = baseline ;
      }   
    } else {
       for ( unsigned int i=nsamples+ibin; i<nsamples; ++i) {
          trWave[i] = baseline;
       }
    }
  } else {
    for ( unsigned int i=0 ; i<theWave.getSize() ; ++i ) trWave[i] = theWave.getSample(i);
  }

  // now move trWave for part of bin
  std::vector<double> tranWave ;
  tranWave.resize(trWave.size());
  double tmpshift = tShift - ibin * Dt;
  if(tmpshift > 0.) {
     tranWave[0] = baseline;
     for(unsigned int i=1; i<trWave.size(); ++i) {
        tranWave[i] = trWave[i] - tmpshift * (trWave[i] - trWave[i-1]) * inv_Dt;
     }
  } else {
     tranWave[trWave.size() - 1] = baseline;
     for(int i=trWave.size()-2; i>=0; --i) {
        tranWave[i] = trWave[i] - tmpshift * (trWave[i] - trWave[i+1]) * inv_Dt;
     }
  }

  return LArWave( tranWave , Dt ) ;
}

unsigned int LArWaveHelper::getMin(const LArWave& theWave) const
{
  const unsigned int N = theWave.getSize() ;
  unsigned int imin = N + 1 ;
  double amin = +9.999E+99 , asample ;
  for ( unsigned int i=1 ; i<N ; i++ ) {
    if ( (asample=theWave.getSample(i)) < amin ) {
      imin = i ;
      amin = asample ;
    }
  }
  return imin ;
}


unsigned int LArWaveHelper::getMax(const LArWave& theWave) const
{
  const unsigned int N = theWave.getSize() ;
  unsigned int imax = N + 1 ;
  double amax = -9.999E+99 , asample ;
  for ( unsigned int i=1 ; i<N ; i++ ) {
    if ( (asample=theWave.getSample(i)) > amax ) {
      imax = i ;
      amax = asample ;
    }
  }
  return imax ;
}

double LArWaveHelper::getDMax(const LArWave& theWave, double &tMax) const
{
//  const unsigned int N = theWave.getSize() ;
  unsigned int imax = getMax(theWave) ;
  double amax = -9.999E+99 ;
  std::vector<double> pfit = polyfit(theWave, imax - 4, imax + 4, 2);
  if(pfit.size() < 3 || pfit[2] == 0.) { tMax = -9.999E+99; return amax; }
  tMax = - pfit[1] / (2 * pfit[2]);
  amax = pfit[0];
  return amax ;
}

// double LArWaveHelper::getMaxAmp(const LArWave& theWave) 
// {
//   const unsigned int N = theWave.getSize() ;
//   const unsigned Nbase =5;
//   double amax = -9.999E+99 , asample ;
//   double base = getBaseline(theWave,Nbase);
//   for ( unsigned int i=1 ; i<N ; i++ ) {
//     asample=theWave.getSample(i);
//        if((asample-base)> amax )amax = asample-base ;
//   }
//   return amax;
// }


double LArWaveHelper::getMaxAmp(const LArWave& theWave) const
{
  const unsigned int N = theWave.getSize() ;
  double amax = -9.999E+99;
  for ( unsigned int i=1; i<N ; ++i ) {
    double asample = theWave.getSample(i);
    if ( asample>amax ) 
      amax = asample;
  }
  return amax;
}

double LArWaveHelper::getArea(const LArWave& theWave) const
{
  const unsigned int N=theWave.getSize();
  double surf=0.;
  const unsigned Nbase=5;
  double amax=-9.999E+99;
  double base = getBaseline(theWave,Nbase);
  for (unsigned int i=1; i<N ; i++) {
    const double asample=theWave.getSample(i);
    if((asample-base)>amax)amax = asample-base;
  } 

  const double inv_amax = 1. / amax;
  for (unsigned int i=0; i<N ; i++){
    const double asample=theWave.getSample(i)-base;  
    if(asample>0)surf+=asample*inv_amax;
  }
  
  return surf;
}

double  LArWaveHelper::getT0(const LArWave& theWave) const
{
  const unsigned Nbase=5;
  double rT0=0.;
  double asamp1=0.;
  double asamp2=0.;
  double base = getBaseline(theWave,Nbase);
  double amax = getMaxAmp(theWave);
  unsigned int imax=getMax(theWave);
  
  for (unsigned int i=1; i<imax ; i++) {
      asamp1=theWave.getSample(i);
      asamp2=theWave.getSample(i-1);
     if((asamp1-base)>amax*0.10 && (asamp2-base)<amax*0.10)
       rT0 = i-1+(0.10*amax-(asamp2-base))/(asamp1-asamp2);
  }
  

  return rT0;
}

double  LArWaveHelper::getT5(const LArWave& theWave) const
{
//  const unsigned Nbase=5;
  double rT5=0.;
//  double base = getBaseline(theWave,Nbase);
  double tmax;
  double amax = getDMax(theWave, tmax);
  unsigned int imax=getMax(theWave);
  
  for (unsigned int i=imax - 20; i>0 ; --i) {
     if(theWave.getSample(i) < 0.05*amax) {
        std::vector<double> pfit = polyfit(theWave, i-3, i+3, 3);
        for(double t=(i+2)*theWave.getDt(); t>=(i-2)*theWave.getDt(); t-= 0.05) {
           if(pfit[0]+pfit[1]*t+pfit[2]*t*t+pfit[3]*t*t*t < 0.05*amax) {
              rT5 = t+0.05;
              break;
           }
        }
        break;
     }
  }
  
  return rT5;
}


double  LArWaveHelper::getWidth(const LArWave& theWave) const
{
  const unsigned int N = theWave.getSize() ;
  unsigned int imax= 0;
  const unsigned Nbase=5;
  double amax=-9.999E+99;
  double base = getBaseline(theWave,Nbase);
  for (unsigned int i=1; i<N ; i++) {
      const double& asample=theWave.getSample(i);
      if((asample-base)>amax){
      imax = i;
      amax = asample-base ;
      }
 }

  double wleft=0.;
  double wright=0.;
  double width=0.;
  double asamp1=0.;
  double asamp2=0.;
  
  for (unsigned int i=1; i<imax; i++){
    asamp1=theWave.getSample(i);
    asamp2=theWave.getSample(i-1);    
    if((asamp1-base)>=amax/2. && (asamp2-base)<=amax/2.)
      { 
	wleft= i-1+(amax/2.-(asamp2-base))/(asamp1-asamp2);
	
       }
  }
  for (unsigned int i=imax; i<N; i++){
       asamp1=theWave.getSample(i);
       asamp2=theWave.getSample(i-1);
    if((asamp1-base)<=amax/2. && (asamp2-base)>=amax/2.) { 
	wright= i-1+((asamp2-base)-amax/2.)/(asamp2-asamp1);
	
  }
  }
   width=wright-wleft;

   return width;
}

LArWave LArWaveHelper::derive(const LArWave& theWave) const
{
  std::vector<double> wave = theWave.getWave() ;
  const double dt = theWave.getDt() ;
  unsigned int nsamples = theWave.getSize() ;
  std::vector<double> dwave ;
  dwave.resize(nsamples,0.0) ;
  if ( nsamples > 1 ) {
    const double inv_dt = 1. / dt;
    dwave[0] = ( wave[1] - wave[0] ) * inv_dt ;
    for ( unsigned int i=1 ; i<nsamples-1 ; i++ ) {
      dwave[i] = ( wave[i+1] - wave[i-1] ) * inv_dt * 0.5;
    }
    dwave[nsamples-1] = ( wave[nsamples-1] - wave[nsamples-2] ) * inv_dt ;
  }
  return LArWave(dwave,dt) ;
}

LArWave LArWaveHelper::derive_smooth(const LArWave& theWave) const
{
  /* ================================================================
   * 7 points first derivative 
   * using Savitsky-Golay smoothing filtering (m = polynomial order)
   * ================================================================ */      
  static double c[3][4][7] = 
    { { { -0.107, -0.071, -0.036,  0.000,  0.036,  0.071,  0.107 } ,  
	{  0.012, -0.071, -0.107, -0.095, -0.036,  0.071,  0.226 } ,  
	{  0.131, -0.071, -0.179, -0.190, -0.107,  0.071,  0.345 } ,   
	{  0.250, -0.071, -0.250, -0.286, -0.179,  0.071,  0.464 } } ,   // m = 2 
      { {  0.087, -0.266, -0.230,  0.000,  0.230,  0.266, -0.087 } ,
	{  0.123, -0.183, -0.218, -0.095,  0.075,  0.183,  0.115 } ,
	{ -0.008,  0.067, -0.040, -0.190, -0.246, -0.067,  0.484 } ,
	{ -0.306,  0.484,  0.306, -0.286, -0.734, -0.484,  1.020 } } ,   // m = 3
      { {  0.087, -0.266, -0.230,  0.000,  0.230,  0.266, -0.087 } ,
	{ -0.049,  0.219, -0.276, -0.439,  0.018,  0.584, -0.057 } ,
	{ -0.079,  0.234, -0.063, -0.333, -0.270,  0.099,  0.413 } ,
	{  0.269, -0.856,  0.497,  0.863, -0.543, -1.824,  1.594 } } } ; // m = 4 
  const int m=3 ; // choose between 2 and 4
  const int ipoly = m-2 ;
  //
  std::vector<double> wave = theWave.getWave() ;
  const double dt = theWave.getDt() ;
  const double inv_dt = 1. / dt;
  unsigned int nsamples = theWave.getSize() ;
  std::vector<double> dwave ;
  dwave.resize(nsamples,0.0) ;
  //
  for ( unsigned int i=0 ; i<nsamples ; i++ ) {   
    if ( i<=2 ) {                   // first 3 points
      int ifilt = 3 - i ;
      for ( unsigned int j=0 ; j<=6 ; j++ )
	dwave[i] += -c[ipoly][ifilt][6-j] * wave[j] ;
    } else if ( i>=nsamples-3 ) {   // last 3 points
      int ifilt = nsamples - i + 2;
      for ( unsigned int j=nsamples-7 ; j<=nsamples-1 ; j++ )
	dwave[i] +=  c[ipoly][ifilt][j-(nsamples-7)] * wave[j] ;
    } else {                        // intermediate points
      int ifilt = 0 ;
      for ( unsigned int j=i-3 ; j<=i+3 ; j++ ) 
	dwave[i] +=  c[ipoly][ifilt][j-(i-3)] * wave[j] ;
    }
    dwave[i] *= inv_dt ;
  }  
  return LArWave( dwave , dt ) ;
}

LArWave LArWaveHelper::autocorr(const LArWave& theWave) const
{
  std::vector<double> wave = theWave.getWave() ;
  double dt = theWave.getDt() ;
  unsigned int nsamples = theWave.getSize() ;
  std::vector<double> ac ;
  ac.resize(nsamples,0.0) ;
  for ( unsigned int i=0 ; i<nsamples ; i++ ) {
    for ( unsigned int k=0 ; k<nsamples-i ; k++ ) {
      ac[i] += wave[i]*wave[i+k] ;
    }
    ac[i] *= dt ;
  }
  return LArWave( ac , dt ) ;
}

LArWave LArWaveHelper::subSample(const LArWave& theWave,unsigned Nfirst,unsigned deltaN)  const
{
  unsigned length = theWave.getSize() ;
  if ( length <= 0 ) return LArWave() ;
  unsigned Nsamp = (length-Nfirst + (deltaN-1)) / deltaN ;
  LArWave w( Nsamp , deltaN*theWave.getDt() ) ;
  for (unsigned i=0;i<Nsamp;i++) {
    unsigned j = Nfirst + i * deltaN ;
    w[i]  = theWave.getSample(j) ;
  }
  return w ;
}

double LArWaveHelper::getBaseline(const LArWave& theWave,unsigned nBase)  const
{
  if (nBase == 0) return 0;
  double base = 0.0 ;
  for ( unsigned i=0 ; i<nBase ; i++ ) base += theWave.getSample(i) ;
  return base/nBase ;
}

double LArWaveHelper::getSumTail(const LArWave& theWave,unsigned iFirst) const
{
  return getSumRegion(theWave,iFirst,theWave.getSize());
}
//   double sum = 0.0 ;
//   //W.L., 2-Sept-09: Speed-up
//   const size_t s=theWave.getSize();
//   for (size_t i=iFirst;i<s;++i) 
//     sum += theWave.getSample(i); 
  


//   return sum ;
// }

double LArWaveHelper::getSumRegion(const LArWave& theWave, const unsigned iFirst, unsigned iLast) const {

  if (iLast>theWave.getSize()) 
    iLast=theWave.getSize();
  if (iFirst >= iLast) return 0.0;

  double sum = 0.0;
  std::vector<double>::const_iterator it=theWave.getWave().begin()+iFirst;
  std::vector<double>::const_iterator it_e=theWave.getWave().begin()+iLast;
  for(;it!=it_e;++it) {
    sum+=(*it);
  }

  return sum;
}


double LArWaveHelper::getSumSquareTail(const LArWave& theWave,unsigned iFirst) const 
{
  return getSumSquareRegion(theWave,iFirst,theWave.getSize());
}

double LArWaveHelper::getSumSquareRegion(const LArWave& theWave, unsigned iFirst, unsigned iLast) const  
{
  if (iLast>theWave.getSize()) 
    iLast=theWave.getSize();
  if (iFirst >= iLast) return 0.0;

  double sum = 0.0;
  std::vector<double>::const_iterator it=theWave.getWave().begin()+iFirst;
  std::vector<double>::const_iterator it_e=theWave.getWave().begin()+iLast;
  for(;it!=it_e;++it) {
    sum+=((*it)*(*it));
  }

  return sum;
}


unsigned int LArWaveHelper::getStart(const LArWave& theWave) const
{
  const unsigned Nbase = 5 ;
  const double   Threshold = 0.001 ;
  double base =  getBaseline(theWave,Nbase) ;
  unsigned iMax = getMax(theWave) ;
  double thr = base + Threshold * ( theWave.getSample( iMax ) - base ) ;
  for ( unsigned i=iMax-1 ; i>0 ; i-- ) 
    if (theWave.getSample(i)<thr)  return i-1 ;
  return 0 ;
}

unsigned int LArWaveHelper::getZeroCross(const LArWave& theWave) const
{
  for ( unsigned i=getMax(theWave)+1 ; i<theWave.getSize() ; i++) 
    if (theWave.getSample(i)<0)  return i ;
  return 0 ;
}


double LArWaveHelper::getJitter(const LArWaveCumul& theWave) const
{
  // Calculate the squared error of the CaliWave
  std::vector<double> errorsSquared(theWave.getErrors());
  for (unsigned int iError = 0; iError < errorsSquared.size(); ++iError) {
    if (errorsSquared[iError] == 0.0) 
      return 0.; // no error in the Wave, cannot compute jitter
    errorsSquared[iError] = errorsSquared[iError] * errorsSquared[iError];
  }
  // Get the wave derivative
  LArWave derivedWave = derive(theWave);
  LArWave derivativeSquared = derivedWave * derivedWave; 
  // Fit jitter
  double rho = 0.;
  std::vector<double> theLinearFit = linfit(derivativeSquared.getWave(),errorsSquared,rho);
  // Output
  if ( theLinearFit[1] > 0. && theLinearFit[1]==theLinearFit[1] ) // skip negative value and (hopefully) NAN
    return sqrt(theLinearFit[1]);
  else
    return 0.;
}


LArWaveDerivedQuantitiesP LArWaveHelper::getDerivedQuantities(const LArWaveCumul& theWave) const
{  
  float baseline = getBaseline(theWave,10);
  float maxAmp   = getMaxAmp(theWave);
  // please note the 'dt' normalization in the following!
  float dt       = theWave.getDt();
  float tmaxAmp  = dt * getMax(theWave);
  float width    = dt * getWidth(theWave);
  float rT0      = dt * getT0(theWave);
  float posLobe  = dt * getSumRegion(theWave,0,getZeroCross(theWave))/maxAmp;  
  float jitter   = getJitter(theWave);
  return LArWaveDerivedQuantitiesP(baseline, maxAmp, tmaxAmp, width, rT0, posLobe, jitter, theWave.getFlag() );
}


std::vector<double> LArWaveHelper::linfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
				     double& rho) const
{
  std::vector<double> X,Y ;
  get_fit_vectors(theWave,iFirst,iLast,X,Y) ;
  return linfit(X,Y,rho) ;
}

std::vector<double> LArWaveHelper::expfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
				     double& rho) const
{
  std::vector<double> X,Y ;
  get_fit_vectors(theWave,iFirst,iLast,X,Y) ;
  return expfit(X,Y,rho) ;
}

std::vector<double> LArWaveHelper::polyfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
				      unsigned Ndeg) const
{
  std::vector<double> X,Y ;
  get_fit_vectors(theWave,iFirst,iLast,X,Y) ;
  return polyfit(X,Y,Ndeg) ;
}



std::vector<LArWave>
LArWaveHelper::linearMasterWave(const std::vector<const LArWave*>& vWaves,
                                const std::vector<double> vAmpli) const
{
  std::vector<LArWave> MWandDAC0 ;
  unsigned nWaves = vWaves.size() ;
  if ( nWaves != vAmpli.size() ) {
    std::cout << "  linearMasterWave ERROR: wrong number of amplitudes!" << std::endl ;
    return MWandDAC0 ;
  }
  double    dt  = vWaves[0]->getDt() ;
  unsigned  len = vWaves[0]->getSize() ;
  for ( unsigned k=1 ; k<nWaves ; k++ ) { 
    if ( vWaves[k]->getDt() != dt ) {
      std::cout << "  linearMasterWave ERROR: waves don't have same dt" << std::endl ;
      return MWandDAC0 ;
    }
    if ( vWaves[k]->getSize() != len ) {
      std::cout << "  linearMasterWave ERROR: waves don't have same length" << std::endl ;
      return MWandDAC0 ;
    }
  }
  LArWave mw(len,dt) ;
  LArWave dac0(len,dt) ;
  for ( unsigned i=0 ; i<len ; i++ ) {
    std::vector<double> ramp ;
    for ( unsigned k=0 ; k<nWaves ; k++ ) { 
      ramp.push_back(vWaves[k]->getSample(i)) ;
    }
    double rho ;
    std::vector<double> par = linfit(vAmpli,ramp,rho) ;
    if (par.size() != 2) {
      std::cout << "  linearMasterWave ERROR: linear fit failed" << std::endl ;
      return MWandDAC0 ;
    }
    mw[i]   = par[1] ;
    dac0[i] = par[0] ;
  }
  MWandDAC0.push_back(dac0) ;
  MWandDAC0.push_back(mw) ;
  return MWandDAC0 ;
}


//
//  private member functions
//

std::vector<double> LArWaveHelper::linfit(const std::vector<double>& X,const std::vector<double>& Y,
					  double& rho) const
{
  std::vector<double> C ;
  C.resize(0) ;
  rho = 0 ;
  unsigned N = X.size() ;
  if ( N<2 || Y.size()!=N ) {
    //    fprintf(stderr,"ERROR: lin. fit on less than 2 points\n") ;
  } else {
    double Sx=0. , Sy=0. , Sx2=0. , Sxy=0. , S=0. , Sy2=0. ;
    for (unsigned i=0;i<N;i++) {
      S   += 1 ;
      Sx  += X[i] ;
      Sy  += Y[i] ;
      Sx2 += X[i] * X[i] ;
      Sxy += X[i] * Y[i] ;
      Sy2 += Y[i] * Y[i] ;
    }
    C.resize(2) ;
    C[1] =   ( S*Sxy - Sx*Sy ) / ( S*Sx2 - Sx*Sx ) ;
    C[0] = ( Sy*Sx2 - Sxy*Sx ) / ( S*Sx2 - Sx*Sx ) ;
    if ( C[1] != C[1] || C[0] != C[0] ) { // protection agains C[i] being nan
      C.resize(0) ;
      return C ;
    }
    rho = ( S*Sxy - Sx*Sy ) / sqrt( (S*Sx2-Sx*Sx) * (S*Sy2-Sy*Sy) ) ;
  }
  return C ;
}

std::vector<double> LArWaveHelper::expfit(const std::vector<double>& X,const std::vector<double>& Y,
					  double& rho) const
{
  std::vector<double> C ;
  C.resize(0) ;
  rho = 0 ;
  unsigned N = X.size() ;
  if ( N>=2 && Y.size()==N ) {
    std::vector<double> logY ;
    logY.resize(N) ;
    for ( unsigned i=0;i<N;i++ ) {
      if ( Y[i]*Y[0] <= 0.0 ) {
	fprintf(stderr,"ERROR: exp. fit of zero-crossing wave\n") ;
	return C ;
      }
      logY[i]  = log(fabs(Y[i])) ;
    }
    C = linfit(X,logY,rho) ;
    C[0] = exp(C[0]) ;
    if ( Y[0] < 0.0 ) C[0] = - C[0] ;
  }
  return C ;
}

std::vector<double> LArWaveHelper::polyfit(const std::vector<double>& X, 
					   const std::vector<double>& Y,
					   unsigned Ndeg) const
{
    // to be implemented - return to avoid compiler warning RDS
    static std::vector<double> dummy;
    if(Ndeg > 2 || (Y.size() == 0) || (X.size() != Y.size())) return (dummy);
    switch (Ndeg) {
       case 0: { dummy.resize(1);
                 double sum = 0.;
                 for(unsigned int i=0; i<Y.size(); ++i) sum += Y[i];
                 dummy[0] = sum / Y.size();
                 return (dummy); }
       case 1: { double rho; return linfit(X,Y,rho);}
       case 2: { // fill the matrix XT*X
                  double a11,a12,a13,a21,a22,a23,a31,a32,a33;
                  a11=1.;
                  double sum =0., sum2=0., sum3=0., sum4=0.;
                  for(unsigned int i=0; i<X.size(); ++i) {
                     sum += X[i];
                     sum2 += X[i]*X[i];
                     sum3 += X[i]*X[i]*X[i];
                     sum4 += X[i]*X[i]*X[i]*X[i];
                  }
                  a12 = a21 = sum;
                  a13 = a22 = a31 = sum2;
                  a23 = a32 = sum3;
                  a33 = sum4;
                  // Inverse
                  double ai11,ai12,ai13,ai21,ai22,ai23,ai31,ai32,ai33;
                  const double det =
                    a11*(a33*a22-a32*a23) - a21*(a33*a12-a32*a13) + a31*(a23*a12-a22*a13);
                  if(fabs(det) < 1.e-6) return (dummy);  // Not invertable
                  ai11 = (a33*a22 - a32*a23);
                  ai12 = ai21 = -(a33*a12-a32*a13);
                  ai22 = a33*a11 - a31*a13;
                  ai13 = ai31 = a23*a12 - a22*a13;
                  ai23 = ai32 = -(a23*a11-a21*a13);
                  ai33 = a22*a11-a21*a12;
                  sum = 0.; sum2=0.; sum3=0.;
                  for(unsigned int i=0; i<X.size(); ++i) {
                     sum += Y[i];
                     sum2 += X[i]*Y[i];
                     sum3 += X[i]*X[i]*Y[i];
                  }
                  dummy.resize(3);
                  const double inv_det = 1. / det;
                  dummy[0] = (ai11*sum + ai12*sum2 + ai13*sum3) * inv_det;
                  dummy[1] = (ai21*sum + ai22*sum2 + ai23*sum3) * inv_det;
                  dummy[2] = (ai31*sum + ai32*sum2 + ai33*sum3) * inv_det;

                  return (dummy);
               }
       default : { // will not happen
                    return (dummy);
                 }
    }
}

unsigned LArWaveHelper::get_fit_vectors(const LArWave theWave,unsigned iFirst,unsigned iLast,
					std::vector<double>& X,std::vector<double>& Y) const
{
  unsigned length = theWave.getSize() ;
  if ( length == 0 ) {
    X.resize(0) ;
    Y.resize(0) ;
    return 0 ;
  } else {
    if ( iLast >= length )  iLast = length - 1 ;
    if ( iFirst > iLast )   iFirst = iLast ;
    unsigned vlength = iLast - iFirst + 1 ;
    X.resize(vlength) ;
    Y.resize(vlength) ;
    double dt = theWave.getDt() ;
    for ( unsigned i=iFirst ; i<=iLast ; i++ ) {
      unsigned k = i - iFirst ;
      X[k] = i * dt ;
      Y[k] = theWave.getSample(i) ;
    }
    return vlength ;
  }
}

