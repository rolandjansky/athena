<!--Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration-->

# MET Trigger configuration guide

This document is intended to describe the process of going from a chain name to
a fully configured sequence. It also describes a few naming conventions that we
should stick to...

**Contents:**
- [Chain name decoding](#chain-name-decoding)
  - [Chain name to chain dict conversion](#chain-name-to-chain-dict-conversion)
  - [Decodoing the chain dict in the MET code](#decoding-the-chain-dict-in-the-met-code)
- [Naming conventions](#naming-conventions)
- [Adding new algorithms and configurations](#adding-new-algorithms-and-configurations)
  - [Adding new configurations to an existing algorithm](#adding-new-configurations-to-an-existing-algorithm)
  - [Adding a new algorithm](#adding-a-new-algorithm)

## Chain name decoding

<!-- 
    TODO - right now we don't have any complicated configurations available in
    MT. This means that we can't use a particularly complicated chain to
    demonstrate the decoding process. When we have more complicated
    configuration options we can revisit this documentation
-->

This section will use the chain `HLT_xe110_tcpufit_lcw_xe70_cell_L1XE50` as an
example.

### Chain name to chain dict conversion

Almost all information used to configure the trigger is contained within the
chain name (except certain things like L1 item), however this is not the most
useful format for the actual configuration (every single signature would have to
implement some form of string parsing and it would be very hard to keep this
standardised). Therefore before the chain is passed to the individual slice
configurations it is broken up into a 'chain dictionary'.

This is done (mostly) by the
`TriggerMenuMT.HLT.Config.Utility.DictFromChainName.analyseChainName` function.

> Note that I am not describing the full process done by this function, just the
> most relevant parts. This is also not an official documentation of this
> function, just based on what I understand it to be doing.

The L1 item and `HLT_` prefix are stripped from the name, then a `regex` defined
by the `TriggerMenuMT.HLT.Menu.SignatureDicts.getBasePattern` function
is used to split the chain name into parts. Each part begins with one of the
trigger type identifiers are defined by
`TriggerMenuMT.HLT.Menu.SignatureDicts.SliceIDDict` and, at the time
of writing, are

| Slice        | Identifier  |
| ------------ | ----------- |
| Electron     | `e`         |
| Jet          | `j`         |
| HT           | `ht`        |
| Photon       | `g`         |
| Muon         | `mu`        |
| Tau          | `tau`       |
| MET          | `xe`        |
| XS           | `xs`        |
| TE           | `te`        |
| AFP          | `afp`       |
| MinBias      | `mb`        |
| HeavyIon     | `hi`        |
| Cosmic       | `cosmic`    |
| Calibration  | `calib`     |
| Streaming    | `streamer`  |
| Monitoring   | `mon`       |
| Beamspot     | `beamspot`  |
| EnhancedBias | `eb`        |
| Test         | `TestChain` |

The `regex` splits shortened chain name to pieces that begin with one of these
identifiers and run to the next. So our example becomes 
`[xe110_tcpufit_lcw, xe70_cell]`.

Each of these chains is then interpreted using the appropriate dictionary in the
`TriggerMenuMT.HLT.Menu.SignatureDicts` module. For us `xe` uses the
`METChainParts` dictionary but each signature has its own. 

The chain part is split on underscores and each individual piece is checked
against the values in the signature dict. Where the piece matches a value, the
corresponding property (the key in that dict) is updated: if it's a list, the
piece is appended, otherwise it's replaced. The starting values for the
properties are set by the default signature dict (for us METChainParts_default).

For our example, the following keys will be extracted for each piece

| Key             | `xe110_tcpufit_lcw` | `xe70_cell` |
| --------------- | ------------------  | ----------- |
| `L1threshold`   | `XE50`              | `XE50`      |
| `trigType`      | `xe`                | `xe`        |
| `extra`         |                     |             |
| `EFmuonCorr`    |                     |             |
| `multiplicity`  | `1`                 | `1`         |
| `EFrecoAlg`     | `tcpufit`           | `cell`      |
| `calib`         | `lcw`               | `lcw`       |
| `L2muonCorr`    |                     |             |
| `signature`     | `MET`               | `MET`       |
| `threshold`     | `110`               | `70`        |
| `addInfo`       |                     |             |
| `chainPartName` | `xe110_tcpufit_lcw` | `xe70_cell` |
| `L2recoAlg`     |                     |             |

(Note that the `L1threshold` key is actually supplied as part of the chain
 building step).

The full chain dict will then contain this information and some extra pieces
shown below. Where the description box is empty it's because I don't know what
it's doing

| Property             | Value in example | Description |
| -------------------- | ---------------- | ----------- |
| `EBstep`             |                  |             |
| `signatures`         | `['MET', 'MET']` | The signatures of each chain part |
| `stream`             | `['Main']`       | The list of streams to which this trigger writes |
| `chainParts`         | See above        | See above   |
| `mergingOrder`       | `[]`             |             |
| `eventBuildType`     |                  |             |
| `chainNameHash`      | `2241623090`     | Hashed value of the full chain name |
| `topo`               | `[]`             | List of L1 topo items |
| `mergingOffset`      | `-1`             |             |
| `groups`             | `['RATE:MultiMET', 'BW:MultiMET']` | Which groups this chain is part of |
| `mergingStrategy`    | `parallel`       |             |
| `chainMultiplicities`| `['1', '1']`     | The multiplicities of each substep |
| `topoStartFrom`      | `False`          |             |
| `L1item`             | `L1_XE50`        | The L1 item that seeds this chain |
| `chainName`          | `HLT_xe110_tcpufit_lcw_xe70_cell_L1XE50` | The full chain name |

The entry point to the signature configuration is then
`TriggerMenuMT.HLT.<sigFolder>.Generate<sig>ChainDefs.generateChainConfigs`
where `sigFolder` and `sig` are determined by the `signatures` and will be `MET`
for us. This master chain dict is broken up into pieces that share a signature
(so an electron+MET chain would be split into those parts for example) and each
signature's code only sees the parts that it knows how to deal with.

### Decoding the chain dict in the MET code

The `TriggerMenuMT.HLT.MET.GenerateMETChainDefs.generateChainConfigs`
function receives the dict described above. It first breaks it down one step
further, producing a copy per `chainPart`. The only difference between these
copies and the full dict is that the value referenced by the `chainParts` key is
the dictionary corresponding to that particular `chainPart`, rather than the
full list.

This `chainParts` dictionary contains all the relevant information for the
sequence configuration. In fact, it contains **more** information than is
necessary for this. For this purpose, we construct a subset of this called
`recoDict` using the
`TriggerMenuMT.HLT.MET.ConfigHelpers.extractMETRecoDict` function. The
configuration of the MET reco sequences is *completely* determined by the
information in this dict.

There are three types of sequence that can be configured by this code

- **Reco sequences** \
  The reco sequence contains everything to actually calculate the MET values,
  including sequencing any inputs
- **Ath sequences** \
  The 'ath' sequence adds the requisite 'input maker' to the reco sequence. For
  the MET slice we always run in a full-scan environment and always use the
  `clusterFSInputMaker`
- **Menu sequences** \
  The menu sequence describes a full 'input maker' + reco + hypo grouping, i.e.
  a segment that calculates a MET value and then creates a decision using it

All of these are divided into 'steps'. As hypos only appear at the end of each
step these are the only points at which early rejection is possible. This
motivates running CPU intensive reconstruction in later steps.

The 'difficult' part of this is the first part - the reco sequence. The MET code
uses a helper class
`TriggerMenuMT.HLT.MET.METRecoSequences.AlgConfig`.
These are split by `EFrecoAlg`, which allows the code to select the correct
config class, which then knows how to interpret the rest of the `recoDict` to
correctly configure the sequence. (In fact this class is used to configure all
stages).

These then further use helper classes based on
`TriggerMenuMT.HLT.MET.AlgInputConfig.AlgInputConfig`
to provide the input sequences (e.g. clusters, tracks).

In our example, `xe110_tcpufit_lcw` gets converted into the `recoDict`
``` python
    {
        'EFrecoAlg' : 'tcpufit',
        'calib'     : 'lcw',
        'addInfo'   : ''
    }
```
This is passed to the `TCPufitConfig` class, which sets up the input sequence
creating the clusters and the MET algorithm calculating the MET according to the
`tcpufit` algorithm. The cluster input sequence is set up by the
`ClusterInputConfig` class.

The same happens to the `xe70_cell` piece, except it gets passed to the
`CellConfig` class.

## Naming conventions

There are several different elements being set up by the MET configuration code,
and these should follow these conventions (this is largely enforced by the code
structure).

Each unique `recoDict` should define a unique configuration and therefore needs
to have a unique name. This is achieved by joining together the values from this
dictionary with underscores. This is done by the
`TriggerMenuMT.HLT.MET.ConfigHelpers.metRecoDictToString` function and
the order in which this happens is determined by the
`TriggerMenuMT.HLT.MET.ConfigHelpers.recoKeys` list.
Any values that are equal to the defaults will be skipped.

The naming conventions are then as follows (where 'suffix' represents the string
described above).

| Object        | Name                |
| ------------- | ------------------- |
| Chain step    | `Step1_met_suffix`  |
| Hypo alg      | `METHypoAlg_suffix` |
| Ath sequence  | `METAthSeq_suffix`  |
| Reco sequence | `METRecoSeq_suffix` |
| Reco alg      | `EFMET_suffix`      |
| MET Container | `HLT_MET_suffix`    |

Additionally, for readability reasons, the names of the `AlgConfig` classes
should match the names of the Fex algorithms themselves and these should be
capitalised versions of the `EFrecoAlg` names.

| EFrecoAlg | Fex Class              | Config Class    |
| --------- | ---------------------- | --------------- |
| `cell`    | `HLT::MET::CellFex`    | `CellConfig`    |
| `mht`     | `HLT::MET::MHTFex`     | `MHTConfig`     |
| `tc`      | `HLT::MET::TCFex`      | `TCConfig`      |
| `tcpufit` | `HLT::MET::TCPufitFex` | `TCPufitConfig` |
| `trkmht`  | `HLT::MET::TrkMHTFex`  | `TrkMHTConfig`  |

Right now there is a one-to-one correspondence between these things (one
EFrecoAlg corresponds to one C++ class corresponds to one `AlgConfig` class) but
only the one-to-one `EFrecoAlg` to `AlgConfig` is really required. One could
imagine a case where one `AlgConfig` class could wish to conditionally set up
different C++ classes or multiple `AlgConfig` classes could wish to set up
different configurations of the same C++ class. **However** this should be well
justified if we decide to do it.

## Adding new algorithms and configurations

This section describes the steps necessary for adding a new algorithm to this
code or for adding new configurations to an existing algorithm.

### Adding new configurations to an existing algorithm

Here 'new configurations' means either adding new properties to a C++ object or
exposing existing properties to the menu configuration (i.e. introducing a new
mapping from the trigger names to athena configurations).

Typically this will involve adding new values to the existing `METChainParts`
dictionary. In the simplest cases you will not have to add any new keys to this
dictionary. If you do however, make sure that you add them to
`TriggerMenuMT.HLT.MET.ConfigHelpers.recoKeys` or your chain will
likely not be configured properly. Try to think carefully about how you add them
to this list - it defines the order in which they appear in all the object
names.

Once you have done this, you need to ensure that your `AlgConfig` class
correctly receives the property in its constructor (and passes it to the base
class).

As you will be creating a new unique suffix(es), you will also be creating a new
output container(s) `HLT_MET_suffix`. This, and the corresponding aux container,
will need to be added to `TrigEDMConfig.TriggerEDMRun3`.

### Adding a new algorithm

A new algorithm requires more work as you will need to create a new `AlgConfig`
class and (at least usually) new C++ classes. You will also need to add a your
new algorithm name to the `EFrecoAlg` field in `METChainParts`. You'll also need
to make sure you've followed the steps for adding new configurations.

#### Creating a new `AlgConfig` class

New `AlgConfig` classes should be added to the
`TriggerMenuMT.HLT.MET.AlgConfigs` module. The reason for this is that
this module is imported in the `ConfigHelpers` module (where `AlgConfig` is
defined) so it's guaranteed that your class will be known to python when
`AlgConfig.fromRecoDict` is called.

A new `AlgConfig` class must define two things
- a `@classmethod` called `algType` that returns the name of the EFrecoAlg that
  it defines
- an `__init__` method that receives the `recoDict` and calculates the necessary
  inputs. The base class `__init__` method must receive the **whole** `recoDict`
  or it will not construct the correct names.
- a make_fex function

Look at the other classes in that module for inspiration and consult the
`AlgConfig` docstring as well.

#### Creating a new input config

You may require a new input config as well (i.e. something which calculates
an input object for one of the MET calculations). In order to do this, read the
documentation in `TriggerMenuMT.HLT.MET.AlgInputConfig` and check the
existing versions in `TriggerMenuMT.HLT.MET.METRecoSequences`.

#### Creating a new C++ class

Where possible new C++ Fex classes should following the naming convention
defined above. They should be defined in the `HLT::MET` namespace and inherit
from `HLT::MET::FexBase`.

All a derived class **has** to define is the `fillMET` function which does the
actual calculation and the `initialize` function. From the `initialize` function
`initializeBase` must be called with a list of the names of all the extra
components the final `xAOD::TrigMissingET` object will contain.

The base class contains other methods that can be overridden. See the
[doxygen page](https://atlas-sw-doxygen.web.cern.ch/atlas-sw-doxygen/atlas_22.0.X-DOX/docs/html/dc/d5c/classHLT_1_1MET_1_1FexBase.html)
for more details.
