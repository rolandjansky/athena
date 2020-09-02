/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMINPUTSVC_H
#define BYTESTREAMCNVSVC_BYTESTREAMINPUTSVC_H

/** @file ByteStreamInputSvc.h
 *  @brief This file contains the class definition for the ByteStreamInputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamInputSvc.h,v 1.51 2009-03-03 16:03:22 gemmeren Exp $
 **/

#include <exception>
#include "AthenaBaseComps/AthService.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvc/ByteStreamExceptions.h"

/** @class ByteStreamInputSvc
 *  @brief This class provides the base class to services to read bytestream data.
 *  The concrete class can provide Raw event from a file, transient store, or through network.
 **/
class ByteStreamInputSvc : public ::AthService {
public:
  /// constructor
  ByteStreamInputSvc(const std::string& name, ISvcLocator* svcloc);
  /// destructor
  virtual ~ByteStreamInputSvc(void);

  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /// virtual method for advance to the next event
  virtual const RawEvent* nextEvent() = 0;
  virtual const RawEvent* previousEvent() = 0;
  virtual void setEvent(void* /*data*/, unsigned int /*status*/) {}
  /// virtual method for accessing the current event
  virtual const RawEvent* currentEvent() const = 0;
  /// virtual method for accessing the current event status
  virtual unsigned int currentEventStatus() const;
  virtual std::pair<long,std::string> getBlockIterator(const std::string /* file */);
  virtual void closeBlockIterator(bool);
  virtual bool ready() const;
  virtual StatusCode generateDataHeader(); 
  virtual long positionInBlock();
  virtual void validateEvent();
};

inline const InterfaceID& ByteStreamInputSvc::interfaceID() {
  /// Declaration of the interface ID ( interface id, major version, minor version)
  static const InterfaceID IID_ByteStreamInputSvc("ByteStreamInputSvc", 1, 0);
  return(IID_ByteStreamInputSvc);
}

inline unsigned int ByteStreamInputSvc::currentEventStatus() const {
  return(0);
}

// Virtual methods needed for file input
inline std::pair<long,std::string> ByteStreamInputSvc::getBlockIterator(const std::string /* file */) {return std::make_pair(-1,"GUID");}
inline void ByteStreamInputSvc::closeBlockIterator(bool) {}
inline bool ByteStreamInputSvc::ready() const {return false;}
inline StatusCode ByteStreamInputSvc::generateDataHeader() {return StatusCode::SUCCESS;}
inline long ByteStreamInputSvc::positionInBlock() {return -1;} 
inline void ByteStreamInputSvc::validateEvent() {}
#endif
