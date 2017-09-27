=======================================================================
PhysicsAnalysis/AnalysisCommon/AssociationUtils - Overlap removal tools
=======================================================================

:authors: Steve Farrell
:contact: Steven.Farrell@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This package contains a set of tools to provide users with the recommended
overlap removal prescriptions for analysis-level objects (electrons, muons,
taus, jets, photons, and fat-jets). This document covers a wide range of
technical details about how the package is structured, but if you're just
looking for the quick answers of how to set things up and use the tools, you
can skip ahead to the `Configuration helpers`_ section.

The current recommendations were summarized by me in these slides from the
flavour-tagging and Hbb workshop on Sep 6, 2017:
https://indico.cern.ch/event/631313/contributions/2683959/

For some history you may also refer to the slides at these meetings:

* 2016-11-14 in physics coordination: https://indico.cern.ch/event/587852/
* 2016-10-21 in ASG: https://indico.cern.ch/event/576538/
* 2016-06-03 (for ICHEP): https://indico.cern.ch/event/539619/

The configuration is quite flexible and a lot of features are supported,
but in summary it is recommended that analyses adopt one of the "working
points" depending on the type of analysis:

* **Standard** - covers normal analyses not falling into the other categories.
* **Heavy-flavor** - for analyses with heavy-flavor jets.
* **Boosted** - for analyses with boosted leptons and jets.
* **Boosted+Heavy-flavor** - for analyses with both HF jets and boosted
  objects.

See the above slides for a good summary of these working points.
There are some variations on the above which favor leptons or photons more,
but I won't cover those details here.

For a quick example of how to use the tools with the configuration helper
code in RootCore, take a look at the tester executable:

* `util/OverlapRemovalTester.cxx <../util/OverlapRemovalTester.cxx>`_

For a corresponding example in Athena (e.g. in AthAnalysis), look at the
tester algorithm and job options:

* `src/OverlapRemovalTestAlg.h <../src/OverlapRemovalTestAlg.h>`_
* `src/OverlapRemovalTestAlg.cxx <../src/OverlapRemovalTestAlg.cxx>`_
* `share/OverlapRemovalTest_jobOptions.py <../share/OverlapRemovalTest_jobOptions.py>`_

For documentation of legacy features, refer to the original package README
in the SVN area:
https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/AssociationUtils/trunk/doc/README.rst

---------------------
Overlap removal tools
---------------------

Interfaces
----------

There are currently two tool interfaces for overlap removal tools:

1. ORUtils::IOverlapTool - Identifies overlaps between two containers. Concrete
   tools that implement this interface represent one step of overlap removal.
   There is thus one method which looks like:
   ``StatusCode findOverlaps(container1, container2);``

2. ORUtils::IOverlapRemovalTool - High-level interface which operates on the
   full set of particle containers and flags all overlaps. The method looks
   like:
   ``StatusCode removeOverlaps(electrons, muons, jets, taus, photons, fatjets);``

Overlap tools
-------------

These tools find and flag overlaps between two containers. Some of them are
type-generic and thus operate on IParticleContainers, whereas others are
type-specific and do a runtime type check.

* DeltaROverlapTool - A simple overlap finder that uses a dR match.
* EleJetOverlapTool - Implements the recommended ele-jet overlap removal
  based on dR cones, JVT, and user-set btagging decorations.
* EleMuSharedTrkOverlapTool - Removes (input) electrons that share an ID track
  with (input) muons. By default vetoes overlapping calo-muons.
* MuJetOverlapTool - Implements the recommended mu-jet overlap removal based
  on dR, jet track multiplicity, mu-jet PT ratios, and user-specified btagging.
* TauLooseEleOverlapTool - Implements overlap removal between taus and loose
  electrons based on the LooseLH ID of the electron and a dR cone.
* TauLooseMuOverlapTool - Implements overlap removal between taus and loose
  muons. The criteria considers muons based on PT and isCombined as
  recommended in the harmonization document.
* ObjLinkOverlapTool - A generic tool which flags overlaps by looking for
  ElementLinks to other particles. This tool can be used to find overlaps in
  two stages. For example, one might use the EleMuSharedTrkOverlapTool to
  write out overlap decorations for electrons and muons sharing tracks, and
  then use that result after slimming away the xAOD ID tracks with this tool
  to find overlaps using the final analysis selections and systematics.

Refer to the class documentation for more details on each one's implementation
or ask me if you have any questions.

Master tools
------------

At the moment there is just one master tool: the OverlapRemovalTool. In order
to use this tool to apply overlap removal steps, the user must configure the
desired OverlapTools and assign them to the master tool.

---------------------
Configuration helpers
---------------------

The modular tool design of AssociationUtils is highly flexible, but
configuration of all tools in a consistent way can be tedious. Thus, two
types of helper code are provided to ease the burden on users. The first is
for C++-based configuration in RootCore or in Athena, while the second is
for traditional python-based configuration in Athena. If you just want the
quick code snippets to set things up, see the examples for each approach
further below.

Configuration in C++ RootCore/Athena
------------------------------------

The C++ configuration helper code works in both RootCore environments and
in Athena environments. There are three pieces:

* The *ORFlags* struct defined in `OverlapRemovalInit.h
  <../AssociationUtils/OverlapRemovalInit.h>`_ defines the high level
  configuration options you can set:

  * the name of the master tool
  * the input and output decoration names
  * the flags to specify the working point
  * flags to specify which objects you use, e.g. doElectrons, doJets, etc.

* The `ToolBox <../AssociationUtils/ToolBox.h>`_ class wraps all of the
  tools in AnaToolHandles for ease of configuration and execution.

  * you can overwrite the handles to switch out tool implementations
  * you can set properties on the tools
  * the ToolBox owns it tools via the AnaToolHandles.
    The user must thus take care to manage the lifetime of the ToolBox,
    preferably as a member of your EventLoop alg or framework equivalent.

* The *recommendedTools* function in
  `AssociationUtils/OverlapRemovalInit.h <../AssociationUtils/OverlapRemovalInit.h>`_
  takes an ORFlags and a ToolBox and configures all of the tools for you.

**How to setup the working points**

*Important notes*:

* In the following snippets, the ORFlags and ToolBox objects are declared as
  locals. In your actual setup you will likely make these private members of
  some EventLoop algorithm or a parent tool or something like that.

* The working points calculate delta-R using _rapidity_ instead of
  pseudo-rapidity, as recommended in the harmonization note. You can override
  this with the UseRapdity property that all relevant tools support.

*Standard working point* - you only need to set the tool and
decoration names:

.. code:: cpp

    ORUtils::ORFlags orFlags(masterToolName, inputLabel, outputLabel);
    ORUtils::ToolBox toolBox;
    CHECK( ORUtils::recommendedTools(orFlags, toolBox) );
    CHECK( toolBox.initialize() );

*Heavy flavor working point* - just specify the name of the decoration
that you will apply to label bjets:

.. code:: cpp

    ORUtils::ORFlags orFlags(masterToolName, inputLabel, outputLabel);
    ORUtils::ToolBox toolBox;
    orFlags.bJetLabel = "isBJet";
    CHECK( ORUtils::recommendedTools(orFlags, toolBox) );
    CHECK( toolBox.initialize() );

*Boosted object working point* - set the appropriate flag to true:

.. code:: cpp

    ORUtils::ORFlags orFlags(masterToolName, inputLabel, outputLabel);
    ORUtils::ToolBox toolBox;
    orFlags.boostedLeptons = true;
    CHECK( ORUtils::recommendedTools(orFlags, toolBox) );
    CHECK( toolBox.initialize() );

*Heavy flavor and boosted object working point* - just combine the above
settings and set both the bJetLabel and the boostedLeptons flag.

*Lepton favored working point* - HSG2 uses a modified overlap removal
prescription including electron-electron cluster matching and the disabling
of the electron and muon rejections by jets. To configure this setup,
do the following:

.. code:: cpp

    ORUtils::ORFlags orFlags(masterToolName, inputLabel, outputLabel);
    ORUtils::ToolBox toolBox;
    CHECK( ORUtils::recommendedTools(orFlags, toolBox) );
    CHECK( toolBox.eleEleORT.setProperty("UseClusterMatch", true) );
    CHECK( toolBox.eleJetORT.setProperty("OuterDR", 0.) );
    CHECK( toolBox.muJetORT.setProperty("OuterDR", 0.) );
    CHECK( toolBox.initialize() );

*How do I use only electrons, muons, and jets?* - To specify which objects
to configure tools for, you just enable/disable the appropriate flags; e.g.:

.. code:: cpp

    orFlags.doElectrons = true;
    orFlags.doMuons = true;
    orFlags.doJets = true;
    orFlags.doTaus = false;
    orFlags.doPhotons = false;

*How do I get out the tool for normal use?* You can use toolBox.masterTool
directly since it is an AnaToolHandle, or you can take a reference to it, or
you can use ``std::move`` to extract the tool to a new AnaToolHandle.

Configuration in Athena job options
-----------------------------------

For Athena, the configuration helper code is much simpler. There is just
one python function which you invoke to get the configurables for all the
overlap removal tools. Keyword arguments to this function are used to
control all the high-level configuration options.

The code is in `python/config.py <../python/config.py>`_. It should be
sufficiently documented so refer to it for the full details.

**How to setup the working points**

Since the settings are simply steered by python function keyword arguments,
I will only show the standard one in detail and then describe which args need
to be set for the other working points.

*Standard working point* - use mostly default arguments:

.. code:: python

    from AssociationUtils.config import recommended_tools
    orTool = recommended_tools(masterName=masterToolName,
                               inputLabel=inputLabel,
                               outputLabel=outputLabel)

*Heavy flavor working point* - set the ``bjet_label`` argument to your bjet
decoration name.

*Boosted object working point* - set the ``boosted_leptons`` argument to True.

*Heavy flavor and boosted object working point* - just set both arguments
above.

*HSG2 overlap removal prescription* - See the explanation of this working point
in the RootCore examples above. Configure like thus:

.. code:: python

    from AssociationUtils.config import recommended_tools
    orTool = recommended_tools(masterName=masterToolName,
                               inputLabel=inputLabel,
                               outputLabel=outputLabel)
    orTool.EleEleORT.UseClusterMatch = True
    orTool.EleJetORT.OuterDR = 0.
    orTool.MuJetORT.OuterDR = 0.

*How do I use only electrons, muons, and jets?* - As with the C++ version,
just set the appropriate flags: doElectrons, doMuons, doJets, etc.

.. code:: python

    orTool = recommended_tools(masterName=masterToolName,
                               inputLabel=inputLabel,
                               outputLabel=outputLabel,
                               doElectrons=True,
                               doMuons=True,
                               doJets=True,
                               doTaus=False,
                               doPhotons=False)

*How do I use this with an Athena algorithm?*

Just add the configurable to your algorithm in the job options.
Please refer to the example job options:
`share/OverlapRemovalTest_jobOptions.py <../share/OverlapRemovalTest_jobOptions.py>`_

-----------------
Advanced features
-----------------

In this section I describe the low-level configurable properties of the
tools and some options for configuring advanced features.

Common tool properties
----------------------
These properties can be set on all overlap tools. They are defined in the
BaseOverlapTool class. The config helper codes (see below) provide the means to
apply such properties globally.

+--------------------+------------+----------------------------------------------+
| Property           | Default    | Description                                  |
+====================+============+==============================================+
| InputLabel         | "selected" | Input object decoration                      |
+--------------------+------------+----------------------------------------------+
| OutputLabel        | "overlaps" | Output object decoration                     |
+--------------------+------------+----------------------------------------------+
| OutputPassValue    | False      | Output decoration value assigned to objects  |
|                    |            | that pass overlap removal. Allows to flip    |
|                    |            | the output logic so a True means the objects |
|                    |            | pass overlap removal (remember to change the |
|                    |            | OutputLabel as well).                        |
+--------------------+------------+----------------------------------------------+
| LinkOverlapObjects | False      | Enable overlap object link decorations.      |
+--------------------+------------+----------------------------------------------+

Master tool properties
----------------------
The master OverlapRemovalTool also uses some of the above common properties
which need to be set and are mainly just for initializing/resetting the output
decorations: InputLabel, OutputLabel, and OutputPassValue.

Additionally, the RequireExpectedPointers property (default true) can be used
to disable errors when expected containers are missing (null).

Sliding delta-R cone for boosted lepton analyses
------------------------------------------------
For boosted lepton analyses, prompt leptons may be close to jets in delta-R
from the physics decay chain. To improve the efficiency of the overlap removal
on these leptons, a sliding-cone association can be used instead of the flat
delta-R. The EleJetOverlapTool and MuJetOverlapTool support this feature, using
the sliding delta-R cone: ``dR = C1 + C2/pt``, where pt is the leptons's
transverse momentum. A maximum value of the cone size can also be configured.
The default values for this approach are C1 = .04, C2 = 10 GeV, and max cone
size = 0.4.

The properties for activating this feature on the EleJetOverlapTool and
MuJetOverlapTool are ``UseSlidingDR``, ``SlidingDRC1``, ``SlidingDRC2``, and
``SlidingDRMaxCone``. The easiest way to configure it is to use the config
helper functions.

Electron-electron overlap removal
---------------------------------
Ele-ele OR is now available. To enable, set the ORFlags::doEleEleOR flag to
true or the doEleEleOR flag in the python helper function.

Bypassing PV dependent quantities in mu-jet OR
----------------------------------------------
The MuJetOverlapTool needs the PV to retrieve the numTrack and sumTrkPt
quantities. This can be a problem for users that filter out the PV, so I've
added two properties that specify user decorations for the required
quantities: JetNumTrackDecoration, JetSumTrackPTDecoration.
