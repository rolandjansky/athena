/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_IBYTESTREAMSINGLEEVENTINPUTSVC_H
#define BYTESTREAMCNVSVC_IBYTESTREAMSINGLEEVENTINPUTSVC_H

/**
   @class     IByteStreamSingleEventInputSvc
   @brief     Interface read a single event from ByteStream file given
              a file name and event position in that file

   @author:      Sergey Panitkin
*/

// FrameWork includes
//#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"

#include <string>

// Raw event definition here
#include "ByteStreamData/RawEvent.h"


/// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IByteStreamSingleEventInputSvc("IByteStreamSingleEventInputSvc",1,0);

//Gaudi style interface to Athena

class IByteStreamSingleEventInputSvc : virtual public IInterface
{

 public:


  virtual ~IByteStreamSingleEventInputSvc(){};
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

 
  /// get event for a given filename and event position
  virtual const RawEvent* getEvent(const std::string& filename, const long long pos)= 0 ;

 protected:
  // /standard constructor
    IByteStreamSingleEventInputSvc(){};
  
};

inline const InterfaceID& IByteStreamSingleEventInputSvc::interfaceID()
{
  return IID_IByteStreamSingleEventInputSvc;
}

#endif
