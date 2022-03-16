# PMG SystematicsTool(s)

Need help to evaluate theory uncertainties?
You are in the right place. I hope this tool helps you save a lot of time. 
Before proceeding, please read the caveats below.


Table of Contents:
- [Introduction](#introduction)
- [Installing the systematics tool](#installing-the-systematics-tool)
	- [What the tool can and cannot do](#what-the-tool-can-and-cannot-do)
- [Instructions for submissionTool](#instructions-for-submissiontool)
- [Instructions for systematicsTool](#instructions-for-systematicstool)
- [Instructions for covarianceTool](#instructions-for-covariancetool)
<br/>

# Introduction

The three main tools in this package are:
- `submissionTool.py`, which helps to submit `EVNT` samples to the Grid running on `Rivet` analysis, where once instance of `Rivet_i` is run for each matrix-element weight. [Documentation](documentation/UsingTheSubmissionTool.md) 
- `systematicsTool.py`, which helps to unpack GRID outputs, merge `yoda` or `root` files across jobs, automatically merge across subsamples/slices with corrected cross-sections for each OTF weight (On-the-fly weight , ie systematics weight), merge OTF weights into systematic variations, and calculate an total uncertainty, and finally to make plots with theory error bands. Can be used as an executable or the individual functions can be imported into a python script like `import systematicsTool as st`. [Documentation](documentation/UsingTheSystematicsTool.md)
- `covarianceTool.py` and `covarianceToolsLibrary.py`, and executable and a library of functions which allow analysts to manipulate and use covariance information. [Documentation](documentation/UsingTheCovarianceTool.md)
- a tool to read the systematics database [Documentation](documentation/QueryingTheSystematicsDatabase.md)

A database of OTF weights and recipes provides the user with the information on how to create the systematic uncertainties from on-the-fly variations stored in a given MC sample (by DSID), and allows the tool to combine OTF-weighted histograms into per-variation histograms automatically.

This document contains examples for the most common uses of the systematics tool.
Please follow the link for the task you are interested in!

**NB: The `systematicsTool.py` executable relies on `numpy` objects. There is a known problem installing numpy on lxplus machines running CentOS7. 

See the discussion here:
https://groups.cern.ch/group/hn-atlas-offlineSWHelp/Lists/Archive/Flat.aspx?RootFolder=%2fgroup%2fhn-atlas-offlineSWHelp%2fLists%2fArchive%2fBroken%20numpy%20in%206%2e14%2e04-x86_64-slc6-gcc62-opt&FolderCTID=0x012002002BD1887D3A9C0D489560C010AA1FE9F4

In the meantime, we suggest to use lxplus6 machines when using `systematicsTool.py` and lxplus7 machines when using `submissionTool.py`**


# Installing the systematics tool

The below are instructions on how to install the tool on lxplus.
```
# for first installation
ssh -Y <username>@lxplus.cern.ch 
mkdir PMGTools
cd PMGTools
git clone ssh://git@gitlab.cern.ch:7999/atlas-physics/pmg/tools/systematics-tools.git
cd systematics-tools
source systematics-tools-bootstrap.sh
# the bootstrap will create a setup script called setupSystematicsTool.sh
# which you can use next time to set everything up easily.
# for example, you can copy this to your home directory
cp setupSystematicsTool.sh ~/.

# for subsequent logins:
ssh -Y <username>@lxplus.cern.ch 
source setupSystematicsTool.sh 

#and you are done!
```
You can test that the loading has happened correctly by calling the executables from anywhere:
```
submissionTool.py -h
systematicsTool.py -h
covarianceTool.py -h
```
And you can test that you are able to import the functions into a custom script like so:
```
lcorpe@lxplus097 systematics-tools]$ python
Type "help", "copyright", "credits" or "license" for more information.
>>> import readDatabase as rdb
>>> import systematicsTool as st
>>> import covarianceToolsLibrary as ct
```

Please see the detailed docuemtenta

## What the tool can and cannot do

***CAN DO***

The tools in this repo can do many things, although for users in PA or CP groups, only a subset of them are likely to be relevant.
The most obvious use is the possibility to automatically combine analysis objects for each on-the-fly (OTF) weight into variations.
For example, one wants to make a plot of jet pT showing the theory uncertainties for PDF, alphaS and Scale.
The tool needs a copy of the jet pT histogram for each OTF-weight (eg 101 copies, one for each PDF weight, two copies, one each for alphaS up and down, and 9 copies for each scale variation) and will look up in a database for a given sample what the relevant uncertainties are, and how they should be combined, and do the combination for you. The output would then be 3 copies of the jet pT histogram (this time as a `TGraph` which is more approrpiate for plotting error bands), one each for PDF uncertainty, alphaS uncertainty and scale uncertainty.  (There may be more than three, this is just an example).

In some cases, if there is a PMG prescription to combine the individual uncertainties into a total error band, the tool will do that too.

The tool will make some plots but more importantly the plots for each variation will be stored in `root` files for you to use downstream in your analysis as you please.

The tool can either be used as an exectuable or the individual functions can be imported into your favourite `python` script.

**CANNOT DO**

The tool can only really evaluate uncertainties which can be derived from OTF weights. For example, it cannot evaluate generator choice uncertainties or radiation uncertainties in some ttbar samples. All the information needs to be contained within a single sample.

The tool cannot combine PDF uncertainties "per event". In other words, you cannot use trees as inputs. The uncertainties are not well defined in that way and you need to propagate the OTF weights all the way to your final observables to evaluate them correctly.

The tool also cannot absolve you of understanding what the uncertainties mean: it is just meant to simplify your life. To find out more about how theory uncertainties, in particular PDF uncertainties, are evaluated, we recommend you study https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PdfRecommendations#PDF_uncertainty_prescriptions and https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgSystematicUncertaintyRecipes. 

In particular, if you are using your own ntuples, you need to make sure you are normalising the weights correctly, eg, following this prescription:
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgSystematicUncertaintyRecipes#On_the_fly_systematic_variations

Now that we are all on the same page, let's continue, and I hope that the tool saves you lots of time avoid re-inventing the wheel.
For clarifications and feature requests, please contact `lcorpe@cern.ch` or add a git issue to this repo.


# Instructions for submissionTool

See the detailed [Documentation](documentation/UsingTheSubmissionTool.md)

# Instructions for systematicsTool

See the detailed [Documentation](documentation/UsingTheSystematicsTool.md)

# Instructions for covarianceTool

See the detailed [Documentation](documentation/UsingTheCovarianceTool.md)
