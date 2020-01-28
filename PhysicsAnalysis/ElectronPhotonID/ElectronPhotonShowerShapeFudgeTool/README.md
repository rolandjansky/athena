# Readme

This is a prototype tool for the application of isolation and shower shape corrections
to variables in the derivation of DxAODs.

It will live in `/head/athena/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool/`.

## Directory Structure

You will find (or should put files to):

- Configuration files in `./data/`
- Header files in `./ElectronPhotonShowerShapeFudgeTool/`
- Source files in `./Root/`
- Unit test files in `./util/`
- Compiled files in `./build/` -- leave them alone (not version controlled)

## Tool Structure

The design of this tool follows the design of the AsgElectronLikelihoodTool.

## How to use the tool (blackbox use)

Description coming, for now contact [Nils Gillwald](mailto:nils.gillwald@desy.de) if you have any questions.

Root file used for testing: `/pnfs/desy.de/atlas/dq2/atlaslocalgroupdisk/rucio/mc16_13TeV/da/80/DAOD_HIGG1D2.18400890._000001.pool.root.1`

## How to change and adapt the tool (non-blackbox use)

### Checkout

First, you need to set up your area:

```bash
mkdir testarea
cd testarea
mkdir build run source
```

Then, to checkout the package, do

```bash
cd source
git clone ssh://git@gitlab.cern.ch:7999/nigillwa/fftoolapply.git
```

### Setup and Compile

The tool is developed using release 21.2.97. To compile, do in the `source` directory

```bash
asetup AnalysisBase,21.2.97,here #just needed once per login session
cd $TestArea/../build
cmake ../source
cmake --build .
```

To be able to run executables, run (only once per login session)

```bash
source $TestArea/../build/$CMTCONFIG/setup.sh
```

The acccording code the compiler sees is in `/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.97/InstallArea/x86_64-centos7-gcc8-opt/src/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool`, as seen by the first path `echo`ed from `$CMAKE_PREFIX_PATH`

### Add a new parameter type for the correction function

If possible, use the provided parameter types which are already implemented and tested. However, if you need to add a new parameter type which needs to have access to some variable which is not currently retreived, here is how to do it.

1. Add the new parameter type to the `enum ParameterType` in the class header. Please make sure to generalize the different parameter types as much as possible while keeping the correction function concise but readable.
2. Add a (private!) getter function for your parameter type. This getter function should be doing all the "dirty" pull work and only return the final correction parameter (possibly as a function of eta and/or pT). If you want an example, have a look at the already implemented functions. Make sure your getter function follows the form `const StatusCode GetNewParameter(float& return_parameter, const& list of needed inputs)`.
3. Add the code to be executed if the correction parameter is of your new parameter type to the following functions:
    - `FFToolApply::GetParameterInformationFromConf`
    - `FFToolApply::GetCorrectionParameters`
    - `FFToolApply::StringToParameterType`
If there is a style which all the other types use to implement their functionalities, follow it. This holds especially for `FFToolApply::GetCorrectionParameters`.
4. Test the functionality of your new parameter type.
5. Add an example of how to use your new parameter type to the example conf file in `./util/`.
6. Submit a merge request to officially add your new parameter type to the tool.
