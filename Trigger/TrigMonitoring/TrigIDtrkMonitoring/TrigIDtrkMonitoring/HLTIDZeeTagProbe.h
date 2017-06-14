/// Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#if 0

#ifndef HLTIDZEETAGPROBE_H
#define HLTIDZEETAGPROBE_H

//#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/Algorithm.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egDetail.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/EMShower.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "egammaInterfaces/IegammaShowerShape.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
//#include "AthenaKernel/DefaultKey.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigDecisionTool/Feature.h"

#include <string>
#include <vector>

#include "TLorentzVector.h"
#include "TFile.h"
#include "TVector.h"
#include "TH1F.h"
#include "TProfile2D.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"


/*class simpleTrack : public TLorentzVector {
 public:
  float d0, z0, nPixHits, nSctHits, nTrtHits, etaR, phiR;
  };*/

class StoreGateSvc;
class TH1I;
class TH1F;
class StatusCode;
//class Electron;

namespace HLTIDTagProbe{

  class simpleTrack : public TLorentzVector {
  public:
    float d0, z0, nPixHits, nSctHits, nTrtHits, etaR, phiR;
      float eta_extrapol, phi_extrapol, charge;
      int author;
  };

  // Helper class for the histograms
  class HistogramKeeper {

  public:

    HistogramKeeper() {}

  public:
    // Histogram names
    std::vector< std::string > par_IDS;      // IDSCAN
    std::vector< std::string > par_SIT;      // SiTrack
    std::vector< std::string > par_TSF;      // TRTSegFinder
    std::vector< std::string > par_EF;       // Event Filter
    std::vector< std::string > par_EFT;      // Event Filter, TRT only
    std::vector< std::string > par_OFF;      // Track parameters
    std::vector< std::string > par_TRU;      // Track parameters

  };

  // struct to hold input variables
  struct SliceVariables {

    bool                       doMon;
    bool                       fullScan;
    float                      ROIHalfWidth;
    std::vector< std::string > sigTracks;
    std::vector< std::string > sigSelect;
    std::vector< bool >        doIDSCAN;
    std::vector< bool >        doSiTrack;
    std::vector< bool >        doTRTSegF;
    std::vector< bool >        doEF;
    std::vector< bool >        doEFtrt;
    std::vector< std::string > IDSCANCollection;
    std::vector< std::string > SiTrackCollection;
    std::vector< std::string > TRTSegFCollection;
    std::vector< std::string > EFCollection;
    std::vector< std::string > EFtrtCollection;
    bool                       staco; // JWH
    std::string                name; // JWH
    //+++ Histograms with track: parameters/residuals/efficies for each signature
    HLTIDTagProbe::HistogramKeeper histKeeper;


  };

}


//class HLTIDZeeTagProbe : public AthAlgorithm
class HLTIDZeeTagProbe : public IHLTMonTool
{
 public:

  //HLTIDZeeTagProbe(const std::string& name, ISvcLocator* pSvcLocator);
  HLTIDZeeTagProbe(const std::string & type, const std::string & name, const IInterface* parent);
  //  StatusCode initialize();
  //  StatusCode finalize();
  //  StatusCode execute();

  StatusCode init();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun);
#endif

  StatusCode fill();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool /* endOfEventsBlock */, bool /* endOfLumiBlock */, bool endOfRun);
#endif

  //  std::vector< HLTIDTagProbe::simpleTrack > GetTrigInDetTrackVectors( std::vector< Trig::Feature<TrigInDetTrackCollection> >& feature,bool selection);
  //  std::vector< HLTIDTagProbe::simpleTrack > GetTrigInDetTrackVectors( std::vector< Trig::Feature<Rec::TrackParticleContainer> >& feature,bool selection);
  std::vector< HLTIDTagProbe::simpleTrack > GetTrigInDetTrackVectors(const Trig::Feature<Rec::TrackParticleContainer>& feature, bool selection, std::vector< HLTIDTagProbe::simpleTrack >, const TrigRoiDescriptor* );
  std::vector< HLTIDTagProbe::simpleTrack > GetTrigInDetTrackVectors(const Trig::Feature<TrigInDetTrackCollection>& feature, bool selection, std::vector< HLTIDTagProbe::simpleTrack >,const TrigRoiDescriptor* );
  std::vector< HLTIDTagProbe::simpleTrack > GetTrigInDetTrackVectors(const Trig::Feature<TrigInDetTrackCollection>& feature, bool selection,const TrigRoiDescriptor*, int alg );

  bool fillProbeHistograms( std::string, std::string, const egamma*, const CaloCluster*, const TrigRoiDescriptor*, std::vector< HLTIDTagProbe::simpleTrack >, HLTIDTagProbe::simpleTrack, int ntracks_tag=-1);
  bool fillProbeHistograms( std::string, std::string, const TrigElectron*, const TrigEMCluster*, const TrigRoiDescriptor*, std::vector< HLTIDTagProbe::simpleTrack >, HLTIDTagProbe::simpleTrack,  int ntracks_tag=-1);

  int counter_roi;

  double dPhiHelper(double phi1, double phi2);

 private:

  StoreGateSvc* m_storeGate;

  //+++ Trigger decision tool related
  ToolHandle<Trig::TrigDecisionTool> TrigDec;


  ToolHandle<IExtrapolateToCaloTool>     tracktocalo;
  ToolHandle<ITrigInDetTrackExtrapolator> m_trackExtrapolator;

  //  ToolHandle<TrigMatchTool> TrigMatch;

  //std::string m_L2IDTracks;
  //std::string m_EFIDTracks;
  //std::string m_OfflineTracks;

  std::string m_tightElectronCollectionName;

  std::vector<std::string> m_TrigChains;
  std::vector<std::string> m_AlgNames;
  std::vector<std::string> m_Parameters;

  std::vector<std::string> HistoNames;

  std::vector<TH1F*> HistoPointers;

  double m_etabins, m_etabinmax, m_etabinmin;
  double m_phibins, m_phibinmax, m_phibinmin;
  double m_ptbins, m_ptbinmax, m_ptbinmin;
  double m_z0bins, m_z0binmax, m_z0binmin;
  double m_d0bins, m_d0binmax, m_d0binmin;
  double m_nvtxbins, m_nvtxbinmax, m_nvtxbinmin;
  double m_mZbins, m_mZbinmax, m_mZbinmin;

  bool do_excludeICelec; //to exclude electrons and custers in the region 1.37 < |eta| < 1.52
  bool do_debugging;

  TH1F* h_tag_eta;
  TH1F* h_tag_pt;
  TH1F* h_tag_phi;
  TH1F* h_probe_eta;
  TH1F* h_probe_pt;
  TH1F* h_probe_phi;
  TH1F* h_Z_mass;
  //TProfile2D* e20_tight_e15_NoCut_Zee_EF_eta_phi_eff;

  std::string m_FileName;
  TFile* m_outputFile;

  //radius and Z of calorimeter face
  float m_RCAL;  //!<  radious of calorimeter face
  float m_ZCAL;  //!<  Z of calorimeter face

  //for number vertices
  int nvtx;

  std::vector< std::string > m_TrigChainName;

};

#endif // HLTIDZEETAGPROBE_H

#endif

