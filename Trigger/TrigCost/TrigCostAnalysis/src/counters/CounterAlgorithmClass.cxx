/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CounterAlgorithmClass.h"

CounterAlgorithmClass::CounterAlgorithmClass(const std::string& name, const MonitorBase* parent) 
  : CounterAlgorithm(name, parent, true)
{
  regHistogram("Time_perCall", "CPU Time/Call;Time [ms];Calls", VariableType::kPerCall, kLog, 0.01, 100000);
  regHistogram("FirstTime_perEvent", "First Call CPU Time;Time [ms];Events", VariableType::kPerCall, kLog, 0.01, 100000);
  regHistogram("Time_perEvent", "CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("Time_perEventFractional", "CPU Time/Event CPU Time;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("AlgCalls_perEvent", "Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 1499.5, 1500);
  regHistogram("InEventView_perCall", "In Event View;Yes or No;Calls", VariableType::kPerCall, kLinear, -0.5, 1.5, 2);
  regHistogram("RoIID_perCall", "RoI ID;RoI ID;Calls", VariableType::kPerCall, kLinear, -1.5, 20.5, 22);
  regHistogram("Request_perEvent", "Number of requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 299.5, 300);
  regHistogram("NetworkRequest_perEvent", "Number of network requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 299.5, 300);
  regHistogram("CachedROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("NetworkROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("RequestTime_perEvent", "ROB Elapsed Time/Event;Elapsed Time [ms];Events", VariableType::kPerEvent);
}

