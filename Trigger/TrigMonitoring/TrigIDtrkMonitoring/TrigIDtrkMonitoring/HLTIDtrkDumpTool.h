/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDtrkDumpTool.h
 *
 *  Contact:
 *  @author Stefan.Ask (AT cern DOT ch)
 *  @author Emily Nurse () 
 *  
 *  Comment: Produce detailed trigger aware Tier0 histograms 
 *           Done for individual benchmark chains within the 
 *           different slices. 
 *
 *  Todo: Make plots aware of warm period, through DCS info (?)
 *
 */

#ifndef HLTIDTRKDUMPTOOL_H
#define HLTIDTRKDUMPTOOL_H

#include <string>
#include <vector>
#include <map>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "tauEvent/TauJetContainer.h" //TJN
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/Feature.h"
#include "TLorentzVector.h"

// Forward declarations
class IInterface;
class StoreGateSvc;
class TH1I;
class TH1F;
class StatusCode;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


namespace HLTIDDump{

  class simpleTrack : public TLorentzVector { 
  public: 
    float d0, z0, nPixHits, nSctHits, nTrtHits, etaR, phiR;
    int author;
  };

  // Helper class for the histograms
  class HistogramKeeper {
    
  public:

    HistogramKeeper() {par_IDS.push_back(""); par_SIT.push_back(""); par_L2S.push_back(""); par_TSF.push_back(""); par_EF.push_back(""); par_EFT.push_back(""); par_OFF.push_back(""); par_TRU.push_back("");}

  public:    
    // Histogram names 
    std::vector< std::string > par_IDS;      // IDSCAN
    std::vector< std::string > par_SIT;      // SiTrack
    std::vector< std::string > par_L2S;      // L2 Star
    std::vector< std::string > par_TSF;      // TRTSegFinder
    std::vector< std::string > par_EF;       // Event Filter
    std::vector< std::string > par_EFT;      // Event Filter, TRT only
    std::vector< std::string > par_OFF;      // Track parameters
    std::vector< std::string > par_TRU;      // Track parameters
    
  };  

  // struct to hold input variables
  class SliceVariables {

  public:

    SliceVariables() {doMon=false; fullScan=false; ROIHalfWidth=0.0; sigTracks.push_back(""); sigSelect.push_back(""); sigSelectL2.push_back(""); doIDSCAN.push_back(false); doSiTrack.push_back(false); doL2Star.push_back(false); doTRTSegF.push_back(false); doEF.push_back(false); doEFtrt.push_back(false); IDSCANCollection.push_back(""); SiTrackCollection.push_back(""); L2StarCollection.push_back("");  TRTSegFCollection.push_back(""); EFCollection.push_back(""); EFtrtCollection.push_back(""); staco=false; name=""; roiName=""; suffix="";}

    bool                       doMon;
    bool                       fullScan;
    float                      ROIHalfWidth;
    std::vector< std::string > sigTracks;
    std::vector< std::string > sigSelect;
    std::vector< std::string > sigSelectL2;
    std::vector< bool >        doIDSCAN;
    std::vector< bool >        doSiTrack;
    std::vector< bool >        doL2Star;
    std::vector< bool >        doTRTSegF;
    std::vector< bool >        doEF;
    std::vector< bool >        doEFtrt;
    std::vector< std::string > IDSCANCollection;
    std::vector< std::string > SiTrackCollection;
    std::vector< std::string > L2StarCollection;
    std::vector< std::string > TRTSegFCollection;
    std::vector< std::string > EFCollection;
    std::vector< std::string > EFtrtCollection;
    bool                       staco; // JWH
    std::string                name; // JWH
    std::string                roiName; // JWH
    std::string                suffix; // JWH
    //+++ Histograms with track: parameters/residuals/efficies for each signature
    HLTIDDump::HistogramKeeper histKeeper;


  };

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class HLTIDtrkDumpTool : public IHLTMonTool
{

 public:

  //+++ Standard Athena algorithm methods
  HLTIDtrkDumpTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTIDtrkDumpTool();
  
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

 private:
  
  /**
   * Detailed monitoring/Validation histograms related to tracking for the muon slice
   */
  StatusCode  cosMon( );
  StatusCode  egMon( );
  StatusCode  egcombMon( );
  StatusCode  tauMon( );
  StatusCode  newtauMon( ); 
  StatusCode  muMon( ); 
  StatusCode  mucombMon( ); //JWH
  StatusCode  fsMon( ); 
  StatusCode  mbMon( ); 
  StatusCode  bMon( ); // JWH
  
  /**
   * Methods for the basic histograms
   */
  
  StatusCode  fillBasicHist( HLTIDDump::SliceVariables & SLICE);
  
  void  bookMonHist( std::vector< std::string > & variables, 
		     HLTIDDump::SliceVariables & Slice,
		     std::string sName, 
		     bool isNewRun );
  
  void fillParameterHist( std::vector< HLTIDDump::simpleTrack > & tracks, 
			  const std::string & sigTracks,
			  const std::string & sigSelect,
			  const std::string & trkType,
			  HLTIDDump::SliceVariables & SLICE );
  
  void  fillResidualHist( std::vector< HLTIDDump::simpleTrack > & recoTracks, 
			  std::vector< HLTIDDump::simpleTrack > & truthTracks,
			  const std::string & sigTracks,
			  const std::string & sigSelect,
			  const std::string & trkType,
			  std::map<int, std::vector<int> > & truth2reco, 
			  HLTIDDump::SliceVariables & SLICE);
  
  void  fillEfficiencyHist( std::vector< HLTIDDump::simpleTrack > & truthTracks,
			    const std::string & sigTracks,
			    const std::string & sigSelect,
			    const std::string & trkType,
			    std::map<int, std::vector<int> > & truth2reco,
			    HLTIDDump::SliceVariables & SLICE );
  
  void  fillFakeHist( std::vector< HLTIDDump::simpleTrack > & recoTracks, 
		      const std::string & sigTracks,
		      const std::string & sigSelect,
		      const std::string & trkType,
		      std::map<int, std::vector<int> > & reco2truth,
		      HLTIDDump::SliceVariables & SLICE );
  
  /**
   * Methods for the physics monitoring histograms
   * 
   * PhysMb:  Min bias related quantities (currently in FullScan slice).
   * PhysLum: Luminosity relted... 
   */
  
  void fillPhysMbHist( std::vector< HLTIDDump::simpleTrack > & tracks, std::string trkType);
  
  
  /**
   * Returns the tracks that were inside any of the RoIs
   * Input: tracks, RoIs, etaHalf, phiHalf
   */ 
  std::vector< HLTIDDump::simpleTrack >  inSideRoi( std::vector< HLTIDDump::simpleTrack > & trackVector, 
						    std::vector< Trig::Feature<TrigRoiDescriptor> > & vecRoi, 
						    float etaRoiHalf, float phiRoiHalf, std::string alg );
  
  /**
   * TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL
   * Returns (selected) tracks in generic a track format.
   */
  std::vector< HLTIDDump::simpleTrack > GetTrackPartVectors( const DataHandle<Rec::TrackParticleContainer> trackPartCont) ;
  
  std::vector< HLTIDDump::simpleTrack > GetTrackPartVectors( const DataHandle<Rec::TrackParticleContainer> trackPartCont,
							     std::vector< HLTIDDump::simpleTrack > & siSelected, 
							     std::vector< HLTIDDump::simpleTrack > & trtSelected, 
							     bool selection = true) ;
  
  std::vector< HLTIDDump::simpleTrack > GetTrackPartVectors( std::vector< HLTIDDump::simpleTrack > trackPartCont, 
							     std::vector< HLTIDDump::simpleTrack > & siSelected, 
							     std::vector< HLTIDDump::simpleTrack > & trtSelected, 
							     bool selection = true) ;
  
  std::vector< HLTIDDump::simpleTrack > GetTrigInDetTrackVectors( const DataHandle<TrigInDetTrackCollection> trigInDetTrkCont, 
								  bool selection) ;
  std::vector< HLTIDDump::simpleTrack > GetTrigInDetTrackVectors( const DataHandle<TrigInDetTrackCollection> trigInDetTrkCont, 
								  std::vector< HLTIDDump::simpleTrack > trackCollection_A,
								  std::vector< HLTIDDump::simpleTrack > trackCollection_B,
								  std::vector< HLTIDDump::simpleTrack > trackCollection_C,
								  bool selection) ;
  std::vector< HLTIDDump::simpleTrack > GetTrigInDetTrackVectors( const std::vector< Trig::Feature<TrigInDetTrackCollection> >& feature,
								  bool selection, bool verbose);
  std::vector< HLTIDDump::simpleTrack > GetTrigInDetTrackVectors( const std::vector< Trig::Feature<TrigInDetTrackCollection> >& feature,
								  bool selection, bool verbose,
								  int alg, std::string collection);
  std::vector< HLTIDDump::simpleTrack > GetTrigInDetTrackVectors( const std::vector< Trig::Feature<Rec::TrackParticleContainer> >&  feature, 
								  bool selection, bool verbose);
  
  std::vector< HLTIDDump::simpleTrack > GetTauTracks();

  std::vector< HLTIDDump::simpleTrack > GetElectronTracks();

  /**
   * TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL
   * Match (dR) of tracks in first vector wrt the second.
   */
  std::map<int, std::vector<int> > MatchTracks( const std::vector< HLTIDDump::simpleTrack > & precisetracks, 
						const std::vector< HLTIDDump::simpleTrack > & coarsetracks, 
						double deltaEta, double deltaPhi, bool verbose );
  
 private:
  
  std::string makeHistName ( const std::string & sigTracks,
			     const std::string & sigSelect,
			     const std::string & par,
			     const std::string & trkType, 
                             HLTIDDump::SliceVariables & SLICE) const; //JWH
  
  void  appendVector(const HLTIDDump::HistogramKeeper & hist,  std::vector <std::string> & allNames);
  
  //  void  divideHistograms(const  std::vector<std::string> & allNames);
  
  bool checkSize(HLTIDDump::SliceVariables & SLICE) ;
  
  //void  declareRunVariables(HLTIDDump::SliceVariables & SLICE, const std::string & sliceName);
  
  
  //+++ Trigger decision tool related
  ToolHandle<Trig::TrigDecisionTool> m_TDT;
  
  //+++ General variables
  bool                       m_data;
  bool                       m_useTDT;
  std::string                m_offlineCollection;
  std::string                m_offlineCollectionStaco; //JWH
  std::string                m_mcCollection;
  std::vector< std::string > m_Variables;  // Histogramed variables
  
  //+++ Track selection and match for basic plots
  double                     m_deltaEta;
  double                     m_deltaEtaTRT;  // To distinguish between specail cases for the TRT.
  double                     m_deltaPhi;
  double                     m_minPt;  
  double                     m_maxAbsEta;  
  double                     m_maxAbsD0; 
  double                     m_maxAbsZ0;
  bool                       m_useSiHits;
  double                     m_minSiHits;    // 2 * pixel + sct
  double                     m_minPixHits;
  double                     m_minSctHits; 
  double                     m_minTrtHits;  
  
  HLTIDDump::SliceVariables m_COSMIC;
  HLTIDDump::SliceVariables m_EGAMMA;
  HLTIDDump::SliceVariables m_EGAMMACOMB;
  HLTIDDump::SliceVariables m_TAU;
  HLTIDDump::SliceVariables m_NEWTAU; //TJN
  HLTIDDump::SliceVariables m_MUON;
  HLTIDDump::SliceVariables m_MUONCOMB; // JWH
  HLTIDDump::SliceVariables m_FULLSCAN;
  HLTIDDump::SliceVariables m_MINBIAS;
  HLTIDDump::SliceVariables m_BJET;
  
  //+++ Selection etc. for physics monitoring
  bool                       m_pMbMon; 
  double                     m_pMbMinPt;
  double                     m_pMbMaxAbsEta;
  double                     m_pMbMaxAbsD0;
  double                     m_pMbMaxAbsZ0;
  std::string                m_pMbTrigChain;
  std::string                m_pMbCollOFF;
  std::string                m_pMbCollEF;
  
};

#endif
