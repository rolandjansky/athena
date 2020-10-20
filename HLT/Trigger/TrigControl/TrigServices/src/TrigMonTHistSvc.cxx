/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonTHistSvc.h"

#include "GaudiKernel/ISvcLocator.h"

#include "CxxUtils/checker_macros.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"

#include "hltinterface/IInfoRegister.h"

#include "TError.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TObject.h"
#include "TROOT.h"
#include "TTree.h"

TrigMonTHistSvc::TrigMonTHistSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc)
{}

StatusCode TrigMonTHistSvc::initialize ATLAS_NOT_THREAD_SAFE()
{
  // Protect against multiple instances of TROOT
  if (0 == gROOT) {
    static TROOT root("root", "ROOT I/O");
  }
  else {
    ATH_MSG_VERBOSE("ROOT already initialized, debug = " << gDebug);
  }
  gErrorIgnoreLevel = kBreak; // ignore warnings see TError.h in ROOT base src

  // compile regexes
  m_excludeTypeRegex = boost::regex(m_excludeType.value());
  m_includeTypeRegex = boost::regex(m_includeType.value());
  m_excludeNameRegex = boost::regex(m_excludeName.value());
  m_includeNameRegex = boost::regex(m_includeName.value());

  // Retrieve and set OH mutex
  ATH_MSG_INFO("Enabling use of OH histogram mutex");
  oh_lock_histogram_mutex::set_histogram_mutex(
      hltinterface::IInfoRegister::instance()->getPublicationMutex());

  return StatusCode::SUCCESS;
}

StatusCode TrigMonTHistSvc::stop()
{
  // Clear list of histograms (actual TH is not owned by us)
  m_hists.clear();

  return StatusCode::SUCCESS;
}

StatusCode TrigMonTHistSvc::finalize()
{
  // Reset OH mutex
  ATH_MSG_DEBUG("Resetting OH histogram mutex");
  oh_lock_histogram_mutex::reset_histogram_mutex();
        
  return StatusCode::SUCCESS;
}

template <typename T>
StatusCode TrigMonTHistSvc::regHist_i(std::unique_ptr<T> hist_unique, const std::string& id,
                                      bool shared, THistID*& phid)
{
  std::scoped_lock lock(m_svcMut);

  // We need to pass ownership to the hist registry now
  phid = nullptr;
  T* hist = nullptr;
  if (hist_unique.get() != nullptr) {
    hist = hist_unique.release();
  }

  if (not isObjectAllowed(id, hist)) {
    return StatusCode::FAILURE;
  }

  if (hist->Class()->InheritsFrom(TH1::Class())) {
    if (hltinterface::IInfoRegister::instance()->registerTObject(name(), id, hist)) {
      auto [iter,inserted] = m_hists.try_emplace(id, id, hist);
      if (not inserted) {
        ATH_MSG_ERROR("Histogram with name " << id << " already registered");
        return StatusCode::FAILURE;
      }
      if (shared) iter->second.mutex = new std::mutex;
      phid = &iter->second;
      ATH_MSG_DEBUG((shared ? "Shared histogram " : "Histogram ")
                    << hist->GetName() << " registered under " << id << " " << name());
    }
    else {
      ATH_MSG_ERROR("Registration of " << hist->ClassName() << " with IInfoRegister failed");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_ERROR("Cannot register " << hist->ClassName()
                                     << " because it does not inherit from TH1");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

template <typename T>
LockedHandle<T> TrigMonTHistSvc::regShared_i(const std::string& id, std::unique_ptr<T> hist)
{
  std::scoped_lock lock(m_svcMut);

  LockedHandle<T> lh(nullptr, nullptr);
  const auto& h = m_hists.find(id);

  // No histogram under that id yet
  if (h == m_hists.end()) {
    T* phist = hist.get();
    THistID* phid = nullptr;
    if (regHist_i(std::move(hist), id, true, phid).isSuccess()) {
      lh.set(phist, phid->mutex);
    }
  }
  // Histogram already registered under that id
  else {
    if (h->second.mutex == nullptr) {
      ATH_MSG_ERROR("regShared: previously registered histogram \"" << id
                                                                    << "\" was not marked shared");
    }
    T* phist = dynamic_cast<T*>(h->second.obj);
    if (phist == nullptr) {
      ATH_MSG_ERROR("regShared: unable to dcast retrieved shared hist \""
                    << id << "\" of type " << h->second.obj->IsA()->GetName()
                    << " to requested type " << System::typeinfoName(typeid(T)));
    }
    else {
      lh.set(phist, h->second.mutex);
      //hist is automatically deleted at end of method
    }
  }
  return lh;
}

template <typename T>
T* TrigMonTHistSvc::getHist_i(const std::string& id, const size_t& /*ind*/, bool quiet) const
{
  std::scoped_lock lock(m_svcMut);

  const auto& h = m_hists.find(id);
  if (h == m_hists.end()) {
    if (!quiet) ATH_MSG_ERROR("could not locate Hist with id \"" << id << "\"");
    return nullptr;
  }

  T* phist = dynamic_cast<T*>(h->second.obj);
  if (phist == nullptr) {
    ATH_MSG_ERROR("getHist: unable to dcast retrieved shared hist \""
                  << id << "\" of type " << h->second.obj->IsA()->GetName() << " to requested type "
                  << System::typeinfoName(typeid(T)));
    return nullptr;
  }
  return phist;
}

StatusCode TrigMonTHistSvc::getTHists_i(const std::string& dir, TList& tl) const
{
  std::scoped_lock lock(m_svcMut);
  for (const auto& [id, h] : m_hists) {
    if (id.find(dir) == 0) { // histogram booking path starts from the dir
      tl.Add(h.obj);
    }
  }
  return StatusCode::SUCCESS;
}

template <typename T>
LockedHandle<T> TrigMonTHistSvc::getShared_i(const std::string& id) const
{
  std::scoped_lock lock(m_svcMut);
  const auto& h = m_hists.find(id);
  if (h != m_hists.end()) {
    if (h->second.mutex == nullptr) {
      ATH_MSG_ERROR("getShared: found Hist with id \"" << id
                                                       << "\", but it's not marked as shared");
      return {};
    }
    T* phist = dynamic_cast<T*>(h->second.obj);
    if (phist == nullptr) {
      ATH_MSG_ERROR("getShared: unable to dcast retrieved shared hist \""
                    << id << "\" of type " << h->second.obj->IsA()->GetName()
                    << " to requested type " << System::typeinfoName(typeid(T)));
      return {};
    }
    return LockedHandle<T>(phist, h->second.mutex);
  }
  ATH_MSG_ERROR("getShared: cannot find histogram with id \"" << id << "\"");
  return {};
}

StatusCode TrigMonTHistSvc::deReg(TObject* optr)
{
  std::scoped_lock lock(m_svcMut);
  // Find the relevant histogram and deregister it
  for (const auto& [name, histid] : m_hists) {
    if (histid.obj == optr) {
      ATH_MSG_DEBUG("Found histogram " << optr << " booked under " << name
                                       << " and will deregister it");
      return deReg(name);
    }
  }
  return StatusCode::FAILURE;
}

StatusCode TrigMonTHistSvc::deReg(const std::string& id)
{
  hltinterface::IInfoRegister::instance()->releaseTObject(name(), id);
  ATH_MSG_DEBUG("Deregistration of " << id << " done");
  return StatusCode::SUCCESS;
}

std::vector<std::string> TrigMonTHistSvc::getHists() const
{
  std::scoped_lock lock(m_svcMut);
  std::vector<std::string> l;
  l.reserve(m_hists.size());
  for (const auto& h : m_hists) l.push_back(h.first);
  return l;
}

bool TrigMonTHistSvc::isObjectAllowed(const std::string& path, const TObject* o) const
{
  boost::cmatch what;

  if (not boost::regex_match(o->ClassName(), what, m_includeTypeRegex)) {
    ATH_MSG_WARNING("Object " << path << " of type " << o->ClassName()
                              << " does NOT match IncludeType \"" << m_includeType << "\"");
    return false;
  }

  if (boost::regex_match(o->ClassName(), what, m_excludeTypeRegex)) {
    ATH_MSG_WARNING("Object " << path << " of type " << o->ClassName() << " matches ExcludeType \""
                              << m_excludeType << "\"");
    return false;
  }

  if (not boost::regex_match(path.c_str(), what, m_includeNameRegex)) {
    ATH_MSG_WARNING("Object " << path << " does NOT match IncludeName \"" << m_includeName << "\"");
    return false;
  }

  if (boost::regex_match(path.c_str(), what, m_excludeNameRegex)) {
    ATH_MSG_WARNING("Object " << path << " matches ExcludeName \"" << m_excludeName << "\"");
    return false;
  }

  return true;
}

bool TrigMonTHistSvc::existsHist(const std::string& name) const
{
  return (getHist_i<TH1>(name, 0, true) != nullptr);
}

/**************************************************************************************
 * Typed interface methods
 * All these are just forwarding to the templated xyz_i methods
 **************************************************************************************/
StatusCode TrigMonTHistSvc::regHist(const std::string& id)
{
  std::unique_ptr<TH1> hist = nullptr;
  THistID* hid = nullptr;
  return regHist_i(std::move(hist), id, false, hid);
}

StatusCode TrigMonTHistSvc::regHist(const std::string& id, std::unique_ptr<TH1> hist)
{
  THistID* hid = nullptr;
  return regHist_i(std::move(hist), id, false, hid);
}

StatusCode TrigMonTHistSvc::regHist(const std::string& id, std::unique_ptr<TH1> hist, TH1* /*hist_ptr*/)
{
  THistID* hid = nullptr;
  return regHist_i(std::move(hist), id, false, hid);
}

StatusCode TrigMonTHistSvc::regHist(const std::string& id, TH1* hist_ptr)
{
  THistID* hid = nullptr;
  std::unique_ptr<TH1> hist(hist_ptr);
  return regHist_i(std::move(hist), id, false, hid);
}

/**************************************************************************************/

StatusCode TrigMonTHistSvc::getHist(const std::string& id, TH1*& hist, size_t ind) const
{
  hist = getHist_i<TH1>(id, ind);
  return (hist != nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::getHist(const std::string& id, TH2*& hist, size_t ind) const
{
  hist = getHist_i<TH2>(id, ind);
  return (hist != nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::getHist(const std::string& id, TH3*& hist, size_t ind) const
{
  hist = getHist_i<TH3>(id, ind);
  return (hist != nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

/**************************************************************************************/

StatusCode TrigMonTHistSvc::getTHists(TDirectory* td, TList& tl, bool recurse) const
{
  if (recurse) ATH_MSG_DEBUG("Recursive flag is not supported in this implementation");
  return getTHists_i(std::string(td->GetPath()), tl);
}

StatusCode TrigMonTHistSvc::getTHists(const std::string& dir, TList& tl, bool recurse) const
{
  if (recurse) ATH_MSG_DEBUG("Recursive flag is not supported in this implementation");
  return getTHists_i(dir, tl);
}

StatusCode TrigMonTHistSvc::getTHists(TDirectory* td, TList& tl, bool recurse, bool reg)
{
  if (recurse || reg)
    ATH_MSG_DEBUG("Recursive flag and automatic registration flag is not "
                  "supported in this implementation");
  return getTHists_i(std::string(td->GetPath()), tl);
}

StatusCode TrigMonTHistSvc::getTHists(const std::string& dir, TList& tl, bool recurse, bool reg)
{
  if (recurse || reg)
    ATH_MSG_DEBUG("Recursive flag and automatic registration flag is not "
                  "supported in this implementation");
  return getTHists_i(dir, tl);
}

/**************************************************************************************/

StatusCode TrigMonTHistSvc::regShared(const std::string& id, std::unique_ptr<TH1> hist,
                                      LockedHandle<TH1>& lh)
{
  lh = regShared_i<TH1>(id, std::move(hist));
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::regShared(const std::string& id, std::unique_ptr<TH2> hist,
                                      LockedHandle<TH2>& lh)
{
  lh = regShared_i<TH2>(id, std::move(hist));
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::regShared(const std::string& id, std::unique_ptr<TH3> hist,
                                      LockedHandle<TH3>& lh)
{
  lh = regShared_i<TH3>(id, std::move(hist));
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

/**************************************************************************************/

StatusCode TrigMonTHistSvc::getShared(const std::string& id, LockedHandle<TH1>& lh) const
{
  lh = getShared_i<TH1>(id);
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::getShared(const std::string& id, LockedHandle<TH2>& lh) const
{
  lh = getShared_i<TH2>(id);
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}

StatusCode TrigMonTHistSvc::getShared(const std::string& id, LockedHandle<TH3>& lh) const
{
  lh = getShared_i<TH3>(id);
  return (lh ? StatusCode::SUCCESS : StatusCode::FAILURE);
}
