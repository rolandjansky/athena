/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PerfMonComps_page PerfMonComps Package
@author Sebastien Binet

@section PerfMonComps_PerfMonCompsIntro Introduction

This package holds the Gaudi components used to achieve Performance Monitoring of Athena jobs.
It ships the concrete implementations of interfaces exported from the PerfMonKernel package.

@section PerfMonComps_PerfMonCompsOverview Class Overview
  The PerfMonComps package contains the following classes:

  - PerfMonSvc : the main central service. It orchestrates the scheduling of the monitoring of the job by delagating to the tools it holds (via a @c ToolHandleArray). This is performed at each step of the Gaudi finite state machine (initialize/execute/finalize) by means of registering callback functions via the @c IIncidentSvc::handle method, dispatching to the relevant monitoring method. This service is also responsible for calling the serialization to disk of the performance monitoring data. It does so by delagating to the @c PerfMon::TupleSvc. The @c PerfMonSvc is configured through a handwritten @c python::Configurable::ConfigurableService python class.
  - PerfMon::TupleSvc : this is the service responsible for writing performance data to disk. It currently uses the Gaudi @c ITHistSvc to do so, thus any class for which a @c Reflex dictionary exists should be correctly handled.
  - PerfMon::CpuTool : a tool to monitor CPU usage of algorithms. It internally uses the Gaudi @c IChronoStatSvc to retrieve user/sys/real CPU timing measurements on a per algorithm basis (all of them or the ones registered through a jobOptions property) and on a 'per-slice' basis (ie: the @c initialize slice, the @c execute slice, ...). It uses the @c PerfMon::CpuHdr and @c PerfMon::CpuData classes to store this monitoring data.
  - PerfMon::IoTool : a tool to monitor CPU usage of T/P converters. It internally uses the Gaudi @c IChronoStatSvc to retrieve user/sys/real CPU timing measurements on a per converter and container basis (reading and writing). It also monitors the overall @c AthenaPoolCnvSvc::commitOutput time (the time to write on disk the whole event). <b>Note</b> that it relies on a well configured @c AthenaPoolCnvSvc, that is, a service being configured to audit the T/P converters (this should be automatically taken care of during the configuration of @c PerfMonSvc). It uses the @c PerfMon::IoHdr and @c PerfMon::IoData classes to store this monitoring data.
  - PerfMon::MemTool : a tool to monitor memory usage of algorithms. It internally uses the Gaudi @c IChronoStatSvc to retrieve virtual memory and resident set size usage before and after calling a given algorithm method (initialize/execute/finalize). <b>Note</b> that it can be very easily fooled by the @c StoreGateSvc memory management. It uses the @c PerfMon::MemHdr and @c PerfMon::MemData classes to store this monitoring data.
  - PerfMon::LdTool : a tool to monitor 'library usage'. This tool retrieves the list of all the shared libraries loaded, and prints the size of this list. Not very useful right now (apart from displaying scarying numbers), but it will eventually build the dependency graph of the libraries to spot suspicious runtime dependencies.
  - PerfMon::PersTool : a tool to monitor persistent classes. This tool uses bare ROOT libraries to open and inspect the output POOL file of all the OutputStream known to the (Athena) @c ApplicationManager. It is the C++ cousin of checkFile.py. It uses the @c PerfMon::PersHdr class to store this monitoring data.
  - PerfMon::PyCfgTool : a tool to monitor the jobOption Configuration step. This tool retrieves CPU and memory monitoring data which took place during the jobOptions Configuration. Obviously, as this (C++) tool does not exist at that time (during python configuration), it only retrieves data through a job property, which is itself filled by a python class monitoring the include of each jobOptions file.
  - PerfMon::HephaestusTool : a tool extracting monitoring data from Hephaestus memory leaks tracker. Dummy for the moment.
  - PerfMonAuditor : an auditor used by various performance monitoring tools to provide the @c IChronoStatSvc with memory informations. This auditor open ups the /proc/[pid]/statm file to get vmem and RSS informations.
  - PerfMon::CallGraphBuilderSvc : a <i>work in progress</i> service, meant to build a callgraph structure out of the sequence of algorithms and attach (performance monitoring) informations at each edge. Not yet in production.
  - PerfMon::CallGraphAuditor : a <i>work in progress</i> auditor, meant to notify the @c PerfMon::CallGraphBuilderSvc when (new) nodes of the callgraph structure should be added/updated. Not yet in production.


*/
