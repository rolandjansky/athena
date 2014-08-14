/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_MUONTRKPHYSMONITORING_H
#define MUONTRKPHYSMONITORING_MUONTRKPHYSMONITORING_H

// *****************************************************************************
// MuonTrkPhysMonitoring.cxx
// AUTHORS: Nektarios Chr.Benekos, Austin Basye, Christina G. Skarpathiotaki
// *****************************************************************************

// STL Libs
//-----------------------------------------------
#include <vector>
#include <iostream>

// Athena Monitoring (Base Class Def)
//-----------------------------------------------
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Generic Tracking and 4Momentum Interfaces
//-----------------------------------------------
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "FourMomUtils/P4Helpers.h"


// Detector Specific Tracking
//-----------------------------------------------
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"

// Root Data Structures
//-----------------------------------------------
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include "MuonTrkPhysMonUtils.h"
#include "DetSpcfcPlots.h"
#include "CbMSIDAlignPlots.h"
#include "BFMonPlots.h"
#include "ZSignalPlots.h"
#include "OnlinePlots.h"
#include "OccupancyPlots.h"
#include "GenTrackingPlots.h"

using namespace MuonTrkPhysMonUtils;

// Begin MuonTrkPhysMonitoring Class Desciption
//---------------------------------------------------------------------------------------
//   The MuonTrkPhysMonitoring slass is the result of simultaneous efforts to both create
// a High-Level muon monitoring package, and to unify the MCP and Muon monitoring packages
// as a result, this package grew out of a prototype for the first effort to encapsulate
// the latter. Thus, this software is quite bifurcated, and efforts are ongoing to stream-
// line the following class defintion.
//---------------------------------------------------------------------------------------
class MuonTrkPhysMonitoring : public ManagedMonitorToolBase
{
	
 public:
	
  /** Conscructor */
  MuonTrkPhysMonitoring( const std::string & type, const std::string & name, const IInterface* parent );
    
  /** Descructor */
  virtual ~MuonTrkPhysMonitoring();

  /** Global Intialization Method */
  StatusCode initialize();
	
  /** Global Histogram Booking and Registration Method */
  // We will call the Monitoring Subgroup Book Methods from here
  //---------------------------------------------------------------------------------------
  //virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  virtual StatusCode bookHistogramsRecurrent();
    
  /** Global Filling Method */
  // We will call the Monitoring Subgroup Fill Methods from here
  virtual StatusCode fillHistograms();
    
  /** Global Proccessing Method */
  // We will call the Monitoring Subgroup Process Methods from here
    
  virtual StatusCode procHistograms();
  //---------------------------------------------------------------------------------------
    
  /** Global Finalization Method */
  StatusCode finalize();
    
  /** fillLumiBlock [??] */
  StatusCode fillLumiBlock();
	
 protected:
    
  // The Following Protected Methods provide functionality for each of our
  //   Monitoring Sub-Groups. (The Directories of the Web Display)
  //
  // Each Group Is defined in the file with the named suffix from the Method Name
  //   For example: bookHistograms_CbMSIDAlign is defined in MTPM_CbMSIDAlign
  //---------------------------------------------------------------------------------------
    
  // Combined Muon Alignment Monitoring
  void bookHistograms_CbMSIDAlign(std::string sPath);
  void fillHistograms_CbMSIDAlign(const xAOD::Muon* Muon, const xAOD::MuonContainer* MuonJar);
  void procHistograms_CbMSIDAlign();
    
  // Magnetic (B)-Field Alignment Monitoring
  void bookHistograms_BFMon(std::string sPath);
  void fillHistograms_BFMon(const xAOD::Muon* Muon);
  void procHistograms_BFMon();
    
  // General Track Monitoring
  void bookHistograms_GenTracking(std::string sPath);
  void fillHistograms_GenTracking(const xAOD::Muon *Muon);
  void procHistograms_GenTracking();
    
  // Z tag-and-probe Signal Monitoring
  void bookHistograms_ZSignal(std::string sPath);
  void fillHistograms_ZSignal( const xAOD::Muon* muon, const xAOD::MuonContainer* MuonJar, const xAOD::TrackParticleContainer* IDTrackContainer);
  void fillTagAndProbe(const xAOD::Muon* Muon, const xAOD::TrackParticle* RecordProbe, const xAOD::MuonContainer* MuonJar);
  void procHistograms_ZSignal();
    
  // Detector Specific Monitoring
  void bookHistograms_DetSpcfc(std::string sPath);
  void fillHistograms_DetSpcfc( const xAOD::Muon* muon );
  void procHistograms_DetSpcfc();
    
 private:

  // Athena Bookkeeping
  //___________________________
  /* Gaudi message stream - msgstream as private member (-> speed) */
  mutable MsgStream      m_log;

  //---------------------------------------------------------------------------------------
  // Miscellaneous Helper Methods
  //---------------------------------------------------------------------------------------
    
  // Tool Initialization
  StatusCode setupTools();

  // General functions for handling containers and analysis objects
  bool handleMuonContainer( std::string collectionName); 
  void handleMuon( const xAOD::Muon* muon, const xAOD::MuonContainer* MuonJar, 
                  const xAOD::TrackParticleContainer* IDTrackContainer, const xAOD::MuonSegmentContainer* SegmentContainer);

  // General Helpers
  bool passTrigger();
  bool passPtCut( float pt );

 // Cb - MS - ID Alignment Monitoring
  bool passZSelection(const xAOD::Muon *Muon, const xAOD::MuonContainer* MuonJar);
  TVector3 BarycentricCenter(const Amg::Vector3D& InnerPoint,const Amg::Vector3D& MiddlePoint,const Amg::Vector3D& OuterPoint);

  // B-Field Monitoring
  void FillSagittaPlots(const Amg::Vector3D& InnerPoint,const Amg::Vector3D& MiddlePoint,const Amg::Vector3D& OuterPoint, const xAOD::Muon* Muon);
  void AddSagittaCalculationInfo(Amg::Vector3D& vec, int& hitInfo, const Trk::TrackParameters* trackParameters);

  // Tag and Probe (ZSignal) Monitoring  
  void fillTagAndProbe( const xAOD::Muon* muon, const xAOD::MuonContainer* MuonJar, const xAOD::TrackParticleContainer* IDTrackContainer);
  bool IsTagMuon(const xAOD::Muon* muon);
  bool IsZTriggerMatched(const xAOD::Muon* muon);
  bool MCPRecommendationsSatisfied(const xAOD::TrackParticle* trk);
  bool IsProbeParticle(const xAOD::TrackParticle* probe,  const xAOD::TrackParticleContainer* IDTrackContainer);
  bool probeIsolation(const xAOD::TrackParticle *trackParticle, const xAOD::TrackParticleContainer* IDTrackContainer, float cut);
  const xAOD::Muon* ProbeToMuonMatching(const xAOD::TrackParticle* RecordProbe, const xAOD::MuonContainer* MuonJar);
  
  // Detector Specific Monitoring
  void CalculateResidualFromTrackStateOnSurface(const Trk::TrackStateOnSurface* stateOnSurface, const Trk::Perigee *measPerigee);
  void PrintResidualDebugInfo(double residual, const Identifier &id);
  void fillResidualPlots(double residual, const Identifier &id, double error,const Trk::Perigee *measPerigee);
  
  // GenTracking Monitoring
  int retrieveLumiBlock();
  bool passIDTrackQuality(const xAOD::TrackParticle *trackParticle);
  bool passMSTrackQuality(const xAOD::TrackParticle *MSTrackParticle, int HitReqI=1, int HitReqM=1, int HitReqO=1, int HitReqTotal=3);
  void FillT02DPlots(TH1F* hist, TH2F* weightshist, TH2F* entrieshist, float xValue, float yValue, float info);
  
  // Specific Data Retrieval
  bool matchSegmentTrk(const Trk::Track* origTrk, const xAOD::MuonSegment* seg);
    
  // Selection Tools
  void fillT0Plots(const xAOD::MuonSegmentContainer* segments, const xAOD::TrackParticle* MSTrackParticle, const xAOD::Muon* muon, bool extendedPlots );
  void FillMSPlots(TH2F *hWeights, TH2F *hEntries, int location, float info);
                        
  // Data Identification
  //___________________________
  /* Trigger Aware Members */
  bool m_useTrigger;
  std::vector<std::string> m_muon_triggers;
  std::string m_MuonTriggerChainName; //[??] <- Does MTPM-Class really need this?
        
  // User Requested Data Container Names
  //___________________________
  std::string               m_InnerTrackContainer;
  std::string               m_trackCollectionName;
        
  // ATLAS Detector Description
  //___________________________
        
  /** ToolHandle idHelper*/
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelperTool;
  ToolHandle<Muon::MuonEDMHelperTool>   m_helperTool;
  ToolHandle<Muon::IMuonHitSummaryTool> m_muonHitSummaryTool;
	
  //<! tool to calculate residuals and pulls
  ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;
    
  // Selection Criteria Tools
  //___________________________
  float m_GlobalPtCut_pp;
  float m_GlobalPtCut_cosmic;
  float m_GlobalPtCut_heavyIon;

  float m_GlobalResidualError;
    
  // Trigger Matching for Tag Muon
  //___________________________
  bool                     m_doZTrigMatch;
  std::string              m_tagMuonTrigger;
    
  //isolation for Tag and Probe
  //___________________________
  double m_isoConeMin;
  double m_isoConeMax;
    
  double m_cut_diMuonMass;
	
  // Waste -or- What is This [??]
  int m_lumiblock;
  int m_eventCounter; //[??] Currently used to swap ZSignal tnp charge
  
  //---------------------------------------------------------------------------------------
  // Muon Track Monitoring Histograms
  //---------------------------------------------------------------------------------------
  OnlinePlots       m_oOnlinePlots;
  OccupancyPlots    m_oOccupancyPlots;
  GenTrackingPlots  m_oGenTrackingPlots;
  BFMonPlots        m_oBFMonPlots;    
  DetSpcfcPlots     m_oDetSpcfcPlots;	
  CbMSIDAlignPlots  m_oCbMSIDAlignPlots;
  ZSignalPlots      m_oZSignalPlots;	
  	
void Book(TH1* hist, MonGroup& monGroup){
  if(monGroup.regHist(hist).isFailure()) ATH_MSG_ERROR(" Failed to register histogram " << hist->GetName());
}

void InitializePlots(PlotBase& oPlots, MonGroup oMonGroup){
  oPlots.setTrackCollectionName(m_trackCollectionName);
  oPlots.initialize();
  for (auto histVec : oPlots.retrieveBooked1DHistograms()) Book(histVec, oMonGroup);
  for (auto histVec : oPlots.retrieveBooked2DHistograms()) Book(histVec, oMonGroup);
}

uint8_t RetrieveHitInfo(const xAOD::Muon* muon,const xAOD::MuonSummaryType& info, const uint8_t defaultVal = 0){
  uint8_t val = defaultVal;
  if (!muon->summaryValue(val,info)) val = defaultVal;
  return val;
}

uint8_t RetrieveHitInfo(const xAOD::TrackParticle* trkprt,const xAOD::SummaryType& info, const uint8_t defaultVal = 0){
  uint8_t val = defaultVal;
  if (!trkprt->summaryValue(val,info)) val = defaultVal;
  return val;
}

};

#endif
