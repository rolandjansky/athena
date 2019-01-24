/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"

void run_loop () {
	gROOT->ProcessLine(".L NSWstudies_match.C++");
	gROOT->ProcessLine("NSWstudies t;");
	gROOT->ProcessLine("t.Loop()");
}

/*
This folder contains files used to makes histograms comparing various EDM objects of the NSW digitization chain. 
The underlying matchin method is integrated in the NSW PRD Test, these files are to document the histogram production. 
This is to allow others to reproduce results produced with these files.

To use: 
Copy the containts of this folder to any directory you like, it works offline.
A NSW Ntuple, as produced by the NSWPRDTest, needs to be present in the same directory.
(By default the filename is hardcoded to "NSWPRDValAlg.reco.ntuple.root", to allow a different name, change the creator in NSWstudies.h)
You will need to crate an extra directory named "Rootfiles", where the output histograms will be placed
To run simply run this file (terminal: root -l -q run_loop.C)
In the beginning of the "NSWstudies_match.C" file, the input variables of the NSW Matching algorithm can be set.
(These have the same default values as the NSWPRDTest version)

By default the program will check RDO<->PRD for both sTGC and MM. 
To test other objects, change the input of the match_Hits_Digits and fillHists in the Loop function of NSWstudies_match.C
*/