/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void Workbench()
{
    gSystem->Load( "libdqm_core.so" );
    gSystem->Load( "libdqm_dummy.so" );
    gSystem->Load( "libdqm_tools.so" );
    gSystem->Load( "libdqm_algorithms.so" );
    gROOT->ProcessLine( ".include .." );
}
