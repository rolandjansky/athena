# Readme

This is a tool for the application of MC corrections to photon and electron auxiliary variables in the derivation of DxAODs: `ElectronPhotonVariableCorrectionTool` allowing to correct multiple variables of one photon or electron simultaneously using the `ElectronPhotonVariableCorrectionBase` class.

It lives in `/head/athena/PhysicsAnalysis/ElectronPhotonID/EGammaVariableCorrection/`.

This README first explains [how to use the `ElectronPhotonVariableCorrectionTool`](#how-to-use-the-tool-single-and-multiple-variable-correction-user-manual). In this section, everything you need in order to setup the tool for corrections and how to integrate it in your code is explained - i.e., everything needed from a user perspective. Also, it explains how to use the tool for applying Gaussian smearing to variables.

Second, the deeper mechanics of the both the `ElectronPhotonVariableCorrectionTool` and the `ElectronPhotonVariableCorrectionBase` class are explained ([in this section](#how-to-change-and-adapt-the-tool-developer-manual)). This section is meant for developers / maintainers of the code, i.e. you only need to read it if you want to change the tool itself.

If you have any questions or requests, please contact [Nils Gillwald](mailto:nils.gillwald@desy.de).

## How to use the tool: single and multiple variable correction (user manual)

This section is meant for **users** of the tool. If you want to read about the deeper mechanics of the tool or want to adapt the inner tool mechanics to your needs, read [this section](#how-to-change-and-adapt-the-tool-developer-manual).

The `ElectronPhotonVariableCorrectionTool` (from now on referred to as tool) is designed to be used embedded in a code which provides it with the objects which should be corrected. It is not able to open root files or retrieve trees or apply object selection itself, but only focuses on the correction of objects passed to it.

You can use one instance of the tool to correct electrons, converted photons and unconverted photons at the same time.

### Instantiate the tool using AnaToolHandle (recommended)

The tool must be integrated in code which provides it with the objects which should be corrected. To include the tool, you need to include its interface via `#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"`. Also, you need to include the according tool handler, for example `#include "AsgTools/AnaToolHandle.h"`. All the other includes needed depend on the wrapping code which provides the objects to the tool.

To declare the tool via the tool handler, you need to do something like this:

```C++
asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> MyTool("ElectronPhotonVariableCorrectionTool/myToolName");
```

The syntax of this command depends on your tool handler. Note that the part of the string provided to the constructor before the forward slash defines which tool from the interface is constructed by the tool handler, and the part after the slash defines the name of the tool!

### Instantiate the tool without using a Tool Handler (not recommended)

As before, the tool must be integrated in code which provides it with the objects which should be corrected. To include the tool, you need to `#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionTool.h"`. All the other includes needed depend on the wrapping code which provides the objects to the tool.

In order to declare the tool, it needs to be named in the constructor, so it can be distinguished. This could look like this:

```C++
ElectronPhotonVariableCorrectionTool MyTool("myToolName");
```

### Initialization of the tool

To initialize the tool, first the configuration file must be provided to the tool using `MyTool.setProperty("ConfigFile",/path/)`. Then, the initialize function must be called on the tool, so it can read out the configuration file and set itself up. This could look like this:

```C++
std::string configFilePath = "MyData/MyConfFile.conf";
ANA_CHECK(MyTool.setProperty("ConfigFile",configFilePath));
ANA_CHECK(MyTool.initialize());
```

The syntax of the configuration files is explained [below](#constructing-a-configuration-file).

### Usage of the tool for the correction of EGamma objects

To correct an object, the object must be passed to the tool. **The tool will overwrite the original properties of the object** -- it will however keep the original value, stored as `variableName_original`. This means that the tool cannot be run on const containers -- a shallow or deep copy must be used. Please refer to the official documentation / tutorials on how to shallow / deep copy a container, if needed. Note that you do not have to pass all photons in an event to the tool, just the ones you want to be corrected.

Assuming a writeable object is used, the code for correcting it using the tool looks for example like this:

```C++
ANA_CHECK(MyTool->applyCorrection(*photon));
```

If electrons should be corrected, of course an electron should be passed to the tool instead.

As mentioned before, the tool overwrites the original variable value but also stores the original values as `variableName_original`. The variables can be accessed in the following way:

```C++
std::string correctionVariable = "nameOfTheVariableYouCorrected";
// construct AUX element accessors
SG::AuxElement::Accessor<float> VariableToCorrect(correctionVariable + "_original");
SG::AuxElement::Accessor<float> CorrectedVariable(correctionVariable);
// get uncorrected and corrected variable value for this photon
float variable_original = VariableToCorrect(*photon);
float variable_corrected = CorrectedVariable(*photon);
```

There is no finalize function which needs to be run. Example code using the tool for correcting photons and electrons can be found in `./util`:

- `testElectronPhotonVariableCorrectionTool.cxx`

### Constructing a configuration file

To steer the tool, configuration files must be passed to it. There are two different types of configuration files which need to be provided to the tool:

- configuration files containing information on how the respective variable should be corrected (referred to as basic configuration file) (explained in [this section](#the-basic-configuration-file))
- a configuration file containing a list of all basic configuration files (referred to as tool configuration file) (explained in [this section](#the-tool-configuration-file))

#### The basic configuration file

For each corrected variable a different configuration file is needed. Also, for each systematic variation of a correction, a separate configuration file is needed. This means that `1 + 2 * #systematic uncertainties` configuration files should be created for each variable. In the following, the composition of the configuration file is explained in detail, and further down, some example configuration files are linked.

The **name of the variable to be corrected** must be specified via the flag `Variable`, for example like this:

```bash
Variable: Rhad
```

This refers to the actual name of the AUX variable in the container file, relative to the object which should be corrected. I.e. if a photon should be corrected, `Rhad` might turn up as `PhotonsAuxDyn.Rhad` in a `TBrowser` or `root.ls`, however, from the photon point of view, `Rhad` is the correct reference.

Also, a **correction function** must be specified. Internally, the tool uses the `TF1` class to apply the correction, so the input can be any function as long as it is representable using the `TF1` class. It is also possible to include functions and definitions from the `TMath` class. The flag for the function is `Function`. For example, this could look like this:

```bash
Function: ([0] * x + [1] * [2]) * 0.7 * [3] - TMath::Pi() * [4] * [4] + + [5] + [1]/[6]
```

The **number of function parameters** must also be given in the configuration file using the flag `nFunctionParameters`. This is necessary so the tool can retrieve the information for all the parameters used in the correction function, and check if any information is missing. For the above function, this looks like this:

```bash
nFunctionParameters: 6
```

Finally, for each function parameter, the **type of the parameter** needs to be specified, as well as the **information connected to that parameter type**. The parameter type is set using the flag `Parameter*Type`, where `*` must be replaced with the parameter number (i.e. `Parameter0Type` for parameter 0 of the correction function, `Parameter1Type` for parameter 1 of the correction function, etc.). There are six different parameter types:

- `EtaDependentTGraph`,
- `PtDependentTGraph`,
- `EtaBinned`,
- `PtBinned`,
- `EtaTimesPtBinned`,
- `EtaTimesPhiTH2`,
- `EventDensity`.

According to the type of parameter, the necessary information which needs to be given to the tool differs:

The **TGraph types** need a filename and a graph name, which is given to the tool using the flags `Parameter*File` and `Parameter*GraphName`. Again, `*` is the number of the respective parameter. The file flag should be handed the path to the root file where the TGraph used for the evaluation of the correction parameter is saved, and the graph name flag should be handed the name of this TGraph inside the root file. An example looks like this:

```bash
Parameter0File: /path/to/file/where/you/saved/the/graph.root
Parameter0GraphName: NameOfTheTGraphInTheRootFile
```

If any of the **binned parameter types** is used, first the binning in eta and/or pt must be provided to the tool. This is done using the flags `EtaBins` and `PtBins`, respectively. The tool expects to be provided the lower bin edges of the binning. For the `PtBins`, it expects the binning to start at 0 and will fail if the lowest bin edge is not 0. The pt binning must be provided in MeV. For the `EtaBins`, the tool expects the binning to either start at 0 - for corrections using `abs(eta)` - or at a negative value, being treated by the tool as negative infinity.
In the latter case, this means that the first correction value of all eta binned variables is used to correct all the objects with an `eta` value between minues infinity and the second `eta` value provided in the binning. The tool will automatically choose the correct `eta` range to use for a given `eta` binning, depending on the range of the provided binning.
For example, this looks like this:

```bash
EtaBins: 0.0; 0.6; 1.37; 1.52; 1.81; 2.37
PtBins: 0.0; 5000.; 100000.; 200000.; 500000.; 1500000.
```

If your correction only affects objects in a certain phase space, please make sure that no objects outside of this phase space are passed to the tool in the first place.
This should be done via an object selection in your code and **cannot be done inside of the tool**! The region between 0 and when you start correcting the objects (or between when you stop correcting objects and infinity, respectively) must then be filled by dummy values inside the configuration file.

The correction values for each respective bin are given to the tool using the flag `Parameter*Values`, where `*` again is the number of the respective parameter. For example, this looks like this:

```bash
Parameter2Values: 1.; 0.9; 0.7; 0.45; 0.6; 1.1
```

If wanted, a partwise linear interpolation between the given pT bin values can be used. For this, simply set the flag `Parameter*Interpolate` to `TRUE` if `Parameter*` is a parameter binned in pT. If the flag is not set, it is assumed to be `FALSE`, however, the code does not complain if you explicitly set the flag to `FALSE`. The implemented method mostly follows the [interpolation method implemented in the ROOT TH1 class](https://root.cern.ch/doc/master/classTH1.html#a8ca269364ca55ea1e7fb65f9d3b21722). For interpolation into the last pt bin, whose upper boundary is infinity, we treat the bin as having the width "w" of the next-to-last bin. Thus, we interpolate a distance w/2 into the last bin, and after this distance the value is constant. For example, this would look like this:

```bash
Parameter2Interpolate: TRUE
```

The **TH2** type needs a filename and a histogram name, which are given to the tool using `Parameter*File` and `Parameter*TH2Name`, respectively. Again, `*` is the number of the respective parameter. The file flag should be handed the path to the root file where the TH2 used for the evaluation of the correction parameter is saved, and the TH2 name flag should be handed the name of this TH2 inside the root file. An example looks like this:

```bash
Parameter5File: /path/to/file/where/you/saved/the/th2.root
Parameter5TH2Name: NameOfTheTH2InTheRootFile
```

The TH2 can contain either corrections for absolute `eta` values or using the full `eta` range. In the former case, the binning of the TH2 should be set up such that it starts at 0, while in the latter case, the binning should start at a negative value being treated as negative infinity in the tool. This means that the first correction value provided by the TH2 is used to correct all the objects with an `eta` value between minues infinity and the upper bin edge of the first bin of the TH2. The tool will automatically choose the correct `eta` range to use for each TH2 individually, depending on the respective range of the provided TH2.

For the **event density**, no further information must be given to the tool. The tool will extract the event density from the event and use it as the respective parameter. It is only necessary to specify the parameter type in this case:

```bash
Parameter6Type: EventDensity
```

If you want to include parameters depending on other quantities, please read the [developer manual](#how-to-change-and-adapt-the-tool-developer-manual) or [contact the developers](mailto:nils.gillwald@desy.de).

In order to check whether the passed object in e.g. `applyCorrection` is intended to be corrected by the current tool instance, the flag `ApplyTo` must be set. There are five different options for this flag:

- `unconvertedPhotons`,
- `convertedPhotons`,
- `allPhotons`,
- `allElectrons`,
- `allEGammaObjects`.

The tool will then check if the passed object is compatible with the `ApplyTo` flag provided, and will fail with a `CP::CorrectionCode::Error` if the object type is not as expected.

If there are **discrete parameter values which should be left uncorrected**, you can use the flag `UncorrectedDiscontinuities`. You should pass all values which should not be corrected to it as a list, like this:

```bash
UncorrectedDiscontinuities: 0.0; 1.0
```

Note that this will work fine for integer-like floats! It will probably fail for longer floats because of the internal floating point precision of C++ (i.e. the code checks equality using `==`, and does not check if the difference between the variable value and the values which should be skipped is smaller than some epsilon). If you need this to work for all floats, contact the maintainers and we'll see what we can do.

An **example configuration file** containing examples for all possible flags can be found in `./data/ElectronPhotonVariableCorrectionBase_ExampleConf.conf`. The complete list of example configuration files is (all in `./data/`):

- `ElectronPhotonVariableCorrectionBase_ExampleConvertedPhotonConf.conf` for converted photons,
- `ElectronPhotonVariableCorrectionBase_ExampleUnconvertedPhotonConf.conf` for unconverted photons,
- `ElectronPhotonVariableCorrectionBase_ExampleElectronConf.conf` for electrons,
- `ElectronPhotonVariableCorrectionBase_ExampleIsoCorrectionConf.conf` for the isolation correction,
- `ElectronPhotonVariableCorrectionBase_ExampleConf.conf` general example showing what the tool can do and how all different possible parameters can be handled.

The .root file currently used for testing is `/pnfs/desy.de/atlas/dq2/atlaslocalgroupdisk/rucio/mc16_13TeV/3b/36/DAOD_HIGG1D2.20317301._000001.pool.root.1`.

#### The tool configuration file

In order to be able to correct multiple variables at once, the tool needs to be provided with a list of configuration files for the `ElectronPhotonVariableCorrectionBase` class. These configuration files each need to be constructed as described [here](#the-basic-configuration-file).

The tool distinguishes the following types of objects:

- Electrons,
- Converted Photons,
- Unconverted Photons.

Each configuration file path must be associated with one of these object types. Thus, there are three different possible flags to pass a configuration file path:

- `ElectronConfigs`,
- `ConvertedPhotonConfigs`,
- `UnconvertedPhotonConfigs`.

Each flag is associated with a respective tool holder in the tool. Please make sure that the `ApplyTo` flag in the respective configuration file is compatible with the associated tool holder, else, the initialization will return a `StatusCode::FAILURE`.

You can add multiple configuration files per object type using `+` in front of the flag, e.g. like this:

```C++
ElectronConfigs: /path/to/first.conf;
+ElectronConfigs: /path/to/second.conf;
+ElectronConfigs: /path/to/third.conf
```

Note that there should be a semicolon after every line but the last one. Also note that you can use one instance of the tool to correct electrons, converted photons and unconverted photons at the same time - just pass all the respective configuration files using the according flags to it. If you want to use the same configuration file for correcting a variable on different object types (i.e. (un)converted photons and electrons), you must provide it twice or three times to the tool - once using each flag.

An example configuration file can be found in `./data/EGammaVariableCorrectionTool_ExampleConf.conf`

#### Configuration file for Gaussian Smearing

Gaussian smearing corrects variables by adding a random shift to the variable, where the random shifts are selected
using a Gaussian probability density function. The parameters of the Gaussian pdf used for each variable are selected
using the separate configuration files for each variable. The configuration files follow the same structure as those described
in the above section, with some additional constraints. Firstly, the tool is told to apply the random smearing with the flag `doGaussianSmearing`,
for example:

```bash
doGaussianSmearing: TRUE
```

Note that it is not necessary to set the flag to `FALSE` if you do not wish to use the smearing correction - the
configuration file will default to doing the standard corrections if the `doGaussianSmearing` flag is not present in the
configuration file.

Additionally, the correction function must be specified as follows:

```bash
Function: TMath::Gaus(x, [0], [1])
```

where the [0] parameter is the mean and the [1] parameter is the width of the Gaussian. The values of these parameters can be set using the methods explained previously, in order to apply shifts using different Gaussian distributions in different regions of the phase space. However, the recommended method is to use the parameter type `EtaTimesPtBinned` in order to capture
the Pt and Eta dependence of the smearing corrections.

## How to change and adapt the tool (developer manual)

This section is meant for **developers**. If you want to use the tool for variable correction, you should read [this section](#how-to-use-the-tool-single-and-multiple-variable-correction-user-manual) instead.

### Checkout

First, you need to set up your area:

```bash
mkdir testarea
cd testarea
mkdir build run source
```

Then, you need to checkout `Athena`. This can be done via a full or a sparse checkout. For the purpose of adapting this tool, I recommend a sparse checkout, but this is entirely up to you. Detailed instructions can be found [here](https://atlassoftwaredocs.web.cern.ch/gittutorial/git-clone/).

Assuming you have completed a sparse checkout of `Athena` as explained in the linked tutorial, you need to then add the `EGammaVariableCorrection` to your sparse checkout. For this, do

```bash
cd source/athena
git atlas addpkg EGammaVariableCorrection
```

You can use `addpkg` to add as many packages as you like, `rmpkg` to remove them and `listpkg` to check what you have checked out. If you want to have a look at the tool handler, you need to checkout the `EgammaAnalysisInterfaces` package.

### Setup and Compile

**WARNING:** The following instructions assume a sparse checkout of Athena. If you made a full checkout, please refer [here](https://atlassoftwaredocs.web.cern.ch/gittutorial/branch-and-change/#setting-up-to-compile-and-test-code-for-the-tutorial) on how to properly compile without fully building Athena.

The tool is developed on the athena master. To set up the according environment, do

```bash
cd build
asetup AnalysisBase master latest
```

If you want to target a specific nightly (often worth it for development) then replace `latest` with `rXX` where `XX` is the day of the month.

To compile, do in the `source` directory

```bash
cd $TestArea/../build
cmake ../source
cmake --build .
```

To be able to run executables, run (only once per login session, or after adding new executables to the CMakeLists.txt file)

```bash
source $TestArea/../build/$CMTCONFIG/setup.sh
```

This will search for the executables produced during compilation and make them accessible from the whole TestArea environment. You should resource this script after you added a new executable and recompiled the code.

The corresponding code the compiler sees is in `/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.97/InstallArea/x86_64-centos7-gcc8-opt/src/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool`, as seen by the first path `echo`ed from `$CMAKE_PREFIX_PATH`. (This is of course only valid information for release 21.2, but it might be a good orientation if you work on another version. Note also that in 21.2, the tool was located in a different package!)

### Usage of the eyVariableCorrectionBase class

The base class is designed to be used embedded in a code which provides it with the objects which should be corrected. It is not able to open root files or retrieve trees or apply object selection itself, but only focuses on the correction of objects passed to it. The base class is designed to be as steerable as possible from its configuration file, so that it can be used for correcting a broad range of variables in a relatively simple way.

#### Instantiate the base class

The base class must be integrated in code which provides it with the objects which should be corrected. To include the base class, you need to `#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase.h"`. All the other includes needed depend on the wrapping code which provides the objects to the base class.

In order to declare the base class, it needs to be named in the constructor, so it can be distinguished. This could look like this:

```C++
ElectronPhotonVariableCorrectionBase MyTool("MyTool");
```

#### Initialization of the base class

To initialize the base class, first the configuration file must be provided to the base class using `MyTool.setProperty("ConfigFile",/path/)`. Then, the initialize function must be called on the base class, so it can read out the configuration file and set itself up. This could look like this:

```C++
std::string configFilePath = "MyData/MyConfFile.conf";
ANA_CHECK(MyTool.setProperty("ConfigFile",configFilePath));
ANA_CHECK(MyTool.initialize());
```

#### Usage of the base class for the correction of EGamma objects

To correct an object, the object must be passed to the base class. **The base class will overwrite the original properties of the object** -- it will however keep the original value, stored as `variableName_original`. This means that the base class cannot be run on const containers -- a shallow or deep copy must be used. Please refer to the official documentation / tutorials on how to shallow / deep copy a container, if needed. Note that you do not have to pass all photons in an event to the base class, just the ones you want to be corrected.

Assuming a writeable object is used, the code for correcting it using the base class looks for example like this:

```C++
ANA_CHECK(MyTool.applyCorrection(*photon));
```

If electrons should be corrected, of course an electron should be passed to the base class instead.

As mentioned before, the base class overwrites the original variable value but also stores the original values as `variableName_original`.

The variables can be accessed in the following way:

```C++
std::string correctionVariable = MyTool.GetCorrectionVariable();
// construct AUX element accessors
SG::AuxElement::Accessor<float> VariableToCorrect(correctionVariable + "_original");
SG::AuxElement::Accessor<float> CorrectedVariable(correctionVariable);
// get uncorrected and corrected variable value for this photon
float variable_original = VariableToCorrect(*photon);
float variable_corrected = CorrectedVariable(*photon);
```

There is no finalize function which needs to be run. Example code using the base class for correcting photons and electrons can be found in `./util`:

- `testElectronPhotonVariableCorrectionBase.cxx`
- `testIsoCorrection.cxx`

### Add a new parameter type for the correction function

If possible, use the provided parameter types which are already implemented and tested. However, if you need to add a new parameter type which needs to have access to some variable which is not currently retreived, here is how to do it.

1. Add the new parameter type to the `enum ParameterType` in the class header. Please make sure to generalize the different parameter types as much as possible while keeping the correction function concise but readable.
2. Add a (private!) getter function for your parameter type. This getter function should be doing all the "dirty" pull work and only return the final correction parameter (possibly as a function of eta and/or pT). If you want an example, have a look at the already implemented functions. Make sure your getter function follows the form `const StatusCode GetNewParameter(float& return_parameter, const& list of needed inputs)`.
3. Add the code to be executed if the correction parameter is of your new parameter type to the following functions:

    - `ElectronPhotonVariableCorrectionBase::GetParameterInformationFromConf`
    - `ElectronPhotonVariableCorrectionBase::setCorrectionParameters`
    - `ElectronPhotonVariableCorrectionBase::StringToParameterType`

If there is a style which all the other types use to implement their functionalities, follow it. This holds especially for `ElectronPhotonVariableCorrectionBase::GetCorrectionParameters`.
4. Test the functionality of your new parameter type.
5. Add an example of how to use your new parameter type to the example conf file in `./util/`.
6. Submit a merge request to officially add your new parameter type to the base class.
