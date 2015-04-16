/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelByteStreamErrorsSvc_h
#define PixelByteStreamErrorsSvc_h
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

//STL includes
#include <string>
#include <set>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

//local includes
#include "PixelConditionsServices/IPixelConditionsSvc.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IIncidentListener;
class PixelID;
class StoreGateSvc;
class IBLParameterSvc;


/**
 * @class PixelByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
**/
class PixelByteStreamErrorsSvc: virtual public IPixelByteStreamErrorsSvc, virtual public IIncidentListener, public AthService{
  friend class SvcFactory<PixelByteStreamErrorsSvc>;
public:

  //@name Service methods
  //@{
  PixelByteStreamErrorsSvc( const std::string & name, ISvcLocator* svc);
  virtual ~PixelByteStreamErrorsSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  virtual void handle(const Incident& inc);
  //@}
  
 
  virtual bool isGood(const IdentifierHash & elementIdHash);  

  virtual unsigned int getModuleErrors(IdentifierHash hashID) {return m_module_errors[(int) hashID];}

  virtual void setModuleErrors(IdentifierHash hashID, unsigned int errorcode) {
    if( static_cast<unsigned int>(hashID) < m_max_hashes ){
      m_module_errors[static_cast<unsigned int>(hashID)] = errorcode;
    }
  };

  virtual void setModuleRODErrors(IdentifierHash hashID, unsigned int errorcode) { // set the ROD errors for each module on that ROD
    if( static_cast<unsigned int>(hashID) < m_max_hashes ){
      m_moduleROD_errors[static_cast<unsigned int>(hashID)] = errorcode;
    }
  };
  
  virtual void resetCounts(); // for the counts used by HLT 
  virtual int getNumberOfErrors(int errorType); // for HLT 
  virtual void resetPixelCounts(); // for the counts used for BS errors report
  virtual void reset(); // resetting of error words, called on a BeginEvent incident

  virtual void addInvalidIdentifier(){m_numInvalidIdentifiers++;}; // wrong FE/col/row number
  virtual void addPreambleError(){m_numPreambleErrors++;};
  virtual void addTimeOutError(){m_numTimeOutErrors++;};
  virtual void addLVL1IDError(){m_numLVL1IDErrors++;};
  virtual void addBCIDError(){m_numBCIDErrors++;};
  virtual void addFlaggedError(){m_numFlaggedErrors++;};
  virtual void addTrailerError(){m_numTrailerErrors++;};
  virtual void addDisabledFEError(){m_numDisabledFEErrors++;};
  virtual void addDecodingError(){m_numDecodingErrors++;};
  virtual void addRODError(){m_numRODErrors++;};
  virtual void addRead(IdentifierHash hashID) {
    if (static_cast<unsigned int>(hashID) < m_max_hashes) {
      m_event_read[static_cast<unsigned int>(hashID)]++;
      m_module_isread[static_cast<unsigned int>(hashID)]=true;
    }
  }
  // FE-I4B trailer errors
  virtual void addLinkMaskedByPPC() {++m_numLinkMaskedByPPC;}
  virtual void addLimitError() {++m_numLimitError;}

  virtual unsigned int getReadEvents(IdentifierHash hashID){return m_event_read[(int)hashID];}
  virtual bool isActive(IdentifierHash hashID){return m_module_isread[(int)hashID];}
  virtual void addBadFE(IdentifierHash hashID, int badFE){
    if( static_cast<unsigned int>(hashID) < m_max_hashes ){
      m_FE_errors[static_cast<unsigned int>(hashID)] |= (1<<badFE);
    }
  };
  virtual unsigned int getBadFE(IdentifierHash hashID) {return m_FE_errors[(int)hashID];}
  virtual StatusCode readData();
  virtual StatusCode recordData();

  // FE-I4B service records
  virtual void updateServiceRecords(int code, unsigned int count) {m_ServiceRecords[code] = count;}
  virtual unsigned int getServiceRecordCount(int code) {return m_ServiceRecords[code];}

private:
  const PixelID* m_pixel_id;

  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;

  unsigned int* m_module_errors;
  unsigned int* m_moduleROD_errors;
  unsigned int* m_event_read;
  unsigned int* m_FE_errors;
  bool* m_module_isread;

  bool m_ibl_is_present;
  bool m_dbm_is_present;

  //number of errors in the event
  int m_numInvalidIdentifiers;
  int m_numPreambleErrors;
  int m_numTimeOutErrors;
  int m_numLVL1IDErrors;
  int m_numBCIDErrors;
  int m_numFlaggedErrors;
  int m_numTrailerErrors;
  int m_numDisabledFEErrors;
  int m_numDecodingErrors;
  int m_numRODErrors;
  int m_numLinkMaskedByPPC;
  int m_numLimitError;

  // FE-I4B service record codes
  // Array of counters for each code, i.e. m_ServiceRecords[0] is the
  // count for code 0 (BCID counter error)
  unsigned int m_ServiceRecords[32];


  //  int m_numRawErrors;
  
  unsigned int m_max_hashes;
  bool m_readESD;
};

#endif
