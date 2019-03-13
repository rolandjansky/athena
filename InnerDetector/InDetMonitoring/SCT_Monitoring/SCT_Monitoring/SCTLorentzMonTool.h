/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTLorentzMonTool.h
 *   Class declaration for SCTLorentzMonTool
 *
 *
 *
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *
 *
 *
 *
 */

#ifndef SCTLORENTZMONTOOL_H
#define SCTLORENTZMONTOOL_H

#include <string>
#include <vector>

// for Updator
#include "TrkToolInterfaces/IUpdator.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//for vertexTool
#include "ITrackToVertex/ITrackToVertex.h" //for  m_trackToVertexTool

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkTrack/TrackCollection.h"

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
class SCTLorentzMonTool : public SCTMotherTrigMonTool{
 public:
  SCTLorentzMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~SCTLorentzMonTool() = default;
  //initialize
  virtual StatusCode initialize() final;
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  //  virtual StatusCode bookHistograms(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun);
  virtual StatusCode bookHistograms();          // hidetoshi 14.01.21
  virtual StatusCode bookHistogramsRecurrent(); // hidetoshi 14.01.21
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() ;
  ///process histograms at the end (we only use 'isEndOfRun')
  //  virtual StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  virtual StatusCode procHistograms();          // hidetoshi 14.01.21
  ///helper function used in procHistograms
  StatusCode checkHists(bool fromFinalize);
//@}

private:
  //@name typedefs centralised to enable easy changing of types
  //@{
  typedef TProfile * Prof_t;
  typedef TH1F * H1_t;
  typedef TH2F * H2_t;
  typedef std::vector<Prof_t> VecProf_t;
  typedef std::vector<H1_t> VecH1_t;
  typedef std::vector<H2_t> VecH2_t;
  //@}
  int m_numberOfEvents;
  //@name Histograms related members
  //@{

  //for Vertex and perigee
  ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;

  /// Vector of pointers to profile histogram of local inc angle (phi) vs nStrips (one/layer)
  Prof_t m_phiVsNstrips[4];
  Prof_t m_phiVsNstrips_100[4];
  Prof_t m_phiVsNstrips_111[4];
  /// Vector of pointers to profile histogram of local inc angle (phi) vs nStrips (one/layer/side)
  Prof_t m_phiVsNstrips_Side[4][2];
  Prof_t m_phiVsNstrips_Side_100[4][2];
  Prof_t m_phiVsNstrips_Side_111[4][2];

  std::string m_stream;
  std::string m_path;
  //@}
  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  //@name Service members
  //@{
  /// Kalman Updator for SCT Unbiased states in Residual calculation
  //  ToolHandle<Trk::IUpdator> m_updator;

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;
  //@}
  //@name  Histograms related methods
  //@{
  // Book Histograms
  //  StatusCode bookLorentzHistos(bool isNewRun,bool isNewLumiBlock);
  StatusCode bookLorentzHistos();                                           // hidetoshi 14.01.22
  //@}

  //@name Service methods
  //@{
  // Calculate the local angle of incidence
  int findAnglesToWaferSurface ( const float (&vec)[3], const float &sinAlpha, const Identifier &id, const InDetDD::SiDetectorElementCollection* elements, float &theta, float &phi );

  ///Factory + register for the 2D profiles, returns whether successfully registered
  Prof_t  pFactory(const std::string & name, const std::string & title, int nbinsx, float xlow, float xhigh, MonGroup & registry, int& iflag);
  ///Factory + register for the 1D histograms, returns whether successfully registered
  bool h1Factory(const std::string & name, const std::string & title, const float extent, MonGroup & registry, VecH1_t & storageVector);
  //@}
};

#endif
