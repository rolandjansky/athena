/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**    @file SCTHitsNoiseMonTool.h
 *   Class declaration for SCTHitsMonTool
 *   
 *                          
 *                          
 *    @author Sofia Chouridou based on code from Luca Fiorini, Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *  
 *   
 * 
 *   
 */

#ifndef SCTHITSNOISEMONTOOL_H
#define SCTHITSNOISEMONTOOL_H

#include <string>
#include <vector>
#include <map>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"


// for CondDB
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

// Forward declarations
class IInterface;
class TH1F;
class TH1I;
class TH2I;
class TH2F;
class TH2D;
class TH1D;
class TProfile2D;
class TProfile;
class Identifier;
class StatusCode;
class SCT_ID;

class PairBuilder;

///Concrete monitoring tool derived from SCTMotherTrigMonTool
class SCTHitsNoiseMonTool : public SCTMotherTrigMonTool{
 public:
  typedef unsigned int  ChipNumberType;
  SCTHitsNoiseMonTool(const std::string & type, const std::string & name,const IInterface* parent); 
  ~SCTHitsNoiseMonTool();
/**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book is called at the beginning
  //  virtual StatusCode bookHistograms(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun ) ;
  virtual StatusCode bookHistograms() ;
  virtual StatusCode bookHistogramsRecurrent() ;
  ///fill is called on each event loop
  virtual StatusCode fillHistograms() ;
  ///checkHists not currently used
  StatusCode checkHists(bool fromFinalize);
  ///procHistograms is called at the end
  //  virtual StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  virtual StatusCode procHistograms();
//@} 
  
private:
  typedef TProfile2D * Prof2_t;
   typedef TProfile * Prof_t;
   typedef TH1I * H1I_t;
   typedef TH1D * H1D_t;
   typedef TH2I * H2I_t;
   typedef TH1F * H1_t;
   typedef TH2F * H2_t;
   typedef std::vector<Prof2_t> VecProf2_t;
   typedef std::vector<H1I_t> VecH1I_t;
   typedef std::vector<H1D_t> VecH1D_t;
   typedef std::vector<H2I_t> VecH2I_t;
   typedef std::vector<H1_t> VecH1_t;
   typedef std::vector<H2_t> VecH2_t;
   typedef std::vector<Identifier> VecId_t;
  /// ---------------------------------------
  //@name Histograms related members
  //@{
  /// Vector of pointers to hitmaps histograms
  VecH1_t m_phitmapHistoVector;
  
  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  VecH2_t m_phitsHistoVector;
  VecH2_t m_phitsHistoVectorRecent;
  VecProf2_t m_pnoiseoccupancymapHistoVector;
  VecProf2_t m_pnoiseoccupancymapHistoVectorRecent;
  VecProf2_t m_pnoiseoccupancymapHistoVectorTrigger;
  VecH2_t m_phitsHistoVectorECp;
  VecH2_t m_phitsHistoVectorRecentECp;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECp;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECpRecent;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECpTrigger;
  VecH2_t m_phitsHistoVectorECm;
  VecH2_t m_phitsHistoVectorRecentECm;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECm;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECmRecent;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECmTrigger;
  
  H1D_t m_nSP;
  int *nSP_buf;
  int nSP_pos;
  H1D_t m_nHits;
  int *nHits_buf;
  int nHits_pos;
  H1D_t m_nmaxHits;
  int *nmaxHits_buf;
  Identifier *nmaxModule_buf;
  H1D_t m_nminHits;
  int *nminHits_buf;
  Identifier *nminModule_buf;

  //Histograms with hits per luminosity block
  H1_t m_numBarrelHitsPerLumiBlock;
  H1_t m_numHitsPerLumiBlockECp;
  H1_t m_numHitsPerLumiBlockECm;

  //Histograms with SPs per luminsity block
  H1_t m_numBarrelSPPerLumiBlock;
  H1_t m_numSPPerLumiBlockECp;
  H1_t m_numSPPerLumiBlockECm;


  H2_t rioMap;
  //@}
  //Histograms with NO distribution
  H1_t m_BARNO;
  H1_t m_BARNOTrigger;
  H1_t m_ECmNO;
  H1_t m_ECmNOTrigger;
  H1_t m_ECpNO;
  H1_t m_ECpNOTrigger;
  H1_t m_SCTNO;
  H1_t m_SCTNOTrigger;

  //---- results required no triggers
  // # of hits vs LBs
  Prof_t m_NallHitsBAR_vsLB;
  Prof_t m_NSPHitsBAR_vsLB;
  Prof_t m_NallHitsECm_vsLB;
  Prof_t m_NSPHitsECm_vsLB;
  Prof_t m_NallHitsECp_vsLB;
  Prof_t m_NSPHitsECp_vsLB;
  // NO with hits subtracted by SP
  Prof_t m_BARNO_vsLB;
  Prof_t m_ECmNO_vsLB;
  Prof_t m_ECpNO_vsLB;
  Prof_t m_SCTNO_vsLB;
  Prof_t m_NoisyModules100_vsLB;
  Prof_t m_NoisyModules1000_vsLB;
  Prof_t m_NoisyModules10000_vsLB;
  
  //---- results required trigger
  // # of hits vs LBs
  Prof_t m_NallHitsTriggerBAR_vsLB;
  Prof_t m_NSPHitsTriggerBAR_vsLB;
  Prof_t m_NallHitsTriggerECm_vsLB;
  Prof_t m_NSPHitsTriggerECm_vsLB;
  Prof_t m_NallHitsTriggerECp_vsLB;
  Prof_t m_NSPHitsTriggerECp_vsLB;
  // NO with hits subtracted by SP
  Prof_t m_BARNOTrigger_vsLB;
  Prof_t m_ECmNOTrigger_vsLB;
  Prof_t m_ECpNOTrigger_vsLB;
  Prof_t m_SCTNOTrigger_vsLB;
  Prof_t m_NoisyModulesTrigger100_vsLB;
  Prof_t m_NoisyModulesTrigger1000_vsLB;
  Prof_t m_NoisyModulesTrigger10000_vsLB;

  //Online code- template vector
  VecH1_t m_pstripmapTemplateVector;
  H1_t m_MaxOccupancyStripHist;
  H1_t m_MinOccupancyStripHist;
  /// Pointers to histogram of SCT cluster width.
  H1D_t m_clusize;
  H1D_t m_clusizeRecent;

   /// Pointers to histogram of hits as a function of TriggerType.
  H1_t m_hitsvstrigger;

  /// Pointers to histogram of hits as a function of L1ID.
  H1_t m_hitsvsL1ID;

  /// Name of the Track collection to use
  std::string m_tracksName;

  /// Name of the L1 Type to use for filling the extra NO histograms
  std::string m_NOTrigger;
 
  /// Pointers to vector of histograms of SCT cluster width; 1 histo per layer and side.
  VecH1D_t m_clusizeHistoVector;
  VecH1D_t m_clusizeHistoVectorECp;
  VecH1D_t m_clusizeHistoVectorECm;
  VecH1D_t m_clusizeHistoVectorRecent;
  VecH1D_t m_clusizeHistoVectorRecentECp;
  VecH1D_t m_clusizeHistoVectorRecentECm;

  /// Pointer to 1D histogram of Number of SCT Clusters per Event
  H1_t m_ncluHisto;

  /// Vector of pointers to 1D histogram of Number of SCT Clusters per Event; 1 histo per layer and side
  VecH1D_t m_ncluHistoVector;

  VecH1D_t m_ncluHistoVectorECp;
  VecH1D_t m_ncluHistoVectorECm;

  //Pointer to histogram of time bin coincidences
  H2I_t coincidenceHist; 

  /// stream stem for the ROOT histograms
  std::string m_stream;

  //Count number of events
  int m_numberOfEvents;
  //Count number of events in the selected stream
  int m_numberOfEventsTrigger;
  //Count number of events since last reset
  int m_numberOfEventsRecent;
  //CAM adds skip events counter
  int m_skipEvents;
  /// Switch on or off the hitmaps histograms
  bool m_boolhitmaps;

  /// Set bin width to increase exponentially (fixed width on a Log-X plot) 
  bool m_doLogXNoise;

  /// Switch on or off the endcaps
  bool m_doPositiveEndcap;
  bool m_doNegativeEndcap;

  ///Select the noise algorithm
  bool m_doTrackBasedNoise;
  bool m_doSpacePointBasedNoise;
  /// Add time-bin filtering to space point NO algorithm
  bool m_doTimeBinFilteringForNoise;

  /// Switch on or off the hits histos as a function of TriggerType and L1ID for TX scan
  bool m_booltxscan;

  /// CheckHists() frequency
  int m_checkrate;
  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;
  /// ChechHists() frequency
  int m_evtsbins;
  /// Number of sigma for noise checking
  int m_numSigma;

  //Things for doing time bin distributions for track hits
  bool m_doTrackHits;
  unsigned int m_maxTracks;
  std::vector<Identifier> m_RDOsOnTracks;
  StatusCode makeVectorOfTrackRDOIdentifiers();
  const DataVector<Trk::Track> *m_tracks;
  VecH2_t m_ptrackhitsHistoVector;
  VecH2_t m_ptrackhitsHistoVectorECp;
  VecH2_t m_ptrackhitsHistoVectorECm;
  VecH2_t m_ptrackhitsHistoVectorRecent;
  VecH2_t m_ptrackhitsHistoVectorRecentECp;
  VecH2_t m_ptrackhitsHistoVectorRecentECm;
  //  StatusCode bookGeneralTrackHits(bool isNewRun, const unsigned int systemIndex);
  //  StatusCode bookGeneralTrackTimeHistos(bool isNewRun , const unsigned int systemIndex);
  StatusCode bookGeneralTrackHits(const unsigned int systemIndex);
  StatusCode bookGeneralTrackTimeHistos( const unsigned int systemIndex);
  VecH1D_t m_tbinHistoVector;
  VecH1D_t m_tbinHistoVectorECp;
  VecH1D_t m_tbinHistoVectorECm;
  VecH1D_t m_tbinHistoVectorRecent;
  VecH1D_t m_tbinHistoVectorRecentECp;
  VecH1D_t m_tbinHistoVectorRecentECm;
  H1D_t m_tbinHisto;
  H1D_t m_tbinHistoECp;
  H1D_t m_tbinHistoECm;
  H1D_t m_tbinHistoRecent;
  H1D_t m_tbinHistoRecentECp;
  H1D_t m_tbinHistoRecentECm;
  Prof2_t m_tbinfrac[8];
  Prof2_t m_tbinfracECp[18];
  Prof2_t m_tbinfracECm[18];
  Prof2_t m_clusizedist[8];
  Prof2_t m_clusizedistECp[18];
  Prof2_t m_clusizedistECm[18];
  Prof_t m_tbinfracall;
  Prof_t m_tbinfracVsLB;
  Prof_t m_tbinfracVsLBECp;
  Prof_t m_tbinfracVsLBECm;
  //std::string m_tracksName;
  bool m_initialize;
 
  //@name Histograms related methods
  //@{
  
  // Book Hitmaps Histograms
  StatusCode book1DHitHists();

  StatusCode bookHitMaps();
  StatusCode bookSPvsEventNumber();
  StatusCode makeSPvsEventNumber();//int NumberOfEvents);
  // Book noise map histograms
  StatusCode bookNoiseMaps();
  StatusCode bookNoiseDistributions();

  // Book noise occupancy map histograms
  StatusCode bookGeneralNoiseOccupancyMaps( const unsigned int systemIndex);
	//Book hit occupancy map histograms
	StatusCode bookGeneralHitOccupancyMaps( const unsigned int systemIndex);
	
  // Book Hits Histograms
  StatusCode 
    bookGeneralHits( const unsigned int systemIndex);

  StatusCode bookClusterSize();
  StatusCode bookGeneralCluSize( const unsigned int systemIndex);

  // Book Correlation Histograms
  StatusCode bookCorrelations();

  //Do checking of noise maps
  StatusCode checkNoiseMaps(bool final);
  //@}
  ///additional maps for track NO to compare with SP NO calc
  std::map<Identifier, int> m_RDOs;
  //CAM adds map for SP NO
  std::string  m_SCTSPContainerName;

  std::map<Identifier, double> m_occSumUnbiased;
  std::map<Identifier, double> m_occSumUnbiasedTrigger;
  std::map<Identifier, double> m_occSumUnbiasedRecent;

  std::map<Identifier, double> m_occSumUnbiased_lb;
  std::map<Identifier, double> m_occSumUnbiasedBAR_lb;
  std::map<Identifier, double> m_occSumUnbiasedECp_lb;
  std::map<Identifier, double> m_occSumUnbiasedECm_lb;
  std::map<Identifier, double> m_occSumUnbiasedTrigger_lb;
  std::map<Identifier, double> m_occSumUnbiasedTriggerBAR_lb;
  std::map<Identifier, double> m_occSumUnbiasedTriggerECp_lb;
  std::map<Identifier, double> m_occSumUnbiasedTriggerECm_lb;
  
  static const long NBINS_LBs = 2000;
  int m_noisyM100[NBINS_LBs+1];
  int m_noisyM1000[NBINS_LBs+1];
  int m_noisyM10000[NBINS_LBs+1];
  int m_occ_lb[NBINS_LBs+1];
  int m_occBAR_lb[NBINS_LBs+1];
  int m_occECp_lb[NBINS_LBs+1];
  int m_occECm_lb[NBINS_LBs+1];
  int m_events_lb;
  int m_noisyMTrigger100[NBINS_LBs+1];
  int m_noisyMTrigger1000[NBINS_LBs+1];
  int m_noisyMTrigger10000[NBINS_LBs+1];
  int m_occTrigger_lb[NBINS_LBs+1];
  int m_occTriggerBAR_lb[NBINS_LBs+1];
  int m_occTriggerECp_lb[NBINS_LBs+1];
  int m_occTriggerECm_lb[NBINS_LBs+1];
  int m_eventsTrigger_lb;
  
	//Added 09.09.2016
	VecProf2_t m_phitoccupancymapHistoVector;
	VecProf2_t m_phitoccupancymapHistoVectorRecent;
	VecProf2_t m_phitoccupancymapHistoVectorTrigger;
	VecProf2_t m_phitoccupancymapHistoVectorECp;
	VecProf2_t m_phitoccupancymapHistoVectorECpRecent;
	VecProf2_t m_phitoccupancymapHistoVectorECpTrigger;
	VecProf2_t m_phitoccupancymapHistoVectorECm;
	VecProf2_t m_phitoccupancymapHistoVectorECmRecent;
	VecProf2_t m_phitoccupancymapHistoVectorECmTrigger;
	
	//Histograms with HO distribution
	H1_t m_BARHO;
	H1_t m_BARHOTrigger;
	H1_t m_ECmHO;
	H1_t m_ECmHOTrigger;
	H1_t m_ECpHO;
	H1_t m_ECpHOTrigger;
	H1_t m_SCTHO;
	H1_t m_SCTHOTrigger;
	
	//---- results required no triggers
	// # of hits vs LBs
	Prof_t m_HallHitsBAR_vsLB;
	Prof_t m_HSPHitsBAR_vsLB;
	Prof_t m_HallHitsECm_vsLB;
	Prof_t m_HSPHitsECm_vsLB;
	Prof_t m_HallHitsECp_vsLB;
	Prof_t m_HSPHitsECp_vsLB;
	 // HO with hits subtracted by SP
	Prof_t m_BARHO_vsLB;
	Prof_t m_ECmHO_vsLB;
	Prof_t m_ECpHO_vsLB;
	Prof_t m_SCTHO_vsLB;
	Prof_t m_NoisyModulesWithHO100_vsLB;
	Prof_t m_NoisyModulesWithHO1000_vsLB;
	Prof_t m_NoisyModulesWithHO10000_vsLB;

	//---- results required trigger
	// # of hits vs LBs
	Prof_t m_HallHitsTriggerBAR_vsLB;
	Prof_t m_HSPHitsTriggerBAR_vsLB;
	Prof_t m_HallHitsTriggerECm_vsLB;
	Prof_t m_HSPHitsTriggerECm_vsLB;
	Prof_t m_HallHitsTriggerECp_vsLB;
	Prof_t m_HSPHitsTriggerECp_vsLB;
	// HO with hits
	Prof_t m_BARHOTrigger_vsLB;
	Prof_t m_ECmHOTrigger_vsLB;
	Prof_t m_ECpHOTrigger_vsLB;
	Prof_t m_SCTHOTrigger_vsLB;
	Prof_t m_NoisyModulesWithHOTrigger100_vsLB;
	Prof_t m_NoisyModulesWithHOTrigger1000_vsLB;
	Prof_t m_NoisyModulesWithHOTrigger10000_vsLB;

	std::map<Identifier, double> m_hitoccSumUnbiased;
	std::map<Identifier, double> m_hitoccSumUnbiasedTrigger;
	std::map<Identifier, double> m_hitoccSumUnbiasedRecent;
	
	std::map<Identifier, double> m_hitoccSumUnbiased_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedBAR_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedECp_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedECm_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedTrigger_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedTriggerBAR_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedTriggerECp_lb;
	std::map<Identifier, double> m_hitoccSumUnbiasedTriggerECm_lb;

  int m_noisyMWithHO100[NBINS_LBs+1];
  int m_noisyMWithHO1000[NBINS_LBs+1];
  int m_noisyMWithHO10000[NBINS_LBs+1];
  int m_hitocc_lb[NBINS_LBs+1];
  int m_hitoccBAR_lb[NBINS_LBs+1];
  int m_hitoccECp_lb[NBINS_LBs+1];
  int m_hitoccECm_lb[NBINS_LBs+1];
  int m_noisyMWithHOTrigger100[NBINS_LBs+1];
  int m_noisyMWithHOTrigger1000[NBINS_LBs+1];
  int m_noisyMWithHOTrigger10000[NBINS_LBs+1];
  int m_hitoccTrigger_lb[NBINS_LBs+1];
  int m_hitoccTriggerBAR_lb[NBINS_LBs+1];
  int m_hitoccTriggerECp_lb[NBINS_LBs+1];
  int m_hitoccTriggerECm_lb[NBINS_LBs+1];

  StatusCode initializeNoiseMaps();

  StatusCode resetNoiseMapHists();

  StatusCode resetNoiseMapsRecent();

  StatusCode resetHitMapHists();

  StatusCode resetVecProf2(VecProf2_t hists);

  StatusCode resetVecH2(VecH2_t hists);

  StatusCode resetVecH1D(VecH1D_t hists);

  StatusCode generalHistsandNoise();
    
  //@nameService members
  //@{
  
  /// Data object name: for the SCT this is "SCT_RDOs"
  std::string m_dataObjectName;

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;

  ///Determines whether a local summary is written
  int m_localSummary;
  H1_t
    h1Factory(const std::string & name, const std::string & title, MonGroup & registry, VecH1_t & storageVector, const float lo, const float hi, const unsigned int nbins);
  H1_t
    h1Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins);
  H1D_t
    h1DFactory(const std::string & name, const std::string & title, MonGroup & registry, VecH1D_t & storageVector, const float lo, const float hi, const unsigned int nbins);
  H1D_t
    h1DFactory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins);
 
  H2_t
    h2Factory(const std::string & name, const std::string & title, const SCT_Monitoring::Bec bec, MonGroup & registry, VecH2_t & storageVector);
  H2I_t
    h2IFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbinx, double xlo, double xhi, int nbiny, double ylo, double yhi);
  Prof2_t
    prof2Factory(const std::string & name, const std::string & title, const SCT_Monitoring::Bec bec, MonGroup & registry, VecProf2_t & storageVector);
  Prof2_t
    prof2DFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbinx, int xlo, int xhi, int nbiny, int ylo, int yhi);
  Prof_t
    profFactory(const std::string & name, const std::string & title, MonGroup & registry, int nbin, int lo, int hi);
  Prof_t
    profFactory(const std::string & name, const std::string & title, MonGroup & registry);
  //@}

  //@name Service methods
  //@{
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_ConfigurationSvc;
 
  ///Format the position as a string
  std::string
  positionString(const Identifier & plane) const;
  //@}
};

#endif
