Git ART Monitoring Project
===========================

Run (from your terminal) using the following:
----------------------------------------------

Locally     -->	art.py run   athena/Reconstruction/egamma/egammaValidation/test outART
On the Grid --> art.py grid  athena/Reconstruction/egamma/egammaValidation/test outART

More info at: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ART#Introduction

NB It runs automatically every night.

Explanation & Output:
----------------------

The command runs the reconstruction of two RAW files (electron and photon reconstruction) using the latest Athena build, and compare them to their respective previous versions to check if the changes work as intended.

The outputs, that can be used to investigate any difference, are the following (before the extension each, file has an additional tag to distinguish which one refers to electrons and which one refers to photons [e.g. checkFile_ART_electron.txt or checkFile_ART_gamma.txt])

- checkFile_ART.txt;
- checkxAOD_ART.txt;
- ART-monitoring.root (root file in which few plots are extrapolated from ART_Reco.pool.root);
- .png and .root version of the overlap of the "Baseline-monitoring_gamma.hist.root" with the "Nightly-monitoring_gamma.hist.root" .
