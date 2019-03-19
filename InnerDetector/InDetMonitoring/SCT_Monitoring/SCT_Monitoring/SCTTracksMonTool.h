/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTTracksMonTool.h
 *   Class declaration for SCTTracksMonTool
 *                         
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *  
 */

#ifndef SCTTRACKSMONTOOL_H
#define SCTTRACKSMONTOOL_H

#include "SCT_Monitoring/SCTMotherTrigMonTool.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

// Forward declarations
class IInterface;
class TH1I;
class TH1F;
class TH2F;
class TProfile2D;
class TProfile;
class StatusCode;
class SCT_ID;

///Concrete monitoring tool derived from SCTMotherTrigMonTool
class SCTTracksMonTool : public SCTMotherTrigMonTool{
 public:
  SCTTracksMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  virtual ~SCTTracksMonTool() = default;
  //initialize
  virtual StatusCode initialize() final;
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() ;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms();
  ///helper function used in procHistograms
  StatusCode checkHists(bool fromFinalize);
//@} 
  
private:
  //@name typedefs centralised to enable easy changing of types
  //@{
  typedef TProfile2D* Prof2_t;
  typedef TProfile* Prof_t;
  typedef TH1F* H1_t;
  typedef TH2F* H2_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  typedef std::vector<H1_t> VecH1_t;
  typedef std::vector<H2_t> VecH2_t;
  //@}
  TH1I* m_nTracks;
  int* m_nTracks_buf;
  int m_nTracks_pos;
  TH1I* m_trackTrigger;
  TProfile* m_trackTriggerRate;
  TH1F* m_totalBarrelResidual;
  TH1F* m_totalEndCapAResidual;
  TH1F* m_totalEndCapCResidual;
  TH1F* m_totalBarrelPull;
  TH1F* m_totalEndCapAPull;
  TH1F* m_totalEndCapCPull;

  int m_numberOfEvents;
  //@name Histograms related members
  //@{
  /// Pointer to 1D histogram of Number of SCT Clusters associated to any Track per Event
  H1_t m_trk_nclu_totHisto;

  // Pointer to 1D histogram of number of tracks in each LB
  H1_t m_tracksPerRegion;

  //Pointer to profile histogram of track rates
  Prof_t m_trackRate;

  /// Pointer to 1D histogram of Number of SCT Clusters per Track
  H1_t m_trk_ncluHisto;

  /// Pointer to 1D histogram of Track chi2
  H1_t m_trk_chi2;
  
  H1_t m_trk_N;

 /// Pointer to 1D histogram of Track chi2
  H1_t m_trk_pt;

 /// Pointer to 1D histogram of Track chi2
  H1_t m_trk_d0;

 /// Pointer to 1D histogram of Track chi2
  H1_t m_trk_z0;

  /// Pointer to 1D histogram of Track eta
  H1_t m_trk_eta;
  
  /// Pointer to 1D histogram of Track chi2
  H1_t m_trk_phi;

  /// Vector of pointers to profile histogram of residuals; 1 histo per layer and side
  VecProf2_t m_psctresidualsHistoVector;
  VecProf2_t m_psctresidualsHistoVectorECp;
  VecProf2_t m_psctresidualsHistoVectorECm;
  /// Vector of pointers to  histogram of residuals RMS; 1 histo per layer and side
  VecH2_t m_psctresidualsRMSHistoVector;
  VecH2_t m_psctresidualsRMSHistoVectorECp;
  VecH2_t m_psctresidualsRMSHistoVectorECm;
  /// Vector of pointers to summary histogram of residuals; 1 histo per layer and side
  VecH1_t m_psctresiduals_summaryHistoVector;
  VecH1_t m_psctresiduals_summaryHistoVectorECp;
  VecH1_t m_psctresiduals_summaryHistoVectorECm;
  /// Vector of pointers to profile histogram of pulls; 1 histo per layer and side
  VecProf2_t m_psctpullsHistoVector;
  VecProf2_t m_psctpullsHistoVectorECp;
  VecProf2_t m_psctpullsHistoVectorECm;
  /// Vector of pointers to  histogram of pulls RMS; 1 histo per layer and side
  VecH2_t m_psctpullsRMSHistoVector;
  VecH2_t m_psctpullsRMSHistoVectorECp;
  VecH2_t m_psctpullsRMSHistoVectorECm;
  /// Vector of pointers to summary histogram of pulls; 1 histo per layer and side
  VecH1_t m_psctpulls_summaryHistoVector;
  VecH1_t m_psctpulls_summaryHistoVectorECp;
  VecH1_t m_psctpulls_summaryHistoVectorECm;
  std::string m_stream;
  std::string m_path;
  bool m_useIDGlobal;
  //@}
  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName;
  /// Cut on number of SCT hits on track
  int m_trackHitCut;
  /// CheckHists() frequency
  int m_checkrate;
 /// Tracks vs evt
  int m_evtsbins;

  bool m_doPositiveEndcap;
  bool m_doNegativeEndcap;
  ToolHandle < Trk::IResidualPullCalculator >   m_residualPullCalculator;
  bool   m_doUnbiasedCalc;


  //@name Service members
  //@{
  /// Kalman Updator for SCT Unbiased states in Residual calculation
  PublicToolHandle<Trk::IUpdator> m_updator
     {this,"KalmanUpdator","Trk::KalmanUpdator/TrkKalmanUpdator",""};

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;
  
  //@}
  //@name  Histograms related methods
  //@{
  // Book Track related  Histograms
  StatusCode bookTrackHistos(const SCT_Monitoring::Bec becVal);
  StatusCode bookGeneralHistos();
  StatusCode bookPositiveEndCapTrackHistos(){ return bookTrackHistos(SCT_Monitoring::ENDCAP_A);}
  StatusCode bookNegativeEndCapTrackHistos(){ return bookTrackHistos(SCT_Monitoring::ENDCAP_C);}
  //@}
  
  //@name Service methods
  //@{
  /// Calculate Pull value for MeasuredAtPlane TrackStates
  float calculatePull(const float, const float, const float);
  
  ///Factory + register for the 2D histos, returns whether successfully registered
  StatusCode h2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, VecH2_t& storageVector);
  
  ///Factory + register for the 2D profiles, returns whether successfully registered
  StatusCode p2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, VecProf2_t& storageVector);
  
  ///Factory + register for the 1D histograms, returns whether successfully registered
  StatusCode h1Factory(const std::string& name, const std::string& title, const float extent, MonGroup& registry, VecH1_t& storageVector);
  //@}
};

#endif
