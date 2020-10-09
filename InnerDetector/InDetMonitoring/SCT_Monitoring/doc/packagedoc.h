/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_Monitoring_page SCT_MonitoringPackage
@author Martin White, Helen Hayward, Shaun Roe, Susumu.Oda@cern.ch

 
@section SCT_Monitoring_SCT_MonitoringIntro Introduction
This package does data monitoring providing algorithms and tools which can give bytestream errors, efficiencies, noises, occupancies, Lorentz angles, tracking quantities for the SCT detector for both online and offline.
The histograms are published and used in the Online Histogram Presenter (OHP).
 
@section SCT_Monitoring_SCT_MonitoringOverview Class Overview
In the new data quality motitoring framework for Run 3, algorithms inherited directly from AthMonitorAlgorithm are used.

In the old data quality monitoring framework up to Run 2, tools inherited directly from ManagedMonitorToolBase are used.
Tools will be removed at some point.

Here are the active monitoring algorithms / tools:

   - SCTErrMonAlg / SCTErrMonTool : bytestream errors
   - SCTHitEffMonAlg / SCTHitEffMonTool : efficiencies
   - SCTHitsNoiseMonAlg / SCTHitsNoiseMonTool : noises, occupancies
   - SCTLorentzMonAlg / SCTLorentzMonTool : Lorentz angles
   - SCTTracksMonAlg / SCTTracksMonTool : tracking quantities

@section SCT_Monitoring_SCT_MonitoringConfiguration Configuration
@subsection SCT_Monitoring_SCT_MonitoringConfigurationNewNew Configuration in the new DQ framework and the new job configuration framework

InnerDetector/InDetMonitoring/SCT_Monitoring/python/SCTMonitoringConfig.py is the main configuration script.
Only SCTLorentzMonAlg and SCTTracksMonAlg are configured in the script using SCTLorentzMonAlg.py and SCTTracksMonAlg.py because the Inner Dector configuration is not fully ready in the new job configuration framework as of October, 2020.

When the Reco_tf.py command is used, the chain is the following:
- Reconstruction/RecJobTransforms/scripts/Reco_tf.py
- Reconstruction/RecJobTransforms/share/skeleton.RAWtoESD_tf.py / Reconstruction/RecJobTransforms/share/skeleton.RAWtoALL_tf.py
- Reconstruction/RecExample/RecExCommon/share/RecExCommon_topOptions.py
- Control/AthenaMonitoring/share/DataQualitySteering_jobOptions.py
- Control/AthenaMonitoring/python/AthenaMonitoringCfg.py
- InnerDetector/InDetMonitoring/SCT_Monitoring/python/SCTMonitoringConfig.py

Individual algorithms can be run using the configuration scripts like:

\verbatim
python -m SCT_Monitoring.SCTLorentzMonAlg
\endverbatim

@subsection SCT_Monitoring_SCT_MonitoringConfigurationNewOld Configuration in the new DQ framework and the old job configuration framework

InnerDetector/InDetExample/InDetRecExample/share/InDetMonitoringSCT.py is the main configuration script.
To use the new DQ framework, the useNewAlgs flag in the script has to be set True.
The default value is True.

When the Reco_tf.py command is used, the chain is the same as above up to DataQualitySteering_jobOptions.py. The following chain is:
- Control/AthenaMonitoring/share/DataQualitySteering_jobOptions.py
- InnerDetector/InDetExample/InDetRecExample/share/InDetMonitoring.py
- InnerDetector/InDetExample/InDetRecExample/share/InDetMonitoringSCT.py

@subsection SCT_Monitoring_SCT_MonitoringConfigurationOldOld Configuration in the old DQ framework and the old job configuration framework

InnerDetector/InDetExample/InDetRecExample/share/InDetMonitoringSCT.py is the main configuration script too.
To use the new DQ framework, the useNewAlgs flag in the script has to be set False.
The default value is True.
The chain is the same as above.

 */
