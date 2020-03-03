# Readme

This is a tool for the application of MC corrections to photon and electron auxiliary variables in the derivation of DxAODs:
`ElectronPhotonVariableCorrectionTool`, together with a tool wrapper `ElectronPhotonVariableCorrectionToolWrapper`
allowing to correct multiple variables of one photon or electron simultaneously.

It lives in `/head/athena/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool/`.

This README first explains
[how to use the `ElectronPhotonVariableCorrectionTool`](#single-variable-correction---electronphotonvariablecorrectiontool)
before explaining the
[functionality of the `ElectronPhotonVariableCorrectionToolWrapper`](#multi-variable-correction---electronphotonvariablecorrectiontoolwrapper).
Note that if you want to correct multiple variables of a photon or electron, you should only create an instance of
`ElectronPhotonVariableCorrectionToolWrapper` - this instance will then handle the `ElectronPhotonVariableCorrectionTool`
for you. You will however need to provide configuration files for the `ElectronPhotonVariableCorrectionTool`, which is
explained in [this section](#constructing-a-configuration-file).

If you have any questions or requests, please contact [Nils Gillwald](mailto:nils.gillwald@desy.de).

## Single variable correction - ElectronPhotonVariableCorrectionTool

### How to use the tool (blackbox use)

The tool is designed to be used embedded in a code which provides it with the objects which should be corrected. It is
not able to open root files or retrieve trees or apply object selection itself, but only focuses on the correction of
objects passed to it. The tool is designed to be as steerable as possible from the configuration file, so that it can
be used for correcting a broad range of variables in a relatively simple way.

#### Instantiate the tool using AnaToolHandle

The tool must be integrated in code which provides it with the objects which should be corrected. To include the tool,
you need to include its interface via `#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"`. Also,
you need to includ the according tool handler, for example `#include "AsgTools/AnaToolHandle.h"`. All the other includes
needed depend on the wrapping code which provides the objects to the tool.

To declare the tool via the tool handler, you need to do something like this:

```C++
asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> MyTool("ElectronPhotonVariableCorrectionTool/myToolName");
```

The syntax of this command depends on your tool handler. Note that the part of the string provided to the constructor
before the forward slash defines which tool from the interface is constructed by the tool handler, and the part after
the slash defines the name of the tool!

#### Instantiate the tool without using a Tool Handler

As before, the tool must be integrated in code which provides it with the objects which should be corrected. To include
the tool, you need to `#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionTool.h"`. All the
other includes needed depend on the wrapping code which provides the objects to the tool.

In order to declare the tool, it needs to be named in the constructor, so it can be distinguished. This could look like
this:

```C++
ElectronPhotonVariableCorrectionTool MyTool("MyTool");
```

#### Initialization of the tool

To initialize the tool, first the configuration file must be provided to the tool using
`MyTool.setProperty("ConfigFile",/path/)`.
Then, the initialize function must be called on the tool, so it can read out the configuration file and set itself up.
This could look like this:

```C++
std::string configFilePath = "ElectronPhotonShowerShapeFudgeTool/MyConfFile.conf";
ANA_CHECK(MyTool.setProperty("ConfigFile",configFilePath));
ANA_CHECK(MyTool.initialize());
```

#### Usage of the tool for the correction of EGamma objects

To correct an object, the object must be passed to the tool.
**The tool will overwrite the original properties of the object** -- it will however keep the original value, stored as
`variableName_original`. This means that the tool cannot be run on const containers -- a shallow or deep copy must be
used. Please refer to the official documentation / tutorials on how to shallow / deep copy a container. Note that you
do not have to pass all photons in an event to the tool, just the ones you want to be corrected.

Assuming a writeable object is used, the code for correcting it using the tool looks for example like this:

```C++
ANA_CHECK(MyTool.applyCorrection(*photon));
```

If electrons should be corrected, of course an electron should be passed to the tool instead.

As mentioned before, the tool overwrites the original variable value but also stores the original values as
`variableName_original`.
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

There is no finalize function which needs to be run.
Example code using the tool for correcting photons and electrons can be found in `./util`:

- `testElectronPhotonVariableCorrectionTool.cxx`
- `testIsoCorrection.cxx`

#### Constructing a configuration file

For each corrected variable, a different configuration file is needed. Also, for each systematic variation of a
correction, a separate configuration file is needed. This means that `1 + 2 * #systematic uncertainties` configuration
files should be created for each variable. In the following, the composition of the configuration file is explained in
detail, and further down, some example configuration files are linked.

The **name of the variable to be corrected** must be specified via the flag `Variable`, for example like this:

```bash
Variable: Rhad
```

This refers to the actual name of the AUX variable in the container file, relative to the object which should be corrected.
I.e. if a photon should be corrected, `Rhad` might turn up as `PhotonsAuxDyn.Rhad` in a `TBrowser` or `root.ls`,
however, from the photon point of view, `Rhad` is the correct reference.

Also, a **correction function** must be specified. Internally, the tool uses the `TF1` class to apply the correction,
so the input can be any function as long as it is representable using the `TF1` class. It is also possible to include
functions and definitions from the `TMath` class. The flag for the function is `Function`. For example, this could
look like this:

```bash
Function: ([0] * x + [1] * [2]) * 0.7 * [3] - TMath::Pi() * [4] * [4] + [1]/[5]
```

The **number of function parameters** must also be given in the configuration file using the flag
`nFunctionParameters`. This is necessary so the tool can retreive the information for all the parameters used in the
correction function, and check if any information is missing. For the above function, this looks like this:

```bash
nFunctionParameters: 6
```

Finally, for each function parameter, the **type of the parameter** needs to be specified, as well as the
**information connected to that parameter type**. The parameter type is set using the flag `Parameter*Type`, where
`*` must be replaced with the parameter number (i.e. `Parameter0Type` for parameter 0 of the correction function,
`Parameter1Type` for parameter 1 of the correction function, etc.). There are six different parameter types:

- `EtaDependentTGraph`,
- `PtDependentTGraph`,
- `EtaBinned`,
- `PtBinned`,
- `EtaTimesPtBinned`,
- `EventDensity`.

According to the type of parameter, the necessary information which needs to be given to the tool differs.

The **TGraph types** need a filename and a graph name, which is given to the tool using the flags `Parameter*File` and
`Parameter*GraphName`. Again, `*` is the number of the respective parameter. The file flag should be handed the path
to the root file where the TGraph used for the evaluation of the correction parameter is saved, and the graph name
flag should be handed the name of this TGraph inside the root file. An example looks like this:

```bash
Parameter0File: /path/to/file/where/you/saved/the/graph.root
Parameter0GraphName: NameOfTheTGraphInTheRootFile
```

If any of the **binned parameter types** is used, first the binning in eta and/or pt must be provided to the tool.
This is done using the flags `EtaBins` and `PtBins`, respectively. The tool expects to be provided the lower bin edges
of the binning. It also expects the binning to start at 0 and will fail if the lowest bin edge is not 0. The pt
binning must be provided in MeV. For example, this looks like this:

```bash
EtaBins: 0.0; 0.6; 1.37; 1.52; 1.81; 2.37
PtBins: 0.0; 5000.; 100000.; 200000.; 500000.; 1500000.
```

If your correction only affects objects in a certain phase space, please make sure that none of those objects are
passed to the tool in the first place via the object selection in your correction code outside of the tool (see the
according section above). This cannot be done inside of the tool! The region between 0 and when you start correcting
the objects must then be filled by dummy values inside the configuration file.

The bin values are given to the tool using the flag `Parameter*Values`, where `*` again is the number of the
respective parameter. For example, this looks like this:

```bash
Parameter2Values: 1.; 0.9; 0.7; 0.45; 0.6; 1.1
```

For the **event density**, no further information must be given to the tool. The tool will extract the event density
from the event and use it as the respective parameter.

In order to check whether the passed object in e.g. `applyCorrection` is intended to be corrected by the current tool
instance, the flag `ApplyTo` must be set. There are five different options for this flag:

- unconvertedPhotons,
- convertedPhotons,
- allPhotons,
- allElectrons,
- allEGammaObjects.

The tool will then check if the passed object is compatible with the `ApplyTo` flag provided, and will fail with a
`StatusCode::FAILURE` if the object type is not as expected.

An **example configuration file** containing examples for all possible flags can be found in
`./data/ElectronPhotonVariableCorrectionTool_ExampleConf.conf`. The complete list of example configuration files is
(all in `./data/`):

- `ElectronPhotonVariableCorrectionTool_ExampleConvertedPhotonConf.conf` for converted photons,
- `ElectronPhotonVariableCorrectionTool_ExampleUnconvertedPhotonConf.conf` for unconverted photons,
- `ElectronPhotonVariableCorrectionTool_ExampleElectronConf.conf` for electrons,
- `ElectronPhotonVariableCorrectionTool_ExampleIsoCorrectionConf.conf` for the isolation correction,
- `ElectronPhotonVariableCorrectionTool_ExampleConf.conf` general example showing what the tool can do and how all different possible parameters can be handled.

The .root file currently used for testing is
`/pnfs/desy.de/atlas/dq2/atlaslocalgroupdisk/rucio/mc16_13TeV/da/80/DAOD_HIGG1D2.18400890._000001.pool.root.1`.

[(back to the top)](#readme)

### How to change and adapt the tool (non-blackbox use)

#### Checkout

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

#### Setup and Compile

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

This will search for the executables produced during compilation and make them accessible from the whole TestArea environment. You should resource this script after you added a new executable and recompiled the code.

The corresponding code the compiler sees is in `/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.97/InstallArea/x86_64-centos7-gcc8-opt/src/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool`, as seen by the first path `echo`ed from `$CMAKE_PREFIX_PATH`

#### Add a new parameter type for the correction function

If possible, use the provided parameter types which are already implemented and tested. However, if you need to add a new parameter type which needs to have access to some variable which is not currently retreived, here is how to do it.

1. Add the new parameter type to the `enum ParameterType` in the class header. Please make sure to generalize the different parameter types as much as possible while keeping the correction function concise but readable.
2. Add a (private!) getter function for your parameter type. This getter function should be doing all the "dirty" pull work and only return the final correction parameter (possibly as a function of eta and/or pT). If you want an example, have a look at the already implemented functions. Make sure your getter function follows the form `const StatusCode GetNewParameter(float& return_parameter, const& list of needed inputs)`.
3. Add the code to be executed if the correction parameter is of your new parameter type to the following functions:
    - `ElectronPhotonVariableCorrectionTool::GetParameterInformationFromConf`
    - `ElectronPhotonVariableCorrectionTool::GetCorrectionParameters`
    - `ElectronPhotonVariableCorrectionTool::StringToParameterType`
If there is a style which all the other types use to implement their functionalities, follow it. This holds especially for `ElectronPhotonVariableCorrectionTool::GetCorrectionParameters`.
4. Test the functionality of your new parameter type.
5. Add an example of how to use your new parameter type to the example conf file in `./util/`.
6. Submit a merge request to officially add your new parameter type to the tool.

## Multi variable correction - ElectronPhotonVariableCorrectionToolWrapper

### How to use the tool

As for the `ElectronPhotonVariableCorrectionTool`, the `ElectronPhotonVariableCorrectionToolWrapper` (from now on referred
to as the `wrapper`), the `wrapper` tool is designed to be used embedded in a code which provides it with the objects
which should be corrected. It is not able to open root files or retrieve trees or apply object selection itself, but only
focuses on the correction of objects passed to it.

#### Instantiate the tool using AnaToolHandle

The tool must be integrated in code which provides it with the objects which should be corrected. To include the tool,
you need to include its interface via `#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"`. Also,
you need to includ the according tool handler, for example `#include "AsgTools/AnaToolHandle.h"`. All the other includes
needed depend on the wrapping code which provides the objects to the tool.

To declare the tool via the tool handler, you need to do something like this:

```C++
asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> MyTool("ElectronPhotonVariableCorrectionToolWrapper/myToolName");
```

The syntax of this command depends on your tool handler. Note that the part of the string provided to the constructor
before the forward slash defines which tool from the interface is constructed by the tool handler, and the part after
the slash defines the name of the tool!

#### Instantiate the tool without using a Tool Handler

As before, the tool must be integrated in code which provides it with the objects which should be corrected. To include
the tool, you need to `#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionToolWrapper.h"`. All the
other includes needed depend on the wrapping code which provides the objects to the tool.

In order to declare the tool, it needs to be named in the constructor, so it can be distinguished. This could look like
this:

```C++
ElectronPhotonVariableCorrectionToolWrapper MyTool("MyTool");
```

#### Initialization of the tool

To initialize the tool, first the configuration file must be provided to the tool using
`MyTool.setProperty("ConfigFile",/path/)`.
Then, the initialize function must be called on the tool, so it can read out the configuration file and set itself up.
This could look like this:

```C++
std::string configFilePath = "ElectronPhotonShowerShapeFudgeTool/MyConfFile.conf";
ANA_CHECK(MyTool.setProperty("ConfigFile",configFilePath));
ANA_CHECK(MyTool.initialize());
```

#### Usage of the tool for the correction of EGamma objects

To correct an object, the object must be passed to the tool.
**The tool will overwrite the original properties of the object** -- it will however keep the original value, stored as
`variableName_original`. This means that the tool cannot be run on const containers -- a shallow or deep copy must be
used. Please refer to the official documentation / tutorials on how to shallow / deep copy a container. Note that you
do not have to pass all photons in an event to the tool, just the ones you want to be corrected.

Assuming a writeable object is used, the code for correcting it using the tool looks for example like this:

```C++
ANA_CHECK(MyTool.applyCorrection(*photon));
```

If electrons should be corrected, of course an electron should be passed to the tool instead.

As mentioned before, the tool overwrites the original variable value but also stores the original values as
`variableName_original`.
The variables can be accessed in the following way:

```C++
std::string correctionVariable = "nameOfTheVariableYouCorrected";
// construct AUX element accessors
SG::AuxElement::Accessor<float> VariableToCorrect(correctionVariable + "_original");
SG::AuxElement::Accessor<float> CorrectedVariable(correctionVariable);
// get uncorrected and corrected variable value for this photon
float variable_original = VariableToCorrect(*photon);
float variable_corrected = CorrectedVariable(*photon);
```

There is no finalize function which needs to be run.
Example code using the tool for correcting photons and electrons can be found in `./util`:

- `testElectronPhotonVariableCorrectionToolWrapper.cxx`

#### Constructing a configuration file

In order to be able to correct multiple variables at once, the `wrapper` needs to be provided with a list of configuration
files for the `ElectronPhotonVariableCorrectionTool`. These configuration files each need to be constructed as described
[here](#constructing-a-configuration-file).

The `wrapper` distinguishes the following types of objects:

- Electrons,
- Converted Photons,
- Unconverted Photons.

Each configuration file path must be associated with one of these object types. Thus, there are three different possible
flags to pass a configuration file path:

- ElectronConfigs,
- ConvertedPhotonConfigs,
- UnconvertedPhotonConfigs.

Each flag is associated with a respective tool holder in the `wrapper`. Please make sure that the `ApplyTo` flag in the
respective configuration file is compatible with the associated tool holder, else, the initialization will return a
`StatusCode::FAILURE`.

You can add multiple configuration files per object type using `+` in front of the flag, e.g. like this:

```C++
ElectronConfigs: /path/to/first.conf;
+ElectronConfigs: /path/to/second.conf;
+ElectronConfigs: /path/to/third.conf
```

Note that there should be a semicolon after every line but the last one.
If you want to use the same configuration file for correcting a variable on different object types (i.e. (un)converted photons
and electrons), you must provide it twice or three times to the tool - once using each flag.

[(back to the top)](#readme)
