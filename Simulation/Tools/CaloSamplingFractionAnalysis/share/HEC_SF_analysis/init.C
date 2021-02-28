/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void init()
{
   gROOT->ProcessLine(".L HEC_SF_analysis/mypage.C");
   gROOT->ProcessLine(".L HEC_SF_analysis/myplot.C");
//
   gROOT->ProcessLine(".L HEC_SF_analysis/samfr_etaX.C+");
   gROOT->ProcessLine(".L HEC_SF_analysis/read_var_cutX.C+");
}
