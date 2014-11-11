========================================================
PhysicsAnalysis/JetMissingEtID/JetSelectorTools:
   Package hosting jet selection (cleaning) tools
========================================================

:authors: Zach Marshall
:contact: ZLMarshall@lbl.gov

.. meta::
   :description: JetSelectorTools: Package hosting jet selection (cleaning) tools
   :keywords: jet, JetSelectorTools

.. contents:: Table of contents

------------
Introduction
------------

This package contains dual-use tools for selecting good jets passing one of
several levels of jet cleaning cuts.  There is only one tool, the **JetCleaningTool**,
which is configurable and provides a good/bad decision for each jet.

The documentation is on the twiki here:
`HowToCleanJets2012
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HowToCleanJets2012>`_.

This procedure applies to all of run 1.

-----
Athena Setup
-----

After athena is set up, simply do:

  cmt co PhysicsAnalysis/JetMissingEtID/JetSelectorTools
  cd PhysicsAnalysis/JetMissingEtID/JetSelectorTools/cmt/

and compile::

  cmt make

AnalysisBase
------------

After RootCore is setup, simply do:

  svn co ${SVNOFF}/PhysicsAnalysis/JetMissingEtID/JetSelectorTools/trunk JetSelectorTools
  rc find_packages
  rc compile_pkg JetSelectorTools

---------------
Tool Usage
---------------

The tool can be constructed in C++ or python using one of three constructors:
  - A constructor taking the name of the tool
  - A constructor taking the cleaning level, which creates a name based on the level
  - A constructor taking both the name and the level

The level can also be explicitly set in job options via the CutLevel property.  The
levels available levels are: VeryLooseBad , LooseBad , MediumBad , and TightBad .  Each
level is described on the TWiki.  There are three interface functions that help you
clean your jets:

  - const ROOT::TAccept& accept( const xAOD::Jet& jet ) : This is the preferred way to
   clean your jets.  The input is an xAOD jet, which has all necessary information attached
   to it.  The result is a ROOT::TAccept object that contains the cleaning decision.

  - int keep(const xAOD::Jet& jet) : This is a trivial forward to the other accept implementation

  - const ROOT::TAccept& accept( ... ) : This is the old way to clean jets.  The function
   takes a long list of doubles that are jet properties.  It should only be used for ntuples - 
   any xAOD-based analysis should prefer one of the previous two functions. 

It is important to note that if the xAOD jet does not have one of its properties available, the
code will be unable to tell.  You will get incorrect cleaning results.  There is not currently
a way that the tool can protect against this issue.

For convenience, the file python/ConfiguredAthJetCleaningTools.py contains several example
instantiations, which can be directly used in your analysis.  The python file
python/JetCleaningCutDefs.py contains a simple helper to set up the tool at a given level.
These functions are a bit of overkill given the current interface, but they may be helpful in
the future if the cleaning functions change significantly.

