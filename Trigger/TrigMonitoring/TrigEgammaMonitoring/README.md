# TrigEgammaMonitoring:

Egamma offline monitoring package.

## How to Run standalone:

Execute the egamma monitoring locally.

### Run:
```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000
```

### Run with emulator (default):

Default configuration can be found [here](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TriggerMenuMT/python/HLT/Egamma/EgammaDefs.py#L22).

```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000 --emulate
```

### Run with emulator (custom):

```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000 --emulate --dnnConfigPath "ElectronPhotonSelectorTools/offline/mc16_20210430" --ringerConfigPath "RingerSelectorTools/TrigL2_20210702_r4"
```

## How to Run on GRID:

### Setup athena:
```
setupATLAS
lsetup git
asetup Athena,master,latest,here
```
### Local Installation (optional):

```
git atlas init-workdir ssh://git@gitlab.cern.ch:7999/jodafons/athena.git athena
cd athena
git fetch upstream
git checkout -b my_modifications
git atlas addpkg TrigEgammaMonitoring
```

### Run with prun:
```
RunTrigEgammaMonitoringOnGrid.py --inDS valid1.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e5112_s3214_d1709_r13044_tid26720967_00 --outDS user.jodafons.my_test_1
```

### Run with prun and emulator:

```
RunTrigEgammaMonitoringOnGrid.py --inDS valid1.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e5112_s3214_d1709_r13044_tid26720967_00 --outDS user.jodafons.my_test_1 --emulate --dnnConfigPath "ElectronPhotonSelectorTools/offline/mc16_20210430" --ringerConfigPath "RingerSelectorTools/TrigL2_20210702_r4"
```
