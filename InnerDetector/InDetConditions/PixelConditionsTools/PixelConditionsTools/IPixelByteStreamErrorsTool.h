/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSTOOLS_IPIXELBYTESTREAMERRORSSVC_H
#define PIXELCONDITIONSTOOLS_IPIXELBYTESTREAMERRORSSVC_H
//STL includes
#include <string>
#include <set>

//Gaudi includes
#include "GaudiKernel/IInterface.h"
class StatusCode;
#include "GaudiKernel/ServiceHandle.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"

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

class IPixelByteStreamErrorsTool: virtual public IInterface, virtual public IAlgTool {

  public:

    enum ErrorType {TimeOut=0, firstErrType=TimeOut, BCID=1, LVL1ID=2, Preamble=3, Trailer=4,
      Flagged=5, DisableFE=6, ROD=7, Decoding=8, Invalid=9, LinkMaskedByPPC=10, Limit=11, lastErrType=Limit };

    virtual ~IPixelByteStreamErrorsTool() = default;
    DeclareInterfaceID(IPixelByteStreamErrorsTool, 1, 0);

    virtual bool isGood(const IdentifierHash & elementIdHash) const =0;  

    virtual unsigned int getModuleErrors(IdentifierHash hashID) const =0;

    virtual void setModuleErrors(IdentifierHash hashID, unsigned int errorcode) const =0;
    virtual void setModuleRODErrors(IdentifierHash hashID, unsigned int errorcode) const =0;

    virtual std::map<IdentifierHash, std::map<unsigned int, unsigned int> > & getAllFeErrors() const =0;
    virtual unsigned int getFeErrorCode(IdentifierHash module, unsigned int fe_number) const =0;
    virtual void setFeErrorCode(IdentifierHash module, unsigned int fe_number, unsigned int errorcode) const =0;

    virtual void setModuleFragmentSize(IdentifierHash hashID, unsigned int length) const =0;
    virtual unsigned int getModuleFragmentSize(IdentifierHash hashID) const =0;

    virtual void resetCounts() const =0; // for the counts used by HLT 
    virtual int getNumberOfErrors(int errorType) const =0; // for HLT 

    virtual void addInvalidIdentifier() const =0;
    virtual void addPreambleError() const =0;
    virtual void addTimeOutError() const =0;
    virtual void addLVL1IDError() const =0;
    virtual void addBCIDError() const =0;
    virtual void addFlaggedError() const =0;
    virtual void addTrailerError() const =0;
    virtual void addDisabledFEError() const =0;
    virtual void addDecodingError() const =0;
    virtual void addRODError() const =0;
    virtual void addRead(IdentifierHash hashID) const =0;
    virtual unsigned int getReadEvents(IdentifierHash hashID) const =0;
    virtual bool isActive(IdentifierHash hashID) const =0;
    virtual void addBadFE(IdentifierHash hashID, int badFE) const =0;
    virtual unsigned int getBadFE(IdentifierHash hashID) const =0;
    virtual StatusCode readData()=0;
    virtual StatusCode recordData()=0;

    // FE-I4B
    virtual void addLinkMaskedByPPC() const =0;
    virtual void addLimitError() const =0;
    virtual void updateServiceRecords(int code, unsigned int count) const =0;
    virtual unsigned int getServiceRecordCount(int code) const =0;

};

#endif
