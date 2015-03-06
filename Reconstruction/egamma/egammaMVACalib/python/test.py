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

        ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")

        cls.photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        cls.electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")

        cls.photon_tool.useClusterIf0(False)
        cls.electron_tool.useClusterIf0(False)

        cls.photon_tool.InitTree(0)
        cls.electron_tool.InitTree(0)

    def test_energy_v1_weights(self):
        """
        test for some fixed case if using run1 weights (v1) the tool reproduces
        the same outputs +/- 1 MeV
        """
        photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")
        photon_tool.InitTree(0)
        electron_tool.InitTree(0)
        # first example
        Etrue, Ecluster, Emva = 163944.71875, 160612.73, 169953.875
        example_inputs = (12222.08, 49425.33, 89170.18, 655.61, 1.663827, 160612.73, 1.6522,
                          2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
                          2, 10, 0, 10, 117.15968322753906)
        self.assertAlmostEqual(photon_tool.getMVAEnergyPhoton(*example_inputs), Emva, delta=1)

        Etrue, Ecluster, Emva = 51110.449624549736, 48970.80859375, 49346.90625
        example_inputs = (2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
                          0.5844721794128418, 48970.80859375, 0.5835072994232178, 2.843465566635132)

        self.assertAlmostEqual(electron_tool.getMVAEnergyElectron(*example_inputs), Emva, delta=1.)

    def test_extensive_v1_electron(self):
        electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")
        electron_tool.InitTree(0)
        self.do_test_file(electron_tool.getMVAEnergyElectron, "electron_response.txt")

    def test_extensive_v1_photon(self):
        photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        photon_tool.InitTree(0)
        self.do_test_file(photon_tool.getMVAEnergyPhoton, "photon_response.txt")

    def do_test_file(self, function, filename):
        with open(filename) as f:
            for line in f:
                left, right = line.split("->")
                left = left.rstrip(' ]').lstrip(' [')
                right = right.strip()

                expected_output = float(right)
                left = left.split(',')
                example_input = [float(r) if "." in r else int(r) for r in left]
                output = function(*example_input)

                self.assertAlmostEqual(output, expected_output, delta=1.)

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

    def test_tool_initialization(self):
        tool = ROOT.egammaMVATool('the_tool')

if __name__ == '__main__':
    unittest.main()
