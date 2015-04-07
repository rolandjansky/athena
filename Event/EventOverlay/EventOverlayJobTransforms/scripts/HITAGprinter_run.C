/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  TChain c("POOLCollectionTree");
  int n=0;
  n=c.Add("/datadisk1/temp2/HITAG/*"); if (n<1) {printf("Added %d files!\n",n); return;} else {printf("Added %d files.\n",n);}
  gROOT->ProcessLine(".L HITAGprinter.C");
  HITAGprinter t(&c);
  t.Loop();

}
