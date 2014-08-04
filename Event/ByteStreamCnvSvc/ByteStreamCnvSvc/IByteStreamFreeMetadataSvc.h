/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_IBYTESTREAMFREEMETADATATOOL_H
#define BYTESTREAMCNVSVC_IBYTESTREAMFREEMETADATATOOL_H

/**
   @class     IByteStreamFreeMetadataSvc
   @brief     Interface read a single event from ByteStream file given
              a file name and event position in that file

   @author:      Sergey Panitkin
*/

// FrameWork includes
#include "GaudiKernel/IService.h"

#include <string>



//Gaudi style interface to Athena

class IByteStreamFreeMetadataSvc : virtual public ::IService
{

 public:


  virtual ~IByteStreamFreeMetadataSvc(){};
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  virtual StatusCode readInit      (const std::vector<std::string>& keys)=0;
  virtual StatusCode toBSMetadata  (const std::vector<std::string>& keys)=0;
  virtual StatusCode fromBSMetadata(const std::vector<std::string>& keys)=0;

 protected:
  // /standard constructor
  IByteStreamFreeMetadataSvc(){};
  
};

/// Declaration of the interface ID ( interface id, major version, minor version)
inline const InterfaceID& IByteStreamFreeMetadataSvc::interfaceID()
{
  static const InterfaceID IID_IByteStreamFreeMetadataSvc("IByteStreamFreeMetadataSvc",1,0);
  return IID_IByteStreamFreeMetadataSvc;
}

#endif
