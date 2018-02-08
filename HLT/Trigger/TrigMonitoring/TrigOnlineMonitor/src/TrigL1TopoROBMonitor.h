/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H
#define TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/HistoProperty.h"
#include <vector>
#include <map>

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

namespace TrigConf {
  class IL1TopoConfigSvc;
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
  static const unsigned int m_nTopoCTPOutputs = 128; //! Number of CTP outputs, used for histogram ranges and loops
  StatusCode doCnvMon(bool);
  StatusCode doRawMon(bool);
  StatusCode doSimMon(bool);
  StatusCode doSimDaq(bool);
  StatusCode doOverflowSimMon();
  StatusCode doWriteValData();
  StatusCode monitorROBs(const std::vector<uint32_t>&, bool);
  StatusCode monitorBlock(uint32_t sourceID, L1Topo::Header& header, std::vector<uint32_t>& vFibreSizes, std::vector<uint32_t>& vFibreStatus, std::vector<L1Topo::L1TopoTOB>& daqTobs);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , const Histo1DProperty& prop, std::string extraName, std::string extraTitle);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , std::string hName, std::string hTitle, int bins, float lowEdge, float highEdge);
  void compBitSets(std::string leftLabel, std::string rightLabel,
                   const std::bitset<m_nTopoCTPOutputs>& left, 
                   const std::bitset<m_nTopoCTPOutputs>& right, 
                   TH1F*& hist);
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;
  BooleanProperty m_doRawMon;
  BooleanProperty m_doCnvMon;
  BooleanProperty m_doSimMon;
  BooleanProperty m_doSimDaq;
  BooleanProperty m_doWriteValData;
  UnsignedIntegerArrayProperty m_vDAQROBIDs;
  UnsignedIntegerArrayProperty m_vROIROBIDs;
  UnsignedIntegerProperty m_prescaleForDAQROBAccess;
  StringProperty m_simTopoCTPLocation;
  StringProperty m_simTopoOverflowCTPLocation;
  StringProperty m_HltResultName;
  HLT::IScaler* m_scaler; //! prescale decision tool
  BooleanProperty m_useDetMask;
  std::map<unsigned int,unsigned int> m_allSIDLabelsToInts;
  //Histo1DProperty m_histPropNoBins; // obsolete?
  TH1F* m_histSIDsViaConverters;
  TH1F* m_histSIDsDirectFromROBs;
  TH1F* m_histCTPSignalPartFromROIConv;
  TH1F* m_histOverflowBitsFromROIConv;
  TH1F* m_histTriggerBitsFromROIConv;
  TH1F* m_histPayloadCRCFromDAQConv;
  TH1F* m_histFibreStatusFlagsFromDAQConv;
  TH1F* m_histTOBCountsFromROIROB;
  TH1F* m_histTOBCountsFromDAQROB;
  TH1F* m_histPayloadSizeDAQROB;
  TH1F* m_histPayloadSizeROIROB;
  TH1F* m_histBCNsFromDAQConv;
  TH1F* m_histTopoSimHdwStatComparison;
  TH1F* m_histTopoSimHdwEventComparison;
  TH1F* m_histTopoSimHdwEventOverflowComparison;
  TH1F* m_histTopoCtpSimHdwEventComparison;
  TH1F* m_histTopoCtpHdwEventComparison;
  TH1F* m_histTopoDaqRobEventComparison;
  TH1F* m_histDaqRobCtpEventComparison;
  TH1F* m_histTopoSimResult;
  TH1F* m_histTopoHdwResult;
  TH1F* m_histTopoSimNotHdwResult;
  TH1F* m_histTopoHdwNotSimResult;
  TH1F* m_histTopoSimOverflow;
  TH1F* m_histTopoHdwOverflow;
  TH1F* m_histTopoSimNotHdwOverflow;
  TH1F* m_histTopoHdwNotSimOverflow;
  TH1F* m_histTopoProblems;
  TH1F* m_histInputLinkCRCfromROIConv;
  TH1F* m_histTopoSimOverfl;
  TH1F* m_histTopoHdwOverfl;
  TH1F* m_histTopoDaqRobSimResult;
  TH1F* m_histTopoDaqRobHdwResult;
  TH1F* m_histTopoDaqRobSimNotHdwResult;
  TH1F* m_histTopoDaqRobHdwNotSimResult;
  std::bitset<m_nTopoCTPOutputs> m_triggerBits; //! trigger bits sent to CTP
  std::bitset<m_nTopoCTPOutputs> m_overflowBits; //! overflow bits corresponding to CTP output
  std::bitset<m_nTopoCTPOutputs> m_topoSimResult; //! simulation of CTP output
  std::bitset<m_nTopoCTPOutputs> m_topoSimOverfl; //! simulation of overflow bits
  std::bitset<m_nTopoCTPOutputs> m_topoCtpResult; //! actual hardware CTP output
  bool m_setTopoSimResult; //! keep track of whether this event's sim results had been stored
  bool m_firstEvent; //! used to toggle the ATH_MSG_INFO only on the first event
  std::bitset<m_nTopoCTPOutputs> m_triggerBitsDaqRob; //! trigger bits for BC0 from the DAQ ROB
  std::bitset<m_nTopoCTPOutputs> m_overflowBitsDaqRob; //! overflow bits for BC0 from the DAQ ROB
  enum class Problems {
    ROI_NO_RDO=0,
    ROI_CNV_ERR,
    ROI_PAYLOAD_EMPTY,
    ROI_BAD_TOB,
    DAQ_NO_RDO,
    DAQ_COLL_EMPTY,
    DAQ_CNV_ERR,
    DAQ_PAYLOAD_EMPTY,
    DAQ_BAD_TOB,
    FIBRE_OVERFLOW,
    FIBRE_CRC
  }; //! used to assign an integer ID and histogram bin to various types of problem that arise and should be reported.
  const std::vector<std::string> m_problems = {
    "ROI no RDO",
    "ROI converter error",
    "ROI payload empty",
    "ROI invalid TOB type",
    "DAQ no RDO",
    "DAQ collection empty",
    "DAQ converter error",
    "DAQ payload empty",
    "DAQ invalid TOB type",
    "Fibre overflow",
    "Fibre CRC"
  }; //! bin labels for problem histograms
};

#endif // TRIGONLINEMONITOR
