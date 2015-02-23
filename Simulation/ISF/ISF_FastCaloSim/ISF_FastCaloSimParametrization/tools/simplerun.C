/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void simplerun(TString filename="ISF_calo__211__E10000_10000__eta20_20_Evts0-1000_z03350.pool.root",TString outputname="output1.root")
{
  gROOT->LoadMacro("FCS_Cell.h+");
  gROOT->LoadMacro("CaloHitAna.C+");
  //CaloHitAna t;
  CaloHitAna t(filename,outputname);
  t.SetDoLayers(1);
  t.SetDoLayerSums(1);
  t.m_PrintOutFrequency=1;
  t.m_max_nentries=5;
  t.m_Debug=2;
  t.Loop();
  //No need to call finish now
}


