/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void compile()
{
  gSystem->Load("../libTJetNet.so");
  gROOT->ProcessLine(".L trainNN.cxx++");
}
