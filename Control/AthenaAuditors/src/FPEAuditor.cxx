///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// FPEAuditor.cxx 
// Implementation file for class FPEAuditor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <stdexcept>

// FrameWork includes
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/INamedInterface.h"

#include "FPEAuditor.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"

// C includes
#include <fenv.h>

extern "C" {
  namespace FPEAudit
  {
    extern void fpe_sig_action(int, siginfo_t *, void *);
    extern struct sigaction oldactHandler; 
    extern void mask_fpe(void);
    extern void unmask_fpe(void);
    extern int n_fp_exceptions;
    extern void *s_array_O[];
    extern void *s_array_I[];
    extern void *s_array_D[];
    extern void resolve(void *, MsgStream& , bool print=false);
  }
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
FPEAuditor::FPEAuditor( const std::string& name, 
			ISvcLocator* pSvcLocator ) : 
  Auditor     ( name, pSvcLocator  ),
  AthMessaging(msgSvc(), name),
  m_fpe_stack(),
  m_CountFPEs(),
  m_NstacktracesOnFPE(0),
  m_SigHandInstalled(false),
  //m_flagp(),
  m_env()
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  // for AuditorHandle ? someday ?
  //declareInterface<IAuditor>(this);
  declareProperty("NStacktracesOnFPE", m_NstacktracesOnFPE,
                  "Flag to configure, how many stack traces in case of FPE are printed. "
                  "Default: Zero, just report that FPE has happened. "
                  "After collecting the stacktrace, the code has to modify the mcontext_t "
                  "struct to ignore FPEs for the rest of the processing of the algorithm/service "
                  "This part is highly non-portable!" );
  declareProperty("EventInfoKey",m_evtInfoKey="EventInfo","SG Key of xAOD::EventInfo obj");
}

// Destructor
///////////////
#include <fstream>
#include <iomanip>
FPEAuditor::~FPEAuditor()
{ 
  //m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

StatusCode FPEAuditor::initialize()
{
  for ( unsigned int i(0); i<FPEAUDITOR_ARRAYSIZE; ++i )
    m_CountFPEs[i]=0;
  //ATH_MSG_INFO("==> initialize");
  // add a fake node here because we may come alive while the AuditorSvc is
  // still initializing (so one edge is left orphaned)
  add_fpe_node();

  return StatusCode::SUCCESS;
}

StatusCode FPEAuditor::finalize()
{
  ATH_MSG_INFO("FPE summary for this job");
  ATH_MSG_INFO(" FPE OVERFLOWs  : " << m_CountFPEs[FPEAUDITOR_OVERFLOW] );
  ATH_MSG_INFO(" FPE INVALIDs   : " << m_CountFPEs[FPEAUDITOR_INVALID]);
  ATH_MSG_INFO(" FPE DIVBYZEROs : " << m_CountFPEs[FPEAUDITOR_DIVBYZERO]);
  
  if ( m_SigHandInstalled )
    UninstallHandler();
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void FPEAuditor::InstallHandler()
{
  struct sigaction act;
  
  // Save the current FP environment.
  fegetenv (&m_env);
  
  memset(&act, 0, sizeof act);
  act.sa_sigaction = FPEAudit::fpe_sig_action;
  act.sa_flags = SA_SIGINFO;
  if ( sigaction(SIGFPE, &act, &FPEAudit::oldactHandler) != 0 )
    {
      ATH_MSG_WARNING ("Printing stacktraces on FPE requested, but unable to install signal handler ! Switched off !");
      m_NstacktracesOnFPE=0;
    }
  if ( m_NstacktracesOnFPE )
    {
      ATH_MSG_INFO ("Installed Signalhandler !");
      FPEAudit::unmask_fpe();
      m_SigHandInstalled=true;
    }
  FPEAudit::s_array_O[0]=NULL;
  FPEAudit::s_array_I[0]=NULL;
  FPEAudit::s_array_D[0]=NULL;
}

void FPEAuditor::UninstallHandler()
{
  ATH_MSG_INFO("uninstalling SignalHandler");
  m_SigHandInstalled = false;
  
  feclearexcept(FE_ALL_EXCEPT);
  fesetenv (&m_env);
  FPEAudit::mask_fpe();
  
  // feenableexcept (0);
  // fedisableexcept (FE_ALL_EXCEPT);
  
  FPEAudit::n_fp_exceptions = -1;
}

void FPEAuditor::beforeInitialize(INamedInterface* /*comp*/)
{
  add_fpe_node();
}

void FPEAuditor::afterInitialize(INamedInterface* comp)
{
  //ATH_MSG_INFO("<== ini [" << comp->name() << "]");
  static const std::string step = "initialize";
  report_fpe(step, comp->name());
  pop_fpe_node();


}

void FPEAuditor::beforeReinitialize(INamedInterface* /*comp*/)
{
  add_fpe_node();
}

void FPEAuditor::afterReinitialize( INamedInterface* comp)
{
  static const std::string step = "reinitialize";
  report_fpe(step, comp->name());
  pop_fpe_node();
}

void FPEAuditor::beforeExecute(INamedInterface* /*comp*/)
{
  add_fpe_node();
}

void FPEAuditor::afterExecute( INamedInterface* comp, 
			       const StatusCode& ) 
{
  static const std::string step = "execute";
  report_fpe(step, comp->name());
  pop_fpe_node();
}

void FPEAuditor::beforeBeginRun(INamedInterface* /*comp*/)
{
  add_fpe_node();
  // install new signal handler here, before it will conflict with
  // CoreDumpSvc, which installs its own signal handler for FPE by default
  if ( m_NstacktracesOnFPE && ! m_SigHandInstalled )
    {
      InstallHandler();
      FPEAudit::n_fp_exceptions = m_NstacktracesOnFPE;
    }
}

void FPEAuditor::afterBeginRun(INamedInterface* comp)
{
  static const std::string step = "beginrun";
  report_fpe(step, comp->name());
  pop_fpe_node();
}

void FPEAuditor::beforeEndRun(INamedInterface* /*comp*/)
{
  add_fpe_node();
}

void FPEAuditor::afterEndRun(INamedInterface* comp)
{
  static const std::string step = "endrun";
  report_fpe(step, comp->name());
  pop_fpe_node();
}

void FPEAuditor::beforeFinalize(INamedInterface* /*comp*/)
{
  add_fpe_node();
}

void FPEAuditor::afterFinalize(INamedInterface* comp)
{
  static const std::string step = "finalize";
  report_fpe(step, comp->name());
  pop_fpe_node();
}

void FPEAuditor::before(CustomEventTypeRef /*evt*/, 
			const std::string& /*caller*/)
{
  add_fpe_node();
}

void FPEAuditor::after(CustomEventTypeRef evt, 
		       const std::string& caller,
		       const StatusCode&)
{
  report_fpe(evt, caller);
  pop_fpe_node();
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/** report fpes which happened during step 'step' on behalf of 'caller'
 */
void
FPEAuditor::report_fpe(const std::string& step, 
		       const std::string& caller)
{
  // store current list of FPE flags which were raised before
  int raised = fetestexcept(FE_OVERFLOW | FE_INVALID | FE_DIVBYZERO);
  if (raised) {
    std::stringstream evStr;
    //try to get the event number:
    StoreGateSvc* m_evtStore=nullptr; 
    if (service("StoreGateSvc",m_evtStore).isSuccess()) {
      const xAOD::EventInfo* ei=m_evtStore->tryConstRetrieve<xAOD::EventInfo>(m_evtInfoKey);
      if (ei) {
	evStr << " on event " << ei->eventNumber();
      }
      else {
	std::cout << "Failed to get EventInfo with key ["<<m_evtInfoKey<<"] while reporting FPE" << std::endl;
      }
    }

    if (raised & FE_OVERFLOW) {
      ATH_MSG_WARNING("FPE OVERFLOW in [" << step << "] of [" << caller << "]" << evStr.str());
      ++m_CountFPEs[FPEAUDITOR_OVERFLOW];
      if ( m_NstacktracesOnFPE && FPEAudit::s_array_O[0] != NULL )
	{
	  for (unsigned int j = 0; j < 100; j++)
	    {
	      if (FPEAudit::s_array_O[j]==NULL) break;
	      this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	      FPEAudit::resolve(FPEAudit::s_array_O[j],this->msg());
	      FPEAudit::s_array_O[j]=NULL;
	      this->msg(MSG::INFO) << endreq;
	    }
	}
    }
    if (raised & FE_INVALID) {
      ATH_MSG_WARNING("FPE INVALID in [" << step << "] of [" << caller << "]" << evStr.str());
      ++m_CountFPEs[FPEAUDITOR_INVALID];
    }
    if ( m_NstacktracesOnFPE && FPEAudit::s_array_I[0] != NULL )
      {
	for (unsigned int j = 0; j < 100; j++)
	  {
	    if (FPEAudit::s_array_I[j]==NULL) break;
	    this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	    FPEAudit::resolve(FPEAudit::s_array_I[j],this->msg());
	    FPEAudit::s_array_I[j]=NULL;
	    this->msg(MSG::INFO) << endreq;
	  }
      }
    if (raised & FE_DIVBYZERO) {
      ATH_MSG_WARNING("FPE DIVBYZERO in [" << step << "] of [" << caller << "]" << evStr.str());
      ++m_CountFPEs[FPEAUDITOR_DIVBYZERO];
      if ( m_NstacktracesOnFPE && FPEAudit::s_array_D[0] != NULL )
	{
	  for (unsigned int j = 0; j < 100; j++)
	    {
	      if (FPEAudit::s_array_D[j]==NULL) break;
	      this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	      FPEAudit::resolve(FPEAudit::s_array_D[j],this->msg());
	      FPEAudit::s_array_D[j]=NULL;
	      this->msg(MSG::INFO) << endreq;
	    }
	}
    }
  }
  if ( FPEAudit::n_fp_exceptions == 0 && m_SigHandInstalled )
    UninstallHandler();
}

void
FPEAuditor::add_fpe_node()
{
  // get current list of FPE flags so far
  int raised = fetestexcept(FE_OVERFLOW | FE_INVALID | FE_DIVBYZERO);
  m_fpe_stack.push_back(std::make_pair(raised, 0));

  // clear FPE status word
  feclearexcept(FE_ALL_EXCEPT);
}

void
FPEAuditor::pop_fpe_node()
{
  if (m_fpe_stack.empty()) {
    ATH_MSG_ERROR("inconsistent fpe-stack !");
    throw std::runtime_error("inconsistent fpe-stack");
  }

  // restore fpe stack info
  int raised = m_fpe_stack.back().first;
  m_fpe_stack.pop_back();
  
  // consolidate
  if (!m_fpe_stack.empty()) {
    m_fpe_stack.back().second |= raised;
  }
  if ( m_SigHandInstalled )
    FPEAudit::unmask_fpe();
}
