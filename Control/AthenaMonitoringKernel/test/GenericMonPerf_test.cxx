/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Performance tests of monitoring framework
 *
 * To profile:
 *   1) cmake -DVALGRIND=ON ...
 *   2) adjust the CALLGRIND_ macros below to target the relevant code
 *   3) valgrind --tool=callgrind --collect-jumps=yes --trace-children=no --instr-atstart=no GenericMonPerf_test.exe
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/checker_macros.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/expect.h"
#include "TestTools/initGaudi.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TInterpreter.h"
#include "TMath.h"

#include "boost/program_options.hpp"

#ifdef ATHMON_VALGRIND
#include "valgrind/callgrind.h"
#else
#define CALLGRIND_START_INSTRUMENTATION
#define CALLGRIND_STOP_INSTRUMENTATION
#endif

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

ITHistSvc* histSvc{nullptr};
size_t RUNS;

void resetHists()
{
  for (const std::string& name : histSvc->getHists()) {
    TH1* h(nullptr);
    histSvc->getHist(name, h).ignore();
    if (h) h->Reset();
  }
}

/**
 * Time func executing N times and calculate mean/stddev over R runs
 */
template <typename F>
std::string timeit(const F& func, size_t N = 10000)
{
  std::vector<int> times;
  times.reserve(RUNS);
  for (size_t r = 0; r < RUNS; ++r) {
    resetHists();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
      func();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
  }
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(0) << TMath::Mean(times.begin(), times.end()) << " +- "
      << TMath::StdDev(times.begin(), times.end()) << " us";

  return oss.str();
}

/**
 * Time function f1 and f2, cross-check #fills on histogram h
 */
template <typename F1, typename F2>
void timeboth(const F1& f1, const F2& f2, const TH1* h, const std::string& title)
{
  auto t1 = timeit(f1);
  int fills = h->GetEntries();
  auto t2 = timeit(f2);
  VALUE(h->GetEntries()) EXPECTED(fills);

  std::cout << std::left << std::setw(30) << title << "MON: " << std::setw(20) << t1 << " ROOT: " << t2
            << std::endl;
}

void fillFromScalar(ToolHandle<GenericMonitoringTool>& monTool)
{
  auto mon = [&]() {
    auto eta = Monitored::Scalar<double>("Eta", -0.2);
    auto group = Monitored::Group(monTool, eta);
  };

  TH1* h(nullptr);
  histSvc->getHist("/EXPERT/TestGroup/Eta", h).ignore();
  auto root = [&]() {
    h->Fill(-0.2);
  };
  timeboth(mon, root, h, "fillFromScalar");
}

void fillFromMultipleScalars(ToolHandle<GenericMonitoringTool>& monTool)
{
  auto mon = [&]() {
    auto eta = Monitored::Scalar<double>("Eta", -0.2);
    auto phi = Monitored::Scalar<double>("Phi", 0.1);
    auto group = Monitored::Group(monTool, eta, phi);
  };

  TH1* h1(nullptr);
  TH1* h2(nullptr);
  TH2* h3(nullptr);
  histSvc->getHist("/EXPERT/TestGroup/Eta", h1).ignore();
  histSvc->getHist("/EXPERT/TestGroup/Phi", h2).ignore();
  histSvc->getHist("/EXPERT/TestGroup/Phi_vs_Eta", h3).ignore();
  auto root = [&]() {
    h1->Fill(-0.2);
    h2->Fill(0.1);
    h3->Fill(-0.2, 0.1);
  };
  timeboth(mon, root, h3, "fillFromMultipleScalars");
}

void fillFromCollection(ToolHandle<GenericMonitoringTool>& monTool)
{
  std::vector<double> v = {1.0, 3.2, -0.2, 0.3, 1.2};
  auto mon = [&]() {
    auto eta = Monitored::Collection("Eta", v);
    auto group = Monitored::Group(monTool, eta);
  };

  TH1* h(nullptr);
  histSvc->getHist("/EXPERT/TestGroup/Eta", h).ignore();
  auto root = [&]() {
    for (double d : v) h->Fill(d);
  };
  timeboth(mon, root, h, "fillFromCollection");
}

int main(int argc, char** argv)
{
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "help message")("runs,r", po::value<size_t>(&RUNS)->default_value(1),
                                               "number of runs");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  CxxUtils::ubsan_suppress([]() { TInterpreter::Instance(); });
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("GenericMonPerf.txt", pSvcLoc)) {
    std::cerr << "ERROR This test can not be run" << std::endl;
    return -1;
  }
  MsgStream log(Athena::getMessageSvc(), "GenericMonPerf_test");

  CHECK_WITH_CONTEXT(pSvcLoc->service("THistSvc", histSvc, true), "GenericMonPerf_test", -1);

  ISvcManager* svcmgr = dynamic_cast<ISvcManager*>(pSvcLoc);
  svcmgr->startServices().ignore();

  ToolHandle<GenericMonitoringTool> monTool("GenericMonitoringTool/MonTool");
  CHECK_WITH_CONTEXT(monTool.retrieve(), "GenericMonPerf_test", -1);

  log << MSG::INFO << "Histograms defined: " << histSvc->getHists() << endmsg;

  fillFromScalar(monTool);

  CALLGRIND_START_INSTRUMENTATION;
  fillFromMultipleScalars(monTool);
  CALLGRIND_STOP_INSTRUMENTATION;

  fillFromCollection(monTool);

  // Make sure that THistSvc gets finalized.
  // Otherwise, the output file will get closed while global dtors are running,
  // which can lead to crashes.
  svcmgr->stopServices().ignore();
  svcmgr->finalizeServices().ignore();

  return 0;
}
