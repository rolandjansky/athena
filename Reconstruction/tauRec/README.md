# tauRec package

This package is the python steering package for tau reconstruction and the c++ algorithms. This package has two algorithms: TauProcessorAlg (also referred to as TauBuilder in some places) and TauRunnerAlg that call tools defined in tauRecTools. This page contains a quick summary of the structure with links to the relevant code. For more details please see the [tauRec r22 tWiki page](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauRecR22).

### Tau Reconstruction chain:

There are a total of 4 algorithms, which are scheduled in the following order: jetTrackAlg, TauBuilder, tauPi0ClusterMaker, and TauRecRunner. The TauBuilder and TauRunner are the main tau algorithms, each of which runs a number of [tauRecTools](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/).

0. Reco_tf.py invokes [CombinedRec_config.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/RecExample/RecExCommon/share/CombinedRec_config.py#L121)
which calls the tau joboptions file: [tauRec_config.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/share/tauRec_config.py).

1. tauRec_config calls [tauRec_jobOptions.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/share/tauRec_jobOptions.py) (and [DiTauRec_config.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/DiTauRec/share/DiTauRec_config.py) if required). tauRec_jobOptions then calls scripts to set up each of the required algorithms:
2. TauRecCoreBuilder, defined in [TauRecBuilder.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecBuilder.py). Sets up the first two algorithms using [TauRecConfigured](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecConfigured.py).
3. Pi0ClusterMaker. This is a CaloRec algorithm, which is run using its own job options [Pi0ClusterMaker_jobOptions.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/share/Pi0ClusterMaker_jobOptions.py). 
4. TauRecRunner, defined in [TauRecRunner.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecRunner.py). Sets the last algorithm using [TauRecRunConfigured.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecRunConfigured.py).

The tool definitions are defined in [TauAlgorithmsHolder.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauAlgorithmsHolder.py)

### TauRecBuilder

Only one class of this is used in r22, the TauRecCoreBuilder:
  * Creates tau candidates from jet seeds
  * Builds a tau candidate if jet seed passes minimal kinematic criteria
  * Associates a vertex
  * Associates tracks
  * Classifies tracks
  * Builds ID variables that require cells or athena geometry
  * Creates Pi0 candidates

TauRecCoreBuilder sets up the required tools. TauRecConfigured is used to configure the algorithm and add the tools to it. 

### TauRecRunner

This algorithm runs calculations that require input from the tools scheduled in the previous algorithms.

  * Construct pi0 clusters (part of substructure)
  * Compute common variables used in tau ID and energy scale calculations
  * Run substructure tools
  * Run PanTau
  * MVA TES
  * TauID
  * TauID score flattening
  * Evaluate and decorate BDT/RNN scores

It has a similar structure to TauRecBuilder, with its own TauRecRunConfigured, used for the algorithm settings and adding the tools.

### TauAlgorithmsHolder

This module contains functions for configuring each tauRecTool.  In the case where multiple instances of a tool may be used, a flexible function is created to retrieve
the instance you want.  For example, the [TauJetBDTEvaluator](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/tauRecTools/TauJetBDTEvaluator.h) is used numerous times : 1p taus, 3p taus, 3 different eta bins for Electron BDT.

```python

def getTauJetBDTEvaluator(_n, weightsFile="", minNTracks=0, maxNTracks=10000, outputVarName="BDTJetScore", minAbsTrackEta=-1, maxAbsTrackEta=-1):
    _name = sPrefix + _n
    from tauRecTools.tauRecToolsConf import TauJetBDTEvaluator
    myTauJetBDTEvaluator = TauJetBDTEvaluator(name=_name,
                                              weightsFile=weightsFile,
                                              minNTracks=minNTracks,
                                              maxNTracks=maxNTracks,
                                              minAbsTrackEta=minAbsTrackEta,
                                              maxAbsTrackEta=maxAbsTrackEta,
                                              outputVarName=outputVarName)
    cached_instances[_name] = myTauJetBDTEvaluator
    return myTauJetBDTEvaluator
```

### tauRecFlags

In order to determine the actual settings for tools, one needs to consult both the TauAlgorithmsHolder module or the TauRecBuilder module--in cases where the
TauAlgorithmsHolder function allows for customization of the tool (see TauJetBDTEvaluator above).  You may also see places in TauAlgorithmsHolder where
the 'declareProperty' is set via a 'tauFlag', as defined in [tauRecFlags](https://gitlab.cern.ch/adbailey/athena/-/blob/master/Reconstruction/tauRec/python/tauRecFlags.py). The reason for this extra level of abstraction is at least two fold:

1. define a variable in one place to be used in numerous tools (e.g. cvmfs CALIBPATH folder)
2. allow for the setting of variables on the command line: Reco_tf.py --preExec 'from tauRec.tauRecFlags import tauFlags; tauFlags.abc=d'

Being able to set variables on the command line is extremely important.  In this way we can request an r-tag without making any code changes.  We did this
while finalizing the TauTrack Classification variables.

