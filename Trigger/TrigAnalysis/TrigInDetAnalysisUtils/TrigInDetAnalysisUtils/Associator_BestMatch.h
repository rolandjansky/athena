/* emacs: this is -*- c++ -*- */
/**
 **     @file    Associator_BestMatch.h
 **
 **       Erkcan's track matchers 
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:23:23 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_ASSOCIATOR_BESTMATCH_H
#define TIDAUTILS_ASSOCIATOR_BESTMATCH_H

#include <set>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"

#include "TrigInDetAnalysisUtils/BestMatcher.h"



typedef BestMatcher<TIDA::Track> Associator_BestMatcher;




class Associator_DeltaRMatcher : public Associator_BestMatcher { 

public:

  Associator_DeltaRMatcher(const std::string& name, double d) : 
    Associator_BestMatcher(name, d) 
  { } 
  
  virtual ~Associator_DeltaRMatcher() { } 

  double distance( const TIDA::Track* t0,  const TIDA::Track* t1 ) const {
    double deta = t0->eta()-t1->eta();
    double dphi = t0->phi()-t1->phi();    
    if ( dphi> M_PI ) dphi-=2*M_PI; 
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return std::sqrt( deta*deta + dphi*dphi );
  }
  
};




class Associator_SecondBestpTMatcher : public Associator_BestMatcher {

public:

  Associator_SecondBestpTMatcher(const std::string& name, double d) :
    Associator_BestMatcher(name, d)
  { }

  virtual ~Associator_SecondBestpTMatcher() { }

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double dpT = fabs( t0->pT()/1000 - t1->pT()/1000 );
    //std::cout << "pT dist = " << dpT << "    dpT / pT = " << dpT/fabs(t0->pT()/1000) << std::endl;
    return dpT/(fabs(t0->pT()/1000));
  }

};



class Associator_BestDeltaRMatcher : public Associator_BestMatcher { 

public:

  Associator_BestDeltaRMatcher(const std::string& name, double d) : 
    Associator_BestMatcher(name, d) 
  { } 

  virtual ~Associator_BestDeltaRMatcher() { } 

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double deta = t0->eta()-t1->eta();
    double dphi = t0->phi()-t1->phi();    
    if ( dphi> M_PI ) dphi-=2*M_PI; 
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return std::sqrt( deta*deta + dphi*dphi );
  }

};








class Associator_BestDeltaRZMatcher : public Associator_BestMatcher { 

public:

  Associator_BestDeltaRZMatcher(const std::string& name, double deta=0.05, double dphi=0.05, double dzed=5) : 
    Associator_BestMatcher(name, 1), 
    m_deta2(deta*deta),
    m_dphi2(dphi*dphi),
    m_dzed2(dzed*dzed)
  {
    std::cout << "Associator: Best RZ Matcher" << std::endl;
  } 

  virtual ~Associator_BestDeltaRZMatcher() { } 

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double deta = t0->eta()-t1->eta();
    double dphi = t0->phi()-t1->phi(); if ( dphi> M_PI ) dphi-=2*M_PI;  if ( dphi<-M_PI ) dphi+=2*M_PI;
    double dzed = t0->z0()-t1->z0();    

    double dr =  std::sqrt( deta*deta/m_deta2 + dphi*dphi/m_dphi2 + dzed*dzed/m_dzed2 );

    //    std::cout << "RZ matcher dr: " << dr 
    //	      << "\t : \t" << deta*deta/m_deta2 << "\t" << dphi*dphi/m_dphi2 << "\t" << dzed*dzed/m_dzed2  << "\t" << *t1 
    //	      << std::endl;

    return dr;
  }

protected: 

  double m_deta2;
  double m_dphi2;
  double m_dzed2;

};





class Associator_BestDeltaRZSinThetaMatcher : public Associator_BestMatcher { 

public:

  Associator_BestDeltaRZSinThetaMatcher(const std::string& name, double deta=0.05, double dphi=0.05, double dzed=5) : 
    Associator_BestMatcher(name, 1), 
    m_deta2(deta*deta),
    m_dphi2(dphi*dphi),
    m_dzed2(dzed*dzed)
  {
    std::cout << "Associator: Best RZ Matcher" << std::endl;
  } 

  virtual ~Associator_BestDeltaRZSinThetaMatcher() { } 

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double theta = 2*std::atan(std::exp(-t1->eta()));

    double deta = t0->eta()-t1->eta();
    double dphi = t0->phi()-t1->phi(); if ( dphi> M_PI ) dphi-=2*M_PI;  if ( dphi<-M_PI ) dphi+=2*M_PI;

    double dzed = (t0->z0()-t1->z0())*std::sin(theta);    

    double dr =  std::sqrt( deta*deta/m_deta2 + dphi*dphi/m_dphi2 + dzed*dzed/m_dzed2 );

    //    std::cout << "RZ matcher dr: " << dr 
    //	      << "\t : \t" << deta*deta/m_deta2 << "\t" << dphi*dphi/m_dphi2 << "\t" << dzed*dzed/m_dzed2  << "\t" << *t1 
    //	      << std::endl;

    return dr;
  }

protected: 

  double m_deta2;
  double m_dphi2;
  double m_dzed2;

};




class Associator_BestSigmaMatcher : public Associator_BestMatcher { 

public:

  Associator_BestSigmaMatcher(const std::string& name, double dsigma=1) : 
    Associator_BestMatcher(name, dsigma) 
  { 
    std::cout << "Associator: Best Sigma Matcher" << std::endl;
  } 

  virtual ~Associator_BestSigmaMatcher() { } 

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double deta = t1->eta()-t0->eta();
    double dphi = t1->phi()-t0->phi(); 
    if ( dphi> M_PI ) dphi-=2*M_PI;  
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    double dzed = t1->z0()-t0->z0();    

    //     std::cout << *t0 << "\n" << *t1 << "\ndphi : " << dphi << std::endl;

    deta /= t1->deta();
    dphi /= t1->dphi();
    dzed /= t1->dz0();

    double dr =  std::sqrt( deta*deta + dphi*dphi + dzed*dzed );

    //    std::cout << "Sigma matcher dr: " << dr 
    //	      << "\t : \t" << deta*deta << "\t" << dphi*dphi << "\t" << dzed*dzed << "\t" << *t1 
    //	      << std::endl;

    return dr;
  }

protected: 

  double m_deta2;
  double m_dphi2;
  double m_dzed2;

};




class Associator_BestDeltaPhiMatcher : public Associator_BestMatcher { 

public:

  Associator_BestDeltaPhiMatcher(const std::string& name, double p) : 
    Associator_BestMatcher(name, p) 
  { } 

  virtual ~Associator_BestDeltaPhiMatcher() { } 

  double distance( const TIDA::Track* t0, const TIDA::Track* t1 ) const {
    double dphi = t0->phi()-t1->phi();    
    if ( dphi> M_PI ) dphi-=2*M_PI; 
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return std::fabs( dphi );
  }

};




#endif //  TIDAUTILS_ASSOCIATOR_BESTMATCH_H
