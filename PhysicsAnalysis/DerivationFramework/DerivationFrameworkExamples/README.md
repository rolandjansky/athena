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
