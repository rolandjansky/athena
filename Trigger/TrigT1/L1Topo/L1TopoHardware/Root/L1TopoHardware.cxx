/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoHardware/L1TopoHardware.h"

#include <iostream>
using namespace std;

std::map<std::string, TCS::HardwareParam> TCS::L1TopoHardwareParameters {};

TCS::HardwareParam::HardwareParam(const std::string & name, uint32_t value, const std::string & description, const std::string & rule) :
   name(name),
   value(value),
   description(description),
   rule(rule)
{}


uint32_t
define(const std::string & name, uint32_t value, const std::string & description, const std::string & rule) {
   TCS::L1TopoHardwareParameters.insert(pair<std::string, TCS::HardwareParam>(name, TCS::HardwareParam(name, value, description, rule)) );
   return value;
}

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1,0)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N
#define macro_dispatcher(func, ...) macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs
#define DEF_HW(...) macro_dispatcher(DEF_HW, __VA_ARGS__)(__VA_ARGS__) class swallow_semicolon

#define DEF_HW1(VARNAME)                                                \
   namespace { const uint32_t VARNAME = define(#VARNAME, TCS::VARNAME, "", ""); }

#define DEF_HW2(VARNAME, DESC)                                          \
   namespace { const uint32_t VARNAME = define(#VARNAME, TCS::VARNAME, DESC, ""); }

#define DEF_HW3(VARNAME, DESC, RULE)                                    \
   namespace { const uint32_t VARNAME = define(#VARNAME, TCS::VARNAME, DESC, RULE); }


DEF_HW(InputWidthEM);
DEF_HW(InputWidthTAU);
DEF_HW(InputWidthJET);
DEF_HW(InputWidthMU);

DEF_HW(InputWidth1stStageSortEM);
DEF_HW(InputWidth1stStageSortTAU);
DEF_HW(InputWidth1stStageSortJET);
DEF_HW(InputWidth1stStageSortMU);

DEF_HW(InputWidth1stStageSelectEM);
DEF_HW(InputWidth1stStageSelectTAU);
DEF_HW(InputWidth1stStageSelectJET);
DEF_HW(InputWidth1stStageSelectMU);

DEF_HW(OutputWidthSortEM);
DEF_HW(OutputWidthSelectEM);
DEF_HW(OutputWidthSortTAU);
DEF_HW(OutputWidthSelectTAU);
DEF_HW(OutputWidthSortJET);
DEF_HW(OutputWidthSelectJET);
DEF_HW(OutputWidthSortMU);
DEF_HW(NumberOfDelayedMuons);
DEF_HW(OutputWidthSelectMU);


DEF_HW(NumberOfAlgorithms);
DEF_HW(NumberOfSortAlgorithms);
DEF_HW(NumberOfResultBits);
DEF_HW(NumberOfOverflowBits);
DEF_HW(NumberOfOutputBits);
	
DEF_HW(KalmanMETCorrection_numberOfEtaBins);
DEF_HW(KalmanMETCorrection_numberOfEtBins);
DEF_HW(KalmanMETCorrection_correctionBitWidth);

DEF_HW(InputWidthMET);
DEF_HW(OutputWidthMET);	
DEF_HW(METEnergyBitWidth);
DEF_HW(METSqrEnergyBitWidth, "", "2 * METEnergyBitWidth");
	
DEF_HW(CosSinBitWidth);
DEF_HW(CoshBitWidth);
DEF_HW(SechTanhBitWidth);
	
DEF_HW(DefaultJetSize);

DEF_HW(ClusterEtBitWidth);
DEF_HW(ClusterIsolBitWidth);
DEF_HW(ClusterEtaBitWidth);
DEF_HW(ClusterPhiBitWidth);
DEF_HW(JetEt1BitWidth);
DEF_HW(JetEt2BitWidth);
DEF_HW(JetEtaBitWidth);
DEF_HW(JetPhiBitWidth);
DEF_HW(MuonPtBitWidth);
DEF_HW(MuonEtaBitWidth);
DEF_HW(MuonPhiBitWidth);
DEF_HW(GenericEtBitWidth);
DEF_HW(GenericEtaBitWidth);
DEF_HW(GenericPhiBitWidth);

DEF_HW(MassSqrBitWidth,"","GenericEtBitWidth +  METEnergyBitWidth + 1 + CosSinBitWidth + 1");

DEF_HW(ParameterBitWidth, "Internal Data width");

DEF_HW(MaxParams, "Maximum number of parameters");
