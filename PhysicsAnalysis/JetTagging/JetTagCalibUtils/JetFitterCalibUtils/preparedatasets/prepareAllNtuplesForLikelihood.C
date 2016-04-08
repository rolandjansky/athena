/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
gROOT->ProcessLine(".L readJFBTagAna.C+");
gROOT->ProcessLine(".L readBaseBTagAnaTree.C+");
gROOT->ProcessLine(".L writeNtuple_Official.cxx+");
gROOT->ProcessLine(".L writeNtupleAll.C+");

writeAllNtuples("../datasets/all.root",false);

}
