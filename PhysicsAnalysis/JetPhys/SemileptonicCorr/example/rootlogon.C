/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void rootlogon()
{
  // Load ATLAS style
  gROOT->LoadMacro("AtlasStyle.C");
  AtlasStyle();
}
