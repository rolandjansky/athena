# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import unittest
import ROOT
from . import get_file, SHARE_DIR, TauCorrUncert as TCU
from ROOT import Long


class BDTSFTest(unittest.TestCase):
    """
    class defining test for BDT sf
    """
    def setUp(self):

        self.sfFile = get_file('ID/bdtsf.root')
        #map converting working point to enum

	# Central Values
        self.BaseName = 'ID_SF'
        self.Eta = ['', '_barrel', '_endcap']
        self.Prongs = ['_1p', '_3p']
        self.nProngs = [1,3]
	# Variations
        self.stat = '_stat'
        self.Sys = ['_sys', '_sys_QCD', '_sys_Wjets', '_sys_TES', '_sys_Other']#
        self.sysConverter = {'_sys': "TOTAL", '_sys_QCD' : "QCD",  '_sys_Wjets' : "WJETS", '_sys_TES' : "TES", '_sys_Other' : "REMAIN"}#
        self.Direction = ['_UP', '_DOWN']

        self.readInputHists()
        #self.validEtaRange = [0., 1.4, 2.2]
        self.validBDTRange = [0.51,0.61,0.81]
        self.validEtaRange = [0.,1.1,1.3,1.5,2.2]

        self.provider = TCU.BDTSF(SHARE_DIR)

    def readInputHists(self):
        self.hists = {}

        for eta in self.Eta:
            for prong in self.Prongs:
		# Central Value
                self.hists[self.BaseName+prong+eta] = self.sfFile.Get(self.BaseName+prong+eta)
		# Stat Up and Down
                for direction in self.Direction:
                    self.hists[self.BaseName+self.stat+direction+prong+eta] = self.sfFile.Get(self.BaseName+self.stat+direction+prong+eta)
		# Sys Up and Down
                for sys in self.Sys:
                    for direction in self.Direction:
                        self.hists[self.BaseName+sys+direction+prong+eta] = self.sfFile.Get(self.BaseName+sys+direction+prong+eta)


    def testDefaultWorkingPoints(self):
        for nprong in self.nProngs:
            if nprong == 1:
                prong = '_1p'
            else:
                prong = '_3p'
            for bdt in self.validBDTRange:
                hist = self.hists[self.BaseName+prong]
                x = hist.FindBin(bdt)
                histValue = hist.GetBinContent(x)
                toolValue = self.provider.getBDTbinnedSF(nprong, bdt)
                self.assertEqual(histValue, toolValue, "BDT: "+str(bdt)+"\nprong: " + str(nprong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

                for eta in self.validEtaRange:
		    # Central Value
                    if eta < 1.37:
                        neta = '_barrel'
                    else:
                        neta = '_endcap'

                    hist = self.hists[self.BaseName+prong+neta]
                    x = hist.FindBin(bdt)
                    histValue = hist.GetBinContent(x)
                    toolValue = self.provider.getBDTbinnedSF(nprong, eta, bdt)
                    self.assertEqual(histValue, toolValue, "eta: " + str(eta) + " prong: " + str(nprong) +"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

		
    def testDefaultWorkingPointsStatUnc(self):
        for nprong in self.nProngs:
            if nprong == 1:
                prong = '_1p'
            else:
                prong = '_3p'
            for bdt in self.validBDTRange:
                for direction in ['DOWN', 'UP']:

                    hist = self.hists[self.BaseName+self.stat+'_'+direction+prong]
                    x = hist.FindBin(bdt)
                    histValue = hist.GetBinContent(x)
                    toolValue = self.provider.getBDTbinnedSFStatUnc(nprong, bdt, direction)
                    self.assertEqual(histValue, toolValue, "BDT: "+str(bdt)+"\nprong: " + str(nprong) +"\n direction: "+direction+"\n hist value: " + str(histValue) + " tool value: " + str(toolValue))

                for eta in self.validEtaRange:
		    # Central Value
                    if eta < 1.37:
                        neta = '_barrel'
                    else:
                        neta = '_endcap'
                    for direction in ['DOWN', 'UP']:
                        hist = self.hists[self.BaseName+self.stat+'_'+direction+prong+neta]
                        x = hist.FindBin(bdt)
                        histValue = hist.GetBinContent(x)
                        toolValue = self.provider.getBDTbinnedSFStatUnc(nprong, eta, bdt, direction)
                        print nprong, eta, bdt, direction
                        self.assertEqual(histValue, toolValue, "eta: " + str(eta) + " prong: " + str(nprong) +"\n direction: " + direction + "\n hist value: " + str(histValue) + " tool value: " + str(toolValue))


    def testDefaultWorkingPointsSysUnc(self):
        for nprong in self.nProngs:
            if nprong == 1:
                prong = '_1p'
            else:
                prong = '_3p'
            for bdt in self.validBDTRange:
                for sys in self.Sys:
                    for direction in ['DOWN', 'UP']:
                        hist = self.hists[self.BaseName+sys+'_'+direction+prong]
                        x = hist.FindBin(bdt)
                        histValue = hist.GetBinContent(x)
                        if sys == '_sys':
                            tempsys  = 'Sys'
                        else:
                            tempsys = sys.replace("_sys_","")
                        toolValue = self.provider.getBDTbinnedSFSysUnc(nprong,
                                                                       bdt,
                                                                       direction,
                                                                       eval('TCU.' + self.sysConverter[sys]))
                        self.assertEqual(histValue, toolValue, "BDT: "+str(bdt)+"\nprong: " + str(nprong) +"\nsystematic: "+tempsys+"\ndirection: "+direction+"\nhist value: " + str(histValue) + "\ntool value: " + str(toolValue))

                    for eta in self.validEtaRange:
			# Central Value
                        if eta < 1.37:
                            neta = '_barrel'
                        else:
                            neta = '_endcap'
                        for direction in ['DOWN', 'UP']:
                            hist = self.hists[self.BaseName+sys+'_'+direction+prong+neta]
                            x = hist.FindBin(bdt)
                            histValue = hist.GetBinContent(x)
                            toolValue = self.provider.getBDTbinnedSFSysUnc(nprong,
                                                                           eta,
                                                                           bdt,
                                                                           direction,
                                                                           eval('TCU.' + self.sysConverter[sys]))
                            self.assertEqual(histValue, toolValue, "eta: " + str(eta) + " prong: " + str(nprong) +"\n systematic: "+sys+"\n direction: " + direction + "\n hist value: " + str(histValue) + " tool value: " + str(toolValue))



                    
if __name__ == '__main__':
    suite1= unittest.TestLoader().loadTestsFromTestCase(BDTSFTest)
    for suite in [suite1]:
        unittest.TextTestRunner(verbosity=2).run(suite)
