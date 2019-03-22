/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"

#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/checker_macros.h"
#include "TrigMonTHistSvc.h"
#include "TrigMonitorBase/TrigLockedHist.h"

#include "hltinterface/IInfoRegister.h"

#include "TROOT.h"
#include "TError.h"
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TTree.h"

#include <iostream>
#include <sstream>
#include <streambuf>
#include <sstream>
#include <map>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
TrigMonTHistSvc::TrigMonTHistSvc( const std::string& name, ISvcLocator* svc ) 
    : THistSvcHLT(name, svc),
      m_excludeType("()"), 
      m_includeType(".+"),
      m_excludeName(".*\\..*"),                                            
      m_includeName("^/((run_[0-9]+/lb_[0-9]+/LB)|(SHIFT)|(EXPERT)|(DEBUG)|(EXPRESS)|(RUNSTAT))/.+/.+") {
      //m_includeName("^/((SHIFT)|(EXPERT)|(DEBUG)|(RUNSTAT))/.+/.+") {     
  declareProperty("ExcludeType", m_excludeType);   
  declareProperty("IncludeType", m_includeType);
  declareProperty("ExcludeName", m_excludeName);
  declareProperty("IncludeName", m_includeName);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
TrigMonTHistSvc::~TrigMonTHistSvc() {

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TrigMonTHistSvc::queryInterface(const InterfaceID & riid,
                                           void **ppvInterface) {
  return THistSvcHLT::queryInterface(riid, ppvInterface);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TrigMonTHistSvc::initialize ATLAS_NOT_THREAD_SAFE() {

  CHECK(THistSvcHLT::initialize());
  CHECK(setProperties());

  msg().setLevel(outputLevel());
  // fix summing up flag if not running in separate threads
 
  // Protect against multiple instances of TROOT
  if ( 0 == gROOT )   {
    static TROOT root("root","ROOT I/O");  
    
  } else {
    ATH_MSG_VERBOSE("ROOT already initialized, debug = " << gDebug);
  }
  gErrorIgnoreLevel=kBreak; // ignore warnings see TError.h in ROOT base src

  // compile regexes
  m_excludeTypeRegex = boost::regex(m_excludeType);
  m_includeTypeRegex = boost::regex(m_includeType);
  m_excludeNameRegex = boost::regex(m_excludeName);
  m_includeNameRegex = boost::regex(m_includeName);
  
  // Retrieve and set OH mutex
  ATH_MSG_INFO("Enabling use of OH histogram mutex");
  lock_histogram_mutex::set_histogram_mutex(
      hltinterface::IInfoRegister::instance()->getPublicationMutex());
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TrigMonTHistSvc::reinitialize() {
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TrigMonTHistSvc::finalize() {
    
  // Reset OH mutex
  ATH_MSG_DEBUG("Resetting OH histogram mutex");
  lock_histogram_mutex::reset_histogram_mutex();
        
  hltinterface::IInfoRegister::THList thlist;
  hltinterface::IInfoRegister::instance()->get(".*", thlist); 
  hltinterface::IInfoRegister::THList::iterator lIter;
  // just listing
  for ( lIter = thlist.begin() ; lIter != thlist.end(); ++lIter ){
    ATH_MSG_INFO("Histogram " <<  lIter->first << " ");    
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigMonTHistSvc::regHist(const std::string& id) {  
  ATH_MSG_DEBUG("registering histogram: " << id <<  " to get it");
  return THistSvcHLT::regHist(id);
}


StatusCode TrigMonTHistSvc::regHist(const std::string& id, std::unique_ptr<TH1> hist) {
  return regHist_i(std::move(hist), id, false);
}

StatusCode TrigMonTHistSvc::regHist(const std::string& id, std::unique_ptr<TH1> hist, TH1* hist_ptr) {
  // This is only to support a common use case where the histogram is used after its registration
  if ( hist_ptr != nullptr ) {
    hist_ptr = hist.get();
  }
  return regHist_i( std::move( hist ), id, false );
}
  
StatusCode TrigMonTHistSvc::regHist(const std::string& id, TH1* hist_ptr) {
  std::unique_ptr<TH1> hist( hist_ptr );
  return regHist_i( std::move( hist ), id, false );
}





StatusCode TrigMonTHistSvc::regTree(const std::string& id) {
  return THistSvcHLT::regTree(id);
}

StatusCode TrigMonTHistSvc::regTree(const std::string&, std::unique_ptr<TTree> tree) {
  // we do not support the trees; then let's make them not to be memory consumers
  ATH_MSG_WARNING("The ROOT TTree is not supported by this service" 
                  << " TTree \"" << tree->GetName() << "\" will be unsaved but will not consume all memory as well" 
                  );
  tree->SetCircular(1);
  return StatusCode::SUCCESS;
}

StatusCode TrigMonTHistSvc::regTree(const std::string& id, TTree* tree_ptr) {
  std::unique_ptr<TTree> tree( tree_ptr );
  return regTree(id, std::move(tree));
}

StatusCode TrigMonTHistSvc::getTree(const std::string&, TTree*&) const {
  ATH_MSG_WARNING("The ROOT TTree is not supported by this service");
  return StatusCode::SUCCESS;
}


StatusCode TrigMonTHistSvc::regGraph(const std::string& id) {
  return THistSvcHLT::regGraph(id);
}

StatusCode TrigMonTHistSvc::regGraph(const std::string&, std::unique_ptr<TGraph>) {
  ATH_MSG_WARNING("The regGraph is not suported by this service");
  return StatusCode::SUCCESS; 
}

StatusCode TrigMonTHistSvc::regGraph(const std::string&, TGraph*) {
  ATH_MSG_WARNING("The regGraph is not suported by this service");
  return StatusCode::SUCCESS; 
}

StatusCode TrigMonTHistSvc::getGraph(const std::string& id, TGraph*& g) const {
  return THistSvcHLT::getGraph(id, g); 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
template <typename T> StatusCode TrigMonTHistSvc::regHist_i(std::unique_ptr<T> hist_unique, const std::string& id, bool /*shared*/) {

  // We need to pass ownership to the hist registry now
  T* hist = nullptr;
  if( hist_unique.get() != nullptr ) {
    hist = hist_unique.release();
  }
  
  std::string regid;
  if ( isObjectAllowed(id, hist).isFailure() ) {
	return  StatusCode::FAILURE;
  }
  regid = id;
  
  if (hist->Class()->InheritsFrom(TH1::Class())) {
    hltinterface::IInfoRegister::instance()->registerTObject(name(), regid, hist);
    ATH_MSG_DEBUG("Histogram " << hist->GetName() 
                  << " registered under " << regid << " " << name());
  } else {
    ATH_MSG_ERROR("Trying to register " << hist->ClassName() 
                  << " but this does not inherit from TH1 histogram");
    return StatusCode::SUCCESS; 
  }
  return StatusCode::SUCCESS; 
}

StatusCode TrigMonTHistSvc::deReg(TObject* optr) {

  std::map<std::string, std::vector<TObject*> > allObjs;
  std::map<std::string, std::vector<TObject*> >::const_iterator mapIt;
  hltinterface::IInfoRegister::instance()->getUnsummed(".+", allObjs); 
  
  // we need now to loop over all this histograms and find if the 
  for ( mapIt = allObjs.begin(); mapIt != allObjs.end(); ++mapIt ) {
    for (std::vector<TObject*>::const_iterator tIt = mapIt->second.begin(); tIt != mapIt->second.end(); ++tIt ) { 
      if ( optr == *tIt ) {
        ATH_MSG_DEBUG("Found histogram by pointer " << optr << " booked under " 
                      << mapIt->first << " will deregister it");
	return deReg(mapIt->first);
      }  
    }
  }    
  return StatusCode::FAILURE;  
}

StatusCode TrigMonTHistSvc::deReg(const std::string& id ) {

  hltinterface::IInfoRegister::instance()->releaseTObject(name(), id);
  ATH_MSG_DEBUG("Deregistration of " << id << " done");
  return StatusCode::SUCCESS;
}

std::vector<std::string> TrigMonTHistSvc::getHists() const {
  std::vector<std::string> l = THistSvcHLT::getHists();   // we need to update the list by objects holded by offline SVC
                 
  hltinterface::IInfoRegister::THList thlist;
  hltinterface::IInfoRegister::instance()->get(".+", thlist); 
  hltinterface::IInfoRegister::THList::iterator lIter;

  l.reserve(l.size()+thlist.size());
  for ( lIter = thlist.begin() ; lIter != thlist.end(); ++lIter ){
    l.push_back(lIter->first);                     // by ones from online
  }
  return l;
}


std::vector<std::string> TrigMonTHistSvc::getTrees() const {

  // we do not keep TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  std::vector<std::string> l;
  return l;
}


StatusCode TrigMonTHistSvc::getTHists(TDirectory *td, TList &tl) const {
  return getTHists_i(std::string(td->GetPath()), tl);
}

StatusCode TrigMonTHistSvc::getTHists(const std::string& dir, TList &tl) const {
  return getTHists_i(dir, tl);
}


StatusCode TrigMonTHistSvc::getTTrees(TDirectory *, TList &) const {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}

StatusCode TrigMonTHistSvc::getTTrees(const std::string&, TList &) const {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}


//
//==========================================================================
//  internal methods 
//==========================================================================
//
StatusCode TrigMonTHistSvc::isObjectAllowed(std::string path, const TObject *o) {

    boost::cmatch what;

    if ( ! boost::regex_match(o->ClassName(),
			      what, m_includeTypeRegex) ) {
      ATH_MSG_WARNING("Object " << path 
                      << " of type " << o->ClassName()
                      << " does NOT match IncludeTypeRegex \"" 
                      << m_includeType << "\"");
      return StatusCode::FAILURE;
    }
      
    if ( boost::regex_match(o->ClassName(),
			    what, m_excludeTypeRegex) ) {
      ATH_MSG_WARNING("Object " << path 
                      << " of type " << o->ClassName()
                      << " matches ExcludeTypeRegex \"" 
                      << m_excludeType << "\"");
      return StatusCode::FAILURE;
    }

    if ( ! boost::regex_match(path.c_str(), what, m_includeNameRegex) ) {
      ATH_MSG_WARNING("Object " << path 
                      << " does NOT match IncludeNameRegex \"" 
                      << m_includeName << "\"");
      return StatusCode::FAILURE;
    }

    if ( boost::regex_match(path.c_str(), what, m_excludeNameRegex) ) {
      ATH_MSG_WARNING("Object " << path 
                      << " matches ExcludeNameRegex \"" 
                      << m_excludeName << "\"");
      return StatusCode::FAILURE;
    }
    // temp hack
    return StatusCode::SUCCESS;

}

StatusCode TrigMonTHistSvc::getTHists_i(const std::string& dir, TList &tl) const {
    hltinterface::IInfoRegister::THList thlist;
    hltinterface::IInfoRegister::instance()->get(".+", thlist); 
    hltinterface::IInfoRegister::THList::iterator lIter;

    for ( lIter = thlist.begin() ; lIter != thlist.end(); ++lIter ){
	if ( lIter->first.find(dir) == 0 ) {
	    // histogram booking path starts from the dir 
	    tl.Add(lIter->second);
	}
    }
    return StatusCode::SUCCESS;
}

//
//==========================================================================
//  Declarations to avoid compiler warning about hidden methods 
//==========================================================================
//

StatusCode TrigMonTHistSvc::getTHists(TDirectory *td, TList &tl, bool recurse) const {
  if (recurse != false)
    ATH_MSG_DEBUG("Recursive flag is not supported in this implementation");
  return getTHists_i(std::string(td->GetPath()), tl);
}

StatusCode TrigMonTHistSvc::getTHists(const std::string& dir, TList &tl, bool recurse) const {
  if (recurse != false)
      ATH_MSG_DEBUG("Recursive flag is not supported in this implementation");
  return getTHists_i(dir, tl);
}

StatusCode TrigMonTHistSvc::getTHists(TDirectory *td, TList &tl, bool recurse, bool reg) {
  if ((recurse != false) || (reg != false))
    ATH_MSG_DEBUG("Recursive flag and automatic registration flag is not supported in this implementation");
  return getTHists_i(std::string(td->GetPath()), tl);
}

StatusCode TrigMonTHistSvc::getTHists(const std::string& dir, TList &tl, bool recurse, bool reg) {
  if ((recurse != false) || (reg != false))
    ATH_MSG_DEBUG("Recursive flag and automatic registration flag is not supported in this implementation");
  return getTHists_i(dir, tl);
}

StatusCode TrigMonTHistSvc::getTTrees(TDirectory *, TList &, bool) const {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}
StatusCode TrigMonTHistSvc::getTTrees(const std::string&, TList &, bool) const {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}
StatusCode TrigMonTHistSvc::getTTrees(TDirectory *, TList &, bool, bool) {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}
StatusCode TrigMonTHistSvc::getTTrees(const std::string&, TList &, bool, bool) {
  // we do not have TTrees
  ATH_MSG_DEBUG("TTres not suported by this implementation");
  return StatusCode::SUCCESS;
}

bool TrigMonTHistSvc::exists( const std::string& name ) const {
  return THistSvcHLT::exists(name);
}

StatusCode TrigMonTHistSvc::notSupported() const {
  ATH_MSG_WARNING("Shared histograms are not yet supported in TrigMonTHistSvc");
  return StatusCode::SUCCESS;
}
