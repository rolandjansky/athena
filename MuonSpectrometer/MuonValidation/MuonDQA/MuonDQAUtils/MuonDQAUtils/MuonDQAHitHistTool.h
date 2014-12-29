/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONDQAHITHISTTOOL_H
#define MUON_MUONDQAHITHISTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonDQAUtils/MuonDQAHistosBase.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TProfile.h"

static const InterfaceID IID_MuonDQAHitHistTool("Muon::MuonDQAHitHistTool",1,0);

class MsgStream;

class TH1;
class TH2;

namespace Muon {

  /**
     @brief Tool to fill hit histograms.
     The tool can be used to:
     - extract information from hits into a data structure where it can be used for further analysis.
     The following information can be extracted
     - hit positions, residuals and pulls 
     - hit counts e.g. number of mdt hits, number of mdt chambers, etc.
     - fill the extracted information into a set of histograms
  */
  
  class MuonDQAHitHistTool : public AthAlgTool {
  public:
    struct DetData {
      DetData();
      std::string name;
      double res;
      double pull;
      double error;
      double lposX;
      double lposY;
    };

    struct HitResidualData {
      HitResidualData();
      std::vector<DetData> all;
      std::vector<DetData> mdts;
      std::vector<DetData> mdtsBarrelA;
      std::vector<DetData> mdtsBarrelC;
      std::vector<DetData> mdtsECA;
      std::vector<DetData> mdtsECC;

      std::vector<DetData> mdtsBarrelA1;
      std::vector<DetData> mdtsBarrelA2;
      std::vector<DetData> mdtsBarrelA3;
      std::vector<DetData> mdtsBarrelA4;
      std::vector<DetData> mdtsBarrelA5;
      std::vector<DetData> mdtsBarrelA6;
      std::vector<DetData> mdtsBarrelA7;
      std::vector<DetData> mdtsBarrelA8;

      std::vector<DetData> mdtsBarrelC1;
      std::vector<DetData> mdtsBarrelC2;
      std::vector<DetData> mdtsBarrelC3;
      std::vector<DetData> mdtsBarrelC4;
      std::vector<DetData> mdtsBarrelC5;
      std::vector<DetData> mdtsBarrelC6;
      std::vector<DetData> mdtsBarrelC7;
      std::vector<DetData> mdtsBarrelC8;

      std::vector<DetData> mdtsECA1;
      std::vector<DetData> mdtsECA2;
      std::vector<DetData> mdtsECA3;
      std::vector<DetData> mdtsECA4;
      std::vector<DetData> mdtsECA5;
      std::vector<DetData> mdtsECA6;
      std::vector<DetData> mdtsECA7;
      std::vector<DetData> mdtsECA8;

      std::vector<DetData> mdtsECC1;
      std::vector<DetData> mdtsECC2;
      std::vector<DetData> mdtsECC3;
      std::vector<DetData> mdtsECC4;
      std::vector<DetData> mdtsECC5;
      std::vector<DetData> mdtsECC6;
      std::vector<DetData> mdtsECC7;
      std::vector<DetData> mdtsECC8;

      std::vector<DetData> cscsEta;
      std::vector<DetData> cscsEta1;
      std::vector<DetData> cscsEta2;
      std::vector<DetData> cscsEta3;
      std::vector<DetData> cscsEta4;
      std::vector<DetData> cscsEta5;
      std::vector<DetData> cscsEta6;
      std::vector<DetData> cscsEta7;
      std::vector<DetData> cscsEta8;
      std::vector<DetData> cscsEta9;
      std::vector<DetData> cscsEta10;
      std::vector<DetData> cscsEta11;
      std::vector<DetData> cscsEta12;
      std::vector<DetData> cscsEta13;
      std::vector<DetData> cscsEta14;
      std::vector<DetData> cscsEta15;
      std::vector<DetData> cscsEta16;
      std::vector<DetData> cscsPhi;
      std::vector<DetData> cscsPhi1;
      std::vector<DetData> cscsPhi2;
      std::vector<DetData> cscsPhi3;
      std::vector<DetData> cscsPhi4;
      std::vector<DetData> cscsPhi5;
      std::vector<DetData> cscsPhi6;
      std::vector<DetData> cscsPhi7;
      std::vector<DetData> cscsPhi8;
      std::vector<DetData> cscsPhi9;
      std::vector<DetData> cscsPhi10;
      std::vector<DetData> cscsPhi11;
      std::vector<DetData> cscsPhi12;
      std::vector<DetData> cscsPhi13;
      std::vector<DetData> cscsPhi14;
      std::vector<DetData> cscsPhi15;
      std::vector<DetData> cscsPhi16; 

      std::vector<DetData> rpcsEta;
      std::vector<DetData> rpcsEtaA;
      std::vector<DetData> rpcsEtaC;

      std::vector<DetData> rpcsEtaA1;
      std::vector<DetData> rpcsEtaA2;
      std::vector<DetData> rpcsEtaA3;
      std::vector<DetData> rpcsEtaA4;
      std::vector<DetData> rpcsEtaA5;
      std::vector<DetData> rpcsEtaA6;
      std::vector<DetData> rpcsEtaA7;
      std::vector<DetData> rpcsEtaA8;

      std::vector<DetData> rpcsEtaC1;
      std::vector<DetData> rpcsEtaC2;
      std::vector<DetData> rpcsEtaC3;
      std::vector<DetData> rpcsEtaC4;
      std::vector<DetData> rpcsEtaC5;
      std::vector<DetData> rpcsEtaC6;
      std::vector<DetData> rpcsEtaC7;
      std::vector<DetData> rpcsEtaC8;

      std::vector<DetData> rpcsPhi;
      std::vector<DetData> rpcsPhiA;
      std::vector<DetData> rpcsPhiC;

      std::vector<DetData> rpcsPhiA1;
      std::vector<DetData> rpcsPhiA2;
      std::vector<DetData> rpcsPhiA3;
      std::vector<DetData> rpcsPhiA4;
      std::vector<DetData> rpcsPhiA5;
      std::vector<DetData> rpcsPhiA6;
      std::vector<DetData> rpcsPhiA7;
      std::vector<DetData> rpcsPhiA8;

      std::vector<DetData> rpcsPhiC1;
      std::vector<DetData> rpcsPhiC2;
      std::vector<DetData> rpcsPhiC3;
      std::vector<DetData> rpcsPhiC4;
      std::vector<DetData> rpcsPhiC5;
      std::vector<DetData> rpcsPhiC6;
      std::vector<DetData> rpcsPhiC7;
      std::vector<DetData> rpcsPhiC8;

      std::vector<DetData> tgcsEta;
      std::vector<DetData> tgcsEtaA;
      std::vector<DetData> tgcsEtaC;
      std::vector<DetData> tgcsPhi;
      std::vector<DetData> tgcsPhiA;
      std::vector<DetData> tgcsPhiC;
    };

    struct DetHistos : public MuonDQAHist::MuonDQAHistosBase {
      DetHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* res;
      TH1* resInner;
      TH1* resMiddle;
      TH1* resOuter;
      TH1* pull;
      TH1* pullInner;
      TH1* pullMiddle;
      TH1* pullOuter;
      TH1* error;
      TH1* lposX;
      TH1* lposY;
      TProfile* resProf;
      TProfile* pullProf;
    };

    struct HitResidualHistos : public MuonDQAHist::MuonDQAHistosBase {
      HitResidualHistos();
      bool wasBooked;
      bool wasRegistered;
      DetHistos all;
      DetHistos mdts;
      DetHistos mdtsBarrelA;
      DetHistos mdtsBarrelC;
      DetHistos mdtsECA;
      DetHistos mdtsECC;

      DetHistos mdtsBarrelA1;
      DetHistos mdtsBarrelA2;
      DetHistos mdtsBarrelA3;
      DetHistos mdtsBarrelA4;
      DetHistos mdtsBarrelA5;
      DetHistos mdtsBarrelA6;
      DetHistos mdtsBarrelA7;
      DetHistos mdtsBarrelA8;

      DetHistos mdtsBarrelC1;
      DetHistos mdtsBarrelC2;
      DetHistos mdtsBarrelC3;
      DetHistos mdtsBarrelC4;
      DetHistos mdtsBarrelC5;
      DetHistos mdtsBarrelC6;
      DetHistos mdtsBarrelC7;
      DetHistos mdtsBarrelC8;

      DetHistos mdtsECA1;
      DetHistos mdtsECA2;
      DetHistos mdtsECA3;
      DetHistos mdtsECA4;
      DetHistos mdtsECA5;
      DetHistos mdtsECA6;
      DetHistos mdtsECA7;
      DetHistos mdtsECA8;

      DetHistos mdtsECC1;
      DetHistos mdtsECC2;
      DetHistos mdtsECC3;
      DetHistos mdtsECC4;
      DetHistos mdtsECC5;
      DetHistos mdtsECC6;
      DetHistos mdtsECC7;
      DetHistos mdtsECC8;

      DetHistos cscsEta;
      DetHistos cscsEta1;
      DetHistos cscsEta2;
      DetHistos cscsEta3;
      DetHistos cscsEta4;
      DetHistos cscsEta5;
      DetHistos cscsEta6;
      DetHistos cscsEta7;
      DetHistos cscsEta8;
      DetHistos cscsEta9;
      DetHistos cscsEta10;
      DetHistos cscsEta11;
      DetHistos cscsEta12;
      DetHistos cscsEta13;
      DetHistos cscsEta14;
      DetHistos cscsEta15;
      DetHistos cscsEta16;
      DetHistos cscsPhi;
      DetHistos cscsPhi1;
      DetHistos cscsPhi2;
      DetHistos cscsPhi3;
      DetHistos cscsPhi4;
      DetHistos cscsPhi5;
      DetHistos cscsPhi6;
      DetHistos cscsPhi7;
      DetHistos cscsPhi8;
      DetHistos cscsPhi9;
      DetHistos cscsPhi10;
      DetHistos cscsPhi11;
      DetHistos cscsPhi12;
      DetHistos cscsPhi13;
      DetHistos cscsPhi14;
      DetHistos cscsPhi15;
      DetHistos cscsPhi16; 

      DetHistos rpcsEta;
      DetHistos rpcsEtaA;
      DetHistos rpcsEtaC;

      DetHistos rpcsEtaA1;
      DetHistos rpcsEtaA2;
      DetHistos rpcsEtaA3;
      DetHistos rpcsEtaA4;
      DetHistos rpcsEtaA5;
      DetHistos rpcsEtaA6;
      DetHistos rpcsEtaA7;
      DetHistos rpcsEtaA8;

      DetHistos rpcsEtaC1;
      DetHistos rpcsEtaC2;
      DetHistos rpcsEtaC3;
      DetHistos rpcsEtaC4;
      DetHistos rpcsEtaC5;
      DetHistos rpcsEtaC6;
      DetHistos rpcsEtaC7;
      DetHistos rpcsEtaC8;

      DetHistos rpcsPhi;
      DetHistos rpcsPhiA;
      DetHistos rpcsPhiC;

      DetHistos rpcsPhiA1;
      DetHistos rpcsPhiA2;
      DetHistos rpcsPhiA3;
      DetHistos rpcsPhiA4;
      DetHistos rpcsPhiA5;
      DetHistos rpcsPhiA6;
      DetHistos rpcsPhiA7;
      DetHistos rpcsPhiA8;

      DetHistos rpcsPhiC1;
      DetHistos rpcsPhiC2;
      DetHistos rpcsPhiC3;
      DetHistos rpcsPhiC4;
      DetHistos rpcsPhiC5;
      DetHistos rpcsPhiC6;
      DetHistos rpcsPhiC7;
      DetHistos rpcsPhiC8;

      DetHistos tgcsEta;
      DetHistos tgcsEtaA;
      DetHistos tgcsEtaC;
      DetHistos tgcsPhi;
      DetHistos tgcsPhiA;
      DetHistos tgcsPhiC;
    };


    struct AllCountData {
      AllCountData();
      int nall;
      int nallCh;
    };
    struct MdtCountData {
      MdtCountData();
      std::string name;
      double chi2;
      int nmdt;
      int nmdtCh;
    };
    struct ClusterCountData {
      ClusterCountData();
      int neta;
      int nphi;
      int netaCh;
      int nphiCh;
    };
    struct HitCountData {
      HitCountData();
      AllCountData all;
      MdtCountData mdts;
      MdtCountData mdtsBarrelA;
      MdtCountData mdtsBarrelC;
      MdtCountData mdtsECA;
      MdtCountData mdtsECC;

      MdtCountData mdtsBarrelA1;
      MdtCountData mdtsBarrelA2;
      MdtCountData mdtsBarrelA3;
      MdtCountData mdtsBarrelA4;
      MdtCountData mdtsBarrelA5;
      MdtCountData mdtsBarrelA6;
      MdtCountData mdtsBarrelA7;
      MdtCountData mdtsBarrelA8;

      MdtCountData mdtsBarrelC1;
      MdtCountData mdtsBarrelC2;
      MdtCountData mdtsBarrelC3;
      MdtCountData mdtsBarrelC4;
      MdtCountData mdtsBarrelC5;
      MdtCountData mdtsBarrelC6;
      MdtCountData mdtsBarrelC7;
      MdtCountData mdtsBarrelC8;

      MdtCountData mdtsECA1;
      MdtCountData mdtsECA2;
      MdtCountData mdtsECA3;
      MdtCountData mdtsECA4;
      MdtCountData mdtsECA5;
      MdtCountData mdtsECA6;
      MdtCountData mdtsECA7;
      MdtCountData mdtsECA8;

      MdtCountData mdtsECC1;
      MdtCountData mdtsECC2;
      MdtCountData mdtsECC3;
      MdtCountData mdtsECC4;
      MdtCountData mdtsECC5;
      MdtCountData mdtsECC6;
      MdtCountData mdtsECC7;
      MdtCountData mdtsECC8;

      ClusterCountData cscs;
      ClusterCountData rpcs;
      ClusterCountData tgcs;
    };
        
    struct AllCountHistos : public MuonDQAHist::MuonDQAHistosBase {
      AllCountHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* nall;
      TH1* nallCh;
    };
    struct MdtCountHistos : public MuonDQAHist::MuonDQAHistosBase {
      MdtCountHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* nmdt;
      TH1* nmdtInner;
      TH1* nmdtMiddle;
      TH1* nmdtOuter;
      TH1* nmdtCh;
      TProfile* nmdtProf;
      TH1* nsegm;
      TProfile* chi2Prof;
    };
    struct ClusterCountHistos  : public MuonDQAHist::MuonDQAHistosBase {
      ClusterCountHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* neta;
      TH1* nphi;
      TH1* netaCh;
      TH1* nphiCh;
    };
    struct HitCountHistos : public MuonDQAHist::MuonDQAHistosBase {
      HitCountHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* nprec;
      TH1* nprecCh;
      TH1* nphi;
      TH1* nphiCh;
      TH1* netaTrig;
      TH1* netaTrigCh;

      AllCountHistos     all;
      MdtCountHistos     mdts;
      MdtCountHistos     mdtsBarrelA;
      MdtCountHistos     mdtsBarrelC;
      MdtCountHistos     mdtsECA;
      MdtCountHistos     mdtsECC;

      MdtCountHistos     mdtsBarrelA1;
      MdtCountHistos     mdtsBarrelA2;
      MdtCountHistos     mdtsBarrelA3;
      MdtCountHistos     mdtsBarrelA4;
      MdtCountHistos     mdtsBarrelA5;
      MdtCountHistos     mdtsBarrelA6;
      MdtCountHistos     mdtsBarrelA7;
      MdtCountHistos     mdtsBarrelA8;

      MdtCountHistos     mdtsBarrelC1;
      MdtCountHistos     mdtsBarrelC2;
      MdtCountHistos     mdtsBarrelC3;
      MdtCountHistos     mdtsBarrelC4;
      MdtCountHistos     mdtsBarrelC5;
      MdtCountHistos     mdtsBarrelC6;
      MdtCountHistos     mdtsBarrelC7;
      MdtCountHistos     mdtsBarrelC8;

      MdtCountHistos     mdtsECA1;
      MdtCountHistos     mdtsECA2;
      MdtCountHistos     mdtsECA3;
      MdtCountHistos     mdtsECA4;
      MdtCountHistos     mdtsECA5;
      MdtCountHistos     mdtsECA6;
      MdtCountHistos     mdtsECA7;
      MdtCountHistos     mdtsECA8;

      MdtCountHistos     mdtsECC1;
      MdtCountHistos     mdtsECC2;
      MdtCountHistos     mdtsECC3;
      MdtCountHistos     mdtsECC4;
      MdtCountHistos     mdtsECC5;
      MdtCountHistos     mdtsECC6;
      MdtCountHistos     mdtsECC7;
      MdtCountHistos     mdtsECC8;

      ClusterCountHistos cscs;
      ClusterCountHistos rpcs;
      ClusterCountHistos tgcs;
    };

    struct HitData {
      HitData();
      HitResidualData residualData;
      HitCountData    countData;
    };


    struct HitHistos : public MuonDQAHist::MuonDQAHistosBase {
      HitHistos();
      bool wasBooked;
      bool wasRegistered;
      HitResidualHistos residualHistos;
      HitCountHistos    countHistos;
    };

  public:
    /** @brief constructor */
    MuonDQAHitHistTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonDQAHitHistTool();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQAHitHistTool; }

    /** @brief book a set of hit histograms, the prefix is added to the histogram names */
    bool book( HitHistos& histos, std::string prefix = "", bool doMDT = true, bool doRPC = true, bool doTGC = true, bool doCSC = false ) const; 
    bool book( HitCountHistos& histos, std::string prefix = "", bool doMDT = true, bool doRPC = true, bool doTGC = true, bool doCSC = false ) const;
    bool book( HitResidualHistos& histos, std::string prefix = "", bool doMDT = true, bool doRPC = true, bool doTGC = true, bool doCSC = false ) const;
    
    /** @brief register histogram to monitoring group*/ 
    void registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, ManagedMonitorToolBase::MonGroup& groupBarrelA, ManagedMonitorToolBase::MonGroup& groupBarrelC, ManagedMonitorToolBase::MonGroup& groupEC, ManagedMonitorToolBase::MonGroup& groupECA, ManagedMonitorToolBase::MonGroup& groupECC, HitHistos& histos );

    void registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, ManagedMonitorToolBase::MonGroup& groupBarrelA, ManagedMonitorToolBase::MonGroup& groupBarrelC, ManagedMonitorToolBase::MonGroup& groupEC, ManagedMonitorToolBase::MonGroup& groupECA, ManagedMonitorToolBase::MonGroup& groupECC, HitResidualHistos& histos );
 
    void registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, ManagedMonitorToolBase::MonGroup& groupBarrelA, ManagedMonitorToolBase::MonGroup& groupBarrelC, ManagedMonitorToolBase::MonGroup& groupEC, ManagedMonitorToolBase::MonGroup& groupECA, ManagedMonitorToolBase::MonGroup& groupECC, HitCountHistos& histos );

    void registerHist( ManagedMonitorToolBase::MonGroup& group, DetHistos& histos );
    
    void registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, DetHistos& histos );

    void registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, AllCountHistos& histos );

    void registerHist( ManagedMonitorToolBase::MonGroup& groupDebug, MdtCountHistos& histos );

    void registerHist( ManagedMonitorToolBase::MonGroup& groupDebug, ClusterCountHistos& histos );

    /** @brief fill the full set of hit histos */
    bool fill( HitHistos& histos, const HitData& hitData ) const;

    /** @brief fill hit-count histograms */
    bool fill( HitCountHistos& histos, const HitCountData& data ) const;
    bool fill( AllCountHistos& histos, const AllCountData& data ) const;
    bool fill( MdtCountHistos& histos, const MdtCountData& data ) const;
    bool fill( ClusterCountHistos& histos, const ClusterCountData& data ) const;

    /** @brief fill hit-residual histograms */
    bool fill( HitResidualHistos& histos, const HitResidualData& data ) const;
    bool fill( DetHistos& histos, const std::vector<DetData>& detDataVec ) const;
    bool fill( DetHistos& histos, const DetData& detData ) const;

  private:
    /** @brief book a set of hit histograms, the prefix is added to the histogram names */
    bool book( DetHistos& histos, std::string resName, std::string pullName, std::string errorName, std::string lposXName, std::string lposYName, std::string prefix ) const;
    bool book( ClusterCountHistos& histos, std::string netaName, std::string nphiName, std::string netaChName, std::string nphiChName, std::string prefix ) const;
    bool book( AllCountHistos& histos, std::string nallName, std::string nallChName, std::string prefix ) const;
    bool book( MdtCountHistos& histos, std::string nmdtName, std::string nmdtChName, std::string prefix ) const;

    MsgStream*  m_log;   //!< Gaudi message stream - msgstream as private member (-> speed)
  };

}

#endif
 
