/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Helper functions used by other methods
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <sstream>
#include <vector>
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/Components.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TProfile2D.h"
#include "TString.h"

std::string PixelMainMon::makeHistname(std::string set, bool ontrk) {
  std::string name = set;
  if (ontrk && m_doOnTrack) name += "_OnTrack";
  return name;
}

std::string PixelMainMon::makeHisttitle(std::string set, std::string axis, bool ontrk) {
  std::string name = set;
  if (ontrk && m_doOnTrack) name += " OnTrack";
  name = name + m_histTitleExt + axis;
  return name;
}

int PixelMainMon::getPixLayerID(int ec, int ld, bool ibl) {
  int layer = 99;
  if (ec == 2) {
    layer = PixLayer::kECA;
  } else if (ec == -2) {
    layer = PixLayer::kECC;
  } else if (ec == 0) {
    if (ibl && ld == 0) layer = PixLayer::kIBL;
    if (ld == 0 + ibl) layer = PixLayer::kB0;
    if (ld == 1 + ibl) layer = PixLayer::kB1;
    if (ld == 2 + ibl) layer = PixLayer::kB2;
  }
  return layer;
}

int PixelMainMon::getPixLayerIDWithDBM(int id) {
  // Translate PixLayerID to PixLayerIDDBM
  if (id >= PixLayer::kIBL) return id + 2;
  return id;
}

int PixelMainMon::getPixLayerIDDBM(int ec, int ld, bool ibl) {
  int layer = 99;
  if (ec == 2) {
    layer = PixLayerDBM::kECA;
  } else if (ec == -2) {
    layer = PixLayerDBM::kECC;
  } else if (ec == 0) {
    if (ibl && ld == 0) layer = PixLayerDBM::kIBL;
    if (ld == 0 + ibl) layer = PixLayerDBM::kB0;
    if (ld == 1 + ibl) layer = PixLayerDBM::kB1;
    if (ld == 2 + ibl) layer = PixLayerDBM::kB2;
  } else {
    if (ec == 4) layer = PixLayerDBM::kDBMA;
    if (ec == -4) layer = PixLayerDBM::kDBMC;
  }
  return layer;
}

int PixelMainMon::getPixLayerIDIBL2D3D(int ec, int ld, int eta, bool ibl) {
  int layer = 99;
  if (ec == 0) {
    if (ibl && ld == 0 && eta < 6 && eta > -7) layer = PixLayerIBL2D3D::kIBL2D;
    if (ibl && ld == 0 && !(eta < 6 && eta > -7)) layer = PixLayerIBL2D3D::kIBL3D;
  }
  return layer;
}

int PixelMainMon::getPixLayerIDIBL2D3DDBM(int ec, int ld, int eta, bool ibl) {
  int layer = 99;
  if (ec == 0) {
    if (ibl && ld == 0 && eta < 6 && eta > -7) layer = PixLayerIBL2D3DDBM::kIBL2D;
    if (ibl && ld == 0 && !(eta < 6 && eta > -7)) layer = PixLayerIBL2D3DDBM::kIBL3D;
  }
  return layer;
}

int PixelMainMon::getPixLayerDiskID(int ec, int ld, bool ibl) {
  int layer = 99;
  if (ec == 2) {
    layer = PixLayerDisk::kECA0 + ld;
  } else if (ec == -2) {
    layer = PixLayerDisk::kECC0 + ld;
  } else if (ec == 0) {
    if (ibl && ld == 0) layer = PixLayerDisk::kIBL;
    if (ld == 0 + ibl) layer = PixLayerDisk::kB0;
    if (ld == 1 + ibl) layer = PixLayerDisk::kB1;
    if (ld == 2 + ibl) layer = PixLayerDisk::kB2;
  }
  return layer;
}

void PixelMainMon::th1FillMonitoring(TH1F_LW* mon, TH1F_LW* tmp) {
  for (unsigned int i = 1; i <= tmp->GetNbinsX(); i++) {
    float content = tmp->GetBinContent(i);
    mon->SetBinContent(i, content);
  }
  tmp->Reset();
}

void PixelMainMon::fillSummaryHistos(PixelMon2DMapsLW* occupancy, TH1F_LW* A, TH1F_LW* C, TH1F_LW* IBL, TH1F_LW* B0, TH1F_LW* B1, TH1F_LW* B2) {
  // if the histos don't exist, dont' fill them
  if (!(A && C && B0 && B1 && B2 && occupancy)) return;
  double events = m_event;
  if (events == 0) return;

  if (IBL) {
    IBL->Reset();
  }
  B0->Reset();
  B1->Reset();
  B2->Reset();
  A->Reset();
  C->Reset();

  if (IBL) {
    for (int etaIndex = 1; etaIndex <= 12; etaIndex++) {
      for (int phiIndex = 1; phiIndex <= 14; phiIndex++) {
        IBL->Fill(occupancy->IBL2D->GetBinContent(etaIndex, phiIndex) / events);
      }
    }
    for (int etaIndex = 1; etaIndex <= 8; etaIndex++) {
      for (int phiIndex = 1; phiIndex <= 14; phiIndex++) {
        IBL->Fill(occupancy->IBL3D->GetBinContent(etaIndex, phiIndex) / events);
      }
    }
  }
  for (int etaIndex = 1; etaIndex <= 13; etaIndex++) {
    for (int phiIndex = 1; phiIndex <= 22; phiIndex++) {
      B0->Fill(occupancy->B0->GetBinContent(etaIndex, phiIndex) / events);
    }
    for (int phiIndex = 1; phiIndex <= 38; phiIndex++) {
      B1->Fill(occupancy->B1->GetBinContent(etaIndex, phiIndex) / events);
    }
    for (int phiIndex = 1; phiIndex <= 52; phiIndex++) {
      B2->Fill(occupancy->B2->GetBinContent(etaIndex, phiIndex) / events);
    }
  }
  for (int etaIndex = 1; etaIndex <= 3; etaIndex++) {
    for (int phiIndex = 1; phiIndex <= 48; phiIndex++) {
      A->Fill(occupancy->A->GetBinContent(etaIndex, phiIndex) / events);
      C->Fill(occupancy->C->GetBinContent(etaIndex, phiIndex) / events);
    }
  }
}

void PixelMainMon::formatPP0Histos(TProfile_LW* D_A, TProfile_LW* D_C, TProfile_LW* B0, TProfile_LW* B1, TProfile_LW* B2, TProfile_LW* IBL_A, TProfile_LW* IBL_C) {
  if (!(D_A && D_C && B0 && B1 && B2)) return;
  if (IBL_A && IBL_C) {
    for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
      IBL_A->GetXaxis()->SetBinLabel(i + 1, PixMon::StavesIBL.at(i).c_str());
      IBL_A->GetXaxis()->SetLabelSize(0.03);
      IBL_A->SetMinimum(0.);
      IBL_C->GetXaxis()->SetBinLabel(i + 1, PixMon::StavesIBL.at(i).c_str());
      IBL_C->GetXaxis()->SetLabelSize(0.03);
      IBL_C->SetMinimum(0.);
    }
  }
  if (B0 && B1 && B2) {
    for (unsigned int i = 0; i < PixMon::kNumStavesL0; ++i) {
      B0->GetXaxis()->SetBinLabel(i + 1, PixMon::StavesL0.at(i).c_str());
      B0->GetXaxis()->SetLabelSize(0.03);
      B0->SetMinimum(0.);
    }
    for (unsigned int i = 0; i < PixMon::kNumStavesL1; ++i) {
      B1->GetXaxis()->SetBinLabel(i + 1, PixMon::StavesL1.at(i).c_str());
      B1->GetXaxis()->SetLabelSize(0.02);
      B1->SetMinimum(0.);
    }
    for (unsigned int i = 0; i < PixMon::kNumStavesL2; ++i) {
      B2->GetXaxis()->SetBinLabel(i + 1, PixMon::StavesL2.at(i).c_str());
      B2->GetXaxis()->SetLabelSize(0.02);
      B2->SetMinimum(0.);
    }
  }
  if (D_A && D_C) {
    for (unsigned int i = 0; i < PixMon::kNumPP0sEC; ++i) {
      D_A->GetXaxis()->SetBinLabel(i + 1, PixMon::PP0sEC.at(i).c_str());
      D_A->GetXaxis()->SetLabelSize(0.02);
      D_A->SetMinimum(0.);
      D_C->GetXaxis()->SetBinLabel(i + 1, PixMon::PP0sEC.at(i).c_str());
      D_C->GetXaxis()->SetLabelSize(0.02);
      D_C->SetMinimum(0.);
    }
  }
}

void PixelMainMon::fillPP0Histos(PixelMon2DMapsLW* occupancy, TProfile_LW* D_A, TProfile_LW* D_C, TProfile_LW* B0, TProfile_LW* B1, TProfile_LW* B2, TProfile_LW* IBL_A, TProfile_LW* IBL_C) {
  if (!(D_A && D_C && B0 && B1 && B2 && occupancy && m_status && m_event5min)) return;
  unsigned int nbinx;
  unsigned int nbiny;
  // use FE occ for IBL, mod occ for the rest
  // fill only if FE is active
  if (IBL_A && IBL_C) {              
    nbinx = occupancy->IBL->GetNbinsX();
    nbiny = occupancy->IBL->GetNbinsY();
    for (unsigned int x = 1; x <= nbinx; ++x) {
      for (unsigned int y = 1; y <= nbiny; ++y) {
        const auto content = occupancy->IBL->GetBinContent(x, y) / (1.0 * m_event5min);
	if (m_status->IBL->GetBinContent(x, y)!=2) {
	  if (x>0.5*nbinx) IBL_A->Fill(y, content);
	  else IBL_C->Fill(y, content);
	}
      }
    }
  }
  nbinx = occupancy->B0->GetNbinsX();
  nbiny = occupancy->B0->GetNbinsY();
  for (unsigned int x = 1; x <= nbinx; ++x) {
    for (unsigned int y = 1; y <= nbiny; ++y) {
      const auto content = occupancy->B0->GetBinContent(x, y) / (1.0 * m_event5min);
      if (m_status->B0->GetBinContent(x, y)!=2) {
	B0->Fill(y, content);
      }
    }
  }
  nbinx = occupancy->B1->GetNbinsX();
  nbiny = occupancy->B1->GetNbinsY();
  for (unsigned int x = 1; x <= nbinx; ++x) {
    for (unsigned int y = 1; y <= nbiny; ++y) {
      const auto content = occupancy->B1->GetBinContent(x, y) / (1.0 * m_event5min);
      if (m_status->B1->GetBinContent(x, y)!=2) {
	B1->Fill(y, content);
      }
    }
  }
  nbinx = occupancy->B2->GetNbinsX();
  nbiny = occupancy->B2->GetNbinsY();
  for (unsigned int x = 1; x <= nbinx; ++x) {
    for (unsigned int y = 1; y <= nbiny; ++y) {
      const auto content = occupancy->B2->GetBinContent(x, y) / (1.0 * m_event5min);
      if (m_status->B2->GetBinContent(x, y)!=2) {
	B2->Fill(y, content);
      }
    }
  }
  nbinx = occupancy->A->GetNbinsX();
  nbiny = occupancy->A->GetNbinsY();
  for (unsigned int x = 1; x <= nbinx; ++x) {
    for (unsigned int y = 1; y <= nbiny; ++y) {
      const auto content = occupancy->A->GetBinContent(x, y) / (1.0 * m_event5min);
      if (m_status->A->GetBinContent(x, y)!=2) {
	D_A->Fill( (x-1)*8 + (y-1)/6 + 1, content);
      }
    }
  }
  nbinx = occupancy->C->GetNbinsX();
  nbiny = occupancy->C->GetNbinsY();
  for (unsigned int x = 1; x <= nbinx; ++x) {
    for (unsigned int y = 1; y <= nbiny; ++y) {
      const auto content = occupancy->C->GetBinContent(x, y) / (1.0 * m_event5min);
      if (m_status->C->GetBinContent(x, y)!=2) {
	D_C->Fill( (x-1)*8 + (y-1)/6 + 1, content);
      }
    }
  }
}

bool PixelMainMon::isOnTrack(Identifier id, bool isCluster) {
  bool onTrack = false;
  if (isCluster) {
    std::pair<Identifier, double> searchVal = std::make_pair(id, -1.0);
    onTrack = std::binary_search(m_ClusterIDs.begin(), m_ClusterIDs.end(), searchVal,
                                 [](std::pair<Identifier, double> l, std::pair<Identifier, double> r) -> bool { return l.first < r.first; });
  } else {
    onTrack = binary_search(m_RDOIDs.begin(), m_RDOIDs.end(), id);
  }
  return onTrack;
}

bool PixelMainMon::isOnTrack(Identifier id, double& cosalpha) {
  bool onTrack(false);
  std::pair<Identifier, double> searchVal = std::make_pair(id, -1.0);
  auto it = std::lower_bound(m_ClusterIDs.begin(), m_ClusterIDs.end(), searchVal,
                             [](std::pair<Identifier, double> l, std::pair<Identifier, double> r) -> bool { return l.first < r.first; });

  if (it != m_ClusterIDs.end() && !(id < (*it).first)) {
    onTrack = true;
    cosalpha = (*it).second;
  }
  return onTrack;
}

// Not yet updated to include IBL:
int PixelMainMon::parseDetailsString(std::string& detailsMod) {
  int modID[4] = {0, 0, 0, 0};
  char* pch;
  pch = new char[detailsMod.size() + 1];
  strncpy(pch, detailsMod.c_str(), detailsMod.size());
  const int nmod = 13;
  const char* mod[nmod] = {"M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A"};
  const int nstave0 = 22;
  const char* stave0[nstave0] = {"B11_S2", "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1",
                                 "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1",
                                 "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1"};
  const int nstave1 = 38;
  const char* stave1[nstave1] = {"B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1", "B04_S2",
                                 "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1", "B08_S2",
                                 "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
                                 "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2", "B16_S1", "B16_S2",
                                 "B17_S1", "B17_S2", "B18_S1", "B18_S2", "B19_S1", "B19_S2"};
  const int nstave2 = 52;
  const char* stave2[nstave2] = {"B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1", "B04_S2", "B05_S1",
                                 "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1",
                                 "B09_S2", "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2", "B13_S1",
                                 "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2", "B16_S1", "B16_S2", "B17_S1",
                                 "B17_S2", "B18_S1", "B18_S2", "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1",
                                 "B21_S2", "B22_S1", "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2", "B25_S1",
                                 "B25_S2", "B26_S1", "B26_S2", "B01_S1"};
  const char* nstaveA[48] = {"B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3", "B01_S2_M4", "B02_S1_M1",
                             "B02_S1_M6", "B02_S1_M2", "B02_S1_M5", "B02_S1_M3", "B02_S1_M4", "B02_S2_M1", "B02_S2_M6",
                             "B02_S2_M2", "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", "B03_S1_M1", "B03_S1_M6", "B03_S1_M2",
                             "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", "B03_S2_M1", "B03_S2_M6", "B03_S2_M2", "B03_S2_M5",
                             "B03_S2_M3", "B03_S2_M4", "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3",
                             "B04_S1_M4", "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5", "B04_S2_M3", "B04_S2_M4",
                             "B01_S1_M1", "B01_S1_M6", "B01_S1_M2", "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"};
  const char* nstaveC[48] = {"B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6", "B01_S2_M1", "B02_S1_M4",
                             "B02_S1_M3", "B02_S1_M5", "B02_S1_M2", "B02_S1_M6", "B02_S1_M1", "B02_S2_M4", "B02_S2_M3",
                             "B02_S2_M5", "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", "B03_S1_M4", "B03_S1_M3", "B03_S1_M5",
                             "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", "B03_S2_M4", "B03_S2_M3", "B03_S2_M5", "B03_S2_M2",
                             "B03_S2_M6", "B03_S2_M1", "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6",
                             "B04_S1_M1", "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2", "B04_S2_M6", "B04_S2_M1",
                             "B01_S1_M4", "B01_S1_M3", "B01_S1_M5", "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"};

  // parse string and get module name
  if (strstr(pch, "D1C")) {
    modID[0] = -2;
    modID[1] = 0;
  } else if (strstr(pch, "D2C")) {
    modID[0] = -2;
    modID[1] = 1;
  } else if (strstr(pch, "D3C")) {
    modID[0] = -2;
    modID[1] = 2;
  } else if (strstr(pch, "D1A")) {
    modID[0] = 2;
    modID[1] = 0;
  } else if (strstr(pch, "D2A")) {
    modID[0] = 2;
    modID[1] = 1;
  } else if (strstr(pch, "D3A")) {
    modID[0] = 2;
    modID[1] = 2;
  }

  if (strstr(pch, "L0")) {
    modID[0] = 0;
    modID[1] = 0;
    for (int i = 0; i < nstave0; i++) {
      if (strstr(pch, stave0[i])) {
        modID[2] = i;
        break;
      }
    }
  } else if (strstr(pch, "L1")) {
    modID[0] = 0;
    modID[1] = 1;
    for (int i = 0; i < nstave1; i++) {
      if (strstr(pch, stave1[i])) {
        modID[2] = i;
        break;
      }
    }
  } else if (strstr(pch, "L2")) {
    modID[0] = 0;
    modID[1] = 2;
    for (int i = 0; i < nstave2; i++) {
      if (strstr(pch, stave2[i])) {
        modID[2] = i;
        break;
      }
    }
  }

  if (modID[0] == 2 || modID[0] == -2) {
    for (int i = 0; i < 48; i++) {
      if (modID[0] == -2 && strstr(pch, nstaveC[i])) {
        modID[2] = i;
        break;
      }
      if (modID[0] == 2 && strstr(pch, nstaveA[i])) {
        modID[2] = i;
        break;
      }
    }
  }
  if (modID[0] == 0) {
    for (int i = 0; i < nmod; i++) {
      if (strstr(pch, mod[i])) {
        modID[3] = i;
        break;
      }
    }
  }

  delete[] pch;
  return (1000000 + (modID[0] + 2) * 100000 + (modID[1]) * 10000 + (modID[2]) * 100 + (modID[3] + 6));
}

bool PixelMainMon::getFEID(int pixlayer, int phiid, int etaid, int& oufephi, int& outfeeta) {
  bool isValid = true;
  int npixPerFe_phi, npixPerFe_eta;
  if (pixlayer == PixLayer::kECA || pixlayer == PixLayer::kECC) {
    npixPerFe_phi = 160 + 4;
    npixPerFe_eta = 18;
  } else if (pixlayer != PixLayer::kIBL) {
    npixPerFe_phi = 160 + 4;
    npixPerFe_eta = 18;
  } else {
    npixPerFe_phi = 336;
    npixPerFe_eta = 80;
    isValid = false;
  }
  oufephi = phiid / npixPerFe_phi;
  outfeeta = etaid / npixPerFe_eta;
  return isValid;
}

void PixelMainMon::divide_TH1F_LW(TH1F_LW* num, TH1F_LW* den) {
  if (!num || !den || (num->GetXaxis()->GetNbins() != den->GetXaxis()->GetNbins())) return;
  unsigned int binidx;
  double numVal, numErr;
  double denVal, denErr;
  const unsigned int entries = num->GetEntries();
  num->resetActiveBinLoop();
  while (num->getNextActiveBin(binidx, numVal, numErr)) {
    den->GetBinContentAndError(binidx, denVal, denErr);
    if (denVal == 0)
      num->SetBinContentAndError(binidx, 0, 0);
    else
      num->SetBinContentAndError(binidx, numVal / denVal, sqrt(pow(numErr*denVal,2)+pow(denErr*numVal,2)) / pow(denVal,2));
  }
  num->SetEntries(entries);
}
