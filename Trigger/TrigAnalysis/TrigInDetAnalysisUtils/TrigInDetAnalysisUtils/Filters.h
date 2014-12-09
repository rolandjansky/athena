

#ifndef TRIGINDETANALYSIS_FILTERS_H
#define TRIGINDETANALYSIS_FILTERS_H

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_TrackQuality.h"

#include "TrigInDetAnalysis/TrackVertex.h"

/// default simple filter which accepts all tracks

class Filter_True : public TrackFilter {  
 public:
  bool select( const TrigInDetAnalysis::Track* /*t*/, const TIDARoiDescriptor* /*r=0*/ ) { return true; }
}; 


class Filter_Author : public TrackFilter {  
 public:

 Filter_Author(const int& author) : m_author(author) { } 

  bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/ ) {
    if ( t->author()>m_author ) return false;
    return true; 
  }

 private:

  int m_author;

}; 



//Filter truth particles based on their Id
class Filter_pdgId : public TrackFilter {  
 public:

 Filter_pdgId(const unsigned int& pdgId) : m_pdgId(pdgId) { } 

  bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/ ) {
    //Author stores pdgId
    if (std::abs(t->author())!=m_pdgId) return false;
    return true; 
  }

 private:

  int m_pdgId;

}; 

//Filter truth particles based on Id and pt and eta
class Filter_pdgIdpTeta : public TrackFilter {
 public:

 Filter_pdgIdpTeta(const unsigned int& pdgId, double etaMax, double  pTMin) : m_pdgId(pdgId), m_etaMax(etaMax), m_pTMin(pTMin)  { } 

  bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/ ) {
    //Author stores pdgId
    if (std::abs(t->author())!=m_pdgId || std::fabs(t->eta())>m_etaMax || std::fabs(t->pT())<m_pTMin) return false;
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
 Filter_Bound(const double &eta_max, const double &pT_min, const double &pT_max) : m_pT_max(pT_max),
    m_pT_min(pT_min), 
    m_eta_max(eta_max) {}
  bool select ( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/) {
    if (fabs(t->eta()) > m_eta_max) return false;
    if (fabs(t->pT()) > m_pT_max) return false;
    if (fabs(t->pT()) < m_pT_min) return false;
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
    //			std::cout << "Filter_Vertex::Filter_Vertex() with d0 cut " << m_d0Cut << "\tz0 cut " << m_z0Cut << std::endl;  

    TDirectory* dir = gDirectory;

    dir->cd();
  } 

  ~Filter_Vertex() { 

  } 

  void setVertex(const TrackVertex& v) { 
    m_v.clear();
    m_v.push_back(v);
  } 

  void setVertex(const std::vector<TrackVertex>& vv) { 
    m_v.clear();
    m_v = vv;
  } 

  bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/ ) {
    /// calculate z, d0 with respect 
    /// to vertex and then cut on them 

    //    return  true;

    /// calculate a0 

    //   std::cout << "Filter_Vertex::select() " << *t << std::endl; 

    for (unsigned int i=0 ; i<m_v.size() ; i++ ) { 

      double* v = m_v[i].position();

      /// NB: tracks now converted back to some reference (beamline) rather than 0,0 
      //      double a0 = t->a0() + v[0]*std::sin( t->phi() ) - v[1]*std::cos( t->phi() );
      double a0 = t->a0();

      /// calculate z

      double theta = 2*std::atan(std::exp( -t->eta() ));

      //      double z = t->z0() + ( v[0]*std::cos( t->phi() ) + v[1]*std::sin( t->phi() ) )/std::atan(theta);
      double z = t->z0();

      double z0sin = (z-v[2])*std::sin(theta);

      /// now test agreement
      if ( std::fabs(a0)<m_d0Cut && std::fabs(z0sin)<m_z0Cut ) return true;

    }

    return false; 
  }

 private:

  double m_d0Cut;
  double m_z0Cut;

  std::vector<TrackVertex> m_v;

}; 



class Filter_Combined : public TrackFilter {  

 public:

 Filter_Combined( TrackFilter* f1, TrackFilter* f2) : mf1(f1), mf2(f2), m_roi(0) { } 

  void setRoi( TIDARoiDescriptor* r ) { m_roi = r; } 

  bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* r=0 ) {
    //    std::cout << this << "\tfilter1 " << mf1->select(t,r) << "\tfilter2 " << mf2->select(t,r) << "\troi " << m_roi << std::endl; 

    if ( r!=0 ) m_roi = r;
    if ( m_roi==0 ) return ( mf1->select(t,r) && mf2->select(t,r) );
    else {

      //     std::cout << this << "\tfilter1 " << mf1->select(t,r) << "\tfilter2 " << mf2->select(t,r) << "\troi " << m_roi << std::endl; 


      double deta = t->eta() - m_roi->eta();
      double dphi = t->phi() - m_roi->phi();
      //smh factor of two
      while ( dphi<-M_PI ) dphi += 2*M_PI; 
      while ( dphi>M_PI  ) dphi -= 2*M_PI; 
      //while ( dphi<-M_PI ) dphi += M_PI; 
      //while ( dphi>M_PI  ) dphi -= M_PI; 


      if ( std::fabs(deta)<m_roi->etaHalfWidth() && 
	   std::fabs(dphi)<m_roi->phiHalfWidth() ) return ( mf1->select(t,m_roi) && mf2->select(t,m_roi) );
      else  return false;

    }
  }  

 private:

  TrackFilter* mf1;
  TrackFilter* mf2;

  const TIDARoiDescriptor* m_roi;

};



//class Filter_TruthParticle : public TrackFilter {  
//
//	public:
//		Filter_TruthParticle( TrackFilter* f1) : mf1(f1), m_roi(0) { } 
//		void setRoi( TIDARoiDescriptor* r ) { m_roi = r; } 
//		bool select( const Track* t, const TIDARoiDescriptor* r=0 ) {
//			return ( mf1->select(t,r) && t->hitPattern()>0 && t->hitPattern()<200000);
//		}  
//
//	private:
//
//		const TIDARoiDescriptor* m_roi;
//		TrackFilter* mf1;
//
//};


#endif /// TRIGINDETANALYSIS_FILTERS_H
