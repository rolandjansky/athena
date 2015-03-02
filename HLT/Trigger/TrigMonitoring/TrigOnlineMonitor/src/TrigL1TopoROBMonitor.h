/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H
#define TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/HistoProperty.h"
#include <vector>

class ITHistSvc;
class IROBDataProviderSvc;
class ITrigROBDataProviderSvc;
class TH1F;    /// for monitoring purposes
class TH2F;    /// for monitoring purposes
class TProfile;/// for monitoring purposes

namespace L1Topo {
  class Header;
  class L1TopoTOB;
}

namespace HLT {
  class IScaler;
}

class TrigL1TopoROBMonitor:public AthAlgorithm {
 public:
  TrigL1TopoROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();  
  StatusCode endRun();
 private:
  StatusCode doCnvMon(bool);
  StatusCode doRawMon(bool);
  StatusCode monitorROBs(const std::vector<uint32_t>&, bool);
  StatusCode monitorBlock(uint32_t sourceID, L1Topo::Header& header, std::vector<uint32_t>& vFibreSizes, std::vector<uint32_t>& vFibreStatus, std::vector<L1Topo::L1TopoTOB>& daqTobs);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , const Histo1DProperty& prop, std::string extraName, std::string extraTitle);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , std::string hName, std::string hTitle, int bins, float lowEdge, float highEdge);
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  BooleanProperty m_doRawMon;
  BooleanProperty m_doCnvMon;
  UnsignedIntegerArrayProperty m_vDAQROBIDs;
  UnsignedIntegerArrayProperty m_vROIROBIDs;
  UnsignedIntegerProperty m_prescaleForDAQROBAccess;
  HLT::IScaler* m_scaler; //! prescale decision tool
  Histo1DProperty m_histPropNoBins;
  TH1F* m_histSIDsViaConverters;//
  TH1F* m_histSIDsDirectFromROBs;//
  TH1F* m_histCTPSignalPartFromROIConv;//
  TH1F* m_histOverflowBitsFromROIConv;
  TH1F* m_histTriggerBitsFromROIConv;
  TH1F* m_histPayloadCRCFromDAQConv;//
  TH1F* m_histFibreStatusFlagsFromDAQConv;
  //TH2F* m_histFibreSizesFromDAQConv;
  TH1F* m_histTOBCountsFromROIROB;//
  TH1F* m_histTOBCountsFromDAQROB;//
};

#endif // TRIGONLINEMONITOR
