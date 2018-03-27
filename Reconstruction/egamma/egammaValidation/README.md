Git ART Monitoring Project
=========================

Run using the following:
-----------------------------------------

	art.py run athena/Reconstruction/egamma/egammaValidation/test outART

Explanation & Output:
----------------------------------------

The command will run the reconstruction of two RAW files (electron and photon reconstruction) using the latest Athena build, and compare them to their respective previous versions to check if the changes work as intended.

The outputs, that can be used to investigate any difference, are the following (before the extension each, file has an additional tag to distinguish which one refers to electrons and which one refers to photons [e.g. checkFile_ART_electron.txt or checkFile_ART_gamma.txt])

- checkFile_ART.txt;
- checkxAOD_ART.txt;
- diffFile.txt (comparison of the checkFile_ART.txt with a reference one);
- diffxAOD.txt (comparison of the checkxAOD_ART.txt with a reference one);
- ART-monitoring.root (root file in which few plots are extrapolated from ART_Reco.pool.root).

In order, then, to compare the nightlie with a Baseline plot, one must run:

- python plotsMaker_gamma.py /eos/atlas/atlascerngroupdisk/data-art/grid-input/egammaValidation/Baseline_Files/rootFiles/Base-monitoring_gamma.root path_ARTRoot/name_ARTRoot_gamma
- python plotsMaker_electron.py /eos/atlas/atlascerngroupdisk/data-art/grid-input/egammaValidation/Baseline_Files/rootFiles/Base-monitoring_electron.root path_ARTRoot/name_ARTRoot_electron

The output will be saved in the directory the command is launched and it consists of a .pdf and .root version of the overlap of the "Baseline-monitoring.root" with the "ART-monitoring.root".
