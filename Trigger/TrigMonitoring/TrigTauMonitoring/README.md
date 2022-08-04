# TrigTauMonitoring:

Tau offline monitoring package.

## How to Run standalone:

Execute the tau monitoring locally excluding all other signatures monitoring.

### Run:
```
Run3DQTestingDriver.py --dqOffByDefault Input.Files="['path_to_myxAOD.root file']" DQ.Steering.doHLTMon=True DQ.Steering.HLT.doBjet=False DQ.Steering.HLT.doBphys=False DQ.Steering.HLT.doCalo=False DQ.Steering.HLT.doEgamma=False DQ.Steering.HLT.doJet=False DQ.Steering.HLT.doMET=False DQ.Steering.HLT.doMinBias=False DQ.Steering.HLT.doMuon=False DQ.Steering.HLT.doInDet=False 
```

## How to Run on GRID:

### Setup athena:
```
setupATLAS
lsetup git
asetup Athena,master,latest,here
```
### Local Installation (optional):

In case you would like to run with a modified version of the monitoring, you would need first to check the TrigTauMonitoring monitoring package following the instructions from:

Clone Athena locally:  https://atlassoftwaredocs.web.cern.ch/gittutorial/git-clone/

and

Compile the TrigTauMonitoring package : https://atlassoftwaredocs.web.cern.ch/gittutorial/branch-and-change/

### Run on the GRID :

Execute the tau monitoring on the GRID excluding all other signatures monitoring.

```
pathena --trf "Run3DQTestingDriver.py --dqOffByDefault DQ.Steering.doHLTMon=True DQ.Steering.HLT.doBjet=False DQ.Steering.HLT.doBphys=False DQ.Steering.HLT.doCalo=False DQ.Steering.HLT.doEgamma=False DQ.Steering.HLT.doJet=False DQ.Steering.HLT.doMET=False DQ.Steering.HLT.doMinBias=False DQ.Steering.HLT.doMuon=False DQ.Steering.HLT.doInDet=False --inputFiles=%IN" --inDS=MyxAOD.root --extOutFile=ExampleMonitorOutput.root --outDS=user.myname.myMonPlots.root

```

