# Introduction
This package has the purpose of producing a (by now, fairly extensive) set of standard tracking performance validation histograms. It is used in Physics Validation, for automatised monitoring, but also for user-level performance checks during tracking development work. 

The principal tool, InDetPhysValMonitoringTool, has a simple structure:
* retrieve containers
* loop over containers using a selection tool to select elements
* decorate those elements
* do plots

The plots are intended to be as simple structures as possible, 
only retrieving and plotting the added decoration variables. 
There are numerous decorators for either truth or track particles,
inheriting from a common interface. 


# Running the code 

The package is included in the `Athena` releases, and can be run "out of the box" without needing to be checked out or built by hand. 
Building by hand is needed only when modifying the code (for example, for adding new plots or flags). We will explain below how to do this. 

There are generally two common ways to run the code. 

1. The first consists of running within the `Reco_tf.py` reconstruction job transform. This is the default when running monitoring or physics validation, and also very useful for validating performance changes while working on changes to the reconstruction itself. It also offers the maximum flexibility. 
2. The second way is intended for running the performance validation on already existing ESD or AOD (or DAOD_IDTRKVALID) files, without a reconstruction step. Here, we run with a set of standalone job options, which require far less configuration than the first approach at the price of reduced flexibility. This is recommended if you are testing the code for the first time, or if you have a set of pre-produced files you would like to study. 

## Setting up the release 

InDetPhysValMonitoring requires the full athena release. 
In `master`, you can set this up using for example 
``` 
asetup Athena,master,latest 
```  
to get the most up-to date nightly or 
```
asetup Athena,22.0.20
``` 
to get a recent stable release. 

This is always needed, regardless of the running mode. 

InDetPhysValMonitoring is fairly independent of the precise release you set up, except for any changes and fixes to the package itself that may have gone in in the course of time. 

It also is fairly independent of the release used to write the input file - so you can use InDetPhysValMonitoring from a recent release also to analyse AOD/ESD made using older releases, even if they are from release 21! 
This is useful for example when you wish to compare the performance between different releases using the same histogramming code. 

## Running within a transform 

The `Reco_tf.py` transform already comes with the ability to run InDetPhysValMonitoring, either after a reconstruction job or as the only task. 

To enable the InDetPhysValMonitoring histograms, add `--valid True  --validationFlags doInDet --outputNTUP_PHYSVALFile physval.root` to the transform you would run without the additional validation histograms. 
- the switch `--valid True` will enable the decoration
- the switch `--validationFlags doInDet` will enable the InDetPhysValMonitoring histograms
- the switch `--outputNTUP_PHYSVALFile physval.root` tells `Reco_tf` to run the physics validation monitoring step and create the histogram files.

For example: 
```   
Reco_tf.py --AMI=q431 \
--valid 'True' \
--validationFlags 'doInDet' \
--outputNTUP_PHYSVALFile "PhysVal.root"
``` 
This will run the standard `q431` data reconstruction test, producing an output ESD/AOD, 
and in the same transform add a 'PhysicsValidation' step, where we decorate the xAODTrackParticles, 
fill the histograms and write them into a `PhysVal.root`. 
Since we run on data, you will not get the full set of histograms (as of the date of writing, the truth record in collision data has yet to be found). 

## Running with standalone job options 

To run with standalone job options, it is sufficient to run in the command line: 
```
athena.py --filesInput <your input file, ESD/AOD/DAOD_IDTRKVALID> InDetPhysValMonitoring/InDetPhysValMonitoring_topOptions.py
``` 

This will schedule the same decoration and histogramming algorithms as done by `Reco_tf.py` above within a minimal environment. 
You can configure the output file name as well as a limited number of the configuration parameters available as described below. 

# Configuring the code 

The InDetPhysValMonitoring framework is designed to be rather flexibly configurable, in order to allow the user to steer which histograms they need. 
To avoid having to manually change the configuration, a set of `InDetPhysValFlags` is defined in [InDetPhysValJobProperties.py](python/InDetPhysValJobProperties.py)

A short overview of some of the most interesting flags:
*  `doValidateTightPrimaryTracks` (defaults to `False`) enables a second instance of the monitoring plots, made considering only reco tracks satisfying the `TightPrimary` working point. This can be useful to check how a tracking changes affects a high-quality subset of the tracks.  
*  `doValidateTracksInJets` and `doValidateTracksInBJets` (both default to `False`) enable additional plots for tracks within jets. These are very useful for CTIDE (tracking in dense environments) and flavour-tagging performance studies.
*  `doExpertOutput` (defaults to `False`) adds an extensive set of further histograms to all categories, which are mainly intended for tracking experts rather than day-to-day validation. Use if you need more detail than the standard plots give you. 
*  `setTruthStrategy` allows you to choose which truth particles to use when calculating Efficiencies and resolutions. The default, `HardScatter`, will only select hard-scatter particles. So the efficiency measured will be the one for particles produced in the hard scatter, and the same holds for the resolutions. The alternate option is `All`, which selects all truth particles in the event. If you run with full pileup truth, this can allow you to include the particles from pileup interactions in your study. 

For more flags, please see the file itself. 

## Configuring when running with Reco_tf

If you run with `Reco_tf`, you can set these flags in the `preExec` of your transform. 
Example: 
``` 
Reco_tf.py --AMI=q431 \
--valid 'True' \
--validationFlags 'doInDet' \
--outputNTUP_PHYSVALFile "PhysVal.root" \
--preExec 'all:from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); InDetPhysValFlags.doExpertOutput.set_Value_and_Lock(True);'
``` 
This will run the same job as in our previous example, but add some additional plots as well as a separate set of plots for tracks pre-selected with the `TightPrimary` working point. 

## Configuring when using standalone job options
The flags are not directly available in the standalone job options. 
There are two ways of steering them in this use case: 

1. You can use a set of command line arguments supported by the [standalone job options](share/InDetPhysValMonitoring_topOptions.py) to have it set the flags for you. The following are supported: 
    * `--doTightPrimary` will set  `InDetPhysValFlags.doValidateTightPrimaryTracks` to `True`
    * `--doTracksInJets` and `--doTracksInBJets` will set `InDetPhysValFlags.doValidateTracksInJets` and `InDetPhysValFlags.doValidateTracksInBJets` to `True`, respectively 
    * `--doExpertPlots` will set `InDetPhysValFlags.doExpertOutput` to `True`
    * `--HSFlag <value>` will set `InDetPhysValFlags.setTruthStrategy` to the passed value 
    * `--outputFile` allows you to set the name of the output file to write into. 
    To use the arguments, please pass them after the end of the standard invocation shown above, with one single `-` delimiter before the arguments. 
    For example, 
    ```
    athena.py --filesInput <your input file, ESD/AOD/DAOD_IDTRKVALID> InDetPhysValMonitoring/InDetPhysValMonitoring_topOptions.py - --doTightPrimary --doExpertPlots --outputFile MyPhysVal.customName.root 
    ```
    will enable the same extra histograms as in the transform example above, and change the output file name to `MyPhysVal.customName.root`. 

2. You can also set missing flags by passing a pre-command to athena using the `-c` argument, using the same syntax that you would use in a `Reco_tf` `preExec` line: 
```
athena.py  -c "from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);InDetPhysValFlags.doExpertOutput.set_Value_and_Lock(True); " --filesInput <your input file, ESD/AOD/DAOD_IDTRKVALID> InDetPhysValMonitoring/InDetPhysValMonitoring_topOptions.py
```
will run the same configuration as the command line arguments above or the transform example. Except for providing an example, there is no real point in using this to set flags that can also be steered more easily using command line arguments, but it can be used to set flags not exposed via this mechanism. 

# Modifying the code 

You may wish to add additional functionality to the package or change histogram binnings. 
Such changes require a `local working copy` of the package to implement the changes.
## Obtaining a working copy

Please follow the [ATLAS GitLab workflow tutorial](https://atlassoftwaredocs.web.cern.ch/gittutorial/git-clone/) for detailed instructions on how to get such a copy - both sparse checkouts and full checkouts with package filters are supported. 

So, for example: 
1. Create an (empty) sparse checkout: 
```  
setupATLAS 
lsetup git
mkdir athena build run
git-atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
```
2. Add the IDPVM package to your checkout: 
```
cd athena
git-atlas addpkg InDetPhysValMonitoring
```
3. Create a development branch to work with: 
``` 
git fetch upstream
git checkout -b MyDevelopmentBranch upstream/master --no-track
``` 
Now you can start making changes! 

## Developing code 

A quick pointer around the package: 
* The [InDetPhysValMonitoringTool](src/InDetPhysValMonitoringTool.cxx) is the top level tool containing the main event loop and truth matching logic. It internally uses an [InDetRttPlots](src/InDetRttPlots.h) instance to handle the plot filling. 
* The [InDetRttPlots](src/InDetRttPlots.cxx) implementation is the point where plot objects are attached and configured. Here you can add further child objects to fill the plots you need, or modify existing ones. 
* The individual plot objects are a good place to add more histograms of the same type - for example, an additional efficiency plot versus a new variable to the existing ones. See for example the [InDetPerfPlot_Efficiency](src/InDetPerfPlot_Efficiency.cxx) implementation for an example of how they work. 
* The binnings are defined in a xml file. For the current ID, take a look at [InDetPVMPlotDefRun2.xml](share/InDetPVMPlotDefRun2.xml)
* Most of the configuration happens in the `python` folder, for example [InDetPhysValMonitoringTool.py](python/InDetPhysValMonitoringTool.py). The flags are defined in [InDetPhysValDecoration.py](python/InDetPhysValDecoration.py), here you can also add new ones. The standalone job options are in [InDetPhysValMonitoring_topOptions.py](share/InDetPhysValMonitoring_topOptions.py)
* Please get in touch with Shaun and Götz, the maintainers, for more details

## Testing your changes 
Now, you need to compile your changes.

This is done using the standard ATLAS build procedure with cmake, see for example [the gitlab tutorial](https://atlassoftwaredocs.web.cern.ch/gittutorial/branch-and-change/) for details. 

For a full checkout, this could look like: 
```
cd build
cmake -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir  # assumes you prevously set up the package_filters.txt! 
make
source x86*/setup.sh   # need to do at least once per session... 
cd - 
```

In addition to just test-running the code, we recommend to also run the unit tests provided with the package when you have made changes. You can do this by typing `ctest` in the build folder after having compiled. 

## Making a merge request
If you have made a modification that may be useful to more users, please consider [making a merge request](https://atlassoftwaredocs.web.cern.ch/gittutorial/merge-request/) into master to share your work with others!