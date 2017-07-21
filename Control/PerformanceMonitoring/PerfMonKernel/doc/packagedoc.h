/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PerfMonKernel_page PerfMonKernel Package
@author Sebastien Binet

@section PerfMonKernel_PerfMonKernelIntro Introduction

This package provides the abstract interfaces to the (Gaudi) components (tools and services) needed and used by the performance monitoring packages.

@section PerfMonKernel_PerfMonKernelOverview Class Overview
  The PerfMonKernel package contains the following classes:

  - IPerfMonSvc : the main service organizing and scheduling a performance monitoring job. Not much is exposed to clients as it is supposed to be a rather opaque black box, but the following methods are heavily suggested to be implemented by derived classes:
     - IPerfMonSvc::pyMonitor
     - IPerfMonSvc::monitorInitialize
     - IPerfMonSvc::monitor
     - IPerfMonSvc::monitorFinalize

However, to minimize coupling between (concrete) classes, 2 methods are implemented here: @c IPerfMonSvc::shutdown( IPerfMonPyTool* ) and @c IPerfMonSvc::shutdown( IPerfMonTool* ). These methods are the last chance provided to these tools to gather metadata and clean-up the state of their performance data. After that, nothing else will be committed to disk (ie: to the tuple) or at least no change will be reflected on the persistent side. 
Note that these methods have been introduced because the meaning of finalizing a tool (in the Gaudi sense) can differ from finalizing a performance monitoring task. So they had to be decoupled.

  - IPerfMonTool : the interface to tools meant to monitor the various stages of the Gaudi finite state machine (@c IAlgorithm::initialize @c IAlgorithm::execute @c IAlgorithm::finalize ...). Hence, the following hopefully self-explanatory methods are declared:
     - @c IPerfMonTool::monitorInitialize
     - @c IPerfMonTool::monitor
     - @c IPerfMonTool::monitorFinalize

Monitoring is supposed to take place upon creation of a concrete tool implementing this @c IPerfMonTool interface, until the call to the @c IPerfMonTool::shutdown method.

 - IPerfMonPyTool : the interface to tools meant to monitor the python Configuration step, hence only one monitoring method has been declared:
     - @c IPerfMonPyTool::pyMonitor

Monitoring is supposed to take place before the creation of a concrete tool implementing this @c IPerfMonPyTool interface, by some python class or other means. The performance data is then transmitted to the tool by some jobOption property.
The assumption that performance data is in good shape and may be committed to persistent storage is nonetheless enforced by the same mechanism (ie: @c IPerfMonPyTool::shutdown)

 - ICallGraphBuilderSvc : the interface to the service building a callgraph out of the sequence of @c Algorithm. It is a rather <i>'work in progress'</i> class at the moment (June 2007) and not (yet) officially part of the toolkit of the Performance Monitoring domain. The main idea is to use the Gaudi @c IAuditor infrastructure to monitor algorithms/services and later on attach to each of the nodes of the callgraph additional informations (CPU, memory consumption,...).



*/
