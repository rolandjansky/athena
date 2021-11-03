/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoHardware/L1TopoHardware.h"

#include "boost/preprocessor/facilities/overload.hpp"

#define DEF_HW(...) \
   BOOST_PP_OVERLOAD(DEF_HW, __VA_ARGS__)(__VA_ARGS__)

#define DEF_HW1(VARNAME)                                               \
   {#VARNAME, TCS::HardwareParam{#VARNAME, TCS::VARNAME, "", ""}}

#define DEF_HW2(VARNAME, DESC)                                          \
   {#VARNAME, TCS::HardwareParam{#VARNAME, TCS::VARNAME, DESC, ""}}

#define DEF_HW3(VARNAME, DESC, RULE)                                    \
   {#VARNAME, TCS::HardwareParam{#VARNAME, TCS::VARNAME, DESC, RULE}}


const std::map<std::string, TCS::HardwareParam>& TCS::L1TopoHWParameters::get() {
   static const std::map<std::string, TCS::HardwareParam> _hw = {

      DEF_HW(eEmInputWidth),
      DEF_HW(eTauInputWidth),

      DEF_HW(jJetInputWidth),
      DEF_HW(jLargeRJetInputWidth),
      DEF_HW(jTauInputWidth),
      DEF_HW(jEmInputWidth),
      DEF_HW(jMetInputWidth),

      DEF_HW(gJetInputWidth),
      DEF_HW(gLargeRJetInputWidth),

      DEF_HW(muonInputWidth),

      DEF_HW(eEmOutputWidthSort),
      DEF_HW(eEmOutputWidthSelect),
      DEF_HW(eTauOutputWidthSort),
      DEF_HW(eTauOutputWidthSelect),
      DEF_HW(jJetOutputWidthSort),
      DEF_HW(jJetOutputWidthSelect),
      DEF_HW(muonOutputWidthSort),
      DEF_HW(NumberOfDelayedMuons),
      DEF_HW(muonOutputWidthSelect),
      DEF_HW(metOutputWidth),

      DEF_HW(InputWidthEM),
      DEF_HW(InputWidthTAU),
      DEF_HW(InputWidthJET),
      DEF_HW(InputWidthMU),

      DEF_HW(InputWidth1stStageSortEM),
      DEF_HW(InputWidth1stStageSortTAU),
      DEF_HW(InputWidth1stStageSortJET),
      DEF_HW(InputWidth1stStageSortMU),

      DEF_HW(InputWidth1stStageSelectEM),
      DEF_HW(InputWidth1stStageSelectTAU),
      DEF_HW(InputWidth1stStageSelectJET),
      DEF_HW(InputWidth1stStageSelectMU),

      DEF_HW(OutputWidthSortEM),
      DEF_HW(OutputWidthSelectEM),
      DEF_HW(OutputWidthSortTAU),
      DEF_HW(OutputWidthSelectTAU),
      DEF_HW(OutputWidthSortJET),
      DEF_HW(OutputWidthSelectJET),
      DEF_HW(OutputWidthSortMU),
      DEF_HW(NumberOfDelayedMuons),
      DEF_HW(OutputWidthSelectMU),

      DEF_HW(NumberOfAlgorithms),
      DEF_HW(NumberOfSortAlgorithms),
      DEF_HW(NumberOfResultBits),
      DEF_HW(NumberOfOverflowBits),
      DEF_HW(NumberOfOutputBits),

      DEF_HW(KalmanMETCorrection_numberOfEtaBins),
      DEF_HW(KalmanMETCorrection_numberOfEtBins),
      DEF_HW(KalmanMETCorrection_correctionBitWidth),

      DEF_HW(InputWidthMET),
      DEF_HW(OutputWidthMET),
      DEF_HW(METEnergyBitWidth),
      DEF_HW(METSqrEnergyBitWidth, "", "2 * METEnergyBitWidth"),

      DEF_HW(CosSinBitWidth),
      DEF_HW(CoshBitWidth),
      DEF_HW(SechTanhBitWidth),

      DEF_HW(DefaultJetSize),

      DEF_HW(ClusterEtBitWidth),
      DEF_HW(ClusterIsolBitWidth),
      DEF_HW(ClusterEtaBitWidth),
      DEF_HW(ClusterPhiBitWidth),
      DEF_HW(JetEt1BitWidth),
      DEF_HW(JetEt2BitWidth),
      DEF_HW(JetEtaBitWidth),
      DEF_HW(JetPhiBitWidth),
      DEF_HW(MuonPtBitWidth),
      DEF_HW(MuonEtaBitWidth),
      DEF_HW(MuonPhiBitWidth),
      DEF_HW(GenericEtBitWidth),
      DEF_HW(GenericEtaBitWidth),
      DEF_HW(GenericPhiBitWidth),

      DEF_HW(MassSqrBitWidth,"","GenericEtBitWidth +  METEnergyBitWidth + 1 + CosSinBitWidth + 1"),

   };

   return _hw;
}
