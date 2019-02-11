This folder contains files used to makes histograms comparing various EDM objects of the NSW digitization chain. 
The underlying matching method is integrated in the NSW PRD Test, these files are to document the histogram production. 
This is to allow others to reproduce results produced with these files.

To use: 
Copy the contents of this folder to any directory you like, it works offline.
A NSW Ntuple, as produced by the NSWValAlg, needs to be present in the same directory.
(By default the filename is hardcoded to "NSWPRDValAlg.reco.ntuple.root", to allow a different name, change the creator in NSWstudies.h)
To run: simply run this file (terminal: root -l -q run_loop.C)

By default the program will check RDO<->PRD for both sTGC and MM. 
To test other objects, change the input of the match_Hits_Digits and fillHists in the Loop function of NSWstudies_match.C

In the beginning of the "NSWstudies_match.C" file, the input variables of the NSW Matching algorithm can be set.
(These have the same default values as the NSWPRDTest version)