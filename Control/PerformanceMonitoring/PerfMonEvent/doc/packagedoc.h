/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PerfMonEvent_page PerfMonEvent Package
@author Sebastien Binet

@section PerfMonEvent_PerfMonEventIntro Introduction

This package holds the data classes modeling performance monitoring data. This performance monitoring data is gathered by the monitoring tools and later on stored into a tuple.
Each class holds information(s) about a very specialized monitoring aspect (CPU, memory,...) and is usually splitted into a header part (job-level meta-data) and a data part (holding the actual data).

@section PerfMonEvent_PerfMonEventOverview Class Overview
  The PerfMonEvent package contains the following data classes:

  - PerfMon::CpuHdr and PerfMon::CpuData : stores CPU related monitoring informations. The header stores the names (called tags) of the entities being monitored (ie: the algorithms). The data stores timings (@c PerfMon::CpuData::Timing) for each tag. These timings consist of user/sys/real CPU timings. No unit is enforced (sadly) as it is 'monitoring dependent'. It is recognized, though, that it 'might' be a source of errors and confusion: it is foreseen to enforce a unit-typed system in a near future.
  - PerfMon::IoHdr and PerfMon::IoData : stores CPU related monitoring informations for read and write (I/O) operations. The header part stores the names (called tags) of the entities being monitored (ie: the various containers and their converters).
  - PerfMon::MemHdr and PerfMon::MemData : stores memory related monitoring informations. The header stores the names (called tags) of the entities being monitored (ie: the algorithms). The data stores pairs (begin/after) of memory usage for virtual memory and RSS on a per tag basis. It also stores the overall job memory usage (vmem and RSS) on a per event basis.
  - PerfMon::PersHdr : stores informations about content hold by a POOL file. This is modeled by a @c PerfMon::PersHdr::PoolRecord class in which one can find the name of the container which has been written out, its memory and compressed sizes as well as the number of such containers one can find into the POOL file.
  - PerfMon::CfgHdr : meta data class holding information about the configuration of the job. It right now only holds the list of all top-level (ie: not the deeply nested) algorithms part of the TopSequence.
  - PerfMon::HephaestusHdr and PerfMon::HephaestusData : <i>dummies for now</i>



*/
