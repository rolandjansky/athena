/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_BYTESTREAM_CONDITIONSSVC_H
#define TRT_BYTESTREAM_CONDITIONSSVC_H

/** @file TRT_ByteStream_ConditionsSvc.h
 *  @brief Service for accessing TRT ByteStream conditions information
 *  @author Denver Whittington
 *  @author Paul Keener
 */

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/// Service for retrieving ByteStream conditions information
class TRT_ByteStream_ConditionsSvc : public AthService,
  virtual public ITRT_ConditionsSvc,
  virtual public ITRT_ByteStream_ConditionsSvc,
  virtual public IIncidentListener
{

 public:

  TRT_ByteStream_ConditionsSvc( const std::string&, ISvcLocator* );
  virtual ~TRT_ByteStream_ConditionsSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  /// @name Functions inherited from ITRT_ConditionsSvc
  //@{

  /// Evaluation for TRT_ConditionsSummarySvc
  /** Function inherited from ITRT_ConditionsSvc.
   *  Simply returns a InDet::TRT_GOOD.
   */
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& );

  //@}

  void handle(const Incident&);

  void resetSets( void );
  void resetCounts( void );

  int getNumberOfErrors( TRTByteStreamErrors::errorTypes ErrType );
  const std::set<std::pair<uint32_t,uint32_t> > & 
    getL1idErrorSet( void );
  const std::set<std::pair<uint32_t,uint32_t> > & 
    getBcidErrorSet( void );
  const std::set<std::pair<uint32_t,uint32_t> > &getRobErrorSet( void );
  const std::set<uint32_t> & getMissingErrorSet( void );
  const std::set<uint32_t> & getErrorErrorSet( void );
  const std::set<uint32_t> & getSidErrorSet( void );

  std::set<std::pair<uint32_t,uint32_t> > * getRodRobErrorSet( TRTByteStreamErrors::errorTypes ErrType );
  std::set<std::pair<uint32_t,uint32_t> > * getIdErrorSet( TRTByteStreamErrors::errorTypes ErrType );
  std::set<uint32_t> * getErrorSet( TRTByteStreamErrors::errorTypes ErrType );

  void add_l1id_error( uint32_t DTMROC_idx );
  void add_bcid_error( uint32_t DTMROC_idx ); 
  void add_missing_error( uint32_t DTMROC_idx );
  void add_error_error( uint32_t DTMROC_idx );
  void add_sid_error( uint32_t DTMROC_idx );

  void add_l1id_error( uint32_t DTMROC_idx, uint32_t l1id );
  void add_bcid_error( uint32_t DTMROC_idx, uint32_t bcid );

  void add_rob_error( uint32_t robSourceId, uint32_t robStatus );

  virtual StatusCode readData( void );
  virtual StatusCode recordData( void );


 private:

  ServiceHandle<StoreGateSvc> m_evtStore;

  std::set<uint32_t> m_l1id_errors;     // Deprecated
  std::set<uint32_t> m_bcid_errors;     // Deprecated
  std::set<uint32_t> m_missing_errors;
  std::set<uint32_t> m_error_errors;
  std::set<uint32_t> m_sid_errors;

  std::set<std::pair<uint32_t,uint32_t> > m_l1id_pair_errors;
  std::set<std::pair<uint32_t,uint32_t> > m_bcid_pair_errors;

  std::set<std::pair<uint32_t,uint32_t> > m_rob_status_errors;

  bool m_writeBCIDError;      // true if we should write BCID errors to SG
  bool m_writeL1IDError;      // true if we should write L1ID errors to SG
  bool m_writeMISSINGError;   // true if we should write MISSING errors to SG
  bool m_writeERRORError;     // true if we should write ERROR errors to SG
  bool m_writeSIDError;       // true if we should write SID errors to SG

  bool m_readCondFromESD;     // true if we are reading ESD and we should
                              //      read the conditions data
  int m_num_l1id_errors;
  int m_num_bcid_errors;
  int m_num_missing_errors;
  int m_num_error_errors;
  int m_num_sid_errors;
  int m_num_robStatus_errors;

  long long m_tot_num_l1id_errors;
  long long m_tot_num_bcid_errors;
  long long m_tot_num_missing_errors;
  long long m_tot_num_error_errors;
  long long m_tot_num_sid_errors;
  long long m_tot_num_robStatus_errors;

  //TRT_BSIdErrContainer* m_IdCont;
  //TRT_BSErrContainer* m_cont;

  SG::ReadHandleKey<TRT_BSIdErrContainer> m_keyerrContid{this,"ByteStreamIdErrsKey","TRT_ByteStreamIdErrs","RHK to retrieve ByteStream Error ID"};
  SG::ReadHandleKey<TRT_BSErrContainer> m_keyerrCont{this,"ByteStreamErrsKey","TRT_ByteStreamErrs","RHK to retrieve ByteStream Errrors"};
  SG::WriteHandleKey<TRT_BSIdErrContainer> m_writekeyErrContID{this,"ByteStreamIdErrsKeyContainer","TRT_ByteStreamIdErrs","WHK to write the ByteStream Error id"};
  SG::WriteHandleKey<TRT_BSErrContainer> m_writekeyErrCont{this,"ByteStreamErrsKeyContainer","TRT_ByteStreamErrs","WHK to write the ByteStream Errors"};

};

/// Query Interface
inline StatusCode TRT_ByteStream_ConditionsSvc::queryInterface( const InterfaceID& riid, void** ppvIf ) {
  if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
  } else if ( ITRT_ByteStream_ConditionsSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_ByteStream_ConditionsSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvIf );
  }
  return StatusCode::SUCCESS;
}

#endif // TRT_BYTESTREAM_CONDITIONSSVC_H
