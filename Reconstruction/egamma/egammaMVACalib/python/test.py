# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    x = xmin
    while True:
        yield x
        x += delta
        if x >= xmax:
            break


class TestEgammaMVACalib(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        import urllib
        urllib.urlretrieve("http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/egammaMVACalib/v1/MVACalib_electron.weights.root", "MVACalib_electron.weights.root")
        urllib.urlretrieve("http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/egammaMVACalib/v1/MVACalib_convertedPhoton.weights.root", "MVACalib_convertedPhoton.weights.root")
        urllib.urlretrieve("http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/egammaMVACalib/v1/MVACalib_unconvertedPhoton.weights.root", "MVACalib_unconvertedPhoton.weights.root")

        ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")

        cls.photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, ".")
        cls.electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, ".")

        cls.photon_tool.useClusterIf0(False)
        cls.electron_tool.useClusterIf0(False)

        cls.photon_tool.InitTree(0)
        cls.electron_tool.InitTree(0)

    def test_energy_single(self):
        Etrue, Ecluster, Emva = 163944.71875, 160612.73, 169953.875
        inputs = (12222.08, 49425.33, 89170.18, 655.61, 1.663827, 160612.73, 1.6522,
                  2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
                  2, 10, 0, 10, 117.15968322753906)
        self.assertAlmostEqual(self.photon_tool.getMVAEnergyPhoton(*inputs), Emva, delta=1)

        Etrue, Ecluster, Emva = 51110.449624549736, 48970.80859375, 49346.90625
        inputs = (2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
                  0.5844721794128418, 48970.80859375, 0.5835072994232178, 2.843465566635132)
        self.assertAlmostEqual(self.electron_tool.getMVAEnergyElectron(*inputs), Emva, delta=1)

    def test_coverage(self):
        for rconv in (0, 117):
            for eta in arange(-3, 3, 0.01):
                inputs = (12222.08, 49425.33, 89170.18, 655.61, eta, 160612.73, eta,
                          2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
                          2, 10, 0, 10, rconv)
                mva_energy = self.photon_tool.getMVAEnergyPhoton(*inputs)
                if 1.370001 < abs(eta) < 1.5199999 or abs(eta) > 2.4700001:
                    self.assertEqual(mva_energy, 0)
                else:
                    self.assertGreater(mva_energy, 0)

        for eta in arange(-3, 3, 0.01):
            inputs = (2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
                      eta, 48970.80859375, eta, 2.843465566635132)
            mva_energy = self.electron_tool.getMVAEnergyElectron(*inputs)
            if 1.370001 < abs(eta) < 1.5199999 or abs(eta) > 2.4700001:
                self.assertEqual(mva_energy, 0)
            else:
                self.assertGreater(mva_energy, 0)


if __name__ == '__main__':
    unittest.main()
