// -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

class TrigMonTHistSvc: virtual public THistSvcHLT,
                       public AthMessaging
{ 
public:
  
  // fwd compat w/ gaudi-21
  using AthMessaging::msg;

  TrigMonTHistSvc(const std::string& name, ISvcLocator *svc );
  virtual ~TrigMonTHistSvc();

  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  
  // implemenataion of the interface from TrigNetTHistSvc
  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();
  
  virtual StatusCode regHist(const std::string& name);
  virtual StatusCode regHist(const std::string& name, std::unique_ptr<TH1> hist);
  virtual StatusCode regHist(const std::string& name, std::unique_ptr<TH1> hist, TH1* hist_ptr);
  virtual StatusCode regHist(const std::string& name, TH1*);

  virtual StatusCode regTree(const std::string& name);
  virtual StatusCode regTree(const std::string& name, std::unique_ptr<TTree>);
  virtual StatusCode regTree(const std::string& name, TTree*);
  virtual StatusCode getTree(const std::string& name, TTree*&) const;

  virtual StatusCode deReg(TObject* obj);            //<! very slow
  virtual StatusCode deReg(const std::string& name); //<! use this instead

  virtual std::vector<std::string> getHists() const;
  virtual std::vector<std::string> getTrees() const;

  virtual StatusCode getTHists(TDirectory *td, TList &) const;
  virtual StatusCode getTHists(const std::string& name, TList &) const;

  virtual StatusCode getTTrees(TDirectory *td, TList &) const;
  virtual StatusCode getTTrees(const std::string& name, TList &) const;

  virtual StatusCode regGraph(const std::string& name);
  virtual StatusCode regGraph(const std::string& name, std::unique_ptr<TGraph>);
  virtual StatusCode regGraph(const std::string& name, TGraph*);
  virtual StatusCode getGraph(const std::string& name, TGraph*&) const;

  virtual StatusCode regShared( const std::string&, std::unique_ptr<TH1>, LockedHandle<TH1>& ) { return notSupported(); }
  virtual StatusCode regShared( const std::string&, std::unique_ptr<TH2>, LockedHandle<TH2>& ) { return notSupported(); }
  virtual StatusCode regShared( const std::string&, std::unique_ptr<TH3>, LockedHandle<TH3>& ) { return notSupported(); }
  virtual StatusCode regShared( const std::string&, std::unique_ptr<TGraph>, LockedHandle<TGraph>& )  { return notSupported(); }
  virtual StatusCode getShared( const std::string&, LockedHandle<TH1>& ) const { return notSupported(); }
  virtual StatusCode getShared( const std::string&, LockedHandle<TH2>& ) const { return notSupported(); }
  virtual StatusCode getShared( const std::string&, LockedHandle<TH3>& ) const { return notSupported(); }
  virtual StatusCode getShared( const std::string&, LockedHandle<TGraph>& ) const { return notSupported(); }

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
  StatusCode isObjectAllowed(std::string path, const TObject *o);

  StatusCode getTHists_i(const std::string& name, TList &) const;
  

private:
  template <typename T>
  StatusCode regHist_i(std::unique_ptr<T> hist, const std::string& name, bool shared);
  StatusCode notSupported() const;

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
