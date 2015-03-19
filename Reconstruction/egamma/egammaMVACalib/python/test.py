# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import math
import ROOT


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    x = xmin
    while True:
        yield x
        x += delta
        if x >= xmax:
            break


def xAOD_electron_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Electrons.size()):
            el = tree.Electrons.at(i)
            yield el


def xAOD_photon_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Photons.size()):
            el = tree.Photons.at(i)
            yield el


class TestEgammaMVACalib(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
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


class TestEgammaMVATool(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        """ using a filename where the egamma.e() is already computed
        v1 MVA calibration
        """
        filename = '~/AOD.01614626._000399.pool.root.9'
        filename = '~/user.blenzi.4956574.EXT0._000001.AOD.pool.root'
        if (not ROOT.xAOD.Init().isSuccess()):
            print "Failed xAOD.Init()"
        treeName = "CollectionTree"

        f = ROOT.TFile.Open(filename)
        if not f:
            print "file %s not found" % filename
        cls.tree = ROOT.xAOD.MakeTransientTree(f, treeName)

        cls.xAOD_tool = ROOT.egammaMVATool("egammaMVATool")
        cls.xAOD_tool.setProperty("folder", "egammaMVACalib/v1")
        cls.xAOD_tool.initialize()

    def testElectrons(self):
        N = 200
        for i, el in enumerate(xAOD_electron_generator(self.tree)):
            old_energy = el.e()
            new_energy = self.xAOD_tool.getEnergy(el.caloCluster(), el)
            self.assertAlmostEqual(old_energy, new_energy, delta=1)
            if i >= N:
                break
        print "tested %d electrons" % i
        self.assertGreater(i, 10, msg="too few electrons")

    def atestPhotons(self):
        N = 200
        for i, ph in enumerate(xAOD_photon_generator(self.tree)):
            if ph.vertex():
                continue
            old_energy = ph.e()
            new_energy = self.xAOD_tool.getEnergy(ph.caloCluster(), ph)
            print old_energy, new_energy, ph.eta()
            self.assertAlmostEqual(old_energy, new_energy, delta=1)
            if i >= N:
                break
        print "tested %d photons" % i
        self.assertGreater(i, 10, msg="too few photons")



class TestEgammaMVATrigger(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        filename = '~/AOD.01614626._000399.pool.root.9'
        if (not ROOT.xAOD.Init().isSuccess()):
            print "Failed xAOD.Init()"
        treeName = "CollectionTree"

        f = ROOT.TFile.Open(filename)
        cls.tree = ROOT.xAOD.MakeTransientTree(f, treeName)

#  bug: https://its.cern.ch/jira/browse/ATLASG-52
#        import tempfile
#        dirpath = tempfile.mkdtemp()
#        import shutil
#        from PathResolver import PathResolver
#        src = PathResolver.FindCalibFile("egammaMVACalib/v1" + "/MVACalib_unconvertedPhoton.weights.root")
#        shutil.copyfile(src, dirpath)
#        print dirpath

        cls.xAOD_tool = ROOT.egammaMVATool("egammaMVATool")
        cls.xAOD_tool.setProperty("folder", "~/trigger_weights")
        cls.xAOD_tool.initialize()

    def testElectron(self):
        for i, el in enumerate(xAOD_electron_generator(self.tree)):
            self.assertAlmostEqual(self.xAOD_tool.getEnergy(el.caloCluster(), "Electron"),
                                   self.xAOD_tool.getEnergy(el.caloCluster(), el))
        self.assertGreater(i, 100, msg="not enough electrons")

    def testPhoton(self):
        for i, ph in enumerate(xAOD_photon_generator(self.tree)):
            trigger_energy = self.xAOD_tool.getEnergy(ph.caloCluster(), "Photon")
            if not ph.vertex():
                self.assertAlmostEqual(trigger_energy,
                                       self.xAOD_tool.getEnergy(ph.caloCluster(), ph))
        self.assertGreater(i, 100, msg="not enough photons")

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    unittest.main()
