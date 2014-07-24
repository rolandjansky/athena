# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import unittest
import ROOT
from . import get_file, SHARE_DIR, TauCorrUncert as TCU
from ROOT import Long


class EVetoTest(unittest.TestCase):
    """
    class defining test for tau eveto sf
    """
    def setUp(self):

        self.sfFile = get_file('eveto/sf.root')
        #map converting working point to enum
        self.EVetoWP = {'None': 'NONE',
                        'EleBDTLoose': 'LOOSE',
                        'EleBDTMedium': 'MEDIUM',
                        'EleBDTTight': 'TIGHT'}
        self.IDWP = {'None': 'NONE',
                     'JetBDTSigLoose': 'BDTLOOSE',
                     'JetBDTSigMedium': 'BDTMEDIUM',
                     'JetBDTSigTight': 'BDTTIGHT'}
        self.OLRWP = {'None': 'OLRNONE',
                      'loosePP': 'LOOSEPP',
                      'mediumPP': 'MEDIUMPP',
                      'tightPP': 'TIGHTPP'}
        self.readInputHists()
        self.validEtaRange = [float(x)*0.2001 for x in range(0, 12)]
        self.provider = TCU.TauSF(SHARE_DIR)

    def readInputHists(self):
        self.hists = {}
        for evetowp in self. EVetoWP.keys():
            for idwp in self.IDWP.keys():
                for olrwp in self.OLRWP.keys():
                    self.hists[evetowp + idwp + olrwp] = self.sfFile.Get("h_" + idwp + "_" + evetowp + "_" + olrwp + "_SF")
                    self.hists[evetowp + idwp + olrwp + "EU"] = self.sfFile.Get("h_" + idwp + "_" + evetowp + "_" + olrwp + "_EU")
                    self.hists[evetowp + idwp + olrwp + "ED"] = self.sfFile.Get("h_" + idwp + "_" + evetowp + "_" + olrwp + "_ED")


    def testWorkingPoints(self):
        processedWP = 0
        for evetowp in self. EVetoWP:
            for idwp in self.IDWP:
                for olrwp in self.OLRWP:
                    hist = self.hists[evetowp + idwp + olrwp]
                    if not hist:
                        continue
                    for eta in self.validEtaRange:
                        histValue = hist.GetBinContent(hist.FindFixBin(eta))
                        toolValue = self.provider.GetEVetoSF(eta,
                                                             eval('TCU.' + self.IDWP[idwp]),
                                                             eval('TCU.' + self.EVetoWP[evetowp]),
                                                             eval('TCU.' + self.OLRWP[olrwp]))
                        self.assertEqual(histValue, toolValue, "WP:" + evetowp + idwp + olrwp + " eta: " + str(eta) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))
                    processedWP += 1
        #check if all provided working points are processed
        self.assertEqual(processedWP, 14)

    def testWorkingPointsED(self):
        processedWP = 0
        for evetowp in self. EVetoWP:
            for idwp in self.IDWP:
                for olrwp in self.OLRWP:
                    hist = self.hists[evetowp + idwp + olrwp+"ED"]
                    if not hist:
                        continue
                    for eta in self.validEtaRange:
                        histValue = hist.GetBinContent(hist.FindFixBin(eta))
                        toolValue = self.provider.GetEVetoSFUnc(eta,
                                                                Long(eval('TCU.' + self.IDWP[idwp])),
                                                                Long(eval('TCU.' + self.EVetoWP[evetowp])),
                                                                Long(eval('TCU.' + self.OLRWP[olrwp])),
                                                                -1)
                        self.assertEqual(histValue, toolValue, "WP:" + evetowp + idwp + olrwp + " eta: " + str(eta) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))
                    processedWP += 1
        #check if all provided working points are processed
        self.assertEqual(processedWP, 14)

    def testWorkingPointsEU(self):
        processedWP = 0
        for evetowp in self. EVetoWP:
            for idwp in self.IDWP:
                for olrwp in self.OLRWP:
                    hist = self.hists[evetowp + idwp + olrwp+"EU"]
                    if not hist:
                        continue
                    for eta in self.validEtaRange:
                        histValue = hist.GetBinContent(hist.FindFixBin(eta))
                        toolValue = self.provider.GetEVetoSFUnc(eta,
                                                                Long(eval('TCU.' + self.IDWP[idwp])),
                                                                Long(eval('TCU.' + self.EVetoWP[evetowp])),
                                                                Long(eval('TCU.' + self.OLRWP[olrwp])),
                                                                1)
                        self.assertEqual(histValue, toolValue, "WP:" + evetowp + idwp + olrwp + " eta: " + str(eta) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))
                    processedWP += 1
        #check if all provided working points are processed
        self.assertEqual(processedWP, 14)
    def testEtaRange(self):
        hist = self.hists["EleBDTLooseJetBDTSigLooseloosePP"]
        histValue = hist.GetBinContent(hist.FindFixBin(2.46))
        toolValue = self.provider.GetEVetoSF(2.5,
                                             Long(TCU.LOOSE),
                                             Long(TCU.LOOSE),
                                             Long(TCU.LOOSE))

        self.assertEqual(histValue, toolValue)

if __name__ == '__main__':
    suite1= unittest.TestLoader().loadTestsFromTestCase(EVetoTest)
    for suite in [suite1]:
        unittest.TextTestRunner(verbosity=2).run(suite)
