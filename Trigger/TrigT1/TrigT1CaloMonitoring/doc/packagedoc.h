/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigT1CaloMonitoring_page 

<b>Contents</b>
- @subpage Intropage
- @subpage Toolpage
  - @ref Globalsection
  - @ref PPMsection
  - @ref CPMsection
  - @ref JEMsection
  - @ref RODsection
- @subpage Rejectionpage
  - @ref Rejectsection
  - @ref Streamsection
- @subpage Testingpage
  - @ref Oldsection
  - @ref Newsection
  - @ref Onlinesection
  - @ref CPUsection
- @subpage Namingpage
- @subpage Mergepage
  - @ref Mergesection
  - @ref MergesUsedsection
  - @ref PostProcessingsection
- @subpage RoIAsymmetrypage
  - @ref CPMRoIAsymmetrysection
  - @ref JEMRoIAsymmetrysection
- @subpage ErrorEventspage

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
are some differences between the two.
Standard JobOptions for TrigT1CaloMonitoring are
TrigT1CaloMonitoring_forRecExCommission.py.

The tools use Light Weight Histograms apart from the global error tool
which uses ROOT histograms as it needs the ROOT Merge method which is
not supported by Light Weight Histograms.

*/

/**

@page Toolpage Monitoring Tools in the Package
                                                               <hr>
@section TrigT1CaloMonitoring_Globalsection Global Monitoring Tool
                                                               <hr><p>
  TrigT1CaloGlobalMonTool <p> @copydoc TrigT1CaloGlobalMonTool <hr>

@section TrigT1CaloMonitoring_PPMsection Preprocessor Monitoring Tools
                                                               <hr><p>
  PPrMon                  <p> @copydoc PPrMon                  <hr><p>
  PPrSpareMon             <p> @copydoc PPrSpareMon             <hr><p>
  PPMSimBSMon             <p> @copydoc PPMSimBSMon             <hr><p>
  PPrStabilityMon         <p> @copydoc PPrStabilityMon         <hr>

@section TrigT1CaloMonitoring_CPMsection Cluster Processor Monitoring Tools
                                                               <hr><p>
  TrigT1CaloCpmMonTool    <p> @copydoc TrigT1CaloCpmMonTool    <hr><p>
  CPMSimBSMon             <p> @copydoc CPMSimBSMon             <hr><p>
  EmEfficienciesMonTool   <p> @copydoc EmEfficienciesMonTool   <hr>

@section TrigT1CaloMonitoring_JEMsection Jet-Energy Processor Monitoring Tools
                                                               <hr><p>
  JEMMon                  <p> @copydoc JEMMon                  <hr><p>
  CMMMon                  <p> @copydoc CMMMon                  <hr><p>
  JEPSimBSMon             <p> @copydoc JEPSimBSMon             <hr><p>
  JetEfficienciesMonTool  <p> @copydoc JetEfficienciesMonTool  <hr>

@section TrigT1CaloMonitoring_RODsection ROD Monitoring Tool
                                                               <hr><p>
  TrigT1CaloRodMonTool    <p> @copydoc TrigT1CaloRodMonTool

*/

/**

@page Rejectionpage Event Rejection Criteria and Stream Restrictions

@section TrigT1CaloMonitoring_Rejectsection Event Rejection Criteria

  An event can be omitted from the monitoring for a number of reasons.
  The following table shows the main ones and which monitoring tools
  are affected.

  <table>
  <tr><th> Monitoring Tool            </th><th> Corrupt </th><th> ROB Errors </th><th> LAr Noise Bursts </th></tr>
  <tr><td> @c TrigT1CaloGlobalMonTool </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c PPrMon                  </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c PPrSpareMon             </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c PPMSimBSMon             </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c PPrStabilityMon         </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c TrigT1CaloCpmMonTool    </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c CPMSimBSMon             </td><td>   Yes   </td><td>    Yes     </td><td>        -         </td></tr>
  <tr><td> @c EmEfficienciesMonTool   </td><td>   Yes   </td><td>     -      </td><td>       Yes        </td></tr>
  <tr><td> @c JEMMon                  </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c CMMMon                  </td><td>   Yes   </td><td>     -      </td><td>        -         </td></tr>
  <tr><td> @c JEPSimBSMon             </td><td>   Yes   </td><td>    Yes     </td><td>        -         </td></tr>
  <tr><td> @c JetEfficienciesMonTool  </td><td>   Yes   </td><td>     -      </td><td>       Yes        </td></tr>
  <tr><td> @c TrigT1CaloRodMonTool    </td><td> Yes (1) </td><td>     -      </td><td>        -         </td></tr>
  </table>

  (1) The errors causing the event rejection are histogrammed in the ROD
  directory tree.

  See the documentation for TrigT1CaloMonErrorTool in TrigT1CaloMonitoringTools
  for the definition of Corrupt.

  ROB Errors means any ROB status errors or bytestream unpacking errors.
  Since these cause the ROB fragment to be skipped they almost inevitably
  mean there will be mismatches in the data/simulation cross-checks.

  <hr>

@section TrigT1CaloMonitoring_Streamsection Event Stream Restrictions

  Not all monitoring is needed or makes sense on all streams on Tier0 or online.
  The following table shows which tools are run on which streams.  The selection
  is made in the jobOptions.

  <table>
  <tr><th> Monitoring Tool            </th><th> express </th><th> Egamma </th><th> JetTauEtmiss </th><th> Muons </th><th> Other </th><th> Online </th></tr>
  <tr><td> @c TrigT1CaloGlobalMonTool </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c PPrMon                  </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c PPrSpareMon             </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c PPMSimBSMon             </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c PPrStabilityMon         </td><td>   Yes   </td><td>   -    </td><td>      -       </td><td>   -   </td><td>   -   </td><td>  Yes (1) </td></tr>
  <tr><td> @c TrigT1CaloCpmMonTool    </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c CPMSimBSMon             </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c EmEfficienciesMonTool   </td><td>   Yes   </td><td>   -    </td><td>     Yes      </td><td>  Yes  </td><td>   -   </td><td>   -    </td></tr>
  <tr><td> @c JEMMon                  </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c CMMMon                  </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c JEPSimBSMon             </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  <tr><td> @c JetEfficienciesMonTool  </td><td>   Yes   </td><td>  Yes   </td><td>      -       </td><td>  Yes  </td><td>   -   </td><td>   -    </td></tr>
  <tr><td> @c TrigT1CaloRodMonTool    </td><td>   Yes   </td><td>  Yes   </td><td>     Yes      </td><td>  Yes  </td><td>  Yes  </td><td>  Yes   </td></tr>
  </table>

  (1) Online PPrStabilityMon is run separately as it produces many thousands of histograms.
  Tier0 runs a much stripped down version.

*/

/**

@page Namingpage Histogram Naming Convention

  Attributes to include in histogram names in order of usage:
    -# cpm/cmm/jem/ppm/rod
    -# em/had/layerSum
    -# 1d/2d/
    -# etaPhi
    -# tt/jetEl/roi/thresh
    -# adc/lut (ppm only)
    -# descriptive name

  separated by "_".  Omit any attribute that doesn't apply.

  Devised by Taylor Childers May 2009

*/

/**

@page Testingpage Testing the Code

@section TrigT1CaloMonitoring_Oldsection Notes from Feb 2010

  The easiest way to run the monitoring nowadays is probably just to run the
  transform used for production at Tier0 - @c Reco_trf.  You have to run this
  anyway before you are allowed to submit code to the Tier0 cache.  The
  advantage of this is that it almost always works out of the box in any recent
  release and you are running it exactly as it will be run on Tier0.  The
  disadvantage is that by default you are running everything so it takes longer
  to run.  You can turn things off to speed it up but often the same jobOptions
  won't work when you move from one release to the next so I don't bother trying
  any more.

  Example use of @c Reco_trf:
  @code
  Reco_trf.py inputBSFile=rawDataFilename outputESDFile=myESD.pool.root \
              outputAODFile=myAOD.pool.root HIST=myHIST.root \
	      autoConfiguration=everything maxEvents=200
  @endcode

  This runs the complete Tier0 processing chain.  The important parts for us
  are RAWtoESD, ESDtoAOD and Histogram Merging.  The latest version of
  TrigT1CaloMonitoring runs some monitoring in the RAWtoESD step and some in
  the ESDtoAOD step which is then merged together.  Older versions ran
  everything in the RAWtoESD step but we have been asked to move as much as
  possible to the ESDtoAOD step.
  I've never tried running on the Grid as it's not necessary for testing.
  I usually use our batch farm here at Birmingham.
  I recommend using the latest release possible, I'm using
  AtlasProduction-15.6.4.1 which is the current Tier0 release, I believe.
  The latest tags of TrigT1CaloMonitoring and TrigT1Monitoring will work
  with this.
  <hr>

@section TrigT1CaloMonitoring_Newsection Update Feb 2013

  The monitoring has been moved back to the RAWtoESD step to avoid reading a
  large database folder in both steps.  But note that the jobOptions are still
  called in both steps so still need to cater for both.

  Latest suggested test job:
  @code
  Reco_trf.py inputBSFile=rawDataFilename --ignoreerrors=True conditionsTag='COMCOND-ES1PA-006-05' \
              autoConfiguration='everything' maxEvents=200 outputESDFile=myESD.pool.root \
	      --omitvalidation=ALL --test outputHISTFile=myHIST.root
  @endcode

  You can find out the current version and job being run on Tier0 by looking
  on the DQ web pages for Tier0 monitoring.  If you click on the tag next
  to the run number it will give you various information including the Atlas
  release used.  To get the actual job parameters use @c GetCommand.py:
  @code
  GetCommand.py AMI=x250
  @endcode
  where @c x250 is the first part of the tag on the DQ page.
  You may need to do:
  @code
  voms-proxy-init -voms atlas
  @endcode
  first to access AMI.

  Before requesting a tag for Tier0 you should test with the latest cache or
  nightly and run these three jobs:
  @code
  Reco_trf.py AMI=q120
  Reco_trf.py AMI=q121
  Reco_trf.py AMI=q122
  @endcode

  If you are running these jobs in an environment that can't access AMI then
  use @c GetCommand.py to get the job parameters you need.
  Check the outputs carefully particularly for the RAWtoESD step.

  <hr>

@section TrigT1CaloMonitoring_Onlinesection Testing Online-specific Code

  The tools which contain online-specific code have a property
  @c OnlineTest which if set to true makes the tool run as if it was
  online even when offline.  (Exception: PPrStabilityMon.)

  <hr>

@section TrigT1CaloMonitoring_CPUsection Monitoring CPU Time

  For Tier0 monitoring it is important to keep CPU and memory usage as 
  low as possible.  To help with this an alternative jobOptions is provided
  which runs every L1Calo monitoring tool in a separate manager so that
  the CPU usage of each tool is given at the end of the
  @c Reco_trf.py job log.  See TrigT1CaloMonitoring_forRecExCommission_cpu.py
  (and TrigT1Monitoring_forRecExCommission_cpu.py for TrigT1Monitoring).

  The following table shows the cpu usage of each tool as a percentage of
  the total L1Calo cpu.  The express stream runs all tools so gives times
  for all of them.  The overall column estimates the contribution of each
  tool for all streams (ES1 and BLK) taking into account numbers of events
  and which streams the tools run in.  Run 215643 and release 17.7.0.2
  together with TrigT1CaloByteStream-00-08-17, TrigT1CaloMonitoring-00-14-06,
  TrigT1CaloMonitoringTools-00-02-01, TrigT1Monitoring-00-05-00 and
  TrigT1CaloCalibTools-00-05-14 were used for this.

  <table>
  <tr><th> Manager             </th><th> Tool(s)                        </th><th> % cpu <br> express    </th><th> % cpu <br> overall    </th></tr>
  <tr><td> L1CaloMonManager0A1 </td><td> Bytestream Unpacking PPM (1)   </td><td><center>  6.7 </center></td><td><center> 12.0 </center></td></tr>
  <tr><td> L1CaloMonManager0A2 </td><td> Bytestream Unpacking CPM (1)   </td><td><center>  1.2 </center></td><td><center>  2.2 </center></td></tr>
  <tr><td> L1CaloMonManager0A3 </td><td> Bytestream Unpacking JEM (1)   </td><td><center>  1.2 </center></td><td><center>  2.2 </center></td></tr>
  <tr><td> L1CaloMonManager0A4 </td><td> Bytestream Unpacking ROD (1)   </td><td><center>  0.2 </center></td><td><center>  0.4 </center></td></tr>
  <tr><td> L1CaloMonManager0B  </td><td> L1CaloMonitoringCaloTool (2)   </td><td><center> 31.0 </center></td><td><center> 18.5 </center></td></tr>
  <tr><td> L1CaloMonManager1A  </td><td> PPrStabilityMon /FineTime      </td><td><center>  2.6 </center></td><td><center>  0.1 </center></td></tr>
  <tr><td> L1CaloMonManager1B  </td><td> PPrStabilityMon /Pedestal      </td><td><center>  5.2 </center></td><td><center>  0.3 </center></td></tr>
  <tr><td> L1CaloMonManager1C  </td><td> PPrStabilityMon /EtCorrelation </td><td><center>  0.8 </center></td><td><center>  0.0 </center></td></tr>
  <tr><td> L1CaloMonManager2   </td><td> PPrMon                         </td><td><center>  1.6 </center></td><td><center>  2.8 </center></td></tr>
  <tr><td> L1CaloMonManager3   </td><td> PPMSimBSMon                    </td><td><center>  3.5 </center></td><td><center>  6.3 </center></td></tr>
  <tr><td> L1CaloMonManager4   </td><td> PPrSpareMon                    </td><td><center>  0.3 </center></td><td><center>  0.5 </center></td></tr>
  <tr><td> L1CaloMonManager5   </td><td> JEMMon                         </td><td><center>  0.5 </center></td><td><center>  0.9 </center></td></tr>
  <tr><td> L1CaloMonManager6   </td><td> CMMMon                         </td><td><center>  0.2 </center></td><td><center>  0.3 </center></td></tr>
  <tr><td> L1CaloMonManager7   </td><td> JEPSimBSMon                    </td><td><center> 14.4 </center></td><td><center> 25.8 </center></td></tr>
  <tr><td> L1CaloMonManager8   </td><td> TrigT1CaloCpmMonTool           </td><td><center>  0.6 </center></td><td><center>  1.1 </center></td></tr>
  <tr><td> L1CaloMonManager9   </td><td> CPMSimBSMon                    </td><td><center>  4.0 </center></td><td><center>  7.2 </center></td></tr>
  <tr><td> L1CaloMonManagerA   </td><td> TrigT1CaloRodMonTool           </td><td><center>  0.2 </center></td><td><center>  0.3 </center></td></tr>
  <tr><td> L1CaloMonManagerB   </td><td> TrigT1CaloGlobalMonTool        </td><td><center>  0.1 </center></td><td><center>  0.3 </center></td></tr>
  <tr><td> L1CaloMonManagerC   </td><td> EmEfficienciesMonTool          </td><td><center>  5.4 </center></td><td><center>  5.3 </center></td></tr>
  <tr><td> L1CaloMonManagerD   </td><td> JetEfficienciesMonTool         </td><td><center>  3.6 </center></td><td><center>  3.0 </center></td></tr>
  <tr><td> L1MonManager0A (3)  </td><td> CalorimeterL1CaloMon           </td><td><center> 15.1 </center></td><td><center>  9.0 </center></td></tr>
  <tr><td> L1MonManager0B (3)  </td><td> L1CaloHVScalesMon (4)          </td><td><center>  1.0 </center></td><td><center>  0.6 </center></td></tr>
  <tr><td> L1MonManager0C (3)  </td><td> L1CaloPMTScoresMon (4)         </td><td><center>  0.1 </center></td><td><center>  0.1 </center></td></tr>
  <tr><td> L1MonManager1 (3)   </td><td> L1CaloCTPMon                   </td><td><center>  0.4 </center></td><td><center>  0.7 </center></td></tr>
  <tr><td> L1MonManager2 (3)   </td><td> L1CaloLevel2Mon                </td><td><center>  0.1 </center></td><td><center>  0.2 </center></td></tr>
  </table>

  (1) Needs to run before any other algorithms that may be reading our data, eg RoIBResultToAOD.<br>
  (2) This tool forms CaloCell Et sums and quality per TriggerTower for the use of other tools.<br>
  (3) TrigT1Monitoring.<br>
  (4) Runs first event of each job only.
  
  To get the cpu times from the job log do:

  @code
  grep 'L1' job.log | grep 'MonManager' | grep 'execute' | sort > cpu.log
  @endcode

  The numbers in the table were generated with this program:

@code
#include <iostream>
#include <iomanip>

int main()
{
  int ntools = 25;
  // relative cpu times for each tool in express stream (from job log)
  float timesE[] = {1.77,0.324,0.318,0.065,8.16,0.673,1.36,0.222,0.420,
                    0.923,0.076,0.128,0.041,3.8,0.161,1.06,0.048,0.039,
		    1.41,0.943,3.98,0.253,0.026,0.105,0.03};
  // flag which tools run in each stream (as in jobOptions)
  int express[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  int jetet[]   = {1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1};
  int egamma[]  = {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,1};
  int muons[]   = {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1};
  int other[]   = {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1};
  // relative number of events per stream
  // (from plots L1Calo/Overview/l1calo_1d_NumberOfEvents)
  float events[] = {0.192, 2.11, 1.53, 1.52, 1.53};
  float timesO[ntools];
  float totalE = 0;
  float totalO = 0;
  for (int i = 0; i < ntools; ++i) {
    totalE += timesE[i];
    timesO[i] = express[i]*timesE[i]*events[0] + jetet[i]*timesE[i]*events[1] +
                 egamma[i]*timesE[i]*events[2] + muons[i]*timesE[i]*events[3] +
		  other[i]*timesE[i]*events[4];
    totalO += timesO[i];
  }
  float percE, percO;
  std::cout << "Express  Overall" << std::endl;
  for (int i = 0; i < ntools; ++i) {
    percE = 100*timesE[i]/totalE;
    percO = 100*timesO[i]/totalO;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint)
              << std::setprecision(1)
              << std::setw(6) << percE
              << std::setw(9) << percO << std::endl;
  }
}
@endcode

  Times are for one input file (683 events).

*/

/**

@page Mergepage Histogram Merging and Post-Processing

@section TrigT1CaloMonitoring_Mergesection Histogram Merging

  On Tier0 the raw data is processed one input file per job
  and the histogram outputs from each job are merged in a later step.
  Merge methods can be defined with the histograms so that the
  merging is done correctly.  This is not an issue online as a single
  job processes all the events in a run.

  The available custom merge methods are defined in 
  @c MonitoringFile_MergeAlgs.cxx and the name strings to use in the code in
  @c MonitoringFile::mergeObjs() both from the package 
  DataQuality/DataQualityUtils.
  If no merge method is specified the default is ROOT @c h1->Add(h2).

  When using custom merge methods or unusual filling schemes the merge
  should be tested to show that it gives the same result as a single
  job with no merges and the same result regardless of the order of
  merging.  This can be done with the @c rootDiff command from
  Sami Kama.  A slightly modified version of this is available in the
  @c test directory of the TrigT1CaloMonitoringTools package.
  This can be used in the following way:

  Set up a suitable Tier0 release then compile the test executable:
  @code
  g++ -o rootDiff rootDiff.cxx
  @endcode

  Run three @c Reco_trf.py jobs on the same raw data file making sure that
  the histograms of interest have some entries (or the test is useless!):
  @code
  Reco_trf.py maxEvents=500 ......
  Reco_trf.py maxEvents=240 ......
  Reco_trf.py maxEvents=260 skipEvents=240 ......
  @endcode

  If the histogram output files from these jobs are
  @c Hist1.root, @c Hist2.root, @c Hist3.root say, then create a file
  @c hist_merge_list1.txt say, containing the names:
  @code
  Hist3.root
  Hist2.root
  @endcode

  Note the reverse order.  Then do:
  @code
  DQHistogramMerge hist_merge_list1.txt Hist4.root False >& DQHistogramMerge1.log
  ./rootDiff Hist1.root Hist4.root >& rootDiff.log
  @endcode

  and check @c rootDiff.log for any L1Calo or LVL1_Interfaces errors.
  <hr>

@section TrigT1CaloMonitoring_MergesUsedsection Custom Merges Used in TrigT1CaloMonitoring

  <table>
  <tr><th> Merge Method        </th><th> Description from DataQualityUtils      </th></tr>
  <tr><td> @c eventSample      </td><td> Merge histograms containing, for example,
                                         event numbers of events with particular
					 types of errors.                       </td></tr>
  <tr><td> @c lowerLB          </td><td> Merge "status" histograms,
                                         i.e filled at start of run/LB.         </td></tr>
  <tr><td> @c mergeRebinned    </td><td> This method provides a correct summation for histograms
                                         that have different binning.           </td></tr>
  <tr><td> @c perBinEffPerCent </td><td> This code assumes that the histogram content
                                         is the efficiency of a given cut or
				         selection in each bin.                 </td></tr>
  <tr><td> @c weightedAverage  </td><td> Merge histograms of weighted averages. </td></tr>
  </table>

  <hr>

@section TrigT1CaloMonitoring_PostProcessingsection Histogram Post-Processing

  Sometimes it's not possible to get the result you want with merging.
  In this case it is necessary to use the DQ histogram post-processing
  algorithms which are run after all the merging is complete.
  The L1calo post-processing code can be found in 
  @c MonitoringFile_L1CaloPostProcess.cxx from DataQualityUtils.

  Note that this code is not run in the standard @c Reco_trf.py job so to
  test it it is necessary to rerun @c DQHistogramMerge like this:
  @code
  DQHistogramMerge hist_merge_list.txt myHIST2.root True >& DQHistogramMerge2.log
  @endcode
  in the output directory of the @c Reco_trf.py job.

*/

/**

@page RoIAsymmetrypage Asymmetry in RoI eta-phi maps

@section TrigT1CaloMonitoring_CPMRoIAsymmetrysection Asymmetry in CPM RoI eta-phi maps

  E-mail from Alan Watson 24 Nov 2008:

  It's a consequence of a small but necessary asymmetry in the EM/Tau algorithm.

  One condition is that the central 2x2 "core" of the window must be an
  ET maximum. However, since a narrow ET deposit (e.g. a single tower)
  can produce identical 2x2 ET value in multiple windows, we can't just
  require that the core cluster is > all (overlapping) neighbours, or
  else the best EM candidates would always be rejected!  Hence we require
  that it is > neighbours along 2 sides, and >= neighbours along the
  opposite sides. In effect, this says "when you have 2 or more identical
  core clusters, take the top-right one of the group". Since neighbouring
  clusters may be being evaluated in different modules or crates, the
  chips cannot compare results, and so we need a convention which is
  consistent between all chips in order to resolve ambiguous situations
  such as this.

  The fact that we always choose the right-most RoI in these circumstances
  is what produces the differences at the two ends. Imagine we have ET
  in the last unmasked tower in eta:

@verbatim
  eta:    -2.7  -2.5  -2.4  -2.3  -2.2
  TT ET:    |  0  |  X  |  0  |  0  |
@endverbatim

    - here we have 2 identical 2x2 clusters, one spanning -2.7 -> -2.4, and
      one spanning -2.5 -> -2.3.
    - we require that core ET > neighbours to right, >= neighbours to left
      Thus the cluster spanning -2.5 -> -2.3 is selected, producing an RoI
      at eta=-2.4 (RoI coordinate = centre of RoI).
    - Since the tower -2.7 -> -2.5 is masked out, the -2.7 -> -2.4 RoI can
      never be more energetic than the -2.5 -> -2.3 one, and so will never be
      accepted. Hence in practice the last RoI is the one at -2.4.

@verbatim
  eta:     2.2   2.3   2.4   2.5   2.7
  TT ET:    |  0  |  0  |  X  |  0  |
@endverbatim

    - again we have 2 identical 2x2 clusters, one spanning 2.3 -> 2.5, and
      one spanning 2.4 -> -2.7.
    - we require that core ET > neighbours to right, >= neighbours to left.
      So again the right-most of the two is selected, but in this case
      that is the RoI spanning 2.4-2.7, which is reported as having eta=2.5.


  There is no way of avoiding this while still keeping the algorithm uniform
  in all CPMs (if the RoI condition was that a single tower was an ET
  maximum it would be more uniform, but that only works with a 3x3 or 5x5
  tower window, not a 4x4 tower one).

@section TrigT1CaloMonitoring_JEMRoIAsymmetrysection Asymmetry in JEM RoI eta-phi maps

  E-mail from Alan Watson 26 Aug 2010:

  We may have a continuous jet algorithm, but we apply different thresholds
  to "forward" jets and count them separately. So how then do we define a
  forward jet?

  The RoI coordinate is defined by requiring that a 2x2 jet element cluster
  is an ET maximum (i.e. the "4x4 jet" cluster). So this leads naturally
  to defining a forward jet as being one where this ET max cluster includes
  the FCAL (the logic being that if the edge of a 6x6 or 8x8 jet overlaps
  the FCAL but the ET max cluster does not then we should count this as a
  central jet). However, because of the way the modularity of the JEMS works
  there are 2 possible locations at the +ive eta end which meet this
  criterion, but only 1 at the -ive eta end.

@verbatim
  -ive eta:

          -4.9             -3.2    -2.9
            -------------------------
	    |                |      |
	    -------------------------

  +ive eta

           2.9    3.2              4.9
            -------------------------
            |      |                |
            -------------------------

                  3.2              4.9
                   ------------------
                   |                |    // only selected if zero ET
                   ------------------    // between 2.9 - 3.2
@endverbatim

  (There is no location at the -ive eta end corresponding to the farthest
  forward at +ive eta, and if there were the asymmetry in the RoI algorithm
  would mean that it would never be selected anyway - in fact the sign of
  the asymmetry was chosen so that this non-existent location could never
  be identified as the RoI position).

  There are some cases where it's useful to know which RoI was selected, but
  only for internal monitoring/debugging. For other users, such as L2, all
  that the coordinate tells you is "it's a forward jet" - they need to look
  at the whole FCAL plus the end of the endcap anyway, and so distinguishing
  in the eta coordinate wasn't actually helpful. It did however cause
  confusion on a fairly regular basis, so in the end we suppressed this in
  decoding the RoI word and just return a single "forward jet" eta coordinate
  at both ends. But for those cases where we do want to distinguish,
  the JEPRoIDecoder (in TrigT1Interfaces), which decodes the RoI word,
  can return a "hardware style coordinate" (crate, module, row, column)
  which can be used to distinguish between the two locations.

*/

/**

@page ErrorEventspage Finding Error Events

The event numbers of error events are given in histograms available online
and in the full histogram output file from Tier0.  These need to be viewed
with the 'text' display option.  If the number is too big to be displayed
fully enter this ROOT command:

@code
  gStyle->SetPaintTextFormat("10.0f")
@endcode

To find the file the event is in one option is to use the script
@c findEventFile which can be found in the @c scripts directory of
TrigT1CaloMonitoringTools.  This works by finding the file GUID in the
relevant TAG file and then matching that to a filename in the Grid
catalog using dq2-ls.  If necessary it will try to download the TAG files
from the Grid and can optionally download the resulting RAW or ESD file.
If the TAG files are not available on the Grid you can retrieve them from
CERN Castor first if you have permission (the l1ccalib account can do it).

*/

/**



*/

/**



*/
