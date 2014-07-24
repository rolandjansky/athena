# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import unittest
import ROOT
from __init__ import get_file, SHARE_DIR, TauCorrUncert as TCU
from ROOT import Long


class IDSFTest(unittest.TestCase):
    """
    class defining test for tau eveto sf
    """
    def setUp(self):

        self.sfFile = get_file('ID/sf.root')
        #map converting working point to enum
        self.IDWP = {'None': 'NONE',
                     'JetBDTSigLoose': 'BDTLOOSE',
                     'JetBDTSigMedium': 'BDTMEDIUM',
                     'JetBDTSigTight': 'BDTTIGHT',
                     'tauJLLHLoose': 'LLHLOOSE',
                     'tauJLLHMedium': 'LLHMEDIUM',
                     'tauJLLHTight': 'LLHTIGHT'}
        self.eta = ["barrel", "endcap"]
        self.prong = [1, 3]
        self.pt = ["ptinc", "ptbin"]
        self.readInputHists()
        #self.validEtaRange = [0., 1.4, 2.2]
        self.validEtaRange = [1.4, 2.2]
        self.provider = TCU.TauSF(SHARE_DIR)

    def readInputHists(self):
        self.hists = {}
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.eta:
                    for pt in self.pt:
                        self.hists[idwp+str(prong)+eta+pt] = self.sfFile.Get("ID_SF_" + idwp + "_" + str(prong)  + "p_" + eta + "_" + pt)
                        self.hists[idwp+str(prong)+eta+pt + "Sys"] = self.sfFile.Get("ID_SF_SysUnc_" + idwp + "_" + str(prong) + "p_" + eta + "_" + pt)
                        self.hists[idwp+str(prong)+eta+pt + "Stat"] = self.sfFile.Get("ID_SF_StatUnc_" + idwp + "_" + str(prong) + "p_" + eta + "_" + pt)
                        if not idwp.count("Tight"):
                            self.hists[idwp+str(prong)+eta+pt + "excl"] = self.sfFile.Get("ID_SF_" + idwp + "_excl_" + str(prong)  + "p_" + eta + "_" + pt)
                            self.hists[idwp+str(prong)+eta+pt + "Sys"+ "excl"] = self.sfFile.Get("ID_SF_SysUnc_" + idwp + "_excl_" + str(prong) + "p_" + eta + "_" + pt)
                            self.hists[idwp+str(prong)+eta+pt + "Stat"+ "excl"] = self.sfFile.Get("ID_SF_StatUnc_" + idwp + "_excl_" + str(prong) + "p_" + eta + "_" + pt)

    def testDefaultWorkingPoints(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    name = idwp + str(prong) + "barrelptinc"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetIDSF(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                      eta,
                                                      prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

    def testDefaultWorkingPointsStatUnc(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    name = idwp + str(prong) + "barrelptincStat"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetIDStatUnc(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                           eta,
                                                           prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

    def testDefaultWorkingPointsSysUnc(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    name = idwp + str(prong) + "barrelptincSys"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetIDSysUnc(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                           eta,
                                                           prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

    def testExclusiveWorkingPoints(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    if not idwp.count("Tight"):
                        name = idwp + str(prong) + "barrelptincexcl"
                    else:
                        name = idwp + str(prong) + "barrelptinc"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetExclIDSF(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                          eta,
                                                          prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

    def testExclusiveWorkingPointsStatUnc(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    if not idwp.count("Tight"):
                        name = idwp + str(prong) + "barrelptincStatexcl"
                    else:
                        name = idwp + str(prong) + "barrelptincStat"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetExclIDStatUnc(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                               eta,
                                                               prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

    def testDefaultWorkingPointsSysUnc(self):
        for idwp in self.IDWP.keys():
            for prong in self.prong:
                for eta in self.validEtaRange:
                    if not idwp.count("Tight"):
                        name = idwp + str(prong) + "barrelptincSysexcl"
                    else:
                        name = idwp + str(prong) + "barrelptincSys"
                    if float(eta) > 1.5:
                        name = name.replace("barrel", "endcap")
                    hist = self.hists[name]
                    if not hist:
                        continue
                    histValue = hist.GetBinContent(1)
                    toolValue = self.provider.GetExclIDSysUnc(eval('ROOT.Long(TCU.' + self.IDWP[idwp] + ")"),
                                                              eta,
                                                              prong)
                    self.assertEqual(histValue, toolValue, "WP:" + idwp + " eta: " + str(eta) + " prong: " + str(prong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))
                    
if __name__ == '__main__':
    suite1= unittest.TestLoader().loadTestsFromTestCase(IDSFTest)
    for suite in [suite1]:
        unittest.TextTestRunner(verbosity=2).run(suite)
