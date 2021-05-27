# JetRecConfig: Smart jet reconstruction configuration

## Introduction

This package is an attempt to build up jet configuration hierarchically through
modular configuration helpers that stay local to the source packages.

## Usage

Before reading further, it may be useful to also refer to the Run 3 configuration
documentation:
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/AthenaJobConfigRun3

The backbone of this configuration framework is a set of classes that encode
what is meaningful in a jet definition or jet constituent. Helper
code will then interpret these definitions to generate all necessary tools/algs.

The definition classes are defined in `JetDefinition.py`, and currently comprise:
* Classes defining the input to jet finding. `JetInputExternal` describes external containers : clusters, tracks, event density... `JetInputConstit` and `JetInputConstitSeq` describe what can be used as constituents of jets.
* `JetDefinition`: A jet configuration, fundamentally the clustering algorithm,
  radius, and input constituent type. Can be extended with lists of jet
  modifiers. 
* `JetModifier`: A configuration of a tool that adds jet moments or otherwise
  manipulates the jet collection.
* There is also jet context dictionnary defined in
  `StandardJetContext.py`. A jet context is simply a set of parameters
  that we want to keep consistent across all components within a
  JetDefinition. These parameters are mainly related to tracks (track
  & vertex container names, trk-vtx association map,...). Each set of
  parameters is kept together within a dict and each dict is stored
  in the `jetContextDic`. For example `jetContextDic["default"]` is
  the parameters for offline reco and `jetContextDic["ftf"]` can be
  used in the trigger config. Each `JetDefinition` has a context
  (which is "default" by default...). 

Once the constituent and definition have been defined, these are passed to
the `JetRecConfig.JetRecCfg` function, which builds up all needed
tools and algorithms sequentially, including any of their prerequisites,
such that one is guaranteed a working reconstruction sequence. These are
placed in a ComponentAccumulator, that can be merged with the main
Athena job and potentially with other jet reconstruction sequences.

As an intermediate step, the dictionary of dependencies for the jet reco job
can be extracted without attempting to configure any Athena tools/algs. This
is done by passing the jet definition to `DependencyHelper.solveDependencies`,
allowing quick checking of the elements that will be integrated to form the
jet collection, including input collections, ghosts and modifiers.

If working with standard objects, the user can to do something like
```
from JetRecConfig.StandardSmallRJets import AntiKt4LCTopo
acc = JetRecConfig.JetRecCfg(AntiKt4LCTopo)
```
For minor variations, one can copy and modify the standard definitions:
```
from JetRecConfig.StandardSmallRJets import AntiKt4LCTopo
from JetRecConfig.StandardJetConstits import stdConstitDic as cst
AntiKt4LCTopoCSSK = AntiKt4LCTopo.clone(inputdef = cst.LCTopoCSSK, modifiers=["Filter:13000"] )

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
turn can delegates setup of the configuration to a helper function (`createfn` argument) that specifies
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

One can pass the properties need for the tool in the ctor of the modifier

```
    ktdr       = JetModifier("KtDeltaRTool", "ktdr", JetRadius = 0.4),
```

Instead of a simple value, the property can be a function. This is
useful when the property needs to depend on the jet definition to
which the modifier is attached. The function will be called with 2
args : the jet definition and a specifier (defaulting to "", see below).

```
def _jetname(jetdef,modspec):
    return jetdef.fullname()

Width =  JetModifier("JetWidthTool", "width", JetContainer = _jetname),
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
by JVT. In the process, jet track selection and ghost association will
be automatically added to the job because they are necessary for JVT.

The set of standard modifiers (for offline reconstruction) is concentrated
in `JetRecConfig.StandardJetMods`, which in turn accesses tool
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
python test_StandardJets.py --help # Get instructions
```
You can dump printouts of the dependency dict and/or component accumulators
for each jet collection or run a full job in serial or multithreaded modes.

# TODO

