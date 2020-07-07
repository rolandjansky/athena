/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMOUTPUTSVC_H
#define BYTESTREAMCNVSVC_BYTESTREAMOUTPUTSVC_H

/** @file ByteStreamOutputSvc.h
 *  @brief This file contains the class definition for the ByteStreamOutputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamOutputSvc.h,v 1.51 2009-03-03 16:03:22 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthService.h"

#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/EventContext.h"

/** @class ByteStreamOutputSvc
 *  @brief This class provides the base class to services to write bytestream data.
 *  The concrete class can provide Raw event to a file, transient store, or through network.
 **/
class ByteStreamOutputSvc : public ::AthService {
public:
  /// constructor
  ByteStreamOutputSvc(const std::string& name, ISvcLocator* svcloc);
  /// destructor
  virtual ~ByteStreamOutputSvc(void);

  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /// virtual method for writing the event
  virtual bool putEvent(RawEvent* re) = 0;

  /// context-aware method for writing the event
  virtual bool putEvent(RawEvent* re, const EventContext& ctx) = 0;
};

inline const InterfaceID& ByteStreamOutputSvc::interfaceID() {
  /// Declaration of the interface ID ( interface id, major version, minor version)
  static const InterfaceID IID_ByteStreamOutputSvc("ByteStreamOutputSvc", 1, 0);
  return(IID_ByteStreamOutputSvc);
}

#endif
