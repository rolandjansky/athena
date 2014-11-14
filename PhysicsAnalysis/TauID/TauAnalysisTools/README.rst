========================================================
TauAnalysisTools: Package hosting tools for tau analysis
========================================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. meta::
   :description: TauAnalysisTools: Package hosting tools for tau analysis
   :keywords: TauAnalysisTools, tau, TauEfficiencyCorrectionsTool, TauSelectionTool, TauSmearingTool, maddog

.. contents:: Table of contents

------------
Introduction
------------

This package is designed to provide dual-use tools for easy use of taus in your
analysis. Currently following tools are available:

* **TauSelectionTool:** generic tool to apply a set of requirements on tau
    candidates
* **TauSmearingTool:** currently support tau energy corrections
* **TauEfficiencyCorrectionsTool:** provides identification scale factors and the
    associated uncertainties
* **TauTruthMatchingTool:** performs matching of taus to the visible truth tau
    4-momentum
* **TauTruthTrackMatchingTool:** performs matching of tracks to truth taus and
    tracks to truth charged particles
    
All relevant information about the actual measurement of these uncertainties can
be found here: `TauRecommendationsWinterConf2013
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsWinterConf2013>`_.
These numbers are mostly valid for 2012 data analysis using reprocessed data,
i.e. p1443 (p1344/p1345).

Most of the functionality of TauEfficiencyCorrectionsTool and TauSmearingTool
was copied from former `TauCorrUncert Tool
<https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauCorrUncert>`_.

Although this package is intended to be dual-use, it is mostly designed for
AnalysisBase. Thus, information on how to use the tool within ATHENA might not
be up to date.

In case of any problems, issues or suggestions don't hesitate to contact the
authors.

-----
Setup
-----

Athena
------

First start with a clean shell and setup ATHENA, for example via::

  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
  setupATLAS

  #use 19.1.2.1 or higher
  asetup AtlasProduction,19.1.2.1,here

get the package and setup environment::
  
  cmt co PhysicsAnalysis/TauID/TauAnalysisTools
  
  cd PhysicsAnalysis/TauID/TauAnalysisTools/cmt/
  source setup.sh

finally compile::
  
  cmt make

AnalysisBase
------------

First start with a clean shell and setup RootCore via::

  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
  setupATLAS

  #use 2.0.11 or higher
  rcSetup Base,2.0.11
  rc find_packages

and compile with::

  rc compile

or directly with::

  rc compile_pkg TauAnalysisTools

---------------
General Remarks
---------------

For each tool the message level can be adjusted like::

  TauSelTool.msg().setLevel( MSG::VERBOSE );

--------
Examples
--------

Athena
------

AnalysisBase
------------

An example implementation of all tools can be found for stand-alone mode in
``TauAnalysisTools/util/TauAnalysisToolsExample.cxx``. The binary file should be
found after compilation in
``RootCoreBin/bin/x86_64-slc6-gcc47-opt/TauAnalysisToolsExample``.

The example can be executed via::

  RootCoreBin/bin/x86_64-slc6-gcc47-opt/TauAnalysisToolsExample FILENAME [NUMEVENTS]

FILENAME has to point to a root file and NUMEVENTS is an integer of events to
process. If NUMEVENTS is not set all events are processed. The same is true if
the actual number of events in the root file is less than the given number. 

  
-----------------------------------
Particular information on the tools
-----------------------------------

More detailed information on how to use the tools can be found here:

* `TauSelectionTool <doc/README-TauSelectionTool.rst>`_
* `TauSmearingTool <doc/README-TauSmearingTool.rst>`_
* `TauEfficiencyCorrectionsTool <doc/README-TauEfficiencyCorrectionsTool.rst>`_
* `TauTruthMatchingTool <doc/README-TauTruthMatchingTool.rst>`_
* `TauTruthTrackMatchingTool <doc/README-TauTruthTrackMatchingTool.rst>`_


