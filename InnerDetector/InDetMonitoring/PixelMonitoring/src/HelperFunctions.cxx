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
#include "PixelMonitoring/PixelMainMon.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
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
  if (ontrk && m_doOnTrack) name += "_OnTrack";
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

void PixelMainMon::fillTimeHisto(double value, TProfile* one = 0, TProfile* two = 0, TProfile* three = 0, double time1 = 10., double time2 = 60., double time3 = 360.) {
  // This function fills time profile histograms
  time_t rawtime;
  struct tm* timeinfo;
  char buffer1[14];

  time_t endtime;
  struct tm* endtimeinfo;
  char buffer2[14];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer1, 14, "%I:%M:%S", timeinfo);
  char* buffer = buffer1;

  // Make m_startTime a global variable
  double lagtime = difftime(rawtime, m_startTime);  // how long since we started the run;

  if (one) {
    int binNo = one->GetNbinsX();
    double histoTime = time1 * 60.;                            // number of seconds this histogram represents;
    double timeInterval = histoTime / binNo;                   // number of seconds per bin
    int intervalNumber = int(lagtime / timeInterval);          // what interval we are in.  Keep this stored as number of Entries;
    int bins2shift = intervalNumber - int(one->GetEntries());  // how many bins to shift the bin contents;
    if (bins2shift) {                                          // shift bins, skip this if we are in the same bin
      for (int iii = 0; iii <= binNo - bins2shift; iii++) {    // loop over all bins that need moved;
        one->SetBinEntries(iii, one->GetBinEntries(iii + bins2shift));                                         // move bins to right
        one->SetBinContent(iii, one->GetBinContent(iii + bins2shift) * one->GetBinEntries(iii + bins2shift));  // move bins to right
      }
      for (int jjj = binNo; jjj > binNo - bins2shift; jjj--) {
        one->SetBinContent(jjj, 0.0);
        one->SetBinEntries(jjj, 0.0);
      }
      one->GetXaxis()->SetBinLabel(binNo, buffer);  // write the timestamp on the first and last bin
      endtime = time_t(rawtime - histoTime);
      endtimeinfo = localtime(&endtime);
      strftime(buffer2, 14, "%I:%M:%S", endtimeinfo);
      char* buffer3 = buffer2;
      one->GetXaxis()->SetBinLabel(1, buffer3);  // write the timestamp on the first and last bin
      one->LabelsOption("h", "X");
    }
    double binEntries = one->GetBinEntries(binNo);
    double binContent = one->GetBinContent(binNo);
    one->SetBinEntries(binNo, binEntries + 1);
    one->SetBinContent(binNo, binContent * binEntries + value);

    one->SetEntries(intervalNumber);  // store the interval number in a convienent place
  }

  if (two) {
    int binNo = two->GetNbinsX();
    double histoTime = time2 * 60.;                            // number of seconds this histogram represents;
    double timeInterval = histoTime / binNo;                   // number of seconds per bin
    int intervalNumber = int(lagtime / timeInterval);          // what interval we are in.  Keep this stored as number of Entries;
    int bins2shift = intervalNumber - int(two->GetEntries());  // how many bins to shift the bin contents;
    if (bins2shift) {                                          // shift bins, skip this if we are in the same bin
      for (int iii = 0; iii <= binNo - bins2shift; iii++) {    // loop over all bins that need moved;
        two->SetBinEntries(iii, two->GetBinEntries(iii + bins2shift));                                         // move bins to right
        two->SetBinContent(iii, two->GetBinContent(iii + bins2shift) * two->GetBinEntries(iii + bins2shift));  // move bins to right
      }
      for (int jjj = binNo; jjj > binNo - bins2shift; jjj--) {
        two->SetBinContent(jjj, 0.0);
        two->SetBinEntries(jjj, 0.0);
      }
      two->GetXaxis()->SetBinLabel(binNo, buffer);  // write the timestamp on the first and last bin
      endtime = rawtime - time2 * 60;
      endtimeinfo = localtime(&endtime);
      strftime(buffer2, 14, "%I:%M:%S", endtimeinfo);
      char* buffer3 = buffer2;
      two->GetXaxis()->SetBinLabel(1, buffer3);  // write the timestamp on the first and last bin
      two->LabelsOption("h", "X");
    }
    double binEntries = two->GetBinEntries(binNo);
    double binContent = two->GetBinContent(binNo);
    two->SetBinEntries(binNo, binEntries + 1);
    two->SetBinContent(binNo, binContent * binEntries + value);

    two->SetEntries(intervalNumber);  // store the interval number in a convienent place
  }

  if (three) {
    int binNo = three->GetNbinsX();
    double histoTime = time3 * 60.;                              // number of seconds this histogram represents;
    double timeInterval = histoTime / binNo;                     // number of seconds per bin
    int intervalNumber = int(lagtime / timeInterval);            // what interval we are in.  Keep this stored as number of Entries;
    int bins2shift = intervalNumber - int(three->GetEntries());  // how many bins to shift the bin contents;
    if (bins2shift) {                                            // shift bins, skip this if we are in the same bin
      for (int iii = 0; iii <= binNo - bins2shift; iii++) {      // loop over all bins that need moved;
        three->SetBinEntries(iii, three->GetBinEntries(iii + bins2shift));                                           // move bins to right
        three->SetBinContent(iii, three->GetBinContent(iii + bins2shift) * three->GetBinEntries(iii + bins2shift));  // move bins to right
      }
      for (int jjj = binNo; jjj > binNo - bins2shift; jjj--) {
        three->SetBinContent(jjj, 0.0);
        three->SetBinEntries(jjj, 0.0);
      }
      three->GetXaxis()->SetBinLabel(binNo, buffer);  // write the timestamp on the first and last bin
      endtime = rawtime - time3 * 60;
      endtimeinfo = localtime(&endtime);
      strftime(buffer2, 14, "%I:%M:%S", endtimeinfo);
      char* buffer3 = buffer2;
      three->GetXaxis()->SetBinLabel(1, buffer3);  // write the timestamp on the first and last bin
      three->LabelsOption("h", "X");
    }
    double binEntries = three->GetBinEntries(binNo);
    double binContent = three->GetBinContent(binNo);
    three->SetBinEntries(binNo, binEntries + 1);
    three->SetBinContent(binNo, binContent * binEntries + value);

    three->SetEntries(intervalNumber);  // store the interval number in a convienent place
  }
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
  strcpy(pch, detailsMod.c_str());
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
