// emacs: this is -*- c++ -*-
// emacs: this is -*- c++ -*-
//
//   @file    Filters.h        
//
//                   
//  
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: Filters.h 767214 2016-08-10 12:23:36Z sutt $


#ifndef  TIDA_FILTERS_H
#define  TIDA_FILTERS_H

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_TrackQuality.h"

#include "TrigInDetAnalysis/TIDAVertex.h"


#include <cmath>

/// default simple filter which accepts all tracks

class Filter_True : public TrackFilter {  
public:
  bool select( const TIDA::Track* , const TIDARoiDescriptor* =0 ) { return true; }
}; 


class Filter_Author : public TrackFilter {  
public:
  
  Filter_Author(const int& author) : m_author(author) { } 
  void setAuthor(unsigned int author) { m_author = author; }
    
  bool select( const TIDA::Track* t, const TIDARoiDescriptor* =0 ) {
    if ( t->author()>m_author ) return false;
    // if ( std::fabs(t->author())!=m_author ) return false;
    return true; 
  }
    
private:
    
  int m_author;
    
}; 



//Filter truth particles based on their Id
class Filter_pdgId : public TrackFilter {  
public:
  
  Filter_pdgId(const unsigned int& pdgId) : m_pdgId(pdgId) { } 
    
  bool select( const TIDA::Track* t, const TIDARoiDescriptor* =0 ) {
    //Author stores pdgId
    if (std::fabs(t->author())!=m_pdgId) return false;
    return true; 
  }
    
private:
    
  int m_pdgId;
    
}; 



//Filter truth particles based on Id and pt and eta
class Filter_pdgIdpTeta : public TrackFilter {

public:

  Filter_pdgIdpTeta(const unsigned int& pdgId, double etaMax, double  pTMin) : m_pdgId(pdgId), m_etaMax(etaMax), m_pTMin(pTMin)  { } 

  bool select( const TIDA::Track* t, const TIDARoiDescriptor* /*r=0*/ ) {
    //Author stores pdgId
    if ( std::abs(t->author())!=m_pdgId || 
	 std::fabs(t->eta())>m_etaMax   ||
	 std::fabs(t->pT())<m_pTMin) return false;

    return true; 
  }
  
private:
  
  int m_pdgId;
  double  m_etaMax;
  double  m_pTMin;
}; 




//Set upper and lower pT and upper eta bound
class Filter_Bound : public TrackFilter {
public:
  Filter_Bound(const double &eta_max, const double &pT_min, const double &pT_max) : 
    m_pT_max(pT_max),
    m_pT_min(pT_min), 
    m_eta_max(eta_max) {}

  bool select ( const TIDA::Track* t, const TIDARoiDescriptor* =0) {
    if (std::fabs(t->eta()) > m_eta_max) return false;
    if (std::fabs(t->pT()) > m_pT_max) return false;
    if (std::fabs(t->pT()) < m_pT_min) return false;
    return true;
  }
private:
  double m_pT_max;
  double m_pT_min;
  double m_eta_max;
};


//class Filter_Roi : public TrackFilter {  
//	public:
//
//		Filter_Roi() : m_set(false), m_roi(0) { 
//			std::cout << "Filter_Roi::Filter_Roi" << std::endl;
//		}  
//
//		void setRoi( TIDARoiDescriptor* roi ) { m_roi = roi; m_set = true; } 
//
//		bool select( const Track* t, const TIDARoiDescriptor* r=0 ) {
//
//			//    std::cout << "select()" << std::endl;
//
//			if ( r==0 ) r = m_roi;
//			if ( r==0 ) { std::cerr << "roi not set" << std::endl; return false; }
//			double deta = t->eta() - r->eta();
//			double dphi = t->phi() - r->phi();
//
//			if ( dphi<-M_PI ) dphi += M_PI; 
//			if ( dphi>M_PI  ) dphi -= M_PI; 
//
//			if ( std::fabs(deta)<r->etaHalfWidth() && 
//					std::fabs(dphi)<r->phiHalfWidth() ) return true;
//			else  return false;
//
//		}
//
//	private:
//
//		bool           m_set;
//		TIDARoiDescriptor* m_roi;
//
//};



class Filter_Vertex : public TrackFilter {  
public:
  
  Filter_Vertex(double d0Cut, double z0Cut) : m_d0Cut(d0Cut), m_z0Cut(z0Cut) { 
    std::cout << "Filter_Vertex::Filter_Vertex() with d0 cut " << m_d0Cut << "\tz0 cut " << m_z0Cut << std::endl;  
    ///    TDirectory* dir = gDirectory;
    ///    dir->cd();
  } 
    
  virtual ~Filter_Vertex() {   } 
    
  void setVertex(const TIDA::Vertex& v) { 
    m_v.clear();
    m_v.push_back(v);
  } 
    
  void setVertex(const std::vector<TIDA::Vertex>& vv) { 
    m_v.clear();
    m_v = vv;
  } 
    
  bool select( const TIDA::Track* t, const TIDARoiDescriptor* =0 ) {
    /// calculate z, d0 with respect 
    /// to vertex and then cut on them 
      
    //    std::cout << "Filter_Vertex::select() " << *t << "\t" << m_v.size() << std::endl; 
    
    if ( m_v.size()==0 ) return true;

    for (unsigned int i=0 ; i<m_v.size() ; i++ ) { 
	
      double* v = m_v[i].position();
      /// tracks should already be corrected to the beam line now
      //     double a0 = t->a0() + v[0]*std::sin( t->phi() ) - v[1]*std::cos( t->phi() );
      double a0 = t->a0(); 
	
      /// calculate z
	
      double theta = 2*std::atan(std::exp( -t->eta() ));
      //      double z = t->z0() + ( v[0]*std::cos( t->phi() ) + v[1]*std::sin( t->phi() ) )/std::atan(theta);
      double z = t->z0();
      double z0sin = (z-v[2])*std::sin(theta);


      //      std::cout << "Filter_Vertex::select()" 
      //		<< "\ta0 "    << std::fabs(a0)    << "(" << m_d0Cut << ")"
      //		<< "\tz0sin " << std::fabs(z0sin) << "(" << m_z0Cut << ")"
      //		<< std::endl;
	
      /// now test agreement
      if ( std::fabs(a0)<m_d0Cut && std::fabs(z0sin)<m_z0Cut ) return true;
    }

    return false; 
  }
    
private:
    
  double m_d0Cut;
  double m_z0Cut;
    
  std::vector<TIDA::Vertex> m_v;
    
}; 






class Filter_Combined : public TrackFilter {  

public:

  Filter_Combined( TrackFilter* f1, TrackFilter* f2) : 
    mf1(f1), mf2(f2), m_roi(0), 
    m_debugPrintout(false), 
    mcontain(true)
  { } 

  void setRoi( TIDARoiDescriptor* r ) { m_roi = r; } 


  /// set / unset the flag to determine whether tracks 
  /// should be fully contained in the RoI or not 

  void containtracks( bool b=true ) { mcontain=b; }


  bool contains( const TIDA::Track* t, const TIDARoiDescriptor* r ) const { 

    if ( r==0 ) { 
      std::cerr << "Filter_Combined::contains() called with null roidescriptor" << std::endl;
      return true;
    }

    if ( r->composite() ) {       
      for ( unsigned i=r->size() ; i-- ; )  if ( contains( t, r->at(i) ) ) return true;
    }
    else { 

      if ( r->isFullscan() ) return true;

      bool contained_phi = false;
      
      /// NB: This isn't actually correct - the tracks can bend out of the 
      ///     Roi even it the perigee phi is withing the Roi
      if ( r->phiMinus()<r->phiPlus() )  contained_phi = ( t->phi()>r->phiMinus() &&  t->phi()<r->phiPlus() );
      else 	                         contained_phi = ( t->phi()>r->phiMinus() ||  t->phi()<r->phiPlus() ); 
      
      bool contained_zed = ( t->z0()>r->zedMinus() && t->z0()<r->zedPlus() ); 

      /// NB: This is *completely* wrong, tracks could be completely contained 
      ///     within an Roi but failthis condition
      bool contained_eta = ( t->eta()<r->etaPlus() && t->eta()>r->etaMinus() );
      
      if ( mcontain ) { 

	///  includes calculation of approximate z position of the 
	///  track at radius r and test if track within that z position at radius r 
       
	double rexit = 0;
	double zexit = 0;

	//	double tantheta = TIDARoiDescriptor::exitpoint( t->z0(), t->eta(), zexit, rexit );

	TIDARoiDescriptor::exitpoint( t->z0(), t->eta(), zexit, rexit );

	/// rather complicated conditions to determine whether a track is  
	/// fully contained in the Roi or not
 
	contained_eta = true;

	if ( zexit<r->zedMinusR() ) contained_eta = false; 

	if ( r->rMinusZed()<r->maxR() ) { 
	  if ( r->etaMinus()<0 ) { 
	    if ( zexit<0 && rexit<r->rMinusZed() ) contained_eta = false;
	  }
	  else if ( r->etaMinus()>0 ) { 
	    if ( zexit>0 && rexit>r->rMinusZed() ) contained_eta = false;
	  }
	  else contained_eta = false;
	}
	
	if ( zexit>r->zedPlusR() ) contained_eta = false; 

	if ( r->rPlusZed()<r->maxR() ) { 
	  if ( r->etaPlus()<0 ) { 
	    if ( zexit<0 && rexit>r->rPlusZed() ) contained_eta = false;
	  }
	  else if ( r->etaPlus()>0 ) { 
	    if ( zexit>0 && rexit<r->rPlusZed() ) contained_eta = false;
	  }
	  else contained_eta = false;
	}
	
	/// now check phi taking account of the track transverse curvature

	double newphi = outerphi( t->pT(), t->phi(), rexit );
	
	if ( newphi==0 ) return false;
	
	if ( r->phiMinus()<r->phiPlus() ) contained_phi &= ( newphi>r->phiMinus() &&  newphi<r->phiPlus() );
	else                              contained_phi &= ( newphi>r->phiMinus() ||  newphi<r->phiPlus() );

      }

      if ( contained_eta  && 
	   contained_phi  &&
	   contained_zed  ) { 
	if ( m_debugPrintout ) std::cout << "\tFilter::inside roi" << std::endl;
	return true;
      }
      
    }
    
    return false;

  }


  bool select( const TIDA::Track* t, const TIDARoiDescriptor* r=0 ) {

    if ( r!=0 ) m_roi = r;
    
    if ( m_debugPrintout ) { 
      std::cout << "\tFilter: " << this << "\tfilter1 " << mf1->select(t,m_roi) << "\tfilter2 " << mf2->select(t,m_roi) << "\troi " << m_roi << std::endl;
    }

    /// no roi so just return the and of the input filters
    if ( m_roi==0 ) return ( mf1->select(t,r) && mf2->select(t,r) );
    else {

      if ( m_debugPrintout ) { 
	std::cout << "\tFilter::filter1 " << mf1->select(t,m_roi) << "\tfilter2 " << mf2->select(t,m_roi) << "\troi " << *m_roi << std::endl; 
      }

      if ( contains( t, m_roi ) )  return ( mf1->select(t,m_roi) && mf2->select(t,m_roi) );
      else  return false;

    }
  }  

  void setDebug(bool b) { m_debugPrintout=b; }  

public:


  /// calculate the (very approximate) phi position 
  /// for a track at a rafius r
  
  static double outerphi( double pt, double phi, double r=1000 ) {  
    
    /// track (signed) radius of curvature
    
    double mqR = 10*pt/(2.99792458*2); // 2.998=speed of light, 2=Atlas B field 
    
    double ratio = 0.5*r/mqR;
    
    double newphi = phi;
    
    /// make sure it escapes the radius 
    /// in question
    if ( std::fabs(ratio)>1 ) return 0; 
    
    /// calculate new position
    newphi -= std::asin( ratio );  
    
    /// wrap to -pi to pi
    while ( newphi<-M_PI ) newphi += 2*M_PI;
    while ( newphi> M_PI ) newphi -= 2*M_PI;
    
    return newphi;
    
  }
  
private:

  TrackFilter* mf1;
  TrackFilter* mf2;

  const TIDARoiDescriptor* m_roi;

  bool  m_debugPrintout;

  bool   mcontain;

};


#endif // TIDA_FILTERS_H

