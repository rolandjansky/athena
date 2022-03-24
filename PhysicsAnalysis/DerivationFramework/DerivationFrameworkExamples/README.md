# DerivationFrameworkExamples

This package contains a series of examples showing how each aspect of the DAOD building (skimming, slimming, thinning, augmentation, scheduling etc) are done. They also act as unit tests allowing each aspect to be run individual, which is especially useful when migrations are being performed. 

The examples are run using `Reco_tf.py` in the same way as any other DAOD format, via the `reductionConf` flag. Each example format uses a flag of the form `TESTX` (e.g. `TEST1`, `TEST2` etc).

The examples are as follows:

1. `SkimmingExample.py`: `TEST1` - demonstration of skimming via a dedicated tool implemented in C++ 
2. `SkimmingExampleStrings.py`: `TEST2` - demonstration of skimming via the generic string parsing tool
3. `ThinningExample.py`: `TEST3` - demonstration of thinning via the generic string parsing tool
4. `SlimmingExample.py`: `TEST4` - demonstration of slimming using the slimming helper
5. `CPToolsExample.py`: `TEST5` - illustration of scheduling CP tools and using the outcome to select events
6. `PreSelectionExample.py`: `TEST6` - illustration of scheduling CPU-heavy operations after a pre-selection skimming step to avoid running expensive calculations for events that will be rejected
7. `AugmentationExample.py` : `TEST6` - demonstration of augmentation of DAODs with derived information, via object decorations, and simple vectors

## Component Accumulator version

In run 3 the derivation framework will move to the component accumulator and will be build differently. This package also contains examples showing how to run this new component accumulator based derivation framework. Unlike the examples above the config files are found in the python/ directory and are all python modules rather than job options. Usage is:

`Derivation_tf.py --CA --inputAODFile aod.pool.root --outputDAODFile test.pool.root --requiredDerivedFormats TEST1 TEST3 ...`

The examples are very similar to the ones above:

1. `TEST1.py`: `TEST1` - demonstration of skimming via a dedicated tool implemented in C++, plus smart slimming
2. `TEST2.py`: `TEST2` - demonstration of skimming via the generic string parsing tool, plus smart slimming
3. `TEST3.py`: `TEST3` - demonstration of thinning via a dedicated tool implemented in C++, plus smart slimming
4. `TEST4.py`: `TEST4` - demonstration of smart slimming using the slimming helper
5. `TEST5.py`: `TEST5` - illustration of object decoration, using an example tool and a CP (muon) tool
6. `TEST6.py`: `TEST6` - illustration of scheduling CPU-heavy operations after a pre-selection skimming step to avoid running expensive calculations for events that will be rejected

