/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_Monitoring_page SCT_MonitoringPackage
@author Martin White, Helen Hayward, Shaun Roe

 
@section SCT_Monitoring_SCT_MonitoringIntro Introduction
This package does data monitoring at the event filter level, providing a tool which can give occupancies and efficiencies for the SCT detector.
The histograms are published and may be used in the Online Histogram Presenter (OHP).
 
@section SCT_Monitoring_SCT_MonitoringOverview Class Overview
All of the classes used directly for monitoring inherit directly from the ManagedMonitorToolBase to enable histogram registration and use in the Online Presenter.
Some of the classes in this package are only intended to be compiled when integrating into the online environment, look out for the 'ifdef ONLINE' preprocessor directive.

Here are the active Monitoring Tools:

   - SCTTracksMonTool
   
   - SCTHitEffMonTool
   
   - SCTHitsNoiseMonTool
   
   - SCTErrMonTool
   
   - SCTInitialiseTool
   
   - SCTHitEfficiencyTool
   
   - SCTEffMonTool
   
The following are only used in the online environment:
   - SCTListener
   
   - Martin.hh (automatically generated for online code integration)
   
The following are utility classes used in the package:
   - FSCTtoolbox
   
   - PairBuilder : Builds all possible pairs of given sets of values for x and y, used for correlation plotting
   
The following <i>may</i> be used in other classes but should probably be deprecated:
   - SCT_ModuleStatistics
   - SCT_PlanePosition
   - SCT_ClusterStruct
   - SCT_StatisticsData

These should definitely be deprecated and removed as soon as possible:
   - StatusCodeAction
   - SCT2004MonTool
   - SCTCosmicHisto
   - SCTCosmicMonTool
   - ParsedSCT_RDO
   - SCTHistoTest
   
 
 */
