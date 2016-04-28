/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ByteStreamErrorsSvc.h
 * interface file for service that keeps track of errors in the bytestream.
 * @author nbarlow@cern.ch
**/

#ifndef ISCT_ByteStreamErrorsSvc_h
#define ISCT_ByteStreamErrorsSvc_h

#include <set>
#include <list>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

/** put this enum here for now - would be better to 
 * have it in SCT_ConditionsData along with other 
 * error types..
 */

namespace SCT_ByteStreamErrors {
  enum errorTypes {
    ByteStreamParseError,
    TimeOutError,
    BCIDError,
    LVL1IDError,
    PreambleError,
    FormatterError,
    TrailerError,
    TrailerOverflowError,
    HeaderTrailerLimitError,
    ABCDError,
    RawError,
    MaskedLink,
    RODClockError,
    TruncatedROD,
    ROBFragmentError,
    MissingLinkHeaderError,
    NUM_ERROR_TYPES  // always have this one last, so we can use it as a loop index
  };
}


/**
 * @class SCT_ByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
**/

class ISCT_ByteStreamErrorsSvc: virtual public ISCT_ConditionsSvc {

public:
  //@name Service methods
  //@{

  virtual ~ISCT_ByteStreamErrorsSvc(){}

  //  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  //@}
  
  virtual std::set<IdentifierHash>* getErrorSet(int errorType)=0;

  virtual void setRODSimulatedData()=0;

  virtual bool isRODSimulatedData()=0;

  virtual void addError(IdentifierHash& id, int errorType)=0;
  virtual void addErrorCount(int errorType)=0;

  virtual void resetSets()=0;
  virtual void resetCounts()=0; /** for the counts used by HLT */
  virtual int getNumberOfErrors(int errorType)=0; /** for HLT */

  virtual void addRODHVCounter(bool HVisOn) = 0;

  virtual bool isCondensedReadout() = 0;

  virtual void setCondensedReadout(bool isCondensed) = 0;

  virtual bool HVisOn() = 0;

  virtual void setDecodedROD(const boost::uint32_t rodId) = 0;
  virtual std::vector<boost::uint32_t> getRODOuts() const = 0;

private:

};

inline const InterfaceID & ISCT_ByteStreamErrorsSvc::interfaceID(){
  static const InterfaceID IID_SCT_ByteStreamErrorsSvc("SCT_ByteStreamErrorsSvc",1,0);
  return IID_SCT_ByteStreamErrorsSvc;
}

#endif
