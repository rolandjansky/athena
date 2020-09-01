/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetToolRunner.cxx 

#include "JetRec/JetToolRunner.h"
#include <iomanip>
#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#include "JetInterface/IJetExecuteTool.h"

using std::string;
using std::setw;
using std::setprecision;
using std::fixed;

//**********************************************************************

JetToolRunner::JetToolRunner(const std::string& myname): 
  AsgTool(myname){
  declareProperty("Timer", m_timer =0);
}

//**********************************************************************

StatusCode JetToolRunner::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_evstools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Eventshape tool retrieval failed.");
  }
  sc = m_exetools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Execute tool retrieval failed.");
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_exetools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  m_execlocks.resize(ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_exetools[itool]->name());
    m_exetools[itool]->print();
    m_execlocks[itool].Reset();
  }
  unsigned int nevstool = m_evstools.size();
  ATH_MSG_INFO("  Tool count: " << nevstool);
  m_evsclocks.resize(nevstool);
  for ( unsigned int itool=0; itool<nevstool; ++itool ) {
    ATH_MSG_INFO("    " << m_evstools[itool]->name());
    m_evstools[itool]->print();
    m_evsclocks[itool].Reset();
  }
  m_nevt = 0;
  m_clock.Reset();
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetToolRunner::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  ATH_MSG_INFO("  Event count: " << m_nevt);
  if ( m_timer > 0 ) {
    double ctime = m_clock.CpuTime();
    double wtime = m_clock.RealTime();
    double actime = 0.0;
    double awtime = 0.0;
    if ( m_nevt > 0 ) {
      actime = ctime/double(m_nevt);
      awtime = wtime/double(m_nevt);
    }
    ATH_MSG_INFO("  Total CPU/wall time: " << ctime << "/" << wtime << " sec");
    ATH_MSG_INFO("   Avg. CPU/wall time: " << actime << "/" << awtime << " sec");
  }
  if ( m_timer > 1 && m_nevt > 0 ) {
    unsigned int nevstool = m_evstools.size();
    ATH_MSG_INFO("  CPU/wall time [s] for " << nevstool << " event shape tools:");
    for ( unsigned int itool=0; itool<nevstool+1; ++itool ) {
      string tname = "TOTAL";
#ifdef USE_BOOST_AUTO
      BOOST_AUTO(pclock, &m_clock);
#else
      TStopwatch* pclock = &m_clock;
#endif
      if ( itool < nevstool ) {
        pclock = &m_evsclocks[itool];
        tname = m_evstools[itool]->name();
        if ( tname.substr(0,8) == "ToolSvc." ) tname = tname.substr(8);
      }
      double tctime = pclock->CpuTime()/double(m_nevt);
      double twtime = pclock->RealTime()/double(m_nevt);
      ATH_MSG_INFO("    " << setw(50) << tname
                   << fixed << setprecision(3) << setw(10) << tctime
                   << fixed << setprecision(3) << setw(10) << twtime);
    }
    unsigned int ntool = m_exetools.size();
    for ( unsigned int itool=0; itool<ntool+1; ++itool ) {
      string tname = "TOTAL";
#ifdef USE_BOOST_AUTO
      BOOST_AUTO(pclock, &m_clock);
#else
      TStopwatch* pclock = &m_clock;
#endif
      if ( itool < ntool ) {
        pclock = &m_execlocks[itool];
        tname = m_exetools[itool]->name();
        if ( tname.substr(0,8) == "ToolSvc." ) tname = tname.substr(8);
      }
      double tctime = pclock->CpuTime()/double(m_nevt);
      double twtime = pclock->RealTime()/double(m_nevt);
      ATH_MSG_INFO("    " << setw(50) << tname
                   << fixed << setprecision(3) << setw(10) << tctime
                   << fixed << setprecision(3) << setw(10) << twtime);
    }
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetToolRunner::execute() const {  
  if ( m_timer > 0 ) m_clock.Start(false);
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  string line = "---------------------------------------------------";
  // Loop over event shape tools.
  unsigned int nevstool = m_evstools.size();
  ATH_MSG_DEBUG("Looping over " << nevstool << " event shape tools.");
  if ( nevstool ) ATH_MSG_DEBUG(line);
  for ( unsigned int itool=0; itool<nevstool; ++itool ) {
    ToolHandle<IEventShapeTool> htool = m_evstools[itool];
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    ATH_MSG_VERBOSE("Retrieving tool with IEventShapeTool interface.");
    if ( m_timer > 1 ) m_evsclocks[itool].Start(false);
    StatusCode sc = htool->fillEventShape();
    if ( m_timer > 1 ) m_evsclocks[itool].Stop();
    if ( ! sc.isSuccess() ) ATH_MSG_INFO("Events shape tool returned error.");
    else ATH_MSG_DEBUG("Tool execution succeeded");
    ATH_MSG_DEBUG(line);
  }
  // Loop over execute tools.
  unsigned int ntool = m_exetools.size();
  ATH_MSG_DEBUG("Looping over " << ntool << " jet execute tools.");
  if ( ntool ) ATH_MSG_DEBUG(line);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ToolHandle<IJetExecuteTool> htool = m_exetools[itool];
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    ATH_MSG_VERBOSE("Retrieving tool with IJetExecuteTool interface.");
    if ( m_timer > 1 ) m_execlocks[itool].Start(false);
    int jstat = htool->execute();
    if ( m_timer > 1 ) m_execlocks[itool].Stop();
    if ( jstat != 0 ) ATH_MSG_INFO("Tool returned error " << jstat);
    else ATH_MSG_DEBUG("Tool execution succeeded");
    ATH_MSG_DEBUG(line);
  }
  ++m_nevt;
  if ( m_timer > 0 ) m_clock.Stop();
  ATH_MSG_DEBUG("  Total CPU/wall time: " << m_clock.CpuTime()
                << "/" << m_clock.RealTime() << " sec");
  return 0;
}

//**********************************************************************

void JetToolRunner::print() const {
  ATH_MSG_INFO("Properties for JetToolRunner " << name());
  ATH_MSG_INFO("  Event Shape tool list has " << m_evstools.size() << " entries:");
  for ( ToolHandleArray<IEventShapeTool>::const_iterator itoo=m_evstools.begin();
        itoo!=m_evstools.end(); ++itoo ) {
    ToolHandle<IEventShapeTool> htool = *itoo;
    ATH_MSG_INFO("    " << htool->name());
  }
  ATH_MSG_INFO("  Execute tool list has " << m_exetools.size() << " entries:");
  for ( ToolHandleArray<IJetExecuteTool>::const_iterator itoo=m_exetools.begin();
        itoo!=m_exetools.end(); ++itoo ) {
    ToolHandle<IJetExecuteTool> htool = *itoo;
    ATH_MSG_INFO("    " << htool->name());
  }
  ATH_MSG_INFO("  Timer: " << m_timer);
}

//**********************************************************************
