/*
 * This is a copy of GaudiSvc/src/THistSvc with THistSvc renamed to THistSvcHLT
 * to allow inheritance of this class by TrigMonTHistSvc.
 */
#ifndef THISTSVCHLT_H
#define THISTSVCHLT_H

// system includes:
#include <map>
#include <set>
#include <string>
#include <vector>

// Gaudi includes:
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

// ROOT includes:
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TList.h"
#include "TObject.h"
#include "TTree.h"

#include "AthenaBaseComps/AthService.h"

class IIncidentSvc;
class THistSvcHLT: public extends3<Service, ITHistSvc, IIncidentListener, IIoComponent> {

class THistSvcHLT : public extends<AthService, ITHistSvc, IIncidentListener, IIoComponent>
{
public:
  
  THistSvcHLT(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override;
  StatusCode reinitialize() override;
  StatusCode finalize() override;
  
  // Methods from ITHistSvc
  /// @name Functions to manage ROOT histograms of any kind
  /// @{
  StatusCode regHist(const std::string& name) override;
  StatusCode regHist(const std::string& name, std::unique_ptr<TH1> hist) override;
  StatusCode regHist(const std::string& name, std::unique_ptr<TH1> hist, TH1* hist_ptr) override;
  StatusCode regHist(const std::string& name, TH1*) override;
  
  StatusCode getHist(const std::string& name, TH1*&, size_t index = 0) const override;
  StatusCode getHist(const std::string& name, TH2*&, size_t index = 0) const override;
  StatusCode getHist(const std::string& name, TH3*&, size_t index = 0) const override;
  /// @}

  /// @name Functions to manage TTrees
  /// @{
  StatusCode regTree(const std::string& name) override;
  StatusCode regTree(const std::string& name, TTree* tree) override;
  StatusCode regTree(const std::string& name, std::unique_ptr<TTree> tree) override;
  
  StatusCode getTree( const std::string& name, TTree*& tree) const override;
  /// @}

  /// @name Functions to manage TGraphs
  /// @{
  StatusCode regGraph(const std::string& name) override;
  StatusCode regGraph(const std::string& name, std::unique_ptr<TGraph> graph) override;
  StatusCode regGraph(const std::string& name, TGraph* graph) override;
  
  StatusCode getGraph(const std::string& name, TGraph*& graph) const override;
  /// @}

  /// @name Functions to manage shared objects
  /// @{
  StatusCode regShared(const std::string& name, std::unique_ptr<TH1> hist, LockedHandle<TH1>& lh) override;
  StatusCode regShared(const std::string& name, std::unique_ptr<TH2> hist, LockedHandle<TH2>& lh) override;
  StatusCode regShared(const std::string& name, std::unique_ptr<TH3> hist, LockedHandle<TH3>& lh) override;
  StatusCode regShared(const std::string& name, std::unique_ptr<TGraph> graph, LockedHandle<TGraph>& lh) override;
  
  StatusCode getShared(const std::string& name, LockedHandle<TH1>& lh) const override;
  StatusCode getShared(const std::string& name, LockedHandle<TH2>& lh) const override;
  StatusCode getShared(const std::string& name, LockedHandle<TH3>& lh) const override;
  StatusCode getShared(const std::string& name, LockedHandle<TGraph>& lh) const override;
  /// @}
  
  /// @name Functions that work on any TObject in the THistSvcMT
  /// @{
  StatusCode deReg(const std::string& name) override;
  StatusCode deReg(TObject* obj) override;

  StatusCode merge(const std::string& id) override;
  StatusCode merge(TObject* obj) override;

  bool exists( const std::string& name ) const override;
  /// @}

  /// @name Functions returning lists of all histograms, trees and graphs
  /// @{
  std::vector<std::string> getHists() const override;
  std::vector<std::string> getTrees() const override;
  std::vector<std::string> getGraphs() const override;

  StatusCode getTHists(TDirectory* td, TList&, bool recurse = false) const override;
  StatusCode getTHists(const std::string& name, TList&, bool recurse = false) const override;
  StatusCode getTHists(TDirectory* td, TList& tl, bool recurse = false, bool reg = false) override;
  StatusCode getTHists(const std::string& name, TList& tl, bool recurse = false, bool reg = false) override;

  StatusCode getTTrees(TDirectory* td, TList&, bool recurse = false) const override;
  StatusCode getTTrees(const std::string& name, TList&, bool recurse = false) const override;
  StatusCode getTTrees(TDirectory* td, TList& tl, bool recurse = false, bool reg = false) override;
  StatusCode getTTrees(const std::string& name, TList& tl, bool recurse = false, bool reg = false) override;
  /// @}

public:
  // Methods from other interfaces
  // From IIncidentListener
  void handle( const Incident& ) override;

  // From IIoComponent
  StatusCode io_reinit() override;

protected:
  ~THistSvcHLT() override = default;

};

// Include template implementation
#include "THistSvcHLT.icc"

#endif // THISTSVCHLT_H
