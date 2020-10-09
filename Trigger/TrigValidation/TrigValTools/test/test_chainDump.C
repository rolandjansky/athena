/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void fillBin(TH2& h, const char* xlabel, const char* ylabel, size_t n) {
  h.SetBinContent(h.GetXaxis()->FindBin(xlabel), h.GetYaxis()->FindBin(ylabel), n);
}

void makeTestFile(const char* fileName) {
  TFile f(fileName, "RECREATE");
  size_t nTotal = 100;

  TH1D total("total", "", 10, 0, 10);
  total.FillRandom("gaus", nTotal);

  TH2D chains("chains", "chains;chain;step", 3, 0, 3, 6, 0, 6);
  TH2D decisions("decisions", "decisions;chain;step", 3, 0, 3, 6, 0, 6);
  for (TH2D* h : {&chains, &decisions}) {
    h->GetXaxis()->SetBinLabel(1, "HLT_testChain1");
    h->GetXaxis()->SetBinLabel(2, "HLT_testChain2");
    h->GetXaxis()->SetBinLabel(3, "HLT_testChain3");
    h->GetYaxis()->SetBinLabel(1, "L1");
    h->GetYaxis()->SetBinLabel(2, "AfterPS");
    h->GetYaxis()->SetBinLabel(3, "Step 0");
    h->GetYaxis()->SetBinLabel(4, "Step 1");
    h->GetYaxis()->SetBinLabel(5, "Step 2");
    h->GetYaxis()->SetBinLabel(6, "Output");
  }
  for (size_t iChain=1; iChain<4; ++iChain) {
    TString chainName = TString::Format("HLT_testChain%lu", iChain);
    size_t nInput = gRandom->Uniform(nTotal);
    size_t nOutput = gRandom->Uniform(nInput/2, nInput);
    fillBin(chains, chainName.Data(), "L1", nInput);
    fillBin(chains, chainName.Data(), "AfterPS", nInput);
    fillBin(chains, chainName.Data(), "Output", nOutput);
    for (size_t iStep=0; iStep<3; ++iStep) {
      TString stepName = TString::Format("Step %lu", iStep);
      size_t nStepMin = (8-iStep)*nInput/10;
      size_t nStepMax = (9-iStep)*nInput/10;
      size_t nStep = gRandom->Uniform(nStepMin, nStepMax);
      size_t nDecisions = gRandom->Uniform(nStepMax, 2*nInput);
      fillBin(chains, chainName.Data(), stepName.Data(), nStep);
      fillBin(decisions, chainName.Data(), stepName.Data(), nDecisions);
    }
  }

  f.Write();
}

void test_chainDump() {
  delete gRandom;
  gRandom = new TRandom3(1234); // arbitrary fixed seed
  Info("test_chainDump", "Creating test file testChainDump1.root");
  makeTestFile("testChainDump1.root");
  gRandom->SetSeed(12345); // different arbitrary fixed seed
  Info("test_chainDump", "Creating test file testChainDump2.root");
  makeTestFile("testChainDump2.root");
}
