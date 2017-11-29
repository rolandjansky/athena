/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for pixel spacepoints
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "PixelMonitoring/PixelMonModules.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookSpacePointMon(void) {
  ATH_MSG_DEBUG("Start booking SpacePoint histogtams..");
  std::string path = "Pixel/SpacePoint";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/SpacePointOnTrack");

  bool st(true);
  MonGroup spacePointHistos(this, path.c_str(), run, ATTRIB_MANAGED);
  if (m_doHighOccupancy) {
    st &= spacePointHistos.regHist(m_num_spacepoints = TH1I_LW::create("num_spacepoints", ("number of pixel spacepoint hits per event" + m_histTitleExt + ";# spacepoints;# events").c_str(), 100, 0., 25000)).isSuccess();
  }
  if (m_doLowOccupancy) {
    st &= spacePointHistos.regHist(m_num_spacepoints_low = TH1I_LW::create("num_spacepoints_low_occupancy", ("number of pixel spacepoint hits per event" + m_histTitleExt + ";# spacepoints;# events").c_str(), 100, -0.5, 99.5)).isSuccess();
  }
  st &= spacePointHistos.regHist(m_spHit_x = TH1F_LW::create("pixel_sp_x", ("x location of hit" + m_histTitleExt + ";x (mm);# hits").c_str(), 320, -160., 160.)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_y = TH1F_LW::create("pixel_sp_y", ("y location of hit" + m_histTitleExt + ";y (mm);# hits").c_str(), 320, -160., 160.)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_z = TH1F_LW::create("pixel_sp_z", ("z location of hit" + m_histTitleExt + ";z (mm);# hits").c_str(), 660, -660., 660.)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_r = TH1F_LW::create("pixel_sp_r", ("r location of hit" + m_histTitleExt + ";r (mm);# hits").c_str(), 230, -0., 230.)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_phi = TH1F_LW::create("pixel_sp_phi", ("phi location of hit" + m_histTitleExt + ";phi (mm);# hits").c_str(), 60, -4., 4.)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_xy = TH2F_LW::create("pixel_sp_x_vs_y", ("xy location of barrel hits" + m_histTitleExt + ";x (mm);y (mm)").c_str(), 100, -160, 160, 100, -160, 160)).isSuccess();
  st &= spacePointHistos.regHist(m_spHit_rz = TH2F_LW::create("pixel_sp_r_vs_z", ("rz location of hit" + m_histTitleExt + ";z (mm);r (mm)").c_str(), 200, -665, 665, 200, -0, 165)).isSuccess();

  if (!st) ATH_MSG_WARNING("Problems with booking SpacePoint histograms");
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillSpacePointMon(void) {
  StatusCode sc = evtStore()->retrieve(m_Pixel_spcontainer, m_Pixel_SpacePointsName);
  if (sc.isFailure() || !m_Pixel_spcontainer) {
    ATH_MSG_WARNING("SpacePoint container for Pixels not found");
    if (m_storegate_errors) m_storegate_errors->Fill(2., 3.);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Si SpacePoint container for Pixels found");
  }

  DataVector<SpacePoint>::const_iterator p_sp;
  Identifier PixelModuleId;
  int nhits = 0;

  //loop over Pixel space points collections
  for (SpacePointContainer::const_iterator it = m_Pixel_spcontainer->begin(); it != m_Pixel_spcontainer->end(); ++it) {
    const SpacePointCollection* colNext = &(**it);
    if (!colNext) {
      if (m_storegate_errors) m_storegate_errors->Fill(2., 5.);  // first entry is for SP, second is for data problem
      continue;
    }
    for (p_sp = colNext->begin(); p_sp != colNext->end(); ++p_sp) {
      const SpacePoint& sp = **p_sp;
      PixelModuleId = sp.clusterList().first->identify();

      if (m_doOnTrack && !isOnTrack(PixelModuleId, true)) {
        // if we only want hits on track, and the hit is NOT on the track, skip filling
        continue;
      }
      Amg::Vector3D point = sp.globalPosition();

      if (m_spHit_x) m_spHit_x->Fill(point.x());
      if (m_spHit_y) m_spHit_y->Fill(point.y());
      if (m_spHit_z) m_spHit_z->Fill(point.z());
      if (m_spHit_r) m_spHit_r->Fill(sqrt(point.y() * point.y() + point.x() * point.x()));  // spacepoint r.
      if (m_spHit_phi) m_spHit_phi->Fill(point.phi());                                      // spacepoint phi.
      if (m_spHit_xy && fabs(point.z()) < 400) m_spHit_xy->Fill(point.x(), point.y());
      if (m_spHit_rz) m_spHit_rz->Fill(point.z(), sqrt(point.y() * point.y() + point.x() * point.x()));
      nhits++;
    }
  }
  if (m_num_spacepoints) m_num_spacepoints->Fill(nhits);
  if (m_num_spacepoints_low) m_num_spacepoints_low->Fill(nhits);
  if (nhits == 0 && m_storegate_errors) m_storegate_errors->Fill(2., 4.);  // first entry for sp, second for size = 0
  return StatusCode::SUCCESS;
}
