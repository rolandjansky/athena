// emacs: this is -*- c++ -*-
//
//   @file    Associator_BestMatch.h         
//   
//            Erkcan's track matchers 
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: Associator_BestMatch.h, v0.0   Wed 28 Oct 2009 02:23:23 CET sutt $


#ifndef __ASSOCIATOR_BESTMATCH_H
#define __ASSOCIATOR_BESTMATCH_H

#include <set>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"



class Associator_MatcherBase : public TrackAssociator {

public:

  Associator_MatcherBase(const std::string& name, double d) : 
    TrackAssociator(name), md(d) 
  { } 

  virtual ~Associator_MatcherBase() { } 

  //  virtual void match( const std::vector<TrigInDetAnalysis::Track*>& ref, const std::vector<TrigInDetAnalysis::Track*>& test ) { 
  virtual void match(const std::vector<TrigInDetAnalysis::Track*>& s1, 
                     const std::vector<TrigInDetAnalysis::Track*>& s2 ) 
  {
    clear();
    
    const std::vector<TrigInDetAnalysis::Track*>& ref  = s1; 
    const std::vector<TrigInDetAnalysis::Track*>& test = s2;
    // loop over reference tracks
    for ( int i=ref.size() ; i-- ; ) {
      
      TrigInDetAnalysis::Track* reftrack = ref[i];
      
      //  find the closest track
      TrigInDetAnalysis::Track* tmptrack = NULL;

      double dmin = 0;

      // loop over test tracks
      for ( int j=test.size() ; j-- ; ) { 

	TrigInDetAnalysis::Track* testtrack = test[j];

        double d = distance( reftrack, testtrack );

        // found a close track
        if ( tmptrack==NULL ||  d<dmin ) { 
          dmin = d;
          tmptrack = testtrack;
        } 
      }

      // is this inside the delta R specification?
      if ( tmptrack && dmin<md ) { 
        mmatched.insert(    track_map::value_type(reftrack,tmptrack) );
        mrevmatched.insert( track_map::value_type(tmptrack,reftrack) );
        
        std::cout << "\t\tmatched " << *reftrack  << "\t -> \t" << *tmptrack << "\tDr=" << dmin << std::endl; 
      }
    }
  }
   

  virtual double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) = 0;


private:

  double md;

};




class Associator_DeltaRMatcher : public Associator_MatcherBase { 

public:

  Associator_DeltaRMatcher(const std::string& name, double d) : 
    Associator_MatcherBase(name, d) 
  { } 
  
  virtual ~Associator_DeltaRMatcher() { } 

  double distance( TrigInDetAnalysis::Track* t0,  TrigInDetAnalysis::Track* t1 ) {
    double deta = t0->eta()-t1->eta();
    double dphi = t0->phi()-t1->phi();    
    if ( dphi> M_PI ) dphi-=2*M_PI; 
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return std::sqrt( deta*deta + dphi*dphi );
  }
  
};






class Associator_BestMatcher : public TrackAssociator {

protected:

  class matched { 
    
  public:
    
    matched(double d, int i, int j) : md(d), mmatch(std::pair<int,int>(i,j)) { } 
    
    double d() const { return md; } 
    
    std::pair<int, int> pair() const { return mmatch; }
    
    int first()  const { return mmatch.first; } 
    int second() const { return mmatch.second; } 
    
    bool operator<(const matched& a) const { return d()<a.d(); }
    bool operator>(const matched& a) const { return d()>a.d(); }
    bool operator==(const matched& a) const { return  d()==a.d(); } 
    bool operator!=(const matched& a) const { return  d()!=a.d(); } 
    
  private:
    double              md;
    std::pair<int, int> mmatch; 
  };

public:

  Associator_BestMatcher(const std::string& name, double d) : 
    TrackAssociator(name), md(d) 
  { 
    //    std::cout << "Associator_BestMatcher::Associator_BestMatcher() " << name << "\td = " << md << std::endl; 
  } 
  
  virtual ~Associator_BestMatcher() { } 
  
  
  //  virtual void match( const std::vector<TrigInDetAnalysis::Track*>& ref, const std::vector<TrigInDetAnalysis::Track*>& test ) { 
  virtual void match(const std::vector<TrigInDetAnalysis::Track*>& ref, 
                     const std::vector<TrigInDetAnalysis::Track*>& test ) 
  {
    clear();
    
    std::map<int,int> matched = matcher( ref, test);

    std::map<int,int>::iterator  mitr = matched.begin();
    while ( mitr!=matched.end() ) {
      mmatched.insert(    track_map::value_type( ref[mitr->first],   test[mitr->second]) );
      mrevmatched.insert( track_map::value_type( test[mitr->second], ref[mitr->first] ) );
      mitr++;
    }  
   
  }
   

  virtual double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) = 0;


protected:


  template<typename Tp>
  std::map<int, int>  matcher( const std::vector<Tp*>& ref, const std::vector<Tp*>& test) { 
    //				double (*distance_func)(Tp*, Tp*), double dr) { 
    
    /// inserting into a multimatch sorts them into order 
    /// as they are added so you get the list of best 
    /// matches imediately 
    
    std::multiset<matched> m;
    
    for (unsigned int i=0 ; i<ref.size() ; i++ ) { 
      
      for (unsigned int j=0 ; j<test.size() ; j++ ) { 
	double d = distance(ref[i], test[j]);          
	if ( d<md ){  
	  m.insert( matched(d, i, j) ); 
	}
      }
    }
    
    /// now go through from best to worst, adding to another
    /// sorted set only the matches that do not use tracks
    /// already used by a better matching pair already found
    
    // lookup of tracks already used
    std::set<int> refind;
    std::set<int> testind;
    
    // set of unique track pairings
    std::multiset<matched> unique;
    
    std::multiset<matched>::iterator mitr = m.begin();
    
    double chi2 = 0;
    
    while ( mitr!=m.end() ) { 
      
      //    std::cout << "new match " << *mitr << "\t" <<  ref[mitr->first()] << "   " << test[mitr->second()] << std::endl;
      
      int rind = mitr->first();
      int tind = mitr->second();
      
      std::set<int>::iterator ritr = refind.find(rind);
      std::set<int>::iterator titr = testind.find(tind);
      
      /// has either track from this track pair already been used...
      if ( ritr==refind.end() && titr==testind.end() ) { 
	unique.insert( *mitr );
	refind.insert(rind);
	testind.insert(tind);
	chi2 += (mitr->d()*mitr->d());
      } 
      
      mitr++;
    }
    
    //    std::cout << "chi2 of matches " << chi2 << std::endl;
   
    /// hooray!! now print out the answer, make a map etc
    /// this isn't technically needed, could just use the 
    /// set of "matched" objects, but hey ho 
    
    //  std::cout << "\nmatched" << std::endl; 
    
    std::map<int, int> matches;
    
    mitr = unique.begin();
    while ( mitr!=unique.end() ) {
      matches.insert( std::map<int, int>::value_type( mitr->first(), mitr->second() ) );
      //   std::cout << "\tbest match " << *mitr << "\t" <<  ref[mitr->first()] << "\t" << test[mitr->second()] << std::endl;
      mitr++;
    }  
    
    return matches;
    
  }


protected:

  double md;

};


class Associator_SecondBestpTMatcher : public Associator_BestMatcher {

public:

  Associator_SecondBestpTMatcher(const std::string& name, double d) :
    Associator_BestMatcher(name, d)
  { }

  virtual ~Associator_SecondBestpTMatcher() { }

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
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

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
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

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
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

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
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

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
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

  double distance( TrigInDetAnalysis::Track* t0, TrigInDetAnalysis::Track* t1 ) {
    double dphi = t0->phi()-t1->phi();    
    if ( dphi> M_PI ) dphi-=2*M_PI; 
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return std::fabs( dphi );
  }

};




#endif //  __ASSOCIATOR_BESTMATCH_H
