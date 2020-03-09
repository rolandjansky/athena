# tauRec package

This package is primarily the python steering package for tau reconstruction.  This package has one
has one factory algorithm: TauProcessorAlg that calls tools defined in tauRecTools.

## Tau Reconstruction chain:

0. Reco_tf.py invokes [CombinedRec_config.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/RecExample/RecExCommon/share/CombinedRec_config.py#L121)
which calls the tau joboptions file: [tauRec_config.py](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/share/tauRec_config.py).

1. tauRec_config calls tauRec_jobOptions and DiTauRec_config.py
2. tauRec_jobOptions imports classes from [TauRecBuilder](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecBuilder.py)
3. classes in TauRecBuilder :
  * declare tool drivers from tauRecTools: [TauBuilderTool](https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/tauRecTools/TauBuilderTool.h) and [TauProcessorTool]((https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/tauRecTools/TauProcessorTool.h)
  * associate tools to the tool drivers
  * add tools to ToolSvc
  * wrap the tool drivers in TauProcessorAlg instances
  * add TauProcessorAlg to the topSequence
4. Majority of tool definitions are defined in [TauAlgorithmsHolder.py]((https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauAlgorithmsHolder.py)

### TauRecBuilder

This module has three classes, all public [TauRecConfigured.py]((https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRec/python/TauRecConfigured.py):

1. TauRecCoreBuilder
  * starts from jet seeds
  * builds a tau candidate if jet seed passes minimal kinematic criteria
  * associates a vertex
  * associates tracks
  * classifies tracks
  * Builds ID variables that require cells or athena geometry
  * NOTE conversion algorithms are not used as far as I know

1a. Special clusters are built at this point using non-tau algrithms which necessitate a break in tau reconstruction.  This is something
that needs to be addressed in athenaMT.  So that we can combine all thre classes into one.

2. TauRecPi0EflowProcessor
  * Construct pi0 clusters (part of substructure)

3. TauRecVariablesProcessor
  * compute common variables used in tau ID and energy scale calculations
  * run substructure algorithms
  * run PanTau
  * MVATES
  * TauID
  * TauID score flattening

Note the last two classes could be combined into one.

### TauRecConfigured class

The motivation for this class is to simplify some tedious tasks:

1. Wrap the tool driver class with a TauProcessorAlg instance.
2. Add tools to the ToolSvc
3. Set some common flags of the tools, e.g. the cvmfs path for the calibration files.  (the path is property of the [TauRecToolBase class]((https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/tauRecTools/TauRecToolBase.h)).

### TauAlgorithmsHolder

This module contains functions for returning tauRecTool instances.  In the case where multiple instances of a tool may be used, a flexible function is created to retrieve
the instance you want.  For example, the [TauJetBDTEvaluator]((https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/tauRecTools/tauRecTools/TauJetBDTEvaluator.h) is used numerous times : 1p taus, 3p taus, 3 different eta bins for Electron BDT.

```python

def getTauJetBDTEvaluator(_n, weightsFile="", minNTracks=0, maxNTracks=10000, outputVarName="BDTJetScore", GradiantBoost=True, minAbsTrackEta=-1, maxAbsTrackEta=-1):
    _name = sPrefix + _n
    from tauRecTools.tauRecToolsConf import TauJetBDTEvaluator
    myTauJetBDTEvaluator = TauJetBDTEvaluator(name=_name,
                                              weightsFile=weightsFile, #update config?
                                              minNTracks=minNTracks,
                                              maxNTracks=maxNTracks,
                                              minAbsTrackEta=minAbsTrackEta,
                                              maxAbsTrackEta=maxAbsTrackEta,
                                              outputVarName=outputVarName,
                                              GradiantBoost=GradiantBoost)
    cached_instances[_name] = myTauJetBDTEvaluator
    return myTauJetBDTEvaluator
```

### tauRecFlags

In order to determine the actual settings for tools, one needs to consult both the TauAlgorithmsHolder module or the TauRecBuilder modcule--in cases where the
TauAlgorithmsHolder function allows for custimization of the tool (see TauJetBDTEvaluator above).  You may also see places in TauAlgorithmsHolder where
the 'declareProperty' is set via a 'tauFlag'.  The reason for this extra level of abstraction is at least two fold:

1. define a variable in one place to be used in numerous tools (e.g. cvmfs CALIBPATH folder)
2. allow for the setting of variables on the command line: Reco_tf.py --preExec 'from tauRec.tauRecFlags import tauFlags; tauFlags.abc=d'

Being able to set variables on the command line is extremely important.  In this way we can request an r-tag without making any code changes.  We did this
while finalizing the TauTrack Classification variables.

