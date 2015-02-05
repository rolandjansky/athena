/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_IPIXELBYTESTREAMERRORSSVC_H
#define PIXELCONDITIONSSERVICES_IPIXELBYTESTREAMERRORSSVC_H
//STL includes
#include <string>
#include <set>

//Gaudi includes
#include "GaudiKernel/IInterface.h"
class StatusCode;
#include "GaudiKernel/ServiceHandle.h"

//Athena includes
//#include "Identifier/Identifier.h"
//#include "Identifier/IdentifierHash.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "PixelConditionsServices/IPixelConditionsSvc.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class PixelID;
class StoreGateSvc;
class Identifier;
class IdentifierHash;
namespace InDetDD{
  class PixelDetectorManager; 
}

  const InterfaceID IID_IPixelByteStreamErrorsSvc("IPixelByteStreamErrorsSvc",1,0);


/**
 * @class PixelByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
**/
class IPixelByteStreamErrorsSvc: virtual public IInterface {

public:

  enum ErrorType {TimeOut=0, firstErrType=TimeOut, BCID=1, LVL1ID=2, Preamble=3, Trailer=4,
                  Flagged=5, DisableFE=6, ROD=7, Decoding=8, Invalid=9, LinkMaskedByPPC=10, Limit=11, lastErrType=Limit };

  virtual ~IPixelByteStreamErrorsSvc(){}
  static const InterfaceID & interfaceID();
  

  virtual bool isGood(const IdentifierHash & elementIdHash)=0;  

  virtual unsigned int getModuleErrors(IdentifierHash hashID)=0;

  virtual void setModuleErrors(IdentifierHash hashID, unsigned int errorcode)=0;
  virtual void setModuleRODErrors(IdentifierHash hashID, unsigned int errorcode)=0;
  
  virtual void resetCounts()=0; // for the counts used by HLT 
  virtual int getNumberOfErrors(int errorType)=0; // for HLT 

  virtual void reset()=0;
  
  virtual void addInvalidIdentifier()=0;
  virtual void addPreambleError()=0;
  virtual void addTimeOutError()=0;
  virtual void addLVL1IDError()=0;
  virtual void addBCIDError()=0;
  virtual void addFlaggedError()=0;
  virtual void addTrailerError()=0;
  virtual void addDisabledFEError()=0;
  virtual void addDecodingError()=0;
  virtual void addRODError()=0;
  virtual void addRead(IdentifierHash hashID)=0;
  virtual unsigned int getReadEvents(IdentifierHash hashID)=0;
  virtual bool isActive(IdentifierHash hashID)=0;
  virtual void addBadFE(IdentifierHash hashID, int badFE)=0;
  virtual unsigned int getBadFE(IdentifierHash hashID)=0;
  virtual StatusCode readData()=0;
  virtual StatusCode recordData()=0;

  // FE-I4B
  virtual void addLinkMaskedByPPC() = 0;
  virtual void addLimitError() = 0;
  virtual void updateServiceRecords(int code, unsigned int count) = 0;
  virtual unsigned int getServiceRecordCount(int code) = 0;


};

inline const InterfaceID & IPixelByteStreamErrorsSvc::interfaceID(){
  return IID_IPixelByteStreamErrorsSvc;
}

#endif
