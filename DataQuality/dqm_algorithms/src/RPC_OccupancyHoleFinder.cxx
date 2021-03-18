/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/*! \file
 * \author Justin Griffiths
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/RPC_OccupancyHoleFinder.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <initializer_list>
#include <tuple>

#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <sstream>
#include <cstdlib>

#include <dqm_core/AlgorithmManager.h>


namespace {
  dqm_algorithms::RPC_OccupancyHoleFinder RPC("RPC");
  dqm_algorithms::RPC_OccupancyHoleFinder Default("");
  bool
  listContains(const std::initializer_list<int>& a, int i) {
    return std::any_of(a.begin(), a.end(), [i](int x) {
      return i == x;
    });
  }
}

dqm_algorithms::RPC_OccupancyHoleFinder::RPC_OccupancyHoleFinder(const std::string& n)
  : m_name(n) {
  if (m_name.size()) dqm_core::AlgorithmManager::instance().registerAlgorithm(m_name + "_RPC_OccupancyHoleFinder",
                                                                              this);
  else dqm_core::AlgorithmManager::instance().registerAlgorithm("RPC_OccupancyHoleFinder", this);
}

dqm_algorithms::RPC_OccupancyHoleFinder::~RPC_OccupancyHoleFinder() {
}

dqm_algorithms::RPC_OccupancyHoleFinder*
dqm_algorithms::RPC_OccupancyHoleFinder::clone() {
  return new RPC_OccupancyHoleFinder(m_name);
}

dqm_core::Result*
dqm_algorithms::RPC_OccupancyHoleFinder::execute(const std::string& name,
                                                 const TObject& object,
                                                 const dqm_core::AlgorithmConfig& config) {
  const TH2* histo = dynamic_cast<const TH2*> (&object);

  if (!histo) {
    throw dqm_core::BadConfig(ERS_HERE, name, "Input object is not a TH2");
  }

  std::vector<std::string>  dead_rpc_panel {};
  std::vector<int>  coun_rpc_panel {};

  const double minstat = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);

  if (histo->GetEntries() < minstat) {
    dqm_core::Result* result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histo->GetEntries();
    return result;
  }

  int TotalPanels = (int) dqm_algorithms::tools::GetFirstFromMap("TotalPanels", config.getParameters(), -99999);

  double gthreshold {};
  double rthreshold {};
  double bin_threshold {};
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap("BinThreshold", config.getParameters());
    rthreshold = dqm_algorithms::tools::GetFromMap("NBins", config.getRedThresholds());
    gthreshold = dqm_algorithms::tools::GetFromMap("NBins", config.getGreenThresholds());
  }
  catch (dqm_core::Exception& ex) {
    throw dqm_core::BadConfig(ERS_HERE, name, ex.what(), ex);
  }

  dead_rpc_panel.clear(   );
  coun_rpc_panel.clear(   );
  using namespace std::string_view_literals; //for sv suffix denoting a string view
  for (int ibinx = 1; ibinx != histo->GetNbinsX() + 1; ++ibinx) {
    for (int ibiny = 1; ibiny != histo->GetNbinsY() + 1; ++ibiny) {
      float content = histo->GetBinContent(ibinx, ibiny);
      if (content < bin_threshold) {
        std::string chamber_name {};
        std::string Title = histo->GetTitle();
        if (Title.find("HighPt0_BA") != std::string::npos || Title.find("HighPt1_BA") != std::string::npos) {
          if (listContains({1, 2, 5, 6, 9, 10, 13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {//14 values
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            //note: the 'sv' suffix in namespace std::string_view_literals denotes a string_view
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOL1DZ1"sv,
              "BOL1DZ2"sv,
              "BOL2DZ1"sv,
              "BOL2DZ2"sv,
              "BOL3DZ1"sv,
              "BOL3DZ2"sv,
              "BOL4DZ1"sv,
              "BOL4DZ2"sv,
              "BOL5DZ1"sv,
              "BOL5DZ2"sv,
              "BOL6DZ1"sv,
              "BOL6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOS1DZ1"sv,
              "BOS1DZ2"sv,
              "BOS2DZ1"sv,
              "BOS2DZ2"sv,
              "BOS3DZ1"sv,
              "BOS3DZ2"sv,
              "BOS4DZ1"sv,
              "BOS4DZ2"sv,
              "BOS5DZ1"sv,
              "BOS5DZ2"sv,
              "BOS6DZ1"sv,
              "BOS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {13};
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOL1DZ1"sv,
              "BOL1DZ2"sv,
              "BOL2DZ1"sv,
              "BOL2DZ2"sv,
              "BOL3DZ1"sv,
              "BOL3DZ2"sv,
              "BOL4DZ1"sv,
              "BOL4DZ2"sv,
              "BOL5DZ1"sv,
              "BOL5DZ2"sv,
              "BOL6DZ1"sv,
              "BOL6DZ2"sv,
              "BOL8DZ1"sv
            };
            //this recreates original logic, but is it correct? Perhaps should 'continue' ?
            if (ibinx <= maxPossible) chamber_name = chamberNames[ibinx - 1];
            //
          } else {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOF1DZ1"sv,
              "BOF1DZ2"sv,
              "BOF2DZ1"sv,
              "BOF2DZ2"sv,
              "BOF3DZ1"sv,
              "BOF3DZ2"sv,
              "BOF4DZ1"sv,
              "BOG0DZ1"sv,
              "BOG1DZ1"sv,
              "BOG2DZ1"sv,
              "BOG3DZ1"sv,
              "BOG4DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("LowPt0_BA") != std::string::npos || Title.find("LowPt1_BA") != std::string::npos) {
          if (listContains({1, 2, 5, 6, 9, 10}, ibiny)) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {9};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {5};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("Pivot0_BA") != std::string::npos || Title.find("Pivot1_BA") != std::string::npos ||
                   Title.find("LowPt_BA") != std::string::npos) {
          if (listContains({1, 2, 5, 6, 9, 10}, ibiny)) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv,
              "BML6DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMS4DZ2"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv,
              "BMF3DZ2"sv,
              "BOF2DZ1"sv,
              "BOF2DZ2"sv,
              "BOF3DZ1"sv,
              "BOF3DZ2"sv,
              "BOF4DZ1"sv,
              "BOG3DZ1"sv,
              "BOG4DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("HighPt_BA") != std::string::npos) {
          if (listContains({1, 2, 5, 6, 9, 10}, ibiny)) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv,
              "BML6DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMS4DZ2"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {6};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv,
              "BMF3DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("HighPt0_BC") != std::string::npos || Title.find("HighPt1_BC") != std::string::npos) {
          if (listContains({1, 2, 5, 6, 9, 10, 13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOL1DZ1"sv,
              "BOL1DZ2"sv,
              "BOL2DZ1"sv,
              "BOL2DZ2"sv,
              "BOL3DZ1"sv,
              "BOL3DZ2"sv,
              "BOL4DZ1"sv,
              "BOL4DZ2"sv,
              "BOL5DZ1"sv,
              "BOL5DZ2"sv,
              "BOL6DZ1"sv,
              "BOL6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOS1DZ1"sv,
              "BOS1DZ2"sv,
              "BOS2DZ1"sv,
              "BOS2DZ2"sv,
              "BOS3DZ1"sv,
              "BOS3DZ2"sv,
              "BOS4DZ1"sv,
              "BOS4DZ2"sv,
              "BOS5DZ1"sv,
              "BOS5DZ2"sv,
              "BOS6DZ1"sv,
              "BOS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {13};
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOL1DZ1"sv,
              "BOL1DZ2"sv,
              "BOL2DZ1"sv,
              "BOL2DZ2"sv,
              "BOL3DZ1"sv,
              "BOL3DZ2"sv,
              "BOL4DZ1"sv,
              "BOL4DZ2"sv,
              "BOL5DZ1"sv,
              "BOL5DZ2"sv,
              "BOL6DZ1"sv,
              "BOL6DZ2"sv,
              "BOL8DZ1"sv
            };
            //recreates original logic, but perhaps should 'continue'?
            chamber_name = (ibinx > maxPossible) ? "" : chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BOF1DZ1"sv,
              "BOF1DZ2"sv,
              "BOF2DZ1"sv,
              "BOF2DZ2"sv,
              "BOF3DZ1"sv,
              "BOF3DZ2"sv,
              "BOF4DZ1"sv,
              "BOG1DZ1"sv,
              "BOG2DZ1"sv,
              "BOG3DZ1"sv,
              "BOG4DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("LowPt0_BC") != std::string::npos || Title.find("LowPt1_BC") != std::string::npos) {
          if (ibiny == 5 || ibiny == 6 || ibiny == 9 || ibiny == 10) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({1, 2, 13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {9};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {12};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {5};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("Pivot0_BC") != std::string::npos || Title.find("Pivot1_BC") != std::string::npos ||
                   Title.find("LowPt_BC") != std::string::npos) {
          if (ibiny == 5 || ibiny == 6 || ibiny == 9 || ibiny == 10) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({1, 2, 13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv,
              "BML6DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMS4DZ2"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv,
              "BMF3DZ2"sv,
              "BOF2DZ1"sv,
              "BOF2DZ2"sv,
              "BOF3DZ1"sv,
              "BOF3DZ2"sv,
              "BOF4DZ1"sv,
              "BOG3DZ1"sv,
              "BOG4DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        } else if (Title.find("HighPt_BC") != std::string::npos) {
          if (ibiny == 5 || ibiny == 6 || ibiny == 9 || ibiny == 10) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({1, 2, 13, 14, 17, 18, 21, 22, 29, 30}, ibiny)) {
            constexpr int maxPossible {10};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BML1DZ1"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML6DZ1"sv,
              "BML6DZ2"sv,
              "BML7DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (ibiny == 25 || ibiny == 26) {
            constexpr int maxPossible {11};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMEDZ1"sv,
              "BML1DZ1"sv,
              "BML1DZ2"sv,
              "BML2DZ1"sv,
              "BML2DZ2"sv,
              "BML3DZ1"sv,
              "BML3DZ2"sv,
              "BML4DZ1"sv,
              "BML5DZ1"sv,
              "BML5DZ2"sv,
              "BML6DZ1"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else if (listContains({3, 4, 7, 8, 11, 12, 15, 16, 19, 20, 31, 32}, ibiny)) {
            constexpr int maxPossible {13};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMS1DZ1"sv,
              "BMS1DZ2"sv,
              "BMS2DZ1"sv,
              "BMS2DZ2"sv,
              "BMH2DZ1"sv,
              "BMS3DZ1"sv,
              "BMS3DZ2"sv,
              "BMS4DZ1"sv,
              "BMS4DZ2"sv,
              "BMH4DZ1"sv,
              "BMS5DZ1"sv,
              "BMS6DZ1"sv,
              "BMS6DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          } else {
            constexpr int maxPossible {6};
            if (ibinx > maxPossible) continue;
            constexpr std::array<std::string_view, maxPossible> chamberNames {
              "BMF1DZ1"sv,
              "BMF1DZ2"sv,
              "BMF2DZ1"sv,
              "BMF2DZ2"sv,
              "BMF3DZ1"sv,
              "BMF3DZ2"sv
            };
            chamber_name = chamberNames[ibinx - 1];
          }
        }
        //FORTMAT BMS4A10.Piv.Dp0.Ly0.Dz2.eta

        std::string sector_name = getChamberName(histo, ibiny);
        std::string chamber {};
        std::string sector {};
        std::string secnum {};
        std::string layer {};
        std::string dphi {};
        std::string gap {};
        std::string dzeta {};
        std::string view {};
        std::string dot = ".";
        if (Title.find("Eta") != std::string::npos) view = "eta";
        if (Title.find("Phi") != std::string::npos) view = "phi";
        secnum = sector_name.substr(1, 2);
        if (sector_name.find("HV") != std::string::npos) {
          dphi = "DP1";
          if (secnum == "02" || secnum == "04" || secnum == "06" || secnum == "08" || secnum == "10" ||
              secnum == "12" || secnum == "14" || secnum == "16") dphi = "DP2";
        } else if (sector_name.find("RO") != std::string::npos) {
          dphi = "DP2";
          if (secnum == "02" || secnum == "04" || secnum == "06" || secnum == "08" || secnum == "10" ||
              secnum == "12" || secnum == "14" || secnum == "16") dphi = "DP1";
        }

        if (Title.find("HighPt0") != std::string::npos) {
          layer = ".CO.";
          gap = ".Ly0.";
        }
        if (Title.find("HighPt1") != std::string::npos) {
          layer = ".CO.";
          gap = ".Ly1.";
        }
        if (Title.find("LowPt0") != std::string::npos) {
          layer = ".CO.";
          gap = ".Ly0.";
        }
        if (Title.find("LowPt1") != std::string::npos) {
          layer = ".CO.";
          gap = ".Ly1.";
        }
        if (Title.find("Pivot0") != std::string::npos) {
          layer = ".PI.";
          gap = ".Ly0.";
        }
        if (Title.find("Pivot1") != std::string::npos) {
          layer = ".PI.";
          gap = ".Ly1.";
        }
        if (Title.find("LowPt_") != std::string::npos) {
          layer = ".";
          gap = ".";
        }
        if (Title.find("HighPt_") != std::string::npos) {
          layer = ".";
          gap = ".";
        }
        sector = sector_name.substr(0, 3);
        dzeta = chamber_name;
        dzeta.erase(0, 4);
        chamber = chamber_name.substr(0, 4);
        std::string panel_name = chamber + sector + layer + dphi + gap + dzeta + dot + view;
        dead_rpc_panel.push_back(panel_name);
        coun_rpc_panel.push_back(content);
      }
    }//ybins
  }//xbins

  //Analyze chambers that died during run
  dqm_core::Result* result = new dqm_core::Result();
  std::map<std::string, double> tags;

  //ensure that we do not write too much info to the webpage
  int writes = 0;

  for (std::vector<std::string>::const_iterator itr = dead_rpc_panel.begin(); itr != dead_rpc_panel.end(); ++itr) {
    tags[TString::Format("%s", (*itr).c_str()).Data()] = coun_rpc_panel.at(writes);
    writes++;
    if (writes > 100) break;
  }

  int count = TotalPanels - dead_rpc_panel.size();

  tags[TString::Format("Alive panels").Data()] = 100. * count / TotalPanels;
  tags[TString::Format("# Alive  panels").Data()] = count;
  tags[TString::Format("# Dead  panels").Data()] = writes;


  if (count >= gthreshold) {
    result->status_ = dqm_core::Result::Green;
  } else if (count > rthreshold) {
    result->status_ = dqm_core::Result::Yellow;
  } else {
    result->status_ = dqm_core::Result::Red;
  }

  result->tags_ = tags;
  return result;
}

void
dqm_algorithms::RPC_OccupancyHoleFinder::printDescription(std::ostream& /*out*/) {
//   out<<"RPC_OccupancyHoleFinder: Checks number of bins N sigma away from reference histogram bin value or given
// Value\n"<<std::endl;
//   out<<"Simply Dump the Bin Number/Name of Bin with bin contents as result" << std::endl;
}

std::string
dqm_algorithms::RPC_OccupancyHoleFinder::getChamberName(const TH2* histo, int biny) {
  if (std::string(histo->GetYaxis()->GetBinLabel(biny)).size()) {
    std::string label = histo->GetYaxis()->GetBinLabel(biny);
    label.erase(6, 4);
    label.erase(3, 1);
    return label;
  }
  std::stringstream ss;
  ss << biny;
  return ss.str();
}
