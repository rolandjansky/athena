///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// FPEAuditor.cxx
// Implementation file for class FPEAuditor
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <mutex>
#include <atomic>

// FrameWork includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "FPEAuditor.h"

#include "CxxUtils/checker_macros.h"

// C includes
#include <fenv.h>


namespace FPEAudit {
  const int MAXARRAY=100;
  struct FPEAuditTLSData {
    void *s_array_O[MAXARRAY];
    void *s_array_I[MAXARRAY];
    void *s_array_D[MAXARRAY];
  };

  thread_local FPEAuditTLSData s_tlsdata;

  std::atomic<bool> s_handlerInstalled = false;
  std::atomic<bool> s_handlerDisabled = false;
  struct sigaction s_oldactHandler ATLAS_THREAD_SAFE;

  std::mutex s_mutex;
  typedef std::lock_guard<std::mutex> lock_t;
}


#if defined(__linux__) && ( defined(__i386__) || defined(__x86_64__) )
# include "FPEAudit_linux.icc"
#else
# include "FPEAudit_dummy.icc"
#endif


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

thread_local FPEAuditor::FpeStack_t FPEAuditor::s_fpe_stack;

// Constructors
////////////////
FPEAuditor::FPEAuditor( const std::string& name,
			ISvcLocator* pSvcLocator ) :
  AthCommonMsg<Auditor>     ( name, pSvcLocator  ),
  m_CountFPEs(),
  m_NstacktracesOnFPE(0),
  m_env(),
  m_nexceptions(0)
{
  declareProperty("NStacktracesOnFPE", m_NstacktracesOnFPE,
                  "Flag to configure, how many stack traces in case of FPE are printed. "
                  "Default: Zero, just report that FPE has happened. "
                  "After collecting the stacktrace, the code has to modify the mcontext_t "
                  "struct to ignore FPEs for the rest of the processing of the algorithm/service "
                  "This part is highly non-portable!" );
}

// Destructor
///////////////
#include <fstream>
#include <iomanip>
FPEAuditor::~FPEAuditor()
{
  //m_msg << MSG::DEBUG << "Calling destructor" << endmsg;
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

  {
    FPEAudit::lock_t lock (FPEAudit::s_mutex);
    if ( FPEAudit::s_handlerInstalled && !FPEAudit::s_handlerDisabled )
      UninstallHandler();
  }

  return StatusCode::SUCCESS;
}

void FPEAuditor::InstallHandler()
{
  struct sigaction act;

  // Save the current FP environment.
  fegetenv (&m_env);

  memset(&act, 0, sizeof act);
  act.sa_sigaction = FPEAudit::fpe_sig_action;
  act.sa_flags = SA_SIGINFO;
  if ( sigaction(SIGFPE, &act, &FPEAudit::s_oldactHandler) != 0 )
    {
      ATH_MSG_WARNING ("Printing stacktraces on FPE requested, but unable to install signal handler ! Switched off !");
      m_NstacktracesOnFPE=0;
    }
  if ( m_NstacktracesOnFPE )
    {
      ATH_MSG_INFO ("Installed Signalhandler !");
      FPEAudit::unmask_fpe();
    }
  FPEAudit::s_tlsdata.s_array_O[0]=NULL;
  FPEAudit::s_tlsdata.s_array_I[0]=NULL;
  FPEAudit::s_tlsdata.s_array_D[0]=NULL;

  FPEAudit::s_handlerInstalled = true;
}

void FPEAuditor::UninstallHandler()
{
  ATH_MSG_INFO("uninstalling SignalHandler");
  FPEAudit::s_handlerDisabled = true;

  feclearexcept(FE_ALL_EXCEPT);
  fesetenv (&m_env);
  FPEAudit::mask_fpe();

  // feenableexcept (0);
  // fedisableexcept (FE_ALL_EXCEPT);
}

void FPEAuditor::before(StandardEventType evt, INamedInterface*)
{
  add_fpe_node();

  if ( evt==IAuditor::Execute ) {
    if ( m_NstacktracesOnFPE && ! FPEAudit::s_handlerInstalled ) {
      FPEAudit::lock_t lock (FPEAudit::s_mutex);
      if ( m_NstacktracesOnFPE && ! FPEAudit::s_handlerInstalled ) {
        InstallHandler();
        m_nexceptions = m_NstacktracesOnFPE;
      }
    }
  }
}

void FPEAuditor::after(StandardEventType evt, INamedInterface* comp, const StatusCode&)
{
  report_fpe(toStr(evt), comp->name());
  pop_fpe_node();

  if ( evt==IAuditor::Initialize ) {
    FPEAudit::lock_t lock (FPEAudit::s_mutex);
    // CoreDumpSvc can also install a FPE handler, grrr.
    if (comp->name() == "CoreDumpSvc") FPEAudit::s_handlerInstalled = false;
    if ( m_NstacktracesOnFPE && ! FPEAudit::s_handlerInstalled ) {
      InstallHandler();
      m_nexceptions = m_NstacktracesOnFPE;
    }
  }
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

/** report fpes which happened during step 'step' on behalf of 'caller'
 */
void
FPEAuditor::report_fpe(const std::string& step,
		       const std::string& caller)
{
  // store current list of FPE flags which were raised before
  int raised = fetestexcept(FE_OVERFLOW | FE_INVALID | FE_DIVBYZERO);
  if (raised) {
    // FIXME: Gaudi should pass context to the auditors.
    std::stringstream evStr;
    const EventContext& ctx = Gaudi::Hive::currentContext();
    if (ctx.valid()) {
      evStr << " on event " << ctx.eventID().event_number();
    }

    if (raised & FE_OVERFLOW) {
      ATH_MSG_WARNING("FPE OVERFLOW in [" << step << "] of [" << caller << "]" << evStr.str() <<
                      " " << m_NstacktracesOnFPE << " " << FPEAudit::s_tlsdata.s_array_O[0]
                      );
      ++m_CountFPEs[FPEAUDITOR_OVERFLOW];
      if ( m_NstacktracesOnFPE && FPEAudit::s_tlsdata.s_array_O[0] != NULL )
	{
	  for (unsigned int j = 0; j < 100; j++)
	    {
              FPEAudit::lock_t lock (FPEAudit::s_mutex);
	      if (FPEAudit::s_tlsdata.s_array_O[j]==NULL) break;
	      this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	      FPEAudit::resolve(FPEAudit::s_tlsdata.s_array_O[j],this->msg());
	      FPEAudit::s_tlsdata.s_array_O[j]=NULL;
	      this->msg(MSG::INFO) << endmsg;
	    }
	}
    }
    if (raised & FE_INVALID) {
      ATH_MSG_WARNING("FPE INVALID in [" << step << "] of [" << caller << "]" << evStr.str()
                      << " " << m_NstacktracesOnFPE << " " << FPEAudit::s_tlsdata.s_array_I[0]
                      );
      ++m_CountFPEs[FPEAUDITOR_INVALID];
    }
    if ( m_NstacktracesOnFPE && FPEAudit::s_tlsdata.s_array_I[0] != NULL )
      {
	for (unsigned int j = 0; j < 100; j++)
	  {
            FPEAudit::lock_t lock (FPEAudit::s_mutex);
            if (FPEAudit::s_tlsdata.s_array_I[j]==NULL) break;
	    this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	    FPEAudit::resolve(FPEAudit::s_tlsdata.s_array_I[j],this->msg());
	    FPEAudit::s_tlsdata.s_array_I[j]=NULL;
	    this->msg(MSG::INFO) << endmsg;
	  }
      }
    if (raised & FE_DIVBYZERO) {
      ATH_MSG_WARNING("FPE DIVBYZERO in [" << step << "] of [" << caller << "]" << evStr.str()
                      << " " << m_NstacktracesOnFPE << " " << FPEAudit::s_tlsdata.s_array_D[0]
                      );
      ++m_CountFPEs[FPEAUDITOR_DIVBYZERO];
      if ( m_NstacktracesOnFPE && FPEAudit::s_tlsdata.s_array_D[0] != NULL )
	{
          FPEAudit::lock_t lock (FPEAudit::s_mutex);
	  for (unsigned int j = 0; j < 100; j++)
	    {
	      if (FPEAudit::s_tlsdata.s_array_D[j]==NULL) break;
	      this->msg(MSG::INFO) << "FPE stacktrace " << j << " :\n";
	      FPEAudit::resolve(FPEAudit::s_tlsdata.s_array_D[j],this->msg());
	      FPEAudit::s_tlsdata.s_array_D[j]=NULL;
	      this->msg(MSG::INFO) << endmsg;
	    }
	}
    }


    FPEAudit::lock_t lock (FPEAudit::s_mutex);
    if ( --m_nexceptions == 0
         && FPEAudit::s_handlerInstalled &&
         !FPEAudit::s_handlerDisabled )
    {
      fprintf(stderr, "too many SIGFPE detected, will be uninstalling signal handler\n");
      UninstallHandler();
    }
  }
}

void
FPEAuditor::add_fpe_node()
{
  // get current list of FPE flags so far
  int raised = fetestexcept(FE_OVERFLOW | FE_INVALID | FE_DIVBYZERO);
  s_fpe_stack.push_back(std::make_pair(raised, 0));

  // clear FPE status word
  feclearexcept(FE_ALL_EXCEPT);

  if (FPEAudit::s_handlerDisabled) {
    // Make sure exceptions have been masked off if the handler has been
    // disabled.  Yes, this is already done in the signal handler.
    // But when TBB starts a new task, it resets the FPU control words
    // to values that were copied when the thread pool was initialized.
    // FIXME: Do it properly with TBB?  Not sure that the current
    // interfaces allow it.
    FPEAudit::mask_fpe();
  }
}

void
FPEAuditor::pop_fpe_node()
{
  if (s_fpe_stack.empty()) {
    ATH_MSG_ERROR("inconsistent fpe-stack !");
    throw std::runtime_error("inconsistent fpe-stack");
  }

  // restore fpe stack info
  int raised = s_fpe_stack.back().first;
  s_fpe_stack.pop_back();

  // consolidate
  if (!s_fpe_stack.empty()) {
    s_fpe_stack.back().second |= raised;
  }
  if ( FPEAudit::s_handlerInstalled && !FPEAudit::s_handlerDisabled) {
    FPEAudit::unmask_fpe();
  }
}
