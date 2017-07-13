/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PerfMonAna_page PerfMonAna Package
@author Sebastien Binet

@section PerfMonAna_PerfMonAnaIntro Introduction

This package contains python classes and python script to analyze tuple files produced by the PerfMonComps package (actually the @c PerfMonSvc and its tools).

@section PerfMonAna_PerfMonAnaOverview Class Overview
  The PerfMonAna package contains the following classes:

  - python::PerfMonProcessing::AnaMgr : the main class driving the analyzer script. It takes a few input filenames (reference and current) and delegates to @c Ana to create and fill histograms. Eventually it gathers and merges all the histograms, prints them into a PDF file and saves them into a separate output ROOT file.

  - python::PerfMonProcessing::Ana : the main analysis class. It aggregates a few specialized analysis classes which look at some quantities related to their domain of expertise (CPU, memory,...). Each of these little workers create, fill and fit histograms which are later on published to the holding @c Ana class.

  - python::Analyzer::Analyzer : the base class for all the analysis objects. It factorizes the interface of a basic analysis of performance data (@c run) and provides hooks for derived classes to specialize the analysis (@c initialize, @c fillHistos and @c fitHistos)

  - python::CpuAnalyzer::CpuAnalyzer : analyzer working on CPU related quantities. It reads the perfmon tuple and extracts user/sys/real CPU timings for the whole Configuration step, the initialize and finalize steps (on a per-algorithm basis) and the execute step (on a per-event and per-algorithm basis).

  - python::IoAnalyzer::IoAnalyzer : analyzer working on I/O related quantities. It reads the perfmon tuple and extracts user+sys CPU timings for each of the 'registered' T/P converters (for T->P and P->T transformations) as well as the overall @c commitOutput timing (from the @c AthenaPoolCnvSvc class) which represent the user CPU time needed to commit transient data on disk.

  - python::MemAnalyzer::MemAnalyzer : analyzer working on memory related quantities. It reads the perfmon tuple and extracts virtual memory and resident set size memory consumptions for the whole Configuration step (at jobOption-include boundaries), the initialize, finalize and execute steps (on a per-algorithm and per-event basis).

The PerfMonAna package also provides a few handy utilities related to management of (Py)Root: importing ROOT without graphics, configuring a less ugly ROOT style, helper classes to setup histogram styles (color, line-style, marker-style, ...)

@section PerfMonAna_PerfMonAnaPyRootScript perfmon.py
  This package also provides a 'drop-in' PyRoot script that one can run on any perfmon tuple to produce PDF and ROOT files containing the histograms created by all the above analyzers: @c bin/perfmon.py.
It is fairly easy to use it, just launch in your shell:
@code
 lxplus> perfmon.py my.perfmon.root
@endcode

Informations about various switches, options and usage are available via:
@code
 lxplus> perfmon.py --help
@endcode




*/
