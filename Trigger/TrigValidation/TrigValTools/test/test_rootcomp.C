// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
// Create two ROOT files with histograms used in test_rootcomp.sh
{
  // Histogram for both files:
  TH1D h1("h1", "", 10, 0, 10);
  h1.Fill(5);
  {
    TFile f("hist1.root", "RECREATE");
    h1.Write();

    TH1D hr("hrandom", "", 10, 0, 10);
    hr.FillRandom("gaus", 1000);

    TH1D hl("hlabel", "", 5, 0, 5);
    hl.Fill("a", 1);
    hl.Fill("b", 2);

    TH1D h2("hextra", "", 10, 0, 10);

    f.Write();
  }

  {
    TFile f("hist2.root", "RECREATE");
    h1.Write();

    TH1D hr("hrandom", "", 10, 0, 10);
    hr.FillRandom("gaus", 1000);

    TH1D hl("hlabel", "", 4, 0, 4);   // one less (empty) bin than above
    hl.Fill("b", 2);
    hl.Fill("a", 1);

    f.Write();
  }
}
