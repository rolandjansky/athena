# TrigEgammaMonitoring:

Egamma offline monitoring package.

## How to Run standalone:

Execute the egamma monitoring locally.

### Run:
```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000
```

### Run with emulator (default):

Default configuration can be found [here](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TriggerMenuMT/python/HLTMenuConfig/Egamma/EgammaDefs.py#L22).

```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000 --emulate
```

### Run with emulator (custom):

```
RunTrigEgammaMonitoring.py -i path/to/AOD.root --nov 1000 --emulate --dnnConfigPath "ElectronPhotonSelectorTools/offline/mc16_20210430" --ringerConfigPath "RingerSelectorTools/TrigL2_20210702_r4"
```

