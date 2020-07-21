=================================================================
DiTauMassTools: Package holding the Missing Mass Calculator (MMC)
=================================================================

.. contents:: Table of contents

------------
Introduction
------------

This package holds two versions of the MMC. The original version and a refactored version (V2) that aims at being more comprehensible, faster and easier to re-tune by reading in the used PDFs from a root-file.
The documentation of the original package can be found here:
`original MMC <doc/README-old.rst>`_.

Please note that the description on how to set up the tool is still valid for the old version and for the most part also for the new version.

CAUTION: The plan is to phase out the original version of the MMC and to be completely replaced by the new version around 2020/06/30. If your analysis needs the original version after that, please contact michael.huebner@cern.ch or the maintainer of this package ahead of time.

Useful links:

* `Twiki <https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MissingMassCalculator>`_
* `Michael Hübner, refactored MMC performance <https://indico.cern.ch/event/858190/contributions/3615217/attachments/1931095/3198553/mmc_hleptons.pdf>`_
* Older tutorials:

  * `Dimitris Varouchas, how to run MMC <https://indico.cern.ch/getFile.py/access?contribId=3&resId=5&materialId=slides&confId=143074>`_
  * `Sasha Pranko, Ideas on how to tune MMC performance <https://indico.cern.ch/getFile.py/access?contribId=5&resId=0&materialId=slides&confId=143074>`_

-----
Setup
-----

(This section is adapted from TauAnalysisTools, since the handling and workflow is very much the same.)

Please have a look into this `tutorial <https://atlassoftwaredocs.web.cern.ch/ABtutorial/>`_ to get general information on how to setup an analysis release. In particular the section about initializing CP tools via `AnaToolHandles <https://atlassoftwaredocs.web.cern.ch/ABtutorial/basic_ana_tool_handle/>`_ might be worth to read.

A vanilla version of DiTauMassTools comes with AnalysisBase and AthAnalysis releases. These can be set up with e.g.::

        asetup 21.2.101,AnalysisBase or
        asetup 21.2.101,AthAnalysis

21.2.101 is the first release containing V2 of the MMC.

The latest releases and changes are documented on the `AnalysisRelease page <https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisBaseReleaseNotes21_2>`_.
If you plan to contribute to DiTauMassTools, please refer to the `ATLAS Git Workflow Tutorial <https://atlassoftwaredocs.web.cern.ch/gittutorial/>`_. This tutorial describes how to get your own copy of the offline code from GitLab and how to prepare a merge request that asks for your code to be added to the main repository. For such developments it is recommended to set up the latest nightly with::

        asetup 21.2,AnalysisBase,latest or
        asetup 21.2,AthAnalysis,latest

The general settings of the tool are handled via properties. The most important ones being:

* Decorate (default: false): This will decorate the results of the MMC to the EventInfo object.
* FloatStoppingCrit (default: true): This activates the floating stopping criterion which saves a factor ~2-3 in CPU time (see presentation by Michael Hübner).
* CalibSet (default: 2019): Needs to be set to the appropriate calibration set depending on the use-case. For Higgs to tau tau the recommendation is to use 2019 (this uses a root file for the PDFs, older versions use hard-coded numbers).
* UseTauProbability (default: false): This includes an additional PDF term if set to true (ratio of neutrino momentum w.r.t. reconstructed tau momentum). It is recommended to set this to true.
* UseMnuProbability (default: false): This includes an additional PDF term if set to true (mass of neutrino system per tau decay, only applied to leptonic tau decays).
* NsigmaMET (default: 3, 4 depending on decay type): This sets the range of the MET scan for the Markov Chain. Can be increased to recover efficiency.
* ParamFilePath (default: MMC_params_v1_fixed.root): Sets the root file used for CalibSet>=2019, contains the PDFs for the likelihood. Can be changed for studies of PDFs (expert use), otherwise use default if the default in your ABR is MMC_params_v1_fixed.root. If you are using release 21.2.101 you have to set this property to the new default listed here.

-------
Example
-------

An minimal example on how to set up the tool can be found in ``test/ut_ditaumasstools_missingmasstoolv2_test.cxx``.

-------
Classes
-------

In order to streamline the code there have been several new classes introduced for the refactored MMC.

* MissingMassInput: handles the input passed to the MMC
* MissingMassOutput: handles the output of the MMC
* HelperFunctions: collection of functions for general usage
* MissingMassProb: handles the probability calculation for the likelihoods
* MissingMassCalculatorV2: uses the other classes and handles the Markov Chain, i.e. the phase space scan and the calculation of the neutrino results

--------------------------
Remarks on the refactoring
--------------------------

The LFV part of the MMC remained the same, which means that the new parametrisations are also not supported.
The new parametrisations are based on Z->tautau MC16 samples, so there are no new parametrisations for Upgrade studies or high mass searches in the new CalibrationSet (2019).

----------
Expert use
----------

If you want to test against the original MMC, it is recommended to compare the output of the method MAXW. You should set the seeds via SetSeed to the same number and comment out the warm up phase of the RNG in the original MMC.

In order to test your own calibration files locally, you could save them in a folder share/file.root and let CMake install the files, e.g. via::

        atlas_install_runtime( share/*.root )
