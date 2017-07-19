/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigT1Monitoring_page 

<b>Contents</b>
- @subpage Intropage
- @subpage Toolpage
  - @ref Calosection
  - @ref CTPsection
  - @ref L2section
- @subpage OtherInfopage
  - @ref Rejectsection
  - @ref Streamsection
  - @ref MergesUsedsection

*/

/**

@page Intropage General Introduction

The L1Calo Athena monitoring comprises the packages TrigT1CaloMonitoring,
TrigT1Monitoring, TrigT1CaloMonitoringTools and a few classes from
TrigT1CaloCalibTools.  TrigT1CaloMonitoring contains the monitoring of
L1Calo sub-detectors, TrigT1Monitoring contains monitoring of L1Calo
interfaces with other sub-detectors (Calo, CTP, L2),
TrigT1CaloMonitoringTools contains helper tools and scripts, and
TrigT1CaloCalibTools contains parts of the Stability monitoring which
are also used by standalone analysis jobs (in TrigT1CaloCalibUtils).

The code is used for both online and Tier0 monitoring although there
may be differences between the two.
Standard JobOptions for TrigT1Monitoring are TrigT1Monitoring_forRecExCommission.py.

The tools use Light Weight Histograms.
See also the documentation for TrigT1CaloMonitoring.

*/

/**

@page Toolpage Monitoring Tools in the Package
                                                         <hr>
@section TrigT1Monitoring_Calosection L1Calo/Calorimeter Monitoring Tools
                                                         <hr><p>
  CalorimeterL1CaloMon <p> @copydoc CalorimeterL1CaloMon <hr>
  L1CaloHVScalesMon    <p> @copydoc L1CaloHVScalesMon    <hr>
  L1CaloPMTScoresMon   <p> @copydoc L1CaloPMTScoresMon   <hr>

@section TrigT1Monitoring_CTPsection L1Calo/CTP Monitoring Tool
                                                         <hr><p>
  L1CaloCTPMon         <p> @copydoc L1CaloCTPMon         <hr><p>

@section TrigT1Monitoring_L2section L1Calo/L2 Monitoring Tool
                                                         <hr><p>
  L1CaloLevel2Mon      <p> @copydoc L1CaloLevel2Mon      <hr><p>

*/

/**

@page OtherInfopage Other Information

@section TrigT1Monitoring_Rejectsection Event Rejection Criteria

  An event can be omitted from the monitoring for a number of reasons.
  The following table shows the main ones and which monitoring tools
  are affected.

  <table>
  <tr><th> Monitoring Tool         </th><th> Corrupt </th><th> ROB Errors </th><th> LAr Noise Bursts </th></tr>
  <tr><td> @c CalorimeterL1CaloMon </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c L1CaloHVScalesMon    </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c L1CaloPMTScoresMon   </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c L1CaloCTPMon         </td><td>   Yes   </td><td>    Yes     </td><td>        -         </td></tr>
  <tr><td> @c L1CaloLevel2Mon      </td><td>   Yes   </td><td>    Yes     </td><td>        -         </td></tr>
  </table>

  See the documentation for TrigT1CaloMonErrorTool in TrigT1CaloMonitoringTools
  for the definition of Corrupt.

  ROB Errors means any ROB status errors or bytestream unpacking errors.
  Since these cause the ROB fragment to be skipped they almost inevitably
  mean there will be mismatches in the data/simulation cross-checks.

  <hr>

@section TrigT1Monitoring_Streamsection Event Stream Restrictions

  Not all monitoring is needed or makes sense on all streams on Tier0 or online.
  The following table shows which tools are run on which streams.  The selection
  is made in the jobOptions.

  <table>
  <tr><th> Monitoring Tool         </th><th> express </th><th> Egamma </th><th> JetTauEtmiss </th><th> Muons </th><th> Other </th><th> Online </th></tr>
  <tr><td> @c CalorimeterL1CaloMon </td><td>   Yes   </td><td>   -    </td><td>     Yes      </td><td>   -   </td><td>   -   </td><td>  Yes   </td></tr>
  <tr><td> @c L1CaloHVScalesMon    </td><td>   Yes   </td><td>   -    </td><td>     Yes      </td><td>   -   </td><td>   -   </td><td>  Yes   </td></tr>
  <tr><td> @c L1CaloPMTScoresMon   </td><td>   Yes   </td><td>   -    </td><td>     Yes      </td><td>   -   </td><td>   -   </td><td>  Yes   </td></tr>
  <tr><td> @c L1CaloCTPMon         </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c L1CaloLevel2Mon      </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  </table>

  <hr>

@section TrigT1Monitoring_MergesUsedsection Custom Merges Used in TrigT1Monitoring

  <table>
  <tr><th> Merge Method        </th><th> Description from DataQualityUtils      </th></tr>
  <tr><td> @c eventSample      </td><td> Merge histograms containing, for example,
                                         event numbers of events with particular
					 types of errors.                       </td></tr>
  <tr><td> @c lowerLB          </td><td> Merge "status" histograms,
                                         i.e filled at start of run/LB.         </td></tr>
  </table>

*/
