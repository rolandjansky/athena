========================================================
PhysicsAnalysis/JetMissingEtID/JetSelectorTools:
   Package hosting jet selection (cleaning) tools
========================================================

:authors: Julia Gonski
:contact: j.gonski@cern.ch

.. meta::
   :description: JetSelectorTools: Package hosting jet selection (cleaning) tools
   :keywords: jet, JetSelectorTools

.. contents:: Table of contents

------------
Introduction
------------

This package contains dual-use tools for selecting good jets passing one of
several levels of jet cleaning cuts.  There are two tools, the **JetCleaningTool**,
which is configurable and provides a good/bad decision for each jet, and the 
**EventCleaningTool**, which performs the full jet cleaning procedure for 
an event. 

The documentation is on the twiki here:
`HowToCleanJets2017
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HowToCleanJets2017>`_.

This procedure applies to all of Run 2. 

------------------------------
Checkout & Setup in Release 21
------------------------------

mkdir source build run 
cd source 

git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
cd athena
git atlas addpkg JetSelectorTools
git checkout -b topic-name upstream/21.2 --no-track

Edit source/athena/Projects/WorkDir/package_filters_example.txt to include the packages
you want to compile.
cd ../../build

asetup AthDerivation,21.2.13.0 (or latest) 
cmake ../source/athena/Projects/WorkDir/
make -j8 
source x86_64-slc6-gcc62-opt/setup.sh

NOTE: currently you will see the following warning when you compile. This is not
relevant to the tool performance and can be disregarded. 

In file included from input_line_9:103:
..... /source/athena/PhysicsAnalysis/JetMissingEtID/JetSelectorTools/JetSelectorTools/EventCleaningTool.h:55:24: warning: 'finalize' overrides a
      member function but is not marked 'override' [-Winconsistent-missing-override]
    virtual StatusCode finalize();
                       ^
/cvmfs/atlas.cern.ch/repo/sw/software/21.2/GAUDI/21.2.13.0/InstallArea/x86_64-slc6-gcc62-opt/include/GaudiKernel/AlgTool.h:74:14: note: overridden virtual function is here
  StatusCode finalize() override;

-----------------------
Jet Cleaning Tool Usage
-----------------------

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

-------------------------
Event Cleaning Tool Usage
-------------------------

The EventCleaningTool is designed to assess the cleanliness of every jet in an event. The tool 
runs over a jet collection which is required to have JVT and overlap removal decorations applied 
prior to event cleaning. For each jet in the collection, the individual cleaning decision is made 
with a JetCleaningTool that is created by the EventCleaningTool. If the jet passes the criteria 
needed for cleaning consideration (passing pT, eta cuts, along with JVT and OR requirements), 
it is decorated as either clean or not clean. As per the official jet cleaning recommendation,
the event level decision is taken as an AND of all jet cleaning decisions; if any event has an
unclean jet, the whole event is flagged as unclean. 

The methods in the tool are: 
    - virtual bool acceptEvent(const xAOD::JetContainer* jets) : Loops over the input decorated jet 
    collection. Checks pT, eta, JVT, and OR. If a jet passes all four of these, the JetCleaningTool 
    is used to check its cleanliness, and it is decorated accordingly. If a jet fails any of these four 
    checks, it is decorated as bad, but will not affect the event level decision. Returns the event 
    cleaning decision as a boolean. 

    - virtual int keep_jet(const xAOD::Jet& jet) const override : Simply calls the keep method 
    of the JetCleaningTool.

The properties of the tool are: 
   - Minimum jet pT cut for cleaning consideration (default 20.0 GeV)
   - Maximum eta cut for cleaning consideration (default 2.8) 
   - The name of the JVT decorator for cleaning consideration (default 'passJvt')
   - The name of the OR decorator for cleaning consideration (default 'passOR') 
   - The prefix for the name of the decoration (default empty)
   - A boolean for whether you want to do the jet clean decorations (default 'true')
   - The cleaning level (default 'LooseBad') for initializing the JetCleaningTool
   - The AnaToolHandle for the JetCleaningTool itself. 

In addition to the EventCleaningTool, there is an EventCleaningTestAlg which calls acceptEvent 
once per event. The algorithm should be passed a configured EventCleaningTool (see ExtendedJetCommon
for an example). The algorithm has an additional property, the name of the jet collection 
(default "AntiKt4EMTopoJets"). It then uses this name to pull the jet collection out of event store. 
The event decision is decorated onto the EventInfo object, using the boolean return value of acceptEvent.

A sample job options file for the algorithm can be found in share/EventCleaningTest_jobOptions.py. 

NOTE: Because the algorithm relies on JVT and OR decorations, it can only be used in an algorithm
sequence where these decorations have already been performed (for example, applyST.py in SUSYTools.) 
If you reach an error like the one below, you need to decorate your AOD first.
ERROR SG::ExcBadAuxVar: Attempt to retrieve nonexistent aux data item `::passJvt' (28).






