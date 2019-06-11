# List of tool properties

This section contains a list of the properties that can be used to control the behaviour of the tools before initializing them, using the syntax below: 

```c++
tool->setProperty("EnergyUnit", "GeV");
tool->initialize();
```

Most of them are common to all tools, while a few are more specific. 

## InputFiles [vector<string>] (all tools)

List of ROOT and/or XML files containing the efficiencies and their uncertainties. The contents of these files must follow the formats described respectively in [this section](import_root.md) and [that one](import_xml.md). 

## Selection [string] (all tools)

This allows the user to specify the selection argument that should be used to compute the total yield and fill histograms. The syntax for this property is described [there](arg_selection.md). The default value is the same as the one returned by `IFakeBkgTool::defaultSelection()`. 

## Process [string] (all tools)

This allows to specify the process argument that should be used to compute the total yield and fill histograms. The syntax for this property is described [there](arg_process.md). The default value is the same as the one returned by `IFakeBkgTool::defaultProcess()`. 

## EnergyUnit [string, default="MeV"] (all tools)

This property can be set either to "MeV" (default) or "GeV". It defines how to interpret the pT bin boundaries specified in the files storing the efficiencies and fake factors. For other potential variables used in the parametrization that have the dimension of energy and are supplied through decorations of the `IParticle` or the `EventInfo`, this property is irrelevant, therefore one should make sure to use the same units for the files and the decorations. 

When using the tool outside of (Ath)AnalysisBase, this property is ignored and consistency is expected as well between the files and `IParticle.pt()`. 

## ConvertWhenMissing [bool, default=false] (all tools)

Fake efficiencies and fake factors slightly differ in definition, but can be converted one into the other by a straightforward mathematical transformation. This property, when set to true, instructs the tools to do the conversion when the needed type of efficiencies are not found (for example, when running the `ApplyFakeFactor` tool with input files prepared for the matrix method). 

## TightDecoration [string, default="Tight,as_char"] (all tools)

Name (and type) of the decoration used to indicate whether leptons pass the 'tight' requirements or not. This should follow the proper syntax for [SelectionHelpers/ISelectionAccessor](PhysicsAnalysis/Algorithms/SelectionHelpers/SelectionHelpers/ISelectionAccessor.h). 

## ProgressFileName [string, default="none"] (all tools)

Name of the ROOT file from which should be loaded the merged outputs of parallel jobs. See usage [here](merging_jobs.md). See also the ProgressFileDirectory property to be used in complement when the intermediate results are not saved at the root directory of the file.

## ProgressFileDirectory [string, default=""] (all tools)

Complementary to ProgressFileName, allows to specify a path to a TDirectory inside the ROOT file from which should be loaded the merged outputs of parallel jobs. See usage [here](merging_jobs.md). 

## FixHistogramNormalization [bool] (only `LhoodMM_tools`)

If set to true, this property will trigger the scaling of histograms with the estimated yields, such that their integral is then equal to the yield computed for the sum of bins. Without the scaling, the two values might not agree, due to the non-linear nature of the method. 

## DoFakeFactorFit [bool, default=false] (only `LhoodMM_tools`)

If set to true, this property will result in replacing the matrix method-based likelihood by one corresponding to the fake factor method. 