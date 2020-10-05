// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

// Forward declarations
class TProfile;
class SCT_ID;

class SCTLorentzMonTool : public ManagedMonitorToolBase {
 public:
  SCTLorentzMonTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCTLorentzMonTool() = default;
  //initialize
  virtual StatusCode initialize() override final;
  /**    @name Book, fill & check (reimplemented from baseclass) */
  //@{
  ///Book histograms in initialization
  virtual StatusCode bookHistograms() override final;
  virtual StatusCode bookHistogramsRecurrent() override final;
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() override final;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms() override final;
  ///helper function used in procHistograms
  virtual StatusCode checkHists(bool fromFinalize) override final;
  //@}

 private:
  enum SiliconSurface { surface100, surface111, nSurfaces };
  enum Sides { side0, side1, nSides };

  // Data member, which is not changed after initialization
  std::string m_path{""};

  //@name Histograms related members
  //@{

  /// Vector of pointers to profile histogram of local inc angle (phi) vs nStrips
  TProfile* m_phiVsNstrips[SCT_Monitoring::N_BARRELS][nSides][nSurfaces]{};

  //@}
  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName{this, "tracksName", "CombinedInDetTracks"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  //@name Tool members
  //@{
  ///Association tool
  ToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};
  //@}

  //@name Service members
  //@{
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};
  //@}

  //@name Flags
  //@{
  // Reject shared hits
  BooleanProperty m_rejectSharedHits{this, "RejectSharedHits", false};
  //@}

  //@name  Histograms related methods
  //@{
  // Book Histograms
  StatusCode bookLorentzHistos();
  //@}

  //@name Service methods
  //@{
  // Calculate the local angle of incidence
  int findAnglesToWaferSurface ( const float (&vec)[3], const float& sinAlpha, const Identifier& id, const InDetDD::SiDetectorElementCollection* elements, float& theta, float& phi ) const;

  ///Factory + register for the 2D profiles, returns whether successfully registered
  TProfile* pFactory(const std::string& name, const std::string& title, int nbinsx, float xlow, float xhigh, MonGroup& registry, int& iflag) const;
  //@}
};

#endif
