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
* **TauOverlappingElectronLLHDecorator:** decorating reconstructed taus with a
    likelihood score of matched reconstructed electrons
    
All relevant information about the actual measurement of uncertainties for run 1
can be found here: `TauRecommendationsWinterConf2013
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsWinterConf2013>`_.
These numbers are mostly valid for 2012 data analysis using reprocessed data,
i.e. p1443 (p1344/p1345).
Information on 2015 tau recommendations can be found here:
`TauPreRecommendations2015 <https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015>`_.

In case of any problems, issues or suggestions don't hesitate to contact the
authors.

-----
Setup
-----

AthAnalysisBase
---------------

First start with a clean shell and setup ATHENA, for example via::

  setupATLAS

  #use 2.4.19 or higher
  asetup AthAnalysisBase,2.4.19,here

Where X stands for a release number. Get the package and setup environment::
  
  cmt co PhysicsAnalysis/TauID/TauAnalysisTools
  
  cd PhysicsAnalysis/TauID/TauAnalysisTools/cmt/
  source setup.sh

Finally compile::
  
  cmt make

AnalysisBase
------------

First start with a clean shell and setup RootCore via::

  setupATLAS

  #use 2.4.19 or higher
  rcSetup Base,2.4.19
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

An example implementation of all tools can be found for stand-alone mode in
``TauAnalysisTools/util/TauAnalysisToolsExample.cxx``. The binary file should be
found after compilation in
``RootCoreBin/bin/x86_64-slc6-gcc47-opt/TauAnalysisToolsExample``.

The example can be executed via::

  TauAnalysisToolsExample FILENAME [NUMEVENTS]

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
* `TauOverlappingElectronLLHDecorator <doc/README-TauOverlappingElectronLLHDecorator.rst>`_
