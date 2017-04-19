/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TTree.h>
#include <cmath>
#include <sstream>

int main ()
{
  for (unsigned jter = 0; jter < 3; ++ jter)
  {
    std::ostringstream strpath;
    strpath << "$ROOTCOREBIN/data/EventLoop/test_ntuple" << jter << ".root";
    TString path = strpath.str();
    gSystem->ExpandPathName (path);
    TFile file (path.Data(), "RECREATE");
    TH1 *hist_n = new TH1F ("hist_n", "el_n", 50, 0, 50);

    if (jter != 0)
    {
      TTree *tree = new TTree ("physics", "test tree");
      Int_t el_n = 0;
      tree->Branch ("el_n", &el_n, "el_n/I");
      std::vector<float> *el_pt = new std::vector<float>;
      tree->Branch ("el_pt", &el_pt);
      Float_t el_tag [20];
      tree->Branch ("el_tag", &el_tag, "el_tag[el_n]/F");

      for (unsigned iter = 0; iter != 10 * jter; ++ iter)
      {
	el_n = floor (sqrt (iter + 3 * jter));
	el_pt->clear ();
	for (unsigned kter = 0, end = el_n; kter != end; ++ kter)
	{
	  el_pt->push_back ((jter + 1) * (iter + 1) * (end - kter) * 1e3);
	  el_tag[kter] = exp (iter + kter) - floor (iter + kter);
	}
	hist_n->Fill (el_n);
	tree->Fill ();
      }
    }
    file.Write ();
    file.Close ();
  }

  return 0;
}
