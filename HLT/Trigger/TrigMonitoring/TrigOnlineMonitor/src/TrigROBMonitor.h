/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "GaudiKernel/HistoProperty.h"
#include "eformat/Status.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class TH1F;    /// for monitoring purposes
class TH2F;    /// for monitoring purposes
class TProfile;/// for monitoring purposes

class TrigROBMonitor:public AthAlgorithm {
public:
  TrigROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode start();

private:

  
  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  /// Reference to the ROBDataProviderSvc service
  IIROBDataProviderSvc_t           m_robDataProviderSvc;

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
  bool verifyROBChecksum(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);

  /// Helper for status bits test
  void verifyROBStatusBits(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag);
};
