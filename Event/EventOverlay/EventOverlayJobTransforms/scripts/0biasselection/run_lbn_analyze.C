/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
 gROOT->ProcessLine(".L lbn_analyze.C+");

 for (int i=0; i<50; ++i){//number of zerobias streams to make
   //gSystem->Sleep(2000);//to get new random number seed
   lbn_analyze(i,50000);//number of events per zerobias stream
 }

}

