// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_PAGEACCESSCONTROLSVC_H
#define ATHENASERVICES_PAGEACCESSCONTROLSVC_H

/**
 * @file   PageAccessControlSvc.h
 * @brief  A service that monitors access to memory pages
 * @author Paolo Calafiura
 *
 * $Id: PageAccessControlSvc.h,v 1.1 2009-03-04 23:44:28 calaf Exp $
 */
#include <signal.h>   /*sigaction*/

// Package includes
#include "AthenaKernel/IPageAccessControlSvc.h"

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "CxxUtils/PageAccessControl.h"
#include "CxxUtils/PtrAccessSEGVHandler.h"
#include "Gaudi/Property.h"   /*BooleanProperty*/

// Forward declarations
template <class TYPE> class SvcFactory;

/**
 * @class  PageAccessControlSvc
 * @brief  Service to monitor access to memory pages
 * @author Paolo Calafiura
 *
 * This service uses @class PageAccessControl to protect pages
 * containing selected addresses and to restore their original protection.
 * When startMonitoring/initialize is called a @class PtrSEGVHandler is 
 * installed that records the access to the address in a protected page
 * and that restores the page protection to its original
 * value(known from @class procmaps).
 * On stopMonitoring/finalize the handler is uninstalled. On finalize a report
 * of the accessed and unaccessed pages and addresses is produced.
 * processing. Additional information can be added via setPageAccessControlInfo().
 *
 *
 * Properties:
 *   AutoMonitor: when true start monitoring on initialize, stop on finalize;
 */

class PageAccessControlSvc : virtual public IPageAccessControlSvc,
  public AthService {
public: 

  /// Standard GAUDI constructor
  PageAccessControlSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Destructor
  virtual ~PageAccessControlSvc() {} 
  
  /// \name IPageAccessControlSvc implementation
  //@{  
  /// In baseline implementation, protect pages and install a SEGV handler
  /// that counts the number of accesses to a protected address.
  virtual bool startMonitoring();
  virtual bool stopMonitoring();
  /// has this pointer been accessed (read/written)
  virtual bool accessed(const void* address) const;
  /// In baseline implementation, controlled via PageAccessControlSvc.OutputLevel  
  virtual void report() const;
  ///Using the underlying @class PageAccessControl, forbid all access 
  ///to page containing address
  virtual bool controlPage(const void* address) {
    return m_accessControl.forbidPage(address);
  }
  //@}


  /// \name Gaudi implementation
  //@{
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  //@}

    
private:
  struct sigaction m_saveSEGVaction; ///< the default action for SIGSEGV
  PageAccessControl m_accessControl;
  PtrAccessSEGVHandler m_SEGVHandler;
  BooleanProperty m_autoMonitor; ///< start on init, stop on finalize
}; 


#endif
