// $Id$
#include <TFile.h>
#include <TError.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "TruthWeightTools/HiggsWeightTool.h"
#include <TH1F.h>

typedef TString Str;
typedef std::vector<Str> StrV;
typedef std::vector<TH1F *> HistV;
typedef std::vector<double> NumV;
void fatal(TString msg) { printf("\nFATAL:\n\n  %s\n\n", msg.Data()); abort(); }
void fillHistos(HistV &hw, double x, NumV ws)
{
  if (hw.size() < ws.size()) { fatal("Bad input to fillHistos"); }

  for (size_t i = 0; i < ws.size(); ++i) { hw[i]->Fill(x, ws[i]); }
}
HistV makeHistos(int N, Str prefix, int Nbins, double min, double max, Str tit = "")
{
  HistV hv;

  for (int i = 1; i <= N; ++i) { hv.push_back(new TH1F(prefix + Form("%i", i), tit, Nbins, min, max)); }

  return hv;
}
HistV makeHistos(StrV names, int Nbins, double min, double max, Str tit = "")
{
  HistV hv;

  for (Str n : names) { hv.push_back(new TH1F(n, tit, Nbins, min, max)); }

  return hv;
}
TString per(double frac)
{
  if (frac >= 0) { return Form("+%.1f%%", frac * 100); }

  return Form("%.1f%%", frac * 100);
}
TString per(double var, double nom) { return per((var - nom) / nom); }

int main(int argc, char *argv[])
{

  // Get the application's name:
  const char *APP_NAME = argv[ 0 ];

  TString fn("/eos/atlas/user/j/jvasquez/public/mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.merge.DAOD_HIGG1D1.e5607_s2726_r7772_r7676_p3015/DAOD_HIGG1D1.10654168._000034.pool.root.1");

  for (int i = 1; i < argc; ++i) {
    TString arg(argv[i]);

    if (arg.Contains(".root")) { fn = arg; }
    else { std::runtime_error(TString("Cannot intepret argument: " + arg).Data()); }
  }

  int  Nbins = 14;
  double min = 0, max = 14;
  Str tit = ";STXS fine index";
  TH1F *h_STXS = new TH1F("STXS", tit, Nbins, min, max);
  HistV h_STXS_pdf4lhc = makeHistos(30, "STXS_pdf4lhc", Nbins, min, max, tit);
  HistV h_STXS_aS = makeHistos({"STXS_aSup", "STXS_aSdn"}, Nbins, min, max, tit);

  // Initialise the application:
  RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

  // Create a TEvent object:
  xAOD::TEvent event(xAOD::TEvent::kBranchAccess);

  xAOD::HiggsWeightTool *higgsMCtool = new xAOD::HiggsWeightTool("HiggsWeightTool");
  higgsMCtool->setProperty("OutputLevel", MSG::DEBUG).ignore();
  higgsMCtool->setProperty("ForceNNLOPS", true).ignore();
  higgsMCtool->setProperty("WeightCutOff", 150.0).ignore();
  higgsMCtool->initialize().ignore();

  // Open the input file:
  std::unique_ptr< ::TFile > ifile(::TFile::Open(fn, "READ"));

  if (! ifile.get()) { fatal("Could not open file " + fn); }

  ::Info(APP_NAME, "Opened file: %s", fn.Data());

  // Connect the TEvent object to it:
  RETURN_CHECK(APP_NAME, event.readFrom(ifile.get()));

  // Loop over a few events:
  const ::Long64_t entries = event.getEntries();

  for (::Long64_t entry = 0; entry < entries; ++entry) {

    // Get the current entry:
    event.getEntry(entry);

    const xAOD::EventInfo *evtInfo;
    RETURN_CHECK(APP_NAME, event.retrieve(evtInfo, "EventInfo"));

    std::vector<float> weights = evtInfo->mcEventWeights();
    int HTXS_index   = evtInfo->auxdata<int>("HTXS_Stage1_FineIndex_pTjet30");

    // Access all Higgs weights
    xAOD::HiggsWeights hw = higgsMCtool->getHiggsWeights();

    // Fill histos
    h_STXS -> Fill(HTXS_index, hw.nominal);
    fillHistos(h_STXS_pdf4lhc, HTXS_index, hw.pdf4lhc_unc);
    fillHistos(h_STXS_aS, HTXS_index, {hw.alphaS_up, hw.alphaS_dn});

    // Give some feedback of where we are:
    if ((entry + 1) % 10000 == 0) { ::Info(APP_NAME, "Processed %5llu/%5llu events", entry + 1, entries); }

  } // for each entry

  higgsMCtool->finalize().ignore();

  // 1. let's normalize to the ggF cross section
  //    this means scale the sumw in each bin by
  //      sig(ggF) / sum w total
  double sf = 48.52 / h_STXS->Integral(0, -1);
  h_STXS->Scale(sf);

  for (auto *h : h_STXS_pdf4lhc) { h->Scale(sf); }

  // Loop over the categories and print out the cross sections
  printf("\n  Cross sections in pb\n\n");
  printf("%5s%6s%9s", "STXS", "sig", "MC-stat");
  int Npdf = 12; // number of colums, cutting off
  int Nstxs = 12; // number of Stage 1 ggF STXS categories

  for (int j = 0; j < Npdf; ++j) { printf("  PDF%2i", j + 1); }

  printf("\n");

  for (int i = 1; i <= Nstxs; ++i) {
    int bin = i + 1;
    // Nominal cross section and MC stat error
    printf("%5i%6.2f +/- %.2f", i, h_STXS->GetBinContent(bin), h_STXS->GetBinError(bin));

    for (int j = 0; j < Npdf; ++j) { printf("%7.2f", h_STXS_pdf4lhc[j]->GetBinContent(bin)); }

    printf("\n");
  }

  printf("%5s%6.2f +/- 0.00", "Tot", h_STXS->Integral(0, -1));

  for (int j = 0; j < Npdf; ++j) { printf("%7.2f", h_STXS_pdf4lhc[j]->Integral(0, -1)); }

  printf("\n\n");

  // same table but realtive uncertaitnies
  printf("\n  Cross section with relative uncertaitnies\n\n");
  printf("%5s%9s", "STXS", "sig");

  for (int j = 0; j < Npdf; ++j) { printf("  PDF%2i", j + 1); }

  printf(" PDF1-30\n");
  double V = 0.0, nom = 0;

  for (int i = 1; i <= Nstxs; ++i) {
    int bin = i + 1;
    nom = h_STXS->GetBinContent(bin);
    // Nominal cross section and MC stat error
    printf("%5i%6.2f pb", i, nom);
    V = 0;

    for (int j = 0; j < 30; ++j) { // loop over all 30 PDF vars
      double nj = h_STXS_pdf4lhc[j]->GetBinContent(bin);
      V += pow(nj - nom, 2);

      if (j < Npdf) { printf("%7s", per(nj, nom).Data()); }
    }

    printf("%8s\n", per(sqrt(V) / nom).Data());
  }

  V = 0;
  nom = h_STXS->Integral(0, -1);
  printf("%5s%6.2f pb", "Tot", nom);

  //   printf("%5s%6.2f pb  %s","Tot",nom,per(0).Data());
  for (int j = 0; j < 30; ++j) {
    double nj = h_STXS_pdf4lhc[j]->Integral();

    if (j < Npdf) { printf("%7s", per(nj, nom).Data()); }

    V += pow(nj - nom, 2);
  }

  printf("%8s\n", per(sqrt(V) / nom).Data());

  // Loop over the categories and print out the cross sections
  printf("\n  Acceptances in percent\n\n");
  printf("%5s%6s%9s", "STXS", "acc", "MC-stat");
  double tot = h_STXS->Integral();

  for (int j = 0; j < Npdf; ++j) { printf("  PDF%2i", j + 1); }

  printf("\n");

  for (int i = 1; i <= Nstxs; ++i) {
    int bin = i + 1;
    // acc = n_i / sum(n_i) = a/(a+b)
    double n_i = h_STXS->GetBinContent(bin), Dn_i = h_STXS->GetBinError(bin),
           acc_i = n_i / tot, Dacc_i = Dn_i / tot;
    printf("%5i%6.1f +/- %4.1f", i, acc_i * 100, Dacc_i * 100);

    for (int j = 0; j < Npdf; ++j)
    { printf("%7.1f", h_STXS_pdf4lhc[j]->GetBinContent(bin) / h_STXS_pdf4lhc[j]->Integral() * 100); }

    printf("\n");
  }

  printf("\n");

  // same table but realtive uncertaitnies
  printf("\n  Acceptance with relative uncertaitnies\n\n");
  printf("%5s%6s", "STXS", "acc");

  for (int j = 0; j < Npdf; ++j) { printf("  PDF%2i", j + 1); }

  printf(" PDF1-30\n");

  for (int i = 1; i <= Nstxs; ++i) {
    int bin = i + 1;
    double acc_nom = h_STXS->GetBinContent(bin) / tot;
    printf("%5i%6.1f", i, acc_nom * 100);
    V = 0;

    for (int j = 0; j < 30; ++j) {
      double acc_j = h_STXS_pdf4lhc[j]->GetBinContent(bin) / h_STXS_pdf4lhc[j]->Integral();
      V += pow(acc_j - acc_nom, 2);

      if (j < Npdf) { printf("%7s", per(acc_j, acc_nom).Data()); }
    }

    printf("%8s\n", per(sqrt(V) / acc_nom).Data());
  }

  printf("\n");

  // Return gracefully:
  return 0;
}

