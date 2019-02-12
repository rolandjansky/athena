/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill offline histograms showing status of modules
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TProfile2D.h"

#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookStatusMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Status" << endmsg;

  std::string path = "Pixel/Status";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/StatusOnTrack");
  MonGroup statusHistos(this, path.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  StatusCode sc;

  m_status = std::make_unique<PixelMon2DProfilesLW>("Map_Of_Modules_Status", ("Modules Status (0=Active+Good, 1=Active+Bad, 2=Inactive)" + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D, true);
  sc = m_status->regHist(statusHistos);
  m_status->setMaxValue(2.0);

  m_status_mon = std::make_unique<PixelMon2DProfilesLW>("Map_Of_Modules_Status_Mon", ("Modules Status (0=Active+Good, 1=Active+Bad, 2=Inactive) for monitoring" + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D, true);
  sc = m_status_mon->regHist(statusHistos);
  m_status_mon->setMaxValue(2.0);

  if (!m_doOnTrack) {   // skip the rest
    if (m_doOffline) {
      m_dqStatus = std::make_unique<PixelMon2DMapsLW>("Ok_modules", ("module problems, empty bin means dead module not listed in status database" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
      sc = m_dqStatus->regHist(statusHistos);
    }
    
    std::string tmp;
    std::string tmp2;
    
    int nbins_LB = m_lbRange;
    double min_LB = -0.5;
    double max_LB = min_LB + (1.0 * nbins_LB);
    std::string atext_LB = ";lumi block";
    std::string atext_nmod = ";# modules/event";
    const std::string modlabel[PixLayerIBL2D3D::COUNT] = {"ECA", "ECC", "B0", "B1", "B2", "IBL", "IBL2D", "IBL3D"};
    
    for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
      tmp = makeHistname(("BadModules_per_lumi_" + modlabel[i]), false);
      tmp2 = makeHisttitle(("Number of bad modules (bad+active) per event per LB, " + modlabel[i]), (atext_LB + atext_nmod), false);
      sc = statusHistos.regHist(m_badModules_per_lumi_mod[i] = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atext_LB + atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
      
      tmp = makeHistname(("DisabledModules_per_lumi_" + modlabel[i]), false);
      tmp2 = makeHisttitle(("Number of disabled modules per event per LB, " + modlabel[i]), (atext_LB + atext_nmod), false);
      sc = statusHistos.regHist(m_disabledModules_per_lumi_mod[i] = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atext_LB + atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
      
      tmp = makeHistname(("BadDisabledModules_per_lumi_" + modlabel[i]), false);
      tmp2 = makeHisttitle(("Number of disabled & bad modules per event per LB, " + modlabel[i]), (atext_LB + atext_nmod), false);
      sc = statusHistos.regHist(m_baddisabledModules_per_lumi_mod[i] = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atext_LB + atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
    }
    
    tmp = makeHistname("DisabledModules_per_lumi_PIX", false);
    tmp2 = makeHisttitle("Number of disabled modules per event per LB for Pixel barrel", (atext_LB + atext_nmod), false);
    sc = statusHistos.regHist(m_disabledModules_per_lumi_PIX = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atext_LB + atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
  }

  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "histograms not booked" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::bookStatusLumiBlockMon(void) {
  if (m_doOnTrack) return StatusCode::SUCCESS;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Status for lowStat" << endmsg;

  std::string path = "Pixel/LumiBlock";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
  MonGroup lumiBlockHist(this, path.c_str(), lowStat, ATTRIB_MANAGED);

  m_status_LB = std::make_unique<PixelMon2DProfilesLW>("Map_Of_Modules_Status_LB", ("Module Status (0=Active+Good, 1=Active+Bad, 2=Inactive)" + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D, true);
  StatusCode sc = m_status_LB->regHist(lumiBlockHist);
  m_status_LB->setMaxValue(2.0);

  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "histograms not booked" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillStatusMon(void) {
  int Index = -1;
  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

  int nBad = 0;
  int nBad_mod[PixLayerIBL2D3D::COUNT] = {0};

  int nDisabled = 0;
  int nDisabled_mod[PixLayerIBL2D3D::COUNT] = {0};

  for (; idIt != idItEnd; ++idIt) {
    Identifier WaferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID);
    int pixlayer = getPixLayerID(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
    int pixlayeribl2d3d = 0;
    if (pixlayer == PixLayer::kIBL) {
      pixlayeribl2d3d = getPixLayerIDIBL2D3D(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_pixelid->eta_module(WaferID), m_doIBL);
    }
    if (pixlayer == 99) continue;

    // check in order of occurrence to reduce number of calls to conditions service
    if (m_pixelCondSummaryTool->isActive(id_hash) == true && m_pixelCondSummaryTool->isGood(id_hash) == true) {
      Index = 0;
    } else if (m_pixelCondSummaryTool->isActive(id_hash) == false) {
      Index = 2;
    } else {
      Index = 1;
    }

    if (m_status) m_status->fill(WaferID, m_pixelid, Index);
    if (m_status_mon) m_status_mon->fill(WaferID, m_pixelid, Index);
    if (m_doOnTrack) continue;

    if (m_doLumiBlock) {
      if (m_status_LB) m_status_LB->fill(WaferID, m_pixelid, Index);
    }

    if (Index > 0) {  // bad but active modules
      if (Index == 1) {
        nBad++;
        nBad_mod[pixlayer]++;
        if (pixlayeribl2d3d != 0) nBad_mod[pixlayeribl2d3d]++;
      }
      // inactive or bad modules
      if (Index == 2) {
        nDisabled++;
        nDisabled_mod[pixlayer]++;
        if (pixlayeribl2d3d != 0) nDisabled_mod[pixlayeribl2d3d]++;
      }
    }
  }  // end of pixelid wafer loop

 if (m_doRefresh && m_status_mon) m_status_mon->reset();

 if (!m_doOnTrack) {
   static float nmod_per_layer[PixLayerIBL2D3D::COUNT] = {144., 144., 286., 494., 676., 280., 168., 112.};
   
   for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
     if (m_badModules_per_lumi_mod[i] && nmod_per_layer[i] > 0) m_badModules_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nBad_mod[i] / nmod_per_layer[i]);
     if (m_disabledModules_per_lumi_mod[i]) m_disabledModules_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nDisabled_mod[i]);
     if (m_baddisabledModules_per_lumi_mod[i]) m_baddisabledModules_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nDisabled_mod[i] + nBad_mod[i]);
   }
   if (m_disabledModules_per_lumi_PIX) m_disabledModules_per_lumi_PIX->Fill(m_manager->lumiBlockNumber(), nDisabled_mod[PixLayerIBL2D3D::kB0] + nDisabled_mod[PixLayerIBL2D3D::kB1] + nDisabled_mod[PixLayerIBL2D3D::kB2]);
   
   if (nDisabled > (1744 + 280 * m_doIBL) * 0.50) {
     m_majorityDisabled = true;
   } else {
     m_majorityDisabled = false;
   }
 }
 return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procStatusMon(void) {
  if (!m_doOnTrack && m_status && m_dqStatus && m_occupancy) {
    if (m_doIBL) {
      for (int i = 1; i <= 12; i++) {
        for (int j = 1; j <= 14; j++) {
          m_dqStatus->IBL2D->SetBinContent(i, j, m_occupancy->IBL2D->GetBinContent(i, j) + m_status->IBL2D->GetBinContent(i, j));
        }
      }
      for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 14; j++) {
          m_dqStatus->IBL3D->SetBinContent(i, j, m_occupancy->IBL3D->GetBinContent(i, j) + m_status->IBL3D->GetBinContent(i, j));
        }
      }
    }
    for (int i = 1; i <= 13; i++) {
      for (int j = 1; j <= 22; j++) {
        m_dqStatus->B0->SetBinContent(i, j, m_occupancy->B0->GetBinContent(i, j) + m_status->B0->GetBinContent(i, j));
      }
      for (int j = 1; j <= 38; j++) {
        m_dqStatus->B1->SetBinContent(i, j, m_occupancy->B1->GetBinContent(i, j) + m_status->B1->GetBinContent(i, j));
      }
      for (int j = 1; j <= 52; j++) {
        m_dqStatus->B2->SetBinContent(i, j, m_occupancy->B2->GetBinContent(i, j) + m_status->B2->GetBinContent(i, j));
      }
    }
    for (int i = 1; i <= 48; i++) {
      for (int j = 1; j <= 3; j++) {
        m_dqStatus->A->SetBinContent(j, i, m_occupancy->A->GetBinContent(j, i) + m_status->A->GetBinContent(j, i));
        m_dqStatus->C->SetBinContent(j, i, m_occupancy->C->GetBinContent(j, i) + m_status->C->GetBinContent(j, i));
      }
    }
  }
  return StatusCode::SUCCESS;
}
