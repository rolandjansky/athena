/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

@section TrigMonitorbase Introduction

There are basically 2 philosophies of writing monitoring tools for HLT.<br/>
1) - do not write any tools. Use IMonitoredAlgo interface (now every HLT algorithm has it in the inheritcance tree).
Then only TrigGenericMonioringTools has to be attached to the algorithm and histograms defined.

2) - write custom tool which is then used by algorithm directly (i.e. ToolHandle is retrieved, and tool methods are called)
This is recommended if monitred should be volatile information accessible only inside algorithms and in the limited scope of execution.
In example loop over cells dooing calibration and partial calibration results are never/nowhere stored however should be monitored.
In this case it compleetly up to user to define filling methods (i.e. can be several) and booking methods. Simply all is custom.
One is to be rememberd however that histograms should be booked during beginRun of algorithm and not deleted in finalize or endRun.
  

@section  TrigMonitorbaseContent Content
The package contains: <br/> 

TrigMonitorToolBase an interface for the
trigger monitoring tools.  If users wnats to have custo monitoring tool in
trigger; option 2) from above, (s)he should inherit from this tool.  Behind this interface
offline implementation is hidden to save trigger developers from
eventual changes there.<br/>
Added things are: prescaling, and trigger specific booking paths.


TrigHLTBaseTreeTool tool for the recording of the Lvl1Id & RoIId into
the monitoring TTrees. It becomes usefull in conjunction with TrigTimeTreeTool.
This is not meant to inherited by users but just plugged into the list of ThenaMonTools.
See more information there: 
<a href="https://twiki.cern.ch/twiki/bin/view/Atlas/TrigTimerSvc">TWIKI page about TrigTimerSvc and this tool.</a>


TrigGenericMonitoringTool this tool is used everywhere in the trigger
for recording of the monitoring variables in the form of
histograms. It is an element of option 1) explained in introduction. 
It only functions together with HLT algorithms thanks to
the IMonitoredAlgo interface defined in TrigInterfaces package.  Usage
of this tool is limited to setting proper configuration. There are
pythons helpers for that python::TrigGenericMonitoringToolConfig and
helper function to define histograms:
python::TrigGenericMonitoringToolConfig::defineHistogram.

<a href="https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerValidationHistograms">TWIKI page about Validation intrumentation for TrigGenericMonitoringTool.</a>



@htmlinclude used_packages.html

@include requirements

*/
