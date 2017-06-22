/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ListItem.h"
#include "MdtSegmentT0Fitter/MdtSegmentT0Fitter.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"
//#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MuonCalibTools/IdToFixedIdTool.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/MdtRtRelation.h"



#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonPrepRawData/MdtPrepData.h"

#include <iostream>
#include <fstream>

#include "TTree.h"
#include "TROOT.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "TMinuit.h"
// tdc count bin size -- seems to be unused -> commenting for the moment as it would be wrong
// #define TDCBINSIZE 0.78125   //1tdc=0.78125ns; 1tdc=0.1953125ns for BMG!

// number of fit parameters
#define NUMPAR 3

// prints a message if a radius is bigger than this
#define MAX_RAD 16

// time corresponding to r=15 mm for internal rt
#define TUBE_TIME 757.22

#define MAX_DRIFT 855

// garbage time value to return when radius isn't wihin rt range
#define R2TSPURIOUS 50000

#define WEAK_TOPO_T0ERROR 10
#define STRONG_TOPO_T0ERROR 50


namespace TrkDriftCircleMath {
  
  bool use_hardcoded;
  bool use_shift_constraint;
  double constrainT0Error;
	
//	int count;
    
  MdtSegmentT0Fitter::MdtSegmentT0Fitter(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
    DCSLFitter(), 
    m_calibDbSvc("MdtCalibrationDbSvc", na),
    m_ntotalCalls(0),
    m_npassedNHits(0),
    m_npassedSelectionConsistency(0),
    m_npassedNSelectedHits(0),
    m_npassedMinHits(0),
    m_npassedMinuitFit(0)
  {
    declareInterface <IDCSLFitProvider> (this);
    declareProperty("MinimumHits", m_minHits = 4);
    declareProperty("DumpToFile", m_dumpToFile = false);
    declareProperty("UseInternalRT", m_useInternalRT = false);
    declareProperty("TraceOperation", m_trace = false);
    declareProperty("DumpNoFit", m_dumpNoFit = false);
    declareProperty("ConstrainShifts", m_constrainShifts = false);
    declareProperty("ConstrainT0Error", m_constrainT0Error = 10.);
    declareProperty("RejectWeakTopologies",  m_rejectWeakTopologies = true);
    declareProperty("RescaleErrors",m_scaleErrors = true );
    declareProperty("PropagateErrors",m_propagateErrors = true );
  }
  
  
  MdtSegmentT0Fitter::~MdtSegmentT0Fitter(){}
  
  StatusCode MdtSegmentT0Fitter::initialize() {
    ATH_CHECK ( AthAlgTool::initialize() );

    use_hardcoded = m_useInternalRT;
    use_shift_constraint = m_constrainShifts;
    constrainT0Error = m_constrainT0Error;
//		count = 0;
    
        
    // Get pointer to MdtCalibrationDbSvc and cache it :
    ATH_CHECK ( m_calibDbSvc.retrieve() );

    TMinuit* oldMinuit = gMinuit;
    m_minuit = new TMinuit(3);
    m_minuit->SetPrintLevel(-1); // -1: no output, 1: std output
    if( msgLvl(MSG::VERBOSE) ) m_minuit->SetPrintLevel(1);
    gMinuit = oldMinuit;

    return StatusCode::SUCCESS;
  }
  
  StatusCode MdtSegmentT0Fitter::finalize() {
    
    double scaleFactor = m_ntotalCalls != 0 ? 1./(double)m_ntotalCalls : 1.;

    ATH_MSG_INFO( "Summarizing fitter statistics " << "\n"
                  << " Total fits       " << std::setw(10) << m_ntotalCalls << "   " << scaleFactor*m_ntotalCalls << "\n"
                  << " hits > 2         " << std::setw(10) << m_npassedNHits << "   " << scaleFactor*m_npassedNHits << "\n"
                  << " hit consis.      " << std::setw(10) << m_npassedSelectionConsistency << "   " << scaleFactor*m_npassedSelectionConsistency << "\n"
                  << " sel. hits > 2    " << std::setw(10) << m_npassedNSelectedHits << "   " << scaleFactor*m_npassedNSelectedHits << "\n"
                  << " Hits > min hits  " << std::setw(10) << m_npassedMinHits << "   " << scaleFactor*m_npassedMinHits << "\n"
                  << " Passed Fit       " << std::setw(10) << m_npassedMinuitFit << "   " << scaleFactor*m_npassedMinuitFit  );
    if( gMinuit == m_minuit ) gMinuit = 0;
    delete m_minuit;
    return AthAlgTool::finalize(); 
  }
  
  
  /***********************************************************************************/
  /// RT function from Craig Blocker
  /// ok for now, possibly replace with actual RT function used to calibrate run
  
  static double t2r_a[10] = {1.184169e-1, 3.32382e-2, 4.179808e-4, -5.012896e-6, 2.61497e-8, -7.800677e-11, 1.407393e-13, -1.516193e-16, 8.967997e-20, -2.238627e-23};
  static double rcorr_a[] = {234.3413, -5.803375, 5.061677e-2, -1.994959e-4, 4.017433e-7, -3.975037e-10, 1.522393e-13};
  
  double rcorr(double tin) {
    double rc;
    if(tin < 16.) {
      rc = -50.;
    } else {
      double tpow = 1.;
      rc = 0.;
      for(int j = 0; j < 7; j++) {
        rc += rcorr_a[j]*tpow;
        tpow *= tin;
      }
    }
    rc = rc*1e-3;
    
    return rc;
  }
  
  double t2r(double tin) {
    if(tin < 0.) return 0.;
    if(tin > MAX_DRIFT) return 20.;
    
    double tpow = 1.;
    double rc = 0.;
    for(int j = 0; j < 10; j++) {
      rc += t2r_a[j]*tpow;
      tpow *= tin;
    }
    
    rc -= rcorr(tin);
    return rc;
  }
  
  /// derivatives of RT function, use to get errors
  double rcorrprime(double tin) {
    double rc;
    if(tin < 16.) {
      rc = 0.;
    } else {
      double tpow = 1.;
      rc = 0.;
      for(int j = 1; j < 7; j++) {
        rc += j*rcorr_a[j]*tpow;
        tpow *= tin;
      }
    }
    rc = rc*1e-3;
    
    return rc;
  }
  
  double t2rprime(double tin) {
    if(tin < 0.) return 0.;
    if(tin > MAX_DRIFT) return 20.;
    
    double tpow = 1.;
    double rc = 0.;
    for(int j = 1; j < 10; j++) {
      rc += j*t2r_a[j]*tpow;
      tpow *= tin;
    }
    
    rc -= rcorrprime(tin);
    return rc;
  }
  
  
  /// use a binary search to get rt-inverse from rt
  /// assumes the function is monotonic, obviously not true for these polynomial parametrizations for all t
  double r2t(double r) {
    double ta = 0;
    double tb = MAX_DRIFT;
    if(r<t2r(ta) ) {
      return -1*R2TSPURIOUS;
    } else if(r>t2r(tb)) {
      return R2TSPURIOUS;
    }
    
    int itr = 0;
    while (ta <= tb) {
      double tm  = (ta + tb) / 2;  // compute mid point.
      double rtm = t2r(tm);
      if(fabs(rtm - r) < 0.001 ) {
        return tm;
      }
      else if (r > rtm) {
        ta = tm;  // repeat search in top half.
      }
      else if (r < rtm ) {
        tb = tm; // repeat search in bottom half.
      }
      
      itr++;
      if(itr>50) return -1;
    }
    return -1;    // failed to find key
  }
  
  double r2t_ext(   std::vector<const MuonCalib::IRtRelation*> *rtpointers, double r, int i) {
    const MuonCalib::IRtRelation* rtrel = rtpointers->at(i);
    //  double ta = 0;
    // double tb = 
    
    double ta = rtrel->tLower();
    double tb = rtrel->tUpper();
    if(r<rtrel->radius(ta) ) {
      return -1*R2TSPURIOUS;
    } else if(r>rtrel->radius(tb)) {
      return R2TSPURIOUS;
    }
    
    int itr = 0;
    while (ta <= tb) {
      double tm  = (ta + tb) / 2;  // compute mid point.
      double rtm = rtrel->radius(tm);
      if(fabs(rtm - r) < 0.001 ) {
        return tm;
      }
      else if (r > rtm) {
        ta = tm;  // repeat search in top half.
      }
      else if (r < rtm ) {
        tb = tm; // repeat search in bottom half.
      }
      
      itr++;
      if(itr>50) return -1;
    }
    return -1;    // failed to find key
  }
  
  
  /// data to pass to fcn
  typedef struct {
    double z;
    double t;
    double y;
    double w;
    double r;
    const MuonCalib::IRtRelation *rt;
  } hitcoords;
  
  std::vector<hitcoords> data;
  double t_lo, t_hi;
  int used;
  int t0Error;
  /// the fit function
  /// gets distance between the line and the hit (in the frame whose direction is given by the line segment
  /// and position is given by the weighted average of all the hits), subtracts off the radius from the rt relation etc 
  void mdtSegmentT0Fcn(Int_t &/*npar*/, Double_t* grad, Double_t &fval, Double_t* par, Int_t jflag) {
    if(jflag < 0) grad[0] = 0.;
    
    double ang = par[0];  
    double b = par[1];
    double t0 = par[2];
    
    double cosin = cos(ang);
    double sinus = sin(ang);
//    if ( sinus < 0.0 ) {
//      sinus = -sinus;
//      cosin = -cosin;
//    } else if ( sinus == 0.0 && cosin < 0.0 ) {
//      cosin = -cosin;
//    }
    
    fval = 0.;
    // Add t0 constraint 
    if(use_shift_constraint) {
     fval += par[2]*par[2]/(constrainT0Error*constrainT0Error);
    } else if (t0Error == WEAK_TOPO_T0ERROR ) {
     fval += par[2]*par[2]/(1.0 *t0Error*t0Error);
    }
    double t, r, z, y, w, dist;
    for(int i = 0; i < used ; i++) {
      t = data[i].t - t0;
      z = data[i].z;
      y = data[i].y;
      w = data[i].w;
      dist = fabs(b*cosin + z*sinus - y*cosin); // same thing as fabs(a*z - y + b)/sqrt(1. + a*a);
      double uppercut = use_hardcoded ? TUBE_TIME : data[i].rt->tUpper();
      double lowercut = use_hardcoded ? 0 : data[i].rt->tLower();
// Penalty for t<lowercut and t >uppercut
      if (t> uppercut ) { // too large 
	fval += (t-uppercut)* (t-uppercut)*0.1;
      }else if (t < 0 ) {// too small
	fval += (t-lowercut)*(t-lowercut)*0.1;
      }
      if(use_hardcoded) {
	  if(t<0) r=0;
	  else r = t2r(t);
      } else {
	  if(t<lowercut) r =  data[i].rt->radius(lowercut);
          else if(t>uppercut)  r =  data[i].rt->radius(uppercut);
	  else r = data[i].rt->radius(t);
      }
      fval += (dist - r)*(dist - r)*w;
      
    }
    
    return;
  }
  /***********************************************************************************/
  
  
  int sign(double a) {
    if(a>0) return 1;
    if(a<0) return -1;
    return 0;
  }
  bool MdtSegmentT0Fitter::fit( const Line& line, const DCOnTrackVec& dcs, const HitSelection& selection, double t0Seed ) const {

    ++m_ntotalCalls;

    if(m_trace) ATH_MSG_DEBUG( "New seg: "  );
 
    const DCOnTrackVec& dcs_keep = dcs;

    unsigned int N = dcs_keep.size();
    used=0;
    m_result.setT0Shift(-99999,-99999);
    
    if(N<2) {
      return false;
    }
    ++m_npassedNHits;
    if( selection.size() != N ) {
      ATH_MSG_ERROR( "MdtSegmentT0Fitter.cxx:fit with t0 <bad HitSelection>"  );
      return false;
    }  
    ++m_npassedSelectionConsistency;
    for(unsigned int i=0;i<N;++i){
      if( selection[i] == 0 ) ++used;
      //      if(m_trace) *m_log << MSG::DEBUG << " selection flag " <<  selection[i] << endmsg;
    }
    if(used < 2){
      if(m_trace) ATH_MSG_DEBUG( "TOO FEW HITS SELECTED"  );
      return false;
    }
    ++m_npassedNSelectedHits;
    if(used < m_minHits) {
      if(m_trace) ATH_MSG_DEBUG( "FEWER THAN Minimum HITS N " << m_minHits << " total hits " <<N<<" used " << used  );

      //
      //     Copy driftcircles and reset the drift radii as they might have been overwritten
      //     after a succesfull t0 fit 
      // 

      DCOnTrackVec::const_iterator it = dcs.begin();
      DCOnTrackVec::const_iterator it_end = dcs.end();
      
      DCOnTrackVec dcs_new;
      dcs_new.reserve(dcs.size());
      double chi2p = 0.;
      for(int i=0; it!=it_end; ++it, ++i ){
	const DriftCircle* ds  = & dcs[i];
        if(fabs(ds->r()-ds->rot()->driftRadius())>0.1&&m_trace) std::cout << " Different radii on dc " << ds->r() << " rot " << ds->rot()->driftRadius() << std::endl;  
	DriftCircle dc_keep(ds->position(), ds->rot()->driftRadius(), ds->dr(), ds->drPrecise(), ds->state(), ds->id(), ds->index(),ds->rot() );
	DCOnTrack dc_new(dc_keep, 0., 0.);
	dc_new.state(dcs[i].state());
	dcs_new.push_back( dc_new );
        if( selection[i] == 0 ){
          double t = ds->rot()->driftTime();
          const MuonCalib::MdtRtRelation *rtInfo = m_calibDbSvc->getRtCalibration(ds->rot()->identify());
          double tUp = rtInfo->rt()->tUpper();
          double tLow = rtInfo->rt()->tLower();
          if(t<tLow) chi2p += (t-tLow)*(t-tLow)*0.1;
          if(t>tUp) chi2p += (t-tUp)*(t-tUp)*0.1;
        }
      }
      if(m_trace&&chi2p>0) std::cout << " NO Minuit Fit TOO few hits Chi2 penalty " << chi2p << std::endl;
      bool oldrefit = DCSLFitter::fit( line, dcs_new, selection ); 
      chi2p += m_result.chi2();
// add chi2 penalty for too large or too small driftTimes  t < 0 or t> t upper
      m_result.set( chi2p, m_result.ndof(),  m_result.dtheta(),  m_result.dy0() );
      int iok = 0;
      if(oldrefit) iok = 1;
      if(m_trace) std::cout << " chi2 total " << m_result.chi2() << " angle " << m_result.line().phi() << " y0 " << m_result.line().y0()  << " nhits "<< selection.size() << " refit ok " << iok << std::endl;
      return oldrefit;
    } else {
      if(m_trace) ATH_MSG_DEBUG( "FITTING FOR T0 N "<<N<<" used " << used  );
    } 

    ++m_npassedMinHits;

    
    if (m_trace) ATH_MSG_DEBUG(" in  MdtSegmentT0Fitter::fit with N dcs "<< N << " hit selection size " <<  selection.size()  );
    if(m_trace) ATH_MSG_DEBUG("in fit "<<m_result.hasT0Shift()<< " " <<m_result.t0Shift() );
    
    
    double Zc=0, Yc=0, S=0, Sz=0, Sy=0;
    std::vector<double> y(N);
    std::vector<double> z(N);
    std::vector<double> w(N); 
    std::vector<double> r(N);
    std::vector<double> dr(N);
    std::vector<double> t(N);
    //   std::vector<double> driftt(N);
    std::vector<const MuonCalib::IRtRelation*> rtpointers(N);
    
    // allocate memory for data
    // allocate memory for data
    if( data.capacity() != 50 ) data.reserve(50);
    data.resize(used);
    {
      DCOnTrackVec::const_iterator it = dcs_keep.begin();
      DCOnTrackVec::const_iterator it_end = dcs_keep.end();
      for(int ii=0 ;it!=it_end; ++it, ++ii ){
        const Muon::MdtDriftCircleOnTrack *roto = it->rot();
        if (!roto) {
          ATH_MSG_ERROR(" MdtSegmentT0Fitter: NO DC ROT pointer found "  );
          return false;
        }
        ATH_MSG_DEBUG( "hit # "<<ii );
        y[ii] = it->y();
        z[ii] = it->x();
        r[ii] = std::abs( roto->driftRadius() );
        dr[ii] = it->dr(); 
        const Muon::MdtPrepData *peerd;
        peerd = dynamic_cast<const Muon::MdtPrepData*>( roto->prepRawData() );
        if(!peerd) {
          ATH_MSG_ERROR("MdtSegmentT0Fitter: Can't convert to MdtPrepData* !! Not fitting for t0"  );
          return false;
        }
        Identifier id = roto->identify();
        const MuonCalib::MdtRtRelation *rtInfo = m_calibDbSvc->getRtCalibration(id);
        rtpointers[ii] = rtInfo->rt();
        t[ii] = roto->driftTime();

	double newerror = m_scaleErrors ? it->drPrecise() : it->dr();        

        if( newerror > 0.) w[ii] = 1./(newerror);
        else w[ii] = 0.;
        w[ii]*=w[ii];
        if(r[ii]<0){
          r[ii] = 0.;
          if(m_trace) ATH_MSG_DEBUG( "MdtSegmentT0Fitter (using times) ERROR: <Negative r> " << r[ii]  );
        }

        if(m_trace) ATH_MSG_DEBUG( "DC:  (" << y[ii] << "," << z[ii] << ") R = " << r[ii] << " W " << w[ii] <<" t " <<t[ii]<< " id: "<<it->id()<<" sel " << selection[ii]  );
        
        if( selection[ii] == 0 ){
          S+=w[ii];
          Sz+= w[ii]*z[ii];
          Sy+= w[ii]*y[ii];
          if(r[ii] > MAX_RAD ) {
            if(m_trace) ATH_MSG_DEBUG("Radius is too big"  );
          }
        }
      }
    }
    const double inv_S = 1. / S;
    Zc = Sz*inv_S;
    Yc = Sy*inv_S;
    
    if(m_trace) ATH_MSG_DEBUG( "Yc " << Yc << " Zc " << Zc  );
    
    /// go to coordinates centered at the average of the hits
    for(unsigned int i=0;i<N;++i) {
      y[i]  -= Yc;
      z[i]  -= Zc;
    }
    
    int selcount(0);
    DCOnTrackVec::const_iterator it = dcs_keep.begin();
    DCOnTrackVec::const_iterator it_end = dcs_keep.end();
    
    t_lo = 1e10;
    t_hi = -1e10;
    // replicate for the case where the external rt is used...
    // each hit has an rt function with some range...we want to fit such that
    // tlower_i < ti - t0 < tupper_i
    double min_tlower=1e10;
    double max_tupper=-1e10;
    
    double t0seed=0; // the average t0 of the hit
    double st0 = 0; // the std deviation of the hit t0s
    double min_t0 = 1e10; // the smallest t0 seen
    double tee0, tl, th;
    
    
    for(int ii=0 ;it!=it_end; ++it, ++ii ){
      if( selection[ii] == 0 ) {
        data[selcount].z = z[ii];
        data[selcount].y = y[ii];
        data[selcount].r = r[ii];
        data[selcount].w = w[ii];
        data[selcount].rt = rtpointers[ii];
        double r2tval;
        if(use_hardcoded) {
          r2tval = r2t(r[ii]);
        } else {
          r2tval = r2t_ext(&rtpointers,  r[ii], ii) ;
          tl = rtpointers[ii]->tLower();
          th = rtpointers[ii]->tUpper();
          if(t[ii] - tl < min_tlower) min_tlower = t[ii] - tl;
          if(t[ii] - th > max_tupper) max_tupper = t[ii] - th;
        }
        tee0 = t[ii] - r2tval;

        if(m_trace) {
          msg() << MSG::DEBUG <<" z "<<z[ii]
                 <<" y "<<y[ii]
                 <<" r "<<r[ii]
                 <<" t "<<t[ii]
                 <<" t0 "<<tee0;
          if(!use_hardcoded) {
            msg() << MSG::DEBUG <<" tLower "<<tl;
            msg() << MSG::DEBUG <<" tUpper "<<th;
          }
          msg() << MSG::DEBUG << endmsg;
        }
        t0seed += tee0;
        st0 += tee0*tee0;
        if(tee0 < min_t0 && fabs(r2tval) < R2TSPURIOUS) min_t0 = tee0;
        
        data[selcount].t = t[ii];
        if(t[ii]< t_lo) t_lo = t[ii];
        if(t[ii] > t_hi) t_hi = t[ii];
        selcount++;
      } 
    }
    t0seed /= selcount;
    st0 = st0/selcount - t0seed*t0seed;
    st0 = st0 > 0. ? sqrt(st0) : 0.;
    t_hi -= MAX_DRIFT;
    
    if(!use_hardcoded) {
      t_hi = max_tupper;
      t_lo = min_tlower;
    }
    
    if(m_trace) ATH_MSG_DEBUG("t_hi "<<t_hi<<" t_lo "<<t_lo<<" t0seed "<<t0seed<<" sigma "<<st0<< " min_t0 "<<min_t0 );
    if(t_hi > t_lo ) {
      if(m_dumpNoFit) {
        std::ofstream gg;
        gg.open("fitnotdone.txt", std::ios::out | std::ios::app);
        DCOnTrackVec::const_iterator it = dcs_keep.begin();
        DCOnTrackVec::const_iterator it_end = dcs_keep.end();
        for(int ii=0 ;it!=it_end; ++it, ++ii ){
          const Muon::MdtDriftCircleOnTrack *roto = it->rot();
          const Muon::MdtPrepData *peerd;
          if(!roto) continue; 
          peerd = dynamic_cast<const Muon::MdtPrepData*>( roto->prepRawData() );
          if(!peerd) continue; 
          Identifier id = roto->identify();
          gg<<id;
          gg<<" z "<<z[ii];
          gg<<" y "<<y[ii];
          gg<<" r "<<r[ii];
          gg<<" t "<<t[ii];
          gg<<" t0 "<<t[ii] - r2t_ext(&rtpointers,  r[ii], ii);
          gg<<" adc "<<peerd->adc();
          gg<<" tdc "<<peerd->tdc();
          gg<<*it;
          gg<<" sel "<<selection[ii];
          gg<< std::endl;
        }
        gg<<"--------------\n";
      }
    }
    
    
    // ************************* seed the parameters
    double theta = line.phi();
    double cosin = cos(theta);
    double sinus = sin(theta);
    
    if ( sinus < 0.0 ) {
      sinus = -sinus;
      cosin = -cosin;
    } else if ( sinus == 0.0 && cosin < 0.0 ) {
      cosin = -cosin;
    }
    if(m_trace) ATH_MSG_DEBUG("before fit theta "<<theta<<" sinus "<<sinus<< " cosin "<< cosin  );
    
    double d = line.y0() + Zc*sinus-Yc*cosin;
    
    if(m_trace) {
      ATH_MSG_DEBUG(" line x y "<<line.position().x()<<" "<<line.position().y() );
      ATH_MSG_DEBUG(" Zc Yc "<< Zc <<" "<<Yc );
      ATH_MSG_DEBUG(" line x0 y0 "<<line.x0()<<" "<<line.y0() );
      ATH_MSG_DEBUG(" hit shift " << -Zc*sinus+Yc*cosin  );
    } 

// Calculate signed radii 

    int nml1p = 0;
    int nml2p = 0;
    int nml1n = 0;
    int nml2n = 0;
    double sdist;
    it = dcs_keep.begin();
    for(int ii=0 ;it!=it_end; ++it, ++ii ){
      if( selection[ii] != 0 ) continue;
      sdist = d*cosin + z[ii]*sinus - y[ii]*cosin; // same thing as fabs(a*z - y + b)/sqrt(1. + a*a);
//      if (r[ii] < 1) continue;
      if(it->id().ml()==0&&sdist > 0) nml1p++;
      if(it->id().ml()==0&&sdist < 0) nml1n++;
      if(it->id().ml()==1&&sdist > 0) nml2p++;
      if(it->id().ml()==1&&sdist < 0) nml2n++;
    }

// Define t0 constraint in Minuit 
    t0Error = STRONG_TOPO_T0ERROR;
    if (nml1p+nml2p < 2 || nml1n+nml2n < 2) t0Error = WEAK_TOPO_T0ERROR;

// Reject topologies where in one of the Multilayers no +- combination is present
    if((nml1p<1||nml1n<1)&&(nml2p<1||nml2n<1)&&m_rejectWeakTopologies) {
      if(m_trace) ATH_MSG_DEBUG(" Combination rejected for positive radii ML1 " <<  nml1p << " ML2 " <<  nml2p << " negative radii ML1 " << nml1n << " ML " << nml2n << " used hits " << used << " t0 Error " << t0Error   );
      it = dcs.begin();
      it_end = dcs.end();
      double chi2p = 0.; 
      DCOnTrackVec dcs_new;
      dcs_new.reserve(dcs.size());
      for(int i=0; it!=it_end; ++it, ++i ){
	const DriftCircle* ds  = & dcs[i];
        if(fabs(ds->r()-ds->rot()->driftRadius())>0.1&&m_trace) std::cout << " Different radii on dc " << ds->r() << " rot " << ds->rot()->driftRadius() << std::endl;  
	DriftCircle dc_keep(ds->position(), ds->rot()->driftRadius(), ds->dr(), ds->drPrecise(), ds->state(), ds->id(), ds->index(),ds->rot() );
	DCOnTrack dc_new(dc_keep, 0., 0.);
	dc_new.state(dcs[i].state());
	dcs_new.push_back( dc_new );
        if( selection[i] == 0 ){
          double t = ds->rot()->driftTime();
          const MuonCalib::MdtRtRelation *rtInfo = m_calibDbSvc->getRtCalibration(ds->rot()->identify());
          double tUp = rtInfo->rt()->tUpper();
          double tLow = rtInfo->rt()->tLower();
          if(t<tLow) chi2p += (t-tLow)*(t-tLow)*0.1;
          if(t>tUp) chi2p += (t-tUp)*(t-tUp)*0.1;
        } 
      }
      if(m_trace&&chi2p>0) ATH_MSG_DEBUG( " Rejected weak topology Chi2 penalty " << chi2p  );
      bool oldrefit = DCSLFitter::fit( line, dcs_new, selection ); 
      chi2p += m_result.chi2();
// add chi2 penalty for too large or too small driftTimes  t < 0 or t> t upper
      m_result.set( chi2p, m_result.ndof(),  m_result.dtheta(),  m_result.dy0() );
      return oldrefit;
    }  // end rejection of weak topologies  

    if(m_trace) ATH_MSG_DEBUG(" positive radii ML1 " <<  nml1p << " ML2 " <<  nml2p << " negative radii ML1 " << nml1n << " ML " << nml2n << " used hits " << used << " t0 Error " << t0Error   );
    
//    m_minuit->BuildArrays(3);
    m_minuit->SetFCN(mdtSegmentT0Fcn);
    Double_t arglist[3];
    
    // Set printout level
    arglist[0] = -1;
    // Clear previous fit
    m_minuit->mncler();
    arglist[0] = -1;
    
    Double_t vstart[3];
    vstart[0] = theta;
    // x' = x - xc, y' = y - yc => y' = m x' + b + m xc - yc
    // and b = yc - m xc
    vstart[1] = d ; 
    vstart[2] = 0 ;

    // if t0Seed value from outside use this
    if(t0Seed > -999.)  vstart[2] = t0Seed; 

    Double_t step[3] = {0.01 , 0.01 , 0.1 };
    
    if(m_trace) {
      ATH_MSG_DEBUG("\n____________INITIAL VALUES________________" );
      ATH_MSG_DEBUG("Theta " << theta << "("<<tan(theta)<<") d " << vstart[1]  );
    }
    
    int errFlag = 0;
    int errFlag1 = 0;
    int errFlag2 = 0;
    int errFlag3 = 0;
    int errFlag4 = 0;
    int errFlag5 = 0;
    m_minuit->mnparm(0, "a", vstart[0], step[0], -0.5, 3.5,errFlag);
    m_minuit->mnparm(1, "b", vstart[1], step[1], 0., 0.,errFlag);
    // m_minuit->SetParameter(2, "t0", vstart[2], step[2], t_hi, t_lo,errFlag);
    m_minuit->mnparm(2, "t0", vstart[2], step[2], 0., 0.,errFlag);

    if (errFlag !=0 &&m_trace)  {
      ATH_MSG_DEBUG(  " ALARM with steering of Minuit variable " << errFlag  );
    }
   
    m_minuit->FixParameter(0);
    m_minuit->FixParameter(1);
    m_minuit->mnexcm("MINIMIZE", arglist, 0,errFlag1);

    m_minuit->Release(0);
    m_minuit->Release(1);
    m_minuit->FixParameter(2);
    m_minuit->mnexcm("MINIMIZE", arglist, 0,errFlag2);
    m_minuit->mnexcm("MIGRAD", arglist, 0,errFlag3);
    m_minuit->Release(2);
    m_minuit->mnexcm("MINIMIZE", arglist, 0,errFlag4);
    m_minuit->mnexcm("MIGRAD", arglist, 0,errFlag5);
    if(errFlag5!=0&&errFlag4==0) {
      m_minuit->mnexcm("MINIMIZE", arglist, 0,errFlag4); 
      if (errFlag4 == 0 &&m_trace)  {
        ATH_MSG_DEBUG(  " ALARM Fall back to MINIMIZE " << errFlag4  );
      }
    } 
    
    // Get the chisquare and errors
    double chisq;
    double edm;
    double errdef;
    int npari, nparx, icstat;
    m_minuit->mnstat(chisq, edm, errdef, npari, nparx, icstat);
   
    int fitretval = errFlag5;
    if (npari<0 || nparx < 0 || chisq < 0) ATH_MSG_ERROR(  " MdtSegmentT0Fitter MINUIT problem " << " chisq "<<chisq<<" npari "<<npari<<" nparx "<< nparx <<" fitretval "<< fitretval );

    if (fitretval !=0 &&m_trace)  {
      ATH_MSG_DEBUG(  " ALARM return value " << fitretval  );
    }
    if(m_dumpToFile) {
      std::ofstream ff;
      ff.open("fitres.txt", std::ios::out | std::ios::app);
//      ff << fitretval<<" 0 "<<chisq<<" "<<t_hi<<" "<<t_lo<<" "<<t0seed<<" "<<st0<<" "<<min_t0<<" "<<m_minuit->GetParameter(2)<<" "<<m_minuit->GetParError(2)<< endmsg;
//			TMinut * tm = (TMinuit*) minuit->GetObjectFit();
      ff<<npari<<" "<<nparx<<" "<<fitretval<<" "<<chisq<< " "<< std::endl;
      ff.close();
    }
    if(m_trace) ATH_MSG_DEBUG("chisq "<<chisq<<" npari "<<npari<<" nparx "<<nparx<<" fitretval "<<fitretval );

    // Do standard DCSL fit if minuit is bad 
    // Do standard DCSL fit if all fits from minuit are bad 
    if (errFlag5!=0&&errFlag4!=0&&errFlag3!=0&&errFlag2!=0&&errFlag1!=0)  {
      if(m_trace)ATH_MSG_DEBUG( " ALARM Minimize fix " << errFlag1 << " ALARM minimize release " << errFlag2 
			<< " ALARM migrad fix 1 " << errFlag3 << " ALARM minimize all free" << errFlag4 << " ALARM migrad all free " 
                                << errFlag5  );

      DCOnTrackVec dcs_new;
      dcs_new.reserve(dcs.size());
      DCOnTrackVec::const_iterator it = dcs.begin();
      DCOnTrackVec::const_iterator it_end = dcs.end();
    
      for(int i=0; it!=it_end; ++it, ++i ){
	const DriftCircle* ds  = & dcs[i];
	DriftCircle dc_keep(ds->position(), ds->rot()->driftRadius(), ds->dr(), ds->drPrecise(), ds->state(), ds->id(), ds->index(),ds->rot() );
	DCOnTrack dc_new(dc_keep, 0., 0.);
	dc_new.state(dcs[i].state());
	dcs_new.push_back( dc_new );
      }

      bool oldrefit =  DCSLFitter::fit( line , dcs_new, selection );
      //int iok = 0;
      //if(oldrefit) iok = 1;
      return  oldrefit;
    } 
    ++m_npassedMinuitFit;

    // Get the fit values
    double aret(0),aErr(0);
    m_minuit->GetParameter(0,aret,aErr); // theta returned
    double dtheta = aErr;
    
    double tana = tan(aret); // tangent of angle
    
    double ang = aret;  // between zero and pi
    cosin = cos(ang);
    sinus = sin(ang);
    if ( sinus < 0.0 ) {
      sinus = -sinus;
      cosin = -cosin;
    } else if ( sinus == 0.0 && cosin < 0.0 ) {
      cosin = -cosin;
    }
    ang = atan2(sinus, cosin);
    
    
    double b(0),bErr(0);
    m_minuit->GetParameter(1,b,bErr); // intercept

    
    double t0(0),t0Err(0);
    m_minuit->GetParameter(2,t0,t0Err);
    
    
    //    double y0 = b * cosin ; // parallel distance
    double dy0 = cosin * bErr - b * sinus * aErr;
    
    double del_t;
    if(use_hardcoded) del_t = fabs(t2r((t0+t0Err)) - t2r(t0));
    else del_t = fabs(rtpointers[0]->radius((t0+t0Err)) - rtpointers[0]->radius(t0)) ;
    
    if(m_trace) {
      ATH_MSG_DEBUG("____________FINAL VALUES________________" );
      ATH_MSG_DEBUG("Values: a "<<tana<<" d "<<b * cosin <<" t0 "<<t0 );
      ATH_MSG_DEBUG("Errors: a "<<aErr<<" b "<<dy0 <<" t0 "<<t0Err );
    }
    
    
    d = b * cosin;
    
    
    double covar[3][3];
    m_minuit->mnemat(&covar[0][0],3);  // 3x3
    if(m_trace) {
      msg() << MSG::DEBUG <<"COVAR  ";
      for(int it1=0; it1<3; it1++) {
        for(int it2=0; it2<3; it2++) {
          msg() << MSG::DEBUG <<covar[it1][it2]<<" ";
        }
        msg() << MSG::DEBUG << endmsg;
      }
    }
    
    m_result.dcs().clear();
    m_result.clusters().clear();
    m_result.emptyTubes().clear();

    if(m_trace) ATH_MSG_DEBUG("after fit theta "<<ang<<" sinus "<<sinus<< " cosin "<< cosin  );
    
    double chi2 = 0;
    unsigned int nhits(0);
    double yl;
    
    
    // calculate predicted hit positions from track parameters
    it = dcs_keep.begin();
    it_end = dcs_keep.end();
    ATH_MSG_DEBUG("------NEW HITS------"  );
    
    for(int i=0; it!=it_end; ++it, ++i ){
      double rad, drad;
      
      double uppercut = use_hardcoded ? TUBE_TIME :  rtpointers[i]->tUpper();
      double lowercut = use_hardcoded ? 0 :  rtpointers[i]->tLower();
      if(use_hardcoded) { 
        rad = t2r(t[i]-t0);
        if(t[i]-t0<lowercut) rad = t2r(lowercut);
        if(t[i]-t0>uppercut) rad = t2r(uppercut);
      } else {
        rad = rtpointers[i]->radius(t[i]-t0);
        if(t[i]-t0<lowercut) rad = rtpointers[i]->radius(lowercut);     
        if(t[i]-t0>uppercut) rad = rtpointers[i]->radius(uppercut);     
      } 
  
      drad = 1.0/sqrt(w[i]) ; 
      
      yl = (y[i] -  tana*z[i] - b);
      if(m_trace) {
        ATH_MSG_DEBUG("i "<<i<<" ");
      }
      
      
      double dth = -(sinus*y[i] + cosin*z[i])*dtheta;
      double residuals = fabs(yl)/sqrt((1+tana*tana)) - rad;
      if(m_trace) {
        ATH_MSG_DEBUG(" dth "<<dth<<" dy0 "<<dy0<<" del_t "<<del_t<<" "  );
      }
      
      double errorResiduals = sqrt( dth*dth + dy0*dy0 + del_t*del_t);
      
      // derivatives of the residual 'R'
      double deriv[3];
      // del R/del theta
      double dd = z[i] * sinus + b *cosin - y[i] * cosin;
      deriv[0] = sign(dd) * (z[i] * cosin - b * sinus + y[i] * sinus);
      // del R / del b
      deriv[1] = sign(dd) * cosin ;
      // del R / del t0
      
      if(use_hardcoded) deriv[2] = -1* t2rprime(t[i]-t0);
      else deriv[2] = -1* rtpointers[i]->driftvelocity(t[i]-t0);
      
      double covsq=0;
      for(int rr=0; rr<3; rr++) {
        for(int cc=0; cc<3; cc++) {
          covsq += deriv[rr]*covar[rr][cc]* deriv[cc];
        }
      }
      if(m_trace) {
	ATH_MSG_DEBUG( " covsquared " << covsq  );
	if( covsq < 0. ){
	  for(int rr=0; rr<3; rr++) {
	    for(int cc=0; cc<3; cc++) {
	      double dot = deriv[rr]*covar[rr][cc]* deriv[cc];
	      ATH_MSG_DEBUG( " adding term " << dot << " dev1 " << deriv[rr] << " cov " << covar[rr][cc]
                             << " dev2 " << deriv[cc]  );
	    }
	  }
	}
      }
      covsq = covsq > 0. ? sqrt(covsq) : 0.;
      const DriftCircle* ds  = & dcs_keep[i];
      if (m_propagateErrors) drad = dr[i]; 
      DriftCircle dc_newrad(dcs_keep[i].position(), rad, drad, ds->state(), dcs_keep[i].id(), dcs_keep[i].index(),ds->rot() );
      DCOnTrack dc_new(dc_newrad, residuals, covsq);
      dc_new.state(dcs_keep[i].state());
      
      if(m_trace) ATH_MSG_DEBUG("T0 Segment hit res "<<residuals<<" eres "<<errorResiduals<<" covsq "<<covsq<<" ri " << r[i]<<" ro "<<rad<<" drad "<<drad << " sel "<<selection[i]<< " inv error " << w[i]  );
      
      if( selection[i] == 0 ) {
        ++nhits;
//        chi2 += residuals*residuals*hitwt;
        if (!m_propagateErrors) {
          chi2 += residuals*residuals*w[i];
        } else {
          chi2 += residuals*residuals/(drad*drad);
        }
        if(m_trace) std::cout <<" T0 Segment hit res "<<residuals<<" eres "<<errorResiduals<<" covsq "<<covsq<<" ri " << r[i]<<" radius after t0 "<<rad<<" radius error "<< drad <<  " original error " << dr[i] << std::endl;
// Put chi2 penalty for drift times outside window
        if (t[i]-t0> uppercut ) { // too large 
	  chi2  += (t[i]-t0-uppercut)* (t[i]-t0-uppercut)*0.1;
        }else if (t[i]-t0 < lowercut ) {// too small
	  chi2 += ((t[i]-t0-lowercut)*(t[i]-t0-lowercut))*0.1;
        }
      } 
      m_result.dcs().push_back( dc_new );
    }
    
    double oldshift;
    oldshift = m_result.t0Shift();
    if(m_trace) ATH_MSG_DEBUG("end fit old "<<oldshift<< " new " <<t0 );
    // Final Minuit Fit result
    if(nhits==NUMPAR) {
      nhits++;
      chi2 += 1.;
    }
    m_result.set( chi2, nhits-NUMPAR, dtheta, -1.*dy0 );
    m_result.line().set( LocPos( Zc - sinus*d, Yc + cosin*d ), ang );
    if(t0==0.) t0=0.00001;
    m_result.setT0Shift(t0,t0Err);
    if(m_trace) std::cout << "Minuit Fit complete: Chi2 " << chi2 << " angle " << m_result.line().phi() << " nhits "<< nhits  << " t0result " << t0 << std::endl;
    
    if(m_trace) {
      ATH_MSG_DEBUG( "Minuit Fit complete: Chi2 " << chi2 << " angle " << m_result.line().phi() << " nhits "<<nhits<<" numpar "<<NUMPAR << " per dof " << chi2/(nhits-NUMPAR)   );

      ATH_MSG_DEBUG( "Fit complete: Chi2 " << chi2 <<" nhits "<<nhits<<" numpar "<<NUMPAR << " per dof " << chi2/(nhits-NUMPAR)   );
      if(chi2/(nhits-NUMPAR) > 5) {
        ATH_MSG_DEBUG( "_______NOT GOOD "  );
      }
      ATH_MSG_DEBUG("chi2 "<<chi2<<" per dof "<<chi2/(nhits-NUMPAR)<<" root chisq "<<chisq );
    }

    // Do standard DCSL fit using the t0 updated results 
    // if minuit MIGRAD failed to converge (and t0 fit converged) 
//    if (errFlag5!=0&&errFlag4!=0)  {
//      bool oldrefit =  DCSLFitter::fit( m_result.line() , m_result.dcs(), selection );
//      int iok = 0;
//      if(oldrefit) iok = 1;
//      if(m_trace)*m_log << MSG::DEBUG << " ALARM Minimize fix " << errFlag1 << " ALARM minimize release " << errFlag2 << " ALARM migrad fix 1 " << errFlag3 << " ALARM minimize all free " << errFlag4 << " ALARM migrad all free " << errFlag5 << endmsg;
//      if(m_trace) std::cout << " ALARM Migrad and Minimize failed DCSL Fit Chi2 " << m_result.chi2() << " angle " << m_result.line().phi() << " y0 " << m_result.line().y0() << " nhits "<< selection.size() << " fitretval " << fitretval << " refit ok " << iok  << std::endl;
//      return  oldrefit;
//    } 
    
      
    return true;
  }
  
  
}
