#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import math
from ROOT import TFile, TH1D, TH2D, gRandom

def fillBin(h, xlabel, ylabel, n):
  h.SetBinContent(h.GetXaxis().FindBin(xlabel), h.GetYaxis().FindBin(ylabel), n)

def makeTestFile(fileName, seed):
  f = TFile(fileName, "RECREATE")
  nTotal = 100

  total = TH1D("total", "", 10, 0, 10)
  total.FillRandom("gaus", nTotal)

  chains = TH2D("chains", "chainschainstep", 3, 0, 3, 6, 0, 6)
  decisions = TH2D("decisions", "decisionschainstep", 3, 0, 3, 6, 0, 6)
  for h in [chains, decisions]:
    h.GetXaxis().SetBinLabel(1, "HLT_testChain1")
    h.GetXaxis().SetBinLabel(2, "HLT_testChain2")
    h.GetXaxis().SetBinLabel(3, "HLT_testChain3")
    h.GetYaxis().SetBinLabel(1, "L1")
    h.GetYaxis().SetBinLabel(2, "AfterPS")
    h.GetYaxis().SetBinLabel(3, "Step 0")
    h.GetYaxis().SetBinLabel(4, "Step 1")
    h.GetYaxis().SetBinLabel(5, "Step 2")
    h.GetYaxis().SetBinLabel(6, "Output")

  for iChain in range(1,4):
    chainName = f"HLT_testChain{iChain}"
    nInput = math.floor(gRandom.Uniform(nTotal))
    nOutput = math.floor(gRandom.Uniform(nInput//2, nInput))
    fillBin(chains, chainName, "L1", nInput)
    fillBin(chains, chainName, "AfterPS", nInput)
    fillBin(chains, chainName, "Output", nOutput)
    for iStep in range(0,3):
      stepName = f"Step {iStep}"
      nStepMin = (8-iStep)*nInput//10
      nStepMax = (9-iStep)*nInput//10
      nStep = math.floor(gRandom.Uniform(nStepMin, nStepMax))
      nDecisions = math.floor(gRandom.Uniform(nStepMax, 2*nInput))
      fillBin(chains, chainName, stepName, nStep)
      fillBin(decisions, chainName, stepName, nDecisions)

  f.Write()


print("Creating test file testChainDump1.root")
gRandom.SetSeed(1234)
makeTestFile("testChainDump1.root", seed=1234)

print("Creating test file testChainDump2.root")
gRandom.SetSeed(12345)
makeTestFile("testChainDump2.root", seed=12345)
