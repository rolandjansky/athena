/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "../../TrigCostAnalysis/TableConstructors/ROSTableConstructor.h"

#include "TH1.h"

ROSTableConstructor::ROSTableConstructor(const std::string& name) : TableConstructorBase(name) {
 
  addExpectedHistogram("Request_perEvent");
  addExpectedHistogram("NetworkRequest_perEvent");
  addExpectedHistogram("CachedROBSize_perEvent");
  addExpectedHistogram("NetworkROBSize_perEvent");
  addExpectedHistogram("Time_perEvent");
  addExpectedHistogram("ROBStatus_perCall");

  getBaseEntry().addColumn("name", "Name", "ROS name");
  getBaseEntry().addColumn("events", "Raw Active Events", "Raw underlying statistics on the number of events in which this ROS was accessed.");
  getBaseEntry().addColumn("eventsWeighted", "Active Events", "How many events in which this sequence was executed.");
  getBaseEntry().addColumn("requestRate", "Data Requests Rate [Hz]", "Rate of ROS access requests each may contain many ROB reads.");
  getBaseEntry().addColumn("networkRequestRate", "Retrieved Data Requests Rate [Hz]", "Rate of ROS access requests that fetch data from the ROBs.");
  getBaseEntry().addColumn("retrievedSizeRate", "Retrieved ROB Data Rate [kB/s]", "Amount of data fetched from the ROBs in kB/s.");
  getBaseEntry().addColumn("cachedSizeRate", "Cached ROB Data Rate [kB/s]", "Amount of cached data requested from the ROBs in kB/s.");
  getBaseEntry().addColumn("time", "Time Per Event [ms]", "Average time for all requests and retrievals per event.");
  getBaseEntry().addColumn("robsUnclassified", "Unclassified ROBs/Event", "ROB calls which were flagged unclassified.");
  getBaseEntry().addColumn("robsRetrieved","Retrieved ROBs/Event","Total number of fetched ROB calls.");
  getBaseEntry().addColumn("robsHLTCached","Cached HLT ROBs/Event","Total number of HLT cached ROB calls.");
  getBaseEntry().addColumn("robsDCMCached","Cached DCM ROBs/Event","Total number of DCM cached ROB calls.");
  getBaseEntry().addColumn("robsIgnored", "Ignored ROBs/Event", "ROB calls which were flagged as ignored.");
  getBaseEntry().addColumn("robsUndefined", "Undefined ROBs/Event", "ROB calls which were not enabled.");
  getBaseEntry().addColumn("robsNotOk","Not OK ROBs/Event", "ROB calls in which the is OK bit was false.");

}

TableEntry ROSTableConstructor::getTableEntry(const std::string name) {
  TableEntry tableEntry(getBaseEntry());
  getHistograms(name);

  tableEntry.setEntry("name", name);
  tableEntry.setEntry("events", hist("Request_perEvent")->GetEntries());
  tableEntry.setEntry("eventsWeighted", hist("Request_perEvent")->Integral());
  tableEntry.setEntry("requestRate", histGetXWeightedIntegral("Request_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("networkRequestRate", histGetXWeightedIntegral("NetworkRequest_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("retrievedSizeRate", histGetXWeightedIntegral("NetworkROBSize_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("cachedSizeRate", histGetXWeightedIntegral("CachedROBSize_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("time", hist("Time_perEvent")->GetMean());

  tableEntry.setEntry("robsUnclassified", hist("ROBStatus_perCall")->GetBinContent(1));
  tableEntry.setEntry("robsRetrieved", hist("ROBStatus_perCall")->GetBinContent(2));
  tableEntry.setEntry("robsHLTCached", hist("ROBStatus_perCall")->GetBinContent(3));
  tableEntry.setEntry("robsDCMCached", hist("ROBStatus_perCall")->GetBinContent(4));
  tableEntry.setEntry("robsIgnored", hist("ROBStatus_perCall")->GetBinContent(5));
  tableEntry.setEntry("robsUndefined", hist("ROBStatus_perCall")->GetBinContent(6)); 
  tableEntry.setEntry("robsNotOk", hist("ROBStatus_perCall")->GetBinContent(7)); 

  return tableEntry;
}

void ROSTableConstructor::tablePostProcessing(std::vector<TableEntry>& tableEntries, const float walltime) {

  for (TableEntry& te : tableEntries) {
    te.normaliseEntry("requestRate", walltime);
    te.normaliseEntry("networkRequestRate", walltime);
    te.normaliseEntry("retrievedSizeRate", walltime);
    te.normaliseEntry("cachedSizeRate", walltime);
  }
}