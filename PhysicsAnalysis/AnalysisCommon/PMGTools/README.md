# PMG Cross Section Tool

## General information

This tool is intended to allow for a central location from which the
cross sections for all processes centrally produced in ATLAS can be
queried to obtain information pertinent to normalizing to the
theoretically predicted cross section. The information that can be
obtained from this tool includes, but is not limited to:

  - Cross Section
  - k-factor
  - Filter Efficiency

When using this tool, you should only have to interact with the
[PMGTools::PMGCrossSectionTool](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/PMGTools/PMGCrossSectionTool.h)
interface.

For details, please thoroughly read this TWiki. If something does not
work, then do not hesitate to email the Generator Infrastructure and
Tools list: <atlas-phys-pmg-git@cern.ch>.

The input cross-section files which are the input for this tool are
stored on CVMFS and duplicated on EOS:

    /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PMGTools/PMGxsecDB_mc16.txt
    /eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/

The files are synchronised with AMI each day, and timestamped versions
are located in the same directory.


## Introduction

When using Monte Carlo samples to predict event yields and differential
distributions to model that in data, in particular when there are
multiple processes involved, it is important to perform a proper scaling
to normalize the Monte Carlo to the theoretically predicted cross
section. From a practical point of view, this scaling is performed on an
event-by-event ($Weight(event)$) basis where the weight with the
characteristics of a single event ($event$) :

$Weight(event)=\frac{w_{exp}(event) \times \sigma \times \epsilon_{filter} \times k}{\sum w_{theory}(event)}$

In this scale factor weight, the following components are

  - $\sigma$ : The raw theoretical cross section for the process
      - This is provided by this method - [getAMIXsection(const int  dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L139)
      - You can also access:
         -  the UP uncertainty - [getXsectionUncertaintyUP(const int dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L161)
         - the DOWN uncertainty - [getXsectionUncertaintyDOWN(const  int dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L170)
        -  the Symmetrized uncertainty - [getXsectionUncertainty(const int dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L179)
  - $\epsilon_{filter}$ : The efficiency of any generator level
    filter that makes the MC production more efficient
      - This is provided by this method - [getFilterEff(const int dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L119)
  - $k$ factor : The theoretical weight coming from e.g. higher order
    calculation schemes
      - This is provided by this method - [getKfactor(const int dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L185)
  - The product of raw $\sigma$, $\epsilon_{filter}$ and $k$-factor:
      - This is provided by this method - [getSampleXsection(const int
        dsid)](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/AnalysisCommon/PMGTools/Root/PMGCrossSectionTool.cxx#L196)
  - $w_{exp}(event)$ : The experimental weight coming from e.g.
    efficiency corrections to object reconstruction and identification
      - This is **NOT** provided by this tool: you need to provide it from the rest of your analysis.
  - $\sum w_{theory}(event)$ : The total sum of the event weights
    for the sample over which you run
      - This is **NOT** provided by this tool: you should access this from the sample metadata (eg CutBookKeeper)

## IMPORTANT: Known issues

Between November 2019 and January 2020, several samples were filled with
a wrong filter efficiency.

**Do not use the xsec files on
`/eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/<YYYY-MM-DD>` with
dates between 2019-10-31 and 2020-01-27.**

## Initial Setup

The recommended release to use is **21.2.4** or later. The following
gives a minimal working example how to use the PMGCrossSectionTool in
your own analysis.

First of all, you have to start with a general AthAnalysis setup:
    
    mkdir PMGCrossSectionTool
    cd PMGCrossSectionTool
    mkdir source build run
    setupATLAS
    lsetup root
    lsetup git
    cd build
    acmSetup AthAnalysis,21.2,latest

Here we assume, that you set up you own analysis from scratch, but the
procedure should be similar when including the PMGCrossSectionTool into
an existing analysis:

    cd ../../
    acm new_skeleton MyCrossSectionToolTest

Add the following dependencies in
`MyCrossSectionToolTest/CMakeLists.txt` (or in the respective
`CMakeLists.txt` file of your analysis):

    # Declare the package's dependencies:
    atlas_depends_on_subdirs( PUBLIC
                              Control/AthToolSupport/AsgTools
                              PhysicsAnalysis/AnalysisCommon/PMGTools
                              PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
                              Tools/PathResolver
    )
    
    ...
    
    # build a shared library
    atlas_add_library( MyCode MyCode/*.h Root/*.h Root/*.cxx ${MyCodeDictSource}
                       PUBLIC_HEADERS MyCode
                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                       LINK_LIBRARIES ${ROOT_LIBRARIES}  xAODBase xAODRootAccess
                       ...
                       AsgTools
                       PMGAnalysisInterfacesLib PMGToolsLib
                       PathResolver
    )

**HEADER**

Include the following headers in
`MyCrossSectionToolTest/src/MyCrossSectionToolTestAlg.h` (or in the
respective header file of your analysis):

    //include the headers of relevance
    #include "AsgTools/AnaToolHandle.h"
    #include "PMGAnalysisInterfaces/IPMGCrossSectionTool.h"
    #include "PMGTools/PMGCrossSectionTool.h"
    #include <PathResolver/PathResolver.h>

and then declare the tool as an AnaToolHandle instance

    private:
      asg::AnaToolHandle<PMGTools::IPMGCrossSectionTool> m_PMGCrossSectionTool; //!

**INITIALIZE**

In the `initialize()` routine of your EventLoop in
`MyCrossSectionToolTest/src/MyCrossSectionToolTestAlg.cxx` (or in the
respective file of your analysis code), configure the tool and then read
in the cross sections file. **NOTE : The configuring of the path will
change in the future but for now it requires the full path to the
directory containing the cross sections file.**

    //Instantiation (if not using some ToolHandle )
    std::cout<<"Initializing Cross Section Tool"<<std::endl;
    ASG_SET_ANA_TOOL_TYPE( m_PMGCrossSectionTool, PMGTools::PMGCrossSectionTool);
    m_PMGCrossSectionTool.setName("myCrossSectionTool");
    m_PMGCrossSectionTool.retrieve();
    m_PMGCrossSectionTool->readInfosFromFiles({PathResolverFindCalibFile("dev/PMGTools/PMGxsecDB_mc16.txt")});

If you want to use the latest available cross section data, you should
choose `/eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/` as input
directory. The cross section files are created on a daily basis. To
reproduce results from a previous date, choose
`/eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/<YYYY-MM-DD>` as
input.

If you are not working on lxplus, you will have to copy the database
files from `/eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/` to
your local computer and change the path in the last line accordingly.

**EXECUTE**

In the `execute()` routine of your EventLoop, you can then call the
various methods accessible via the interface. An example of printing the
cross section and filter efficiency for the DSID 301300 may look like :

    int myDSID = 301300;
    std::cout<<"Checking PMG Tool : "<<m_PMGCrossSectionTool->getAMIXsection(myDSID)<<"   "
                                     <<m_PMGCrossSectionTool->getFilterEff(myDSID)<<std::endl;

The DSID is an attribute of an MC event, along with the identifier that
says whether an event is MC or data. These two attributes can be
accessed through the EventInfo object as described in [the ATLAS software tutorial ](AtlasComputing.SoftwareTutorialxAODAnalysisInROOT#General_event_information).

Having modified the code, you should compile:

    acm compile

To execute the example above, you have to create a python steering file
in the `run` directory:

    cd run
    touch runMyCrossSectionToolTest.py

The minimal example to run the code should contain the following lines
in `runMyCrossSectionToolTest.py`:

    theApp.EvtMax=1
    athAlgSeq += CfgMgr.MyCrossSectionToolTestAlg("MyCrossSectionToolTestAlg")

Then you can execute the tool via:

    athena runMyCrossSectionToolTest.py

In the output you should find the following line giving the cross
section and filter efficiency of the sample defined above with the DSID:

    Checking PMG Tool : 6.2668   1

## For Developers

This tool is nominally maintained by the PMG group. However, it is the
responsibility of everyone using it to be critical of the results, and
when necessary, contribute to improving it. In this case, that boils
down to being comfortable submitting a merge request ( [follow these
directions](https://atlassoftwaredocs.web.cern.ch/gittutorial/)).

## One of the cross sections is wrong\!

This is a frequent occurrence because there are thousands of cross
sections and filter efficiencies etc. If you find this is the case,
please report this issue in a Central Page JIRA ticket [as described
here](%SCRIPTURL{"view"}%auth/AtlasProtected/PmgCentralPage#Reporting_issues).

