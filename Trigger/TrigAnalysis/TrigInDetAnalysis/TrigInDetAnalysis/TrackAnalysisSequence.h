/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackAnalysisSequence.h
 **
 **     @author  mark sutton
 **     @date    Tue  3 Nov 2009 17:01:50 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_TRACKANALYSISSEQUENCE_H
#define TIDA_TRACKANALYSISSEQUENCE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

// #include "GaudiKernel/IToolSvc.h"
// #include "TrigDecisionTool/TrigDecisionTool.h"

using namespace TIDA;

#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/TrackFilter.h"

// class TrackSelector;
// class TrackAssociator;
// class TrackAnalysis;
// class TrackFilter;


class TrackAnalysisSequence {

public:

  TrackAnalysisSequence(const std::string& _name, 
			const std::string& _chain,    const std::string& _key,
			const std::string& _refchain, const std::string& _refkey, 
			TrackSelector*   selector, 
			TrackFilter*     filter,
			TrackSelector*   refselector,
			TrackFilter*     reffilter,
			TrackAssociator* matcher, 
			TrackAnalysis*   analysis,
			bool seeded=true )
    : m_name(_name), 
      m_chainName(_chain),        m_key(_key), 
      m_chainRefName(_refchain),  m_keyRef(_refkey), 
      m_testSelector(selector),   m_testFilter(filter), 
      m_refSelector(refselector),  m_refFilter(reffilter), 
      m_matcher(matcher), 
      m_analysis(analysis),
      m_seeded(seeded)
  {  
    // std::map<std::string, TrackAnalysisSequence*>::iterator mitr = m_map.find(m_name);
    // if ( mitr!=m_map.end() ) { 
    //   // do we want to forbid this?
    //   std::cerr << "analysisBuilder() analysis " << name << " already exists" << std::endl;
    //   //      return NULL;
    //   m_map.insert( std::map<std::string, TrackAnalysisSequence*>::value_type( m_name, this ) );
    // }
  } 


  TrackAnalysisSequence(const std::string& _name, 
			const std::string& _chain,    const std::string& _key,
			const std::string& _refchain, const std::string& _refkey, 
			TrackSelector*   selector, 
			TrackSelector*   refselector,
			TrackAssociator* matcher, 
			TrackAnalysis*   analysis,
			bool seeded=true )
    : m_name(_name), 
      m_chainName(_chain),        m_key(_key), 
      m_chainRefName(_refchain),  m_keyRef(_refkey), 
      m_testSelector(selector),   m_testFilter(NULL), 
      m_refSelector(refselector),  m_refFilter(NULL), 
      m_matcher(matcher), 
      m_analysis(analysis),
      m_seeded(seeded)
  {   } 


  TrackAnalysisSequence(const std::string& _name, 
			const std::string& _chain,   const std::string& _key,
			TrackSelector*   selector, 
			TrackSelector*   refselector,
			TrackAssociator* matcher, 
			TrackAnalysis*   analysis,
			bool seeded=true )
    : m_name(_name), 
      m_chainName(_chain),        m_key(_key), 
      m_chainRefName(_chain),     m_keyRef(_key), 
      m_testSelector(selector),   m_testFilter(NULL), 
      m_refSelector(refselector),  m_refFilter(NULL), 
      m_matcher(matcher), 
      m_analysis(analysis),
      m_seeded(seeded)
  {   } 


  TrackAnalysisSequence(const std::string& _chain,  const std::string& _key,
			TrackSelector*   selector, 
			TrackSelector*   refselector,
			TrackAssociator* matcher, 
			TrackAnalysis*   analysis,
			bool seeded=true )
    : m_name(_chain), 
      m_chainName(_chain),        m_key(_key), 
      m_chainRefName(_chain),     m_keyRef(_key), 
      m_testSelector(selector),   m_testFilter(NULL), 
      m_refSelector(refselector),  m_refFilter(NULL), 
      m_matcher(matcher), 
      m_analysis(analysis),
      m_seeded(seeded)
  {   } 


  ~TrackAnalysisSequence() { } 
  
  const std::string& name()         const { return m_name; } 
  
  const std::string& chainName()    const { return m_chainName; }
  const std::string& chainRefName() const { return m_chainRefName; }

  const std::string& key()    const { return m_key; }
  const std::string& keyRef() const { return m_keyRef; }

  void setSeeded(bool b) { m_seeded=b; } 
  bool isSeeded()  const { return m_seeded; } 


  TrackSelector*   getTestSelector() { return m_testSelector; }
  TrackSelector*   getRefSelector()  { return m_refSelector; }
  TrackAssociator* getAssociator()   { return m_matcher; }
  TrackAnalysis*   getAnalysis()     { return m_analysis; }


  void setTestSelector( TrackSelector* selector) { m_testSelector = selector; }
  void setRefSelector( TrackSelector* selector)  { m_refSelector  = selector; }
  void setAssociator( TrackAssociator* matcher)  { m_matcher  = matcher; }
  void setAnalysis( TrackAnalysis* analysis)     { m_analysis = analysis; }
  

private:

  std::string m_name;

  std::string m_chainName;
  std::string m_key;

  std::string m_chainRefName;
  std::string m_keyRef;


  TrackSelector*   m_testSelector;
  TrackFilter*     m_testFilter;

  TrackSelector*   m_refSelector;
  TrackFilter*     m_refFilter;
  
  TrackAssociator* m_matcher;

  //  std::vector<TrackAnalysis*>   m_analysis;
  TrackAnalysis*   m_analysis;
 
  //  static std::map<std::string, TrackAnalysisSequence*> m_map;
  
  bool m_seeded;

};

inline std::ostream& operator<<( std::ostream& s, const TrackAnalysisSequence& /* _t */ ) { 
  return s;
}


#endif  // TIDA_TRACKANALYSISSEQUENCE_H 










