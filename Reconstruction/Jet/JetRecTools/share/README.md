# JetRecTools jobOptions

## JO list

* `BuildModifiedConstits.py` -- Example for reading in an AOD file and writing out an xAOD containing topoclusters and PFOs with constituent modifications (SoftKiller). Makes use of `JetRecTools/python/ConstModHelpers.py`
* `BuildConstitModJets.py` -- Example for reading in an AOD file and writing out an xAOD containing jets reconstructed from topoclusters and PFOs with constituent modifications (SoftKiller). Makes use of `JetRecTools/python/ConstModHelpers.py`
* `BuildConstitModJetMET.py` -- Example for reading in an AOD file and writing out an xAOD containing jets and MET reconstructed from topoclusters and PFOs with constituent modifications (SoftKiller). Makes use of `JetRecTools/python/ConstModHelpers.py`

## Executing jobOptions

The JOs in this package make use of the "modern" approach to supplying input files, i.e. command line arguments. They can be called with e.g.:
`athena BuildModifiedConstituents.py --filesInput [comma-separated list of files] --evtMax [number of events to run]`

It is recommended to use an AthDerivation 21.2 release to execute these JOs. AthAnalysis can in principle be used for `BuildModifiedConstits.py` and `BuildConstitModJets.py`, provided the necessary packages are installed.

## Outdated JO

* `JetTowerBuilder_jobOptions.py` --  Builds topotowers (towers from cells in topoclusters) and jets from the topotowers. Requires cell access.
* `runJetReco_topOptions.py` -- [Outdated] Calls `JetTowerBuilder_jobOptions.py`, then writes jets to CBNT.
* `samp.wt.CT7.py` -- [Outdated] Sampling weights for calibration?
* `samp.wt.CTfTC7.py` -- [Outdated] Sampling weights for calibration?
* `samp.wt.TC7.py` -- [Outdated] Sampling weights for calibration?


> Written with [StackEdit](https://stackedit.io/).
