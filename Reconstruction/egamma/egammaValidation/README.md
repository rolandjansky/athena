Git PhotonRecoValidation Project
=========================

Run using the following:
-----------------------------------------

art.py run /afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test outART

Explanation & Output:
----------------------------------------

The command will run the reconstruction of a RAW file using the latest Athena build, and compare it to a previous one to check if the changes work as intended.

The outputs that can be used to investigate any differences can be found in "outART/egammaValidation/test_ART/" and are the following

- checkFile_ART.txt;
- checkxAOD_ART.txt;
- diffFile.txt (comparison of the checkFile_ART.txt with a reference one);
- diffxAOD.txt (comparison of the checkxAOD_ART.txt with a reference one).