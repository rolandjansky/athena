========================================================
TauAnalysisTools: Package hosting tools for tau analysis
========================================================

:authors: Dirk Duschinger, David Kirchmeier
:contact: david.kirchmeier@cern.ch

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
* **DiTauEfficiencyCorrectionsTool:** provides identification scale factors and the
    associated uncertainties for boosted di-taus
* **DiTauTruthMatchingTool:** performs matching of boosted di-taus to the visible truth tau
    4-momenta
    
The latest documentation about tau performance measurements in run 2
can be found here: `ATL-PHYS-PUB-2015-045
<https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PUBNOTES/ATL-PHYS-PUB-2015-045/>`_.
Information on latest tau recommendations can be found here:
`TauRecommendationsR21 <https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauRecommendationsR21>`_.

In case of any problems, issues or suggestions don't hesitate to contact the
authors.

-----
Setup
-----

Please have a look into this `tutorial <https://atlassoftwaredocs.web.cern.ch/ABtutorial/>`_ 
to get general information on how to setup an analysis release. In particular the section about initializing CP tools via `AnaToolHandles <https://atlassoftwaredocs.web.cern.ch/ABtutorial/basic_ana_tool_handle/>`_ might be worth to read.

--------
Examples
--------

An example implementation of all tools can be found for stand-alone mode in
``TauAnalysisTools/util/TauAnalysisToolsExample.cxx``.

After setting up an ``AnalysisBase`` release the example can be executed via::

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
