/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "../../TrigCostAnalysis/TableConstructors/ThreadOccupancyTableConstructor.h"

#include "TH1.h"

ThreadOccupancyTableConstructor::ThreadOccupancyTableConstructor(const std::string& name) : TableConstructorBase(name) {
  addExpectedHistogram("AlgCalls_perEvent");
  addExpectedHistogram("ThisAlgCalls_perEvent");
  addExpectedHistogram("OtherAlgCalls_perEvent");
  addExpectedHistogram("AlgCalls_perEvent");
  addExpectedHistogram("AlgSlot_perCall");

  addExpectedHistogram("WallTime_perEvent");

  addExpectedHistogram("AlgTime_perEvent");
  addExpectedHistogram("AlgTime_perEventFractional");

  addExpectedHistogram("FrameworkTime_perEvent");
  addExpectedHistogram("FrameworkTime_perEventFractional");

  addExpectedHistogram("UnmonitoredTime_perEvent");
  addExpectedHistogram("UnmonitoredTime_perEventFractional");


  getBaseEntry().addColumn("name", "Thread Enumerator", "Counting number assinged to thread");
  getBaseEntry().addColumn("events", "Raw Active Events", "Raw underlying statistics on number of events processed at least partially on this thread");
  getBaseEntry().addColumn("eventsWeighted", "Active Events", "Total weighted number of events processed at least partially on this thread");
  getBaseEntry().addColumn("callsPerEvent", "Alg Calls/Event", "Mean number of alg calls per processed event on this thread");
  getBaseEntry().addColumn("thisCallsPerEvent", "This Event Algs/Event", "Mean number of alg calls from the monitored event on this thread");
  getBaseEntry().addColumn("otherCallsPerEvent", "Other Event Algs/Event", "Mean number of alg calls from other events on this thread");

  getBaseEntry().addColumn("wallTimePerEvent", "Wall time/Event [ms]", "Mean Total event processing walltime per event");
  getBaseEntry().addColumn("algTimePerEvent", "Alg Time/Event [ms]", "Mean algorithm execution time per event");
  getBaseEntry().addColumn("frameworkTimePerEvent", "Framework Time/Event [ms]", "Mean time spent outside of algorithm execution per event");
  getBaseEntry().addColumn("unmonitoredTimePerEvent", "Unmonitored Time/Event [ms]", "Mean time which could not be monitored per event");

}


TableEntry ThreadOccupancyTableConstructor::getTableEntry(const std::string name) {
  TableEntry tableEntry(getBaseEntry());
  getHistograms(name);

  const float weightedEvents = hist("AlgTime_perEvent")->Integral(); // Filled once per event with event weight

  tableEntry.setEntry("name", name);
  tableEntry.setEntry("events", hist("AlgTime_perEvent")->GetEntries());
  tableEntry.setEntry("eventsWeighted", weightedEvents);
  tableEntry.setEntry("callsPerEvent", hist("AlgCalls_perEvent")->GetMean());
  tableEntry.setEntry("thisCallsPerEvent", hist("ThisAlgCalls_perEvent")->GetMean());
  tableEntry.setEntry("otherCallsPerEvent", hist("OtherAlgCalls_perEvent")->GetMean());

  tableEntry.setEntry("wallTimePerEvent", hist("WallTime_perEvent")->GetMean());
  tableEntry.setEntry("algTimePerEvent", hist("AlgTime_perEvent")->GetMean());
  tableEntry.setEntry("frameworkTimePerEvent", hist("FrameworkTime_perEvent")->GetMean());
  tableEntry.setEntry("unmonitoredTimePerEvent", hist("UnmonitoredTime_perEvent")->GetMean());

  return tableEntry;
}

void ThreadOccupancyTableConstructor::tablePostProcessing(std::vector<TableEntry>&, const float) {
}

