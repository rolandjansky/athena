/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/foreach.hpp>
#include <cmath>

//#include <iostream>
//using namespace std;

#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TProfile.h"

#include "TH2I.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TProfile2D.h"

namespace AllTrigLBNHists {
  static std::set<ITrigLBNHist*> s_lbnHistograms;
}


//std::set<ITrigLBNHist*> AllTrigLBNHists::s_lbnHistograms = std::set<ITrigLBNHist*>();

ITrigLBNHist::ITrigLBNHist()
  : m_depth(1),
    m_group(1),
    m_last_lbn(0),
    m_last_force(1),
    m_histSvc(0) {
  AllTrigLBNHists::s_lbnHistograms.insert(this);
}

ITrigLBNHist::~ITrigLBNHist() {
  AllTrigLBNHists::s_lbnHistograms.erase(this);
}


void ITrigLBNHist::set_lbn(unsigned int l) {
  BOOST_FOREACH(ITrigLBNHist* h, AllTrigLBNHists::s_lbnHistograms)
    h->lbn(l);  
}

// this is not bad duplication we can not use the same function, guess why!
void ITrigLBNHist::force_lbn(unsigned int l ) {  
  BOOST_FOREACH(ITrigLBNHist* h, AllTrigLBNHists::s_lbnHistograms)
    h->force(l);
}



void ITrigLBNHist::setDepth( unsigned int depth ) {
  //   cerr << "LBN history depth for the histogram : " << depth << endl;
  m_depth = depth;
}
void ITrigLBNHist::setGroup( unsigned int group ) {
  //   cerr << "LBN history depth for the histogram : " << depth << endl;
  m_group = group;
}
void ITrigLBNHist::setITHistSvc( ITHistSvc* hs) {
  m_histSvc = hs;
}
void ITrigLBNHist::setPath( const std::string& path ){
  m_path = path;
} 

///////////////////////////////////////////
// real implementation

template<class HTYPE> 
TrigLBNHist<HTYPE>::TrigLBNHist( const HTYPE& his_template ) 
  : HTYPE(his_template),    
    m_lateEvents(lbn_tag(0,1), 0),
    m_current(m_lateEvents) {
  m_lateEvents.histogram = 0;
  m_name = his_template.GetName();
  m_title = his_template.GetTitle();
  this->SetNameTitle((m_name+"_runsummary").c_str(), (m_title+" Run Summary").c_str() );
}



template<class HTYPE, bool> struct fill_x;

template<class HTYPE> 
struct fill_x<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, Double_t x) { return h->HTYPE::Fill(x); }
}; 

template<class HTYPE> 
struct fill_x<HTYPE, false>
{
  static Int_t do_it(HTYPE* , Double_t) { return -1; }
}; 


template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill( Double_t x) {
  fill_x<HTYPE, !boost::is_same<TProfile, HTYPE>::value>::do_it(this, x);     
  
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;
  fill_x<HTYPE, !boost::is_same<TProfile, HTYPE>::value>::do_it(theHis, x);  
  return 1;
}

template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill( const char* x, Double_t y ) {
  HTYPE::Fill(x, y);
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;

  theHis->Fill(x,y);
  return 1;
}

template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill( Double_t x, Double_t y ) {
  HTYPE::Fill(x, y);
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;

  theHis->Fill(x,y);
  return 1;
}


// variant of 2D histogram Fill

template<class HTYPE, bool> struct fill_xyw;

template<class HTYPE> 
struct fill_xyw<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, Double_t x, Double_t y, Double_t w) { return h->HTYPE::Fill(x,y,w); }
}; 

template<class HTYPE> 
struct fill_xyw<HTYPE, false>
{
  static Int_t do_it(HTYPE* , Double_t , Double_t , Double_t ) { return -1; }
}; 



template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill(Double_t x, Double_t y, Double_t w) 
{   
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;
  if( boost::is_base_of<TH2, HTYPE>::value ){
    fill_xyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it(theHis, x, y, w);  
    fill_xyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);
  } else if ( boost::is_same<TProfile, HTYPE>::value ) {
    fill_xyw<HTYPE, boost::is_same<TProfile, HTYPE>::value>::do_it(theHis, x, y, w);
    fill_xyw<HTYPE, boost::is_same<TProfile, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);
  } else if ( boost::is_same<TProfile2D, HTYPE>::value ) {
    fill_xyw<HTYPE, boost::is_same<TProfile2D, HTYPE>::value>::do_it(theHis, x, y, w);
    fill_xyw<HTYPE, boost::is_same<TProfile2D, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);    
  } else {
    return -1;  
  }
  return 1;
}


// variant of 2D histogram Fill - needed for 2dProfile

template<class HTYPE, bool> struct fill_xyzw;

template<class HTYPE> 
struct fill_xyzw<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, Double_t x, Double_t y, Double_t z, Double_t w) { return h->HTYPE::Fill(x,y,z,w); }
}; 

template<class HTYPE> 
struct fill_xyzw<HTYPE, false>
{
  static Int_t do_it(HTYPE* , Double_t , Double_t , Double_t, Double_t ) { return -1; }
}; 



template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill(Double_t x, Double_t y, Double_t z, Double_t w) 
{   
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;
  if ( boost::is_same<TProfile2D, HTYPE>::value ) {
    fill_xyzw<HTYPE, boost::is_same<TProfile2D, HTYPE>::value>::do_it(theHis, x, y, z, w);
    fill_xyzw<HTYPE, boost::is_same<TProfile2D, HTYPE>::value>::do_it((HTYPE*)this, x, y, z, w);    
  } else {
    return -1;  
  }
  return 1;
}

// another variant of 2D histogram Fill

template<class HTYPE, bool> struct fill_xnameyw;

template<class HTYPE> 
struct fill_xnameyw<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, Double_t x, const char* y, Double_t w) { return h->HTYPE::Fill(x,y,w); }
}; 

template<class HTYPE> 
struct fill_xnameyw<HTYPE, false>
{
  static Int_t do_it(HTYPE* , Double_t , const char* , Double_t ) { return -1; }
}; 



template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill(Double_t x, const char* y, Double_t w) {
  if ( boost::is_base_of<TH2, HTYPE>::value ) {
    //    cerr << "will do 2D through fill_xnameyw" << endl;
    HTYPE *theHis = fetchHist();
    if(theHis == 0)
      return 0;
    fill_xnameyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it(theHis, x, y, w);  
    fill_xnameyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);  
    return 1;
  }
  return -1;
}

// another variant of 2D Fill

template<class HTYPE, bool> struct fill_namexyw;

template<class HTYPE> 
struct fill_namexyw<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, const char * x, Double_t y, Double_t w) { return h->HTYPE::Fill(x,y,w); }
}; 

template<class HTYPE> 
struct fill_namexyw<HTYPE, false>
{
  static Int_t do_it(HTYPE* , const char * , Double_t , Double_t ) { return -1; }
}; 


template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill(const char* x, Double_t y, Double_t w) 
{
  HTYPE *theHis = fetchHist();
  if(theHis == 0)
    return 0;
  
  if ( boost::is_base_of<TH2, HTYPE>::value ) {  
    fill_namexyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it(theHis, x, y, w);  
    fill_namexyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);  
  } else if ( boost::is_same<TProfile, HTYPE>::value ) {  
    fill_namexyw<HTYPE, boost::is_base_of<TProfile, HTYPE>::value>::do_it(theHis, x, y, w);  
    fill_namexyw<HTYPE, boost::is_base_of<TProfile, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);  
  } else {
    return -1;
  }
  return 1;
}



// another varain of Fill

template<class HTYPE, bool> struct fill_namexnameyw;

template<class HTYPE> 
struct fill_namexnameyw<HTYPE, true>
{
  static Int_t do_it(HTYPE* h, const char * x, const char* y, Double_t w) { return h->HTYPE::Fill(x,y,w); }
}; 

template<class HTYPE> 
struct fill_namexnameyw<HTYPE, false>
{
  static Int_t do_it(HTYPE* , const char * , const char* , Double_t ) { return -1; }
}; 


template<class HTYPE> 
Int_t TrigLBNHist<HTYPE>::Fill(const char* x, const char* y, Double_t w) {
  if ( boost::is_base_of<TH2, HTYPE>::value ) {
    //    cerr << "will do 2D through fill_namexnameyw" << endl;
    HTYPE *theHis = fetchHist();
    if(theHis == 0)
      return 0;
    fill_namexnameyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it(theHis, x, y, w);  
    fill_namexnameyw<HTYPE, boost::is_base_of<TH2, HTYPE>::value>::do_it((HTYPE*)this, x, y, w);  
    return 1;
  }
  return -1;
}


template<class HTYPE> 
void  TrigLBNHist<HTYPE>::reg() {
  std::string newpath = m_path+"/"+this->GetName();
  if (m_histSvc) {
    m_histSvc->regHist( newpath, this ).ignore();
  }
}

/////////////////////////////////////////////////////////////////////////////
// History maintanance.
// If you touch this make sure that make check gives no differences (or differences are understood).
//


template<class HTYPE> 
bool TrigLBNHist<HTYPE>::lbn(unsigned int l) {
  if ( m_depth == 0 || l == 0 ) // case when LBN history is disabled or we run MC where LBN ==0
    return true;

  // check if current hist is already OK
  if ( !(m_current.tag == l) ) {
    // check pool of already booked histograms
    typename std::list<tagged_histogram>::const_iterator buf
      = find_if(m_theHists.begin(), m_theHists.end(), matching_lbn_tag(l));
    if ( buf != m_theHists.end() ) {
      m_current = *buf;
    } else {    
      // if all failed check if we are already to late
      typename std::list<lbn_tag>::const_iterator late
	= find_if(m_deregistered.begin(), m_deregistered.end(), matching_lbn_tag(l) );
      
      if ( late != m_deregistered.end() ){
	fetchLateEventsHist();
	m_current = m_lateEvents;	
      } else {	
	// it means we realy need new histogram
	addNewHist(l); 
	// but
	buf = find_if(m_theHists.begin(), m_theHists.end(), matching_lbn_tag(l));
	if ( buf != m_theHists.end() ) {
	  m_current = *buf;  
	}
      }
    }
  }
  // try to release old histograms
  if ( l != m_last_lbn ) 
    releaseOldHist(l);
  m_last_lbn = l;  
  return true;
}


template<class HTYPE> 
bool TrigLBNHist<HTYPE>::force(unsigned int l) {
  m_last_force = l;

  // check if current hist is exactly starting in this LBN - then we do nothing
  if ( !(l == m_current.tag.begin_lbn) ) {
    // else new histogram is needed
    addNewHist(l, true);
    m_current = m_theHists.back();
  }

  // in in this case if we can drop some old stuff
  releaseOldHist(l);
  m_last_lbn = l;
  return true;
}


template<class HTYPE> 
bool TrigLBNHist<HTYPE>::fetchLateEventsHist() {
  if ( m_lateEvents.histogram )
    return true;
  //   cerr << "late events for " << this->GetName() << endl;

  //m_lateEvents.histogram = (HTYPE*)lock_histogram_operation<HTYPE>((HTYPE*)this)->Clone((m_name+"_lateevents").c_str());
  TH1::AddDirectory(false);
  m_lateEvents.histogram = new HTYPE(*this);
  m_lateEvents.histogram->SetName((m_name+"_lateevents").c_str());
  m_lateEvents.histogram->Reset();
  m_lateEvents.histogram->SetTitle((m_title+" Late Events").c_str());
   
  // book
  std::string newpath = m_path+"/"+m_lateEvents.histogram->GetName();
  if (m_histSvc)
    m_histSvc->regHist( newpath, m_lateEvents.histogram ).ignore();
  return true;
}

/////////////////////////////////////////////////////////////////////////////
template<class HTYPE> 
bool TrigLBNHist<HTYPE>::addNewHist(unsigned lbn, bool force) {
  //std::cout << " will make new histogram " << m_name << " for LBN : " << lbn << " force " << force << std::endl;

  //  HTYPE *newHist = (HTYPE*) lock_histogram_operation<HTYPE>((HTYPE*)this)->Clone(m_name.c_str());

  TH1::AddDirectory(false);
  HTYPE* newHist = new HTYPE(*this);
  if ( newHist == 0 ) 
    return false;

  newHist->SetName(m_name.c_str());
  newHist->Reset();
  newHist->SetTitle(m_title.c_str());

  // now need to calculate begin LBN and end LBN
  unsigned begin(0);
  unsigned end(0);
  if ( force ) {
    // eazy part
    begin = lbn;
    end = begin+m_group;
  } else {    
    if (lbn >= m_last_force) {
      begin = m_last_force;
      end = begin + m_group; 
      while ( !(begin <= lbn &&  lbn < end )) {
	begin += m_group;
	end += m_group;
      }

    } else {
      begin = (m_group*((lbn-1)/m_group))+1;
      end = begin+m_group;
    }

  }

  //  std::cout << " will make new histogram, begin LBN "<< begin << " end LBN " << end << std::endl;

  // remember the hist  
  append(m_theHists,  tagged_histogram( lbn_tag(begin, end), newHist ) );

  // book the histogram
  // create new path
  std::string newpath = "/run_1/lb_"+boost::lexical_cast<std::string>(begin)+"/LB"+m_path+"/"+newHist->GetName();  
  if (m_histSvc)
    m_histSvc->regHist( newpath, newHist ).ignore();
  return true;
}

template<class HTYPE> 
void TrigLBNHist<HTYPE>::append(std::list<tagged_histogram>& hists, const tagged_histogram& h) {
  hists.push_back(h);
  hists.sort();
  // In case we have overlaping tags we need to resolve it (close previous tag)
  typename std::list<tagged_histogram>::iterator i,j;
  for ( i = hists.begin(); i != hists.end(); ++i ) {
    j = i;
    std::advance(j, 1);
    if ( j == hists.end())
      break;
    if ( i->tag.end_lbn > j->tag.begin_lbn )
      i->tag.end_lbn = j->tag.begin_lbn;
  }


}

/////////////////////////////////////////////////////////////////////////////
template<class HTYPE> 
void TrigLBNHist<HTYPE>::releaseOldHist(unsigned lbn) {
  // keep removing hsitograms from the front
  while ( !m_theHists.empty() && (m_theHists.front().tag.end_lbn-1 + m_depth <= lbn ) ) {
    //    cout << "releasing " << m_theHists.front().histogram->GetName()
    //	 << " at lbn " << lbn << " endtag " <<  m_theHists.front().tag.end_lbn
    //	 << " depth " << m_depth << " size now " << m_theHists.size() << std::endl;
    if (m_histSvc){
      //m_histSvc->deReg(m_theHists.front().histogram).ignore();
      std::string path = "/run_1/lb_"+boost::lexical_cast<std::string>(m_theHists.front().tag.begin_lbn)
	+"/LB"+m_path+"/"+m_theHists.front().histogram->GetName();
      m_histSvc->deReg(path).ignore();
    }

    m_deregistered.push_back(m_theHists.front().tag);    
    m_theHists.pop_front();
    
    if ( m_deregistered.size() > 20 )  // we also need to protect that this list does not grow to much
      m_deregistered.pop_front();
  }
  
}

/////////////////////////////////////////////////////////////////////////////
template<class HTYPE> 
HTYPE* TrigLBNHist<HTYPE>::fetchHist() {
  return m_current.histogram;
} 


/////////////////////////////////////////////////////////////////////////////
template<class HTYPE> 
TrigLBNHist<HTYPE>::~TrigLBNHist() {  
}

/////////////////////////////////////////////////////////////////////////////
template<class HTYPE> 
void TrigLBNHist<HTYPE>::print() {
  std::cout << "histogram: " << m_path << "\n deregistered: ";

  typename std::list<lbn_tag>::const_iterator t;
  for( t = m_deregistered.begin(); t != m_deregistered.end(); ++t ) {     
    std::cout << "[" << t->begin_lbn  << "," << t->end_lbn << ") ";
  }
  std::cout << " active: ";
  typename std::list<tagged_histogram>::const_iterator th;
  for ( th = m_theHists.begin(); th != m_theHists.end(); ++th ) {
    std::cout << "[" << th->tag.begin_lbn  << "," << th->tag.end_lbn << ") ";
  }
  std::cout << std::endl;  
}


// This and only this are specializations we want!
// This let's us keep the code in the cxx
template class TrigLBNHist<TH1I>;
template class TrigLBNHist<TH1F>;
template class TrigLBNHist<TH1D>;
template class TrigLBNHist<TProfile>;

template <> Int_t TrigLBNHist<TH2I>::Fill(Double_t) { return -1; }
template <> Int_t TrigLBNHist<TH2I>::Fill(const char*, Double_t) { return -1; }
template <> Int_t TrigLBNHist<TH2F>::Fill(Double_t) { return -1; }
template <> Int_t TrigLBNHist<TH2F>::Fill(const char*, Double_t) { return -1; }
template <> Int_t TrigLBNHist<TH2D>::Fill(Double_t) { return -1; }
template <> Int_t TrigLBNHist<TH2D>::Fill(const char*, Double_t) { return -1; } 
template <> Int_t TrigLBNHist<TProfile2D>::Fill(Double_t) { return -1; }
template <> Int_t TrigLBNHist<TProfile2D>::Fill(const char*, Double_t) { return -1; }
template <> Int_t TrigLBNHist<TProfile2D>::Fill(Double_t, Double_t) { return -1; }

template class TrigLBNHist<TH2I>;
template class TrigLBNHist<TH2F>;
template class TrigLBNHist<TH2D>;
template class TrigLBNHist<TProfile2D>;

