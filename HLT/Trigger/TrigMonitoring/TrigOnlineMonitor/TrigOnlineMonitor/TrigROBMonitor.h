/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/HistoProperty.h"
#include "eformat/Status.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class StoreGateSvc;
class IROBDataProviderSvc;
class ITrigROBDataProviderSvc;
class TH1F;    /// for monitoring purposes
class TH2F;    /// for monitoring purposes
class TProfile;/// for monitoring purposes

class TrigROBMonitor:public Algorithm {
public:
  TrigROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();  
  StatusCode endRun();

private:
  /** @brief Pointer to MsgStream.*/
  MsgStream* m_msg;

  /**
  * @brief Accessor method for the MsgStream.
  * @return handle to the MsgStream.
  */
  inline MsgStream& logStream() const { return *m_msg; }
	
  /**
   * @brief Accessor method for the message level variable.
   * @return value of the message level for this algorithm.
   */
  inline MSG::Level logLevel() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Reference to StoreGateSvc;
  StoreGateSvc_t                   m_storeGateSvc;
  
  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  /// Reference to the ROBDataProviderSvc service
  IIROBDataProviderSvc_t           m_robDataProviderSvc;
  /// Reference to a ROBDataProviderSvc which implements also the trigger additions
  SmartIF<ITrigROBDataProviderSvc> m_trigROBDataProviderSvc;

  /// Switch for setting the debug StreamTag and name for debug stream
  BooleanProperty                  m_setDebugStream;
  StringProperty                   m_debugStreamName;

  /// Switch for ROB checksum test
  BooleanProperty                  m_doROBChecksum;
  TH1F*                            m_hist_failedChecksumForROB;
  Histo1DProperty                  m_histProp_failedChecksumForROB;
  TH1F*                            m_hist_failedChecksumForSD;
  Histo1DProperty                  m_histProp_failedChecksumForSD;

  /// Switch for ROB status bit histograms
  BooleanProperty                  m_doROBStatus;
  TH2F*                            m_hist_genericStatusForROB;
  TH2F*                            m_hist_specificStatusForROB;
  std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
  std::vector<std::string>                      m_vec_SpecificStatus;

  /// Switch for ROB data volume histograms
  BooleanProperty                  m_doROBDataVolume;
  TH1F*                            m_hist_totalDataVolumeROB;
  Histo1DProperty                  m_histProp_totalDataVolumeROB;
  TProfile*                        m_hist_dataVolumeFractionForSD;

  /// Helper for checksum test
  /// returns true if a ROB checksum failed
  bool verifyROBChecksum(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for status bits test
  void verifyROBStatusBits(MsgStream& log, OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);
};
