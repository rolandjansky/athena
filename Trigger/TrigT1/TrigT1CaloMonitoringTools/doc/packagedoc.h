/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigT1CaloMonitoringTools_page 

Tools used by the packages TrigT1Calomonitoring and TrigT1Monitoring.  See also the documentation for those packages.

<hr><b>Athena Tools</b>                                          <hr><p>
TrigT1CaloLWHistogramTool <p> @copydoc TrigT1CaloLWHistogramTool <hr><p>
TrigT1CaloMonErrorTool    <p> @copydoc TrigT1CaloMonErrorTool    <p>

<hr><b>Other Tools</b>                                           <hr><p>

<b>JobOption flags for steering the stability monitoring online:</b>

<table>
<tr><th> Flag                                           </th><th> Use For                                              </th></tr>
<tr><td> @c LVL1CaloMonFlags::doPPrStabilityMon         </td><td> @copydoc LVL1CaloMonFlags::doPPrStabilityMon         </td></tr>
<tr><td> @c LVL1CaloMonFlags::doFineTimeMonitoring      </td><td> @copydoc LVL1CaloMonFlags::doFineTimeMonitoring      </td></tr>
<tr><td> @c LVL1CaloMonFlags::doPedestalMonitoring      </td><td> @copydoc LVL1CaloMonFlags::doPedestalMonitoring      </td></tr>
<tr><td> @c LVL1CaloMonFlags::doEtCorrelationMonitoring </td><td> @copydoc LVL1CaloMonFlags::doEtCorrelationMonitoring </td></tr>
</table>

Note: These flags are not designed for use on Tier0.

<hr>

<b>Miscellaneous Utilities:</b>

<table>
<tr><th> Script                   </th><th> Use For                                     </th></tr>
<tr><td> @c scripts/findEventFile </td><td> Finding which file a particular event is in </td></tr>
</table>

You'll need to check out the package to access these.
For more information see the documentation for TrigT1CaloMonitoring.

<hr>



*/
