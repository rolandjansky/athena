/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigInDetSctKFitter.h
// 
// authors:   original SctKFitting code
//               Patrice LEBRUN <lebrun@in2p3.fr>
//               Sijin QIAN <sijin.qian@cern.ch>
//               Thorsten HUEHN <huehn@scri.fsu.edu>
//
//            idScan version
//               Nikos Konstantinidis
//               nk@hep.ucl.ac.uk
//
//            migration to Athena
//               Malte Muller 
//               mm@hep.ucl.ac.uk
//
//	      AlgTool version and migration from IdScan
//               Dmitry Emeliyanov
//               D.Emeliyanov@rl.ac.uk	 
//
// Description: simple Kalman fitting engine
// 
// date: 19/11/2002
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRIGINDETSCTKFITTER_H__
#define __TRIGINDETSCTKFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"

class TrigInDetSctKFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter   
{
 public:
  void fit(TrigInDetTrackCollection* recoTracks );
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis&) {return nullptr;}
  TrigInDetSctKFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetSctKFitter();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
 private:  

  bool m_doBremmCorr;
  bool m_doMultScatt;
  double m_DChi2;

  /////////////////////////////////
  // mostly FORTRAN migrated code
  /////////////////////////////////
  
  // parameters
  long paramset;
  double minTrackRadius[4];
  double maxDChi2[4];
  
  void cyl_prg_convertor( double *pari, double *covi, double *r__, 
			  double *xb, double *yb, 
			  double *par, double *cov, long *ierr );
  
  void matrix_inverter( double *wgt, double *cov, long *ierr );
  void tkffit( long *np, double *rcoord, double *phcoord, double *zcoord, 
	       double *errxy, double *errz, double *errr, long *ihtype, double *rf,
	       double *paraf, double *em1, double *chi2, long *ihdrop, long *ierr,
	       bool electron );
  void tfit3p(double *rco, double *fco, double *zco, double *sigxy, 
	      double *sigz, double *sigr, double *r3, double *para, 
	      double *wgc, double *chi2c, long *ierr);
  void tvtoxx(double *pari, double *wctr, double *par, double *wf);		
  void txprcw(double *wgi, double *der, double *wgf);
  void txprpw(double *wgi, double *der, double *wgf);
  void txxdcy(double *p2, double *p1, double *der);
  void txxdpl(double *p2, double *p1, double *der);
  void tswtch(long *iz,  double *p1, double *wm1);

  void txtrpa(double *parami, long *idir, double *radf,
	      double *zmin, double *zmax, double *sinbmx, 
	      long *iopt, double *paramf, double *der, 
	      double *alrphi, long *ierr);
  
  void txxpla(double *parami, long *idir, double *zf, double *rmin, 
	      double *rmax, long *iopt, double *paramf, 
	      double *der, double *alrphi, long *ierr);
  
  void tmscat(double *sinth, double *pinv, double *xrl, double *wg, 
	      double *dir);
  
  void add_E_loss(double *x0, double *dir, double *par6, double *wg);
  
  void trf2xy(long *idir, double *r, double *phi, double *wm) const;
  
  void taddpt(double *df, double *dz,   double *wff,   double *wzz, 
	      double *dp, double *wotr, double *achi2, long *ierr);
  
  void tsolve(double *w, double *vy, double *vz, double *d, long *ier);
};

#endif

