Git ART Monitoring Project
=========================

Run using the following:
-----------------------------------------

	art.py run /afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test outART

Explanation & Output:
----------------------------------------

The command will run the reconstruction of a RAW file using the latest Athena build, and compare it to a previous one to check if the changes work as intended.

The outputs, that can be used to investigate any difference, can be found in "outART/egammaValidation/test_artoverseer/" and are the following

- checkFile_ART.txt;
- checkxAOD_ART.txt;
- diffFile.txt (comparison of the checkFile_ART.txt with a reference one);
- diffxAOD.txt (comparison of the checkxAOD_ART.txt with a reference one);
- ART-monitoring.root (root file in which few plots are extrapolated from ART_Reco.pool.root).

In order, then, to compare the nightlie with a Baseline plot, one must run:

   python plotsMaker.py <path_BaselineRoot><name_BaselineRoot> <path_ARTRoot><name_ARTRoot>

The output will be saved in the directory the command is launched and it consists of a .pdf and .root version of the overlap of the "Baseline-monitoring.root" with the "ART-monitoring.root".