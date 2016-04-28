/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsSvc.h
 * header file for service that keeps track of errors in the bytestream.
 * @author nbarlow@cern.ch
**/

#ifndef SCT_ByteStreamErrorsSvc_h
#define SCT_ByteStreamErrorsSvc_h
///STL includes

#include <string>
#include <set>
#include <list>

///Gaudi includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
///Athena includes
#include "AthenaBaseComps/AthService.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
/** needs to be included here for gcc 4.3 compatibility */
#include "StoreGate/StoreGateSvc.h"

/** forward declarations */
template <class TYPE> class SvcFactory;
class ISvcLocator;
class SCT_ID;
class Identifier;
class IdentifierHash;
class StatusCode;
class ISCT_CablingSvc;
class ISCT_ConfigurationConditionsSvc;

/**
 * @class SCT_ByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
**/

class SCT_ByteStreamErrorsSvc: virtual public ISCT_ByteStreamErrorsSvc, virtual public IIncidentListener, virtual public AthService{
  friend class SvcFactory<SCT_ByteStreamErrorsSvc>;
public:
  //@name Service methods
  //@{
  SCT_ByteStreamErrorsSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_ByteStreamErrorsSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  virtual void handle(const Incident& inc);
  //@}
  
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isGood(const IdentifierHash & elementIdHash);
  
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();
  
  virtual StatusCode fillData(int&, std::list<std::string>&) {
    return StatusCode::FAILURE;
  }
  
  virtual bool filled() const;
  
  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize(){ return false; }


  std::set<IdentifierHash>* getErrorSet(int errorType);

  void setRODSimulatedData();

  bool isRODSimulatedData();

  void addError(IdentifierHash& id, int errorType);
  void addErrorCount(int errorType);

  virtual void resetSets();
  virtual void resetCounts(); /** for the counts used by HLT */
  virtual int getNumberOfErrors(int errorType); /** for HLT */

  virtual bool HVisOn();
  virtual void addRODHVCounter(bool HVisOn);
  
  virtual bool isCondensedReadout();
  virtual void setCondensedReadout(bool isCondensed);

  virtual void disableRODs();

  virtual void setDecodedROD(const boost::uint32_t rodId);
  virtual std::vector<boost::uint32_t> getRODOuts() const;

private:

  const SCT_ID* m_sct_id;

  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_config;

  bool m_filled;
  bool m_lookForSGErrContainer;
  std::set<IdentifierHash>* m_timeOutErrors;
  std::set<IdentifierHash>* m_bcIdErrors;
  std::set<IdentifierHash>* m_lvl1IdErrors;
  std::set<IdentifierHash>* m_preambleErrors;
  std::set<IdentifierHash>* m_formatterErrors;
  std::set<IdentifierHash>* m_trailerErrors;
  std::set<IdentifierHash>* m_trailerOverflowErrors;
  std::set<IdentifierHash>* m_headerTrailerLimitErrors;
  std::set<IdentifierHash>* m_ABCDErrors;
  std::set<IdentifierHash>* m_rawErrors;
  std::set<IdentifierHash>* m_byteStreamParseErrors;
  std::set<IdentifierHash>* m_maskedLinks;
  std::set<IdentifierHash>* m_rodClockErrors;
  std::set<IdentifierHash>* m_truncatedRod;
  std::set<IdentifierHash>* m_robFragErrors;
  std::set<IdentifierHash>* m_missingLinkHeaderErrors;

  std::set<IdentifierHash>* m_rxRedundancy;

  int m_numTimeOutErrors;
  int m_numBCIDErrors;
  int m_numLVL1IDErrors;
  int m_numPreambleErrors;
  int m_numFormatterErrors;
  int m_numTrailerErrors;
  int m_numABCDErrors;
  int m_numRawErrors;
  int m_numDecodingErrors;
  int m_numMaskedLinks;
  int m_numRodClockErrors;
  int m_numTruncatedRod;
  int m_numRobFragErrors;
  int m_numMissingLinkHeaderErrors;

  bool m_isRODSimulatedData;

  std::string m_bsErrContainerName;

  bool m_useDCSfromBS;
  int m_numRODsHVon;
  int m_numRODsTotal;
  bool m_condensedMode;
  bool m_useRXredundancy;

  bool m_disableRODs;
  double m_rodFailureFraction;

  std::map<boost::uint32_t, bool> m_rodDecodeStatuses;
};

#endif
