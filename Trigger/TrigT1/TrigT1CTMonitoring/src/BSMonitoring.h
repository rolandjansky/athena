/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_CTMONITORING_BSMONITORING_H
#define TRIGT1_CTMONITORING_BSMONITORING_H

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <bitset>
#include <deque>
#include <inttypes.h>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

// RPC and TGC includes for access to SL data
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonDigitContainer/TgcDigit.h"

class CTP_RDO;
class CTP_RIO;
class MuCTPI_RDO;
class MuCTPI_RIO;
class CTP_BC;



class TH2F_LW;
class TH1F_LW;
class TProfile_LW;

namespace cool {
  class IHvsNode;
}

namespace TrigConf {
  class ILVL1ConfigSvc;
}

namespace ROIB {
  class RoIBResult;
}

namespace LVL1 {
  class MuCTPICTP;
  class EmTauCTP;
  class JetCTP;
  class EnergyCTP;
  class MbtsCTP;
  class BcmCTP;
  class TrtCTP;
  class LucidCTP;
  class ZdcCTP;
  class BptxCTP;
  class NimCTP;
  class RecMuonRoiSvc;
}


namespace TrigT1CTMonitoring {

  const std::string ID_RecRpcRoiSvc = "LVL1RPC::RPCRecRoiSvc";
  const std::string ID_RecTgcRoiSvc = "LVL1TGC::TGCRecRoiSvc";

  typedef std::map<std::string, LWHist*> HistMap_t;

  // enum for the LHC beam modes, see https://edms.cern.ch/document/1070479/
  enum BeamMode {
    SETUP = 1,
    ABORT = 2,
    INJECTIONPROBEBEAM = 3,
    INJECTIONSETUPBEAM = 4,
    INJECTIONPHYSICSBEAM = 5,
    PREPARERAMP = 6,
    RAMP = 7,
    FLATTOP = 8,
    SQUEEZE = 9,
    ADJUST = 10,
    STABLEBEAMS = 11,
    UNSTABLEBEAMS = 12,
    BEAMDUMP = 13,
    RAMPDOWN = 14,
    CYCLING = 15,
    RECOVERY = 16,
    INJECTANDDUMP = 17,
    CIRCULATEANDDUMP = 18,
    NOBEAM = 19,
    UNKNOWN = 20 // not official, just used here to catch unknown beam modes
  };

  class BSMonitoring : public ManagedMonitorToolBase {

  public:

    BSMonitoring(const std::string & type, const std::string & name,
		 const IInterface * parent);

  private:
    
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoiSvc; ///RPC rec. RoI service
    ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoiSvc; ///TGC rec. RoI service
      
    StatusCode initialize();
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode initMuctpiHistograms();
    StatusCode initCtpHistograms();
    StatusCode initCtpMuctpiHistograms();
    StatusCode initRoIHistograms();

    StatusCode registerTH1(const std::string& histName, const std::string& histTitle,
			   int nbins, float minX, float maxX);
    StatusCode registerTProfile(const std::string& histName,
				const std::string& histTitle, int nbins,
				float minX, float maxX, float minY,
				float maxY);
    StatusCode registerTH2(const std::string& histName, const std::string& histTitle,
			   int nbinsX, float minX, float maxX,
			   int nbinsY, float minY, float maxY);
    TH1F_LW *getTH1(const char* histLabel);
    TH2F_LW *getTH2(const char* histLabel);
    TProfile_LW *getTProfile(const char* histLabel);

    void getCoolData(unsigned int runNumber);

    void doMuonRoI(const DataHandle < MuCTPI_RDO > theMuCTPI_RDO,
		   const DataHandle < MuCTPI_RIO > theMuCTPI_RIO,
		   const DataHandle < ROIB::RoIBResult > roib);
    void doCtp(const DataHandle < CTP_RDO > theCTP_RDO,
	       const DataHandle < CTP_RIO > theCTP_RIO);
    void doMuctpi(const DataHandle < MuCTPI_RDO > theMuCTPI_RDO,
		  const DataHandle < MuCTPI_RIO > theMuCTPI_RIO,
		  const DataHandle < RpcSectorLogicContainer > theRPCContainer,
		  const DataHandle < Muon::TgcCoinDataContainer > theTGCContainer);
    void doCtpMuctpi(const DataHandle < CTP_RDO > theCTP_RDO,
		     const DataHandle < CTP_RIO > theCTP_RIO,
		     const DataHandle < MuCTPI_RDO > theMuCTPI_RDO,
		     const DataHandle < MuCTPI_RIO > theMuCTPI_RIO);
    void dumpData(const DataHandle < CTP_RDO > theCTP_RDO,
		  const DataHandle < CTP_RIO > theCTP_RIO,
		  const DataHandle < MuCTPI_RDO > theMuCTPI_RDO,
		  const DataHandle < MuCTPI_RIO > theMuCTPI_RIO,
		  const DataHandle < ROIB::RoIBResult > roib);
    StatusCode compareRerun(const CTP_BC &bunchCrossing);

    std::string getEventInfoString();
    double getFrequencyMeasurement(uint64_t eventTimeInNs) const;
    TrigT1CTMonitoring::BeamMode getBeamMode(uint64_t eventTimeInNs) const;
    bool getDataTakingMode(uint32_t lbNumber) const;

    bool m_histogramsBooked;
    bool m_retrievedLumiBlockTimes;
    uint32_t m_currentLumiBlock;
    unsigned int m_runNumber;
    unsigned int m_eventNumber;

    bool m_inclusiveTriggerThresholds;
    bool m_processMuctpi;
    bool m_processMuctpiRIO;
    bool m_processCTP;
    bool m_processRoIB;
    bool m_compareRerun;
    bool m_runOnESD;
    bool m_retrieveCoolData;
    std::string m_baseDirName;
    std::string m_dirName;
    HistMap_t m_histNames;
    uint32_t m_eventCount;
    uint64_t m_firstEventTime;
    int64_t m_firstEventBcid;
    int64_t m_firstEventTC;
    uint32_t m_filledEventCount;
    uint32_t m_maxLumiBlock;
    uint32_t m_lumiBlockOfPreviousEvent;

    // COOL folder names
    std::string m_lbTimeCoolFolderName;
    std::string m_rfPhaseCoolFolderName;
    std::string m_fillStateCoolFolderName;
    std::string m_dataTakingModeCoolFolderName;
    //
    double m_defaultBcIntervalInNs;
    int64_t m_bcsPerTurn;

    // STL containers for info from COOL
    std::vector<uint32_t> m_lumiBlocks;
    std::map<uint32_t,uint64_t> m_lbStartTimes;
    std::map<uint32_t,uint64_t> m_lbEndTimes;
    std::map<uint32_t,double> m_lbStartFreqMeasurements;
    std::map<uint64_t,double> m_freqMeasurements;
    std::map<uint64_t,BeamMode> m_beamMode;
    std::map<uint32_t,bool> m_dataTakingMode;

    unsigned int m_nLB;

    //Vectors of TBP/TAP/TAB
    std::vector<int> m_tbpItems;
    std::vector<int> m_tapItems;
    std::vector<int> m_tavItems;
    std::vector<int> m_tbpBC;
    std::vector<int> m_tapBC;
    std::vector<int> m_tavBC;

    //String cache to reduce malloc/free traffic:
    std::string m_tmpstr;

    //TK: Like this so we can use LW hists when not in online mode:
    void updateRangeUser();
    int m_lastminbc;
    int m_lastmaxbc;
  };
}
#endif
