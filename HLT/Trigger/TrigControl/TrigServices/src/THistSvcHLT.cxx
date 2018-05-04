/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "THistSvcHLT.h"

DECLARE_COMPONENT(THistSvcHLT)

THistSvcHLT::THistSvcHLT(const std::string& name, ISvcLocator* svcLoc)
: base_class(name, svcLoc)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  declareProperty("AutoSave",          m_autoSave=0);
  declareProperty("AutoFlush",         m_autoFlush=0);
  declareProperty("PrintAll",          m_print=false);
  declareProperty("MaxFileSize",       m_maxFileSize=10240,
                  "maximum file size in MB. if exceeded, will cause an abort. -1 to never check.");
  declareProperty("CompressionLevel",  m_compressionLevel=1);
                  /*->declareUpdateHandler(&THistSvcHLT::setupCompressionLevel, this);*/
  declareProperty("Output",            m_outputfile);
                  /*->declareUpdateHandler(&THistSvcHLT::setupOutputFile, this);*/
  declareProperty("Input",             m_inputfile);
                  /*->declareUpdateHandler(&THistSvcHLT::setupInputFile,  this);*/
  verbose() << "end of " << __FUNCTION__ << endmsg;
}

StatusCode THistSvcHLT::initialize()
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::reinitialize()
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::finalize()
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regHist(const std::string& name)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regHist(const std::string& name, std::unique_ptr<TH1> hist)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regHist(const std::string& name, std::unique_ptr<TH1> hist, TH1* hist_ptr)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regHist(const std::string& name, TH1*)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getHist(const std::string& name, TH1*&, size_t index) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getHist(const std::string& name, TH2*&, size_t index) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getHist(const std::string& name, TH3*&, size_t index) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regTree(const std::string& name)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regTree(const std::string& name, TTree* tree)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regTree(const std::string& name, std::unique_ptr<TTree> tree)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTree( const std::string& name, TTree*& tree) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regGraph(const std::string& name)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regGraph(const std::string& name, std::unique_ptr<TGraph> graph)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regGraph(const std::string& name, TGraph* graph)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getGraph(const std::string& name, TGraph*& graph) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regShared(const std::string& name, std::unique_ptr<TH1> hist, LockedHandle<TH1>& lh)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regShared(const std::string& name, std::unique_ptr<TH2> hist, LockedHandle<TH2>& lh)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regShared(const std::string& name, std::unique_ptr<TH3> hist, LockedHandle<TH3>& lh)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::regShared(const std::string& name, std::unique_ptr<TGraph> graph, LockedHandle<TGraph>& lh)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getShared(const std::string& name, LockedHandle<TH1>& lh) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getShared(const std::string& name, LockedHandle<TH2>& lh) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getShared(const std::string& name, LockedHandle<TH3>& lh) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getShared(const std::string& name, LockedHandle<TGraph>& lh) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::deReg(const std::string& name)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::deReg(TObject* obj)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::merge(const std::string& id)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::merge(TObject* obj)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

bool THistSvcHLT::exists( const std::string& name ) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return false;
}

std::vector<std::string> THistSvcHLT::getHists() const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return {};
}

std::vector<std::string> THistSvcHLT::getTrees() const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return {};
}

std::vector<std::string> THistSvcHLT::getGraphs() const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return {};
}

StatusCode THistSvcHLT::getTHists(TDirectory* td, TList&, bool recurse) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTHists(const std::string& name, TList&, bool recurse) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTHists(TDirectory* td, TList& tl, bool recurse, bool reg)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTHists(const std::string& name, TList& tl, bool recurse, bool reg)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTTrees(TDirectory* td, TList&, bool recurse) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTTrees(const std::string& name, TList&, bool recurse) const
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTTrees(TDirectory* td, TList& tl, bool recurse, bool reg)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode THistSvcHLT::getTTrees(const std::string& name, TList& tl, bool recurse, bool reg)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

void THistSvcHLT::handle( const Incident& )
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
}

StatusCode THistSvcHLT::io_reinit()
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

