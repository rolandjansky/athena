// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONTHISTSVC_THISTSVC_H
#define TRIGMONTHISTSVC_THISTSVC_H


/**
 * @file TrigMonTHistSvc.h
 * @author Tomasz Bold
 * @date 2005-09-27
 * @brief The HLT histogramming service 
 * It forwards histograms to the monitoring infrastructure.
 */


#include "GaudiKernel/Service.h"
#include "THistSvcHLT.h"
#include "AthenaBaseComps/AthMessaging.h"

class TObject;
class TH1;
class TH2;
class TH3;
class TGraph;
class TTree;

#include <vector>
#include <string>

#include <boost/regex.hpp>

// Forward declarations
template <class TYPE> class SvcFactory;

class TrigMonTHistSvc: virtual public THistSvcHLT,
                       public AthMessaging
{ 
public:
  
  // fwd compat w/ gaudi-21
  using AthMessaging::msg;

  // implemenataion of the interface from TrigNetTHistSvc
  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();
  
  // Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, 
  				     void** ppvInterface );
  

  virtual StatusCode regHist(const std::string& name);
  virtual StatusCode regHist(const std::string& name, TH1*);
  virtual StatusCode regHist(const std::string& name, TH2*);
  virtual StatusCode regHist(const std::string& name, TH3*);
  
  virtual StatusCode getHist(const std::string& name, TH1*&) const;
  virtual StatusCode getHist(const std::string& name, TH2*&) const;
  virtual StatusCode getHist(const std::string& name, TH3*&) const;

  virtual StatusCode regTree(const std::string& name);
  virtual StatusCode regTree(const std::string& name, TTree*);
  virtual StatusCode getTree(const std::string& name, TTree*&) const;

  // new since Gaudi 0.16.1.11
  virtual StatusCode deReg(TObject* obj);            //<! very slow
  virtual StatusCode deReg(const std::string& name); //<! use this instead

  virtual std::vector<std::string> getHists() const;
  virtual std::vector<std::string> getTrees() const;

  virtual StatusCode getTHists(TDirectory *td, TList &) const;
  virtual StatusCode getTHists(const std::string& name, TList &) const;

  virtual StatusCode getTTrees(TDirectory *td, TList &) const;
  virtual StatusCode getTTrees(const std::string& name, TList &) const;

  // new since Gaudi 19
  virtual StatusCode regGraph(const std::string& name);
  virtual StatusCode regGraph(const std::string& name, TGraph*);
  virtual StatusCode getGraph(const std::string& name, TGraph*&) const;

  TrigMonTHistSvc(const std::string& name, ISvcLocator *svc );

  // new since Gaudi 20

  virtual StatusCode getTHists(TDirectory *td, TList &,
                               bool recurse=false) const;
  virtual StatusCode getTHists(const std::string& name, TList &,
                               bool recurse=false) const;
  virtual StatusCode getTHists(TDirectory *td, TList &tl,
                               bool recurse=false, bool reg=false);
  virtual StatusCode getTHists(const std::string& name, TList &tl,
                               bool recurse=false, bool reg=false);

  virtual StatusCode getTTrees(TDirectory *td, TList &,
                               bool recurse=false) const;
  virtual StatusCode getTTrees(const std::string& name, TList &,
                               bool recurse=false) const;
  virtual StatusCode getTTrees(TDirectory *td, TList & tl,
                               bool recurse=false, bool reg=false);
  virtual StatusCode getTTrees(const std::string& name, TList & tl,
                               bool recurse=false, bool reg=false);

  virtual bool exists(const std::string& name) const;

protected:
  StatusCode  isObjectAllowed(std::string path, const TObject *o);

  StatusCode getTHists_i(const std::string& name, TList &) const;
  
  virtual ~TrigMonTHistSvc();

private:
  template <typename T>
  StatusCode regHist_i(T* hist, const std::string& name);
  bool m_add;
  std::string m_excludeType;
  std::string m_includeType;
  std::string m_excludeName;
  std::string m_includeName;
  // compiled regexes
  boost::regex m_excludeTypeRegex;
  boost::regex m_includeTypeRegex;
  boost::regex m_excludeNameRegex;
  boost::regex m_includeNameRegex;
};

#endif // TRIGMONTHISTSVC_THISTSVC_H
