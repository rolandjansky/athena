/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: interaction.cc,v 1.5 2008-07-23 20:51:23 adye Exp $
#include <string>

#include "TTree.h"
#include "TCanvas.h"
#include "TCut.h"

using std::string;

void interaction (TTree *tree, const string & plotprefix="interaction_", const string & plotext="",
                  const TCut mc_cut="")
{

  //cppcheck-suppress incorrectStringBooleanError
  TCut cut_electron  (mc_cut && "abs(mctrack_pdgid)==11  && sqrt(pow(mctrack_endvertexx,2)+pow(mctrack_endvertexy,2))<3000");
  //cppcheck-suppress incorrectStringBooleanError
  TCut cut_pion      (mc_cut && "abs(mctrack_pdgid)==211  && sqrt(pow(mctrack_endvertexx,2)+pow(mctrack_endvertexy,2))<3000");

 

  TCanvas *end_e    =  new TCanvas("end_e",     "electron truth track end vertex",800,400);
  TCanvas *end_pi   =  new TCanvas("end_pi",    "pion truth track end vertex",800,400);

  end_e->Divide(2,1);
  end_pi->Divide(2,1);

  end_e->cd(1);
  tree->Draw("mctrack_endvertexx:mctrack_endvertexy",cut_electron);
  end_e->cd(2);
  tree->Draw("sqrt(pow(mctrack_endvertexx,2)+pow(mctrack_endvertexy,2)):mctrack_endvertexz",cut_electron);
  end_pi->cd(1);
  tree->Draw("mctrack_endvertexx:mctrack_endvertexy",cut_pion);
  end_pi->cd(2);
  tree->Draw("sqrt(pow(mctrack_endvertexx,2)+pow(mctrack_endvertexy,2)):mctrack_endvertexz",cut_pion);

  if (plotext != "") {
    string plotfile;
    plotfile = plotprefix+"e" +plotext; end_e ->Print(plotfile.c_str());
    plotfile = plotprefix+"pi"+plotext; end_pi->Print(plotfile.c_str());
  }

  delete end_e;
  delete end_pi;
}

