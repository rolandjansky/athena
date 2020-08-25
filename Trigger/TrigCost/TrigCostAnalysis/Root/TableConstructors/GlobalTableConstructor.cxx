/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "../../TrigCostAnalysis/TableConstructors/GlobalTableConstructor.h"

#include "TH1.h"

GlobalTableConstructor::GlobalTableConstructor(const std::string& name) : TableConstructorBase(name) {
  addExpectedHistogram("AlgTime_perEvent");
  addExpectedHistogram("AlgTime_perCall");
  addExpectedHistogram("AlgCalls_perEvent");
  addExpectedHistogram("SteeringTime_perEvent");
  addExpectedHistogram("LbLength");

  getBaseEntry().addColumn("name", "Name", "Algorithms name");
  getBaseEntry().addColumn("lbLength", "Lumi Block Length [s]", "Length of this luminosity block or blocks");
  getBaseEntry().addColumn("events", "Raw Active Events", "Raw underlying statistics on number of events processed");
  getBaseEntry().addColumn("eventsWeighted", "Active Events", "Total weighted number of events processed");
  getBaseEntry().addColumn("callsPerEvent", "Alg Calls/Event", "Mean number of alg calls");
  getBaseEntry().addColumn("eventRate", "Event Rate [Hz]", "LB normalised rate of events in the HLT");
  getBaseEntry().addColumn("callRate", "Alg Call Rate [Hz]", "LB normalised rate of alg calls in events in the HLT");
  getBaseEntry().addColumn("steeringTime", "Total Steering Time [s]", "Total weighted integrated steering time");
  getBaseEntry().addColumn("steeringTimePerEvent", "Steering Time/Event [ms]", "Mean integrated steering time per event");
  getBaseEntry().addColumn("totalTimeSec", "Alg Total Time [s]", "Total weighted integrated walltime of all algs");
  getBaseEntry().addColumn("timePerCall", "Alg Time/Call [ms]", "Mean weighted alg time normalised to all alg calls");
  getBaseEntry().addColumn("timePerEvent", "Alg Time/Event [ms]", "Mean weighted alg time normalised to all events");
}


TableEntry GlobalTableConstructor::getTableEntry(const std::string name) {
  TableEntry tableEntry(getBaseEntry());
  getHistograms(name);

  const float weightedEvents = hist("SteeringTime_perEvent")->Integral(); // Filled once per event with event weight
  const float weightedCalls = histGetXWeightedIntegral("AlgCalls_perEvent", /*isLog*/ false);
  const float lbLength = hist("LbLength")->GetBinContent(1);

  tableEntry.setEntry("name", name);
  tableEntry.setEntry("lbLength", lbLength);
  tableEntry.setEntry("events", hist("SteeringTime_perEvent")->GetEntries());
  tableEntry.setEntry("eventsWeighted", weightedEvents);

  tableEntry.setEntry("callsPerEvent", hist("AlgCalls_perEvent")->GetMean());

  tableEntry.setEntry("eventRate", weightedEvents / lbLength);
  tableEntry.setEntry("callRate", weightedCalls / lbLength);

  tableEntry.setEntry("steeringTime", histGetXWeightedIntegral("SteeringTime_perEvent", /*isLog*/ true) * 1e-3);
  tableEntry.setEntry("steeringTimePerEvent", hist("SteeringTime_perEvent")->GetMean());

  tableEntry.setEntry("totalTimeSec", histGetXWeightedIntegral("AlgTime_perEvent", /*isLog*/ true) * 1e-3);
  tableEntry.setEntry("timePerCall", hist("AlgTime_perCall")->GetMean());
  tableEntry.setEntry("timePerEvent", hist("AlgTime_perEvent")->GetMean());

  return tableEntry;
}

void GlobalTableConstructor::tablePostProcessing(std::vector<TableEntry>&, const float) {
}

