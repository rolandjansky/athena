/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

executeCreateAllHistograms(int number=0,int flavour=0) {
  gROOT->ProcessLine(".L createTheHistograms.cxx+g");
  gROOT->ProcessLine(".L createAllHistograms.cxx+g");
//  createAllHistograms(number,flavour);
  createAllHistograms();

}

