/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigInDetSctKFitter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTrackFitter/TrigInDetSctKFitter.h"

#include <cstring>
using std::memset;

TrigInDetSctKFitter::TrigInDetSctKFitter(const std::string& t, 
					 const std::string& n,
					 const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigInDetTrackFitter >( this );
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremmCorr=false);
  declareProperty( "Chi2Cut", m_DChi2 = 200.0);
  // set parameters
  paramset = 0;
  memset( minTrackRadius, 0, sizeof(minTrackRadius) );
  memset( maxDChi2, 0, sizeof(maxDChi2) );
  
  maxDChi2[0] = m_DChi2;
  maxDChi2[1] = m_DChi2;
}

StatusCode TrigInDetSctKFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO("SctKFitter constructed ");
  return sc;
}

StatusCode TrigInDetSctKFitter::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigInDetSctKFitter::~TrigInDetSctKFitter()
{

}

void TrigInDetSctKFitter::fit(TrigInDetTrackCollection* recoTracks)
{

  long ier, nHits, ihtype[73], ihdrop[73];
  double wm1[15],cov1[15],par_kf[5],par_official[5],x0=0.,y0=0.,rp,cov2[15];
  double rco[73],fco[73],zco[73],errr[73],errxy[73],errz[73],chi2;
  bool electron=m_doBremmCorr;
  
  const double consb = 0.006;

// to convert GeV to MeV and cm to mm
  const double GeVToMeVConv = 1000.;
  const double mmTocmConv = 0.1;
  const double cmTommConv = 10.;

  double Pt, Phi0, Eta, Z0, D0, errD0, errZ0, errEta, errPhi0, errPt;


  TrigInDetTrackCollection::iterator trIt = recoTracks->begin();
  TrigInDetTrackCollection::iterator lastIt = recoTracks->end();
  for(; trIt !=lastIt; trIt++) 
    {
      if((*trIt)->siSpacePoints()->size()==0) 
	{
    ATH_MSG_ERROR("SctKFitter Failed -- TrigInDetTrack has no hits"); 
	  continue;
	}
      std::vector<const TrigSiSpacePoint*>::iterator pSPIt=(*trIt)->siSpacePoints()->begin();
      std::vector<const TrigSiSpacePoint*>::iterator lastSPIt=(*trIt)->siSpacePoints()->end();
      int i=0;
      for(; pSPIt != lastSPIt; pSPIt++) 
	{
	  const TrigSiSpacePoint* pSP=(*pSPIt);

	  // to convert GeV to MeV and cm to mm
	  rco[i] = pSP->r()*mmTocmConv;
	  fco[i] = pSP->phi(); 
	  zco[i] = pSP->z()*mmTocmConv;
      
	  errr[i] = pSP->dr()*mmTocmConv;
	  errxy[i]= pSP->dphi() * pSP->r()*mmTocmConv; //this converts dPhi into dXY
	  errz[i] = pSP->dz()*mmTocmConv;

	  if( errr[i] > errz[i] ) 
	    {
	      ihtype[i]=1;                  //  endcap
	    }
	  else 
	    {
	      ihtype[i]=0;                  //  barrel
	    }
	  i++;
	}
      nHits=i;
      tkffit(&nHits,&rco[0],&fco[0],&zco[0],&errxy[0],&errz[0],
	     &errr[0],&ihtype[0],&rp,&par_kf[0],&wm1[0],&chi2,
	     &ihdrop[0],&ier,electron);
    
      if ( ier ) {
				ATH_MSG_ERROR("SctKFitter: error in tkffit -> " << ier); 
				continue;
      }
      long dropped = 0;
      for (long j=0; j<nHits; j++) {
	dropped += ihdrop[j];
      }
        
      //convert weight matrix to cov matrix
      matrix_inverter(&wm1[0],&cov1[0],&ier);
      if( ier ) {
				ATH_MSG_ERROR(" SctKFitter: error in matrix conversion -> " << ier); 
	continue;
      }
      cyl_prg_convertor(&par_kf[0], &cov1[0], &rp, &x0, &y0, 
			&par_official[0], &cov2[0], &ier);
    
      // to convert GeV to MeV and cm to mm
      Pt = -consb/par_official[4]*GeVToMeVConv;   // put the right sign on Pt
      Phi0 = par_official[3];
      Eta = -log(tan(par_official[2]/2.));
      Z0 = par_official[1]*cmTommConv;
      D0 = par_official[0]*cmTommConv;
    
      errD0 = sqrt(cov2[0])*cmTommConv;
      errZ0 = sqrt(cov2[2])*cmTommConv;
      errEta = sqrt(cov2[5])/fabs(sin(par_official[2]));
      errPhi0 = sqrt(cov2[9]);
      errPt = consb*sqrt(cov2[14])/(par_official[4]*par_official[4])*GeVToMeVConv;
			ATH_MSG_DEBUG("Perigee parameters: d0="<<D0<<" phi0="<<Phi0<<" z0="<<Z0
				<<" eta0="<<Eta<<" pt="<<Pt);

      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
								    errD0,errPhi0,errZ0,errEta,errPt,NULL);

      if(param!=NULL) delete param;
      (*trIt)->param(tidtfp);
      (*trIt)->chi2(chi2);
      (*trIt)->siSpacePoints()->clear();

      std::vector<const TrigSiSpacePoint*> spacePointsOnTrack;
     	
      std::vector<const TrigSiSpacePoint*>::iterator  hitItr = (*trIt)->siSpacePoints()->begin();

      for( long i = 0; i < nHits; ++i, ++hitItr )
	if( ! ihdrop[i] )   
	  (*trIt)->siSpacePoints()->push_back((*hitItr));
    }
}

void TrigInDetSctKFitter::cyl_prg_convertor(double *pari, double *covi, double *r__, 
				       double *xb, double *yb, 
				       double *par, double *cov, long *ierr)
{
    double der11, der14, der15, der23, der45, cosf, sinf, curv;
    long i__, j;
    double rdphi, rsinb, cotth;
    double x0, y0;
    double tem, psi;

/* Function   : Compute the "perigee" parameters (at the point of the closest*/
/*          approach to the beam axis or to a reference position near a*/
/*          secondary vertex) from the cylindrical-polar parameters */
/*          of reconstructed tracks, and their covariance and weight */
/*          matrices. */
/*            For error propagation, an approximation is made, i.e. assuming*/
/*          that the r coordinate of reference point is small w.r.t. the*/
/*          radius of curvature. This is always true if the tracks are */
/*          extrapolated to the positions where are close to the beam (or the*/
/*          reference position near a secondary vertex) before calling this*/
/*          routine. */
/*            No account is taken for multiple scattering (m/s) in this*/
/*          routine, so that, for the primary vertex, all tracks should be*/
/*          extrapolated into the inside of the beam pipe before calling this*/
/*          routine; i.e. the m/s should be taken care in the extrapolation.*/

/* Arguments   : Input  : PARI(1:5)  :  track parameters in cylindrical-polar*/
/*                                      coordinates, PARI(1) -- phi (rad.)*/
/*                                                   PARI(2) -- z (cm)*/
/*                                                   PARI(3) -- zeta (rad.)*/
/*                                                   PARI(4) -- beta (rad.)*/
/*                                                   PARI(5) -- 1/Rtr(1/cm)*/
/*                        COVI(1:15) :  covariance matrix of PARI. */
/*                                the numbering convention is  1 */
/*                                                             2  3 */
/*                                                             4  5  6 */
/*                                                             7  8  9  10*/
/*                                                             11 12 13 14 15*/
/*                        R        :  the radius of the reference point where*/
/*                                    above parameters are defined (in cm).*/
/*                        XB,YB    :  coordinates of the "beam". */

/*                Output : PAR(1:5)  :  "perigee" parameters, */
/*                      1 : epsilon (impact par. in xy projection, with sign)*/
/*                      2 : z coordinate */
/*                      3 : theta */
/*                      4 : phi angle */
/*                      5 : 1/Rtr (Rtr = radius of curvature, with sign)*/
/*                         COV(1:15) :  covariance matrix of PAR. */
/*                         WGT(1:15) :  weight matrix of PAR. */

/*  Errors      :  IERR = 0 :  no error; */
/*                        1 :  incorrect data in inputs PARI, COVI or R; */
/*                        2 :  error in converting COV (WGT not computed). */

/*  Authers     :  P. BILLOIR,  S. QIAN  July 1990 */

/*  Modified by :  S. QIAN for CMS       March 1994 */
/*              (1) the output parameter "theta" is changed to "ctg(theta)";*/
/*              (2) the appoximate perigee formula is replaced by a precise one.*/

/**************************************************************************/

/* Internal variables */

/*      DATA CONSB /.0053964/ */

/*If magnetic field is not 4 Tesla, this CONSB has to be modified, or to be*/
/*   calculated by reading the field map. */

    /* Parameter adjustments */
    //    --wgt;
    --cov;
    --par;
    --covi;
    --pari;

    /* Function Body */
/* ________________________ */

    *ierr = 0;
    for (i__=1; i__<6; i__++) par[i__]=0.;

/*   exit if incorrect data */

    if (*r__ == 0. || pari[3] == 0. || covi[1] < 0. || 
	covi[3] < 0. || covi[6] < 0. || covi[10] < 0. || covi[15] < 0.) {
	*ierr = 1;
	return;
    }

/*   computation of "perigee" parameters */

    x0 = *r__ * cos(pari[1]) - *xb;
    y0 = *r__ * sin(pari[1]) - *yb;
    curv = pari[5];
    psi = pari[1] + pari[4];
    sinf = sin(psi);
    cosf = cos(psi);
    rsinb = x0 * sinf - y0 * cosf;

/*   epsilon (impact parameter in xy projection, with geometrical sign) */
/*   phi at perigee (range 0,2*PI) */

    if (curv == 0.) {
	par[1] = rsinb;
	par[4] = psi;
	rdphi = -x0 * cosf - y0 * sinf;
    } else {
	tem = (x0 * x0 + y0 * y0) * curv - rsinb * 2.;
	par[1] =  - tem / (sqrt(curv * tem + 1.) + 1.);
	par[4] = fmod(atan2(-x0*curv + sinf, y0*curv + cosf) + 2*M_PI, 2*M_PI);
	rdphi = (fmod(par[4] - psi + 3*M_PI, 2*M_PI) - M_PI) / curv;
    }

/*   z of perigee */
    cotth = 1. / tan(pari[3]);
    par[2] = pari[2] + cotth * rdphi;

/*   theta and 1/R at perigee (unchanged) */
    par[3] = pari[3];
    par[5] = curv;

/*   Computation of covariance matrix */

/*    transformation from (Phi, z, theta, beta, 1/Rtr) */
/*                     to (Phi, z, theta, psi,  1/Rtr) */
    cov[1] = covi[1];
    cov[2] = covi[2];
    cov[3] = covi[3];
    cov[4] = covi[4];
    cov[5] = covi[5];
    cov[6] = covi[6];
    cov[7] = covi[7]+covi[1];
    cov[8] = covi[8]+covi[2];
    cov[9] = covi[9]+covi[4];
    cov[10] = covi[10]+covi[1]+2.*covi[7];
    cov[11] = covi[11];
    cov[12] = covi[12];
    cov[13] = covi[13];
    cov[14] = covi[14]+covi[11];
    cov[15] = covi[15];

/*   transformation from Phi to r*Phi */

    for (i__ = 1; i__ <= 5; ++i__) {
	j = i__ * (i__ - 1) / 2 + 1;
	cov[j] *= *r__;
    }
    cov[1] *= *r__;
    if (pari[5] == 0.) {
	for(i__=11; i__<15; i__++) cov[i__]=0.;
	cov[15] = 1.;
	der15 = 0.;
	der45 = 0.;
    } else {

/*   transformation from (r*Phi,   psi, z0,    theta, 1/Rtr) */
/*                    to (epsilon, zp,  theta, phip,  1/Rtr) */

/*  approximation for derivatives d(epsilon)/d(1/Rtr),    d(phip)/d(1/Rtr),*/
/*           d(epsilon)/d(r*Phi),  d(epsilon)/d(psi), and  d(zp)/d(theta), */
/*   the others are exactly or approximately 1 (diagonal terms) */
/*                                        or 0 (non-diagonal terms). */
	der15 = -(rdphi * rdphi) / 2.;
	der45 = rdphi;
    }

    der11 = rdphi / sqrt(x0 * x0 + y0 * y0 + 1e-20);
    der14 = -rdphi;
    der23 = -(cotth * cotth + 1.) * rdphi;

    cov[1] = der11 * der11 * cov[1] 
           + der11 * 2. * (der14 * cov[7] + der15 * cov[11]) 
           + der14 * (der14 * cov[10] + der15 * 2. * cov[14]) 
           + der15 * der15 * cov[15];
    cov[2] = der11 * (cov[2]  + der23 * cov[4]) 
           + der14 * (cov[8]  + der23 * cov[9]) 
           + der15 * (cov[12] + der23 * cov[13]);
    cov[3] = cov[3] + der23 * 2. * cov[5] + der23 * der23 * cov[6];
    cov[4] = der11 * cov[4] + der14 * cov[9] + der15 * cov[13];
    cov[5] += der23 * cov[6];
    cov[7] = der11 * (cov[7]  + der45 * cov[11]) 
           + der14 * (cov[10] + der45 * cov[14]) 
           + der15 * (cov[14] + der45 * cov[15]);
    cov[8] =          cov[8]  + der23 * cov[9] 
           + der45 * (cov[12] + der23 * cov[13]);
    cov[9] += der45 * cov[13];
    cov[10] = cov[10] + der45 * 2. * cov[14] + der45 * der45 * cov[15];
    cov[11] =  der11 * cov[11] + der14 * cov[14] + der15 * cov[15];
    cov[12] += der23 * cov[13];
    cov[14] += der45 * cov[15];

/*   transformation from theta to ctg(theta) 

    tem = -1. - cotth * cotth;
    cov[4]  *= tem;
    cov[5]  *= tem;
    cov[6]  *= tem*tem;
    cov[9]  *= tem;
    cov[13] *= tem;

    invert matrix COV to get WGT 
*/

    //    tmxinv_(&cov[1], &wgt[1], ierr);

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::matrix_inverter(double *wgt, double *covm, long *ierr)
{
  double t11, s12, s13, s14, s15, t22, s23, s24, s25, t33, s34, s35,
         t44, s45, t55, t45, t34, t35, t23, t24, t25, t12, t13, t14, t15;

/************************************************************************/
/*   Inversion of a (5x5) symmetric positive matrix (Cholesky method)   */
/*   Internal variables are in double precision                         */
/*   Check on positivity (IERR=2 if WGT not positive)                   */
/*   COVM (output) may overwrite WGT (input)                            */
/************************************************************************/

    /* Parameter adjustments */
    --covm;
    --wgt;

    /* Function Body */
    *ierr = 0;

    if (wgt[1] <= 0.) {
        *ierr = 2;
        return;
    }
    t11 = 1. / sqrt(wgt[1]);
    s12 = wgt[2] * t11;
    s13 = wgt[4] * t11;
    s14 = wgt[7] * t11;
    s15 = wgt[11] * t11;

    t22 = wgt[3] - s12 * s12;
    if (t22 <= 0.) {
        *ierr = 2;
        return;
    }
    t22 = 1. / sqrt(t22);
    s23 = (wgt[5]  - s12 * s13) * t22;
    s24 = (wgt[8]  - s12 * s14) * t22;
    s25 = (wgt[12] - s12 * s15) * t22;

    t33 = wgt[6] - s13 * s13 - s23 * s23;
    if (t33 <= 0.) {
        *ierr = 2;
        return;
    }
    t33 = 1. / sqrt(t33);
    s34 = (wgt[9]  - s13 * s14 - s23 * s24) * t33;
    s35 = (wgt[13] - s13 * s15 - s23 * s25) * t33;

    t44 = wgt[10] - s14 * s14 - s24 * s24 - s34 * s34;
    if (t44 <= 0.) {
        *ierr = 2;
        return;
    }
    t44 = 1. / sqrt(t44);
    s45 = (wgt[14] - s14 * s15 - s24 * s25 - s34 * s35) * t44;

    t55 = wgt[15] - s15 * s15 - s25 * s25 - s35 * s35 - s45 * s45;
    if (t55 <= 0.) {
        *ierr = 2;
        return;
    }
    t55 = 1. / sqrt(t55);

    t45 = -t44 * (s45 * t55);
    t34 = -t33 * (s34 * t44);
    t35 = -t33 * (s34 * t45 + s35 * t55);
    t23 = -t22 * (s23 * t33);
    t24 = -t22 * (s23 * t34 + s24 * t44);
    t25 = -t22 * (s23 * t35 + s24 * t45 + s25 * t55);
    t12 = -t11 * (s12 * t22);
    t13 = -t11 * (s12 * t23 + s13 * t33);
    t14 = -t11 * (s12 * t24 + s13 * t34 + s14 * t44);
    t15 = -t11 * (s12 * t25 + s13 * t35 + s14 * t45 + s15 * t55);

    covm[1]  = t11 * t11 + t12 * t12 + t13 * t13 + t14 * t14 + t15 * t15;
    covm[2]  = t12 * t22 + t13 * t23 + t14 * t24 + t15 * t25;
    covm[3]  = t22 * t22 + t23 * t23 + t24 * t24 + t25 * t25;
    covm[4]  = t13 * t33 + t14 * t34 + t15 * t35;
    covm[5]  = t23 * t33 + t24 * t34 + t25 * t35;
    covm[6]  = t33 * t33 + t34 * t34 + t35 * t35;
    covm[7]  = t14 * t44 + t15 * t45;
    covm[8]  = t24 * t44 + t25 * t45;
    covm[9]  = t34 * t44 + t35 * t45;
    covm[10] = t44 * t44 + t45 * t45;
    covm[11] = t15 * t55;
    covm[12] = t25 * t55;
    covm[13] = t35 * t55;
    covm[14] = t45 * t55;
    covm[15] = t55 * t55;

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::tkffit(long *np, double *rcoord, double *phcoord, double *zcoord, 
		double *errxy, double *errz, double *errr, long *ihtype, double *rf,
		double *paraf, double *em1, double *chi2, long *ihdrop, long *ierr,
		bool electron)
{
    /* System generated locals */
    long i__1;
    double r__1;

    /* Local variables */
    double pari[5], sinb, cons, sigr[3], sigz[3], sint, achi2;
    long i__;
    double d1, d2;
    long i3[3];
    double sigxy[3], r3, x0;
    long ih;
    double dp[5], parabb[6], parabe[6], w11, w22;
    double wm[15], alrphi;
    double wm1[15];
    double fco[3], der[8], dir;
    long ier;
    double phi, rco[3], zco[3];
    double c_b7=-9999., c_b8=9999., c_b9=.9999, c_b20=0.;
    long c__0=0, c__1=1, c_n1=-1;

    double fac1=M_PI/180.;

/* -------------------------------- */

/* Main routine of the Kalman-Filtering method for track fitting (at this */
/*     moment for barrel tracker only; the forwards region and barrel/forward*/
/*     mixed region will be integrated in later). Some subroutines are */
/*     learnt from Prof. Billior. */

/* In order to feed the vertex fitting package (i.e. TVSIMP and TVFULL), */
/*     after calling this routine, another 2 routines have to be called */
/*     sequentially: */
/*     (1) TXTOBP: to extrapolate the fitted track into the beam-pipe and */
/*                to take the multiple scattering of beam-pipe into account;*/
/*         and/or, to re-order the track cylindrical-polar parameters from */
/*                 (phi,z,theta,beta,1/R) to (phi,psi,z,theta,1/p), if the */
/*                 track is generated outside the beam pipe (i.e. it does */
/*                 not intercept with the beam pipe); */
/*     (2) TCLPRG: to convert the cylindrical-polar parameters into the */
/*         perigee parameters. */
/*                                                   -Sijin QIAN,  3/4/1994*/

/*These 2 steps have been already integrated into a vertex fitting steering*/
/*    routine, i.e. TVSTEE; so now the fitted vertex and re-fitted tracks can*/
/*     be obtained by calling TVSTEE right after this TKFFIT. */

/*                                                   -Sijin QIAN, 22/4/1994*/

/* The merge of the barrel and forward regions is completed. */
/*                                                   -Sijin QIAN,  7/5/1994*/
/* ------------------------------------------- */

/*Modification for the OO tracker model: ignor the multiple scattering of the*/
/*                                       material in the tracker temporarily.*/
/*                                                   -Sijin QIAN, 29/6/1997*/
/* ------------------------------------------- */

/* Modification for the Atlas OO TRT tracker: */
/*   (1) increase the max. points on track to 73; */
/*   (2) activate the multiple scattering of material in the tracker, and the*/
/*       material on each hit now depends on the number of hits. */
/*                                                   -Sijin QIAN, 30/11/1998*/
/* ------------------------------------------- */
/* Input: np            -- Number of points on the track (max. 73, min. 3) */
/*        rcoord(1:73)  -- r coordinates of points */
/*                         (ordered from small to large) (cm) */
/*        phcoord(1:73) -- phi coord. of points (radians) */
/*        zcoord(1:73)  -- z coord. of points (cm) */
/*        errxy(1:73)   -- error on r-phi (cm) */
/*        errz(1:73)    -- error on z (cm) */
/*        errr(1:73)    -- error on r (cm) */
/*        ihtype(1:73)  -- type of the hit (0 for barrel, 1 for forward) */

/*Output: rf           -- radial location of the fitted track parameters (cm)*/
/*        paraf(1:5)   -- fitted track parameters (phi,z,theta,beta,1/R) */
/*        em1(1:15)    -- error matrix for the parameters */
/*                        (now it's a weight matrix for Atlas,   Dec.1,1998)*/
/*        chi2         -- Chi2 of the fit */
/*        ihdrop(1:73) -- index for the rejected hit (1 for yes, 0 for no)*/
/*        ierr         -- error message, =1 np<=3 or np>73 */
/*                                       =2 error in TFIT3P */
/*                                       =3 error in TVTOXX or TXXTOV */
/*                                       =4 error in TSOLVE */
/* -------------------------------- */

    /* Parameter adjustments */
    --ihdrop;
    --em1;
    --paraf;
    --ihtype;
    --errr;
    --errz;
    --errxy;
    --zcoord;
    --phcoord;
    --rcoord;

    /* Function Body */
    *ierr = 0;
    if (*np < 3 || *np > 73) {
	*ierr = 1;
	return;
    }
    //cons = 83.3;
    cons = 166.6;
    for (i__ = 1; i__ <= 73; ++i__) ihdrop[i__] = 0;

    x0 = .16 / double(*np);

/* determine 3 initial points */

    i3[0] = *np;
    if(electron) {
      i3[1] = *np - 1;
      i3[2] = *np - 2;
    } else {
      i3[1] = (*np + 2) / 2;
      i3[2] = 1;
    }

/* start the track from the outmost layer and extrapolate it from point */
/*   to point. */

    for (ih = 0; ih < 3; ++ih) {
	rco[ih] = rcoord[i3[ih]];
	fco[ih] = fmod(phcoord[i3[ih]] + 2*M_PI, 2*M_PI) / fac1;
	zco[ih] = zcoord[i3[ih]];
	sigxy[ih] = errxy[i3[ih]];
	sigz[ih] = errz[i3[ih]];
	sigr[ih] = errr[i3[ih]];
    }

    //    tfit3p_(rco, fco, zco, sigxy, sigz, sigr, &r3, pari, wm, chi2, &ier);

    tfit3p(&rco[0], &fco[0], &zco[0], &sigxy[0], &sigz[0], &sigr[0], &r3, &pari[0], &wm[0], chi2, &ier);
    if (ier != 0) {
	*ierr = 2;
	return;
    }
    pari[0] *= fac1;

    dir = 1.;
    //    tvtoxx_(pari, wm, &parabb[1], wm1);
    tvtoxx(&pari[0], &wm[0], &parabb[1], &wm1[0]);
    for(ih=0;ih<15;ih++) wm[ih]=wm1[ih];

    parabb[0] = r3;
    //    tswtch(&c__0, parabb, wm);
    if (ihtype[*np] == 1) tswtch(&c__0, &parabb[0], &wm[0]);

    for (ih = *np; ih >= 1; --ih) {
	if (ihtype[ih] == 0) {
	  /*   txtrpa_(parabb, &c__0, &rcoord[ih], &c_b7, &c_b8, &c_b9, &c__0, 
		    parabe, der, &alrphi, &ier);
	  */
	    txtrpa(&parabb[0], &c__0, &rcoord[ih], &c_b7, &c_b8, &c_b9, &c__0, 
		   &parabe[0], &der[0], &alrphi, &ier);
	    if (ier != 0) {
		goto L11;
	    }
	    txxdcy(&parabe[0], &parabb[0], &der[0]);
	    txprcw(&wm[0], &der[0], &wm[0]);

/* Add m/s into error matrix, then convert it to weight matrix. (Barrel) */

	} else {
	  /*
	    txxpla_(parabb, &c__0, &zcoord[ih], &c_b20, &c_b8, &c__0, parabe, 
		    der, &alrphi, &ier);
	  */
	    txxpla(&parabb[0], &c__0, &zcoord[ih], &c_b20, &c_b8, &c__0, &parabe[0], 
		   &der[0], &alrphi, &ier);
	    if (ier != 0) {
		goto L11;
	    }
	    txxdpl(&parabe[0], &parabb[0], &der[0]);
	    txprpw(&wm[0], &der[0], &wm[0]);

/* Add m/s into error matrix, then convert it to weight matrix. (Forward) */

	}
	sint = sin(parabe[3]);
	r__1 = parabe[5] * cons * sint;
	//	tmscat_(&sint, &r__1, &x0, wm, &dir);
	tmscat(&sint, &r__1, &x0, &wm[0], &dir);
	if(electron==true) add_E_loss(&x0, &dir, &parabb[5], &wm[0]);

	if (ih == i3[0] || ih == i3[1] || ih == i3[2]) {
	    for(long ii=0; ii<6; ii++) parabb[ii]=parabe[ii];
	    goto L11;
	}

	parabb[0] = parabe[0];
	if (ihtype[ih] == 0) {
/* Computing 2nd power */
	    r__1 = rcoord[ih] / errxy[ih];
	    w11 = r__1 * r__1;
	    d1 = fmod(phcoord[ih] - parabe[1] + 2*M_PI + M_PI, 2*M_PI) - M_PI;
	    w22 = 1. / (errz[ih] * errz[ih]);
	    d2 = zcoord[ih] - parabe[2];
	} else {
	    r3 = sqrt(parabe[1] * parabe[1] + parabe[2] * parabe[2]);
	    phi = fmod(atan2(parabe[2], parabe[1])+2*M_PI,2*M_PI);
	    //	    trf2xy_(&c_n1, &r3, &phi, wm);
	    trf2xy(&c_n1, &r3, &phi, &wm[0]);
	    w11 = 1. / (errr[ih] * errr[ih]);
	    d1 = rcoord[ih] - r3;
/* Computing 2nd power */
	    r__1 = rcoord[ih] / errxy[ih];
	    w22 = r__1 * r__1;
	    d2 = fmod(phcoord[ih] - phi + 2*M_PI + M_PI, 2*M_PI) - M_PI;
	}

/* Add a point to the track, update the parameter and weight matrix. */

	taddpt(&d1, &d2, &w11, &w22, &dp[0], &wm[0], &achi2, &ier);

	if (ier != 0) {
	    ihdrop[ih] = 1;
	    for(long ii=1; ii<6; ii++) parabb[ii]=parabe[ii];
	} else {
	    if (ihtype[ih] == 0) {
	        for(long ii=1; ii<6; ii++) parabb[ii]=parabe[ii]+dp[ii-1];
		sinb = fabs(sin(parabb[4]));
	    } else {
		r3 += dp[0];
		phi += dp[1];
		parabb[1] = r3 * cos(phi);
		parabb[2] = r3 * sin(phi);
	        for(long ii=3; ii<6; ii++) parabb[ii]=parabe[ii]+dp[ii-1];
		sinb = fabs(sin(parabb[4] - phi));
		trf2xy(&c__1, &r3, &phi, &wm[0]);
	    }

	    if (sinb >= 1.) {
		*ierr = 2;
		return;
	    }
	    *chi2 += achi2;
/* 		parabb(6)=1./(1./parabb(6)-SIGN(8.e-2,PARABb(6))) */
	}
L11:
	if (ih==1) goto L10;

	if ((i__1 = ihtype[ih] - ihtype[ih - 1]) < 0) {
	    goto L311;
	} else if (i__1 == 0) {
	    goto L312;
	} else {
	    goto L313;
	}
L311:
	tswtch(&c__0, &parabb[0], &wm[0]);
L312:
	goto L10;
L313:
	tswtch(&c__1, &parabb[0], &wm[0]);
L10:
	;
    }

    if (ihtype[1] == 1) {
	tswtch(&c__1, &parabb[0], &wm[0]);
    }
    for(long ii=1; ii<6; ii++) paraf[ii]=parabb[ii];
    *rf = parabb[0];
    for(long ij=1; ij<16; ij++) em1[ij]=wm[ij-1];
/*        CALL TMXINV(WM,EM1,IERR) */

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::tfit3p(double *rco, double *fco, double *zco, double *sigxy, 
		   double *sigz, double *sigr, double *r3, double *para,
		   double *wgc, double *chi2c, long *ierr)
{
    double der13, der23, der24, der15, der25, cosa, d21sq, fmax, d23sq; 
    double sina, sinb, sdet, tanl, tnsb, rotf, rinv, rtrk, alph3; 
    double alph[2], sinb2, r1, r2, r__3, sinb22[2];

    long i;
    double x[3], y[3], z[3];
    
    double z0, sumwz, d23, ds[2], xc, yc, rr[3], sumwsz, fac;
    double dx21, dy21, dx23, dy23;
    double wff[3], tem, rfz[9]	/* was [3][3] */, wzz[3];
    double d_r = M_PI/180.;

/* ---------------------------------------------------------- */

/* Subroutine for fitting 3 points to a helix */
/* 
   error code: ierr=1 --- Track radius too small ( < 0.5 GeV, i.e. 83 cm)
                   =2 --- sin(beta) >=1
                   =3 --- two hits have the same (x,y)
*/

/* ---------------------------------------------------------- */

    /* Parameter adjustments */
    --wgc;
    --para;
    --sigr;
    --sigz;
    --sigxy;
    --zco;
    --fco;
    --rco;

    /* Function Body */
    for (i = 1; i <= 3; ++i) {
	rfz[i * 3 - 3] = rco[i];
	rfz[i * 3 - 2] = fco[i];
	rfz[i * 3 - 1] = zco[i];
    }

/* Rotate 3 points to 0-90 degrees region */

    fmax = std::max(std::max(rfz[1], rfz[4]),rfz[7]);
    if (fmax <= 90.) {
	rotf = 0.;
	goto L77;
    }

    if (fmax >= 270. && std::min(std::min(rfz[1],rfz[4]),rfz[7]) <= 90.) {
	rotf = 10. - fmax;
	goto L77;
    }
    rotf = 80. - fmax;
L77:

/* The X and Y coords. of 3 points */

    for (i = 1; i <= 3; ++i) {
	x[i - 1] = rfz[i * 3 - 3] * cos((rfz[i * 3 - 2] + rotf) * d_r);
	y[i - 1] = rfz[i * 3 - 3] * sin((rfz[i * 3 - 2] + rotf) * d_r);
	z[i - 1] = rfz[i * 3 - 1];
	rr[i - 1] = rfz[i * 3 - 3];
    }

/* To calculate the parameters (in XY projection) at the point 3 */

   dy21 = y[1] - y[0];
    dx21 = x[1] - x[0];
    d21sq = dx21 * dx21 + dy21 * dy21;
    dy23 = y[1] - y[2];
    dx23 = x[1] - x[2];
    d23sq = dx23 * dx23 + dy23 * dy23;
    if (d21sq==0 || d23sq==0) {*ierr=3; return;}
    d23 = sqrt(d23sq);

    tem = dx21 * dy23 - dx23 * dy21;
    if (tem == 0.) {
	rtrk = 15000.;
	xc=yc=sdet=0.;
    } else {
      sdet = .5 / tem;
      xc = x[1] + sdet * (d23sq * dy21 - d21sq * dy23);
      yc = y[1] + sdet * (d21sq * dx23 - d23sq * dx21);
      
      r1 = xc - x[1];
      r2 = yc - y[1];
      rtrk = sqrt(r1 * r1 + r2 * r2);
    }

/* If the radius of helix .LT. 10 m: exact solution */

    //    if(rtrk<1000.) {

/* To reject the helix with radius .LT. the RMIN */

/* For Pmin=1GeV */
/*        IF(RTRK.LT.72.) THEN */
/* For Pmin=0.5GeV */
    //      if (rtrk < 17.) {
    if (rtrk < minTrackRadius[paramset]) {
	*ierr = 1;
	return;
      }

/* About the sign of the curvature */

      if (sdet < 0.) rtrk *= -1.;
      rinv = 1. / rtrk;

/* Angles at the point 3 and curvature distances of 3-2 and 3-1 */

    if(tem!=0) {

      sina = (x[2] - xc) * rinv;
      cosa = (yc - y[2]) * rinv;
      alph3 = atan2(sina, cosa);
      alph[1] = atan2((x[1] - xc) * rinv, (yc - y[1]) * rinv);
      alph[0] = atan2((x[0] - xc) * rinv, (yc - y[0]) * rinv);
      ds[1] = rtrk * (alph[1] - alph3);
      ds[0] = rtrk * (alph[0] - alph3);
      for(i=0; i<2; i++) {
	sinb = (sin(alph[i]) * x[i] - cos(alph[i]) * y[i]) / rr[i];
	if (fabs(sinb) >= 1.) {
	  *ierr = 2;
	  return;
	}
	sinb22[i]=sinb*sinb;
      }
    }

/* If the radius of helix .GT. 10 m: approximation to the 1st order of 1/r */

    else {
      
      double rrr = 2.*(y[1]-(y[2]*dx21-y[0]*dx23)/(dx21-dx23))/(dx23*dx21);
      double tem1=dx23/d23;
      tem1 = tem1*tem1*tem1*rrr;
      sina = dy23/d23 - fabs(.5*d23*tem1)*(dy23>0. ? 1. : -1.);
      if(fabs(sina)>=1.) {
	*ierr = 1;
	return;
      }
      cosa = sqrt(1.-sina*sina)*(dx23>0. ? 1. : -1.);
      ds[1] = d23;
      ds[0] = sqrt(d21sq)+d23;
    }

/* Parameters phi and sin(beta) at point 3 */

    sinb = (sina * x[2] - cosa * y[2]) / rr[2];
    if (fabs(sinb) >= 1.) {
	*ierr = 2;
	return;
    }
   
    *r3 = rr[2];
    para[1] = rfz[7];
    para[3] = sinb;
    para[5] = rinv;

/* Calculation of weight on phi, z, sin(beta), cot(theta) and 1/R */
/*   and adjustment on z, cot(theta) */

    tanl = (z[1] - z[2]) / d23;
    tnsb = sinb * tanl;

    for(i=1;i<16;i++) wgc[i]=0.;

/* Contribution of the point 3 */

    sinb2 = sinb * sinb;
    wgc[1] = *r3 * *r3 / (sigxy[3] * sigxy[3] + 
			  sigr[3] * sigr[3] * sinb2 / (1. - sinb2));
    r2 = sigr[3] * tanl;
    wgc[3] = 1. / (sigz[3] * sigz[3] + r2 * r2 / (1. - sinb2));
    wzz[2] =wgc[3];

    if(tem==0) sinb22[0]=sinb22[1]=sinb2;

    sumwz = wgc[3] * z[2];
    sumwsz = 0.;

/* Contribution of points 1 and 2 */

    for (i = 1; i < 3; ++i) {

/* Contribution at weight on phi and sin(beta) */

	r1 = rr[i - 1];
	wff[i- 1] = r1 * r1 / (sigxy[i] * sigxy[i] + 
			 sigr[i] * sigr[i] * sinb22[i-1] / (1. - sinb22[i-1]));
	r2 = sigr[i] * tanl;
	wzz[i- 1] = 1. / (sigz[i] * sigz[i] + r2 * r2 / (1. - sinb22[i-1]));

	fac = ds[i - 1] / (1. - sinb22[i-1]);
	der24 = ds[i - 1];
	der23 = fac * tnsb;
	der13 = fac / rr[i - 1];
	der15 = (rr[i - 1] - rr[2]) * .5 * der13;
	der25 = der15 * tnsb;
	wgc[1] += wff[i - 1];
	wgc[4] += wff[i - 1] * der13;
	wgc[6] += wff[i - 1] * der13 * der13;
	wgc[11] += wff[i - 1] * der15;
	wgc[13] += wff[i - 1] * der15 * der13;
	wgc[15] += wff[i - 1] * der15 * der15;
	
/* Contribution at weight on z and cot(theta) */

	wgc[3] += wzz[i - 1];
	wgc[8] += wzz[i - 1] * der24;
	wgc[10] += wzz[i - 1] * der24 * der24;
	
	sumwz += z[i - 1] * wzz[i - 1];
	sumwsz += z[i - 1] * wzz[i - 1] * der24;

/*Contribution of correlations between (phi,sin(beta),1/R) and (z,cot(
theta))*/
	wgc[5] += wzz[i - 1] * der23;
	wgc[6] += wzz[i - 1] * der23 * der23;
	wgc[9] += wzz[i - 1] * der23 * der24;
	wgc[12] += wzz[i - 1] * der25;
	wgc[13] += wzz[i - 1] * der25 * der23;
	wgc[14] += wzz[i - 1] * der25 * der24;
	wgc[15] += wzz[i - 1] * der25 * der25;
    }

/* Resolution of the system equation on z,cot(theta) */

    sdet = wgc[3] * wgc[10] - wgc[8] * wgc[8];
    if (sdet == 0.) {
/*       print *,' Tfit3p : sdet = 0. ' */
	sdet = 0.;
    } else {
	sdet = 1. / sdet;
    }
/* wiw*      SDET=1./(WGC(3)*WGC(10)-WGC(8)**2) */
    z0 = (wgc[10] * sumwz - wgc[8] * sumwsz) * sdet;
    tanl = (wgc[3] * sumwsz - wgc[8] * sumwz) * sdet;

/* Calculation of chi2 */

    r1 = z[2] - z0;
    r2 = z[1] - z0 - tanl * ds[1];
    r__3 = z[0] - z0 - tanl * ds[0];
    *chi2c = wzz[2] * (r1 * r1) + wzz[1] * (r2 * r2) + wzz[0] * (r__3 * r__3);

    para[2] = z0;
    para[4] = tanl;

/* 	print *,'chi2 in fit3=',chi2c */
    *ierr = 0;

    return;

}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::tvtoxx(double *pari, double *wctr, double *par, double *wf)
{
    double r1,cf;

/* ----------------------------------------------- */
/* The routine for transformation from internal parameters to */
/*   cylindrical-polar parameters. */
/*                                            -By Sijin QIAN, 3/1994 */

/* Input: pari --- internal track parameters (phi,Z,sinb,ctg(theta),1/Rtr)
 */
/*        WCTR --- weight matrix of pari, packed in lower triangle. */

/* Output: PAR(5) --- Cylindrical-polar track parameters */
/*                      (Phi,Z,Theta,beta,1/Rtr) */
/*         wf(15) --- Weight matrix corresponding to PAR, */
/*                      packed in lower triangle. */
/* ----------------------------------------------- */

/* Transformation for WEIGHT Matrix from (sin(beta),ctg(theta)) */
/*                                    to (theta,    sin(beta)) */

    /* Parameter adjustments */
    --wf;
    --par;
    --wctr;
    --pari;
   
    //    for(int i=0; i<5; i++) { par[i]=pari[i]; }
    //    for(int i=0; i<15; i++){ wf[i]=wctr[i]; }
    par[2]=pari[2];
    par[5]=pari[5];
    for(long i=1; i<4; i++){ wf[i]=wctr[i]; }
    wf[11]=wctr[11];
    wf[12]=wctr[12];
    wf[15]=wctr[15];

    wf[ 4] = wctr[ 7];
    wf[ 7] = wctr[ 4];
    wf[ 5] = wctr[ 8];
    wf[ 8] = wctr[ 5];
    wf[ 6] = wctr[10];
    wf[10] = wctr[ 6];
    wf[13] = wctr[14];
    wf[14] = wctr[13];

    // had to modify the following to compile on HP-Unix
    if (pari[4]==0.) {
      par[3] = 0.5*M_PI;
    } else {
      double at = atan(1./pari[4]);
      par[3] = fmod(at+M_PI, M_PI); 
    }
    //par[3] = pari[4]==0. ? 0.5*pi : fmod(at+pi, pi); 

    r1 = sin(par[3]);
    cf = -1. / (r1 * r1);

    //    vscale(&wf[4], &cf, &wf[4], &c3);
    wf[4]  = cf * wf[4];
    wf[5]  = cf * wf[5];
    wf[6]  = cf * wf[6] * cf;
    wf[9]  = cf * wctr[9];
    wf[13] = cf * wf[13];

/* Transformation from sin(beta) to beta */

    par[1] = fmod(pari[1]+2*M_PI, 2*M_PI);
    par[4] = asin(pari[3]);
    cf = cos(par[4]);
    //    vscale(&wf[7], &cf, &wf[7], &c4);
    wf[ 7] = cf * wf[ 7];
    wf[ 8] = cf * wf[ 8];
    wf[ 9] = cf * wf[ 9];
    wf[10] = cf * wf[10] * cf;
    wf[14] = cf * wf[14];

    return ;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::txprcw(double *wgi, double *der, double *wgf)
{
  double w31, w41, w51, w52, w53, w54, w55, w44, w43, w42, w33, w32;
/*                                                                      * 
*/
/*   propagation of weight matrix from a cylinder to another one        * 
*/
/*   Input  : WGI  : initial weight matrix                              * 
*/
/*            DER  : inverse derivatives                                * 
*/
/*   Output : WGF  : final weight matrix (may overwrite WGI)            * 
*/
/* ***********************************************************************
*/
/*   computation of   A  =  DER transp * WG */

    /* Parameter adjustments */
    --wgf;
    --der;
    --wgi;

    /* Function Body */
    w55 = wgi[11] * der[3] + wgi[12] * der[6] + wgi[14] * der[8] + wgi[15];
    w54 = wgi[7] * der[3] + wgi[8] * der[6] + wgi[10] * der[8] + wgi[14];
    w53 = wgi[4] * der[3] + wgi[5] * der[6] + wgi[9] * der[8] + wgi[13];
    w52 = wgi[2] * der[3] + wgi[3] * der[6] + wgi[8] * der[8] + wgi[12];
    w51 = wgi[1] * der[3] + wgi[2] * der[6] + wgi[7] * der[8] + wgi[11];

    w44 = wgi[7] * der[2] + wgi[8] * der[5] + wgi[10] * der[7];
    w43 = wgi[4] * der[2] + wgi[5] * der[5] + wgi[9] * der[7];
    w42 = wgi[2] * der[2] + wgi[3] * der[5] + wgi[8] * der[7];
    w41 = wgi[1] * der[2] + wgi[2] * der[5] + wgi[7] * der[7];

    w33 = wgi[4] * der[1] + wgi[5] * der[4] + wgi[6];
    w32 = wgi[2] * der[1] + wgi[3] * der[4] + wgi[5];
    w31 = wgi[1] * der[1] + wgi[2] * der[4] + wgi[4];

/*   computation of   A * DER */

    wgf[1] = wgi[1];
    wgf[2] = wgi[2];
    wgf[3] = wgi[3];

    wgf[4] = w31;
    wgf[5] = w32;
    wgf[6] = w31 * der[1] + w32 * der[4] + w33;

    wgf[7] = w41;
    wgf[8] = w42;
    wgf[9] = w41 * der[1] + w42 * der[4] + w43;
    wgf[10] = w41 * der[2] + w42 * der[5] + w44 * der[7];

    wgf[11] = w51;
    wgf[12] = w52;
    wgf[13] = w51 * der[1] + w52 * der[4] + w53;
    wgf[14] = w51 * der[2] + w52 * der[5] + w54 * der[7];
    wgf[15] = w51 * der[3] + w52 * der[6] + w54 * der[8] + w55;

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::txprpw(double *wgi, double *der, double *wgf)
{
  double w31, w41, w51, w52, w53, w54, w55, w44, w43, w42, w34, w33, w32;

/*   propagation of weight matrix from a plane to another one           * 
*/
/*   Input  : WGI  : initial weight matrix                              * 
*/
/*            DER  : inverse derivatives                                * 
*/
/*   Output : WGF  : final weight matrix (may overwrite WGI)            * 
*/
/* ***********************************************************************
 */
/*   propagation of weight matrix from a plane to a plane */

/*   computation of   W  =  DER transp * WG */

    /* Parameter adjustments */
    --wgf;
    --der;
    --wgi;

    /* Function Body */
    w55 = wgi[11] * der[3] + wgi[12] * der[6] + wgi[14] * der[8] + wgi[15];
    w54 = wgi[7] * der[3] + wgi[8] * der[6] + wgi[10] * der[8] + wgi[14];
    w53 = wgi[4] * der[3] + wgi[5] * der[6] + wgi[9] * der[8] + wgi[13];
    w52 = wgi[2] * der[3] + wgi[3] * der[6] + wgi[8] * der[8] + wgi[12];
    w51 = wgi[1] * der[3] + wgi[2] * der[6] + wgi[7] * der[8] + wgi[11];

    w44 = wgi[7] * der[2] + wgi[8] * der[5] + wgi[10];
    w43 = wgi[4] * der[2] + wgi[5] * der[5] + wgi[9];
    w42 = wgi[2] * der[2] + wgi[3] * der[5] + wgi[8];
    w41 = wgi[1] * der[2] + wgi[2] * der[5] + wgi[7];

    w34 = wgi[7] * der[1] + wgi[8] * der[4] + wgi[9] + wgi[10] * der[7];
    w33 = wgi[4] * der[1] + wgi[5] * der[4] + wgi[6] + wgi[9] * der[7];
    w32 = wgi[2] * der[1] + wgi[3] * der[4] + wgi[5] + wgi[8] * der[7];
    w31 = wgi[1] * der[1] + wgi[2] * der[4] + wgi[4] + wgi[7] * der[7];

/*   computation of   W * DER */

    wgf[1] = wgi[1];
    wgf[2] = wgi[2];
    wgf[3] = wgi[3];

    wgf[4] = w31;
    wgf[5] = w32;
    wgf[6] = w31 * der[1] + w32 * der[4] + w33 + w34 * der[7];

    wgf[7] = w41;
    wgf[8] = w42;
    wgf[9] = w41 * der[1] + w42 * der[4] + w43 + w44 * der[7];
    wgf[10] = w41 * der[2] + w42 * der[5] + w44;

    wgf[11] = w51;
    wgf[12] = w52;
    wgf[13] = w51 * der[1] + w52 * der[4] + w53 + w54 * der[7];
    wgf[14] = w51 * der[2] + w52 * der[5] + w54;
    wgf[15] = w51 * der[3] + w52 * der[6] + w54 * der[8] + w55;

    return;
}



//-------------------------------------------------------------------------




/* *********************************************************************** */
/*                                                                      * */
/*   compute derivatives for a propagation cylinder -> cylinder         * 
*/
/*   knowing the initial and final parameters                           * 
*/
/*   see routine TXTRPA for the definition of PARAMI,PARAMF,DER         * 
*/
/*   and comments                                                       * 
*/
/* ***********************************************************************
 */

void TrigInDetSctKFitter::txxdcy(double *parami, double *paramf, double *der)
{
/* Local variables */
     double radf, fact, radi, dphi, cosb, sinb, rtrk, cdphi, cosbf, 
	    ccpsf, sinbf, ccpsi, cpsif, rdphi, cpsii, sdphi, cotth, 
	    scpsi, spsif, spsii, xc, yc, rc2;
     
     double dphimn=0.01;

/*   radius of the helix, cot(theta) and sin(beta),cos(beta) at both ends 
*/
    /* Parameter adjustments */
    --der;
    --paramf;
    --parami;

    /* Function Body */
    radi = parami[1];
    radf = paramf[1];
    rtrk = 1. / parami[6];
    cotth = 1. / tan(parami[4]);
    sinb = sin(parami[5]);
    cosb = cos(parami[5]);
    sinbf = sin(paramf[5]);
    cosbf = cos(paramf[5]);

/*   variation DPHI of phi angle */

    dphi = paramf[2] + paramf[5] - parami[2] - parami[5];
    dphi = fmod(dphi + 2. * 2*M_PI + M_PI, 2*M_PI) - M_PI;

/*   if |DPHI| >  DPHIMN , use "exact" formulae */

    if (fabs(dphi) >= dphimn) {
	xc = radi - rtrk * sinb;
	yc = rtrk * cosb;

	rc2 = xc * xc + yc * yc;

	ccpsi = radi - rtrk * sinb;
	scpsi = rtrk * cosb;
	ccpsf = radf - rtrk * sinbf;
	//	scpsf = rtrk * cosbf;

	cpsii = rtrk - radi * sinb;
	spsii = -radi * cosb;
	cpsif = rtrk - radf * sinbf;
	spsif = -radf * cosbf;

	sdphi = sin(dphi);
	cdphi = cos(dphi);

	der[1] = 0.;
	fact   = -rtrk / spsif;
	der[2] = sdphi * fact;
	der[3] = fact * rtrk * (1. - cdphi);
	der[4] = -rtrk * dphi * (1. + cotth * cotth);
	der[5] = rtrk * cotth * (radf * ccpsf * spsii / spsif - radi * ccpsi) 
		/ rc2;
	der[6] = rtrk * rtrk * cotth * (-dphi + sinbf / cosbf - (radi * scpsi 
		+ radf * ccpsf * cpsii / spsif) / rc2);
	der[7] = spsii / spsif;
	der[8] = rtrk * (cpsif - cpsii) / spsif;
    }

/*   if |DPHI| < DPHIMN , use first order in 1/R */
    else {
	rdphi  = rtrk * dphi;
	der[1] = 0.;
	der[2] = rdphi / (radf * cosbf);
	der[3] = rdphi * .5 * der[2];
	der[4] = -rdphi * (cotth * cotth + 1.);
	der[5] = radi * cotth * sin(paramf[2] - parami[2]) / cosbf;
	der[6] = rdphi * .5 * der[5];
	der[7] = radi * cosb / (radf * cosbf);
	der[8] = rdphi * .5 * (der[7] + 1.);
    }

    return;
}



//-------------------------------------------------------------------------



/* ******************************************************************** * 
*/
/*   compute derivatives for a propagation plane -> plane               * 
*/
/*   knowing the initial and final parameters                           * 
*/
/*   see routine TXXPLA for the definition of PARAMI,PARAMF,DER         * 
*/
/*   and comments                                                       * 
*/
/* ***********************************************************************
 */

void TrigInDetSctKFitter::txxdpl(double *parami, double *paramf, double *der)
{

 /* Local variables */
    double dphi, rtrk, cosf0, cosf1, sinf0, sinf1, dcosf, dsinf, 
           rdphi, tanth, dz;

 /* Parameter adjustments */
    --der;
    --paramf;
    --parami;

    /* Function Body */
    cosf0 = cos(parami[5]);
    cosf1 = cos(paramf[5]);
    sinf0 = sin(parami[5]);
    sinf1 = sin(paramf[5]);
    tanth = tan(parami[4]);
    dz = paramf[1] - parami[1];
    rdphi = tanth * dz;

    double tem = (tanth * tanth + 1.) * dz;
    der[1] = tem * cosf1;
    der[4] = tem * sinf1;
    der[7] = tem * parami[6];
    der[8] = rdphi;

    dphi = rdphi * parami[6];

/*   "exact" formulae if |DPHI| > DPHIMN */

    if (fabs(dphi) >= .01) {
	rtrk = 1. / parami[6];
	dcosf = cosf1 - cosf0;
	dsinf = sinf1 - sinf0;
	der[2] = rtrk * dcosf;
	tem = rtrk *rtrk;
	der[3] = tem * (dphi * cosf1 - dsinf);
	der[5] = rtrk * dsinf;
	der[6] = tem * (dphi * sinf1 + dcosf);

/*   first order in 1/R if |DPHI| < DPHIMN */
    } else {
	der[2] = -rdphi * sinf0;
	der[3] = rdphi * .5 * der[2];
	der[5] = rdphi * cosf0;
	der[6] = rdphi * .5 * der[5];
    }

    return ;
}



//-------------------------------------------------------------------------



/* ********************************************************************** 
*/
/*   change from "plane" parameters to "barrel" parameters              * 
*/
/*   at a given point (without propagation), or conversely,             * 
*/
/*   and tranform the deviations from ref. traj. and the weight matrix  * 
*/
/*   Input   :   ITYP      :  type to be changed ('PLAN'=1 or 'CYLI'=0) * 
*/
/*               PAR(1-6)  :  plane  parameters                         * 
*/
/*                             z(fixed),x,y,theta,phi,1/R               * 
*/
/*                            or cylinder parameters                    * 
*/
/*                             r(fixed),Phi,z,theta,beta=phi-Phi,1/R    * 
*/
/**               DQ(1-5)   :  deviations from reference trajectory      
**/
/**                             on x,y,theta,phi,1/R       (plane)       
**/
/**                             or Phi,z,theta,beta,1/R    (cylinder)    
**/
/*               WG(1-15)  :  weight matrix on these parameters         * 
*/
/*                                                                      * 
*/
/*   Output  :   PAR(1-6)  :  parameters with the new type              * 
*/
/*               DQ(1-5)   :  deviations from reference trajectory      * 
*/
/*               WG(1-15)  :  weight matrix on these parameters         * 
*/
/************************************************************************/

 void TrigInDetSctKFitter::tswtch(long *ityp, double *par, double *wg)
{
    double cosb, cosf, sinf, x, y, z, rcosb, tanth, d11, d21, 
           d12, d22, d42, d41, w11, w12, w13, w14, w15, w21, w22, capphi, 
           rad, phi;

    /* Parameter adjustments */
    --wg;
    --par;

    /* Function Body */
    if (*ityp == 1) {

/*   transformation of parameters */

	z = par[1];
	x = par[2];
	y = par[3];
	phi = par[5];

	par[1] = sqrt(x*x + y*y); 
	par[2] = fmod(atan2(y,x)+2*M_PI,2*M_PI);
	par[3] = z;
	par[5] = fmod(phi-par[2]+2*M_PI+M_PI, 2*M_PI)-M_PI;

/* * */
/* *   transformation of deviations */
/* * */
/*        RCOSB=PAR(1)*COS(PAR(5)) */
	
	tanth = tan(par[4]);
	cosf = cos(phi);
	sinf = sin(phi);
/*        DQQ=(X*DQ(1)+Y*DQ(2))/RCOSB */
/*        DQ(1)= (-SINF*DQ(1)+COSF*DQ(2))/RCOSB */
/*        DQ(2)=-DQQ/TANTH */
/*        DQ(4)=DQ(4)-PAR(6)*DQQ-DQ(1) */

/*   transformation of weight matrix  x,y,phi -> Phi,z,phi */

	d11 = -y;
	d21 = x;
	d12 = -tanth * cosf;
	d22 = -tanth * sinf;
	d42 = -tanth * par[6];

	w11 = d11 * wg[1] + d21 * wg[2];
	w12 = d11 * wg[2] + d21 * wg[3];
	w13 = d11 * wg[4] + d21 * wg[5];
	w14 = d11 * wg[7] + d21 * wg[8];
	w15 = d11 * wg[11] + d21 * wg[12];
	w21 = d12 * wg[1] + d22 * wg[2] + d42 * wg[7];
	w22 = d12 * wg[2] + d22 * wg[3] + d42 * wg[8];
	wg[5] = d12 * wg[4] + d22 * wg[5] + d42 * wg[9];
	wg[8] = d12 * wg[7] + d22 * wg[8] + d42 * wg[10];
	wg[12] = d12 * wg[11] + d22 * wg[12] + d42 * wg[14];

	wg[1] = w11 * d11 + w12 * d21;
	wg[2] = w21 * d11 + w22 * d21;
	wg[3] = w21 * d12 + w22 * d22 + wg[8] * d42;

/*   transformation  Phi,phi -> Phi,beta */

	wg[1] = wg[1] + w14 * 2. + wg[10];
	wg[2] += wg[8];
	wg[4] = w13 + wg[9];
	wg[7] = w14 + wg[10];
	wg[11] = w15 + wg[14];

/*   transformation from 'CYLI' to 'PLAN' -------------------------- */

    } else {

/*   transformation of parameters */

	rad = par[1];
	capphi = par[2];
	cosb = cos(par[5]);

	par[1] = par[3];
	par[2] = rad * cos(capphi);
	par[3] = rad * sin(capphi);
	par[5] = fmod(capphi+par[5]+2.*2*M_PI,2*M_PI);
	cosf = cos(par[5]);
	sinf = sin(par[5]);
	tanth = tan(par[4]);
/* * */
/* *   transformation of deviations */
/* * */
/*        DQ(4)=DQ(4)-TANTH*PAR(6)*DQ(2)+DQ(1) */
/*        DQ1= -PAR(3)*DQ(1)-TANTH*COSF*DQ(2) */
/*        DQ(2)=PAR(2)*DQ(1)-TANTH*SINF*DQ(2) */
/*        DQ(1)=DQ1 */

/*   transformation of weight matrix  Phi,beta -> Phi,phi */

	wg[1] = wg[1] - wg[7] * 2. + wg[10];
	wg[2] -= wg[8];
	wg[4] -= wg[9];
	wg[7] -= wg[10];
	wg[11] -= wg[14];

/*   transformation of weight matrix  Phi,z,phi -> x,y,phi */

	rcosb = rad * cosb;
	d11 = -sinf / rcosb;
	d12 = cosf / rcosb;
	double tem = tanth * rcosb;
	d21 = -par[2] / tem;
	d22 = -par[3] / tem;
	tem = par[6] / rcosb;
	d41 = -par[2] * tem;
	d42 = -par[3] * tem;

	w11 = d11 * wg[1] + d21 * wg[2] + d41 * wg[7];
	w12 = d11 * wg[2] + d21 * wg[3] + d41 * wg[8];
	w13 = d11 * wg[4] + d21 * wg[5] + d41 * wg[9];
	w14 = d11 * wg[7] + d21 * wg[8] + d41 * wg[10];
	w15 = d11 * wg[11] + d21 * wg[12] + d41 * wg[14];
	w21 = d12 * wg[1] + d22 * wg[2] + d42 * wg[7];
	w22 = d12 * wg[2] + d22 * wg[3] + d42 * wg[8];
	wg[5] = d12 * wg[4] + d22 * wg[5] + d42 * wg[9];
	wg[8] = d12 * wg[7] + d22 * wg[8] + d42 * wg[10];
	wg[12] = d12 * wg[11] + d22 * wg[12] + d42 * wg[14];

	wg[1] = w11 * d11 + w12 * d21 + w14 * d41;
	wg[2] = w21 * d11 + w22 * d21 + wg[8] * d41;
	wg[3] = w21 * d12 + w22 * d22 + wg[8] * d42;
	wg[4] = w13;
	wg[7] = w14;
	wg[11] = w15;
    }

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::txtrpa(double *parami, long *idir, double *radf,
			double *zmin, double *zmax, double *sinbmx, 
			long *iopt, double *paramf, double *der, 
			double *alrphi, long *ierr)
{
    /* System generated locals */
    double r__1;

    double dphimn=0.0001;

    /* Local variables */
    double alff, beta, fact, radi, dphi, cosb, phii, delt, sinb, 
           cosf, phif, sinf, rtrk, cdphi, cosbf, ccpsf, dcosf, sinbf, ccpsi, 
           dsinf, cpsif, rdphi, cpsii, sdphi, rcosb, sphif, cotth, 
          scpsi, spsif, spsii, aa, xc, yc, xf, yf, zf, rc2, dr2, pph, rrr;

/* ***********************************************************************
 */
/*    AIM :                                                             * */
/*    -----                                                             * 
*/
/*    Extrapolate a helix defined by the initial parameters PARAMI      * */
/*    up to a given cylinder, and compute if requested the derivatives  * */
/*    of the final parameters w.r.t. the initial ones                   * */
/*    The computation uses double precision on intermediate variables   * */
/*    If the variation of phi angle is less than DPHIMN (.0001 in this  * */
/*    version) the computation is done at first order in 1/R in order   * */
/*    to avoid rounding errors, especially in the derivatives           * */
/*                                                                      * */
/*    Input  :  PARAMI(1-6)   : initial parameters                      * */
/*                              (r,Phi,z,theta,beta,1/R)                * */
/*                              with beta = phi-Phi                     * */
/*              IDIR    :  if  1 : positive extrapolation only          * */
/*                         if -1 : negative         "                   * */
/*                         if  0 : extrapolation on both sides          * */
/*              RADF          : radius of the cylinder                  * */
/*              ZMIN          : lower z limit of the cylinder           * */
/*              ZMAX          : upper z limit of the cylinder           * */
/*              SINBMX        : maximum allowed for |sin(beta)| at the  * */
/*                              intersection                            * */
/*              IOPT          : 0 if derivatives not requested          * */
/*                              1 if derivatives requested              * */
/*                                                                      * */
/*    Output :  IERR          : 0 if intersection found                 * */
/*                              1 if no intersection with the cylinder  * */
/*                              2 if SINBMX exceeded                    * */
/*                              3 if intersection outside of z limits   * */
/*                              4 if 1/Rtr = 0                          * */
/*              PARAMF(1-6)   : final parameters                        * */
/*              DER(1-8)      : deriv. of final w.r.t. initial param.   * */
/*                              DER(1) = d(1/R)/d(1/R)                  * */
/*                              DER(2) = d(Phi)/d(beta)                 * */
/*                              DER(3) = d(Phi)/d(1/R)                  * */
/*                              DER(4) = d(z)/d(theta)                  * */
/*                              DER(5) = d(z)/d(beta)                   * */
/*                              DER(6) = d(z)/d(1/R)                    * */
/*                              DER(7) = d(beta)/d(beta)                * */
/*                              DER(8) = d(beta)/d(1/R)                 * */
/*              ALRPHI        : length (in r-phi projection) from start * */
/*                              to extrapolation, with a sign (positive * */
/*                              if the extrapolation is towards the     * */
/*                              direction defined by theta,phi)         * */
/* ***********************************************************************
 */
    /* Parameter adjustments */
    --der;
    --paramf;
    --parami;

    /* Function Body */
    *ierr = 0;

    if (parami[6] == 0.) {
	*ierr = 4;
	return;
    }
    radi = parami[1];
    phii = parami[2];
    beta = parami[5];
    cosb = cos(beta);
    sinb = sin(beta);
    cotth = 1. / tan(parami[4]);
    rtrk = 1. / parami[6];

/*  center and squared radius of the projected helix, in a rotated frame 
*/
/*  (x-axis through the starting point) */
    xc = radi - rtrk * sinb;
    yc = rtrk * cosb;
    rc2 = xc * xc + yc * yc;

/*  resolution of a second order equation */
/* Computing 2nd power */
    r__1 = *radf;
    rrr = (r__1 * r__1 - rtrk * rtrk - rc2) / (rtrk * 2.);
    delt = rc2 - rrr * rrr;

/*   intersection exists if DELT > 0 */

    if (delt <= 0.) {
	*ierr = 1;
    } else {
	delt = sqrt(delt);

/*   choose intersection on the same side as the starting point */
/*   w.r.t. the plane containing the z axis and the axis of the helix */

	sinf = (xc * rrr + yc * delt) / rc2;
	cosf = (xc * delt - yc * rrr) / rc2;
	xf = xc + rtrk * sinf;
	yf = yc - rtrk * cosf;
	sinbf = (sinf * xf - cosf * yf) / *radf;

/*   exit if beta too large at the intersection */
	if (fabs(sinbf) > *sinbmx) {
	    *ierr = 2;
	} else {
	    alff = atan2(sinf, cosf);
	    dphi = fmod(alff - beta + 2*M_PI + M_PI, 2*M_PI) - M_PI;
	    *alrphi = rtrk * dphi;

/*   select positive or negative extrapolations, or both */

	    if (*alrphi * *idir < 0.) {
		*ierr = 1;
		return;
	    }

/*   switch to approximate expressions if the variation of phi */
/*   is less than DPHIMN */

/*   "exact" expressions --------------------------------------- */

	    if (fabs(dphi) >= dphimn) {

		zf = parami[3] + cotth * rtrk * dphi;

/*   exit if outside of limits in z,theta,Phi */

		phif = atan2(yf, xf);
		pph = phif;
		if (pph < 0.) {
		    pph += 2*M_PI;
		}
		if (zf < *zmin || zf > *zmax) {
		    *ierr = 3;
		} else {

/*   final parameters */
		    paramf[1] = *radf;
		    paramf[2] = fmod(phii + phif + 2*M_PI, 2*M_PI);
		    paramf[3] = zf;
		    paramf[4] = parami[4];
		    paramf[5] = fmod(alff - phif + 2*M_PI + M_PI, 2*M_PI) - M_PI;
		    paramf[6] = parami[6];

/*   computation of derivatives --------- */

		    if (*iopt == 1) {
			cosbf = sqrt(1. - sinbf*sinbf);

/*  CCPSI = RC*cos(cap.psi) ; SCPSI = RC*sin(cap.psi)     (initial point) */
/*  CCPSF = RC*cos(cap.psi) ; SCPSF = RC*sin(cap.psi)     (final point) */
			ccpsi = radi - rtrk * sinb;
			scpsi = rtrk * cosb;
			ccpsf = *radf - rtrk * sinbf;
			

/*  CPSII = sgn*RC*cos(psi) ; SPSII = sgn*RC*sin(psi)     (initial point) */
/*  CPSIF = sgn*RC*cos(psi) ; SPSIF = sgn*RC*sin(psi)     (final point) */
			cpsii = rtrk - radi * sinb;
			spsii = -radi * cosb;
			cpsif = rtrk - *radf * sinbf;
			spsif = -(*radf) * cosbf;

			sdphi = sin(dphi);
			cdphi = cos(dphi);

			der[1] = 0.;
			fact = -rtrk / spsif;
			der[2] = sdphi * fact;
			der[3] = fact * rtrk * (1. - cdphi);
			der[4] = -rtrk * dphi * (cotth * cotth + 1.);
			der[5] = rtrk * cotth * (*radf * ccpsf * spsii / 
						 spsif - radi * ccpsi) / rc2;
			der[6] = rtrk * rtrk * cotth * (-dphi + sinbf / cosbf 
				- (radi * scpsi + *radf * ccpsf * cpsii / 
				   spsif) / rc2);
			der[7] = spsii / spsif;
			der[8] = rtrk * (cpsif - cpsii) / spsif;
		    }
		}

/*   approximation at first order in 1/R --------------------
------ */

	    } else {

		dr2 = *radf * *radf - radi * radi;
		rcosb = radi * cosb;
		aa = 1. - radi * sinb / rtrk;
		delt = rcosb * rcosb + aa * dr2;
/*   exit if no solution */
		if (delt <= 0.) {
		    *ierr = 1;
		} else {
		    rdphi = (sqrt(delt) - rcosb) / aa;
		    dphi = rdphi / rtrk;
		    dcosf = -sinb - cosb * .5 * dphi;
		    cosf = cosb + dcosf * dphi;
		    yf = -rdphi * dcosf;
		    dsinf = cosb - sinb * .5 * dphi;
		    sinf = sinb + dsinf * dphi;
		    xf = radi + rdphi * dsinf;
		    sinbf = (sinf * xf - cosf * yf) / *radf;
		    zf = parami[3] + cotth * rdphi;

/*   exit if outside of limits in z,theta,Phi */

		    phif = atan2(yf, xf);
		    pph = phif;
		    if (pph < 0.) {
			pph += 2*M_PI;
		    }
		    if (zf < *zmin || zf > *zmax) {
			*ierr = 3;
		    } else {
			paramf[1] = *radf;
			phif = atan2(yf, xf);
			paramf[2] = fmod(parami[2] + phif + 2*M_PI, 2*M_PI);
			paramf[3] = zf;
			paramf[4] = parami[4];
			paramf[5] = parami[5] + dphi - phif;
			paramf[6] = parami[6];

/*   Computation of derivatives -------------- */

			if (*iopt == 1) {
			    cosbf = sqrt(1. - sinbf * sinbf);
			    sphif = yf / *radf;

			    der[1] = 0.;
			    der[2] = rdphi / (*radf * cosbf);
			    der[3] = rdphi * .5 * der[2];
			    der[4] = -rdphi * (cotth * cotth + 1.);
			    der[5] = radi * cotth * sphif / cosbf;
			    der[6] = rdphi * .5 * der[5];
			    der[7] = radi * cosb / (*radf * cosbf);
			    der[8] = rdphi * .5 * (der[7] + 1.);
			}
		    }
		}
	    }
	}
    }

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::txxpla(double *parami, long *idir, double *zf, double *rmin, 
			double *rmax, long *iopt, double *paramf, 
			double *der, double *alrphi, long *ierr)
{
 
    /* Local variables */
    double dphi, rtrk, cosf0, cosf1, sinf0, sinf1, dcosf, dsinf, 
           rdphi, tanth, r1, x1, y1, ct2inv, xc, yc, dz, phi1;

/* ***********************************************************************
 */
/*    AIM :                                                             * */
/*    -----                                                             * 
*/
/*    extrapolate a helix defined by the initial parameters PARAMI      * */
/*    up to a given z-plane, and compute the derivatives of the         * */
/*    final parameters w.r.t. the initial ones                          * */
/*                                                                      * */
/*    The computation uses double precision on intermediate variables   * */
/*    If the variation of phi angle is less than DPHIMN (.0001 in this  * */
/*    version) the computation is done at first order in 1/R in order   * */
/*    to avoid rounding errors in the derivatives                       * */
/*                                                                      * */
/*    Input  :  PARAMI(1-6)   : initial parameters                      * */
/*                              (z,x,y,theta,phi,1/R)                   * */
/*              ZF            : z of the final plane                    * */
/*              IDIR    :  if  1 : positive extrapolation only          * */
/*                         if -1 : negative         "                   * */
/*                         if  0 : extrapolation on both sides          * */
/*              RMIN          : lower limit of r on the plane           * */
/*              RMAX          : upper limit of r on the plane           * */
/*              IOPT          : 0 if derivatives not requested          * */
/*                              1 if derivatives requested              * */
/*                                                                      * */
/*    Output :  IERR          : 0 if OK                                 * */
/*                              1 if no intersection found              * */
/*                              3 if intersection outside of limits     * */
/*              PARAMF(1-6)   : final parameters                        * */
/*              DER(1-8)      : deriv. of final w.r.t. initial param.   * */
/*                              DER(1) = d(x)/d(theta)                  * */
/*                              DER(2) = d(x)/d(phi)                    * */
/*                              DER(3) = d(x)/d(1/R)                    * */
/*                              DER(4) = d(y)/d(theta)                  * */
/*                              DER(5) = d(y)/d(phi)                    * */
/*                              DER(6) = d(y)/d(1/R)                    * */
/*                              DER(7) = d(phi)/d(theta)                * */
/*                              DER(8) = d(phi)/d(1/R)                  * */
/*              ALRPHI        : length (in r-phi projection) from start * */
/*                              to extrapolation, with a sign (positive * */
/*                              if the extrapolation is towards the     * */
/*                              direction defined by theta,phi)         * */
/* ***********************************************************************
 */
/*      REAL RZSURF,ZRMIN,ZRMAX,THXMIN,THXMAX,PHYMIN,PHYMAX */

    /* Parameter adjustments */
    --der;
    --paramf;
    --parami;

    /* Function Body */
    *ierr = 0;

    dz = *zf - parami[1];
    tanth = tan(parami[4]);
    rdphi = dz * tanth;

/*   select positive or negative extrapolation, or both */
    if (rdphi * *idir < 0.) {
	*ierr = 1;
    } else {
	*alrphi = rdphi;

/*   x,y coordinates of intersection */
	rtrk = 1. / parami[6];
	cosf0 = cos((double) parami[5]);
	sinf0 = sin((double) parami[5]);
	xc = parami[2] - rtrk * sinf0;
	yc = parami[3] + rtrk * cosf0;
	dphi = parami[6] * rdphi;
	phi1 = fmod(parami[5] + dphi, 2*M_PI);
	cosf1 = cos(phi1);
	sinf1 = sin(phi1);
	x1 = xc + rtrk * sinf1;
	y1 = yc - rtrk * cosf1;
	r1 = sqrt(x1 * x1 + y1 * y1);

/*   intersection outside of limits in r,x,y */
	if (r1 < *rmin || r1 > *rmax) {
	    *ierr = 3;

/*   parameters at the intersection */
	} else {
	    paramf[1] = *zf;
	    paramf[2] = x1;
	    paramf[3] = y1;
	    paramf[4] = parami[4];
	    paramf[5] = phi1;
	    if (paramf[5] < 0.) paramf[5] += 2*M_PI;
	    paramf[6] = parami[6];

/*   computation of derivatives ----------------------------------- */

	    if (*iopt == 1) {
		ct2inv = (tanth * tanth + 1.) * dz;
		der[1] = ct2inv * cosf1;
		der[4] = ct2inv * sinf1;
		der[7] = parami[6] * ct2inv;
		der[8] = rdphi;

/*   "exact" formulae if |DPHI| > DPHIMN */
		if (fabs(dphi) >= 1e-4) {
		    dcosf = cosf1 - cosf0;
		    dsinf = sinf1 - sinf0;
		    der[2] = rtrk * dcosf;
		    der[3] = rtrk * rtrk * (dphi * cosf1 - dsinf);
		    der[5] = rtrk * dsinf;
		    der[6] = rtrk * rtrk * (dphi * sinf1 + dcosf);

/*   first order in 1/R if |DPHI| < DPHIMN */
		} else {
		    der[2] = -rdphi * sinf0;
		    der[3] = rdphi * .5 * der[2];
		    der[5] = rdphi * cosf0;
		    der[6] = rdphi * .5 * der[5];
		}
	    }
	}
    }

    return;
}



//-------------------------------------------------------------------------



/* *******************************************************************
                                                                   
              TMSCAT (float *SINTH,float *PINV,float *XRL,float *WG,float *dir)
                                                                     
							   
 * Transformation of weight matrix to account for multiple scatt.    *
*   at a given point                                                  *
*                                                                     *
*   Input  :  SINTH  :  sin(theta)                                    *
*             PINV   :  1/p                                           *
*             XRL    :  number of rad. lengths crossed at this point  *
*             WG     :  initial weight matrix                         *
*             dir    :  direction of update (+1. = add a hit,         *
*                                            -1. = subtract a hit)    *
*                                                                     *
*   Output :  WG     :  final weight matrix                           *
*                                                                     *
***********************************************************************
*/

void TrigInDetSctKFitter::tmscat(double *sinth, double *pinv, double *xrl, double *wg, 
			double *dir)
{  //local variable

      double  th2, xtt, xtb, xbb, q, ubb, utb, utt, s13, s14, s23, s24;
      double  s33, s34, s43, s44, s53, s54, wg4, wg5,wg9, wg13; 
      
      /* Parameter adjustments*/
      
      --wg;
/*
*   mean squared scattering angle in a direction perpendicular to
*   the trajectory (assuming large ratio momentum/mass)
*/
      th2 = 0.0136*(*pinv)*(1.+0.038*log(*xrl));
      th2 = fabs(th2*th2*(*xrl))*(*dir);
    
     // matrix (2x2)  X = (cov.m.s.(theta,beta))**(-1) + TH2 * WG(theta,beta)
      xtt = 1.+ th2*wg[6];
      xtb = th2*wg[9];
      xbb = (*sinth)*(*sinth) + th2*wg[10];

     //matrix  U = TH2 * X**(-1)
      q=th2/(xbb*xtt - xtb*xtb);
      ubb=q*xtt;
      utb=-q*xtb;
      utt=q*xbb;

      //computation of  WG - WG * (U ext. to 5x5) * WG

      s13    = -wg[ 4]*utt - wg[ 7]*utb;
      s14    = -wg[ 4]*utb - wg[ 7]*ubb;
      s23    = -wg[ 5]*utt - wg[ 8]*utb;
      s24    = -wg[ 5]*utb - wg[ 8]*ubb;
      s53    = -wg[13]*utt - wg[14]*utb;
      s54    = -wg[13]*utb - wg[14]*ubb;
      wg[ 1] =  wg[ 1] + s13*wg[ 4] + s14*wg[ 7];
      wg[ 2] =  wg[ 2] + s13*wg[ 5] + s14*wg[ 8];
      wg[11] =  wg[11] + s13*wg[13] + s14*wg[14];
      wg[ 3] =  wg[ 3] + s23*wg[ 5] + s24*wg[ 8];
      wg[12] =  wg[12] + s23*wg[13] + s24*wg[14];
      wg[15] =  wg[15] + s53*wg[13] + s54*wg[14];

      s33    =  1.-wg[6]*utt-wg[ 9]*utb;
      s34    =    -wg[6]*utb-wg[ 9]*ubb;
      s43    =    -wg[9]*utt-wg[10]*utb;
      s44    =  1.-wg[9]*utb-wg[10]*ubb;
      wg4    =  s33*wg[ 4] + s34*wg[ 7];
      wg5    =  s33*wg[ 5] + s34*wg[ 8];
      wg[ 6] =  s33*wg[ 6] + s34*wg[ 9];
      wg9    =  s33*wg[ 9] + s34*wg[10];
      wg13   =  s33*wg[13] + s34*wg[14];
      wg[ 7] =  s43*wg[ 4] + s44*wg[ 7];
      wg[ 8] =  s43*wg[ 5] + s44*wg[ 8];
      wg[10] =  s43*wg[ 9] + s44*wg[10];
      wg[14] =  s43*wg[13] + s44*wg[14];
      wg[ 4] =  wg4;
      wg[ 5] =  wg5;
      wg[ 9] =  wg9;
      wg[13] =  wg13;

      return;
}



//-------------------------------------------------------------------------



// To add the energy loss effect in:-----------------------------------------
//									                                        *
// Input: x0  -- the material budget associated at a point in the unit of   *
//               radiation lengths;                                         *
//        dir -- direction of the reconstruction process                    *
//               (1. for going inwards, -1. for going outwards)             *
//    									                                    *
// Input and output: par6   -- the 6th track parameter, i.e. 1/Rtr          *
//    		     wg[15] -- weight matrix			                	    *
//    									                                    *
// Authors: S. Qian							                                *
//									                                        *
// Date: Nov 16, 1999							                            *
//--------------------------------------------------------------------------*

void TrigInDetSctKFitter::add_E_loss(double *x0, double *dir, double *par6, double *wg) {

  double q55, tem;
  q55 = (*dir)*(*par6)*(*par6)*(*x0)*(0.415-0.744*(*x0));
  if(q55==0.) return;
  tem = 1./q55+wg[14];
  if(tem==0.) {return;} else {tem=1./tem;}
  *par6 *= (1. - (*dir)*(*x0)*(1.-0.5*(*x0)));
  wg[0] -= tem*wg[10]*wg[10];
  wg[1] -= tem*wg[10]*wg[11];
  wg[2] -= tem*wg[11]*wg[11];
  wg[3] -= tem*wg[10]*wg[12];
  wg[4] -= tem*wg[11]*wg[12];
  wg[5] -= tem*wg[12]*wg[12];
  wg[6] -= tem*wg[10]*wg[13];
  wg[7] -= tem*wg[11]*wg[13];
  wg[8] -= tem*wg[12]*wg[13];
  wg[9] -= tem*wg[13]*wg[13];
  wg[10] -= tem*wg[10]*wg[14];
  wg[11] -= tem*wg[11]*wg[14];
  wg[12] -= tem*wg[12]*wg[14];
  wg[13] -= tem*wg[13]*wg[14];
  wg[14] -= tem*wg[14]*wg[14];
}



//-------------------------------------------------------------------------



 void TrigInDetSctKFitter::trf2xy(long *idir, double *r, double *phi, double *wm) const
{
  /* transform weight matrix from (r,phi) to (x.y) (when idir= 1)
                             from (x,y) to (r,phi) (when idir= -1)
			     
     input: idir --- direction of the tranformation
            r    --- in (cm)
	    phi  --- in (radian)
     input and output: wm (15)

     Auther: Sijin Qian, 4/1994
  */

  double d11, d21, d12, d22, wm1[12];

  // d21 = sin(*phi) * pow((-(*r)), ((1-(*idir))/2));
  // d22 = d11/r**(idir)

  d11 = cos(*phi);
  if((*idir) > 0) {
    d21 = sin(*phi);
    d22 = d11/(*r);
  } else {
    d21 = - sin(*phi) * (*r);
    d22 = d11*(*r);
  }
  d12 = -d21/(*r);

  for (long i=0; i<12; i++) {wm1[i]=wm[i];}

  wm[ 0] = d11*d11*wm1[ 0] +        2.*d11*d12*wm1[1] + d12*d12*wm1[2];
  wm[ 1] = d11*d21*wm1[ 0] + (d12*d21+d11*d22)*wm1[1] + d12*d22*wm1[2];
  wm[ 2] = d21*d21*wm1[ 0] +        2.*d21*d22*wm1[1] + d22*d22*wm1[2];
  wm[ 3] =     d11*wm1[ 3] +               d12*wm1[4];
  wm[ 4] =     d21*wm1[ 3] +               d22*wm1[4];
  wm[ 6] =     d11*wm1[ 6] +               d12*wm1[7];
  wm[ 7] =     d21*wm1[ 6] +               d22*wm1[7];
  wm[10] =     d11*wm1[10] +               d12*wm1[11];
  wm[11] =     d21*wm1[10] +               d22*wm1[11];

  return ;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::taddpt(double *df, double *dz,   double *wff,   double *wzz, 
		   double *dp, double *wotr, double *achi2, long *ierr)
{
    double r__1, r__2;
    long ier;

/* ------------------------------------------------- */

/* Subroutine for adding a point to a track */

/*Input           : df, dz    -- Deviation of first two of 5 track parameters*/
/*                  wff, wzz  -- Weight on the two parameters */

/* Input and output : wotr(1:15) -- weight matrix of parameters */

/* Output           : dp(1:5)   -- Adjustment on track parameters */
/*                    achi2     -- Incremental of Chi2 by adding this point*/
/*                    ierr      -- error code (0 = OK) */
/*                                            (1 = Chi2 increase too much)*/
/*                                            (2 = error in TSOLVE) */
/* ------------------------------------------------- */

    /* Parameter adjustments */
    --wotr;
    --dp;

    /* Function Body */

/* To add the weight of the point to the weght matrix */
/*   and to solve the linear system */

    wotr[1] += *wff;
    wotr[3] += *wzz;
		
	if (wotr[1] <= 0. || wotr[3] <= 0.) {
	*ierr = 3;
	goto L20;
    }
    r__1 = *wff * *df;
    r__2 = *wzz * *dz;
    tsolve(&wotr[1], &r__1, &r__2, &dp[1], &ier);
    
	if (ier != 0) {
	*ierr = 2;
	goto L20;
    }
    if (*wff < 0.) {
	return;
    }

/* Increase of chi2 */

    r__1 = *df - dp[1];
    r__2 = *dz - dp[2];

    *achi2 =

/* Contribution of the added point */
 
      *wff * (r__1 * r__1) + *wzz * (r__2 * r__2) 

/* Variation for previous ensembled points */

      + (wotr[1] - *wff) * (dp[1] * dp[1]) 
      + (2. *  wotr[2] * dp[1] + (wotr[3] - *wzz) * dp[2]) * dp[2] 
      + (2. * (wotr[4] * dp[1] + wotr[5] * dp[2]) + wotr[6] * dp[3]) * dp[3] 
      + (2. * (wotr[7] * dp[1] + wotr[8] * dp[2]  + wotr[9] * dp[3]) 
	                                         + wotr[10] * dp[4]) * dp[4] 
      + (2. * (wotr[11] * dp[1]  + wotr[12] * dp[2] + wotr[13] * dp[3] + 
	       wotr[14] * dp[4]) + wotr[15] * dp[5]) * dp[5];

/* chi2 cut */

    *ierr = (*achi2 <= maxDChi2[paramset]) ? 0 : 1;
	
/* In the failure case, restore the previous weight matrix */

L20:
	
/*      IF(IERR.NE.0.AND.WFF.GT.0.) THEN */
    if (*ierr != 0) {
	wotr[1] -= *wff;
	wotr[3] -= *wzz;
    }

    return;
}



//-------------------------------------------------------------------------



void TrigInDetSctKFitter::tsolve(double *w, double *vy, double *vz, double *d, long *ier)
{
    /* Local variables */
    double  q, r11, r12, r13, r14, r22, r23, r24, r33, r34, r44, tem, 
            piv3, piv4, piv5;

/* ---------------------------------------------------------- */
/* Subroutine to solve a linear system */
/* ---------------------------------------------------------- */

    /* Parameter adjustments */
    --d;
    --w;

    /* Function Body */
    *ier = 0;
    piv5 = 1. / w[15];
    q = w[11] * piv5;
    r11 = w[1] - q * w[11];
    r12 = w[2] - q * w[12];
    r13 = w[4] - q * w[13];
    r14 = w[7] - q * w[14];
    q = w[12] * piv5;
    r22 = w[3] - q * w[12];
    r23 = w[5] - q * w[13];
    r24 = w[8] - q * w[14];
    q = w[13] * piv5;
    r33 = w[6] - q * w[13];
    r34 = w[9] - q * w[14];
    r44 = w[10] - w[14] * w[14] * piv5;
    if (r44 == 0.) {
	*ier = 1;
	return;
    }
    piv4 = 1. / r44;
    q = r14 * piv4;
    r11 -= q * r14;
    r12 -= q * r24;
    r13 -= q * r34;
    q = r24 * piv4;
    r22 -= q * r24;
    r23 -= q * r34;
    r33 -= r34 * r34 * piv4;
    if (r33 == 0.) {
	*ier = 1;
	return;
    }
    piv3 = 1. / r33;
    q = r13 * piv3;
    r11 -= q * r13;
    r12 -= q * r23;
    r22 -= r23 * r23 * piv3;
    tem = r11 * r22 - r12 * r12;
    if (tem == 0.) {
	*ier = 1;
	return;
    }
    q = 1. / tem;
    d[1] = q * ( r22 * *vy - r12 * *vz);
    d[2] = q * (-r12 * *vy + r11 * *vz);
    d[3] = -piv3 * (r13 * d[1] + r23 * d[2]);
    d[4] = -piv4 * (r14 * d[1] + r24 * d[2] + r34 * d[3]);
    d[5] = -piv5 * (w[11] * d[1] + w[12] * d[2] + w[13] * d[3] + w[14] * d[4]);

    return;
}


