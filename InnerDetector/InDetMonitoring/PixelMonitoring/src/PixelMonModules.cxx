/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 1D module histograms
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMonModules.h"
#include <string.h>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/StatusCode.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TProfile_LW.h"

PixelMonModulesProf::PixelMonModulesProf(std::string name, std::string title, int nbins, double* arr) {
  m_nBins = nbins;
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i) = TProfile_LW::create((getHistName(i, false) + "_" + name).c_str(),
                                     (getHistName(i, false) + " " + title).c_str(), nbins, arr);
  }
  if (m_doIBL == false) {
    for (int i = 1744; i < 2024; i++) {
      getHist(i) = nullptr;
    }
  }
  formatHist("");
  m_Dummy = 0;
}

PixelMonModulesProf::PixelMonModulesProf(std::string name, std::string title, int nbins, double low, double high) {
  m_nBins = nbins;
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i) = TProfile_LW::create((getHistName(i, false) + "_" + name).c_str(),
                                     (getHistName(i, false) + " " + title).c_str(), nbins, low, high);
  }
  if (m_doIBL == false) {
    for (int i = 1744; i < 2024; i++) {
      getHist(i) = nullptr;
    }
  }
  formatHist("");
  m_Dummy = 0;
}

PixelMonModules1D::PixelMonModules1D(std::string name, std::string title, int nbins, double* arr) {
  m_nBins = nbins;
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i) = new TH1F((getHistName(i, false) + "_" + name).c_str(),
                          (getHistName(i, false) + " " + title).c_str(), nbins, arr);
  }
  if (m_doIBL == false) {
    for (int i = 1744; i < 2024; i++) {
      getHist(i) = nullptr;
    }
  }
  formatHist("");
  m_Dummy = 0;
}

PixelMonModules1D::PixelMonModules1D(std::string name, std::string title, int nbins, double low, double high) {
  m_nBins = nbins;
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i) = new TH1F((getHistName(i, false) + "_" + name).c_str(),
                          (getHistName(i, false) + " " + title).c_str(), nbins, low, high);
  }
  if (m_doIBL == false) {
    for (int i = 1744; i < 2024; i++) {
      getHist(i) = nullptr;
    }
  }
  formatHist("");
  m_Dummy = 0;
}

PixelMonModules2D::PixelMonModules2D(std::string name, std::string title, int nbins0, double low0, double high0, int nbins1, double low1, double high1) {
  m_nBins = nbins0 * nbins1;
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i) = new TH2F((getHistName(i, false) + "_" + name).c_str(),
                          (getHistName(i, false) + " " + title).c_str(),
                          nbins0, low0, high0, nbins1, low1, high1);
  }
  if (m_doIBL == false) {
    for (int i = 1744; i < 2024; i++) {
      getHist(i) = nullptr;
    }
  }
  formatHist("");
  m_Dummy = 0;
}

void PixelMonModulesProf::reset() {
  for (int i = 0; i < 2024; i++) {
    if (getHist(i)) getHist(i)->Reset();
  }
}

StatusCode PixelMonModulesProf::regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    ManagedMonitorToolBase::MonGroup mgroup(thisptr, (path + "/" + getHistName(i, true)).c_str(), Run);
    if (mgroup.regHist(getHist(i)).isFailure()) {
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

void PixelMonModules1D::reset() {
  for (int i = 0; i < 2024; i++) {
    if (getHist(i)) getHist(i)->Reset();
  }
}

void PixelMonModules2D::reset() {
  for (int i = 0; i < 2024; i++) {
    if (getHist(i)) getHist(i)->Reset();
  }
}

void PixelMonModulesProf::fill(double value0, double value1, Identifier& id, const PixelID* pixID) {
  const int bec = pixID->barrel_ec(id);
  const int pm = pixID->phi_module(id);
  int ld = pixID->layer_disk(id);

  if (bec == 2) {
    A[ld][pm]->Fill(value0, value1);
  } else if (bec == -2) {
    C[ld][pm]->Fill(value0, value1);
  } else if (bec == 0) {
    if (m_doIBL) ld--;
    const int em = pixID->eta_module(id);
    if (ld == 0) {
      B0[em + 6][pm]->Fill(value0, value1);
    } else if (ld == 1) {
      B1[em + 6][pm]->Fill(value0, value1);
    } else if (ld == 2) {
      B2[em + 6][pm]->Fill(value0, value1);
    } else if (ld == -1) {
      IBL[em + 10][pm]->Fill(value0, value1);
    }
  }
}

void PixelMonModules2D::fill(double value0, double value1, Identifier& id, const PixelID* pixID, double weight) {
  const int bec = pixID->barrel_ec(id);
  const int pm = pixID->phi_module(id);
  int ld = pixID->layer_disk(id);

  if (bec == 2) {
    A[ld][pm]->Fill(value0, value1, weight);
  } else if (bec == -2) {
    C[ld][pm]->Fill(value0, value1, weight);
  } else if (bec == 0) {
    if (m_doIBL) ld--;
    int em = pixID->eta_module(id);
    if (ld == 0) {
      B0[em + 6][pm]->Fill(value0, value1, weight);
    } else if (ld == 1) {
      B1[em + 6][pm]->Fill(value0, value1, weight);
    } else if (ld == 2) {
      B2[em + 6][pm]->Fill(value0, value1, weight);
    } else if (ld == -1) {
      IBL[em + 10][pm]->Fill(value0, value1, weight);
    }
  }
}

double PixelMonModules1D::getBinContent(double value, Identifier& id, const PixelID* pixID) {
  const int bec = pixID->barrel_ec(id);
  const int pm = pixID->phi_module(id);
  int ld = pixID->layer_disk(id);

  if (bec == 2) {
    return A[ld][pm]->GetBinContent(A[ld][pm]->GetXaxis()->FindBin(value));
  } else if (bec == -2) {
    return C[ld][pm]->GetBinContent(C[ld][pm]->GetXaxis()->FindBin(value));
  } else if (bec == 0) {
    const int em = pixID->eta_module(id);
    if (ld == 0) {
      return B0[em + 6][pm]->GetBinContent(B0[em + 6][pm]->GetXaxis()->FindBin(value));
    } else if (ld == 1) {
      return B1[em + 6][pm]->GetBinContent(B1[em + 6][pm]->GetXaxis()->FindBin(value));
    } else if (ld == 2) {
      return B2[em + 6][pm]->GetBinContent(B2[em + 6][pm]->GetXaxis()->FindBin(value));
    } else if (ld == -1) {
      return IBL[em + 10][pm]->GetBinContent(IBL[em + 10][pm]->GetXaxis()->FindBin(value));
    }
  }
  return 0.0;
}

void PixelMonModules1D::fill(double value, Identifier& id, const PixelID* pixID) {
  const int bec = pixID->barrel_ec(id);
  const int pm = pixID->phi_module(id);
  int ld = pixID->layer_disk(id);

  if (bec == 2) {
    A[ld][pm]->Fill(value);
  } else if (bec == -2) {
    C[ld][pm]->Fill(value);
  } else if (bec == 0) {
    if (m_doIBL) ld--;
    const int em = pixID->eta_module(id);
    if (ld == 0) {
      B0[em + 6][pm]->Fill(value);
    } else if (ld == 1) {
      B1[em + 6][pm]->Fill(value);
    } else if (ld == 2) {
      B2[em + 6][pm]->Fill(value);
    } else if (ld == -1) {
      IBL[em + 10][pm]->Fill(value);
    }
  }
}

StatusCode PixelMonModules1D::regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    ManagedMonitorToolBase::MonGroup mgroup(thisptr, (path + "/" + getHistName(i, true)).c_str(), Run);
    if (mgroup.regHist(getHist(i)).isFailure()) {
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelMonModules2D::regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    ManagedMonitorToolBase::MonGroup mgroup(thisptr, (path + "/" + getHistName(i, true)).c_str(), Run);
    if (mgroup.regHist(getHist(i)).isFailure()) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

void PixelMonModules1D::formatHist(std::string opt) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    if (!opt.compare("status")) getHist(i)->GetXaxis()->SetRangeUser(1., 2.);
    getHist(i)->SetMinimum(0);
  }
}

void PixelMonModulesProf::formatHist(std::string /*opt*/) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i)->SetMinimum(0);
  }
}

void PixelMonModules2D::formatHist(std::string /*opt*/) {
  for (int i = 0; i < 1744 + 280 * m_doIBL; i++) {
    getHist(i)->SetMinimum(0);
    getHist(i)->SetOption("colz");
    getHist(i)->SetStats(0.);
  }
}

void PixelMonModules1D::setBinLabel(const char* label, int binN) {
  if (binN > m_nBins) return;
  for (int i = 0; i < 2024; i++) {
    if (getHist(i)) getHist(i)->GetXaxis()->SetBinLabel(binN, label);
  }
}

TProfile_LW*& PixelMonModulesProf::getHist(int i) {
  if (i < 286) return B0[i / 22][i % 22];
  i -= 286;
  if (i < 494) return B1[i / 38][i % 38];
  i -= 494;
  if (i < 676) return B2[i / 52][i % 52];
  i -= 676;
  if (i < 144) return A[i / 48][i % 48];
  i -= 144;
  if (i < 144) return C[i / 48][i % 48];
  i -= 144;
  if (i < 280) return IBL[i / 14][i % 14];
  return m_Dummy;
}

TH1F*& PixelMonModules1D::getHist(int i) {
  if (i < 286) return B0[i / 22][i % 22];
  i -= 286;
  if (i < 494) return B1[i / 38][i % 38];
  i -= 494;
  if (i < 676) return B2[i / 52][i % 52];
  i -= 676;
  if (i < 144) return A[i / 48][i % 48];
  i -= 144;
  if (i < 144) return C[i / 48][i % 48];
  i -= 144;
  if (i < 280) return IBL[i / 14][i % 14];
  return m_Dummy;
}

TH2F*& PixelMonModules2D::getHist(int i) {
  if (i < 286) return B0[i / 22][i % 22];
  i -= 286;
  if (i < 494) return B1[i / 38][i % 38];
  i -= 494;
  if (i < 676) return B2[i / 52][i % 52];
  i -= 676;
  if (i < 144) return A[i / 48][i % 48];
  i -= 144;
  if (i < 144) return C[i / 48][i % 48];
  i -= 144;
  if (i < 280) return IBL[i / 14][i % 14];
  return m_Dummy;
}

std::string PixelMonModules::getHistName(int i, bool forPath) {
  const int ndisk = 3;
  const int nphi = 48;
  const int nmod = 13;
  const int nmodIBL = 20;
  const int nstaveb = 14;
  const int nstave0 = 22;
  const int nstave1 = 38;
  const int nstave2 = 52;
  std::string diskA[ndisk] = {"D1A", "D2A", "D3A"};
  std::string diskC[ndisk] = {"D1C", "D2C", "D3C"};
  std::string barrel[3] = {"L0", "L1", "L2"};
  std::string newbarrel[4] = {"LI", "L0", "L1", "L2"};
  if (forPath) {
    diskA[0].replace(diskA[0].begin(), diskA[0].end(), "ECA/Disk1");
    diskA[1].replace(diskA[1].begin(), diskA[1].end(), "ECA/Disk2");
    diskA[2].replace(diskA[2].begin(), diskA[2].end(), "ECA/Disk3");
    diskC[0].replace(diskC[0].begin(), diskC[0].end(), "ECC/Disk1");
    diskC[1].replace(diskC[1].begin(), diskC[1].end(), "ECC/Disk2");
    diskC[2].replace(diskC[2].begin(), diskC[2].end(), "ECC/Disk3");
  }
  std::string mod[nmod] = {"M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0",
                           "M1A", "M2A", "M3A", "M4A", "M5A", "M6A"};
  std::string modIBL[nmodIBL] = {
      "M4_C8_2", "M4_C8_1", "M4_C7_2", "M4_C7_1", "M3_C6",   "M3_C5",  "M2_C4",
      "M2_C3",   "M1_C2",   "M1_C1",   "M1_A1",   "M1_A2",   "M2_A3",  "M2_A4",
      "M3_A5",   "M3_A6",   "M4_A7_1", "M4_A7_2", "M4_A8_1", "M4_A8_2"};
  std::string staveb[nstaveb] = {"S01", "S02", "S03", "S04", "S05",
                                 "S06", "S07", "S08", "S09", "S10",
                                 "S11", "S12", "S13", "S14"};
  std::string stave0[nstave0] = {
      "B11_S2", "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1",
      "B03_S2", "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1",
      "B06_S2", "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1",
      "B09_S2", "B10_S1", "B10_S2", "B11_S1"};
  std::string stave1[nstave1] = {
      "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1",
      "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2",
      "B08_S1", "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1",
      "B11_S2", "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2",
      "B15_S1", "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1",
      "B18_S2", "B19_S1", "B19_S2"};
  std::string stave2[nstave2] = {
      "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1", "B04_S2",
      "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1",
      "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1", "B11_S2",
      "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1",
      "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
      "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2", "B22_S1",
      "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2", "B25_S1", "B25_S2",
      "B26_S1", "B26_S2", "B01_S1"};
  std::string staveA[nphi] = {
      "B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3",
      "B01_S2_M4", "B02_S1_M1", "B02_S1_M6", "B02_S1_M2", "B02_S1_M5",
      "B02_S1_M3", "B02_S1_M4", "B02_S2_M1", "B02_S2_M6", "B02_S2_M2",
      "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", "B03_S1_M1", "B03_S1_M6",
      "B03_S1_M2", "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", "B03_S2_M1",
      "B03_S2_M6", "B03_S2_M2", "B03_S2_M5", "B03_S2_M3", "B03_S2_M4",
      "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3",
      "B04_S1_M4", "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5",
      "B04_S2_M3", "B04_S2_M4", "B01_S1_M1", "B01_S1_M6", "B01_S1_M2",
      "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"};
  std::string staveC[nphi] = {
      "B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6",
      "B01_S2_M1", "B02_S1_M4", "B02_S1_M3", "B02_S1_M5", "B02_S1_M2",
      "B02_S1_M6", "B02_S1_M1", "B02_S2_M4", "B02_S2_M3", "B02_S2_M5",
      "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", "B03_S1_M4", "B03_S1_M3",
      "B03_S1_M5", "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", "B03_S2_M4",
      "B03_S2_M3", "B03_S2_M5", "B03_S2_M2", "B03_S2_M6", "B03_S2_M1",
      "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6",
      "B04_S1_M1", "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2",
      "B04_S2_M6", "B04_S2_M1", "B01_S1_M4", "B01_S1_M3", "B01_S1_M5",
      "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"};

  if (forPath) {
    std::string joint = "/";
    if (i < 286) return barrel[0] + joint + stave0[i % 22];
    i -= 286;
    if (i < 494) return barrel[1] + joint + stave1[i % 38];
    i -= 494;
    if (i < 676) return barrel[2] + joint + stave2[i % 52];
    i -= 676;
    if (i < 144) return diskA[i / 48];
    i -= 144;
    if (i < 144) return diskC[i / 48];
    i -= 144;
    if (m_doIBL && i < 280) return newbarrel[0] + joint + staveb[i % 14];

  } else {
    std::string joint = "_";
    if (i < 286) {
      return barrel[0] + joint + stave0[i % 22] + joint + mod[i / 22];
    }
    i -= 286;
    if (i < 494) {
      return barrel[1] + joint + stave1[i % 38] + joint + mod[i / 38];
    }
    i -= 494;
    if (i < 676) {
      return barrel[2] + joint + stave2[i % 52] + joint + mod[i / 52];
    }
    i -= 676;
    if (i < 144) return diskA[i / 48] + joint + staveA[i % 48];
    i -= 144;
    if (i < 144) return diskC[i / 48] + joint + staveC[i % 48];
    i -= 144;
    if (m_doIBL && i < 280) {
      return newbarrel[0] + joint + staveb[i % 14] + joint + modIBL[i / 14];
    }
  }
  std::string dummy = "wrong initialization";
  return dummy;  // should never get here
}

const bool PixelMonModules::m_doIBL{true};
