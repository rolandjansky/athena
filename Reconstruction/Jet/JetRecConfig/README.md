# JetRecConfig: Smart jet reconstruction configuration

## Introduction

This package is an attempt to build up jet configuration hierarchically through
modular configuration helpers that stay local to the source packages.

## Usage

Before reading further, it may be useful to also refer to the Run 3 configuration
documentation:
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/AthenaJobConfigRun3

The backbone of this configuration framework is a set of classes that encode
what is meaningful in a jet definition or jet constituent, such that helper
code can interpret the definitions to generate all necessary tools/algs.
These are defined in `JetDefinition.py`, and currently comprise:
* `JetConstit`: A set of constituents used for jet finding, which may have
  constituent modifiers applied.
* `JetDefinition`: A jet configuration, fundamentally the clustering algorithm,
  radius, and input constituent type. Can be extended with lists of jet
  modifiers. Jet grooming is still to be added (might be a derived class?)
* `JetGhost`: A set of objects to be ghost-associated to the jets. Very
  simplistic class at present.
* `JetModifier`: A configuration of a tool that adds jet moments or otherwise
  manipulates the jet collection.

Once the constituent and definition have been defined, these are passed to
the `JetRecConfig.JetRecCfg` function, which builds up all needed
tools and algorithms sequentially, including any of their prerequisites,
such that one is guaranteed a working reconstruction sequence. These are
placed in a ComponentAccumulator, that can be merged with the main
Athena job and potentially with other jet reconstruction sequences.

As an intermediate step, the dictionary of dependencies for the jet reco job
can be extracted without attempting to configure any Athena tools/algs. This
is done by passing the jet definition to `JetRecConfig.resolveDependencies`,
allowing quick checking of the elements that will be integrated to form the
jet collection, including input collections, ghosts and modifiers.

If working with standard objects, the user can to do something like
```
from StandardJetDefs import AntiKt4LCTopo
lcjetcfg = JetRecConfig.JetRecCfg(jetseq,AntiKt4LCTopo)
```
For minor variations, one can copy and modify the standard definitions:
```
from StandardJetDefs import LCTopoOrigin, AntiKt4LCTopo
from copy import deepcopy
LCTopoCSSK = deepcopy(LCTopoOrigin)
LCTopoCSSK.modifiers += ["CS","SK"]
AntiKt4LCTopoCSSK = deepcopy(AntiKt4LCTopo)
AntiKt4LCTopoCSSK.inputdef = LCTopoCSSK
lccsskjetcfg = JetRecConfig.JetRecCfg(jetseq,AntiKt4LCTopoCSSK)
```
The definitions can of course be built up from scratch by users who want full
control.

At this time, the only use of configuration flags is to peek in the input
file (the first one) to determine if certain prerequisite collections are
present and therefore need not be built.
See:
https://indico.cern.ch/event/697121/contributions/2859415/attachments/1585431/2506572/JobConfig_SoftCoord_Jan18.pdf

### Note on JetModifier configuration

To provide a large variety of modifier tool configurations in an organised way,
each modifier tool is configured via the `JetModifier` helper class. This in
turn delegates setup of the configuration to a helper function that specifies
non-default tool properties, and may take into account the details of the jets.
Every modifier keeps track of its prerequisites, which may be other modifiers,
jet input objects (e.g. tracks) or ghosts (e.g. GhostTracks).

The simplest possible `JetModifier` is one that only needs to produce a default
tool, e.g.
```
jetsort = JetModifier("JetSorter","jetsort")
```
Another modifier may not need any special configuration, but does require that
some other mods be run first:
```
jvt = JetModifier("JetVertexTaggerTool", "jvt",
                  prereqs = [ "mod:JVF" ])
```
Finally, a helper function might be defined to set up the tool correctly,
possibly with input from a string encoding of the modifier setup:
```
def getJetFilterTool(modspec):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    ca = ComponentAccumulator()
    threshold = int(modspec)
    jetptfilter = JetRecConf.JetFilterTool("jetptfilter_{0}mev".format(threshold))
    jetptfilter.PtMin = threshold
    ca.addPublicTool( jetptfilter )
    return jetptfilter, ca

"Filter": JetModifier("JetFilterTool","jetptfilter",helperfn=getJetFilterTool),
```
A helper function can potentially be provided for the prereqs as well.

This permits us to build up a dictionary of modifier configurations, keyed by
strings, so a user can simply set a list of strings describing how they want
to apply the modifiers, e.g.
```
AntiKt4LCTopo.modifiers = ["Calib:AnalysisLatest:mc","Sort","JVT"]
```
which will first ensure that all tools are called that are needed for
calibration, then sort the collection (a filter is also applied, controlled
by other options), and finally compute JVT, again adding all modifiers required
by JVT. In the process, jet track selection and ghost association will be
added to the job.

The set of standard modifiers (for offline reconstruction) is concentrated
in JetRecConfig/python/StandardJetMods.py, which in turn accesses tool
configuration helpers that are defined in the packages containing the tool
C++ implementations. This allows a local lookup of the specific default
configurations, while making it easy to determine where the source code
for those tools live.

### Instructions

To run:
```
cd $MYWORKDIR
mkdir run
cd run
python JetRecTestCfg.py -H # Get instructions
```
You can dump printouts of the dependency dict and/or component accumulators
for each jet collection or run a full job in serial or multithreaded modes.

# TODO

* Set up configuration helpers for:
  * Jet substructure moment tools
* Develop a generic configuration helper for grooming jets
  * This should support both finding+grooming and standalone grooming based
    on the input collection being pre-made.
  * Some autoconfiguration could be useful here? However, missing data
    dependencies should be covered by the AthenaMT scheduler.
* Some steering flags will eventually be needed to cope with different
  situations. However, hopefully they can mostly just modify the top-level
  configurations, without needing anything propagated into the helpers.
