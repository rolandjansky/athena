/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doCreateTheHistograms(TString collectionName,
                           int flavour)
{
  gROOT->ProcessLine(".L createTheHistograms.cxx+");
  createTheHistograms(flavour,
                      collectionName);
}
