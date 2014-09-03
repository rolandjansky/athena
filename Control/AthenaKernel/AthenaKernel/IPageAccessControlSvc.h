/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IPAGEACCESSCONTROLSVC_H
#define ATHENAKERNEL_IPAGEACCESSCONTROLSVC_H

/**
 * @file   IPageAccessControlSvc.h
 * @brief  Interface to a service that monitors memory page accesses
 * @author Paolo Calafiura
 *
 * $Id: IPageAccessControlSvc.h,v 1.1 2009-03-04 23:38:52 calaf Exp $
 */

#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif

/**
 * @class  IPageAccessControlSvc
 * @brief  Interface to a service that monitors memory page accesses
 * @author Paolo Calafiura
 *
 * This interface allows to start and stop the monitoring and to print a report
 */

class PageAccessControl;

class IPageAccessControlSvc: virtual public IInterface {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /// Virtualize D'tor
  virtual ~IPageAccessControlSvc() {}

  /// In baseline implementation, protect pages and install a SEGV handler
  /// that counts the number of accesses to a protected address.
  virtual bool startMonitoring() = 0;
  virtual bool stopMonitoring() = 0;
  /// has this pointer been accessed (read/written)
  virtual bool accessed(const void* address) const =0;
  /// In baseline implementation, controlled via PageAccessControlSvc.OutputLevel
  virtual void report() const = 0;

  ///control access to the page containing address
  virtual bool controlPage(const void* address) = 0;

  ///FIXME Access to the underlying @class PageAccessControl, used to 
  ///protect/restore page access
  //hopefully not necessary  virtual PageAccessControl* pac() = 0;
};


inline const InterfaceID& IPageAccessControlSvc::interfaceID()
{
  static const InterfaceID IID_IPageAccessControlSvc("IPageAccessControlSvc", 1, 0);
  return IID_IPageAccessControlSvc;
}

#endif // ATHENAKERNEL_IPAGEACCESSCONTROLSVC_H
